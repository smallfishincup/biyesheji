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
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
		
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	 
}
