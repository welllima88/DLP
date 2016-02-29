#if !defined(AFX_FRMDIAL_H__7FD532A3_8B2F_11D5_AFD6_0000F68597F0__INCLUDED_)
#define AFX_FRMDIAL_H__7FD532A3_8B2F_11D5_AFD6_0000F68597F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmDial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FrmDial dialog

class FrmDial : public CDialog
{
// Construction
public:
	FrmDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FrmDial)
	enum { IDD = _UNKNOWN_RESOURCE_ID_ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FrmDial)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual int DoModal();
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FrmDial)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMDIAL_H__7FD532A3_8B2F_11D5_AFD6_0000F68597F0__INCLUDED_)
