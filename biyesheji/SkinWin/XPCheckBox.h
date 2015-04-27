#if !defined(AFX_XPCHECKBOX_H__FD7B875A_E781_4855_996D_DF6802711C44__INCLUDED_)
#define AFX_XPCHECKBOX_H__FD7B875A_E781_4855_996D_DF6802711C44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPCheckBox.h : header file
//
#define NOCHECKNORMAL   0
#define NOCHECKOVER     1 
#define NOCHECKDOWN     2
#define CHECKNORMAL     3
#define CHECKOVER       4
#define CHECKDOWN       5
#define NOCHECKDISABLE  6
#define CHECKDISABLE    7
/////////////////////////////////////////////////////////////////////////////
// CXPCheckBox window

class CXPCheckBox : public CButton
{
// Construction
public:
	CXPCheckBox();

// Attributes
public:

// Operations
public:
	BOOL SetCheckBoxBmp(LPCTSTR strCheckBoxBmp);
	BOOL SetCheckBoxBmp(UINT CheckBoxBmpID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPCheckBox)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXPCheckBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXPCheckBox)
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
private:
	void DrawText(CDC *pDC);            
	void DrawCheckBoxBmp(UINT state);

private:
	CBitmap m_CheckBoxbmp;
	BOOL m_bOver, m_bTracking, m_bLBtnDown;
	UINT m_style;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPCHECKBOX_H__FD7B875A_E781_4855_996D_DF6802711C44__INCLUDED_)
