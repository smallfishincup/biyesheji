#if !defined(AFX_XPEDIT_H__C20BB97D_997E_4874_9275_4AA5609C4429__INCLUDED_)
#define AFX_XPEDIT_H__C20BB97D_997E_4874_9275_4AA5609C4429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXPEdit window

class CXPEdit : public CEdit
{
// Construction
public:
	CXPEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXPEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXPEdit)
	afx_msg void OnNcPaint();
	//}}AFX_MSG
private:
	CPen m_pen[2]; //»æÖÆNormal ºÍ Disable ×´Ì¬µÄ»­±Ê

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPEDIT_H__C20BB97D_997E_4874_9275_4AA5609C4429__INCLUDED_)
