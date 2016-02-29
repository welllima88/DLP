//*****
//* STM - Station de Travail Monétique
//* API STM - Utilitaires STM
//* Couche API exportée par la DLL
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
//*****
//* apiSTM.cpp
//*****

#include "STM95DLLpriv.h"
#include "apiSTM.h"
#include <time.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// Couche API exportée par la DLL

// Lecture de la date et de l'heure système
STM95DLL_API BOOL WINAPI STM_GetDateTime(STM_LPDATETIME lpDateTime)
{
	// Vérifier les paramètres
	if (lpDateTime == NULL)
	{
		return FALSE;
	}

	// Lire la date et l'heure système
	time_t timeNow = ::time(NULL);
	tm* ptmNow = ::localtime(&timeNow);
	if (ptmNow == NULL)
	{
		return FALSE;
	}

	// Retourner la date et l'heure système
	char buffer[50];
	
	lpDateTime->wYear = short(ptmNow->tm_year + 1900);
	lpDateTime->byMonth = char(itoa(ptmNow->tm_mon + 1, buffer, 10));
	lpDateTime->byDay = char(itoa(ptmNow->tm_mday, buffer, 10));
	lpDateTime->byHour = char(itoa(ptmNow->tm_hour, buffer, 10));
	lpDateTime->byMin = char(itoa(ptmNow->tm_min, buffer, 10));
	return TRUE;
}
