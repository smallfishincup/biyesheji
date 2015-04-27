#ifndef _PIC_WND_H
#define _PIC_WND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPictureWnd : public CWnd
{
public:
	CPictureWnd();
	virtual ~CPictureWnd();

	DECLARE_DYNAMIC(CPictureWnd)

public:
	BOOL	SetBitmap(HBITMAP hBitmap);
	BOOL	GetPicPoint(POINT pt, POINT* pOut);

protected:
	RECT	GetPictureHwndRect();
	RECT	GetPictureRect();

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPictureWnd)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CRect		m_rcPicturehWnd;
	CRect		m_rcPicture;
	
	COLORREF	m_Textcr;	

	CBitmap		m_hBitmap;
};

#endif