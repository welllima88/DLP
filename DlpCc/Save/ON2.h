/*****
* STM - Station de Travail Monétique
* API ON2 - Communication avec le serveur monétique ON/2 sur le S/88
* Couche haut niveau: protocole ON/2
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* ON2.h
*****/

#ifndef __ON2_H__
#define __ON2_H__

#include "apiSTM.h"
#include "apiON2.h"
#include "SockON2.h"

/*****
* Classe CON2
*   Note: Les timeouts sont exprimés en millisecondes
*****/

#ifdef __cplusplus

class CON2 : public CSockON2
{
// Construction/Destruction
public:
	CON2();
	~CON2();

// Operations
public:
	BOOL SendPayment(	STM_LPDATETIME lpDateTime,
						ON2_LPPAYMENT lpPayment,
						ON2_LPAPPROVAL lpApproval,
						DWORD dwTimeout,
						char []);
	BOOL SendCommit(	STM_LPDATETIME lpDateTime,
						ON2_LPCOMMIT lpCommit );
	BOOL SendRefund(	STM_LPDATETIME lpDateTime,
						ON2_LPREFUND lpRefund,
						ON2_LPAPPROVAL lpApproval,
						DWORD dwTimeout );

// Implementation
protected:
	BOOL ReceiveApproval(	ON2_LPREQUEST lpRequest,
							ON2_LPAPPROVAL lpApproval,
							DWORD dwTimeout,
							WORD wSequenceNumber,
							WORD wApprovalType );

protected:
	static WORD GetNextSequenceNumber();

protected:
	static WORD	m_wSequenceNumber;

protected:
	bool IsLuhn(char []);
	bool VerifyCardDate(char []);
};

#endif

#endif /* __ON2_H__ */
