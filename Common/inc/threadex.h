//***************************************************************************
//	Description		: Thread Class
//  File Name		: ThreadEx.h
//***************************************************************************

#if !defined(AFX_THREADEX_H__3BC1B255_F0E5_47D8_A09F_5D9E275424D9__INCLUDED_)
#define AFX_THREADEX_H__3BC1B255_F0E5_47D8_A09F_5D9E275424D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThreadEx
{
public:
	HANDLE m_hThread;
	unsigned int m_nThreadID;
public:
	CThreadEx();
	DWORD ResumeThread();
	DWORD SuspendThread();
	int   GetThreadPriority();
	BOOL  SetThreadPriority(int nPriority);
	BOOL  CreateThread(unsigned int dwCreateFlags,
					   UINT  nStackSize,
					   LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
	
	BOOL  ThreadRunning() {return (m_iSuspendCount == 0);};

	virtual BOOL KillThread();
	virtual BOOL InitInstance();
	virtual ~CThreadEx();

	virtual int  Run()			= 0;
	virtual int  ExitInstance() = 0;

public:
	int			m_iSuspendCount;	//Suspend Count;
	BOOL		m_bExit;			//�߳��˳���־λ
	HANDLE		m_hEvtExit;			//�߳��˳��¼�

protected:
	void	InitialRes();
};

#endif // !defined(AFX_THREADEX_H__3BC1B255_F0E5_47D8_A09F_5D9E275424D9__INCLUDED_)
