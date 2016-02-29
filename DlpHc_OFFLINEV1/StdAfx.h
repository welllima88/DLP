// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_)
#define AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_

#define  CURRENCY_FRANCS "250"	/*numerical ISO for transaction in francs*/
#define  CURRENCY_EURO "978"		/*numerical ISO for transaction in euros*/
#define  NO_WAIT_KEY       0
#define  WAIT_KEY          1
#define  WAIT_1_SEC        2

#define SOCKON2_HEADER_FORMAT			"%c%04u%04u"
#define SOCKON2_HEADER_LEN				9
#define SOCKON2_HEADERTYPE_CONVERSATION	'C'
#define SOCKON2_HEADERTYPE_ACKNOWLEDGE	'A'
#define SOCKON2_HEADERTYPE_KEEPALIVE	'K'
#define SOCKON2_HEADERNUMBER_MAX		9999
#define SOCKON2_HEADERLENGTH_MAX		9999
#define SOCKON2_TIMEOUT_ACKNOWLEDGE		10000
#define SOCKON2_TIMEOUT_DISCARD			1000
#define SOCKON2_DISCARDBUFFER_LEN		64

#define DLPHC_PARAM_PATH				"SOFTWARE\\Disneyland Paris\\DlpHc\\Settings"
#define SERVER_IP						"ServerIP"
#define	SERVER_PORT						"ServerPort"

//#define HC_SECURITY_CODE				

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)



#include <windows.h>
#include <winreg.h>
#include <winerror.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

#include "DlpHcStructs.h"




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_)


