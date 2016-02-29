// EncoDeco.cpp : définit le point d'entrée pour l'application DLL.
//

#include "stdafx.h"
#include "EncoDeco.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// Il s'agit d'un exemple de variable exportée
ENCODECO_API int nEncoDeco=0;

// Il s'agit d'un exemple de fonction exportée.
ENCODECO_API int fnEncoDeco(void)
{
	return 42;
}

ENCODECO_API void Encode(void)
{
}

ENCODECO_API void Decode(void)
{
}


// Il s'agit du constructeur d'une classe qui a été exportée.
// consultez EncoDeco.h pour la définition de la classe
CEncoDeco::CEncoDeco()
{
	return;
}
