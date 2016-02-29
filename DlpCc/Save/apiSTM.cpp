//*****
//* STM - Station de Travail Mon�tique
//* API STM - Utilitaires STM
//* Couche API export�e par la DLL
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
//*****
//* apiSTM.cpp
//*****

#include "STM95DLLpriv.h"
#include "apiSTM.h"
#include <time.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// Couche API export�e par la DLL

// Lecture de la date et de l'heure syst�me
STM95DLL_API BOOL WINAPI STM_GetDateTime(STM_LPDATETIME lpDateTime)
{
	// V�rifier les param�tres
	if (lpDateTime == NULL)
	{
		return FALSE;
	}

	// Lire la date et l'heure syst�me
	time_t timeNow = ::time(NULL);
	tm* ptmNow = ::localtime(&timeNow);
	if (ptmNow == NULL)
	{
		return FALSE;
	}

	// Retourner la date et l'heure syst�me
	char buffer[50];
	
	lpDateTime->wYear = short(ptmNow->tm_year + 1900);
	lpDateTime->byMonth = char(itoa(ptmNow->tm_mon + 1, buffer, 10));
	lpDateTime->byDay = char(itoa(ptmNow->tm_mday, buffer, 10));
	lpDateTime->byHour = char(itoa(ptmNow->tm_hour, buffer, 10));
	lpDateTime->byMin = char(itoa(ptmNow->tm_min, buffer, 10));
	return TRUE;
}
