#pragma once
#include "threadex.h"
class CListenConnectionThread : public CThreadEx
{
public:
	// Construction & Destruction
	CListenConnectionThread(HWND hWndMsg,UINT nListenPort = CLIENT_LISTEN_PORT);
	virtual				~CListenConnectionThread();
			
	BOOL				UpdateSvrSystemInfo(SOCKET hSocket,SYSTEM_INFO_S emSysInfo);
	BOOL				DeleteComputer(SOCKET hSocket);
	
	CString				GetServerIP(SOCKET hSocket);
	CString				GetServerAddress(SOCKET hSocket);

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();

	//					�������������߳�Socket
	BOOL				CreateListenSocket();

	//					���ټ����߳�Socket
	BOOL				DeleteListenSocket();
		
	//					�ر������˵�����
	BOOL				CloseAllServerSocket();
	
	void				EmptyServer(SERVER_REMOTE_S emServer);
	CString				GetSocketIP(SOCKET hSocket);
	BOOL				IsComputerExist(CString strIP,CString strComputerName);			// ������Ƿ��Ѿ�����
		
public:
	SOCKET				m_hSocketListen;				// ���������߳�Socket

	CArray<SERVER_REMOTE_S,SERVER_REMOTE_S> m_arrServer;// Զ�˼����������Ϣ
	
private:
	HWND				m_hWndMsg;						// ������Ϣ���ھ��
	UINT				m_nListenPort;					// �����˿�
	
	BOOL				m_bSafeExit;					// �߳��˳���־
	BOOL				m_bThreadRolling;				// �߳�ѭ�������ļ����ݱ�־
};

