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

	//					创建监听服务线程Socket
	BOOL				CreateListenSocket();

	//					销毁监听线程Socket
	BOOL				DeleteListenSocket();
		
	//					关闭与服务端的连接
	BOOL				CloseAllServerSocket();
	
	void				EmptyServer(SERVER_REMOTE_S emServer);
	CString				GetSocketIP(SOCKET hSocket);
	BOOL				IsComputerExist(CString strIP,CString strComputerName);			// 计算机是否已经存在
		
public:
	SOCKET				m_hSocketListen;				// 监听服务线程Socket

	CArray<SERVER_REMOTE_S,SERVER_REMOTE_S> m_arrServer;// 远端计算机配置信息
	
private:
	HWND				m_hWndMsg;						// 接收消息窗口句柄
	UINT				m_nListenPort;					// 监听端口
	
	BOOL				m_bSafeExit;					// 线程退出标志
	BOOL				m_bThreadRolling;				// 线程循环处理文件数据标志
};

