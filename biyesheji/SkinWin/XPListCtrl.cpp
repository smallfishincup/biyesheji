// ColorListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "XPListCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPListCtrl
CXPListCtrl::CXPListCtrl():m_bSortAscending(TRUE),m_iSortColumn( -1 )
{
	m_bSortItem = FALSE;
}

CXPListCtrl::~CXPListCtrl()
{
}


BEGIN_MESSAGE_MAP(CXPListCtrl, CListCtrl)
//{{AFX_MSG_MAP(CXPListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPListCtrl message handlers

void CXPListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if(m_bSortItem)
	{
		for(int i=0;i<GetItemCount();i++)
		{
			SetItemData(i,i);
		}
		
		m_iSortColumn = pNMListView->iSubItem;
		if (m_iSortColumn != -1) 
		{
			VERIFY( SortItems( CompareFunction, reinterpret_cast<DWORD>( this ) ) );
		}
		
		
		m_bSortAscending = m_bSortAscending ? FALSE : TRUE;
	}	
	
	
	*pResult = 0;
}

int CALLBACK CXPListCtrl::CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CXPListCtrl* pListCtrl = reinterpret_cast<CXPListCtrl*>( lParamData );
	ASSERT( pListCtrl->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );
	
	CString    strItem1 = pListCtrl->GetItemText(lParam1, pListCtrl->m_iSortColumn);
	CString    strItem2 = pListCtrl->GetItemText(lParam2, pListCtrl->m_iSortColumn);
	
	if( pListCtrl->IsNumber( strItem1 ) )
		return pListCtrl->m_bSortAscending ? pListCtrl->NumberCompare( strItem1, strItem2 ) : pListCtrl->NumberCompare( strItem2, strItem1 );
	else if( pListCtrl->IsDate( strItem1 ) )
		return pListCtrl->m_bSortAscending ? pListCtrl->DateCompare( strItem1, strItem2 ) : pListCtrl->DateCompare( strItem2, strItem1 );
	else
		return pListCtrl->m_bSortAscending ? lstrcmp( strItem1, strItem2 ) : lstrcmp( strItem2, strItem1 );
	
	return 0;
}


bool CXPListCtrl::IsNumber(LPCTSTR pszText)
{
	CString strText;
	strText = pszText;

	if(strText.IsEmpty())
		return false;
	
	for( int i = 0; i < lstrlen( pszText ); i++ )
	{
		if( !_istdigit( pszText[ i ] ) )
		{
			return false;
		}
	}
	return true;
}

int CXPListCtrl::NumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2)
{
	const int iNumber1 = atoi( pszNumber1 );
	const int iNumber2 = atoi( pszNumber2 );
	
	if( iNumber1 < iNumber2 )
		return -1;
	
	if( iNumber1 > iNumber2 )
		return 1;
	
	
	return 0;
}

bool CXPListCtrl::IsDate(LPCTSTR pszText)
{
	if( lstrlen( pszText ) != 10 )
		return false;
	
	return _istdigit( pszText[ 0 ] )
		&& _istdigit( pszText[ 1 ] )
		&& pszText[ 2 ] == _T('-')
		&& _istdigit( pszText[ 3 ] )
		&& _istdigit( pszText[ 4 ] )
		&& pszText[ 5 ] == _T('-')
		&& _istdigit( pszText[ 6 ] )
		&& _istdigit( pszText[ 7 ] )
		&& _istdigit( pszText[ 8 ] )
		&& _istdigit( pszText[ 9 ] );
}

int CXPListCtrl::DateCompare(const CString &strDate1, const CString &strDate2)
{
	const int iYear1 = atoi( strDate1.Mid( 6, 4 ) );
	const int iYear2 = atoi( strDate2.Mid( 6, 4 ) );
	
	if( iYear1 < iYear2 )
		return -1;
	
	if( iYear1 > iYear2 )
		return 1;
	
	const int iMonth1 = atoi( strDate1.Mid( 3, 2 ) );
	const int iMonth2 = atoi( strDate2.Mid( 3, 2 ) );
	
	if( iMonth1 < iMonth2 )
		return -1;
	
	if( iMonth1 > iMonth2 )
		return 1;
	
	const int iDay1 = atoi( strDate1.Mid( 0, 2 ) );
	const int iDay2 = atoi( strDate2.Mid( 0, 2 ) );
	
	if( iDay1 < iDay2 )
		return -1;
	
	if( iDay1 > iDay2 )
		return 1;
	
	return 0;
}

LRESULT CXPListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_VSCROLL||message==WM_HSCROLL)
	{
		WORD sbCode=LOWORD(wParam);
		if(sbCode==SB_THUMBTRACK
			||sbCode==SB_THUMBPOSITION)
		{
			SCROLLINFO siv={0};
			siv.cbSize=sizeof(SCROLLINFO);
			siv.fMask=SIF_ALL;
			SCROLLINFO sih=siv;
			int nPos=HIWORD(wParam);
			CRect rcClient;
			GetClientRect(&rcClient);
			GetScrollInfo(SB_VERT,&siv);
			GetScrollInfo(SB_HORZ,&sih);
			SIZE sizeAll;
			if(sih.nPage==0) 
				sizeAll.cx=rcClient.right;
			else
				sizeAll.cx=rcClient.right*(sih.nMax+1)/sih.nPage ;
			if(siv.nPage==0)
				sizeAll.cy=rcClient.bottom;
			else
				sizeAll.cy=rcClient.bottom*(siv.nMax+1)/siv.nPage ;
			
			SIZE size={0,0};
			if(WM_VSCROLL==message)
			{
				size.cx=sizeAll.cx*sih.nPos/(sih.nMax+1);
				size.cy=sizeAll.cy*(nPos-siv.nPos)/(siv.nMax+1);
			}else
			{
				size.cx=sizeAll.cx*(nPos-sih.nPos)/(sih.nMax+1);
				size.cy=sizeAll.cy*siv.nPos/(siv.nMax+1);
			}
			Scroll(size);
			return 1;
		}
	}
	return CListCtrl::WindowProc(message, wParam, lParam);
}

BOOL CXPListCtrl::SetScrollBarBmp(LPCTSTR strScrollBarBmp)
{
	CBitmap bmp;
	HANDLE hBmp = ::LoadImage(NULL,strScrollBarBmp,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if (hBmp) //装入位图成功
	{
		bmp.DeleteObject();
		bmp.Attach(hBmp);
	}
	else
		return FALSE;
	m_hBmpScrollBar=(HBITMAP)bmp.Detach();
    SkinWndScroll(this, m_hBmpScrollBar);
	bmp.DeleteObject();

	return TRUE;
}

void CXPListCtrl::PreSubclassWindow() 
{
	m_ctlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
	CListCtrl::PreSubclassWindow();
}

