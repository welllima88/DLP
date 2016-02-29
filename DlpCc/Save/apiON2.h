/*****
* STM - Station de Travail Monétique
* API ON2 - Communication avec le serveur monétique ON/2 sur le S/88
* Couche API exportée par la DLL
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* apiON2.h
*****/

#ifndef __APION2_H__
#define __APION2_H__

#include "apiSTM.h"

/*****
* Structures
*****/

#define ON2_SYSTEM_MAX					STM_SYSTEM_MAX
#define ON2_TERMINAL_MAX				STM_TERMINAL_MAX
#define ON2_TRANSACTION_MAX				STM_TRANSACTION_MAX
#define ON2_CURRENCY_MAX				STM_CURRENCY_MAX
#define ON2_AMOUNT_MAX					STM_AMOUNT_MAX

#define ON2_CARDTRACK2DATA_SIZE			STM_CARDTRACK2DATA_SIZE
#define ON2_CARDCERTIFICATE_SIZE		STM_CARDCERTIFICATE_SIZE
#define ON2_CARDSTATUS_TRACK			STM_CARDSTATUS_TRACK
#define ON2_CARDSTATUS_CHIP				STM_CARDSTATUS_CHIP
#define ON2_CARDSTATUS_PHONE			STM_CARDSTATUS_PHONE
#define ON2_CARDSTATUS_ALMOSTFULL		STM_CARDSTATUS_ALMOSTFULL
#define ON2_CARDSTATUS_FULL_TRANSAC		STM_CARDSTATUS_FULL_TRANSAC
#define ON2_CARDSTATUS_FULL_PIN			STM_CARDSTATUS_FULL_PIN
#define ON2_CARDSTATUS_VPP_FAILURE1		STM_CARDSTATUS_VPP_FAILURE1
#define ON2_CARDSTATUS_VPP_FAILUREN		STM_CARDSTATUS_VPP_FAILUREN
#define ON2_CARDSTATUS_MAX				STM_CARDSTATUS_MAX
#define ON2_CARDCALL_NONE				STM_CARDCALL_NONE
#define ON2_CARDCALL_LIMIT_REACHED		STM_CARDCALL_LIMIT_REACHED
#define ON2_CARDCALL_FULL_TRANSAC		STM_CARDCALL_FULL_TRANSAC
#define ON2_CARDCALL_FULL_PIN			STM_CARDCALL_FULL_PIN
#define ON2_CARDCALL_MAX				STM_CARDCALL_MAX

#define ON2_CASHIER_SIZE				(6 + 1)
#define ON2_FORCEPOST_NONE				0
#define ON2_FORCEPOST_ONLINE			1
#define ON2_FORCEPOST_OFFLINE			2
#define ON2_FORCEPOST_MAX				2

#define ON2_APPROVALCODE_SIZE			STM_APPROVALCODE_SIZE
#define ON2_APPROVALTEXT_SIZE			(18 + 1)
#define ON2_APPROVALSTATUS_APPROVED		0
#define ON2_APPROVALSTATUS_REJECTED		1
#define ON2_APPROVALSTATUS_REFERRAL		2
#define ON2_APPROVALRESPONSE_APPROVED	0		/* avec ON2_APPROVALSTATUS_APPROVED */
#define ON2_APPROVALRESPONSE_REJECTED	25		/* avec ON2_APPROVALSTATUS_REJECTED */
#define ON2_APPROVALRESPONSE_KEEPCARD	26		/* avec ON2_APPROVALSTATUS_REJECTED */
#define ON2_APPROVALRESPONSE_CALLBANK	27		/* avec ON2_APPROVALSTATUS_REFERRAL */
#define ON2_APPROVALRESPONSE_IDHOLDER	28		/* avec ON2_APPROVALSTATUS_REFERRAL */
#define ON2_APPROVALRESPONSE_DEFAULTS	29		/* avec ON2_APPROVALSTATUS_REJECTED */

typedef struct
{
	WORD		wSystem;
	WORD		wTerminal;
	WORD		wTransaction;
	char		szCashier[ON2_CASHIER_SIZE];
	BYTE		byCardStatusCode;
	BYTE		byCardCallCode;
	char		szCardTrack2Data[ON2_CARDTRACK2DATA_SIZE];
	BYTE		abyAlign[2];
	WORD		wCurrency;
	DWORD		dwAmount;
} ON2_REQUEST, FAR* ON2_LPREQUEST;

typedef struct
{
	ON2_REQUEST	Request;
	BYTE		byForcePostFlag;
	char		szApprovalCode[ON2_APPROVALCODE_SIZE];
} ON2_PAYMENT, FAR* ON2_LPPAYMENT;

typedef struct
{
	ON2_PAYMENT	Payment;
	char		szCardCertificate[ON2_CARDCERTIFICATE_SIZE];
	BYTE		abyAlign[3];
} ON2_COMMIT, FAR* ON2_LPCOMMIT;

typedef struct
{
	ON2_REQUEST	Request;
} ON2_REFUND, FAR* ON2_LPREFUND;

typedef struct
{
	BYTE	byApprovalStatusCode;
	BYTE	byApprovalResponseCode;
	char	szApprovalText[ON2_APPROVALTEXT_SIZE];
	char	szApprovalCode[ON2_APPROVALCODE_SIZE];
} ON2_APPROVAL, FAR* ON2_LPAPPROVAL;

/*****
* Fonctions
*   Note: Les timeouts sont exprimés en millisecondes
*****/

#ifdef __cplusplus
extern "C" {
#endif

/* Ouverture de la connexion avec le serveur monétique */
STM95DLL_API BOOL WINAPI ON2_Open(LPCSTR lpszAddress, WORD wPort);

/* Fermeture de la connexion avec le serveur monétique */
STM95DLL_API BOOL WINAPI ON2_Close();

/* Interruption de la connexion avec le serveur monétique */
STM95DLL_API BOOL WINAPI ON2_Abort();

/* Demande d'autorisation de paiement, ou émission d'une transaction forcée */
STM95DLL_API BOOL WINAPI ON2_SendPayment(	STM_LPDATETIME lpDateTime,
											ON2_LPPAYMENT lpPayment,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout,
											char errLbL[]);

/* Confirmation de paiement */
STM95DLL_API BOOL WINAPI ON2_SendCommit(	STM_LPDATETIME lpDateTime,
											ON2_LPCOMMIT lpCommit );

/* Demande de remboursement */
STM95DLL_API BOOL WINAPI ON2_SendRefund(	STM_LPDATETIME lpDateTime,
											ON2_LPREFUND lpRefund,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout );

#ifdef __cplusplus
}
#endif

#endif /* __APION2_H__ */
