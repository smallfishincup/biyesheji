#include "stdafx.h"
#include "opendesktop.h"
#include "stdio.h"
#include "atlbase.h"

COpenDesktop::COpenDesktop()
{
	_OpenDesktop();
}

COpenDesktop::COpenDesktop(LPCWSTR szName)
{
	_OpenDesktop(szName);
}

COpenDesktop::~COpenDesktop()
{
	_CloseDesktop();
}

BOOL COpenDesktop::_OpenDesktop(LPCWSTR szName)
{
	WCHAR pvInfo[128] = {0};
	if(szName != NULL)
	{
		lstrcpy((LPSTR)pvInfo, (LPCSTR)szName);
	}
	else
	{
		HDESK hActiveDesktop;
		DWORD dwLen;
		hActiveDesktop = OpenInputDesktop(DF_ALLOWOTHERACCOUNTHOOK, FALSE, MAXIMUM_ALLOWED);
		GetUserObjectInformation(hActiveDesktop, UOI_NAME, pvInfo, sizeof(pvInfo), &dwLen);
		CloseDesktop(hActiveDesktop);
	}
	
	m_hwinstaCurrent = GetProcessWindowStation();
	CheckPointer(m_hwinstaCurrent,FALSE);
	
    m_hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
	CheckPointer(m_hdeskCurrent,FALSE);
	
	// 打开winsta0
    m_hwinsta = OpenWindowStation(_T("winsta0"), FALSE,                          
									WINSTA_ACCESSCLIPBOARD   |
									WINSTA_ACCESSGLOBALATOMS |
									WINSTA_CREATEDESKTOP     |
									WINSTA_ENUMDESKTOPS      |
									WINSTA_ENUMERATE         |
									WINSTA_EXITWINDOWS       |
									WINSTA_READATTRIBUTES    |
									WINSTA_READSCREEN        |
									WINSTA_WRITEATTRIBUTES);
	CheckPointer(m_hwinsta,FALSE);
	
	
    if (!SetProcessWindowStation(m_hwinsta))
	{
		return FALSE;
    }
	
	// 打开desktop
    m_hdesk = OpenDesktop((LPSTR)pvInfo, 0, FALSE,                
							DESKTOP_CREATEMENU |
							DESKTOP_CREATEWINDOW |
							DESKTOP_ENUMERATE    |
							DESKTOP_HOOKCONTROL  |
							DESKTOP_JOURNALPLAYBACK |
							DESKTOP_JOURNALRECORD |
							DESKTOP_READOBJECTS |
							DESKTOP_SWITCHDESKTOP |
							DESKTOP_WRITEOBJECTS);
	CheckPointer(m_hdesk,FALSE);
	
	SetThreadDesktop(m_hdesk);

	return TRUE;
}

BOOL COpenDesktop::_CloseDesktop()
{
	if (!SetProcessWindowStation(m_hwinstaCurrent))
		return FALSE;
	
	if (!SetThreadDesktop(m_hdeskCurrent))
		return FALSE;
	
	if (!CloseWindowStation(m_hwinsta))
		return FALSE;
	
	if (!CloseDesktop(m_hdesk))
		return FALSE;
	
	return TRUE;
}
