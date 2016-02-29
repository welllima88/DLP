//*****
//* STM - Station de Travail Mon�tique
//* API TPE - Communication Titus
//* Couche API export�e par la DLL
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
//*****
//* apiTPE.cpp
//*****

#include "STM95DLLpriv.h"
#include "apiTPE.h"
#include "TPE.h"

/////////////////////////////////////////////////////////////////////////////
// Instance unique et locale de la classe CTPE

static CTPE theTPE;

/////////////////////////////////////////////////////////////////////////////
// Couche API export�e par la DLL
//   Note: Les timeouts sont exprim�s en millisecondes

// Initialisation du TPE et retour � l'�tat repos
STM95DLL_API BOOL WINAPI TPE_Reset(LPCSTR lpszPortName)
{
	return theTPE.Reset(lpszPortName);
}

// Fermeture de la connexion avec le TPE
STM95DLL_API BOOL WINAPI TPE_Close()
{
	return theTPE.Close();
}

// Lecture de l'�tat du TPE
STM95DLL_API BOOL WINAPI TPE_GetStatus(	DWORD dwTimeout,
										TPE_LPSTATUS lpStatus )
{
	return theTPE.GetStatus(dwTimeout, lpStatus);
}

// Lecture d'une carte sur le TPE
STM95DLL_API BOOL WINAPI TPE_ReadCard(	DWORD dwTimeout,
										TPE_LPREAD lpRead,
										STM_LPDATETIME lpDateTime )
{
	return theTPE.ReadCard(dwTimeout, lpRead, lpDateTime);
}

// Ecriture d'une transaction et/ou impression du ticket sur le TPE
STM95DLL_API BOOL WINAPI TPE_WriteCard(	DWORD dwTimeout,
										TPE_LPWRITE lpWrite )
{
	return theTPE.WriteCard(dwTimeout, lpWrite);
}

// Invalidation de la carte � puce ins�r�e dans le TPE
STM95DLL_API BOOL WINAPI TPE_LockCard(	DWORD dwTimeout,
										TPE_LPLOCK lpLock )
{
	return theTPE.LockCard(dwTimeout, lpLock);
}
