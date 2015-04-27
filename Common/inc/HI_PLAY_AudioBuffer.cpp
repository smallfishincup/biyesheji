#include "stdafx.h"
#include "HI_PLAY_AudioBuffer.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p != NULL) { delete (p);     (p) = NULL; } }   //Delete object by New create 
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHI_PLAY_AudioBuffer::CHI_PLAY_AudioBuffer(DWORD dwSize, BOOL bAutoDelete)
{
	m_bAutoDelete = bAutoDelete;
	try
	{
		ptr.b = new BYTE[dwSize];
		if (ptr.b) 
		{
			iByteLen = dwSize;
		}
	}
	catch(...)
	{
		AfxMessageBox("Out of memory!");
	}
}

CHI_PLAY_AudioBuffer::CHI_PLAY_AudioBuffer(void *buffer, DWORD dwlength)
{
   GetBuffer(buffer, dwlength);
}

void  CHI_PLAY_AudioBuffer::GetBuffer(void *buffer, DWORD dwlength)
{
	m_bAutoDelete = FALSE;
	if(buffer)
	{
		ptr.v = buffer;
		iByteLen = dwlength;
	}
	else
	{
		ptr.b = NULL;
		iByteLen = 0;
	}
}

CHI_PLAY_AudioBuffer::~CHI_PLAY_AudioBuffer()
{
	// remember to delete the memory
	if ((m_bAutoDelete) && (ptr.b != NULL))  
	{
		SAFE_DELETE(ptr.b);
	}
}

void CHI_PLAY_AudioBuffer::Erase()
{
	if (ptr.b)
	{
		ZeroMemory(ptr.b, iByteLen);
	}
}

short CHI_PLAY_AudioBuffer::GetUsedLength()
{
	return ptr.s[1]+2;
}
