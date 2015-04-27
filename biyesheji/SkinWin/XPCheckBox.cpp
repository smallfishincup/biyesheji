// XPCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "XPCheckBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPCheckBox

CXPCheckBox::CXPCheckBox()
{
	m_style = NOCHECKNORMAL;
	m_bOver = m_bTracking = m_bLBtnDown = FALSE;
}

CXPCheckBox::~CXPCheckBox()
{
	m_CheckBoxbmp.DeleteObject();
}


BEGIN_MESSAGE_MAP(CXPCheckBox, CButton)
	//{{AFX_MSG_MAP(CXPCheckBox)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPCheckBox message handlers
BOOL CXPCheckBox::SetCheckBoxBmp(UINT CheckBoxBmpID)
{
	if( !m_CheckBoxbmp.LoadBitmap(CheckBoxBmpID))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CXPCheckBox::SetCheckBoxBmp(LPCTSTR strCheckBoxBmp)
{
	HANDLE hBmp = ::LoadImage(NULL,strCheckBoxBmp,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if (hBmp) 
		m_CheckBoxbmp.Attach(hBmp);
	else
		return FALSE;

	m_bTracking = FALSE;
	return TRUE;
}

void CXPCheckBox::DrawCheckBoxBmp(UINT state)
{
	CRect wndRct;
	GetWindowRect(&wndRct);
	CDC *pDC = GetDC();
	CDC MDC;
	MDC.CreateCompatibleDC(pDC);
	CBitmap *oldbitmap = MDC.SelectObject(&m_CheckBoxbmp);
	int itop = ( wndRct.Height() - 13 )/2 ;

	//图片由8个小图片组成，根据不同的状态显示不同的图片
	int nXSrc,nYSrc;
	switch(state)
	{
	case NOCHECKNORMAL:  nXSrc = 0; nYSrc = 0; break;
	case NOCHECKOVER:    nXSrc = 13; nYSrc = 0; break;
	case NOCHECKDOWN:    nXSrc = 26; nYSrc = 0; break;
	case NOCHECKDISABLE: nXSrc = 39; nYSrc = 0; break;

	case CHECKNORMAL:    nXSrc = 0; nYSrc = 13; break;
	case CHECKOVER:      nXSrc = 13; nYSrc = 13; break;
	case CHECKDOWN:      nXSrc = 26; nYSrc = 13; break;
	case CHECKDISABLE:   nXSrc = 39; nYSrc = 13; break;
	}

	pDC->BitBlt(0,itop,13,13,&MDC,nXSrc,nYSrc,SRCCOPY);
	MDC.SelectObject(oldbitmap);
    ReleaseDC(pDC);
}

void CXPCheckBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CXPCheckBox::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL);
	return 0;
}

LRESULT CXPCheckBox::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}

void CXPCheckBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//*******************************************************************
	if( GetCheck() == BST_CHECKED)
	{
		if(GetStyle() & WS_DISABLED)
			m_style = CHECKDISABLE;
		else
		{
			if(m_bOver && !m_bLBtnDown)
			{
				m_style = CHECKOVER;
			}
			if(!m_bOver)
				m_style = CHECKNORMAL;
			if(m_bLBtnDown)
				m_style = CHECKDOWN;
		}
	}
	else
	{
		if(GetStyle() & WS_DISABLED)
			m_style = NOCHECKDISABLE;
		else
		{
			if(m_bOver && !m_bLBtnDown)
				m_style = NOCHECKOVER;
			if(!m_bOver)
				m_style = NOCHECKNORMAL;
			if(m_bLBtnDown)
				m_style = NOCHECKDOWN;
		}
	}
	//*******************************************************************

	DrawCheckBoxBmp(m_style);//根据状态绘制位图
	DrawText(&dc);
}

void CXPCheckBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = FALSE;
	CButton::OnLButtonUp(nFlags, point);
	InvalidateRect(NULL);
}

void CXPCheckBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);
	InvalidateRect(NULL);
}

void CXPCheckBox::DrawText(CDC *pDC)
{
	if(GetStyle() & WS_DISABLED)
		pDC->SetTextColor(RGB(161,161,146));
	CRect wndRct;
	GetWindowRect(&wndRct);
	ScreenToClient(&wndRct);
	wndRct.InflateRect(-13,0,0,0);
	CString str_text;
	GetWindowText(str_text);
	pDC->SetBkMode(TRANSPARENT);
	CFont *pFont = GetFont();
	CFont *oldfont = pDC->SelectObject(pFont);
	pDC->DrawText(str_text,&wndRct,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	pDC->SelectObject(oldfont);
}

LRESULT CXPCheckBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT result = CButton::WindowProc(message, wParam, lParam);

	//绘制位图，阻止控件自绘
	if(message == BM_SETCHECK)
	{
		//*************************************************
		if(wParam == BST_CHECKED)
		{
			if(GetStyle() & WS_DISABLED)
				m_style = CHECKDISABLE;
			else
				m_style = CHECKNORMAL;
		}
		else
		{
			if(GetStyle() & WS_DISABLED)
				m_style = NOCHECKDISABLE;
			else
				m_style = NOCHECKNORMAL;
		}
		//*************************************************
		
		DrawCheckBoxBmp(m_style);
	}
	
	if(message == BM_SETSTATE)
	{
		OnPaint();
	}
	if(message == WM_ENABLE)
	{
		Invalidate();
	}

	return result;
}