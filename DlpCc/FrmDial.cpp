// FrmDial.cpp : implementation file
//

#include "stdafx.h"
#include "dlpcc.h"
#include "FrmDial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FrmDial dialog


FrmDial::FrmDial(CWnd* pParent /*=NULL*/)
	: CDialog(FrmDial::IDD, pParent)
{
	//{{AFX_DATA_INIT(FrmDial)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void FrmDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FrmDial)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FrmDial, CDialog)
	//{{AFX_MSG_MAP(FrmDial)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FrmDial message handlers

BOOL FrmDial::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

int FrmDial::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

BOOL FrmDial::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}

LRESULT FrmDial::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL FrmDial::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.x=0000x;
	cs.y=0000x;
	cs.lpszName = "Fenêtre virtuelle";  

	return CDialog::PreCreateWindow(cs);
}

BOOL FrmDial::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreCreateWindow(cs);
}
