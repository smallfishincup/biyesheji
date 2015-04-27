#if !defined(AFX_XPTREECTRL_H__7FEEFAC4_4D88_41AF_B6B6_475B943886C7__INCLUDED_)
#define AFX_XPTREECTRL_H__7FEEFAC4_4D88_41AF_B6B6_475B943886C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXPTreeCtrl window
#include "SkinScrollBar.h"
#include "SkinScrollWnd.h"
class CXPTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CXPTreeCtrl();

// Attributes
public:
	HBITMAP	m_hBmpScrollBar	;

// Operations
public:
	BOOL SetScrollBarBmp(UINT bmpID);
	BOOL SetScrollBarBmp(LPCTSTR strScrollBarBmp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch=TRUE);
	virtual ~CXPTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXPTreeCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	UINT m_uFlags;
	void TravelSiblingAndParent(HTREEITEM hItem, int nState);
	void TravelChild(HTREEITEM hItem, int nState);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPTREECTRL_H__7FEEFAC4_4D88_41AF_B6B6_475B943886C7__INCLUDED_)
