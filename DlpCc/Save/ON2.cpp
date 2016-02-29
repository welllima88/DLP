//*****
//* STM - Station de Travail Monétique
//* API ON2 - Communication avec le serveur monétique ON/2 sur le S/88
//* Couche haut niveau: protocole ON/2
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
//*****
//* ON2.cpp
//*****

#include "STM95DLLpriv.h"
#include "ON2.h"

#define ON2_PAYMENTMESSAGE_FORMAT	"0%02u00000%02u\x2E" "%-6.6s%01u%04u%04u00000000 %09lu%04u%01u"	\
									"0000%02u%02u%02u%02u%02u%01X%01u00000000" "%03u000000000000"	\
									"%-6.6s00000000000000000000%-40.40s00000000000000000000\x0D"
#define ON2_PAYMENTMESSAGE_SIZE		(175 + 1)

#define ON2_COMMITMESSAGE_FORMAT	"0%02u00000%02u\x2F" "%-6.6s%01u%04u%04u00000000 %09lu%04u%01u"	\
									"0000%02u%02u%02u%02u%02u%01X%01u%-8.8s"   "%03u000000000000"	\
									"%-6.6s00000000000000000000%-40.40s00000000000000000000\x0D"
#define ON2_COMMITMESSAGE_SIZE		(175 + 1)

#define ON2_REFUNDMESSAGE_FORMAT	"0%02u00000%02u\x2E" "000000%01u%04u%04u00000000-%09lu%04u0"	\
									"0000%02u%02u%02u%02u%02u%01X%01u00000000" "%03u000000000000"	\
									"%-6.6s00000000000000000000%-40.40s00000000000000000000\x0D"
#define ON2_REFUNDMESSAGE_SIZE		(175 + 1)



#define ON2_APPROVALMESSAGE_SIZE	(73 + 1)
#define ON2_APPROVALMESSAGE_EOMCHAR	'\x0D'
#define ON2_APPROVALSTATUS_OFFSET	10
#define ON2_APPROVALRESPONSE_OFFSET	19
#define ON2_APPROVALTEXT_OFFSET		33
#define ON2_APPROVALCODE_OFFSET		51
#define ON2_APPROVALEOMCHAR_OFFSET	(ON2_APPROVALMESSAGE_SIZE - 2)

#define ON2_APPROVALCHECK_FORMAT	"000%02u000%02u_\x2E%01u%04u%01u1__      %06u"
#define ON2_APPROVALCHECK_SIZE		(33 + 1)
#define ON2_APPROVALTYPE_PAYMENT	2
#define ON2_APPROVALTYPE_REFUND		4
#define ON2_APPROVALCHECK_OFFSET1	0
#define ON2_APPROVALCHECK_LENGTH1	10
#define ON2_APPROVALCHECK_OFFSET2	11
#define ON2_APPROVALCHECK_LENGTH2	8
#define ON2_APPROVALCHECK_OFFSET3	21
#define ON2_APPROVALCHECK_LENGTH3	12

#define ON2_SEQUENCENUMBER_MAX		99
#define ON2_SOURCETERMINAL_MODULUS	100
#define ON2_IDTERMINAL_MODULUS		10

/////////////////////////////////////////////////////////////////////////////
// Classe CON2
//   Note: Les timeouts sont exprimés en millisecondes

WORD CON2::m_wSequenceNumber = 0;

// Construction/Destruction
CON2::CON2()
{
}

CON2::~CON2()
{
}

// Operations
int CON2::SendPayment(	STM_LPDATETIME lpDateTime,
						ON2_LPPAYMENT lpPayment,
						ON2_LPAPPROVAL lpApproval,
						DWORD dwTimeout,
						char lblError[])
{
	// Vérifier les paramètres
	//   Note: lpApproval doit être NULL pour une transaction forcée
	if (   (lpDateTime == NULL)
		|| (lpPayment == NULL)
		|| ((lpPayment->byForcePostFlag == ON2_FORCEPOST_NONE) && (lpApproval == NULL))
		|| ((lpPayment->byForcePostFlag != ON2_FORCEPOST_NONE) && (lpApproval != NULL)) )
	{
		return FALSE;
	}

	// Vérifier les paramétres transmis
	if(lpPayment->Request.wSystem > ON2_SYSTEM_MAX)				
		{
		strcpy(lblError,"wSystem field error");
		return(false);
		}
	if(lpPayment->Request.wTerminal > ON2_TERMINAL_MAX)			
		{
		strcpy(lblError,"wTerminal field error");
		return(false);
		}
	if(lpPayment->Request.wTransaction > ON2_TRANSACTION_MAX)	
		{
		strcpy(lblError,"wTransaction field error");
		return(false);
		}
	if(lpPayment->Request.wCurrency > ON2_CURRENCY_MAX)			
		{
		strcpy(lblError,"wCurrency field error");
		return(false);
		}
	if(lpPayment->Request.dwAmount > ON2_AMOUNT_MAX)			
		{
		strcpy(lblError,"dwAmount field error");
		return(false);
		}
	if(lpPayment->Request.byCardStatusCode > ON2_CARDSTATUS_MAX)
		{
		strcpy(lblError,"byCardStatusCode field error");
		return(false);
		}
	if(lpPayment->Request.byCardCallCode > ON2_CARDCALL_MAX)	
		{
		strcpy(lblError,"byCardCallCode field error");
		return(false);
		}
	if(lpPayment->byForcePostFlag > ON2_FORCEPOST_MAX)			
		{
		strcpy(lblError,"byForcePostFlag field error");
		return(false);
		}
	if(!VerifyCardDate(lpPayment->Request.szCardTrack2Data))	
		{
		strcpy(lblError,"szCardTrack2Data field error : Date d'expiration erronée");
		return(false);
		}
	if(!IsLuhn(lpPayment->Request.szCardTrack2Data))			
		{
		strcpy(lblError,"szCardTrack2Data field error : numéro de carte erroné");
		return(false);
		}
	
	// Emettre la demande d'autorisation de paiement, ou la transaction forcée
	char szPaymentMessage[ON2_PAYMENTMESSAGE_SIZE];
	WORD wSequenceNumber = GetNextSequenceNumber();
	if (   (::wsprintfA(	szPaymentMessage,
							ON2_PAYMENTMESSAGE_FORMAT,
							lpPayment->Request.wTerminal % ON2_SOURCETERMINAL_MODULUS,
							wSequenceNumber,
							lpPayment->szApprovalCode,
							lpPayment->Request.wTerminal % ON2_IDTERMINAL_MODULUS,
							lpPayment->Request.wSystem,
							lpPayment->Request.wTerminal,
							lpPayment->Request.dwAmount,
							lpPayment->Request.wTransaction,
							lpPayment->byForcePostFlag,
							lpDateTime->wYear % 100,
							lpDateTime->byMonth,
							lpDateTime->byDay,
							lpDateTime->byHour,
							lpDateTime->byMin,
							lpPayment->Request.byCardStatusCode,
							lpPayment->Request.byCardCallCode,
							lpPayment->Request.wCurrency,
							lpPayment->Request.szCashier,
							lpPayment->Request.szCardTrack2Data )
			!= (ON2_PAYMENTMESSAGE_SIZE - 1) )
		|| !Send(szPaymentMessage, ON2_PAYMENTMESSAGE_SIZE - 1) )
	{
		return FALSE;
	}

	// Recevoir l'autorisation de paiement
	if (lpPayment->byForcePostFlag == ON2_FORCEPOST_NONE)
	{
		// Recevoir, vérifier et décoder le message d'autorisation de paiement
		if (!ReceiveApproval(	&lpPayment->Request,
								lpApproval,
								dwTimeout,
								wSequenceNumber,
								ON2_APPROVALTYPE_PAYMENT ))
		{
			return FALSE;
		}

		// Copier le code autorisation obtenu
		::lstrcpynA(	lpPayment->szApprovalCode,
						lpApproval->szApprovalCode,
						sizeof(lpPayment->szApprovalCode) );
	}

	// Emission de la demande de paiement ou de la transaction forcée terminée
	return TRUE;
}

BOOL CON2::SendCommit(	STM_LPDATETIME lpDateTime,
						ON2_LPCOMMIT lpCommit )
{
	// Vérifier les paramètres
	if (   (lpDateTime == NULL)
		|| (lpCommit == NULL) )
	{
		return FALSE;
	}

	// Vérifier les paramétres transmis
	if (   (lpCommit->Payment.Request.wSystem > ON2_SYSTEM_MAX)
		|| (lpCommit->Payment.Request.wTerminal > ON2_TERMINAL_MAX)
		|| (lpCommit->Payment.Request.wTransaction > ON2_TRANSACTION_MAX)
		|| (lpCommit->Payment.Request.wCurrency > ON2_CURRENCY_MAX)
		|| (lpCommit->Payment.Request.dwAmount > ON2_AMOUNT_MAX)
		|| (lpCommit->Payment.Request.byCardStatusCode > ON2_CARDSTATUS_MAX)
		|| (lpCommit->Payment.Request.byCardCallCode > ON2_CARDCALL_MAX)
		|| (lpCommit->Payment.byForcePostFlag > ON2_FORCEPOST_MAX) )
	{
		return FALSE;
	}

	// Emettre la confirmation de paiement
	char szCommitMessage[ON2_COMMITMESSAGE_SIZE];
	WORD wSequenceNumber = GetNextSequenceNumber();
	if (   (::wsprintfA(	szCommitMessage,
							ON2_COMMITMESSAGE_FORMAT,
							lpCommit->Payment.Request.wTerminal % ON2_SOURCETERMINAL_MODULUS,
							wSequenceNumber,
							lpCommit->Payment.szApprovalCode,
							lpCommit->Payment.Request.wTerminal % ON2_IDTERMINAL_MODULUS,
							lpCommit->Payment.Request.wSystem,
							lpCommit->Payment.Request.wTerminal,
							lpCommit->Payment.Request.dwAmount,
							lpCommit->Payment.Request.wTransaction,
							lpCommit->Payment.byForcePostFlag,
							lpDateTime->wYear % 100,
							lpDateTime->byMonth,
							lpDateTime->byDay,
							lpDateTime->byHour,
							lpDateTime->byMin,
							lpCommit->Payment.Request.byCardStatusCode,
							lpCommit->Payment.Request.byCardCallCode,
							lpCommit->szCardCertificate,
							lpCommit->Payment.Request.wCurrency,
							lpCommit->Payment.Request.szCashier,
							lpCommit->Payment.Request.szCardTrack2Data )
			!= (ON2_COMMITMESSAGE_SIZE - 1) )
		|| !Send(szCommitMessage, ON2_COMMITMESSAGE_SIZE - 1) )
	{
		return FALSE;
	}

	// Emission de la confirmation de paiement terminée
	return TRUE;
}

BOOL CON2::SendRefund(	STM_LPDATETIME lpDateTime,
						ON2_LPREFUND lpRefund,
						ON2_LPAPPROVAL lpApproval,
						DWORD dwTimeout )
{
	// Vérifier les paramètres
	if (   (lpDateTime == NULL)
		|| (lpRefund == NULL)
		|| (lpApproval == NULL) )
	{
		return FALSE;
	}

	// Vérifier les paramétres transmis
	if (   (lpRefund->Request.wSystem > ON2_SYSTEM_MAX)
		|| (lpRefund->Request.wTerminal > ON2_TERMINAL_MAX)
		|| (lpRefund->Request.wTransaction > ON2_TRANSACTION_MAX)
		|| (lpRefund->Request.wCurrency > ON2_CURRENCY_MAX)
		|| (lpRefund->Request.dwAmount > ON2_AMOUNT_MAX)
		|| (lpRefund->Request.byCardStatusCode > ON2_CARDSTATUS_MAX)
		|| (lpRefund->Request.byCardCallCode > ON2_CARDCALL_MAX) )
	{
		return FALSE;
	}

	// Emettre la demande de remboursement
	char szRefundMessage[ON2_REFUNDMESSAGE_SIZE];
	WORD wSequenceNumber = GetNextSequenceNumber();
	if (   (::wsprintfA(	szRefundMessage,
							ON2_REFUNDMESSAGE_FORMAT,
							lpRefund->Request.wTerminal % ON2_SOURCETERMINAL_MODULUS,
							wSequenceNumber,
							lpRefund->Request.wTerminal % ON2_IDTERMINAL_MODULUS,
							lpRefund->Request.wSystem,
							lpRefund->Request.wTerminal,
							lpRefund->Request.dwAmount,
							lpRefund->Request.wTransaction,
							lpDateTime->wYear % 100,
							lpDateTime->byMonth,
							lpDateTime->byDay,
							lpDateTime->byHour,
							lpDateTime->byMin,
							lpRefund->Request.byCardStatusCode,
							lpRefund->Request.byCardCallCode,
							lpRefund->Request.wCurrency,
							lpRefund->Request.szCashier,
							lpRefund->Request.szCardTrack2Data )
			!= (ON2_REFUNDMESSAGE_SIZE - 1) )
		|| !Send(szRefundMessage, ON2_REFUNDMESSAGE_SIZE - 1) )
	{
		return FALSE;
	}

	// Recevoir, vérifier et décoder le message d'acceptation de remboursement
	if (!ReceiveApproval(	&lpRefund->Request,
							lpApproval,
							dwTimeout,
							wSequenceNumber,
							ON2_APPROVALTYPE_REFUND ))
	{
		return FALSE;
	}

	// Emission de la demande de remboursement terminée
	return TRUE;
}

// Implementation
BOOL CON2::ReceiveApproval(	ON2_LPREQUEST lpRequest,
							ON2_LPAPPROVAL lpApproval,
							DWORD dwTimeout,
							WORD wSequenceNumber,
							WORD wApprovalType )
{
	// Vérifier les paramètres
	if (   (lpRequest == NULL)
		|| (lpApproval == NULL) )
	{
		return FALSE;
	}

	// Vérifier les paramétres transmis
	if (   (lpRequest->wSystem > ON2_SYSTEM_MAX)
		|| (lpRequest->wTerminal > ON2_TERMINAL_MAX)
		|| (lpRequest->wTransaction > ON2_TRANSACTION_MAX)
		|| (wSequenceNumber > ON2_SEQUENCENUMBER_MAX)
		|| (   (wApprovalType != ON2_APPROVALTYPE_PAYMENT)
			&& (wApprovalType != ON2_APPROVALTYPE_REFUND) ) )
	{
		return FALSE;
	}

	// Recevoir la réponse
	char szApprovalMessage[ON2_APPROVALMESSAGE_SIZE];
	if (   !Receive(dwTimeout, szApprovalMessage, ON2_APPROVALMESSAGE_SIZE - 1)
		|| (szApprovalMessage[ON2_APPROVALEOMCHAR_OFFSET] != ON2_APPROVALMESSAGE_EOMCHAR) )
	{
		return FALSE;
	}
	szApprovalMessage[ON2_APPROVALMESSAGE_SIZE - 1] = 0;

	// Vérifier que la réponse reçue correspond à la demande d'autorisation
	char szApprovalCheck[ON2_APPROVALCHECK_SIZE];
	if (   (::wsprintfA(	szApprovalCheck,
							ON2_APPROVALCHECK_FORMAT,
							lpRequest->wTerminal % ON2_SOURCETERMINAL_MODULUS,
							wSequenceNumber,
							wApprovalType,
							lpRequest->wSystem,
							lpRequest->wTerminal % ON2_IDTERMINAL_MODULUS,
							lpRequest->wTransaction )
			!= (ON2_APPROVALCHECK_SIZE - 1) )
		|| (::memcmp(	szApprovalMessage + ON2_APPROVALCHECK_OFFSET1,
						szApprovalCheck   + ON2_APPROVALCHECK_OFFSET1,
						ON2_APPROVALCHECK_LENGTH1 ) != 0)
		|| (::memcmp(	szApprovalMessage + ON2_APPROVALCHECK_OFFSET2,
						szApprovalCheck   + ON2_APPROVALCHECK_OFFSET2,
						ON2_APPROVALCHECK_LENGTH2 ) != 0)
		|| (::memcmp(	szApprovalMessage + ON2_APPROVALCHECK_OFFSET3,
						szApprovalCheck   + ON2_APPROVALCHECK_OFFSET3,
						ON2_APPROVALCHECK_LENGTH3 ) != 0) )
	{
		return FALSE;
	}

	// Décoder le message de réponse à la demande d'autorisation
	if (   (szApprovalMessage[ON2_APPROVALSTATUS_OFFSET] < '0')
		|| (szApprovalMessage[ON2_APPROVALSTATUS_OFFSET] > '9')
		|| (szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET] < '0')
		|| (szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET] > '9')
		|| (szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET + 1] < '0')
		|| (szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET + 1] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	lpApproval->byApprovalStatusCode = char(itoa(szApprovalMessage[ON2_APPROVALSTATUS_OFFSET] - '0',buffer,10));
	lpApproval->byApprovalResponseCode	= char(itoa((szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET    ] - '0') * 10
										+ (szApprovalMessage[ON2_APPROVALRESPONSE_OFFSET + 1] - '0'),buffer,10));
	::lstrcpynA(	lpApproval->szApprovalText,
					szApprovalMessage + ON2_APPROVALTEXT_OFFSET,
					sizeof(lpApproval->szApprovalText) );
	::lstrcpynA(	lpApproval->szApprovalCode,
					szApprovalMessage + ON2_APPROVALCODE_OFFSET,
					sizeof(lpApproval->szApprovalCode) );

	// Réception, vérification et décodage de la réponse à la demande d'autorisation terminés
	return TRUE;
}

WORD CON2::GetNextSequenceNumber()
{
	// Ramener le numéro séquentiel entre 0 et (ON2_SEQUENCENUMBER_MAX - 1)
	m_wSequenceNumber %= ON2_SEQUENCENUMBER_MAX;

	// Retourner un numéro séquentiel compris entre 1 et ON2_SEQUENCENUMBER_MAX
	return ++m_wSequenceNumber;
}


bool CON2::VerifyCardDate(char PisteCB[])
//prend en parametre la piste de la carte
{
int i=1;
char Piste[41];
strcpy(Piste, PisteCB);

//recuperation de la date formatée et verification de la numericite
i=1;
while(Piste[i] != '=') i++;
for(int j=1;i<5;i++) if(Piste[i+j]<'0' || Piste[i+j]>'9') return(false);

return(true);

}//fin de la fonction


bool CON2::IsLuhn(char PisteCB[])
{    
int indice, num,i=1, CheckSum;
bool dbl;
char NumCarte[19];

//recuperation du numero de carte et vérification

while(PisteCB[i] != '=' && i<20)
	{
	NumCarte[i-1] = PisteCB[i];
	i++;
	}
    
dbl = false;
CheckSum = 0;

for(indice = strlen(NumCarte);indice>=1;indice--)
	{
    num = NumCarte[indice];
    if(dbl)
		{
		dbl = false;
		num *= 2;
		if(num>=10) num -= 9;
		}	else dbl = true;
    CheckSum += num;
	}//fin du for
    
return((CheckSum%10) == 0);

}//fin de la fonction



