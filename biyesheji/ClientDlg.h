
// ClientDlg.h : 头文件
//

#pragma once


// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BIYESHEJI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CXPListCtrl	m_listMsg;
	CXPListCtrl	m_listComputer;

	UINT m_nListenPort;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListenConnectionThread*  m_pListenThread;
	afx_msg void OnBnClickedMonitor();
	afx_msg void OnBnClickedMakeserver();
	afx_msg LRESULT OnConnectClient(WPARAM wParam,LPARAM lParam);
private:
	void InitControls(void);
	BOOL StartListenThread();
	BOOL StopListenThread();
	BOOL RefreshComputerList();
	void AddMsgToList(CString strMsg);
	SOCKET GetSelectComputerSocket();
	int GetListSelect();
// 向计算机列表添加信息
	BOOL AddMsgToComputerList(SERVER_REMOTE_S emServerInfo);
	CString GetSystemEditionString(DWORD dwMajorVersion,DWORD dwMinorVersion, DWORD dwPlatformId);
};
