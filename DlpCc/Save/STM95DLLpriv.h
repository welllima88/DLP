/*****
* STM - Station de Travail Mon�tique
* Header commun priv� de STM95DLL.DLL
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
*****
* STM95DLLpriv.h
*****/

#ifndef __STM95DLLPRIV_H__
#define __STM95DLLPRIV_H__

/*****
* D�sactivation de quelques warnings
*****/

#pragma warning(disable: 4201)
#pragma warning(disable: 4214)
#pragma warning(disable: 4514)
#ifndef _DEBUG
#pragma warning(disable: 4702)
#endif

/*****
* Header principal du SDK
*****/

#include <windows.h>

/*****
* Exportation des API de STM95DLL.DLL
*****/

#define STM95DLL_API __declspec(dllexport)

#endif /* __STM95DLLPRIV_H__ */
