#pragma once
#include "threadex.h"
class CReceiveScreenThread :
	public CThreadEx
{
public:
		CReceiveScreenThread(HWND hWndMsg,SOCKET hSocketScreen,RTP_ON_RECV_CB OnRecvData,void* pRTPRecv);
	virtual				~CReceiveScreenThread();
			
	BOOL				RequistFrame(int nFrameType);

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
		
	//					�����߳�Socket
	BOOL				DeleteScreenSocket();
	BOOL				NewRTPDataMemory();
	BOOL				NewTCPDatamemory();
		
private:
	HWND				m_hWndMsg;						// ������Ϣ���ھ��
	SOCKET				m_hSocketScreen;				// ����Զ����Ļ�߳�Socket
	SOCKET				m_hSocketListen;				// ��������SOCKET

	BOOL				m_bSafeExit;					// �߳��˳���־
	BOOL				m_bThreadRolling;				// �߳�ѭ�������ļ����ݱ�־
	
	BYTE*				m_pRTPRecvData;					// ��Ļ����
	BYTE*				m_pTCPData;						// TCP���ӷ�ʽʱ���ݻ���,�����������������
	UINT				m_nTCPDataLeft;					// TCP���ݻ����ֽ���
	BOOL				m_bNeedRTPHead;					// ��Ҫ������һ��RTP����ͷ
	
	RTP_ON_RECV_CB		RTP_On_Recv_Data;				// ����Ƶ���ݻص�����
	void*				m_pRTPRecv;						// ����������ָ��
};

