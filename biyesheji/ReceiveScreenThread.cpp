#include "StdAfx.h"
#include "ReceiveScreenThread.h"


CReceiveScreenThread::CReceiveScreenThread(HWND hWndMsg,SOCKET hSocketScreen,RTP_ON_RECV_CB OnRecvData,void* pRTPRecv)
{
	m_hWndMsg			= hWndMsg;
	m_hSocketListen		= hSocketScreen;
	m_hSocketScreen		= INVALID_SOCKET;
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;

	m_pRTPRecvData		= NULL;
	m_pTCPData			= NULL;
	m_nTCPDataLeft		= 0;
	m_bNeedRTPHead		= FALSE;

	RTP_On_Recv_Data    = OnRecvData;
	m_pRTPRecv		    = pRTPRecv;	
}

CReceiveScreenThread::~CReceiveScreenThread()
{
	if(m_bSafeExit)
		return;

	DeleteScreenSocket();

	m_bSafeExit = TRUE;
//	while(m_bThreadRolling)
//		Sleep(1);

	SAFE_DELETEA(m_pRTPRecvData);
	SAFE_DELETEA(m_pTCPData);

	KillThread();
}

BOOL CReceiveScreenThread::NewRTPDataMemory()
{
	if(m_pRTPRecvData == NULL)
	{
		m_pRTPRecvData = new BYTE [RTP_MAX_PACKET_BUFF];
		memset(m_pRTPRecvData,0,RTP_MAX_PACKET_BUFF);
	}
	
	return (m_pRTPRecvData != NULL);
}

BOOL CReceiveScreenThread::NewTCPDatamemory()
{
	if(m_pTCPData == NULL)
	{
		m_pTCPData			= new BYTE [RTP_MAX_PACKET_BUFF * 3];
		memset(m_pTCPData,0,RTP_MAX_PACKET_BUFF * 3);
	}
	
	return (m_pTCPData != NULL);
}

// 销毁CMD线程Socket
BOOL CReceiveScreenThread::DeleteScreenSocket()
{
	if(m_hSocketListen != INVALID_SOCKET)
	{
		shutdown(m_hSocketListen,SD_BOTH);
		closesocket(m_hSocketListen);
		m_hSocketListen = NULL;
	}

	if(m_hSocketScreen != INVALID_SOCKET)
	{
		shutdown(m_hSocketScreen,SD_BOTH);
		closesocket(m_hSocketScreen);
		m_hSocketScreen = INVALID_SOCKET;
	}

	return TRUE;
}

// 初始化资源
BOOL CReceiveScreenThread::InitInstance()
{
	return CThreadEx::InitInstance();
}

int CReceiveScreenThread::ExitInstance()
{
	return CThreadEx::ExitInstance();
}

BOOL CReceiveScreenThread::RequistFrame(int nFrameType)
{
	if(m_bSafeExit || m_hSocketScreen == INVALID_SOCKET)
		return FALSE;

	return (send(m_hSocketScreen,(char*)&nFrameType,sizeof(char),0) > 0);
}	

int CReceiveScreenThread::Run()
{
	int		nByteRecv	= 0;
	int		nSelectRes  = 0;
	fd_set	readfds;		
	struct	timeval tv;
	
	m_hSocketScreen = accept(m_hSocketListen, NULL, NULL);
	int nSocketOpt = 0xFFFFFFFF;
	if (setsockopt(m_hSocketScreen,SOL_SOCKET,SO_RCVBUF,(const char *)(&nSocketOpt),sizeof(int)) == SOCKET_ERROR) 
	{
		closesocket(m_hSocketScreen);
		return 0;
	}
	
	while(!m_bSafeExit && m_hSocketScreen != INVALID_SOCKET)
	{
		m_bThreadRolling = TRUE;   
		try
		{
			tv.tv_sec  = 3;
			tv.tv_usec = 0;
			
			FD_ZERO(&readfds);
			FD_SET(m_hSocketScreen, &readfds);
			
			nSelectRes = select(m_hSocketScreen + 1, &readfds, NULL, NULL, &tv);
			if(nSelectRes == -1)
			{
				m_bThreadRolling = FALSE;   
				DW("Receive Screen Select Failed.nSelectRes = -1");
				return 0;
			}
			else if (nSelectRes == 0)
			{
				m_bThreadRolling = FALSE;   
				continue;
			}
			else
			{
				if(FD_ISSET(m_hSocketScreen, &readfds))
				{
					// 如果接收RTP数据缓存无效,则重新申请内存
					if(m_pRTPRecvData == NULL)
					{
						if(!NewRTPDataMemory())
						{
							m_bThreadRolling = FALSE;   
							continue;
						}
					}
					if(m_pTCPData == NULL)
					{
						m_bNeedRTPHead = TRUE;
						if(!NewTCPDatamemory())
						{
							m_bThreadRolling = FALSE;   
							continue;
						} 
					}

					// TCP方式接收数据,由于TCP粘包,所以对接收到的数据要进行分析,然后进行投递给解码显示模块
					nByteRecv = recv(m_hSocketScreen, (char *)m_pRTPRecvData, RTP_MAX_PACKET_BUFF, 0);
					if(nByteRecv <= 0)
					{
						m_bThreadRolling = FALSE;   
						int nErr = WSAGetLastError();
						DW(_T("TCP接收屏幕数据异常: 接收到数据长度 = %d,自动关闭当前连接 nErr = %d"),nByteRecv,nErr);
						return 0;
					}
				
					if(m_nTCPDataLeft + nByteRecv > RTP_MAX_PACKET_BUFF * 3)
					{
						DW("TCP接收屏幕数据缓冲溢出,清空缓存,重新接收数据");
						
						// 当缓存已满,则清空缓存继续等下一个有效RTP序列
						m_bNeedRTPHead		= TRUE;
						m_nTCPDataLeft		= 0;
						m_bThreadRolling	= FALSE;   
						continue;
					}

					// 需要重新等待有效RTP包处理过程
					if(m_bNeedRTPHead)
					{
						// 不是完整RTP头则直接返回,等待下一个RTP包
						if(nByteRecv < sizeof(RTP_HDR_S))
						{
							m_bThreadRolling = FALSE;   
							continue;
						}
						
						RTP_HDR_S*	pHdr = (RTP_HDR_S*)m_pRTPRecvData;
						// 判断包,如果类型不正确,则直接丢包
						if(pHdr->nPacketType != PACKET_TYPE_SCREEN)
						{
							m_bThreadRolling = FALSE;   
							continue;
						}

						m_nTCPDataLeft = 0;
						memcpy(m_pTCPData,m_pRTPRecvData,nByteRecv);
						m_nTCPDataLeft = nByteRecv;	
						m_bNeedRTPHead = FALSE;
					}
					else
					{
						RTP_HDR_S*	pHdr    = NULL;
						UINT nNeedSendBuff	= 0;						
						BYTE *pBuffExchange	= m_pTCPData + m_nTCPDataLeft;
						
						// 将接收到的RTP数据填充到保存RTP数据尾部
						memcpy(pBuffExchange,m_pRTPRecvData,nByteRecv);
						m_nTCPDataLeft += nByteRecv;
						
						if( m_nTCPDataLeft >= sizeof(RTP_HDR_S) )
						{
							pHdr			= (RTP_HDR_S*)m_pTCPData;
							nNeedSendBuff   = pHdr->nRTPData + sizeof(RTP_HDR_S);
							
							// 判断音视频包,如果类型不正确,则直接丢包
							if((pHdr->nPacketType != PACKET_TYPE_SCREEN))
							{
								DW("Check RTP Head Failed. pHdr->nPacketType = %02X nNeedSendBuff = %d",pHdr->nPacketType,nNeedSendBuff);
								
								// 校验RTP头出错则清空缓存,重新接收RTP数据
								m_bNeedRTPHead	 = TRUE;
								m_bThreadRolling = FALSE;   
								continue;
							}

							// 循环发送数据
							while (((UINT)(m_nTCPDataLeft) >= nNeedSendBuff) && (nNeedSendBuff > 0) && !m_bSafeExit)
							{
								if(m_bSafeExit)
								{
									m_bThreadRolling = FALSE;
									return 0;
								}

								// 转发音视频压缩数据
								if(RTP_On_Recv_Data && m_pRTPRecv && !m_bSafeExit)
								{
									RTP_On_Recv_Data(m_pTCPData, nNeedSendBuff,m_pRTPRecv);
								}
								
								// 重新统计缓存数据
								pBuffExchange	 = m_pTCPData + nNeedSendBuff;
								m_nTCPDataLeft	-= nNeedSendBuff;
								
								memcpy(m_pTCPData,pBuffExchange,m_nTCPDataLeft);
								
								if(m_nTCPDataLeft < sizeof(RTP_HDR_S))
								{
									break;
								}
								
								pHdr			= (RTP_HDR_S*)m_pTCPData;
								// 判断音视频包,如果类型不正确,则直接丢包
								if(pHdr->nPacketType != PACKET_TYPE_SCREEN)
								{
									m_bNeedRTPHead   = TRUE;
									break;
								}
								
								nNeedSendBuff   = pHdr->nRTPData + sizeof(RTP_HDR_S);
							}	
						}									
					} 
				}
			} 
		}
		catch(...)
		{
			DeleteScreenSocket();
			m_bThreadRolling = FALSE;   
			DW("Catch Receive Screen Select Failed.");
			break;
		}
	}

	m_bThreadRolling = FALSE;   

	return 0;
}
