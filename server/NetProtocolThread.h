#pragma once
#include "threadex.h"
class CNetProtocolThread : public CThreadEx
{
public:
	// Construction & Destruction
	CNetProtocolThread(HWND hWndMsg,CString strClientIP,UINT nListenPort);
	virtual	~CNetProtocolThread();
			
	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();

	//					�������ӿͻ���SOCKET
	BOOL				CreateProtocolSocket();
	//					�������ӿͻ���SOCKET
	BOOL				DeleteProtocolSocket();
	
	void				MSleep(DWORD dwMSleep);
		
public:
	SOCKET				m_hSocketProtocol;				// ���ӿͻ���SOCKET
	
private:
	HWND				m_hWndMsg;
	CString				m_strClientIP;					// �ͻ���IP��ַ
	UINT				m_nProtocolPort;				// Э������˿�	
	BOOL				m_bSafeExit;
	BOOL				m_bThreadRolling;
};

