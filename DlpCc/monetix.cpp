// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "monetix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// _Dialogs properties

/////////////////////////////////////////////////////////////////////////////
// _Dialogs operations

short _Dialogs::axDisplayPos(BSTR* Text)
{
	short result;
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x60030002, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		Text);
	return result;
}
