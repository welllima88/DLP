// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_)
#define AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_

#define CURRENCY_FRANCS "250"	/*numerical ISO for transaction in francs*/
#define CURRENCY_EURO "978"		/*numerical ISO for transaction in euros*/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)



// Pour ActiveX #include <windows.h>
//#include <memory.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "AfxDisp.h"
//#include "MonetiX.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6798E144_2E88_11D5_B8F8_0000F685900E__INCLUDED_)
