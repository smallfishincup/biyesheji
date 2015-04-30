#include "StdAfx.h"
#include "DealCMDThread.h"
#include "SystemInfo.h"

CDealCMDThread::CDealCMDThread(HWND hWndMsg,int* pWndMain,SOCKET hSocket,CString strClientIP)
{
	m_hWndMsg			= hWndMsg;
	m_pWndMain			= pWndMain;
	m_strClientIP		= strClientIP;
	m_hSocketClient		= hSocket;
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;
 

}

CDealCMDThread::~CDealCMDThread(void)
{
	m_bSafeExit = TRUE;
	while(m_bThreadRolling)
		Sleep(1);

	KillThread();
}

BOOL CDealCMDThread::InitInstance()
{
	return CThreadEx::InitInstance();
}

int CDealCMDThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

// 线程回调
int CDealCMDThread::Run()
{
	int		nByteRecv	= 0;
	int		nSelectRes  = 0;
	fd_set	readfds;		
	struct	timeval tv;
	NM_CMD_S emCMD;
	
	while(!m_bSafeExit && m_hSocketClient != INVALID_SOCKET)
	{
		try
		{
			m_bThreadRolling = TRUE;
			tv.tv_sec  = 3;
			tv.tv_usec = 0;
			
			FD_ZERO(&readfds);
			FD_SET(m_hSocketClient, &readfds);
			
			nSelectRes = select(m_hSocketClient + 1, &readfds, NULL, NULL, &tv);
			if(nSelectRes == -1)
			{
				m_bThreadRolling = FALSE;
//				DW("Receive CMD Select Failed.nSelectRes = -1");
				::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)FALSE,0);				
				break;
			}
			else if (nSelectRes == 0)
			{
				m_bThreadRolling = FALSE;
				continue;
			}
			else
			{
				if(FD_ISSET(m_hSocketClient, &readfds))
				{
					ZeroMemory(&emCMD,sizeof(NM_CMD_S));
					// TCP方式接收数据,由于TCP粘包,所以对接收到的数据要进行分析,然后进行投递给解码显示模块
					nByteRecv = recv(m_hSocketClient, (char *)&emCMD, sizeof(NM_CMD_S), 0);
					if(nByteRecv <= 0)
					{
						m_bThreadRolling = FALSE;
//						DW("Receive CMD Select Failed. nByteRecv = %d",nByteRecv);
						::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)FALSE,0);	
						break;
					}

					ParseCMD(emCMD);
				}

				m_bThreadRolling = FALSE;
			} 
		}
		catch(...)
		{
			m_bThreadRolling = FALSE;
//			DW("Catch Receive CMD Select Failed.");
			::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)FALSE,0);	
		}
	}

	m_bThreadRolling = FALSE;

	return 0;
}

// 发送系统信息到客户端
BOOL CDealCMDThread::Command_SendSystemInfo()
{
	SYSTEM_INFO_S emSysInfo;
	FillSystemInfo(emSysInfo);
	
	NM_CMD_S nmCmd;
	memset(&nmCmd,0,sizeof(NM_CMD_S));
	nmCmd.dwCmd = CMD_GETSYSTEMINFO;
	memcpy(nmCmd.pBuff , &emSysInfo,sizeof(SYSTEM_INFO_S));

	return SendMsgToClient(&nmCmd,sizeof(NM_CMD_S));
}

BOOL CDealCMDThread::SendMsgToClient(NM_CMD_S* pNMCmd,LONG lCmd)
{
	CheckPointer(pNMCmd,FALSE);
	if(lCmd <= 0)
		return FALSE;
	
	// 检查当前SOCKET是否可写
	if(!SOKCET_Select(m_hSocketClient,30,TRUE))
	{
		int nErr = WSAGetLastError();
//		DW("SendMsgToClient select Failed. nErr = %d",nErr);
		return FALSE;
	}	

	return (send(m_hSocketClient,(const char *)pNMCmd, lCmd, 0) == lCmd);
}


// 获取系统信息
BOOL CDealCMDThread::FillSystemInfo(SYSTEM_INFO_S &emSysInfo_s)
{
	BOOL bRes = TRUE;
	
	CSystemInfo emSystemInfo;
	CString strCPUInfo,strComputerName,strUserName,strLocalIP;

	bRes &= emSystemInfo.GetCPUInfo(strCPUInfo);
	strcpy(emSysInfo_s.szCPUType,strCPUInfo);

	bRes &= emSystemInfo.GetLocalComputerName(strComputerName);
	strcpy(emSysInfo_s.szComputerName,strComputerName);

//	bRes &= emSystemInfo.GetLocalUserName(strUserName);
//	strcpy(emSysInfo_s.szUserName,strUserName);

	bRes &= emSystemInfo.GetWindowsVersion(emSysInfo_s.dwMajorVersion,emSysInfo_s.dwMinorVersion,emSysInfo_s.dwPlatformId);
	
	bRes &= emSystemInfo.GetMemorySize(emSysInfo_s.dwMemorySize);

	bRes &= emSystemInfo.GetRunTime(emSysInfo_s.nRunDays,emSysInfo_s.nRunHours,emSysInfo_s.nRunMinutes,emSysInfo_s.nRunSenconds);

	emSysInfo_s.bHaveCamera	= emSystemInfo.IsHaveCamera();

	return bRes;
}

BOOL CDealCMDThread::SOKCET_Select(SOCKET hSocket,int nTimeOut,BOOL bWrite)
{
	fd_set    fdset;
	struct timeval tv;
	FD_ZERO(&fdset);
    FD_SET(hSocket, &fdset);
    tv.tv_sec  = nTimeOut;
    tv.tv_usec = 0;
	
	int nRes = 0;
	if(bWrite)
		nRes = select(hSocket + 1, NULL , &fdset, NULL, &tv);
	else
		nRes = select(hSocket + 1, &fdset, NULL , NULL, &tv);
	
	if(nRes == 0)
	{
//		DW(_T("SOKCET_Select Time Out"));
		return FALSE;
	}
	
	if(nRes < 0)
	{
		int nErr = WSAGetLastError();
//		DW(_T("SOKCET_Select Select Error = %d"),nErr);
		return FALSE;
	}
	
	if(FD_ISSET(hSocket, &fdset))
		return TRUE;
	
	return FALSE;
}
BOOL CDealCMDThread::Command_ReqCMDStatus(BOOL bSuccess)
{
	NM_CMD_S nmCmd;
	memset(&nmCmd,0,sizeof(NM_CMD_S));
	nmCmd.dwCmd = bSuccess ? CMD_SUCCESS : CMD_ERROR;
	return SendMsgToClient(&nmCmd,sizeof(NM_CMD_S));
}

// 分析指令
BOOL CDealCMDThread::ParseCMD(NM_CMD_S emCMD)
{
	
	switch(emCMD.dwCmd)
	{

	case CMD_GETSYSTEMINFO:		// 要求本地计算机配置信息
		{
			Command_SendSystemInfo();
		}
		break;
	case CMD_PROTOCOL_PING:
		{
			Command_ReqCMDStatus(TRUE);
		}
		break;
	}
	
	return TRUE;
}
