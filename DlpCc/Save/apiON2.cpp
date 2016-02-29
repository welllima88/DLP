//*****
//* STM - Station de Travail Mon�tique
//* API ON2 - Communication avec le serveur mon�tique ON/2 sur le S/88
//* Couche API export�e par la DLL
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
//*****
//* apiON2.cpp
//*****

#include "STM95DLLpriv.h"
#include "apiON2.h"
#include "ON2.h"

/////////////////////////////////////////////////////////////////////////////
// Instance unique et locale de la classe CON2

static CON2 theON2;

/////////////////////////////////////////////////////////////////////////////
// Couche API export�e par la DLL
//   Note: Les timeouts sont exprim�s en millisecondes

// Ouverture de la connexion avec le serveur mon�tique
STM95DLL_API BOOL WINAPI ON2_Open(LPCSTR lpszAddress, WORD wPort)
{
	return theON2.Open(lpszAddress, wPort);
}

// Fermeture de la connexion avec le serveur mon�tique
STM95DLL_API BOOL WINAPI ON2_Close()
{
	return theON2.Close();
}

// Interruption de la connexion avec le serveur mon�tique
STM95DLL_API BOOL WINAPI ON2_Abort()
{
	return theON2.Abort();
}

// Demande d'autorisation de paiement, ou �mission d'une transaction forc�e
STM95DLL_API BOOL WINAPI ON2_SendPayment(	STM_LPDATETIME lpDateTime,
											ON2_LPPAYMENT lpPayment,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout,
											char errLbL[])
{
	return theON2.SendPayment(lpDateTime, lpPayment, lpApproval, dwTimeout, errLbL[]);
}

// Confirmation de paiement
STM95DLL_API BOOL WINAPI ON2_SendCommit(	STM_LPDATETIME lpDateTime,
											ON2_LPCOMMIT lpCommit )
{
	return theON2.SendCommit(lpDateTime, lpCommit);
}

// Demande de remboursement
STM95DLL_API BOOL WINAPI ON2_SendRefund(	STM_LPDATETIME lpDateTime,
											ON2_LPREFUND lpRefund,
											ON2_LPAPPROVAL lpApproval,
											DWORD dwTimeout )
{
	return theON2.SendRefund(lpDateTime, lpRefund, lpApproval, dwTimeout);
}
