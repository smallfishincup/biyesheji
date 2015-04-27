// SortHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SortHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

CSortHeaderCtrl::CSortHeaderCtrl()
{
	m_VerLineBrush.CreateSolidBrush(RGB(199, 197, 178));
	m_VerLineBrush2.CreateSolidBrush(RGB(255, 255, 255));
	m_HorLineBrush[0].CreateSolidBrush(RGB(226, 222, 205));
	m_HorLineBrush[1].CreateSolidBrush(RGB(214, 210, 194));
	m_HorLineBrush[2].CreateSolidBrush(RGB(203, 199, 184));
	
	m_HorLineBrush[3].CreateSolidBrush(RGB(249, 169, 0));
	m_HorLineBrush[4].CreateSolidBrush(RGB(246, 196, 86));
	m_HorLineBrush[5].CreateSolidBrush(RGB(249, 177, 25));

	m_HorLineBrush[6].CreateSolidBrush(RGB(165, 165, 151));
	m_HorLineBrush[7].CreateSolidBrush(RGB(193,194,184));
	m_HorLineBrush[8].CreateSolidBrush(RGB(208,209,201));

	m_MouseOverbk.CreateSolidBrush(RGB(250, 249, 244));
	m_Normalbk.CreateSolidBrush(RGB(235, 234, 219));
	m_MouseDownbk.CreateSolidBrush(RGB(222, 223, 216));
	
	m_Font.CreatePointFont(90,"ËÎÌå");
	m_NewHeaderFont.CreatePointFont(110,"MS Serif");
    m_bSelected = m_bTracking = FALSE;
	nItemMouseOver = -1;
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
	m_Font.DeleteObject();
	m_VerLineBrush.DeleteObject();
	m_VerLineBrush2.DeleteObject();
	m_MouseOverbk.DeleteObject();
	m_Normalbk.DeleteObject();
	m_MouseDownbk.DeleteObject();
	m_NewHeaderFont.DeleteObject();
	for(int i=0;i<9;i++)
		m_HorLineBrush[i].DeleteObject();
}


BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
//{{AFX_MSG_MAP(CSortHeaderCtrl)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl message handlers
void CSortHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawBkgnd(&dc);
	DeawText(&dc);
	// Do not call CHeaderCtrl::OnPaint() for painting messages
}



void CSortHeaderCtrl::DrawBkgnd(CDC *pDC)
{
	CRect ItemRct;
	CRect wndRct;
	GetWindowRect(&wndRct);
	ScreenToClient(&wndRct);
	pDC->FillRect(wndRct, &m_Normalbk);
	
	pDC->FillRect(CRect(wndRct.left,wndRct.bottom-3,wndRct.right,wndRct.bottom-2),
		&m_HorLineBrush[0]);
	pDC->FillRect(CRect(wndRct.left,wndRct.bottom-2,wndRct.right,wndRct.bottom-1),
		&m_HorLineBrush[1]);	
	pDC->FillRect(CRect(wndRct.left,wndRct.bottom-1,wndRct.right,wndRct.bottom),
		&m_HorLineBrush[2]);
	
	for(int i=0; i<GetItemCount(); i++)
	{
		GetItemRect(i,&ItemRct);
		pDC->FillRect(CRect(ItemRct.right-2,ItemRct.top+2,ItemRct.right-1,ItemRct.bottom-5),
			&m_VerLineBrush);
		pDC->FillRect(CRect(ItemRct.right-1,ItemRct.top+2,ItemRct.right,ItemRct.bottom-5),
			&m_VerLineBrush2);
		
		if(i == nItemMouseOver)
		{
			if(!m_bSelected)
			{
				pDC->FillRect(&ItemRct, &m_MouseOverbk);
				pDC->FillRect(CRect(ItemRct.left,ItemRct.bottom-3,ItemRct.right,ItemRct.bottom-2),
					&m_HorLineBrush[3]);
				pDC->FillRect(CRect(ItemRct.left+1,ItemRct.bottom-2,ItemRct.right-1,ItemRct.bottom-1),
					&m_HorLineBrush[4]);	
				pDC->FillRect(CRect(ItemRct.left+2,ItemRct.bottom-1,ItemRct.right-2,ItemRct.bottom),
					&m_HorLineBrush[5]);
			}
			else
			{
				pDC->FillRect(&ItemRct, &m_MouseDownbk);
				pDC->FillRect(CRect(ItemRct.left,ItemRct.bottom-1,ItemRct.right,ItemRct.bottom),
					&m_HorLineBrush[6]);
				pDC->FillRect(CRect(ItemRct.left,ItemRct.top,ItemRct.left+1,ItemRct.bottom-1),
					&m_HorLineBrush[6]);
				pDC->FillRect(CRect(ItemRct.right-1,ItemRct.top,ItemRct.right,ItemRct.bottom-1),
					&m_HorLineBrush[6]);
				pDC->FillRect(CRect(ItemRct.left+1,ItemRct.top,ItemRct.right-1,ItemRct.top+1),
					&m_HorLineBrush[7]);
				pDC->FillRect(CRect(ItemRct.left+1,ItemRct.top+1,ItemRct.left+2,ItemRct.bottom-1),
					&m_HorLineBrush[7]);
				pDC->FillRect(CRect(ItemRct.left+2,ItemRct.top+1,ItemRct.right-1,ItemRct.top+2),
					&m_HorLineBrush[8]);
				pDC->FillRect(CRect(ItemRct.left+2,ItemRct.top+2,ItemRct.left+3,ItemRct.bottom-1),
					&m_HorLineBrush[8]);
			}
		}
	}	
}

void CSortHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect ItemRct;
	for(int i=0; i<GetItemCount(); i++)
	{
		GetItemRect(i,&ItemRct);
		if(i == 0)
			ItemRct.InflateRect(0,0,-10,0);
		else
			ItemRct.InflateRect(-10,0,-10,0);
		if(ItemRct.PtInRect(point))
		{
			nItemMouseDown = i;
			m_bSelected = TRUE;
		}
	}
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CSortHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	nItemMouseDown = -1;
	m_bSelected = FALSE;
	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CSortHeaderCtrl::DeawText(CDC *pDC)
{
	//»æÖÆÎÄ×Ö
	CRect ItemRct;
	CFont* hOldFont =pDC->SelectObject(&m_Font);
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	TCHAR szText[ 256 ];	
	hditem.pszText = szText;
	hditem.cchTextMax = 255;
	pDC->SetBkMode(TRANSPARENT);
	for(int i=0;i<GetItemCount(); i++)
	{
		VERIFY(GetItem(i, &hditem ));
		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS ;	
		if( hditem.fmt & HDF_CENTER)
		{
			uFormat |= DT_CENTER;
		}
		else if( hditem.fmt & HDF_RIGHT)
			uFormat |= DT_RIGHT;
		else
			uFormat |= DT_LEFT;	
		GetItemRect(i,&ItemRct);
		ItemRct.InflateRect(-2,-2,-3,-2);	
		ItemRct.InflateRect(-2,0,0,0);
		if((i == nItemMouseDown) && (m_bSelected))
		{
			ItemRct.InflateRect(-1,-2,0,0);
		}
		if(ItemRct.Width()>0)
		{
			pDC->DrawText(szText, -1, ItemRct, uFormat );
		}
	}
	pDC->SelectObject(hOldFont);
}

void CSortHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	
	CRect ItemRct;
	int nPreItemMouseOver = nItemMouseOver;
	for(int i=0; i<GetItemCount(); i++)
	{
		GetItemRect(i,&ItemRct);
		if(i == 0)
			ItemRct.InflateRect(0,0,10,0);
		else
			ItemRct.InflateRect(-10,0,10,0);
		
		if( ItemRct.PtInRect(point) )
		{
			nItemMouseOver = i;
			if(nPreItemMouseOver != nItemMouseOver)
				Invalidate(NULL);
			break;
		}
		GetItemRect( (GetItemCount()-1 ),&ItemRct);
		if(point.x > ItemRct.right)
		{
			nItemMouseOver = -1;
			if(nPreItemMouseOver != nItemMouseOver)
				Invalidate(NULL);
			nPreItemMouseOver = nItemMouseOver;
		}
	}
	
	CHeaderCtrl::OnMouseMove(nFlags, point);
}

LRESULT CSortHeaderCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	nItemMouseOver = -1;
	Invalidate(NULL);
	return 0;
}

LRESULT CSortHeaderCtrl::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CSortHeaderCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetFont(&m_NewHeaderFont);
	CHeaderCtrl::PreSubclassWindow();
}
