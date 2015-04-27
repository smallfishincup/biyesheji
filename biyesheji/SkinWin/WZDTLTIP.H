//***************************************************************************
//	Description		: Tool Tip Class
//  File Name		: WzdTlTip.h
//***************************************************************************

#if !defined(AFX_WZDTLTIP_H__1E222DA3_97EF_11D2_A18D_C32FFDBA4686__INCLUDED_)
#define AFX_WZDTLTIP_H__1E222DA3_97EF_11D2_A18D_C32FFDBA4686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzdTlTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWzdToolTipCtrl window

class CWzdToolTipCtrl : public CToolTipCtrl
{
// Construction
public:
	CWzdToolTipCtrl();

// Attributes
public:

// Operations
public:
	BOOL AddTool( UINT nID, LPCTSTR lpszText = LPSTR_TEXTCALLBACK  );
	BOOL AddTool( UINT nID, UINT nIDText  );
	BOOL AddArea( UINT nID, LPRECT lpRect, LPCTSTR lpszText = LPSTR_TEXTCALLBACK  );
	BOOL AddArea( UINT nID, LPRECT lpRect, UINT nIDText );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzdToolTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWzdToolTipCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWzdToolTipCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZDTLTIP_H__1E222DA3_97EF_11D2_A18D_C32FFDBA4686__INCLUDED_)
