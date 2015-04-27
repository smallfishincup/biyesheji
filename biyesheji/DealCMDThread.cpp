#include "StdAfx.h"
#include "DealCMDThread.h"
#include "NL_QQwry.h"

CDealCMDThread::CDealCMDThread(HWND hWndMsg,SOCKET hSocketCMD)
{
	m_hWndMsg			= hWndMsg;
	m_hSocketCMD		= hSocketCMD;

	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;

	m_dwHeartProtocol	= GetTickCount();
}

CDealCMDThread::~CDealCMDThread()
{
	if(m_bSafeExit)
		return;

	DeleteCMDSocket();
	
	m_bSafeExit = TRUE;
	while(m_bThreadRolling)
		Sleep(1);

	KillThread();
}

// 销毁CMD线程Socket
BOOL CDealCMDThread::DeleteCMDSocket()
{
	if(m_hSocketCMD != INVALID_SOCKET)
	{
		shutdown(m_hSocketCMD,SD_BOTH);
		closesocket(m_hSocketCMD);
		m_hSocketCMD = INVALID_SOCKET;
	}

	return TRUE;
}

// 初始化资源
BOOL CDealCMDThread::InitInstance()
{
	return CThreadEx::InitInstance();
}

int CDealCMDThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

// 播放进度处理
int CDealCMDThread::Run()
{
	int		nByteRecv	= 0;
	NM_CMD_S emCMD;
	
	while(!m_bSafeExit && m_hSocketCMD != INVALID_SOCKET)
	{
		m_bThreadRolling = TRUE;
	}

	m_bThreadRolling = FALSE;   

	return 0;
}

// 毫秒级延时	nMSleep毫秒数
void CDealCMDThread::MSleep(int nMSleep)
{
	DWORD dwStart = GetTickCount();

	DWORD dwVal = 0;
	DWORD dwCurrTick = 0;
	while(!m_bSafeExit && m_hSocketCMD != INVALID_SOCKET)
	{
		dwVal = GetTickCount() - dwStart;
		if( dwVal < 0 || dwVal >= (DWORD)nMSleep)
			break;
		
		Sleep(100);
	}
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
		DW(_T("SOKCET_Select Time Out"));
		return FALSE;
	}
	
	if(nRes < 0)
	{
		int nErr = WSAGetLastError();
		DW(_T("SOKCET_Select Select Error = %d"),nErr);
		return FALSE;
	}
	
	if(FD_ISSET(hSocket, &fdset))
		return TRUE;
	
	return FALSE;
}

// 向远程发送指令
BOOL CDealCMDThread::SendCMDToSvr(NM_CMD_S* pNMCmd,LONG lCmd)
{
	CheckPointer(pNMCmd,FALSE);
	if(lCmd <= 0)
		return FALSE;

	// 检查当前SOCKET是否可写
	if(!SOKCET_Select(m_hSocketCMD,30,TRUE))
	{
		int nErr = WSAGetLastError();
		DW("SendMsgToClient select Failed. nErr = %d",nErr);
		return FALSE;
	}	

	if((send(m_hSocketCMD,(const char *)pNMCmd, lCmd, 0) == lCmd))
	{
		m_dwHeartProtocol = GetTickCount();
		return TRUE;
	}
	
	return FALSE;
}

// 从远程获取指令返回信息
BOOL CDealCMDThread::GetCMDReq(NM_CMD_S* pNMCmd)
{
	CheckPointer(pNMCmd,FALSE);

	// 检查当前SOCKET是否可写
	if(!SOKCET_Select(m_hSocketCMD,30,FALSE))
	{
		int nErr = WSAGetLastError();
		DW("GetCMDReq select Failed. nErr = %d",nErr);
		return FALSE;
	}	

	return (recv(m_hSocketCMD,(char *)pNMCmd, sizeof(NM_CMD_S), 0) == sizeof(NM_CMD_S));
}

// 从远程获取命令缓冲
BOOL CDealCMDThread::GetCMDBuf(BYTE* pData, DWORD dwData)
{
	CheckPointer(pData,FALSE);
	if(dwData <= 0)
		return FALSE;

	// 检查当前SOCKET是否可写
	if(!SOKCET_Select(m_hSocketCMD,30,FALSE))
	{ 
		int nErr = WSAGetLastError();
		DW("GetCMDReq select Failed. nErr = %d",nErr);
		return FALSE;
	}	

	int nTotal = 0;
	int nRecv  = 0;
	while(nTotal < dwData)
	{
		nRecv = recv(m_hSocketCMD,(char *)(pData + nTotal),dwData - nTotal, 0);
		if(nRecv <= 0)
			return FALSE;

		nTotal += nRecv;
	}
	
	return TRUE;
}
BOOL CListenConnectionThread::UpdateSvrSystemInfo(SOCKET hSocket,SYSTEM_INFO_S emSysInfo)
{
	SERVER_REMOTE_S emServer;
	for(int nIndex = 0; nIndex < m_arrServer.GetSize(); nIndex ++)
	{
		emServer = m_arrServer.GetAt(nIndex);
		if(emServer.hSocketServer == hSocket)	
		{
			memcpy(&emServer.emSystemInfo,&emSysInfo,sizeof(SYSTEM_INFO_S));

			// 获取计算机实际地址
			CString strAddress;
			NL_QQwry *qq = new NL_QQwry;
			strAddress = qq->IP2Add(emServer.szServerIP);
			delete qq;
			strcpy(emServer.szAddress,strAddress);

			m_arrServer.SetAt(nIndex,emServer);

			return TRUE;
		}
	}
	
	return FALSE;
}

void CListenConnectionThread::EmptyServer(SERVER_REMOTE_S emServer)
{
	emServer.hSocketServer	= INVALID_SOCKET;
	memset(emServer.szServerIP,0,_MAX_PATH);
	memset(&emServer.emSystemInfo,0,sizeof(SYSTEM_INFO_S));
}
