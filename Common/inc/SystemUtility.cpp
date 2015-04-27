// SystemUtility.cpp: implementation of the CSystemUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemUtility.h"
#include "TLHELP32.H"
#include "Winsvc.h"
#include "ValidFunction.h"
#include "Psapi.h"
#include "dt.h"
#include "UtilityMacro.h"

#pragma comment(lib,"Psapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemUtility::CSystemUtility()
{

}

CSystemUtility::~CSystemUtility()
{

}

// 获取系统调试模式权限
BOOL CSystemUtility::EnableDebugPriv()
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
        CloseHandle(hToken);
        return FALSE;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid			= sedebugnameValue;
    tkp.Privileges[0].Attributes	= SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
        CloseHandle(hToken);
        return FALSE;
    }

    return TRUE;
}

// 强制性
//uFlags = EWX_LOGOFF			注销
// uFlags = EWX_POWEROFF		关闭电源
// uFlags = EWX_REBOOT			重启计算机
// uFlags = EWX_SHUTDOWN		关闭计算机
BOOL CSystemUtility::ForceExitWindow(UINT uFlags)
{
	HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 

    // Get a token for this process. 
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
        return FALSE;
	
    // Get the LUID for the shutdown privilege. 
    if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid))
		return FALSE;
	
    tkp.PrivilegeCount				= 1;  // one privilege to set    
    tkp.Privileges[0].Attributes	= SE_PRIVILEGE_ENABLED; 
	
    // Get the shutdown privilege for this process. 
    if(!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
		return FALSE;
	
    // Cannot test the return value of AdjustTokenPrivileges. 
    if (GetLastError() != ERROR_SUCCESS) 
        return FALSE;
	
    // Shut down the system and force all applications to close. 
    if (!ExitWindowsEx(uFlags | EWX_FORCE, 0)) 
        return FALSE;

	return TRUE;
}

// 在屏幕上输出文字			(!!!有问题函数,当多次写屏时,无法刷新前一次文字)
BOOL CSystemUtility::DrawScreenText(CString strText,UINT nFontHeight,COLORREF clrFont)
{
	HDC HScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(HScreenDC == NULL)
		return FALSE;
	
	int nScreenX = GetDeviceCaps(HScreenDC, HORZRES);
	int nScreenY = GetDeviceCaps(HScreenDC, VERTRES);
	
	CRect rcRect(0,nScreenY / 2,nScreenX,nScreenY);
	
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));		// zero out structure
	lf.lfWeight		= FW_BOLD;
	lf.lfHeight		= nFontHeight;          // request a 12-pixel-height font
	strcpy_s(lf.lfFaceName, "Arial");			// request a face name "Arial"
	if(!font.CreateFontIndirect(&lf))		// create the font
		return FALSE;
	
	HFONT oldfont	= (HFONT)SelectObject(HScreenDC,font);
	
	SetTextColor(HScreenDC,clrFont);
	SetBkMode(HScreenDC,TRANSPARENT);
	
	DrawText(HScreenDC,strText,strText.GetLength(),&rcRect,DT_CENTER);
	
	SelectObject(HScreenDC,oldfont);
	DeleteObject(font);
	DeleteDC(HScreenDC);
	
	return TRUE;
}

// 运行本地文件
BOOL CSystemUtility::LocalFileExcute(CString strFileName,BOOL bNormal)
{
	if(!CValidFunction::IsFileExist(strFileName))
		return FALSE;

	HINSTANCE  hInst = ShellExecute(NULL, "open", strFileName, NULL,NULL, bNormal ? SW_SHOW : SW_HIDE);
	return ((int)hInst > 32);
}

// 删除本地目录或文件
BOOL CSystemUtility::LocalFileDelete(CString strFileName,BOOL bDir)
{
	if(bDir)
		return DeleteDirectory(strFileName.GetBuffer(MAX_PATH));
	else
		return DeleteFile(strFileName);

	return FALSE;
}

BOOL CSystemUtility::DeleteDirectory(char *DirName)
{
    CFileFind tempFind;
    char tempFileFind[MAX_PATH];
    sprintf_s(tempFileFind,"%s\\*.*",DirName);
    BOOL IsFinded = (BOOL)tempFind.FindFile(tempFileFind);
    while(IsFinded)
	{
		IsFinded = (BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			char foundFileName[MAX_PATH];
			strcpy_s(foundFileName,tempFind.GetFileName().GetBuffer(200));
			if(tempFind.IsDirectory())
			{
				char tempDir[MAX_PATH];
				sprintf_s(tempDir,"%s\\%s",DirName,foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				char tempFileName[MAX_PATH];
				sprintf_s(tempFileName,"%s\\%s",DirName,foundFileName);
				DeleteFile(tempFileName);
			}
		}
	}
	tempFind.Close();
	if(!RemoveDirectory(DirName))
	{
		DT("删除目录失败！警告信息");
		return FALSE;
	}
	return TRUE;
}  

// 创建本地目录
BOOL CSystemUtility::LocalFileDirCreate(CString strFileName)
{
	return CValidFunction::CreatePath(strFileName);
}

// 获取指定路径下的文件夹和文件
BOOL CSystemUtility::GetPathList(CString strPath,std::vector<FILE_INFO_S>* pVecTor)
{
	WIN32_FIND_DATA wfd;			// 查找文件结构
	HANDLE hFind;					// 查找文件句柄
	
	CString sFullPath;				// 文件名
	CString strFindFilter;			// 文件过滤名
	CString strInfo;				// 文件的信息
    FILE_INFO_S emFileInfo;			// 文件属性结构

	strFindFilter  = strPath;
	strFindFilter += _T("\\*.*");   // 显示所有的文件
	
    // 句柄无效则退出
	if((hFind = FindFirstFile(strFindFilter, &wfd)) == INVALID_HANDLE_VALUE)
	{
		DW("Init Finder Failed.");
		return FALSE;
	}
	do
	{
		if (_tcscmp(wfd.cFileName, _T(".")) == 0 || _tcscmp(wfd.cFileName, _T("..")) == 0 )
		{
			continue;
		}
		
		sFullPath = strPath;
//		sFullPath += _T('\\');
		sFullPath += wfd.cFileName;
				
		// 文件的属性
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			emFileInfo.bPath			= TRUE;
			emFileInfo.dwFileAttributes	= wfd.dwFileAttributes;
			strcpy_s(emFileInfo.cFileName,wfd.cFileName);
			strcpy_s(emFileInfo.cFullPathName,sFullPath);
		}
		else
		{
			emFileInfo.bPath			= FALSE;
			emFileInfo.dwFileAttributes	= wfd.dwFileAttributes;
			emFileInfo.dwSize			= wfd.nFileSizeLow;		
			strcpy_s(emFileInfo.cFileName,wfd.cFileName);
			strcpy_s(emFileInfo.cFullPathName,sFullPath);
		}

		SYSTEMTIME tmSys;
		
		FileTimeToSystemTime(&wfd.ftCreationTime,&tmSys);
		sprintf_s(emFileInfo.cCreateTime,"%d-%d-%d %d:%d:%d",tmSys.wYear,tmSys.wMonth,tmSys.wDay,tmSys.wHour,tmSys.wMinute,tmSys.wSecond);
		
		FileTimeToSystemTime(&wfd.ftLastAccessTime,&tmSys);
		sprintf_s(emFileInfo.cAccessTime,"%d-%d-%d %d:%d:%d",tmSys.wYear,tmSys.wMonth,tmSys.wDay,tmSys.wHour,tmSys.wMinute,tmSys.wSecond);
		
		FileTimeToSystemTime(&wfd.ftLastWriteTime,&tmSys);
		sprintf_s(emFileInfo.cModifyTime,"%d-%d-%d %d:%d:%d",tmSys.wYear,tmSys.wMonth,tmSys.wDay,tmSys.wHour,tmSys.wMinute,tmSys.wSecond);	
		
		pVecTor->push_back(emFileInfo);
	}
	while (FindNextFile(hFind, &wfd));
	FindClose(hFind);

	return true;
}

// 获取窗口列表
DWORD CSystemUtility::GetWindowsList(std::vector<WINDOW_INFO_S>* pVecTor)
{
	pVecTor->clear();
	
	EnumWindows(CSystemUtility::EnumWindowsProc, (LPARAM)&pVecTor);
	
	return pVecTor->size();
}

BOOL CSystemUtility::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	std::vector<WINDOW_INFO_S>* pVecTor = *(std::vector<WINDOW_INFO_S>**)lParam;
	
	WINDOW_INFO_S emWin;
	memset(&emWin,0,sizeof(WINDOW_INFO_S));

	// HANDLE
	emWin.dwHWND = (DWORD)hWnd;

	// TITLE
	CHAR szTitleName[MAX_PATH];
	memset(szTitleName,0,MAX_PATH);
	::GetWindowText(hWnd,szTitleName, MAX_PATH);
	strcpy_s(emWin.szTitleName,szTitleName);

	// CLASS NAME
	CHAR szClassName[MAX_PATH];
	memset(szClassName,0,MAX_PATH);
	::GetClassName(hWnd,szClassName,MAX_PATH);
	strcpy_s(emWin.szClassName,szClassName);

	// PID
	DWORD	dwProcessID = 0;
	GetWindowThreadProcessId(hWnd,&dwProcessID);
	emWin.dwPid = dwProcessID;

	// PNAME
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwProcessID);
	if(hProcess)
	{
		CHAR szProcessName[MAX_PATH];
		memset(szProcessName,0,MAX_PATH);		
		GetModuleBaseName(hProcess,NULL,szProcessName,MAX_PATH);
		strcpy_s(emWin.szProcessName,szProcessName);
		CloseHandle(hProcess);
	}

	pVecTor->push_back(emWin);
	
	return TRUE;
}

// 获取当前进程列表
DWORD CSystemUtility::GetProcessList(std::vector<PROCESS_INFO_S>* pVecTor)
{
	pVecTor->clear();
	
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	
	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return( FALSE );
	
	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	
	// Retrieve information about the first process,
	// and exit if unsuccessful
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
		return( FALSE );
	}
	
	PROCESS_INFO_S emProcessInfo;

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		memset(&emProcessInfo,0,sizeof(PROCESS_INFO_S));
		
		strcpy_s(emProcessInfo.szFileName,pe32.szExeFile);
		emProcessInfo.dwPid			= pe32.th32ProcessID;
		emProcessInfo.dwParentPID	= pe32.th32ParentProcessID;
		emProcessInfo.dwPriority	= pe32.pcPriClassBase;
		emProcessInfo.dwThreads		= pe32.cntThreads;
		
		pVecTor->push_back(emProcessInfo);

	} while( Process32Next( hProcessSnap, &pe32 ) );
	
	CloseHandle( hProcessSnap );

	return pVecTor->size();
}

// 关闭进程
BOOL CSystemUtility::KillProcess(DWORD dwProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	CheckPointer(hProcess,FALSE);

	BOOL bRes = TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	return bRes;
}

// 添加或删除用户
BOOL CSystemUtility::NetUser(BOOL bAdd,CString strUser,CString strPWD)
{
	HINSTANCE hInst = NULL;
	CString strCMD;
	if(bAdd)
		strCMD.Format(_T("user %s %s /add"),strUser,strPWD);
	else
		strCMD.Format(_T("user %s /delete"),strUser);		
	
	hInst = ShellExecute(NULL, _T("open"), _T("net.exe"), strCMD, NULL, SW_HIDE);
	
	if((int)hInst <= 32)
		return FALSE;
	
	if(bAdd)
	{
		strCMD.Format(_T("localgroup administrators %s /add"),strUser);
		hInst = ShellExecute(NULL, _T("open"), _T("net.exe"), strCMD, NULL, SW_HIDE);
		
		if((int)hInst <= 32)
			return FALSE;
	}
	
	return TRUE;
}

// 将某磁盘做为共享或关闭共享
// strDisk = _T("C:\") 或 strDisk = _T("C:")
BOOL CSystemUtility::NetShare(BOOL bAdd,CString strDisk)
{
	HINSTANCE hInst = NULL;
	CString strCMD,strDiskRoot;

	strDiskRoot = strDisk.Left(1);
	if(bAdd)
		strCMD.Format(_T("share %s%c=%s"),strDiskRoot,'$',strDisk);
	else
		strCMD.Format(_T("share %s%c /delete"),strDiskRoot,'$');
	
	hInst = ShellExecute(NULL, _T("open"), _T("net.exe"), strCMD, NULL, SW_HIDE);
	
	return (int)hInst > 32;
}

// 开始或关闭WINDOWS防火墙
BOOL CSystemUtility::NetFireWall(BOOL bStart)
{
	HINSTANCE hInst = NULL;
	CString strCMD;

	strCMD = bStart ? _T("start sharedaccess") : _T("stop sharedaccess");
	
	hInst = ShellExecute(NULL, _T("open"), _T("net.exe"), strCMD, NULL, SW_HIDE);
	
	return (int)hInst > 32;
}

// 删除自身程序文件,该函数调用结束,需退出程序
BOOL CSystemUtility::DeleteSelf()
{
	TCHAR	szModule [MAX_PATH],
			szComspec[MAX_PATH],
			szParams [MAX_PATH];
	
	MEMORY_BASIC_INFORMATION mbi;
	static int ndummy;
	VirtualQuery(&ndummy,&mbi,sizeof(mbi));

	// get file path names:
	if( (GetModuleFileName(reinterpret_cast<HMODULE>(mbi.AllocationBase),szModule,MAX_PATH)		!= 0) &&
		(GetShortPathName(szModule,szModule,MAX_PATH)					!= 0) &&
		(GetEnvironmentVariable("COMSPEC",szComspec,MAX_PATH)			!= 0) )
	{
		// set command shell parameters
		lstrcpy(szParams," /c  del ");
		lstrcat(szParams, szModule);
		lstrcat(szParams, " > nul");
		lstrcat(szComspec, szParams);
		
		::SetFileAttributes(szModule,FILE_ATTRIBUTE_NORMAL);

		// set struct members
		STARTUPINFO	si			= {0};
		PROCESS_INFORMATION	pi	= {0};
		si.cb					= sizeof(si);
		si.dwFlags				= STARTF_USESHOWWINDOW;
		si.wShowWindow			= SW_HIDE;
		
		// increase resource allocation to program
		::SetPriorityClass(GetCurrentProcess(),	REALTIME_PRIORITY_CLASS);
		::SetThreadPriority(GetCurrentThread(),	THREAD_PRIORITY_TIME_CRITICAL);
		
		// invoke command shell
		if(CreateProcess(0, szComspec, 0, 0, 0,CREATE_SUSPENDED | DETACHED_PROCESS, 0, 0, &si, &pi))
		{
			// suppress command shell process until program exits
			::SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			::SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE); 
			
			// resume shell process with new low priority
			::ResumeThread(pi.hThread);
			
			// everything seemed to work
			return TRUE;
		}
		else // if error, normalize allocation
		{
			::SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
			::SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		}
	}
	
	return FALSE;
}

// 获取当前服务列表
DWORD CSystemUtility::GetServiceList(std::vector<SERVICE_INFO_S>* pVecTor)
{
	pVecTor->clear();

	SC_HANDLE	hSCM				= NULL;
	DWORD		dwBytesNeeded		= 0;
	DWORD		dwServicesReturned	= 0;
	DWORD		dwResume			= 0;
	hSCM = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS | SC_MANAGER_ENUMERATE_SERVICE);
	CheckPointer(hSCM,0);
	
	ENUM_SERVICE_STATUS Services;
	::EnumServicesStatus(hSCM,SERVICE_WIN32,SERVICE_STATE_ALL,&Services,sizeof(Services),&dwBytesNeeded,&dwServicesReturned,&dwResume);
	
	DWORD dwBufSize = dwBytesNeeded;	
	LPENUM_SERVICE_STATUS lpServices = (LPENUM_SERVICE_STATUS) malloc(dwBufSize);
	EnumServicesStatus(hSCM,SERVICE_WIN32,SERVICE_STATE_ALL,lpServices,dwBufSize,&dwBytesNeeded,&dwServicesReturned,&dwResume);
	
	SC_HANDLE hService = NULL;
	for(DWORD  dwIndex = 0; dwIndex < dwServicesReturned; dwIndex ++)
	{
		hService = ::OpenService(hSCM,lpServices[dwIndex].lpServiceName,SC_MANAGER_ALL_ACCESS);
		if(hService)
		{
			QUERY_SERVICE_CONFIG  emConfig;
			::QueryServiceConfig(hService,&emConfig,sizeof(QUERY_SERVICE_CONFIG),&dwBytesNeeded);
			
			SERVICE_INFO_S emService;
			emService.dwRunState	= lpServices[dwIndex].ServiceStatus.dwCurrentState;
			emService.dwStartType	= emConfig.dwStartType;
			strcpy_s(emService.szServiceName,lpServices[dwIndex].lpServiceName);
			strcpy_s(emService.szDisplayName,lpServices[dwIndex].lpDisplayName);
		
			pVecTor->push_back(emService);
		}
		
		CloseServiceHandle(hService);
		hService = NULL;
	}
	
	free(lpServices);
	CloseServiceHandle(hSCM);

	return pVecTor->size();
}

// 修改服务启动类型
BOOL CSystemUtility::SetServiceStartType(CString strServiceName,DWORD dwStartType)
{ 
    SC_LOCK		sclLock		= NULL;		
	SC_HANDLE	hSCM		= NULL;	
	SC_HANDLE   hService	= NULL;
	hSCM = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	CheckPointer(hSCM,FALSE);

    // Need to acquire database lock before reconfiguring. 
    sclLock = LockServiceDatabase(hSCM); 
	if(sclLock == NULL)
	{
		// Close The Service Manage
		CloseServiceHandle(hSCM);
	}

    // The database is locked, so it is safe to make changes. 
    // Open a handle to the service. 
    hService = OpenService( 
        hSCM,						// SCManager database 
        strServiceName,				// name of service 
        SERVICE_CHANGE_CONFIG);		// need CHANGE access 
    if (hService == NULL) 
    {
        DW("OpenService failed (%d)\n", GetLastError()); 
		
		// Release the database lock. 
		UnlockServiceDatabase(sclLock); 
			
		// Close The Service Manage
		CloseServiceHandle(hSCM);

		return FALSE;
    }
 
    // Make the changes. 
    if (! ChangeServiceConfig( 
        hService,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        dwStartType,       // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        NULL,              // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change 
        NULL,              // password: no change 
        NULL) )            // display name: no change
    {
        DW("ChangeServiceConfig failed (%d)\n", GetLastError()); 
		// Release the database lock. 
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		CloseServiceHandle(hService); 
		
		// Close The Service Manage
		CloseServiceHandle(hSCM);
		return FALSE;
    }

    // Release the database lock. 
    UnlockServiceDatabase(sclLock); 
 
    // Close the handle to the service. 
    CloseServiceHandle(hService); 

	// Close The Service Manage
	CloseServiceHandle(hSCM);

    return TRUE;
}

// 修改服务状态
BOOL CSystemUtility::SetServiceState(CString strServiceName,DWORD dwState)
{
	BOOL bRes = FALSE;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if(hSCM)
	{
		SC_HANDLE hService = OpenService(hSCM,strServiceName,SERVICE_ALL_ACCESS);
		if(hService)
		{
			SERVICE_STATUS ss;			
			if(dwState == SERVICE_RUNNING)
			{
				bRes = ::StartService(hService, 0, NULL);
			}
			else if(dwState == SERVICE_STOPPED)
			{
				bRes = ControlService(hService,SERVICE_CONTROL_STOP,&ss);
			}
			else if(dwState == SERVICE_PAUSED)
			{
				bRes = ControlService(hService,SERVICE_CONTROL_PAUSE,&ss);
			}
			else if(dwState == SERVICE_CONTINUE_PENDING)
			{
				bRes = ControlService(hService,SERVICE_CONTROL_CONTINUE,&ss);				
			}
			
			CloseServiceHandle(hService);
			hService = NULL;
		}	

		CloseServiceHandle(hSCM);
		hSCM = NULL;
	}
	
	return bRes;
}
