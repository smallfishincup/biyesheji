#if !defined(AFX_XPLISTCTRL_H__AB3C1358_23DB_4B53_930C_AFC6C48EA200__INCLUDED_)
#define AFX_XPLISTCTRL_H__AB3C1358_23DB_4B53_930C_AFC6C48EA200__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "SortHeaderCtrl.h"
#include "SkinScrollBar.h"
#include "SkinScrollWnd.h"

//#pragma warning (disable :4786)
/////////////////////////////////////////////////////////////////////////////
// CXPListCtrl window

class CXPListCtrl : public CListCtrl
{
	// Construction
public:
	CXPListCtrl();
	
	// Attributes
public:
	BOOL m_bSortItem;//设置是否有排序的功能
	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPListCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	BOOL	SetScrollBarBmp(LPCTSTR strScrollBarBmp);
	void	SetSortStyle(BOOL bSort){m_bSortItem = bSort;};
	virtual ~CXPListCtrl();
	
	// Generated message map functions
protected:
	static int CALLBACK CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );

	//{{AFX_MSG(CXPListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
private:
	int DateCompare( const CString& strDate1, const CString& strDate2 );
	bool IsDate(LPCTSTR pszText );
	int NumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2);
	bool IsNumber(LPCTSTR pszText);
	
private:
	HBITMAP	m_hBmpScrollBar	;
	int m_iSortColumn;
	BOOL m_bSortAscending;
	CSortHeaderCtrl m_ctlHeader;
	
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPLISTCTRL_H__AB3C1358_23DB_4B53_930C_AFC6C48EA200__INCLUDED_)
