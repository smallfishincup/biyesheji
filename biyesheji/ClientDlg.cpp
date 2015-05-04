
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "biyesheji.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include "ScreenDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CClientDlg �Ի���




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


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������		
	InitControls();
	m_nListenPort = 80;
	StartListenThread();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CClientDlg::OnBnClickedMonitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SOCKET hSocket = GetSelectComputerSocket();
	if(hSocket != INVALID_SOCKET)
	{
		// �����鿴Զ����Ļ���ڲ������յ���ͼ��������ʾ����
		CScreenDlg dlg(NULL,this,hSocket);
		dlg.DoModal();
	}
}



void CClientDlg::OnBnClickedMakeserver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}




void CClientDlg::InitControls(void) {
	m_listComputer.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_GRIDLINES);
	m_listComputer.SetTextColor(RGB(0,0,255));

	int nFmt = LVCFMT_CENTER;
	int iSub = 0;
	
	m_listComputer.InsertColumn(iSub ++,"��������",		LVCFMT_CENTER,200);	
	m_listComputer.InsertColumn(iSub ++,"IP��ַ",		LVCFMT_CENTER,120);
	m_listComputer.InsertColumn(iSub ++,"����ϵͳ����",	LVCFMT_CENTER,150);
	m_listComputer.InsertColumn(iSub ++,"CPU����",		LVCFMT_CENTER,300);
	m_listComputer.InsertColumn(iSub ++,"�ڴ��С",		LVCFMT_CENTER,80);	
	m_listComputer.InsertColumn(iSub ++,"���������",	LVCFMT_CENTER,120);
	m_listComputer.InsertColumn(iSub ++,"����ͷ",		LVCFMT_CENTER,80);
	m_listComputer.InsertColumn(iSub ++,"����ʱ��",		LVCFMT_CENTER,150);
	m_listComputer.InsertColumn(iSub ++,"����ʱ��",		LVCFMT_CENTER,150);

	m_listComputer.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_GRIDLINES);
	m_listMsg.InsertColumn(iSub ++,"������Ϣ",		LVCFMT_LEFT,650);
}

// ��ʼ�ͻ��˷��ͺͽ��չ���
BOOL CClientDlg::StartListenThread()
{
	StopListenThread();
	
	m_pListenThread = new CListenConnectionThread(m_hWnd,m_nListenPort);
	CheckPointer(m_pListenThread,FALSE);
	
	m_pListenThread->ResumeThread();
	 return TRUE;
}

// ֹͣ�ͻ��˷��ͺͽ��չ���
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
		// ����
		
		// �����б�
		RefreshComputerList();
		
		strAction.LoadString(IDS_STRING_ONLINE);
	}
	else
	{
		// ����
		m_pListenThread->DeleteComputer(hSocket);
		
		// �����б�
		RefreshComputerList();
		
		strAction.LoadString(IDS_STRING_OFFLINE);
	}

	strMsg.Format("%s IP��ַ: %s �����ַ: %s",strAction,strIP,strAddress);
	AddMsgToList(strMsg);
	return 1;
}


// ˢ�¼�����б�
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
// ����Ϣ�б���ӿ�����Ϣ
void CClientDlg::AddMsgToList(CString strMsg)
{
	m_listMsg.InsertItem(0,strMsg);
}

// �������б������Ϣ
BOOL CClientDlg::AddMsgToComputerList(SERVER_REMOTE_S emServerInfo)
{
	BOOL bRes = TRUE;

	int nItem			= m_listComputer.GetItemCount();
	int nSub			= 1;

	CString strSysVersion,strMemory,strCamera,strRunTime,strOnlineTime;

	// ��������
	m_listComputer.InsertItem(nItem,emServerInfo.szAddress,emServerInfo.emSystemInfo.bHaveCamera ? 1 : 0);
	
	// IP��ַ
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.szServerIP);
	
	// ����ϵͳ����
	strSysVersion = GetSystemEditionString(emServerInfo.emSystemInfo.dwMajorVersion,emServerInfo.emSystemInfo.dwMinorVersion,emServerInfo.emSystemInfo.dwPlatformId);
	m_listComputer.SetItemText(nItem,nSub ++,strSysVersion);

	// CPU����
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szCPUType);

	// �ڴ��С
	strMemory.Format(_T("%d M"), emServerInfo.emSystemInfo.dwMemorySize);	
	m_listComputer.SetItemText(nItem,nSub ++,strMemory);	
	
	// ���������
	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szComputerName);
	
	// ��ǰ�û�����
//	m_listComputer.SetItemText(nItem,nSub ++,emServerInfo.emSystemInfo.szUserName);	

	// ����ͷ
	strCamera = emServerInfo.emSystemInfo.bHaveCamera ? _T("��") : _T("û��");
	m_listComputer.SetItemText(nItem,nSub ++,strCamera);
	
	// ����ʱ��
	strRunTime.Format("%d�� %dСʱ %d�� %d��",emServerInfo.emSystemInfo.nRunDays,emServerInfo.emSystemInfo.nRunHours,emServerInfo.emSystemInfo.nRunMinutes,emServerInfo.emSystemInfo.nRunSenconds);
	m_listComputer.SetItemText(nItem,nSub ++,strRunTime);
	
	// ����ʱ��
	strOnlineTime = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	m_listComputer.SetItemText(nItem,nSub ++,strOnlineTime);	

	return TRUE;
}
// ��ȡ����ϵͳ����
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
		strRet = "δ֪�汾";
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
// ��ȡ�б�ѡ����
int CClientDlg::GetListSelect()
{
	return m_listComputer.GetNextItem(-1,LVNI_SELECTED);
}
