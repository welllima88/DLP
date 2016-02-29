//*****
//* DlpCc - Interface Galaxy - C3
//* Initialisation et terminaison de DLPCCDLL.DLL
//* SJU - 26 avril 2001
//*****
//* DLPCCDLL.cpp
//*****

#include "DLPCCDLLpriv.h"


/////////////////////////////////////////////////////////////////////////////
// Initialisation et terminaison de DLPCCDLL.DLL

BOOL WINAPI DllMain(HANDLE /*hDllHandle*/, DWORD dwReason, LPVOID /*lpReserved*/)
{
	int szRet;

	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			 //szRet = MessageBox(NULL,"DLL_PROCESS_ATTACH","DLPCCDLL.cpp",MB_OK);

		}
		// PAS DE break;

		case DLL_THREAD_ATTACH:
		{
			//szRet = MessageBox(NULL,"DLL_THREAD_ATTACH","DLPCCDLL.cpp",MB_OK);
			return TRUE;
		}
		break;

		case DLL_THREAD_DETACH:
		{
			//szRet = MessageBox(NULL,"DLL_THREAD_DETACH","DLPCCDLL.cpp",MB_OK);
			return TRUE;
		}
		break;

		case DLL_PROCESS_DETACH:
		{
			//szRet = MessageBox(NULL,"DLL_PROCESS_DETACH","DLPCCDLL.cpp",MB_OK);

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
