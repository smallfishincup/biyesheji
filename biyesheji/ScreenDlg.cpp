// ScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "biyesheji.h"
#include "ScreenDlg.h"
#include "afxdialogex.h"
#include "ClientDlg.h"
#include "DealCMDThread.h"
#include "lz77.h"

// CScreenDlg 对话框
#define TIMER_SCREEN		7878
#define ELAPSE_SCREEN		1000

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/,CClientDlg* pClientDlg,SOCKET hSocket)
	: CDialogEx(CScreenDlg::IDD, pParent)
{
	m_nFrameRate = 1;
	m_hConnectionSocket = hSocket;
	m_pClientDlg		= pClientDlg;
	m_hConnectionSocket	= hSocket;
	m_pScreenThread		= NULL;
	m_pScreenBuffer		= NULL;
	m_pOriBuffer		= new BYTE[1024 * 1024 * 5];		// 5M 原始数据缓冲
	m_dwFrameCount		= 0;
	m_dwScreenSize		= 0;
	//m_hBKBrush			= CreateSolidBrush(MAIN_FRAME_BK_COLOR);
}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序
BOOL CScreenDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	CRect	rcWnd;
	GetClientRect(&rcWnd);
	m_stVideo.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL, CRect(0,0,900,440), this, 0);
	m_stVideo.ModifyStyle(NULL, WS_HSCROLL | WS_VSCROLL);

	InitScreen();
	return TRUE;
}
//重载函数 控件跟着对话框的改变而改变
void CScreenDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	if(nType == 1) return;
	// TODO: Add your message handler code here
	CRect	rcWnd;
	GetClientRect(&rcWnd);

	CWnd *pWnd;   
    pWnd = GetDlgItem(IDC_BUTTON_START);  
    ChangeSize(pWnd, cx, cy);
	pWnd = GetDlgItem(IDC_BUTTON_STOP);  
    ChangeSize(pWnd, cx, cy);

	if(m_stVideo.GetSafeHwnd())
	{
		m_stVideo.SetWindowPos(NULL, rcWnd.left + 10, rcWnd.top + 10, 
			rcWnd.Width() - 20, rcWnd.Height() - 100,
			SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	GetClientRect(&m_rect);
}
void CScreenDlg::ChangeSize(CWnd *pWnd, int cx, int cy){
	 if(pWnd)   
    {  
        CRect rect;    
        pWnd->GetWindowRect(&rect);  
        ScreenToClient(&rect); 
  
        //    
        rect.left=rect.left*cx/m_rect.Width(); 
        rect.right=rect.right*cx/m_rect.Width();  
        rect.top=rect.top*cy/m_rect.Height();  
        rect.bottom=rect.bottom*cy/m_rect.Height();  
        pWnd->MoveWindow(rect);
    }  
}

BOOL CScreenDlg::InitScreen()
{
	CheckPointer((m_hConnectionSocket != INVALID_SOCKET),FALSE);
	CheckPointer(m_pClientDlg->m_pListenThread,FALSE);

	SERVER_REMOTE_S emServer;
	int nSize = m_pClientDlg->m_pListenThread->m_arrServer.GetSize();
	for(int nIndex = 0; nIndex < nSize; nIndex ++)
	{
		emServer = m_pClientDlg->m_pListenThread->m_arrServer.GetAt(nIndex);
		if(emServer.hSocketServer == m_hConnectionSocket)
		{
			m_dwFrameCount = 0;

			// 找到对应SOCKET
			UINT nLocalPort = 0;
			SOCKET hSocket  = INVALID_SOCKET;
			if(!AllocLocalDataPort(hSocket,nLocalPort))
			{
				DW("分配本地数据传输端口失败");
				return FALSE;
			}
			
			if(!StartScreenThread(hSocket))
			{
				DW("建立屏幕传输线程失败");
				return FALSE;
			}
			
			// 发送指令
			CDealCMDThread* pCMDThread = (CDealCMDThread*)emServer.pCMDThread;
			if(pCMDThread == NULL)
			{
				DW("发送指令线程为空,发送指令失败");
				return FALSE;
			}
			
			NM_CMD_S nmCmd;
			memset(&nmCmd,0,sizeof(NM_CMD_S));
			nmCmd.dwCmd = CMD_GETSCREEN;
			nmCmd.dw1   = TRUE;
			nmCmd.dw2   = nLocalPort;
			if(!pCMDThread->SendCMDToSvr(&nmCmd,sizeof(NM_CMD_S)))
			{
				DW("发送查看屏幕指令失败");
				return FALSE;
			}

			if(!pCMDThread->GetCMDReq(&nmCmd) || 
				nmCmd.dwCmd != CMD_SUCCESS)
			{
				DW("接收查看屏幕指令返回失败");
				return FALSE;
			}
			
			StartScreenTimer();

			return TRUE;
		}
	}

	return FALSE;
}

// 动态分配本地传输数据端口
BOOL CScreenDlg::AllocLocalDataPort(SOCKET &hSocketData,UINT &nLocalPort)
{
	sockaddr_in addr;
	SOCKET		hSocket = INVALID_SOCKET;
	
	hSocket = socket(AF_INET,SOCK_STREAM,0);	
	if(hSocket == INVALID_SOCKET)
		return FALSE;
	
	//准备服务器的信息，这里需要指定服务器的地址
	addr.sin_family				= AF_INET;
	addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	addr.sin_port				= htons(0);
	
	int nSocketOpt = 0xFFFFFFFF;
	if (setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)(&nSocketOpt),sizeof(int)) == SOCKET_ERROR) 
	{
		int nErr = WSAGetLastError();
		DW(_T("AllocLocalDataPort setsockopt SO_RCVBUF Failed. nErr = %d"),nErr);
		closesocket (hSocket);
		return FALSE;
	}
	
	if(SOCKET_ERROR == bind(hSocket, (LPSOCKADDR)&addr, sizeof(addr)))
	{ 
		int nErr = WSAGetLastError();
		DW(_T("AllocLocalDataPort bind Failed. nErr = %d"),nErr);
		closesocket (hSocket);
		return FALSE;
	}
	
	if(SOCKET_ERROR == listen(hSocket, 1))
	{
		int nErr = WSAGetLastError();
		DW(_T("AllocLocalDataPort listen Failed. nErr = %d"),nErr);
		closesocket (hSocket);
		return FALSE;
	}
	
	// 动态获取绑定端口
	struct sockaddr_in *SockAddrIn_RTP = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	int nNameLen = sizeof(struct sockaddr_in);
	if (getsockname (hSocket, (struct sockaddr *) SockAddrIn_RTP, &nNameLen) == SOCKET_ERROR)
	{
		int nErr = WSAGetLastError();
		DW(_T("AllocLocalDataPort getsockname Failed. nErr = %d"),nErr);
		free(SockAddrIn_RTP);
		closesocket (hSocket);
		return FALSE;
	}
	
	// 获取RTP端口
	nLocalPort	= ntohs( SockAddrIn_RTP->sin_port );
	hSocketData	= hSocket;
	
	free(SockAddrIn_RTP);
	
	return TRUE;
}

// 开始查看远程屏幕线程
BOOL CScreenDlg::StartScreenThread(SOCKET hSocket)
{
	StopScreenThread();
	
	m_pScreenThread = new CReceiveScreenThread(m_hWnd,hSocket,CScreenDlg::OnRecvData,(void*)this);
	CheckPointer(m_pScreenThread,FALSE);
	
	m_pScreenThread->ResumeThread();
	
	return TRUE;
}

// 停止查看远程屏幕线程
BOOL CScreenDlg::StopScreenThread()
{
	StopScreenTimer();
	SAFE_DELETE(m_pScreenThread);
	
	return TRUE;
}

// 定时更新屏幕数据
void CScreenDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == TIMER_SCREEN)
	{
		int nFrameType = SCREEN_FULL;
		
		if (m_dwFrameCount & 15)
			nFrameType = SCREEN_XOR;
		
		if(m_pScreenThread == NULL || !m_pScreenThread->RequistFrame(nFrameType))
		{
			}
	}

	CDialog::OnTimer(nIDEvent);
}

void CScreenDlg::StartScreenTimer()
{
	SetTimer(TIMER_SCREEN,ELAPSE_SCREEN / m_nFrameRate,NULL);
}

void CScreenDlg::StopScreenTimer()
{
	KillTimer(TIMER_SCREEN);
}

// 接收数据回调函数
UINT CScreenDlg::OnRecvData(BYTE* pRtpRecv,UINT nRecv,void* pRTPRecvWnd)
{
	CScreenDlg* pScreen = (CScreenDlg*)pRTPRecvWnd;
	CheckPointer(pScreen,0);
	CheckPointer(pRtpRecv,0);
	
	// 分析RTP包头
	RTP_HDR_S*	pHdr	= (RTP_HDR_S*)pRtpRecv;
	BYTE*		pData	= pRtpRecv + sizeof(RTP_HDR_S);
	
	CheckPointer(pData,0);
	
	// 判断包,如果类型不正确,则直接丢包
	if(pHdr->nPacketType != PACKET_TYPE_SCREEN)
		return 0;
	
	// RTP包长度不正确,直接丢包
	if(pHdr->nRTPData + sizeof(RTP_HDR_S) != nRecv)
		return 0;	
	
	pScreen->DecDraw(pData,pHdr->nRTPData,(pHdr->nPacketType == PACKET_TYPE_SCREEN));
	
	return 0;
}

// 解码显示
// pRTPData		有效数据
// nRTPData		数据长度
BOOL CScreenDlg::DecDraw(BYTE* pRTPData,int nRTPData,BOOL bVideoPacket)
{
//	CAutoLock lock(&m_csLock);
	
	CheckPointer(pRTPData,FALSE);
	if(nRTPData <= 0)
		return FALSE;

	Lz77Decompress(m_pOriBuffer, pRTPData, nRTPData);

	PSCREENCAP_SCREENBITMAP psb = (PSCREENCAP_SCREENBITMAP)m_pOriBuffer;
	BITMAPINFOHEADER bih;

	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biWidth			= psb->Width;
	bih.biHeight		= - psb->Height;
	bih.biPlanes		= 1;
	bih.biBitCount		= psb->Depth;
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	BITMAPFILEHEADER bfh;
	bfh.bfReserved1		= bfh.bfReserved2 = 0;
	bfh.bfType			= ((WORD)('M'<< 8)|'B');
	bfh.bfSize			= 54 + labs(bih.biWidth * bih.biHeight);
	bfh.bfOffBits		= 54;

	if (psb->Method == SCREEN_DIFFERENCE)
	{
		memcpy(m_pScreenBuffer,&bfh,sizeof(BITMAPFILEHEADER));
		memcpy(m_pScreenBuffer + sizeof(BITMAPFILEHEADER),&bih,sizeof(BITMAPINFOHEADER));

		if (psb->Size != 0)
			UpdateBuffer((PBYTE)psb->Bits, (PBYTE)m_pScreenBuffer + sizeof(bih) + sizeof(bfh), psb->Size, psb->Width);
	}
	else if (psb->Method == SCREEN_FULL)
	{
		if (m_pScreenBuffer != NULL)
			HeapFree(GetProcessHeap(), 0, m_pScreenBuffer);
		
		m_pScreenBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, psb->Size + sizeof(bih) + sizeof(bfh));
		m_dwScreenSize	= psb->Size + sizeof(bih) + sizeof(bfh);
		
		memcpy(m_pScreenBuffer,&bfh,sizeof(BITMAPFILEHEADER));
		memcpy(m_pScreenBuffer + sizeof(BITMAPFILEHEADER),&bih,sizeof(BITMAPINFOHEADER));
		
		if (m_pScreenBuffer != NULL)
		{
			memcpy(m_pScreenBuffer + sizeof(bih) + sizeof(bfh), psb->Bits, psb->Size);
		}
	}
	else if (psb->Method == SCREEN_XOR)
	{
		memcpy(m_pScreenBuffer,&bfh,sizeof(BITMAPFILEHEADER));
		memcpy(m_pScreenBuffer + sizeof(BITMAPFILEHEADER),&bih,sizeof(BITMAPINFOHEADER));

		if (psb->Size != 0)
			XorBuffer((PBYTE)psb->Bits, (PBYTE)m_pScreenBuffer + sizeof(bih) + sizeof(bfh), psb->Size);
	}

	HBITMAP hBitmap = m_Snap.ChangeDataToBitmap((PBYTE)m_pScreenBuffer);
	if(hBitmap)
	{
		m_dwFrameCount ++;
		
		m_stVideo.SetBitmap(hBitmap);
	
	}
	
	
	return TRUE;
}

void CScreenDlg::UpdateBuffer(PBYTE src, PBYTE dst, DWORD len, DWORD width)
{
	PBYTE out = dst;
	
	for (PBYTE p = src + len; src < p;)
	{
		DWORD temp = *src++;
		
		if (temp >= 248)
		{
			DWORD y = ((temp & 0x07) << 8) | *src++;
			
			out = dst + y * width * 2;
			memcpy(out, src, width * 2);
			src += width * 2;
		}
		else if (temp >= 240)
		{
			DWORD y = ((temp & 0x07) << 8) | *src++;
			
			out = dst + y * width * 2;
		}
		else
		{
			DWORD offset = temp & 0x07;
			
			if (temp >= 224)
				offset = (offset << 8) | *src++;
			
			DWORD length = *((PBYTE)src)++;
			
			if (length >= 240)
				length = ((length & 0x0f) << 8) | *src++;
			
			memcpy(out + offset, src, length);
			src += length;
			out += offset + length;
		}
	}
}

void CScreenDlg::XorBuffer(PBYTE src, PBYTE dst, DWORD len)
{
	for (DWORD i = 0; i < len / 4; i++)
	{
		((PDWORD)dst)[i] ^= ((PDWORD)src)[i];
	}
}
