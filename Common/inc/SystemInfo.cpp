// SystemInfo.cpp: implementation of the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemInfo.h"
#include "Vfw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib,"Vfw32.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemInfo::CSystemInfo()
{
}

CSystemInfo::~CSystemInfo()
{
}

// ��ȡϵͳ�汾
BOOL CSystemInfo::GetWindowsVersion(DWORD &dwMajorVersion, DWORD &dwMinorVersion, DWORD &dwPlatformId)
{
	OSVERSIONINFOEX osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx ((OSVERSIONINFO *) &osvi))
		return FALSE;
	
	dwMajorVersion = osvi.dwMajorVersion;
	dwMinorVersion = osvi.dwMinorVersion;
	dwPlatformId   = osvi.dwPlatformId;
	return TRUE;
}

// ��ȡ�ڴ��С (MΪ��λ)
BOOL CSystemInfo::GetMemorySize(DWORD &dwMemorySize)
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	dwMemorySize = (DWORD)(ms.dwTotalPhys / (1024 * 1024));
	
	return TRUE;
}

// ��ȡCPU��Ϣ
BOOL CSystemInfo::GetCPUInfo(CString &strCPUInfo)
{
	CRegisterKey RegKey;
	if(RegKey.RegOpen(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0") == ERROR_SUCCESS)
	{
		RegKey.RegRead("ProcessorNameString", strCPUInfo);
		RegKey.RegClose();
		strCPUInfo.TrimLeft();
		strCPUInfo.TrimRight();
		return TRUE;
	}
		
	return FALSE;
}

// ��ȡϵͳ����ʱ�䳤��
BOOL CSystemInfo::GetRunTime(DWORD &nDays,DWORD &nHours,DWORD &nMinutes,DWORD &nSenconds)
{
	DWORD dwTicks	= GetTickCount();
	nDays			= (dwTicks / (24 * 60 * 60 * 1000));
	nHours			= (dwTicks / (60 * 60 * 1000)) % 24;
	nMinutes		= (dwTicks / (60 * 1000)) % 60;
	nSenconds		= (dwTicks / 1000) % 60;
	return TRUE;
}

// ��ȡ���������
BOOL CSystemInfo::GetLocalComputerName(CString &strComputerName)
{
	DWORD dwSize = _MAX_PATH;
	if(GetComputerName(strComputerName.GetBuffer(_MAX_PATH),&dwSize))
		return TRUE;

	return FALSE;
}

// ��ȡ��ǰ��½�û�
BOOL CSystemInfo::GetLocalUserName(CString &strUserName)
{
	DWORD dwSize = _MAX_PATH;
	if(GetUserName(strUserName.GetBuffer(_MAX_PATH),&dwSize))
		return TRUE;
	
	return FALSE;
}

// �Ƿ�������ͷ
BOOL CSystemInfo::IsHaveCamera()
{
	char szVfwName[MAX_PATH];
	char szVfwVersion[MAX_PATH];

	memset(szVfwName,0,MAX_PATH);
	memset(szVfwVersion,0,MAX_PATH);

	capGetDriverDescription(0, szVfwName, MAX_PATH, szVfwVersion, MAX_PATH);
	if(lstrlen(szVfwName) == 0)
		return FALSE;

	return TRUE;
}
