#if !defined(AFX_COLORPROGRESS_H__63DD96A0_C952_4AF1_90CF_95FEC50637EF__INCLUDED_)
#define AFX_COLORPROGRESS_H__63DD96A0_C952_4AF1_90CF_95FEC50637EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorProgress window

class CColorProgress : public CStatic
{
// Construction
public:
	CColorProgress();
	virtual ~CColorProgress();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorProgress)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void Stop();
	void Start();
	BOOL SetProgressBmp(LPCTSTR pBmpName);
	BOOL SetProgressBmp(UINT nProgressID);

	// Generated message map functions
protected:
	void DealColorProgress();

	//Attributes
	CBitmap        m_bmpBk;
	BITMAP         m_bm;
	//{{AFX_MSG(CColorProgress)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPROGRESS_H__63DD96A0_C952_4AF1_90CF_95FEC50637EF__INCLUDED_)
