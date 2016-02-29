//*****
//* STM - Station de Travail Monétique
//* Initialisation et terminaison de STM95DLL.DLL
//* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
//*****
//* STM95DLL.cpp
//*****

#include "STM95DLLpriv.h"

/////////////////////////////////////////////////////////////////////////////
// Initialisation et terminaison de STM95DLL.DLL

BOOL WINAPI DllMain(HANDLE /*hDllHandle*/, DWORD dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
		}
		// PAS DE break;

		case DLL_THREAD_ATTACH:
		{
			return TRUE;
		}
		break;

		case DLL_THREAD_DETACH:
		{
			return TRUE;
		}
		break;

		case DLL_PROCESS_DETACH:
		{
			return TRUE;
		}
		break;

		default:
		{
			return FALSE;
		}
		break;
	}
}
