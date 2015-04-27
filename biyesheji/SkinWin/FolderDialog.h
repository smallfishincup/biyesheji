// FolderDialog.h: interface for the CFolderDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOLDERDIALOG_H__899A86E2_4103_11D4_B8DD_0050BA030A6E__INCLUDED_)
#define AFX_FOLDERDIALOG_H__899A86E2_4103_11D4_B8DD_0050BA030A6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
class CFolderDialog  
{
public:
	CFolderDialog(LPCTSTR lpszFolderName = NULL, DWORD dwFlags = NULL, CWnd* pParentWnd = NULL,
		LPCTSTR szCaption = NULL,LPCTSTR strTitle = NULL );
	virtual ~CFolderDialog();

	CString GetPathName();
	virtual int DoModal();
	BROWSEINFO m_bi;

	friend static int CALLBACK BrowseDirectoryCallBack(
		HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData );

protected:
	CString ShortName(const CString &szName);
	void SetStatusText(LPCTSTR pszStatusText);
	void SetSelection(ITEMIDLIST * pIdl);
	void SetSelection(LPCTSTR pszSelection);
	void EnableOK(BOOL bEnable);
	virtual void OnSelChanged( ITEMIDLIST * pIdl );
	virtual void OnInitDialog();
	virtual void CallBackFun(HWND hWnd, UINT uMsg, LPARAM lParam);
	CString m_szInitFolderName;
	CString m_szFinalFolderName;
	TCHAR m_szDisplayName[_MAX_PATH];
	TCHAR m_szPath[_MAX_PATH];
	HWND m_hDialogBox;
	TCHAR m_szCaption[80];
};

#endif // !defined(AFX_FOLDERDIALOG_H__899A86E2_4103_11D4_B8DD_0050BA030A6E__INCLUDED_)
