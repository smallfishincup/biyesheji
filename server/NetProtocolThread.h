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

	//					创建连接客户端SOCKET
	BOOL				CreateProtocolSocket();
	//					销毁连接客户端SOCKET
	BOOL				DeleteProtocolSocket();
	
	void				MSleep(DWORD dwMSleep);
		
public:
	SOCKET				m_hSocketProtocol;				// 连接客户端SOCKET
	
private:
	HWND				m_hWndMsg;
	CString				m_strClientIP;					// 客户端IP地址
	UINT				m_nProtocolPort;				// 协议监听端口	
	BOOL				m_bSafeExit;
	BOOL				m_bThreadRolling;
};

