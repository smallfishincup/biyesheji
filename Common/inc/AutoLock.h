#ifndef _AUTO_LOCK_H
#define _AUTO_LOCK_H

#include <afxmt.h>

class CAutoLock
{
public:
	CAutoLock(CSyncObject *pso)
	{
		m_pso = pso;
		m_pso->Lock();
	}
	virtual ~CAutoLock()
	{
		if(m_pso)
		{
			m_pso->Unlock();
		}
	}
	
	BOOL Attach(CSyncObject *pso)
	{
		if(m_pso)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		m_pso = pso;
	}
	
	void Deatch()
	{
		ASSERT(m_pso);
		m_pso = NULL;
	}

protected:
	CSyncObject* m_pso;
};

#endif

