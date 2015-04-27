#pragma once
#include "threadex.h"

class CDealCMDThread : public CThreadEx  
{
public:
	// Construction & Destruction
	CDealCMDThread(HWND hWndMsg,SOCKET hSocketCMD);
	virtual				~CDealCMDThread();
	
	//					向远程发送指令
	BOOL				SendCMDToSvr(NM_CMD_S* pNMCmd,LONG lCmd);

	//					从远程获取指令返回信息
	BOOL				GetCMDReq(NM_CMD_S* pNMCmd);
	
	//					从远程获取命令缓冲
	BOOL				GetCMDBuf(BYTE* pData, DWORD dwData);

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
		
	//					销毁CMD线程Socket
	BOOL				DeleteCMDSocket();
		
	BOOL				SOKCET_Select(SOCKET hSocket,int nTimeOut,BOOL bWrite);
		
	//					毫秒级延时	nMSleep毫秒数
	void				MSleep(int nMSleep);
		
private:
	HWND				m_hWndMsg;						// 接收消息窗口句柄
	SOCKET				m_hSocketCMD;					// 指令处理线程Socket
	
	BOOL				m_bSafeExit;					// 线程退出标志
	BOOL				m_bThreadRolling;				// 线程循环处理文件数据标志
	
	DWORD				m_dwHeartProtocol;				// 心跳计时
};	
