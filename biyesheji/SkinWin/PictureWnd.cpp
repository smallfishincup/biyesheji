// PictureWnd.cpp
//

#include "stdafx.h"
#include "PictureWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPictureWnd, CWnd)

CPictureWnd::CPictureWnd()
{
}

CPictureWnd::~CPictureWnd()
{
}

BEGIN_MESSAGE_MAP(CPictureWnd, CWnd)
	//{{AFX_MSG_MAP(CPictureWnd)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureWnd message handlers
void CPictureWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nSBCode != SB_ENDSCROLL)	
	{
		SCROLLINFO vStructure;
		GetScrollInfo(SB_VERT, &vStructure);
		vStructure.fMask	= SIF_ALL;
		vStructure.nPos		= nPos;
		SetScrollInfo(SB_VERT, &vStructure);
	}

	Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPictureWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nSBCode != SB_ENDSCROLL)	
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_HORZ, &hStructure);
		hStructure.fMask	= SIF_ALL;
		hStructure.nPos		= nPos;
		SetScrollInfo(SB_HORZ, &hStructure);
	}
	
	Invalidate();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPictureWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	CRect rc;
	GetClientRect(&rc);	
	if( m_hBitmap.GetSafeHandle())
	{
		CDC hdc ;
		hdc.CreateCompatibleDC(&dc);

		CBitmap* hOld = hdc.SelectObject(&m_hBitmap); 

		BITMAP              bitmap ;
		m_hBitmap.GetBitmap(&bitmap);

		SCROLLINFO hStructure, vStructure;
		GetScrollInfo(SB_HORZ, &hStructure);
		GetScrollInfo(SB_VERT, &vStructure);

		DWORD dwX = (int)bitmap.bmWidth - rc.right;

		if(dwX > (DWORD)hStructure.nPos)
			dwX = (DWORD)hStructure.nPos;
		DWORD dwY = (DWORD)bitmap.bmHeight - rc.bottom;
		if(dwY > (DWORD)vStructure.nPos)
			dwY = (DWORD)vStructure.nPos;	
		
		UINT32 nX;
		if( bitmap.bmWidth > rc.Width() )
			nX = 0;
		else
			nX = ( rc.Width() - bitmap.bmWidth) / 2;

		UINT32 nY;
		if( bitmap.bmHeight > rc.Height() )
			nY = 0;
		else
			nY = (rc.Height() - rc.Height()) / 2;

		m_rcPicturehWnd.left	= dwX;
		m_rcPicturehWnd.top		= dwY;
		m_rcPicturehWnd.right	= dwX + ( rc.Width() - dwX * 2 );
		m_rcPicturehWnd.bottom	= dwY + ( rc.Height() - dwY * 2 );

		m_rcPicture.left		= nX;
		m_rcPicture.top			= nY;
		m_rcPicture.right		= nX + ( rc.Width() - dwX * 2 );
		m_rcPicture.bottom		= nY + ( rc.Height() - dwY * 2 );

		dc.BitBlt(nX, nY, rc.Width(), rc.Height(), &hdc, dwX, dwY, SRCCOPY);
		dc.SelectObject(hOld);

		hStructure.fMask		= SIF_ALL;
		hStructure.nMin			= 0;
		hStructure.nMax			= bitmap.bmWidth ;
		hStructure.nPage		= rc.right * rc.right / hStructure.nMax ;
		SetScrollInfo(SB_HORZ, &hStructure);

		vStructure.fMask		= SIF_ALL;
		vStructure.nMin			= 0;
		vStructure.nMax			= bitmap.bmHeight ;
		vStructure.nPage		= rc.bottom * rc.bottom / vStructure.nMax;
		SetScrollInfo(SB_VERT, &vStructure);
	}

	dc.Draw3dRect(rc,RGB(255,128,64),RGB(255,128,64));
}

RECT CPictureWnd::GetPictureHwndRect()
{
	return m_rcPicturehWnd;
}

BOOL CPictureWnd::SetBitmap(HBITMAP hBitmap)
{
	CheckPointer(hBitmap,FALSE);
	
	if( m_hBitmap.GetSafeHandle() )
		m_hBitmap.DeleteObject();
	
	BOOL bRes = m_hBitmap.Attach(hBitmap);
	
	Invalidate();
	
	return bRes;
}

BOOL CPictureWnd::GetPicPoint(POINT pt, POINT* pOut)
{
	if( !m_rcPicturehWnd.PtInRect(pt) )
		return FALSE;

	pOut->x = ( pt.x - m_rcPicturehWnd.left ) + m_rcPicture.left;
	pOut->y = ( pt.y - m_rcPicturehWnd.top ) + m_rcPicture.top;
	
	return TRUE;
}
