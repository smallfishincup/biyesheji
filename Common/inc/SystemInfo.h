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

	//			��ȡCPU��Ϣ
	BOOL		GetCPUInfo(CString &strCPUInfo);				
	
	//			��ȡϵͳ�汾
	BOOL		GetWindowsVersion(DWORD &dwMajorVersion, DWORD &dwMinorVersion, DWORD &dwPlatformId);			
	
	//			��ȡ�ڴ��С	(MΪ��λ)
	BOOL		GetMemorySize(DWORD &dwMemorySize);				
	
	//			��ȡϵͳ����ʱ�䳤��
	BOOL		GetRunTime(DWORD &nDays,DWORD &nHours,DWORD &nMinutes,DWORD &nSenconds);

	//			��ȡ���������
	BOOL		GetLocalComputerName(CString &strComputerName);

	//			��ȡ��ǰ��½�û�
	BOOL		GetLocalUserName(CString &strUserName);

	//			�Ƿ�������ͷ
	BOOL		IsHaveCamera();
	
};

#endif // !defined(AFX_SYSTEMINFO_H__C3398299_8CD1_4A32_99EC_6C216D371481__INCLUDED_)
