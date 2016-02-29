//*****
//* STM - Station de Travail Monétique
//* API TPE - Communication Titus
//* Couche haut niveau: protocole TPE
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
//*****
//* TPE.cpp
//*****

#include "STM95DLLpriv.h"
#include "TPE.h"
#include <stdlib.h>

#define TPE_QUERYMESSAGE_STATUS			"1"

#define TPE_QUERYMESSAGE_READ_FORMAT	"2%01u%08lu%02u%02u%02u%02u%02u%03u"
#define TPE_QUERYMESSAGE_READ_SIZE		(23 + 1)
#define TPE_READTERMINAL_MODULUS		1000

#define TPE_QUERYMESSAGE_LOCK_FORMAT	"4%-19.19s"
#define TPE_QUERYMESSAGE_LOCK_SIZE		(20 + 1)
#define TPE_LOCKCARDNUMBER_PAD			'?'

#define TPE_QUERYMESSAGE_WRITE_FORMAT	"5%01u%01u%-20.20s%-15.15s%-15.15s%-7.7s%04u%03u%04u%c%-6.6s%-10.10s"
#define TPE_QUERYMESSAGE_WRITE_SIZE		(88 + 1)
#define TPE_WRITETERMINAL_MODULUS		1000

#define TPE_QUERYMESSAGE_RESET			"6"

#define TPE_ANSWERCHARIZE(AnswerValue)	#@AnswerValue
#define TPE_ANSWERCHAR(AnswerType)		TPE_ANSWERCHARIZE(AnswerType)
#define TPE_ANSWERCHAR_STATUS			TPE_ANSWERCHAR(TPE_ANSWERTYPE_STATUS)
#define TPE_ANSWERCHAR_READ_REPLY		TPE_ANSWERCHAR(TPE_ANSWERTYPE_READ_REPLY)
#define TPE_ANSWERCHAR_MESSAGE			TPE_ANSWERCHAR(TPE_ANSWERTYPE_MESSAGE)
#define TPE_ANSWERCHAR_LOCK_REPLY		TPE_ANSWERCHAR(TPE_ANSWERTYPE_LOCK_REPLY)
#define TPE_ANSWERCHAR_WRITE_REPLY		TPE_ANSWERCHAR(TPE_ANSWERTYPE_WRITE_REPLY)

#define TPE_ANSWER_STATUS_LEN			23
#define TPE_ANSWER_READ_REPLY_LEN		46
#define TPE_ANSWER_MESSAGE_LEN			23
#define TPE_ANSWER_LOCK_REPLY_LEN		23
#define TPE_ANSWER_WRITE_REPLY_LEN		11
#define TPE_ANSWER_MAXSIZE				(46 + 1)

/////////////////////////////////////////////////////////////////////////////
// Classe CTPE
//   Note: Les timeouts sont exprimés en millisecondes

// Messages retournés en cas d'erreur
char CTPE::m_szNoAnswerMessage[] =		"199REPONSE NON RECUE   ";
char CTPE::m_szSwappedMessage[] =		"198TPE INTERVERTI      ";

// Message retourné dès l'émission de l'ordre de lecture d'une carte
char CTPE::m_szInsertCardMessage[] =	"300INSERER CARTE       ";

// Construction/Destruction
CTPE::CTPE()
	: m_bOpened(FALSE)
	, m_bReadCardInProgress(FALSE)
{
}

CTPE::~CTPE()
{
	if (m_bOpened)
	{
		m_Titus.Close();
	}
}

// Operations
BOOL CTPE::Reset(LPCSTR lpszPortName)
{
	// Vérifier les paramètres
	if (lpszPortName == NULL)
	{
		return FALSE;
	}

	// Initialisation éventuelle du TPE Titus
	if (!m_bOpened)
	{
		m_bOpened = m_Titus.Open(lpszPortName);
		if (!m_bOpened)
		{
			return FALSE;
		}
	}

	// Retour à l'état repos
	m_bReadCardInProgress = FALSE;
	return m_Titus.SendMessage(TPE_QUERYMESSAGE_RESET);
}

BOOL CTPE::Close()
{
	// Fermeture de la connexion avec le TPE Titus
	if (m_bOpened)
	{
		m_Titus.Close();
		m_bOpened = FALSE;
		m_bReadCardInProgress = FALSE;
	}
	return TRUE;
}

BOOL CTPE::GetStatus(DWORD dwTimeout, TPE_LPSTATUS lpStatus)
{
	// Vérifier les paramètres
	if (lpStatus == NULL)
	{
		return FALSE;
	}

	// Le TPE Titus doit avoir été initialisé
	if (!m_bOpened)
	{
		return FALSE;
	}

	// Demander l'état du TPE Titus
	if (!m_Titus.SendMessage(TPE_QUERYMESSAGE_STATUS))
	{
		return FALSE;
	}

	// Lire l'état du TPE Titus
	char szMessage[TPE_ANSWER_MAXSIZE];
	LPSTR lpszMessage = szMessage;
	if (!m_Titus.ReadMessage(dwTimeout, lpszMessage, sizeof(szMessage)))
	{
		lpszMessage = m_szNoAnswerMessage;
	}

	// Retourner l'état du TPE Titus
	return SetAnswer(lpszMessage, lpStatus);
}

BOOL CTPE::ReadCard(DWORD dwTimeout, TPE_LPREAD lpRead, STM_LPDATETIME lpDateTime)
{
	// Vérifier les paramètres
	if ((lpRead == NULL) || (lpDateTime == NULL))
	{
		return FALSE;
	}

	// Le TPE Titus doit avoir été initialisé
	if (!m_bOpened)
	{
		return FALSE;
	}

	// Faut-il débuter une nouvelle lecture ou poursuivre la lecture en cours ?
	char szMessage[TPE_ANSWER_MAXSIZE];
	LPSTR lpszMessage = szMessage;
	if (!m_bReadCardInProgress)
	{
		// Vérifier les paramétres transmis
		if (   (lpRead->Query.byReadType > TPE_READTYPE_MAX)
			|| (lpRead->Query.dwReadAmount > TPE_READAMOUNT_MAX)
			|| (lpRead->Query.wReadTerminal > TPE_READTERMINAL_MAX) )
		{
			return FALSE;
		}

		// Demander la lecture d'une carte sur le TPE Titus
		char szReadQuery[TPE_QUERYMESSAGE_READ_SIZE];
		if (   (::wsprintfA(	szReadQuery,
								TPE_QUERYMESSAGE_READ_FORMAT,
								lpRead->Query.byReadType,
								lpRead->Query.dwReadAmount,
								lpDateTime->wYear % 100,
								lpDateTime->byMonth,
								lpDateTime->byDay,
								lpDateTime->byHour,
								lpDateTime->byMin,
								lpRead->Query.wReadTerminal % TPE_READTERMINAL_MODULUS )
				!= (TPE_QUERYMESSAGE_READ_SIZE - 1) )
			|| !m_Titus.SendMessage(szReadQuery) )
		{
			return FALSE;
		}
		lpszMessage = m_szInsertCardMessage;
	}
	else
	{
		// Poursuivre la lecture de la carte sur le TPE Titus
		if (!m_Titus.ReadMessage(dwTimeout, lpszMessage, sizeof(szMessage)))
		{
			lpszMessage = m_szNoAnswerMessage;
		}
	}

	// Retourner l'état d'avancement de la lecture de la carte sur le TPE Titus
	switch (lpszMessage[0])
	{
		case TPE_ANSWERCHAR_READ_REPLY:
		{
			lpRead->byAnswerType = TPE_ANSWERTYPE_READ_REPLY;
			m_bReadCardInProgress = FALSE;
			WORD wTerminal;
			if (!SetAnswer(lpszMessage, &lpRead->Reply, &wTerminal))
			{
				return FALSE;
			}
			if (wTerminal == (lpRead->Query.wReadTerminal % TPE_READTERMINAL_MODULUS))
			{
				return TRUE;
			}
			lpszMessage = m_szSwappedMessage;
		}
		// PAS DE break;

		case TPE_ANSWERCHAR_STATUS:
		{
			lpRead->byAnswerType = TPE_ANSWERTYPE_STATUS;
			m_bReadCardInProgress = FALSE;
			return SetAnswer(lpszMessage, &lpRead->Status);
		}
		break;

		case TPE_ANSWERCHAR_MESSAGE:
		{
			lpRead->byAnswerType = TPE_ANSWERTYPE_MESSAGE;
			return m_bReadCardInProgress = SetAnswer(lpszMessage, &lpRead->Message);
		}
		break;

		default:
		{
			return m_bReadCardInProgress = FALSE;
		}
		break;
	}
}

BOOL CTPE::WriteCard(DWORD dwTimeout, TPE_LPWRITE lpWrite)
{
	// Vérifier les paramètres
	if (lpWrite == NULL)
	{
		return FALSE;
	}

	// Le TPE Titus doit avoir été initialisé
	if (!m_bOpened)
	{
		return FALSE;
	}

	// Vérifier les paramétres transmis
	if (   (lpWrite->Query.byWriteWrite > TPE_WRITEWRITE_MAX)
		|| (lpWrite->Query.byWritePrint > TPE_WRITEPRINT_MAX)
		|| (lpWrite->Query.wWriteSystem > TPE_WRITESYSTEM_MAX)
		|| (lpWrite->Query.wWriteTerminal > TPE_WRITETERMINAL_MAX)
		|| (lpWrite->Query.wWriteTransaction > TPE_WRITETRANSACTION_MAX)
		|| (   (lpWrite->Query.byWriteTransactionType != TPE_WRITETRANSACTIONTYPE_DEBIT_NORMAL)
			&& (lpWrite->Query.byWriteTransactionType != TPE_WRITETRANSACTIONTYPE_DEBIT_APPROVED)
			&& (lpWrite->Query.byWriteTransactionType != TPE_WRITETRANSACTIONTYPE_DEBIT_FORCED)
			&& (lpWrite->Query.byWriteTransactionType != TPE_WRITETRANSACTIONTYPE_CREDIT) ) )
	{
		return FALSE;
	}

	// Ecriture d'une transaction et/ou impression du ticket sur le TPE Titus
	char szWriteQuery[TPE_QUERYMESSAGE_WRITE_SIZE];
	if (   (::wsprintfA(	szWriteQuery,
							TPE_QUERYMESSAGE_WRITE_FORMAT,
							lpWrite->Query.byWriteWrite,
							lpWrite->Query.byWritePrint,
							lpWrite->Query.szWriteTicketHeader,
							lpWrite->Query.szWriteTicketName,
							lpWrite->Query.szWriteTicketLocation,
							lpWrite->Query.szWriteMerchant,
							lpWrite->Query.wWriteSystem,
							lpWrite->Query.wWriteTerminal % TPE_WRITETERMINAL_MODULUS,
							lpWrite->Query.wWriteTransaction,
							lpWrite->Query.byWriteTransactionType,
							lpWrite->Query.szWriteApprovalCode,
							lpWrite->Query.szWriteCardType )
			!= (TPE_QUERYMESSAGE_WRITE_SIZE - 1) )
		|| !m_Titus.SendMessage(szWriteQuery) )
	{
		return FALSE;
	}

	// Lire le compte-rendu d'écriture de la transaction et/ou d'impression du ticket
	char szMessage[TPE_ANSWER_MAXSIZE];
	LPSTR lpszMessage = szMessage;
	if (!m_Titus.ReadMessage(dwTimeout, lpszMessage, sizeof(szMessage)))
	{
		lpszMessage = m_szNoAnswerMessage;
	}

	// Retourner le compte-rendu
	switch (lpszMessage[0])
	{
		case TPE_ANSWERCHAR_WRITE_REPLY:
		{
			lpWrite->byAnswerType = TPE_ANSWERTYPE_WRITE_REPLY;
			return SetAnswer(lpszMessage, &lpWrite->Reply);
		}
		break;

		case TPE_ANSWERCHAR_STATUS:
		{
			lpWrite->byAnswerType = TPE_ANSWERTYPE_STATUS;
			return SetAnswer(lpszMessage, &lpWrite->Status);
		}
		break;

		default:
		{
			return FALSE;
		}
		break;
	}
}

BOOL CTPE::LockCard(DWORD dwTimeout, TPE_LPLOCK lpLock)
{
	// Vérifier les paramètres
	if (lpLock == NULL)
	{
		return FALSE;
	}

	// Le TPE Titus doit avoir été initialisé
	if (!m_bOpened)
	{
		return FALSE;
	}

	// Mettre en forme le numéro de carte transmis en paramétre
	for (	int iPos = ::lstrlenA(lpLock->Query.szLockCardNumber) ;
			(iPos < (sizeof(lpLock->Query.szLockCardNumber) - 1)) ;
			iPos++ )
	{
		lpLock->Query.szLockCardNumber[iPos] = TPE_LOCKCARDNUMBER_PAD;
	}
	lpLock->Query.szLockCardNumber[iPos] = 0;

	// Invalidation de la carte à puce insérée dans le TPE Titus
	char szLockQuery[TPE_QUERYMESSAGE_LOCK_SIZE];
	if (   (::wsprintfA(	szLockQuery,
							TPE_QUERYMESSAGE_LOCK_FORMAT,
							lpLock->Query.szLockCardNumber )
			!= (TPE_QUERYMESSAGE_LOCK_SIZE - 1) )
		|| !m_Titus.SendMessage(szLockQuery) )
	{
		return FALSE;
	}

	// Lire le compte-rendu d'invalidation de la carte à puce
	char szMessage[TPE_ANSWER_MAXSIZE];
	LPSTR lpszMessage = szMessage;
	if (!m_Titus.ReadMessage(dwTimeout, lpszMessage, sizeof(szMessage)))
	{
		lpszMessage = m_szNoAnswerMessage;
	}

	// Retourner le compte-rendu
	switch (lpszMessage[0])
	{
		case TPE_ANSWERCHAR_LOCK_REPLY:
		{
			lpLock->byAnswerType = TPE_ANSWERTYPE_LOCK_REPLY;
			return SetAnswer(lpszMessage, &lpLock->Reply);
		}
		break;

		case TPE_ANSWERCHAR_STATUS:
		{
			lpLock->byAnswerType = TPE_ANSWERTYPE_STATUS;
			return SetAnswer(lpszMessage, &lpLock->Status);
		}
		break;

		default:
		{
			return FALSE;
		}
		break;
	}
}

// Implementation
BOOL CTPE::SetAnswer(LPCSTR lpszAnswer, TPE_LPSTATUS lpStatus)
{
	if (   (lpszAnswer == NULL)
		|| (lpStatus == NULL)
		|| (lpszAnswer[0] != TPE_ANSWERCHAR_STATUS)
		|| (::lstrlenA(lpszAnswer) != TPE_ANSWER_STATUS_LEN)
		|| (lpszAnswer[1] < '0')
		|| (lpszAnswer[1] > '9')
		|| (lpszAnswer[2] < '0')
		|| (lpszAnswer[2] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	lpStatus->byStatusCode = char(itoa((lpszAnswer[1] - '0') * 10 + (lpszAnswer[2] - '0'),buffer,10));
	::lstrcpynA(lpStatus->szStatusText, lpszAnswer + 3, sizeof(lpStatus->szStatusText));
	return TRUE;
}

BOOL CTPE::SetAnswer(LPCSTR lpszAnswer, TPE_LPREAD_REPLY lpReadReply, LPWORD lpwTerminal)
{
	if (   (lpszAnswer == NULL)
		|| (lpReadReply == NULL)
		|| (lpwTerminal == NULL)
		|| (lpszAnswer[0] != TPE_ANSWERCHAR_READ_REPLY)
		|| (::lstrlenA(lpszAnswer) != TPE_ANSWER_READ_REPLY_LEN)
		|| (lpszAnswer[1] < '0')
		|| ((lpszAnswer[1] > '9') && (lpszAnswer[1] < 'A'))
		|| (lpszAnswer[1] > 'F')
		|| (lpszAnswer[2] < '0')
		|| (lpszAnswer[2] > '9')
		|| (lpszAnswer[43] < '0')
		|| (lpszAnswer[43] > '9')
		|| (lpszAnswer[44] < '0')
		|| (lpszAnswer[44] > '9')
		|| (lpszAnswer[45] < '0')
		|| (lpszAnswer[45] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	if (lpszAnswer[1] > '9')
	{
		lpReadReply->byReadStatus = char(itoa(lpszAnswer[1] - 'A' + 10,buffer,10));
	}
	else
	{
		lpReadReply->byReadStatus = char(itoa(lpszAnswer[1] - '0',buffer,10));
	}
	lpReadReply->byReadCall = char(itoa(lpszAnswer[2] - '0',buffer,10));
	::lstrcpynA(lpReadReply->szReadTrack2Data, lpszAnswer + 3, sizeof(lpReadReply->szReadTrack2Data));
	*lpwTerminal	= short((lpszAnswer[43] - '0') * 100
					+ (lpszAnswer[44] - '0') * 10
					+ (lpszAnswer[45] - '0'));
	return TRUE;
}

BOOL CTPE::SetAnswer(LPCSTR lpszAnswer, TPE_LPMESSAGE lpMessage)
{
	if (   (lpszAnswer == NULL)
		|| (lpMessage == NULL)
		|| (lpszAnswer[0] != TPE_ANSWERCHAR_MESSAGE)
		|| (::lstrlenA(lpszAnswer) != TPE_ANSWER_MESSAGE_LEN)
		|| (lpszAnswer[1] < '0')
		|| (lpszAnswer[1] > '9')
		|| (lpszAnswer[2] < '0')
		|| (lpszAnswer[2] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	lpMessage->byMessageCode = char(itoa((lpszAnswer[1] - '0') * 10 + (lpszAnswer[2] - '0'),buffer,10));
	::lstrcpynA(lpMessage->szMessageText, lpszAnswer + 3, sizeof(lpMessage->szMessageText));
	return TRUE;
}

BOOL CTPE::SetAnswer(LPCSTR lpszAnswer, TPE_LPWRITE_REPLY lpWriteReply)
{
	if (   (lpszAnswer == NULL)
		|| (lpWriteReply == NULL)
		|| (lpszAnswer[0] != TPE_ANSWERCHAR_WRITE_REPLY)
		|| (::lstrlenA(lpszAnswer) != TPE_ANSWER_WRITE_REPLY_LEN)
		|| (lpszAnswer[1] < '0')
		|| ((lpszAnswer[1] > '9') && (lpszAnswer[1] < 'A'))
		|| (lpszAnswer[1] > 'F')
		|| (lpszAnswer[2] < '0')
		|| (lpszAnswer[2] > '1') )
	{
		return FALSE;
	}
	char buffer[50];
	if (lpszAnswer[1] > '9')
	{
		lpWriteReply->byWriteWritten = char(itoa(lpszAnswer[1] - 'A' + 10,buffer,10));
	}
	else
	{
		lpWriteReply->byWriteWritten= char(itoa(lpszAnswer[1] - '0',buffer,10));
	}
	lpWriteReply->byWritePrinted = char(itoa(lpszAnswer[2] - '0',buffer,10));
	::lstrcpynA(	lpWriteReply->szWriteCertificate,
					lpszAnswer + 3,
					sizeof(lpWriteReply->szWriteCertificate) );
	return TRUE;
}

BOOL CTPE::SetAnswer(LPCSTR lpszAnswer, TPE_LPLOCK_REPLY lpLockReply)
{
	if (   (lpszAnswer == NULL)
		|| (lpLockReply == NULL)
		|| (lpszAnswer[0] != TPE_ANSWERCHAR_LOCK_REPLY)
		|| (::lstrlenA(lpszAnswer) != TPE_ANSWER_LOCK_REPLY_LEN)
		|| (lpszAnswer[1] < '0')
		|| (lpszAnswer[1] > '9')
		|| (lpszAnswer[2] < '0')
		|| (lpszAnswer[2] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	lpLockReply->byLockCode = char(itoa((lpszAnswer[1] - '0') * 10 + (lpszAnswer[2] - '0'),buffer,10));
	::lstrcpynA(lpLockReply->szLockText, lpszAnswer + 3, sizeof(lpLockReply->szLockText));
	return TRUE;
}
