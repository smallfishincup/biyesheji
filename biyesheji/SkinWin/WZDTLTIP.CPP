// WzdTlTip.cpp : implementation file
//

#include "stdafx.h"
#include "WzdTlTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzdToolTipCtrl

CWzdToolTipCtrl::CWzdToolTipCtrl()
{
}

CWzdToolTipCtrl::~CWzdToolTipCtrl()
{
}


BEGIN_MESSAGE_MAP(CWzdToolTipCtrl, CToolTipCtrl)
	//{{AFX_MSG_MAP(CWzdToolTipCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzdToolTipCtrl message handlers

BOOL CWzdToolTipCtrl::AddTool( UINT nID, LPCTSTR lpszText  )
{
	CWnd* pOwner  = GetOwner();
	CWnd* pParent = GetParent();
	ASSERT(pOwner);
	ASSERT(pParent);

	TOOLINFO ti;
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize	= sizeof(TOOLINFO);
	ti.hwnd		= pOwner->GetSafeHwnd();
	ti.uFlags	= TTF_IDISHWND|TTF_SUBCLASS;
	ti.uId		= (UINT)pParent->GetDlgItem(nID)->m_hWnd;
	ti.lpszText = (LPSTR)lpszText;
	
	return (BOOL)SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL CWzdToolTipCtrl::AddTool( UINT nID, UINT nIDText )
{
	CString str;
	VERIFY(str.LoadString(nIDText));
	return AddTool(nID,str);
}

BOOL CWzdToolTipCtrl::AddArea( UINT nID, LPRECT lpRect, LPCTSTR lpszText  )
{
	TOOLINFO ti;
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize	= sizeof(TOOLINFO);
	ti.hwnd		= GetOwner()->GetSafeHwnd();
	ti.uFlags	= TTF_SUBCLASS;
	ti.uId		= nID;
	memcpy(&ti.rect, lpRect, sizeof(RECT));
	ti.lpszText = (LPSTR)lpszText;

	return (BOOL)SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL CWzdToolTipCtrl::AddArea( UINT nID, LPRECT lpRect, UINT nIDText )
{
	CString str;
	VERIFY(str.LoadString(nIDText));
	return AddArea(nID,lpRect,str);
}
