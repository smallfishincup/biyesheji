// SystemInfo.h: interface for the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMINFO_H__C3398299_8CD1_4A32_99EC_6C216D371481__INCLUDED_)
#define AFX_SYSTEMINFO_H__C3398299_8CD1_4A32_99EC_6C216D371481__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegisterKey.h"
class CSystemInfo  
{
public:
	CSystemInfo();
	virtual ~CSystemInfo();

	//			获取CPU信息
	BOOL		GetCPUInfo(CString &strCPUInfo);				
	
	//			获取系统版本
	BOOL		GetWindowsVersion(DWORD &dwMajorVersion, DWORD &dwMinorVersion, DWORD &dwPlatformId);			
	
	//			获取内存大小	(M为单位)
	BOOL		GetMemorySize(DWORD &dwMemorySize);				
	
	//			获取系统运行时间长度
	BOOL		GetRunTime(DWORD &nDays,DWORD &nHours,DWORD &nMinutes,DWORD &nSenconds);

	//			获取计算机名称
	BOOL		GetLocalComputerName(CString &strComputerName);

	//			获取当前登陆用户
	BOOL		GetLocalUserName(CString &strUserName);

	//			是否有摄像头
	BOOL		IsHaveCamera();
	
};

#endif // !defined(AFX_SYSTEMINFO_H__C3398299_8CD1_4A32_99EC_6C216D371481__INCLUDED_)
