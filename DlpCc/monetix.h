// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// _Dialogs wrapper class

class _Dialogs : public COleDispatchDriver
{
public:
	_Dialogs() {}		// Calls COleDispatchDriver default constructor
	_Dialogs(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Dialogs(const _Dialogs& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	//	short axDisplayPos(BSTR* Text);
		short axDisplayPos(LPSTR Text);
};
