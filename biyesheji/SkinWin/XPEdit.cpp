// XPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "XPEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPEdit

CXPEdit::CXPEdit()
{
	m_pen[0].CreatePen(PS_SOLID,1,RGB(127,157,185));
	m_pen[1].CreatePen(PS_SOLID,1,RGB(202,200,187));
}

CXPEdit::~CXPEdit()
{
	for(int i=0;i<2;i++)
		m_pen[i].DeleteObject();
}


BEGIN_MESSAGE_MAP(CXPEdit, CEdit)
//{{AFX_MSG_MAP(CXPEdit)
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPEdit message handlers

void CXPEdit::OnNcPaint() 
{
	CRect wndRct;
	GetWindowRect(&wndRct);
	ScreenToClient(&wndRct);
	
	wndRct.InflateRect(-1,-1,-1,-1);
	CDC *pDC = GetDC();
	CPen *oldpen;
	if(GetStyle() & WS_DISABLED || GetStyle() & ES_READONLY)
		oldpen = pDC->SelectObject(&m_pen[1]);
	else
		oldpen = pDC->SelectObject(&m_pen[0]);
	
	pDC->Rectangle(&wndRct);
	pDC->SelectObject(oldpen);
	ReleaseDC(pDC);
	
}
