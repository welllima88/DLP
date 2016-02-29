/*****
* DLPCC - Interface Caisse <==> Protocole Resa DLRP
*
* Composant monétique - Stéphane JUILLARD (SJU) - Février 2007
*****
* apiON2.h
****
*/

#include "EdlStd.h"

#ifndef __APION2_H__
#define __APION2_H__



/*****
* Structures
*****/

#define ON2_CASHIER_SIZE				(6 + 1)
#define ON2_CARDTRACK2DATA_SIZE			(40 + 1)
#define ON2_NUM_AUTO					(10 + 1)
#define ON2_CARDCERTIFICATE_SIZE		(8 + 1)

#define ON2_FORCEPOST_NONE				0
#define ON2_FORCEPOST_ONLINE			1
#define ON2_FORCEPOST_OFFLINE			2
#define ON2_FORCEPOST_MAX				2

#define ON2_APPROVALCODE_SIZE			(6 + 1)
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
	WORD		wCvx2;
	char		szNumAuto[ON2_NUM_AUTO];
} ON2_REQUEST, FAR* ON2_LPREQUEST;

typedef struct
{
	char        byForcePostFlag[1];
	char		szApprovalCode[ON2_APPROVALCODE_SIZE];
	ON2_REQUEST	Request;
} ON2_PAYMENT, FAR* ON2_LPPAYMENT;

typedef struct
{
	char		szCardCertificate[ON2_CARDCERTIFICATE_SIZE];
	BYTE		abyAlign[3];
	ON2_PAYMENT	Payment;
} ON2_COMMIT, FAR* ON2_LPCOMMIT;

typedef struct
{
	ON2_REQUEST	Request;
} ON2_REFUND, FAR* ON2_LPREFUND;


typedef struct
{
  ON2_REQUEST	Request;
} ON2_VERIF, FAR* ON2_LPVERIF;

typedef struct
{
	BYTE	byApprovalStatusCode;
	BYTE	byApprovalResponseCode;
	char	szApprovalText[ON2_APPROVALTEXT_SIZE];
	char	szApprovalCode[ON2_APPROVALCODE_SIZE];
	char    szAlias[20]; //SJ pour alias
} ON2_APPROVAL, FAR* ON2_LPAPPROVAL;

/*****
* Fonctions
*   Note: Les timeouts sont exprimés en millisecondes
*****/



typedef struct
{
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	BYTE	byHour;
	BYTE	byMin;
	BYTE	abyAlign[2];
} STM_DATETIME, FAR* STM_LPDATETIME;




extern int DLP_GetSecurity();
extern int DLP_GetKey(); 
extern int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay);
extern void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm);
extern LPVOID CreateMappingFile();
extern void DestroyMappingFile(LPVOID MPtr);
extern void DLP_PrintTicket (char * messageToPrint);



/*****
* Fonctions internes
*****/
 

bool TraiteResa(CcPosMsgSend *lpGalxSend, CcPosMsgRecv *lpGalxRecv);
bool FormateMsgPourResa( CcPosMsgSend *lpGalxSend, ON2_LPPAYMENT lpPayment, 
						 STM_LPDATETIME lpDateTime, ON2_LPREFUND lpRefund);
bool TraiteResa_EcranMonetique(char *Msg, lpCcPosMsgSend Galaxy);

bool FormateCommitPourResa( CcPosMsgSend *lpGalxSend, ON2_LPPAYMENT lpPayment, 
						 STM_LPDATETIME lpDateTime, ON2_LPCOMMIT lpCommit);

bool FormateCommitCreditPourResa (CcPosMsgSend *lpGalxSend, ON2_LPREFUND lpRefund, 
						 STM_LPDATETIME lpDateTime, ON2_LPCOMMIT lpCommit);
 






/*****
* Fonctions importées
*****/

extern bool GetAxisCom(char *fConfig); 
extern bool IsPosteSTM(char *fConfig); 
extern bool IsPostePKG(char *fConfig); 

/* Ouverture de la connexion avec le serveur monétique */
extern "C"  BOOL __stdcall ON2_Open(LPCSTR , WORD);

/* Fermeture de la connexion avec le serveur monétique */
extern "C" BOOL __stdcall ON2_Close(); 

/* Interruption de la connexion avec le serveur monétique */
extern BOOL ON2_Abort(void);

/* Demande d'autorisation de paiement, ou émission d'une transaction forcée */
extern "C" int __stdcall ON2_SendPayment(	STM_LPDATETIME lpDateTime,
											ON2_LPPAYMENT lpPayment,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout , 
											char errLbL[]);

/* Confirmation de paiement */
extern "C" BOOL __stdcall ON2_SendCommit(	STM_LPDATETIME lpDateTime,
											ON2_LPCOMMIT lpCommit );

/* Demande de remboursement */
extern "C" BOOL __stdcall ON2_SendRefund(	STM_LPDATETIME lpDateTime,
											ON2_LPREFUND lpRefund,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout );


extern BOOL WINAPI STM_GetDateTime(STM_LPDATETIME lpDateTime);



#endif /* __APION2_H__ */
