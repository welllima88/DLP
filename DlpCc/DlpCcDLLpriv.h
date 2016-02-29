/*****
* DlpCc - Inteface Galaxy - C3
* Header commun privé de DLPCCDLL.DLL
* SJU - 26 avril 2001
*****
* DlpCcDLLpriv.h
*****/

#ifndef __DLPCCDLLPRIV_H__
#define __DLPCCDLLPRIV_H__

/*****
* Désactivation de quelques warnings
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
* Exportation des API de DLPCCDLL.DLL
*****/

#define DLPCCDLL_API __declspec(dllexport)

#endif /* __DLPCCDLLPRIV_H__ */
