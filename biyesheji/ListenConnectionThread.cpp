#include "StdAfx.h"
#include "ListenConnectionThread.h"
#include "DealCMDThread.h"

CListenConnectionThread::CListenConnectionThread(HWND hWndMsg,UINT nListenPort) {
	m_hWndMsg			= hWndMsg;
	m_nListenPort		= nListenPort;
	m_hSocketListen		= INVALID_SOCKET;

	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;

	m_arrServer.RemoveAll();

	if(!CreateListenSocket())
	{
		DW("Create Listen Socket Failed.");
		return;
	}
}

CListenConnectionThread::~CListenConnectionThread(void) {
		if(m_bSafeExit)
		return;
	
	DeleteListenSocket();
	CloseAllServerSocket();

	m_bSafeExit = TRUE;	
	while(m_bThreadRolling)
		Sleep(1);

	KillThread();
}

// �������������߳�Socket
BOOL CListenConnectionThread::CreateListenSocket()
{
	 AfxSocketInit();
	sockaddr_in addr;
	SOCKET		hSock = INVALID_SOCKET;
	
	DeleteListenSocket();
	
	hSock = socket(AF_INET,SOCK_STREAM,0);	
	if(hSock == INVALID_SOCKET)
	{
		int nError = WSAGetLastError();
//		DW(_T("CNetProtocolThread::CreateProtocolSocket.Create Socket Failed,(Error = %d)"),nError);
		return FALSE;
	}
	
	//׼������������Ϣ��������Ҫָ���������ĵ�ַ
	addr.sin_family				= AF_INET;
	addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	addr.sin_port				= htons(m_nListenPort);		//�ı�˿ںŵ����ݸ�ʽ 80
	
	char* OptVal = _T("1");
	if(SOCKET_ERROR != setsockopt(hSock ,SOL_SOCKET,SO_REUSEADDR,OptVal,sizeof(char)))
	{
		if(SOCKET_ERROR != bind(hSock, (LPSOCKADDR)&addr, sizeof(addr)))
		{ 
			if(SOCKET_ERROR != listen(hSock, 1024))			//���֧��1024������
			{
				m_hSocketListen = hSock;
				
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

// ��ʼ����Դ
BOOL CListenConnectionThread::InitInstance()
{


	return CThreadEx::InitInstance();
}

int CListenConnectionThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

// ���Ž��ȴ���
int CListenConnectionThread::Run() {
	 
	while(!m_bSafeExit && m_hSocketListen != INVALID_SOCKET)
	{
		m_bThreadRolling = TRUE;

		SOCKET hSocket = accept(m_hSocketListen, NULL, NULL);
		if(hSocket != INVALID_SOCKET)
		{
			// ��������ָ��SOCKET
			SERVER_REMOTE_S emServer;
			memset(&emServer,0,sizeof(SERVER_REMOTE_S));
			EmptyServer(emServer);
			
			// ���浱ǰSOCKET
			emServer.hSocketServer = hSocket;
			
			// ���浱ǰIP��ַ
			CString strIP;
			strIP = GetSocketIP(hSocket);
			strcpy(emServer.szServerIP,strIP);
			
			CDealCMDThread* pCMDThread = new CDealCMDThread(m_hWndMsg,hSocket);
			pCMDThread->ResumeThread();
			emServer.pCMDThread = pCMDThread;
		
			// ��һ�����ӵķ���˻��Զ�����ϵͳ������Ϣ���ͻ���
			// ���͵ĵ�һ��ָ�.��ϵͳ������Ϣ,ͬ����Ҫ�˿�
			NM_CMD_S emCMD;			
			if(!pCMDThread->GetCMDReq(&emCMD) ||
				emCMD.dwCmd != CMD_GETSYSTEMINFO)
			{
				DW("Parse CMD_GETSYSTEMINFO Failed.");
				// ���յ�ǰ���ӷ����ָ��ʧ��,��˿���ǰ����,�ȴ�����
				shutdown(hSocket,SD_BOTH);
				closesocket(hSocket);
				hSocket = INVALID_SOCKET;
				break;
			}
			
			m_arrServer.Add(emServer);

			// �����ϵͳ��Ϣ
			SYSTEM_INFO_S emSysInfo;
			memcpy(&emSysInfo,emCMD.pBuff,sizeof(SYSTEM_INFO_S));
			UpdateSvrSystemInfo(hSocket,emSysInfo);

			::PostMessage(m_hWndMsg,WM_CONNECT_CLIENT,(WPARAM)TRUE,hSocket);							
		}

		m_bThreadRolling = FALSE;   
	}

	m_bThreadRolling = FALSE;   

	return 0;

}

// ���ټ����߳�Socket
BOOL CListenConnectionThread::DeleteListenSocket() {
	if(m_hSocketListen != INVALID_SOCKET)
	{
		shutdown(m_hSocketListen,SD_BOTH);
		closesocket(m_hSocketListen);
		m_hSocketListen = INVALID_SOCKET;
	}

	return TRUE;
}

// �ر������˵�����
BOOL CListenConnectionThread::CloseAllServerSocket()
{
	SERVER_REMOTE_S emServer;
	int nServers = m_arrServer.GetSize();
	
	
	return TRUE;
}

CString CListenConnectionThread::GetSocketIP(SOCKET hSocket)
{
	if(hSocket == INVALID_SOCKET)
		return "";

	CString strIP;

	struct sockaddr_in *SockAddrIn_RTP = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	int nNameLen = sizeof(struct sockaddr_in);
	if (getpeername(hSocket, (struct sockaddr *) SockAddrIn_RTP, &nNameLen) == SOCKET_ERROR)
	{
		int nErr = WSAGetLastError();
		DW(_T("RTSP_AllocLocalRTPPort getsockname Failed. nErr = %d"),nErr);
		free(SockAddrIn_RTP);
		return "";
	}
	
	// ��ȡIP��ַ
	strIP = inet_ntoa(SockAddrIn_RTP->sin_addr);
	int nPort = ntohs( SockAddrIn_RTP->sin_port );
	free(SockAddrIn_RTP);

	DT("Remote IP = %s , Port = %d",strIP,nPort);

	return strIP;
}
BOOL CListenConnectionThread::DeleteComputer(SOCKET hSocket)
{
	SERVER_REMOTE_S emServer;
	CDealCMDThread*pCMDThread = NULL;
	for(int nIndex = 0; nIndex < m_arrServer.GetSize(); nIndex ++)
	{
		emServer = m_arrServer.GetAt(nIndex);
		if(emServer.hSocketServer == hSocket)
		{
			pCMDThread = (CDealCMDThread*)emServer.pCMDThread;
			SAFE_DELETE(pCMDThread);
			m_arrServer.RemoveAt(nIndex);
			
			return TRUE;
		}
	}
	
	return FALSE;
}
CString CListenConnectionThread::GetServerIP(SOCKET hSocket)
{
	CString strServerIP;
	SERVER_REMOTE_S emServer;
	CDealCMDThread*pCMDThread = NULL;

	for(int nIndex = 0; nIndex < m_arrServer.GetSize(); nIndex ++)
	{
		emServer = m_arrServer.GetAt(nIndex);
		if(emServer.hSocketServer == hSocket)
		{
			strServerIP = emServer.szServerIP;
				
			return strServerIP;
		}
	}
	
	return strServerIP;
}

CString CListenConnectionThread::GetServerAddress(SOCKET hSocket)
{
	CString strAddress;
	SERVER_REMOTE_S emServer;
	CDealCMDThread*pCMDThread = NULL;
	
	for(int nIndex = 0; nIndex < m_arrServer.GetSize(); nIndex ++)
	{
		emServer = m_arrServer.GetAt(nIndex);
		if(emServer.hSocketServer == hSocket)
		{
			strAddress = emServer.szAddress;
			
			return strAddress;
		}
	}
	
	return strAddress;
}