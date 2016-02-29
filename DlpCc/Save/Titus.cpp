//*****
//* STM - Station de Travail Mon�tique
//* API TPE - Communication Titus
//* Couche bas niveau: ligne s�rie et messages Titus
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
//*****
//* Titus.cpp
//*****

#include "STM95DLLpriv.h"
#include "Titus.h"

#define TITUS_BUFFER_SIZE	256

#define ASCII_ENQ	0x05
#define ASCII_ACK	0x06
#define ASCII_NACK	0x15
#define ASCII_EOT	0x04
#define ASCII_STX	0x02
#define ASCII_ETX	0x03

/////////////////////////////////////////////////////////////////////////////
// Classe CTitus
//   Note: Les timeouts sont exprim�s en millisecondes

// Construction/Destruction
CTitus::CTitus()
	: m_hTitus(INVALID_HANDLE_VALUE)
{
}

CTitus::~CTitus()
{
	Close();
}

// Operations
BOOL CTitus::Open(LPCSTR lpszPortName)
{
	// Fermer toute �ventuelle connexion pr�c�dente au port s�rie
	Close();

	// V�rifier les param�tres
	if (lpszPortName == NULL)
	{
		return FALSE;
	}

	// Etablir la connexion au port s�rie d�sign�
	if ((m_hTitus = ::CreateFileA(	lpszPortName,
									GENERIC_READ | GENERIC_WRITE,
									0,
									NULL,
									OPEN_EXISTING,
									0,
									NULL )) == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// Obtenir la configuration initiale du port s�rie
	DCB dcb;
	COMMTIMEOUTS cto;
	char szSettings[32];
	if (   !::GetCommState(m_hTitus, &dcb)
		|| !::GetCommTimeouts(m_hTitus, &cto)
		|| (::GetProfileStringA(	"Ports",
									lpszPortName,
									"1200,E,7,1,",
									szSettings,
									sizeof(szSettings) ) >= (sizeof(szSettings) - 1))
		|| !ParsePortSettings(szSettings, &dcb) )
	{
		::CloseHandle(m_hTitus);
		m_hTitus = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// D�finir la valeur des timeouts du port s�rie
	// Timeout en lecture entre 2 octets = 4 fois le temps de transmission d'un seul octet
	cto.ReadIntervalTimeout         = 4 * 10000 / dcb.BaudRate;
	// Timeout en lecture du premier octet fourni par l'argument de ReadMessage
	cto.ReadTotalTimeoutMultiplier  = 4 * 10000 / dcb.BaudRate;
	cto.ReadTotalTimeoutConstant    = 0;
	// Timeouts en �criture = 4 fois le temps de transmission d'un seul octet
	cto.WriteTotalTimeoutMultiplier = 4 * 10000 / dcb.BaudRate;
	cto.WriteTotalTimeoutConstant   = 4 * 10000 / dcb.BaudRate;

	// Configurer le port s�rie
	// Dimensionner les tampons d'entr�e et de sortie
	// Purger les �ventuels caract�res r�siduels
	// Effacer les �ventuels codes d'erreur r�siduels
	DWORD dwErrors;
	if (   !::SetCommState(m_hTitus, &dcb)
		|| !::SetCommTimeouts(m_hTitus, &cto)
		|| !::SetupComm(m_hTitus, TITUS_BUFFER_SIZE, TITUS_BUFFER_SIZE)
		|| !::PurgeComm(m_hTitus,  PURGE_TXABORT | PURGE_RXABORT
								 | PURGE_TXCLEAR | PURGE_RXCLEAR)
		|| !::ClearCommError(m_hTitus, &dwErrors, NULL) )
	{
		::CloseHandle(m_hTitus);
		m_hTitus = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// Ouverture de la connexion au port s�rie termin�e
	return TRUE;
}

void CTitus::Close()
{
	if (m_hTitus != INVALID_HANDLE_VALUE)
	{
		// Fermer la connexion au port s�rie
		PurgeAndClearError();
		::CloseHandle(m_hTitus);
		m_hTitus = INVALID_HANDLE_VALUE;
	}
}

BOOL CTitus::SendMessage(LPCSTR lpszMessage)
{
	// V�rifier les param�tres
	if (lpszMessage == NULL)
	{
		return FALSE;
	}

	// Calcul du LRC sur les octets du message et l'ETX final
	char cLRC = ASCII_ETX;
	for (LPCSTR lpcLRC = lpszMessage ; (*lpcLRC) ; lpcLRC++)
	{
		cLRC ^= *lpcLRC;
	}

	// Pr�paration � l'�mission du message
	DWORD dwLenMessage = ::lstrlenA(lpszMessage);
	if (!SetReadTimeout(600))
	{
		return FALSE;
	}

	// Automate de gestion du protocole d'�mission de message
	int nFail = 0;
	int iStep = 0;
	while (nFail < 5)
	{
		switch (iStep)
		{
			case 0:
			{
				// Emission ENQ et attente ACK
				char cACK;
				if (   SendChar(ASCII_ENQ)
					&& ReadChar(&cACK)
					&& (cACK == ASCII_ACK) )
				{
					iStep = 1;
					continue;
				}
				else
				{
					nFail++;
					iStep = 2;
					continue;
				}
			}
			break;

			case 1:
			{
				// Emission de la trame STX Message ETX LRC et attente ACK
				char cACK;
				if (   SendChar(ASCII_STX)
					&& SendBuffer(lpszMessage, dwLenMessage)
					&& SendChar(ASCII_ETX)
					&& SendChar(cLRC)
					&& ReadChar(&cACK)
					&& ((cACK == ASCII_ACK) || (cACK == ASCII_NACK)) )
				{
					if (cACK == ASCII_ACK)
					{
						// Emission EOT pour signaler la fin des �changes
						if (SendChar(ASCII_EOT))
						{
							// Emission du message Titus termin�e
							return TRUE;
						}
						else
						{
							nFail++;
							iStep = 2;
							continue;
						}
					}
					else
					{
						nFail++;
						continue;
					}
				}
				else
				{
					nFail++;
					iStep = 2;
					continue;
				}
			}
			break; 

			case 2:
			{
				// Eliminer tout pour transmettre EOT dans de bonnes conditions
				PurgeAndClearError();

				// Emission EOT pour signaler l'abandon de l'�change en cours
				SendChar(ASCII_EOT);

				// Pause avant d'essayer un nouvel �change
				::Sleep(1500);

				// Eliminer les caract�res �ventuellement re�us pendant la pause
				PurgeAndClearError();

				// Essayer un nouvel �change
				iStep = 0;
				continue;
			}
			break;
		}
	}

	// Trop d'erreurs ont �t� rencontr�es
	PurgeAndClearError();
	SendChar(ASCII_EOT);
	return FALSE;
}

BOOL CTitus::ReadMessage(DWORD dwTimeout, LPSTR lpszBuffer, DWORD dwBufferSize)
{
	// V�rifier les param�tres
	if ((lpszBuffer == NULL) || (dwBufferSize == 0))
	{
		return FALSE;
	}

	// Garantir un d�lai d'attente minimal en cas de reprise
	if (dwTimeout < 1500)
	{
		dwTimeout = 1500;
	}

	// Automate de gestion du protocole de r�ception de message
	BOOL bOkMessage = FALSE;
	BOOL bOkFrame = FALSE;
	LPCH lpcChar = NULL;
	char cComputedLRC = 0;
	int iStep = 0;
	while (TRUE)
	{
		switch (iStep)
		{
			case 0:
			{
				// D�lai d'attente de r�ception de ENQ
				if (!SetReadTimeout(dwTimeout))
				{
					iStep = 6;
					continue;
				}

				iStep = 1;
			}
			// PAS DE break;

			case 1:
			{
				// Attente ENQ
				char cENQ;
				BOOL bTimeout;
				if (!ReadChar(&cENQ, &bTimeout))
				{
					if (bTimeout)
					{
						iStep = 6;
						continue;
					}
					ClearError();
					continue;
				}
				if (cENQ != ASCII_ENQ)
				{
					continue;
				}

				// Emission ACK
				if (!SendChar(ASCII_ACK))
				{
					ClearError();
					continue;
				}

				iStep = 2;
			}
			// PAS DE break;

			case 2:
			{
				// D�lai d'attente de r�ception de STX
				if (!SetReadTimeout(600))
				{
					iStep = 6;
					continue;
				}

				// Attente STX
				char cSTX;
				BOOL bTimeout;
				if (!ReadChar(&cSTX, &bTimeout))
				{
					if (bTimeout)
					{
						iStep = 0;
						continue;
					}
					ClearError();
					bOkFrame = FALSE;
					iStep = 3;
					continue;
				}
				if (cSTX == ASCII_EOT)
				{
					iStep = 0;
					continue;
				}
				if (cSTX != ASCII_STX)
				{
					bOkFrame = FALSE;
					iStep = 3;
					continue;
				}

				bOkFrame = TRUE;
				iStep = 3;
			}
			// PAS DE break;

			case 3:
			{
				// D�lai d'attente de r�ception des caract�res de la trame
				if (!SetReadTimeout())
				{
					iStep = 6;
					continue;
				}

				bOkMessage = FALSE;
				lpcChar = lpszBuffer;
				cComputedLRC = 0;
				iStep = 4;
			}
			// PAS DE break;

			case 4:
			{
				// Attente des caract�res de la trame
				BOOL bTimeout;
				if (!ReadChar(lpcChar, &bTimeout))
				{
					if (bTimeout)
					{
						iStep = 5;
						continue;
					}
					ClearError();
					bOkFrame = FALSE;
					continue;
				}
				cComputedLRC ^= *lpcChar;
				if (*lpcChar != ASCII_ETX)
				{
					if ((lpcChar - lpszBuffer) < (dwBufferSize - 1))
					{
						lpcChar++;
					}
					else
					{
						bOkFrame = FALSE;
					}
					continue;
				}
				*lpcChar = 0;

				// Attente LRC
				char cReadLRC;
				if (!ReadChar(&cReadLRC, &bTimeout))
				{
					if (bTimeout)
					{
						iStep = 5;
						continue;
					}
					ClearError();
					iStep = 5;
					continue;
				}
				if (!bOkFrame || (cReadLRC != cComputedLRC))
				{
					iStep = 5;
					continue;
				}
				bOkMessage = TRUE;

				// Emission ACK
				if (!SendChar(ASCII_ACK))
				{
					ClearError();
					iStep = 2;
					continue;
				}

				// D�lai d'attente de r�ception de EOT
				if (!SetReadTimeout(600))
				{
					iStep = 6;
					continue;
				}

				// Attente EOT
				char cEOT;
				if (!ReadChar(&cEOT, &bTimeout))
				{
					if (bTimeout)
					{
						iStep = 0;
						continue;
					}
					ClearError();
					iStep = 0;
					continue;
				}
				if (cEOT == ASCII_STX)
				{
					iStep = 3;
					continue;
				}
				if (cEOT != ASCII_EOT)
				{
					iStep = 0;
					continue;
				}

				// R�ception du message Titus termin�e
				return TRUE;
			}
			break;

			case 5:
			{
				// Emission NACK
				if (!SendChar(ASCII_NACK))
				{
					ClearError();
					iStep = 2;
					continue;
				}
				iStep = 2;
				continue;
			}
			break;

			case 6:
			{
				// Abandon de la r�ception
				// Une trame compl�te et correcte a-t-elle �t� re�ue auparavant ?
				if (!bOkMessage)
				{
					// Aucune trame re�ue correctement
					return FALSE;
				}

				// R�ception du message Titus consid�r�e comme termin�e correctement
				return TRUE;
			}
			break;
		}
	}
}

// Implementation
BOOL CTitus::ParsePortSettings(LPCSTR lpszPortSettings, LPDCB lpDCB)
{
	// V�rifier les param�tres
	if ((lpszPortSettings == NULL) || (lpDCB == NULL))
	{
		return FALSE;
	}

	// D�coder la vitesse de transmission
	lpDCB->BaudRate = 0;
	while ((*lpszPortSettings >= '0') && (*lpszPortSettings <= '9'))
	{
		lpDCB->BaudRate *= 10;
		lpDCB->BaudRate += *lpszPortSettings++ - '0';
	}
	if ((lpDCB->BaudRate == 0) || (*lpszPortSettings++ != ','))
	{
		return FALSE;
	}

	// D�coder la parit�
	switch (*lpszPortSettings++)
	{
		case 'N':	// None
		case 'n':
			lpDCB->Parity  = NOPARITY;
			lpDCB->fParity = FALSE;
		break;

		case 'O':	// Odd
		case 'o':
			lpDCB->Parity  = ODDPARITY;
			lpDCB->fParity = TRUE;
		break;

		case 'E':	// Even
		case 'e':
			lpDCB->Parity  = EVENPARITY;
			lpDCB->fParity = TRUE;
		break;

		case 'M':	// Mark
		case 'm':
			lpDCB->Parity  = MARKPARITY;
			lpDCB->fParity = TRUE;
		break;

		case 'S':	// Space
		case 's':
			lpDCB->Parity  = SPACEPARITY;
			lpDCB->fParity = TRUE;
		break;

		default:
			return FALSE;
		break;
	}
	if (*lpszPortSettings++ != ',')
	{
		return FALSE;
	}

	// D�coder le nombre de bits transmis
	if ((*lpszPortSettings >= '4') && (*lpszPortSettings <= '8'))
	{
		char buffer[50];
		lpDCB->ByteSize = char(itoa(*lpszPortSettings++ - '0',buffer,10));
	}
	else
	{
		return FALSE;
	}
	if (*lpszPortSettings++ != ',')
	{
		return FALSE;
	}

	// D�coder le nombre de bits de stop
	switch (*lpszPortSettings++)
	{
		case '1':
			if ((lpszPortSettings[0] == '.') && (lpszPortSettings[1] == '5'))
			{
				lpDCB->StopBits = ONE5STOPBITS;
				lpszPortSettings += 2;
			}
			else
			{
				lpDCB->StopBits = ONESTOPBIT;
			}
		break;

		case '2':
			lpDCB->StopBits = TWOSTOPBITS;
		break;

		default:
			return FALSE;
		break;
	}
	if (*lpszPortSettings != 0)
	{
		if (*lpszPortSettings++ != ',')
		{
			return FALSE;
		}
	}

	// D�coder le protocole de contr�le de flux
	switch (*lpszPortSettings)
	{
		case 0:		// None
		case 'N':
		case 'n':
		case 'E':
		case 'e':
		case 'R':
		case 'r':
			lpDCB->fOutxCtsFlow = FALSE;
			lpDCB->fOutxDsrFlow = FALSE;
			lpDCB->fRtsControl  = RTS_CONTROL_ENABLE;
			lpDCB->fDtrControl  = DTR_CONTROL_ENABLE;
			lpDCB->fOutX        = FALSE;
			lpDCB->fInX         = FALSE;
		break;

		case 'X':	// XOn/XOff
		case 'x':
			lpDCB->fOutxCtsFlow = FALSE;
			lpDCB->fOutxDsrFlow = FALSE;
			lpDCB->fRtsControl  = RTS_CONTROL_ENABLE;
			lpDCB->fDtrControl  = DTR_CONTROL_ENABLE;
			lpDCB->fOutX        = TRUE;
			lpDCB->fInX         = TRUE;
		break;

		case 'B':	// Hardware
		case 'b':
		case 'P':
		case 'p':
			lpDCB->fOutxCtsFlow = TRUE;
			lpDCB->fOutxDsrFlow = TRUE;
			lpDCB->fRtsControl  = RTS_CONTROL_HANDSHAKE;
			lpDCB->fDtrControl  = DTR_CONTROL_HANDSHAKE;
			lpDCB->fOutX        = FALSE;
			lpDCB->fInX         = FALSE;
		break;

		default:
			return FALSE;
		break;
	}
	if (*lpszPortSettings != 0)
	{
		if (*(++lpszPortSettings) != 0)
		{
			return FALSE;
		}
	}

	// Terminer la configuration du port s�rie
	lpDCB->fBinary           = TRUE;
	lpDCB->fDsrSensitivity   = FALSE;
	lpDCB->fTXContinueOnXoff = TRUE;
	lpDCB->fErrorChar        = FALSE;
	lpDCB->fNull             = FALSE;
	lpDCB->fAbortOnError     = FALSE;
	lpDCB->XonLim            = TITUS_BUFFER_SIZE / 8;
	lpDCB->XoffLim           = TITUS_BUFFER_SIZE / 8;

	// D�codage de la cha�ne de param�trage du port s�rie termin�
	return TRUE;
}

void CTitus::PurgeAndClearError()
{
	// Purger les �ventuels caract�res re�us ou non encore �mis
	::PurgeComm(m_hTitus, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// Effacer les �ventuels codes d'erreur
	DWORD dwErrors;
	::ClearCommError(m_hTitus, &dwErrors, NULL);
}

void CTitus::ClearError()
{
	// Effacer les �ventuels codes d'erreur
	DWORD dwErrors;
	::ClearCommError(m_hTitus, &dwErrors, NULL);
}

BOOL CTitus::SetReadTimeout(DWORD dwTimeout /*=0*/)
{
	// Configurer le timeout en lecture du premier octet
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hTitus, &cto))
	{
		return FALSE;
	}
	if (dwTimeout == 0)
	{
		// Utiliser la valeur affect�e au timeout en lecture entre 2 octets 
		dwTimeout = cto.ReadIntervalTimeout;
	}
	cto.ReadTotalTimeoutConstant = dwTimeout;
	if (!::SetCommTimeouts(m_hTitus, &cto))
	{
		return FALSE;
	}

	// Configuration du timeout en lecture termin�e
	return TRUE;
}

BOOL CTitus::SendBuffer(LPCVOID lpBuffer, DWORD dwLenBuffer)
{
	// V�rifier les param�tres
	if (lpBuffer == NULL)
	{
		return FALSE;
	}

	// Emettre en plusieurs passes si le buffer est trop grand
	while (dwLenBuffer != 0)
	{
		// D�terminer la taille du bloc � �mettre
		DWORD dwLenToWrite = dwLenBuffer;
		if (dwLenToWrite > TITUS_BUFFER_SIZE)
		{
			dwLenToWrite = TITUS_BUFFER_SIZE;
		}

		// Emettre le bloc courant
		DWORD dwLenWritten = 0;
		if (   !::WriteFile(m_hTitus, lpBuffer, dwLenToWrite, &dwLenWritten, NULL)
			|| (dwLenWritten != dwLenToWrite) )
		{
			return FALSE;
		}

		// Passer au bloc suivant
		lpBuffer = ((LPCSTR)lpBuffer) + dwLenToWrite;
		dwLenBuffer -= dwLenToWrite;
	}

	// Emission du buffer termin�e
	return TRUE;
}

BOOL CTitus::ReadChar(LPCH lpcChar, LPBOOL lpbTimeoutError /*=NULL*/)
{
	// V�rifier les param�tres
	if (lpcChar == NULL)
	{
		return FALSE;
	}

	// Substituer une variable locale si l'argument n'est pas transmis
	BOOL bFakeParameter;
	if (lpbTimeoutError == NULL)
	{
		lpbTimeoutError = &bFakeParameter;
	}

	// Lire le caract�re
	DWORD dwLenRead = 0;
	if (   !(*lpbTimeoutError = ::ReadFile(m_hTitus, lpcChar, 1, &dwLenRead, NULL))
		|| (*lpbTimeoutError = (dwLenRead != 1)) )
	{
		return FALSE;
	}

	// Lecture du caract�re termin�e
	return TRUE;
}
