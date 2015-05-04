#pragma once


// CScreenDlg 对话框
class CClientDlg;
class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = NULL,CClientDlg* pClientDlg = NULL,SOCKET hSocket = INVALID_SOCKET);  // 标准构造函数
	virtual ~CScreenDlg();
	static UINT  __stdcall OnRecvData(BYTE* pRtpRecv,UINT nRecv,void* pRTPRecvWnd);
	
	BOOL		DecDraw(BYTE* pRTPData,int nRTPData,BOOL bVideoPacket);		
	
	BOOL		InitScreen();
// 对话框数据
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	BOOL		StartScreenThread(SOCKET hSocket);//开启接收数据线程
	BOOL		StopScreenThread();
	BOOL		AllocLocalDataPort(SOCKET &hSocketData,UINT &nLocalPort);//动态分配传输端口
	BOOL		ChangeFrameRate(UINT nFrameRate);	
	void		XorBuffer(PBYTE src, PBYTE dst, DWORD len);
	void		UpdateBuffer(PBYTE src, PBYTE dst, DWORD len, DWORD width);
	void		StartScreenTimer();
	void		StopScreenTimer();

	CPictureWnd						m_stVideo;
	CRect							m_rect;
	SOCKET							m_hConnectionSocket;
	CReceiveScreenThread*			m_pScreenThread;			// 查看屏幕线程
	CScreenSnap						m_Snap;

	CClientDlg*						m_pClientDlg;
	DWORD							m_dwFrameCount;				// 接收屏幕镇帧数
	PBYTE							m_pScreenBuffer;
	DWORD							m_dwScreenSize;
	PBYTE							m_pOriBuffer;
	UINT							m_nFrameRate;

	void ChangeSize(CWnd *pWnd, int cx, int cy);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};
