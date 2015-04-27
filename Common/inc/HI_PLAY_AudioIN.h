// **************************************************************
// File		   : hi_play_audioin.h
// Description : ÒôÆµÊäÈëÀà
// Author	   :  
// Date		   : 
// Revisions   :
// **************************************************************

#if !defined(AFX_HI_PLAY_AUDIOIN_H__7C96B411_7A43_4235_A5D0_7BFCCA58D92B__INCLUDED_)
#define AFX_HI_PLAY_AUDIOIN_H__7C96B411_7A43_4235_A5D0_7BFCCA58D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HI_PLAY_AudioBase.h" 
#include "HI_PLAY_RingBuffer.h"

class CHI_PLAY_AudioIn  : public CHI_PLAY_AudioBase 
{
	friend void CALLBACK AudioInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

public:
	CHI_PLAY_AudioIn(void);
	virtual ~CHI_PLAY_AudioIn(void);

	void SetAudioDevice(CString strAudioDev){m_strAudioDev = strAudioDev;};	
	BOOL Start(void); 
	void Stop(void);
	BOOL IsOpen(void);

	BOOL IsExistDevice(void);
	WAVEINCAPS * GetDeviceCap(void);

	// pointer to callback function
	void (*DataFromAudioIn)(CHI_PLAY_AudioBuffer *buffer, void *pOwner);
	void *				m_pOwner;
	CHI_PLAY_RingBuffer m_Audio_RB;
	UINT				m_u32Len;
		
private:
	BOOL		OpenDevice(void);
	void		CloseDevice(void);
    BOOL		PerpareBuffer(void);
    void		UnperpareBuffer(void);
	BOOL		StartData(void);
	void		CloseData(void);
	void		FreeBuffer(void);
	
protected:
	BOOL		m_bDevOpen;
	BOOL		m_bStartData;
	BOOL		m_bBuffer;

	WAVEINCAPS 	m_waveCaps;
	HWAVEIN		m_hWaveIn;
	WAVEHDR		*m_pWaveHead;
	MMRESULT	m_mmr;
	CString		m_strAudioDev;	
};

#endif // !defined(AFX_HI_PLAY_AUDIOIN_H__7C96B411_7A43_4235_A5D0_7BFCCA58D92B__INCLUDED_)
