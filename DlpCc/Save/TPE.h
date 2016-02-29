/*****
* STM - Station de Travail Monétique
* API TPE - Communication Titus
* Couche haut niveau: protocole TPE
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* TPE.h
*****/

#ifndef __TPE_H__
#define __TPE_H__

#include "apiSTM.h"
#include "apiTPE.h"
#include "Titus.h"

/*****
* Classe CTPE
*   Note: Les timeouts sont exprimés en millisecondes
*****/

#ifdef __cplusplus

class CTPE
{
// Construction/Destruction
public:
	CTPE();
	~CTPE();

// Operations
public:
	BOOL Reset(LPCSTR lpszPortName);
	BOOL Close();
	BOOL GetStatus(DWORD dwTimeout, TPE_LPSTATUS lpStatus);
	BOOL ReadCard(DWORD dwTimeout, TPE_LPREAD lpRead, STM_LPDATETIME lpDateTime);
	BOOL WriteCard(DWORD dwTimeout, TPE_LPWRITE lpWrite);
	BOOL LockCard(DWORD dwTimeout, TPE_LPLOCK lpLock);

// Implementation
protected:
	BOOL SetAnswer(LPCSTR lpszAnswer, TPE_LPSTATUS lpStatus);
	BOOL SetAnswer(LPCSTR lpszAnswer, TPE_LPREAD_REPLY lpReadReply, LPWORD lpwTerminal);
	BOOL SetAnswer(LPCSTR lpszAnswer, TPE_LPMESSAGE lpMessage);
	BOOL SetAnswer(LPCSTR lpszAnswer, TPE_LPWRITE_REPLY lpWriteReply);
	BOOL SetAnswer(LPCSTR lpszAnswer, TPE_LPLOCK_REPLY lpLockReply);

protected:
	CTitus	m_Titus;
	BOOL	m_bOpened;
	BOOL	m_bReadCardInProgress;

protected:
	static char m_szNoAnswerMessage[];
	static char m_szSwappedMessage[];
	static char m_szInsertCardMessage[];
};

#endif

#endif /* __TPE_H__ */
