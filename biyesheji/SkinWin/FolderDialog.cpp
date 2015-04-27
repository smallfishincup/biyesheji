// FolderDialog.cpp: implementation of the CFolderDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int CALLBACK BrowseDirectoryCallBack(
		HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	CFolderDialog * pFd = (CFolderDialog*) lpData;
	pFd->CallBackFun( hWnd, uMsg, lParam );
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFolderDialog::CFolderDialog( LPCTSTR lpszFolderName,
					DWORD dwFlags, CWnd* pParentWnd,
					LPCTSTR szCaption,LPCTSTR strTitle)
{

	if( lpszFolderName == NULL )
		m_szInitFolderName = _T("");
	else
		m_szInitFolderName = lpszFolderName;

	if(szCaption == NULL)
	{
	   	lstrcpy(m_szCaption, _T("Folder"));	
	}
	else
	{
		lstrcpy(m_szCaption, szCaption);
	}

	memset( &m_bi, '\0', sizeof(BROWSEINFO) );

	if( pParentWnd==NULL )
		m_bi.hwndOwner=0;
	else
		m_bi.hwndOwner=pParentWnd->m_hWnd;

	m_bi.pidlRoot = NULL;
	m_bi.pszDisplayName = m_szDisplayName;
	if (strTitle == NULL)
	{
		m_bi.lpszTitle = _T("Local");
	}
	else
	{
		m_bi.lpszTitle = strTitle;
	}
	m_bi.ulFlags = dwFlags|BIF_STATUSTEXT;
	m_bi.lpfn    = BrowseDirectoryCallBack;
	m_bi.lParam  = (LPARAM)this;
}
 
CFolderDialog::~CFolderDialog()
{

}

void CFolderDialog::CallBackFun(HWND hWnd, UINT uMsg, LPARAM lParam)
{
	m_hDialogBox = hWnd;
	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		OnInitDialog();
		::SetWindowText(m_hDialogBox, m_szCaption);
		break;
	case BFFM_SELCHANGED:
		OnSelChanged( (ITEMIDLIST*)lParam );
		break;
	}
}

void CFolderDialog::OnInitDialog()
{
	SetSelection(m_szInitFolderName);
	SetStatusText( ShortName(m_szInitFolderName) );
}

void CFolderDialog::OnSelChanged(ITEMIDLIST *pIdl)
{
	::SHGetPathFromIDList( pIdl, m_szPath );
	m_szFinalFolderName = m_szPath;
	SetStatusText( ShortName(m_szFinalFolderName) );
}

int CFolderDialog::DoModal()
{
	int nReturn = IDOK;

	m_szFinalFolderName = m_szInitFolderName;
	ITEMIDLIST * piid = NULL;

	piid = ::SHBrowseForFolder( &m_bi );
	if( piid && ::SHGetPathFromIDList(piid,m_szPath) )
	{
		m_szFinalFolderName = m_szPath;
		nReturn = IDOK;
	}
	else
	{
		nReturn = IDCANCEL;
	}

	if( piid )
	{
		LPMALLOC lpMalloc;
		VERIFY( ::SHGetMalloc(&lpMalloc)==NOERROR );
		lpMalloc->Free(piid);
		lpMalloc->Release();
	}
	return nReturn;
}

void CFolderDialog::EnableOK(BOOL bEnable)
{
	::SendMessage( m_hDialogBox, BFFM_ENABLEOK, 0, (bEnable?1:0) );
}

void CFolderDialog::SetSelection(LPCTSTR pszSelection)
{
	::SendMessage( m_hDialogBox, BFFM_SETSELECTION, TRUE, (LPARAM)pszSelection );
}

void CFolderDialog::SetSelection(ITEMIDLIST *pIdl)
{
	::SendMessage( m_hDialogBox, BFFM_SETSELECTION, FALSE, (LPARAM)pIdl );
}

void CFolderDialog::SetStatusText(LPCTSTR pszStatusText)
{
	::SendMessage( m_hDialogBox, BFFM_SETSTATUSTEXT, 0, (LPARAM)pszStatusText );
}

CString CFolderDialog::ShortName(const CString &szName)
{
	CString szShort;
	if( szName.GetLength()<=35 )
		szShort=szName;
	else
		szShort=szName.Left(35)+_T("...");
	return szShort;
}

CString CFolderDialog::GetPathName()
{
	return m_szFinalFolderName;
}
