#pragma once
#include "threadex.h"
class CReceiveScreenThread :
	public CThreadEx
{
public:
		CReceiveScreenThread(HWND hWndMsg,SOCKET hSocketScreen,RTP_ON_RECV_CB OnRecvData,void* pRTPRecv);
	virtual				~CReceiveScreenThread();
			
	BOOL				RequistFrame(int nFrameType);

	// Operations & Overridables
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
		
	//					销毁线程Socket
	BOOL				DeleteScreenSocket();
	BOOL				NewRTPDataMemory();
	BOOL				NewTCPDatamemory();
		
private:
	HWND				m_hWndMsg;						// 接收消息窗口句柄
	SOCKET				m_hSocketScreen;				// 接收远程屏幕线程Socket
	SOCKET				m_hSocketListen;				// 监听连接SOCKET

	BOOL				m_bSafeExit;					// 线程退出标志
	BOOL				m_bThreadRolling;				// 线程循环处理文件数据标志
	
	BYTE*				m_pRTPRecvData;					// 屏幕数据
	BYTE*				m_pTCPData;						// TCP连接方式时数据缓冲,用来保存分析包数据
	UINT				m_nTCPDataLeft;					// TCP数据缓冲字节数
	BOOL				m_bNeedRTPHead;					// 需要解析第一个RTP数据头
	
	RTP_ON_RECV_CB		RTP_On_Recv_Data;				// 接收频数据回调函数
	void*				m_pRTPRecv;						// 接收数据类指针
};

