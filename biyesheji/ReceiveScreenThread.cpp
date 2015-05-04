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

// ����CMD�߳�Socket
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

// ��ʼ����Դ
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
					// �������RTP���ݻ�����Ч,�����������ڴ�
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

					// TCP��ʽ��������,����TCPճ��,���ԶԽ��յ�������Ҫ���з���,Ȼ�����Ͷ�ݸ�������ʾģ��
					nByteRecv = recv(m_hSocketScreen, (char *)m_pRTPRecvData, RTP_MAX_PACKET_BUFF, 0);
					if(nByteRecv <= 0)
					{
						m_bThreadRolling = FALSE;   
						int nErr = WSAGetLastError();
						DW(_T("TCP������Ļ�����쳣: ���յ����ݳ��� = %d,�Զ��رյ�ǰ���� nErr = %d"),nByteRecv,nErr);
						return 0;
					}
				
					if(m_nTCPDataLeft + nByteRecv > RTP_MAX_PACKET_BUFF * 3)
					{
						DW("TCP������Ļ���ݻ������,��ջ���,���½�������");
						
						// ����������,����ջ����������һ����ЧRTP����
						m_bNeedRTPHead		= TRUE;
						m_nTCPDataLeft		= 0;
						m_bThreadRolling	= FALSE;   
						continue;
					}

					// ��Ҫ���µȴ���ЧRTP���������
					if(m_bNeedRTPHead)
					{
						// ��������RTPͷ��ֱ�ӷ���,�ȴ���һ��RTP��
						if(nByteRecv < sizeof(RTP_HDR_S))
						{
							m_bThreadRolling = FALSE;   
							continue;
						}
						
						RTP_HDR_S*	pHdr = (RTP_HDR_S*)m_pRTPRecvData;
						// �жϰ�,������Ͳ���ȷ,��ֱ�Ӷ���
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
						
						// �����յ���RTP������䵽����RTP����β��
						memcpy(pBuffExchange,m_pRTPRecvData,nByteRecv);
						m_nTCPDataLeft += nByteRecv;
						
						if( m_nTCPDataLeft >= sizeof(RTP_HDR_S) )
						{
							pHdr			= (RTP_HDR_S*)m_pTCPData;
							nNeedSendBuff   = pHdr->nRTPData + sizeof(RTP_HDR_S);
							
							// �ж�����Ƶ��,������Ͳ���ȷ,��ֱ�Ӷ���
							if((pHdr->nPacketType != PACKET_TYPE_SCREEN))
							{
								DW("Check RTP Head Failed. pHdr->nPacketType = %02X nNeedSendBuff = %d",pHdr->nPacketType,nNeedSendBuff);
								
								// У��RTPͷ��������ջ���,���½���RTP����
								m_bNeedRTPHead	 = TRUE;
								m_bThreadRolling = FALSE;   
								continue;
							}

							// ѭ����������
							while (((UINT)(m_nTCPDataLeft) >= nNeedSendBuff) && (nNeedSendBuff > 0) && !m_bSafeExit)
							{
								if(m_bSafeExit)
								{
									m_bThreadRolling = FALSE;
									return 0;
								}

								// ת������Ƶѹ������
								if(RTP_On_Recv_Data && m_pRTPRecv && !m_bSafeExit)
								{
									RTP_On_Recv_Data(m_pTCPData, nNeedSendBuff,m_pRTPRecv);
								}
								
								// ����ͳ�ƻ�������
								pBuffExchange	 = m_pTCPData + nNeedSendBuff;
								m_nTCPDataLeft	-= nNeedSendBuff;
								
								memcpy(m_pTCPData,pBuffExchange,m_nTCPDataLeft);
								
								if(m_nTCPDataLeft < sizeof(RTP_HDR_S))
								{
									break;
								}
								
								pHdr			= (RTP_HDR_S*)m_pTCPData;
								// �ж�����Ƶ��,������Ͳ���ȷ,��ֱ�Ӷ���
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
