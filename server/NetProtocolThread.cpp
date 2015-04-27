#include "StdAfx.h"
#include "NetProtocolThread.h"


CNetProtocolThread::CNetProtocolThread(HWND hWndMsg,CString strClientIP,UINT nProtocolPort)
{
	m_hWndMsg			= hWndMsg;
	m_strClientIP		= strClientIP;
	m_nProtocolPort		= nProtocolPort;
	m_hSocketProtocol	= INVALID_SOCKET;
	
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;
}

CNetProtocolThread::~CNetProtocolThread()
{
	DeleteProtocolSocket();

	m_bSafeExit = TRUE;
	while(m_bThreadRolling)
		Sleep(1);

	KillThread();
}

// 创建连接客户端SOCKET
BOOL CNetProtocolThread::CreateProtocolSocket()
{
	DeleteProtocolSocket();

	SOCKET hSocket = socket(AF_INET,SOCK_STREAM,0);	
	if(hSocket == INVALID_SOCKET)
	{
		int nError = WSAGetLastError();
//		DW(_T("CNetProtocolThread::CreateProtocolSocket.Create Socket Failed,(Error = %d)"),nError);
		return FALSE;
	}
	
	int nSocketOpt = 0xFFFFFFFF;
	if (setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)(&nSocketOpt),sizeof(int)) == SOCKET_ERROR) 
	{
		int nErr = WSAGetLastError();
//		DW(_T("CNetProtocolThread::CreateProtocolSocket setsockopt SO_RCVBUF Failed. nErr = %d"),nErr);
		shutdown(hSocket,SD_BOTH);
		closesocket (hSocket);
		return FALSE;
	}

    
	struct sockaddr_in addr;
	addr.sin_family				= AF_INET;
	addr.sin_addr.S_un.S_addr	= inet_addr(m_strClientIP);
	addr.sin_port				= htons(m_nProtocolPort);
	
	if(connect(hSocket,(const struct sockaddr*)&addr,sizeof(addr)) == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();
//		DW(_T("CNetProtocolThread::CreateProtocolSocket.connect Failed,(Error = %d)"),nError);
		shutdown(hSocket,SD_BOTH);
		closesocket(hSocket);
		return FALSE;
	}

	m_hSocketProtocol = hSocket;

	return TRUE;
}

// 销毁连接客户端SOCKET
BOOL CNetProtocolThread::DeleteProtocolSocket()
{
	if(m_hSocketProtocol != INVALID_SOCKET)
	{
		shutdown(m_hSocketProtocol,SD_BOTH);
		closesocket(m_hSocketProtocol);
		m_hSocketProtocol = INVALID_SOCKET;
	}

	return TRUE;
}

// 初始化资源
BOOL CNetProtocolThread::InitInstance()
{
	AfxSocketInit();
	return CThreadEx::InitInstance();
}

int CNetProtocolThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

// 播放进度处理
int CNetProtocolThread::Run()
{
	while(!m_bSafeExit && m_hSocketProtocol == INVALID_SOCKET)
	{
		m_bThreadRolling = TRUE;
		if(!CreateProtocolSocket())
		{
			// 连接失败情况下,过一分钟继续连接
			DWORD dwMSleep = 1000 * 30;
			MSleep(dwMSleep);
			::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)FALSE,0);
			break;
		}
		else
		{
			m_bThreadRolling = FALSE;
			::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)TRUE,0);
			break;
		}
		m_bThreadRolling = FALSE;
	}

	m_bThreadRolling = FALSE;

	return 0;
}

// 毫秒级延时	dwMSleep毫秒数
void CNetProtocolThread::MSleep(DWORD dwMSleep)
{
	DWORD dwStart = GetTickCount();
	
	DWORD dwVal = 0;
	DWORD dwCurrTick = 0;
	while(!m_bSafeExit)
	{
		dwVal = GetTickCount() - dwStart;
		if( dwVal < 0 || dwVal >= dwMSleep)
			break;
		
		Sleep((dwMSleep - dwVal) / 1000);
	}
}