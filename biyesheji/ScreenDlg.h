#pragma once


// CScreenDlg 对话框
class CClientDlg;
class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = NULL,CClientDlg* pClientDlg = NULL);  // 标准构造函数
	virtual ~CScreenDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
