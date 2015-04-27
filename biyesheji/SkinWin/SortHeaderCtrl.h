#if !defined(AFX_SORTHEADERCTRL_H__672D6911_B63B_4F4D_AF5F_570DD7E83916__INCLUDED_)
#define AFX_SORTHEADERCTRL_H__672D6911_B63B_4F4D_AF5F_570DD7E83916__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SortHeaderCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl window

class CSortHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSortHeaderCtrl();

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
private:
	void DeawText(CDC *pDC);
	void DrawBkgnd(CDC *pDC);
	
private:
	HD_ITEM hditem;
	BOOL m_bSelected;	//按钮被按下该值为true
	
	UINT HeaderItemStyle;//设置HeaderItem的外形
	CFont m_Font;
	CBrush m_VerLineBrush, m_VerLineBrush2;
	CBrush m_HorLineBrush[9];
	CBrush m_MouseOverbk, m_Normalbk, m_MouseDownbk;
	
	int nItemMouseOver, nItemMouseDown;
	BOOL m_bTracking;
	CFont m_NewHeaderFont;
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTHEADERCTRL_H__672D6911_B63B_4F4D_AF5F_570DD7E83916__INCLUDED_)
