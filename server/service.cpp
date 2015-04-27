#include "stdafx.h"
#include "server.h"
#include "serverDlg.h"

extern CserverApp theApp;
void ServiceStart() {
	 
	CserverDlg dlg;
	theApp.m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
		
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	 
}
