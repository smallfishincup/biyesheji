#pragma once
#include "threadex.h"
#include "ScreenSnap.h"
class CSendScreenThread : public CThreadEx
{
public:
	// Construction & Destruction
	CSendScreenThread(HWND hWndMsg,CString strClientIP,UINT nDataPort);
	virtual	~CSendScreenThread();

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();

	BOOL				ConnectDataPort();
	BOOL				DisConnectDataPort();
		
	BOOL				SendSocketData(char* pData,DWORD dwData);
	BOOL				CompressAndSendPack(SOCKET hSocket, PVOID buffer, DWORD length);
		
public:
	SOCKET				m_hSocketData;	
private:
	HWND				m_hWndMsg;
	CString				m_strClientIP;					// �ͻ���IP��ַ
	UINT				m_nDataPort;					// ���ܴ���ͼ�����ݶ˿�
	BOOL				m_bSafeExit;
	BOOL				m_bThreadRolling;

	PBYTE				m_pCompressData;				// ��Ļѹ�����ݻ���
	
	CScreenSnap			m_Snap;
	
};

