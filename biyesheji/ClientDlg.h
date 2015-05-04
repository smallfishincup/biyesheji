
// ClientDlg.h : ͷ�ļ�
//

#pragma once


// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BIYESHEJI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	CXPListCtrl	m_listMsg;
	CXPListCtrl	m_listComputer;

	UINT m_nListenPort;
	// ���ɵ���Ϣӳ�亯��
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
// �������б������Ϣ
	BOOL AddMsgToComputerList(SERVER_REMOTE_S emServerInfo);
	CString GetSystemEditionString(DWORD dwMajorVersion,DWORD dwMinorVersion, DWORD dwPlatformId);
};
