////////////////////////////////////////////////////////////////////////////
// SkinComboBox .cpp : implementation file
#include "stdafx.h"
#include "SkinComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SET_ZERO(X) memset((void *)&X,0,sizeof(X));

CSkinComboBox ::CSkinComboBox ()
{
	m_nIDLeft   = 0;
	m_nIDRight  = 0;
	m_nIDCenter = 0;

	m_Font.CreateFont(15,          // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,				   // nWeight FW_MEDIUM
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        _T("MS Sans Serif"));
}

CSkinComboBox ::~CSkinComboBox ()
{
}

BEGIN_MESSAGE_MAP(CSkinComboBox , CComboBox)
	//{{AFX_MSG_MAP(CSkinComboBox )
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinComboBox  message handlers
void CSkinComboBox ::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rc;
	CDC MemDC;
	
	GetClientRect(&rc);
	MemDC.CreateCompatibleDC(&dc);
	
	CBitmap bmpComboRight,bmpComboLeft, bmpComboCenter;
	BITMAP bitRight, bitLeft, bitCenter;
	SET_ZERO(bitRight); 
	SET_ZERO(bitLeft); 
	SET_ZERO(bitCenter);
	if( m_nIDLeft )
	{
		bmpComboLeft.LoadBitmap(m_nIDLeft);
		bmpComboLeft.GetBitmap(&bitLeft);
		MemDC.SelectObject(bmpComboLeft);
		dc.StretchBlt(rc.left,rc.top,bitLeft.bmWidth,rc.bottom - rc.top,&MemDC,0,0,bitLeft.bmWidth,bitLeft.bmHeight,SRCCOPY);
	}
	if( m_nIDRight )
	{
		bmpComboRight.LoadBitmap(m_nIDRight);
		bmpComboRight.GetBitmap(&bitRight);
		MemDC.SelectObject(bmpComboRight);
		dc.StretchBlt(rc.right-bitRight.bmWidth,rc.top,bitRight.bmWidth,rc.bottom - rc.top,&MemDC,0,0,bitRight.bmWidth,bitRight.bmHeight,SRCCOPY);
	}

	if( m_nIDCenter )
	{
		bmpComboCenter.LoadBitmap(m_nIDCenter);
		bmpComboCenter.GetBitmap(&bitCenter);
		MemDC.SelectObject(bmpComboCenter);
		
		dc.StretchBlt(rc.left + bitLeft.bmWidth,rc.top,rc.right - bitLeft.bmWidth - bitRight.bmWidth,
			rc.bottom - rc.top,&MemDC,0,0,bitCenter.bmWidth,bitCenter.bmHeight,SRCCOPY);
	}
	
	CString strText;
	GetWindowText(strText);

	RECT rcRepaint;
	GetClientRect(&rcRepaint);
	rcRepaint.left    = rcRepaint.left  + bitLeft.bmWidth + 2;
	rcRepaint.right   = rcRepaint.right - bitRight.bmWidth;
	rcRepaint.top    += 2; 
	rcRepaint.bottom -= 2;
	
	BOOL bEnable = IsWindowEnabled();
	COLORREF clrBack = bEnable ? RGB(251, 250, 251) : RGB(212,208,200);
	dc.SetBkColor(clrBack);
	
	CRect rcFill;
	GetClientRect(&rcFill);
	rcFill.left   = rcFill.left   + bitLeft.bmWidth;
	rcFill.right  = rcFill.right  - bitRight.bmWidth;	
	rcFill.top    = rcFill.top    + 2;
	rcFill.bottom = rcFill.bottom - 2;

	dc.FillSolidRect(&rcFill,clrBack);

	dc.SelectObject(&m_Font);
	dc.DrawText(strText	,&rcRepaint,DT_VCENTER | DT_SINGLELINE);

	// Do not call CComboBox::OnPaint() for painting messages
}

BOOL CSkinComboBox ::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Remove the CBS_SIMPLE and CBS_DROPDOWN styles and add the one I'm designed for
	dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST;

	// Make sure to use the CBS_OWNERDRAWVARIABLE style
	dwStyle |= CBS_OWNERDRAWVARIABLE;

	// Use default strings. We need the itemdata to store the state of the lamps
	dwStyle |= CBS_HASSTRINGS;	

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CSkinComboBox ::SetComboBitmap(UINT nIDLeft, UINT nIDRight, UINT nIDCenter)
{
	m_nIDCenter = nIDCenter; 
	m_nIDLeft   = nIDLeft; 
	m_nIDRight  = nIDRight;
}
