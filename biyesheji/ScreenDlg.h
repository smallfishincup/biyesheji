#pragma once


// CScreenDlg �Ի���
class CClientDlg;
class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = NULL,CClientDlg* pClientDlg = NULL);  // ��׼���캯��
	virtual ~CScreenDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
