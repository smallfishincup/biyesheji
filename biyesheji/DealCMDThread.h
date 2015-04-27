#pragma once
#include "threadex.h"

class CDealCMDThread : public CThreadEx  
{
public:
	// Construction & Destruction
	CDealCMDThread(HWND hWndMsg,SOCKET hSocketCMD);
	virtual				~CDealCMDThread();
	
	//					��Զ�̷���ָ��
	BOOL				SendCMDToSvr(NM_CMD_S* pNMCmd,LONG lCmd);

	//					��Զ�̻�ȡָ�����Ϣ
	BOOL				GetCMDReq(NM_CMD_S* pNMCmd);
	
	//					��Զ�̻�ȡ�����
	BOOL				GetCMDBuf(BYTE* pData, DWORD dwData);

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
		
	//					����CMD�߳�Socket
	BOOL				DeleteCMDSocket();
		
	BOOL				SOKCET_Select(SOCKET hSocket,int nTimeOut,BOOL bWrite);
		
	//					���뼶��ʱ	nMSleep������
	void				MSleep(int nMSleep);
		
private:
	HWND				m_hWndMsg;						// ������Ϣ���ھ��
	SOCKET				m_hSocketCMD;					// ָ����߳�Socket
	
	BOOL				m_bSafeExit;					// �߳��˳���־
	BOOL				m_bThreadRolling;				// �߳�ѭ�������ļ����ݱ�־
	
	DWORD				m_dwHeartProtocol;				// ������ʱ
};	
