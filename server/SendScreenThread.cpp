#include "StdAfx.h"
#include "SendScreenThread.h"
#include "lz77.h"

CSendScreenThread::CSendScreenThread(HWND hWndMsg,CString strClientIP,UINT nDataPort)
{
	m_hWndMsg			= hWndMsg;
	m_strClientIP		= strClientIP;
	m_nDataPort			= nDataPort;
	m_hSocketData		= INVALID_SOCKET;
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;
	m_pCompressData		= NULL;

	ConnectDataPort();
}

CSendScreenThread::~CSendScreenThread()
{
	if(m_bSafeExit)
		return;

	DisConnectDataPort();
	
	m_bSafeExit = TRUE;
	while(m_bThreadRolling)
		Sleep(1);

	SAFE_DELETE(m_pCompressData);

	KillThread();
}

// 连接客户端数据端口
BOOL CSendScreenThread::ConnectDataPort()
{
	DisConnectDataPort();

	SOCKET hSocket = socket(AF_INET,SOCK_STREAM,0);	
	if(hSocket == INVALID_SOCKET)
	{
		int nError = WSAGetLastError();
//		DW(_T("CSendScreenThread::ConnectDataPort.Create Socket Failed,(Error = %d)"),nError);
		return FALSE;
	}
	
	int nSocketOpt = 0xFFFFFFFF;
	if (setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)(&nSocketOpt),sizeof(int)) == SOCKET_ERROR) 
	{
		closesocket(hSocket);
		return FALSE;
	}

	sockaddr_in addr;
	addr.sin_family				= AF_INET;
	addr.sin_addr.S_un.S_addr	= inet_addr(m_strClientIP);
	addr.sin_port				= htons(m_nDataPort);
	
	BOOL bConnectted = FALSE;
	for(int nIndex = 0; nIndex < 3; nIndex ++)
	{
		if(connect(hSocket,(LPSOCKADDR)&addr,sizeof(addr)) != SOCKET_ERROR)
		{
			bConnectted = TRUE;
			break;
		}
		
		int nError = WSAGetLastError();
	}
	
	if(!bConnectted)
	{
		closesocket(hSocket);
		return FALSE;
	}

	m_hSocketData = hSocket;

	return TRUE;
}

// 销毁连接客户端SOCKET
BOOL CSendScreenThread::DisConnectDataPort()
{
	if(m_hSocketData != INVALID_SOCKET)
	{
		shutdown(m_hSocketData,SD_BOTH);
		closesocket(m_hSocketData);
		m_hSocketData = INVALID_SOCKET;
	}

	return TRUE;
}

// 初始化资源
BOOL CSendScreenThread::InitInstance()
{
	return CThreadEx::InitInstance();
}

int CSendScreenThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

// 播放进度处理
int CSendScreenThread::Run()
{
	int		nByteRecv	= 0;
	int		nFrameType = 0;
	
	PSCREENCAP_SCREENBITMAP pFullScreen = NULL;
	PSCREENCAP_SCREENBITMAP pPartScreen = NULL;
	PSCREENCAP_SCREENBITMAP pScreenXor = NULL;
	
	DWORD dwSize = 0;
	if(m_pCompressData == NULL && m_Snap.GetScreenDataSize(dwSize))
	{
		m_pCompressData = new BYTE[dwSize];
	}

	while(!m_bSafeExit && m_hSocketData != INVALID_SOCKET)
	{
		m_bThreadRolling = TRUE;

		// 接收屏幕请求指令
		nByteRecv = recv(m_hSocketData, (char *)&nFrameType, sizeof(char), 0);
		if (nByteRecv <= 0)
		{
			m_bThreadRolling = FALSE;
			break;
		}

		switch (nFrameType)
		{
		case SCREEN_FULL:
			{
				if (pFullScreen != NULL)
					HeapFree(GetProcessHeap(), 0, pFullScreen);
				
				pFullScreen = m_Snap.ScrCapGetFullScreen();
				
				if (pFullScreen != NULL)
				{
					CompressAndSendPack(m_hSocketData, pFullScreen, sizeof(SCREENCAP_SCREENBITMAP) + pFullScreen->Size);
				}
			}
			break;
			
		case SCREEN_DIFFERENCE:
			{
				pPartScreen = m_Snap.ScrCapGetPartScreen(pFullScreen);
				
				if (pPartScreen != NULL)
				{
					CompressAndSendPack(m_hSocketData, pPartScreen, sizeof(SCREENCAP_SCREENBITMAP) + pPartScreen->Size);
					HeapFree(GetProcessHeap(), 0, pPartScreen);
				}
			}
			break;
			
		case SCREEN_XOR:
			{
				pScreenXor = pFullScreen;
				pFullScreen = m_Snap.ScrCapGetFullScreen();
				
				if (pFullScreen == NULL)
				{
					pFullScreen = pScreenXor;
				}
				else
				{
					for (int i = 0; i < pFullScreen->Width * pFullScreen->Height * 2 / 4; i++)
					{
						((PDWORD)pScreenXor->Bits)[i] ^= ((PDWORD)pFullScreen->Bits)[i];
					}
					
					pScreenXor->Method = SCREEN_XOR;
					CompressAndSendPack(m_hSocketData, pScreenXor, sizeof(SCREENCAP_SCREENBITMAP) + pScreenXor->Size);
					HeapFree(GetProcessHeap(), 0, pScreenXor);
				}
			}
			break;
		}
	}
	
	if (pFullScreen != NULL)
		HeapFree(GetProcessHeap(), 0, pFullScreen);

	m_bThreadRolling = FALSE;

	return 0;
}

BOOL CSendScreenThread::SendSocketData(char* pData,DWORD dwData)
{
	CheckPointer(pData,FALSE);
	
	int nTotal = 0;
	int nSend  = 0;
	while(nTotal < dwData)
	{
		nSend = send(m_hSocketData,(pData + nTotal), dwData - nTotal, 0);
		if(nSend <= 0)
		{
			return FALSE;
		}
		
		nTotal += nSend;
	}

	return TRUE;
}

BOOL CSendScreenThread::CompressAndSendPack(SOCKET hSocket, PVOID buffer, DWORD length)
{
	ULONG ulCompressData = Lz77Compress(m_pCompressData, buffer, length, 2);
	
	if(ulCompressData > 0)
	{
		RTP_HDR_S emRTP;
		emRTP.nPacketType = PACKET_TYPE_SCREEN;
		emRTP.nRTPData	  = ulCompressData;
		
		if(!SendSocketData((char*)&emRTP,sizeof(RTP_HDR_S)))
		{
//			GlobalFree(hLZWDib);
			return FALSE;
		}
		
		if(!SendSocketData((char*)m_pCompressData,ulCompressData))
		{
//			GlobalFree(hLZWDib);
			return FALSE;
		}
	}
	
//	GlobalFree(hLZWDib);

	return TRUE;
}
