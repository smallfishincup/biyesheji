
// server.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CserverApp:
// �йش����ʵ�֣������ server.cpp
//

class CserverApp : public CWinApp
{
public:
	CserverApp();
	CString SZSERVICENAME;
	CString  SZSERVICEDISPLAYNAME;
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

protected:
	HANDLE		m_hAppMutex;
};

extern CserverApp theApp;