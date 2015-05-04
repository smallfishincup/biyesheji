// ScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "biyesheji.h"
#include "ScreenDlg.h"
#include "afxdialogex.h"


// CScreenDlg 对话框

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/,CClientDlg* pClientDlg )
	: CDialogEx(CScreenDlg::IDD, pParent)
{

}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序
