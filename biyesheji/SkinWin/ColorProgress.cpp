// ColorProgress.cpp : implementation file
//

#include "stdafx.h"
#include "ColorProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorProgress
#define IDT_STARTPROGRESS    1
CColorProgress::CColorProgress()
{
}

CColorProgress::~CColorProgress()
{
	if(m_bmpBk.GetSafeHandle())
		m_bmpBk.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorProgress, CStatic)
	//{{AFX_MSG_MAP(CColorProgress)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorProgress message handlers

BOOL CColorProgress::SetProgressBmp(LPCTSTR pBmpName)
{
	BOOL bRes = FALSE;
	HANDLE hBmp = ::LoadImage(NULL,pBmpName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if (hBmp) //装入位图成功
	{
		if(m_bmpBk.GetSafeHandle())
			m_bmpBk.DeleteObject();

		m_bmpBk.Attach(hBmp);

		m_bmpBk.GetBitmap(&m_bm);
		SetWindowPos(NULL, 0, 0, m_bm.bmWidth, m_bm.bmHeight, SWP_NOMOVE | SWP_SHOWWINDOW);
		
		bRes = TRUE;
	}	
	
	return bRes;
}

BOOL CColorProgress::SetProgressBmp(UINT nProgressID)
{
	if(m_bmpBk.GetSafeHandle())
		m_bmpBk.DeleteObject();

	m_bmpBk.LoadBitmap(nProgressID);
	if(!m_bmpBk.GetSafeHandle())
		return FALSE;

	m_bmpBk.GetBitmap(&m_bm);

	SetWindowPos(NULL, 0, 0, m_bm.bmWidth, m_bm.bmHeight, SWP_NOMOVE | SWP_SHOWWINDOW);
	
	return TRUE;
}

void CColorProgress::Start()
{
	SetTimer(IDT_STARTPROGRESS, 50, NULL);
}

void CColorProgress::Stop()
{
	KillTimer(IDT_STARTPROGRESS);
}

void CColorProgress::OnTimer(UINT nIDEvent) 
{
	DealColorProgress();
	CStatic::OnTimer(nIDEvent);
}

void CColorProgress::DealColorProgress()
{
	int nMarge = 15;

	static int nPos = nMarge;
	if(nPos > m_bm.bmWidth - (nMarge * 2) )
		nPos = nMarge;
	
	CDC *pDC = GetDC();
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap *oldbmp = MemDC.SelectObject(&m_bmpBk);

	pDC->BitBlt(0, 0, nMarge, m_bm.bmHeight, &MemDC, 0, 0, SRCCOPY);
	pDC->BitBlt(m_bm.bmWidth - nMarge, 0, nMarge, m_bm.bmHeight, &MemDC, m_bm.bmWidth - nMarge, 0, SRCCOPY);

    pDC->BitBlt(nPos, 0, m_bm.bmWidth - nMarge - nPos, m_bm.bmHeight, &MemDC, nMarge, 0, SRCCOPY);
	pDC->BitBlt(nMarge, 0, nPos, m_bm.bmHeight, &MemDC, m_bm.bmWidth - nMarge - nPos, 0, SRCCOPY);

	MemDC.SelectObject(oldbmp);
	ReleaseDC(pDC);

	nPos += 2;
}

void CColorProgress::OnDestroy() 
{
	KillTimer(IDT_STARTPROGRESS);
	CStatic::OnDestroy();	
}


void CColorProgress::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}
