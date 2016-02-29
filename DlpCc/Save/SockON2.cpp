//*****
//* STM - Station de Travail Mon�tique
//* API ON2 - Communication avec le serveur mon�tique ON/2 sur le S/88
//* Couche moyen niveau: messages ON/2 sur sockets TCP/IP
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
//*****
//* SockON2.cpp
//*****

#include "STM95DLLpriv.h"
#include "SockON2.h"

#define SOCKON2_HEADER_FORMAT			"%c%04u%04u"
#define SOCKON2_HEADER_LEN				9

#define SOCKON2_HEADERTYPE_CONVERSATION	'C'
#define SOCKON2_HEADERTYPE_ACKNOWLEDGE	'A'
#define SOCKON2_HEADERTYPE_KEEPALIVE	'K'

#define SOCKON2_HEADERNUMBER_MAX		9999
#define SOCKON2_HEADERLENGTH_MAX		9999

#define SOCKON2_TIMEOUT_ACKNOWLEDGE		10000
#define SOCKON2_TIMEOUT_DISCARD			1000

#define SOCKON2_DISCARDBUFFER_LEN		64

/////////////////////////////////////////////////////////////////////////////
// Classe CSockON2
//   Note: Les timeouts sont exprim�s en millisecondes

WORD CSockON2::m_wMessageNumber = 0;

// Construction/Destruction
CSockON2::CSockON2()
{
}

CSockON2::~CSockON2()
{
}

// Operations
BOOL CSockON2::Send(LPCSTR lpData, DWORD dwLenData)
{
	// V�rifier les param�tres
	if (lpData == NULL)
	{
		return FALSE;
	}

	// Construire l'en-t�te ON/2 sp�cifique aux communications TCP/IP
	// Emettre l'en-t�te
	// Emettre les donn�es du message
	char szHeader[SOCKON2_HEADER_LEN + 1];
	WORD wMessageNumber = GetNextMessageNumber();
	if (   !BuildHeader(szHeader, SOCKON2_HEADERTYPE_CONVERSATION, wMessageNumber, dwLenData)
		|| !CSock::Send(szHeader, SOCKON2_HEADER_LEN)
		|| !CSock::Send(lpData, dwLenData) )
	{
		return FALSE;
	}

	// Attendre l'acquittement du message
	while (TRUE)
	{
		// Recevoir un en-t�te
		// D�coder l'en-t�te re�u
		char szAck[SOCKON2_HEADER_LEN + 1];
		char cAckType;
		WORD wAckNumber;
		DWORD dwAckLenData;
		if (   !CSock::Receive(SOCKON2_TIMEOUT_ACKNOWLEDGE, szAck, SOCKON2_HEADER_LEN)
			|| !DecodeHeader(szAck, cAckType, wAckNumber, dwAckLenData) )
		{
			return FALSE;
		}

		// L'en-t�te re�u est-il l'acquittement du message ?
		if (   (cAckType == SOCKON2_HEADERTYPE_ACKNOWLEDGE)
			&& (wAckNumber == wMessageNumber)
			&& (dwAckLenData == 0) )
		{
			break;
		}

		// Eliminer les �ventuelles donn�es du message associ� au mauvais en-t�te re�u
		if ((dwAckLenData != 0) && !Discard(dwAckLenData))
		{
			return FALSE;
		}
	}

	// Emission du message termin�e
	return TRUE;
}

BOOL CSockON2::Receive(DWORD dwTimeout, LPSTR lpData, DWORD dwLenData)
{
	// V�rifier les param�tres
	if (lpData == NULL)
	{
		return FALSE;
	}

	// Attendre le message
	WORD wHeaderNumber;
	while (TRUE)
	{
		// Recevoir un en-t�te
		// D�coder l'en-t�te re�u
		char szHeader[SOCKON2_HEADER_LEN + 1];
		char cHeaderType;
		DWORD dwHeaderLenData;
		if (   !CSock::Receive(dwTimeout, szHeader, SOCKON2_HEADER_LEN)
			|| !DecodeHeader(szHeader, cHeaderType, wHeaderNumber, dwHeaderLenData) )
		{
			return FALSE;
		}

		// L'en-t�te re�u est-il celui du message attendu ?
		if (   (cHeaderType == SOCKON2_HEADERTYPE_CONVERSATION)
			&& (dwHeaderLenData == dwLenData) )
		{
			break;
		}

		// Eliminer les �ventuelles donn�es du message associ� au mauvais en-t�te re�u
		if ((dwHeaderLenData != 0) && !Discard(dwHeaderLenData))
		{
			return FALSE;
		}
	}

	// Recevoir le message
	// Construire l'en-t�te ON/2 d'acquittement du message re�u
	// Emettre l'acquittement du message re�u
	char szAck[SOCKON2_HEADER_LEN + 1];
	if (   !CSock::Receive(dwTimeout, lpData, dwLenData)
		|| !BuildHeader(szAck, SOCKON2_HEADERTYPE_ACKNOWLEDGE, wHeaderNumber)
		|| !CSock::Send(szAck, SOCKON2_HEADER_LEN) )
	{
		return FALSE;
	}

	// R�ception du message termin�e
	return TRUE;
}

// Implementation
BOOL CSockON2::Discard(DWORD dwLenData)
{
	// Eliminer les donn�es ind�sirables en plusieurs passes
	char szDiscard[SOCKON2_DISCARDBUFFER_LEN];
	while (dwLenData != 0)
	{
		// D�terminer la longueur des donn�es � �liminer
		DWORD dwLenToDiscard = dwLenData;
		if (dwLenToDiscard > sizeof(szDiscard))
		{
			dwLenToDiscard = sizeof(szDiscard);
		}

		// Eliminer le bloc courant
		if (!CSock::Receive(SOCKON2_TIMEOUT_DISCARD, szDiscard, dwLenToDiscard))
		{
			return FALSE;
		}

		// Passer au bloc suivant
		dwLenData -= dwLenToDiscard;
	}

	// Elimination des donn�es ind�sirables termin�e
	return TRUE;
}

WORD CSockON2::GetNextMessageNumber()
{
	// Ramener le num�ro s�quentiel de message entre 0 et (SOCKON2_HEADERNUMBER_MAX - 1)
	m_wMessageNumber %= SOCKON2_HEADERNUMBER_MAX;

	// Retourner un num�ro s�quentiel de message compris entre 1 et SOCKON2_HEADERNUMBER_MAX
	return ++m_wMessageNumber;
}

BOOL CSockON2::BuildHeader(	LPSTR lpszHeader,
							char cHeaderType,
							WORD wMessageNumber,
							DWORD dwLenData /*=0*/ )
{
	// V�rifier les param�tres
	if (   (lpszHeader == NULL)
		|| (wMessageNumber > SOCKON2_HEADERNUMBER_MAX)
		|| (dwLenData > (SOCKON2_HEADERLENGTH_MAX - SOCKON2_HEADER_LEN)) )
	{
		return FALSE;
	}

	// Construire l'en-t�te
	return (::wsprintfA(	lpszHeader,
							SOCKON2_HEADER_FORMAT,
							cHeaderType,
							wMessageNumber,
							dwLenData + SOCKON2_HEADER_LEN ) == SOCKON2_HEADER_LEN);
}

BOOL CSockON2::DecodeHeader(	LPCSTR lpszHeader,
								char& rcHeaderType,
								WORD& rwMessageNumber,
								DWORD& rdwLenData )
{
	// V�rifier les param�tres
	if (lpszHeader == NULL)
	{
		return FALSE;
	}

	// D�coder le type de l'en-t�te
	if (   (lpszHeader[0] != SOCKON2_HEADERTYPE_CONVERSATION)
		&& (lpszHeader[0] != SOCKON2_HEADERTYPE_ACKNOWLEDGE)
		&& (lpszHeader[0] != SOCKON2_HEADERTYPE_KEEPALIVE) )
	{
		return FALSE;
	}
	rcHeaderType = lpszHeader[0];

	// D�coder le num�ro s�quentiel du message
	if (   (lpszHeader[1] < '0') || (lpszHeader[1] > '9')
		|| (lpszHeader[2] < '0') || (lpszHeader[2] > '9')
		|| (lpszHeader[3] < '0') || (lpszHeader[3] > '9')
		|| (lpszHeader[4] < '0') || (lpszHeader[4] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	rwMessageNumber	= char(itoa((lpszHeader[1] - '0') * 1000
					+ (lpszHeader[2] - '0') * 100
					+ (lpszHeader[3] - '0') * 10
					+ (lpszHeader[4] - '0'),buffer,10));

	// D�coder la longueur des donn�es du message
	if (   (lpszHeader[5] < '0') || (lpszHeader[5] > '9')
		|| (lpszHeader[6] < '0') || (lpszHeader[6] > '9')
		|| (lpszHeader[7] < '0') || (lpszHeader[7] > '9')
		|| (lpszHeader[8] < '0') || (lpszHeader[8] > '9') )
	{
		return FALSE;
	}
	rdwLenData	= (lpszHeader[5] - '0') * 1000
				+ (lpszHeader[6] - '0') * 100
				+ (lpszHeader[7] - '0') * 10
				+ (lpszHeader[8] - '0');
	if (rdwLenData < SOCKON2_HEADER_LEN)
	{
		return FALSE;
	}
	rdwLenData -= SOCKON2_HEADER_LEN;

	// D�codage de l'en-t�te termin�
	return TRUE;
}
