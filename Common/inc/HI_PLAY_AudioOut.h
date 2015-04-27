// **************************************************************
// File		   : hi_play_audioout.h
// Description : ÒôÆµÊä³öÀà
// Author	   :  
// Date		   : 
// Revisions   :
// **************************************************************

#if !defined(AFX_HI_PLAY_AUDIOOUT_H__F95B3637_F92C_4F66_A8C4_4CB552FDB8E9__INCLUDED_)
#define AFX_HI_PLAY_AUDIOOUT_H__F95B3637_F92C_4F66_A8C4_4CB552FDB8E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HI_PLAY_AudioBase.h" 
#include "HI_PLAY_RingBuffer.h"

class CHI_PLAY_AudioOut : public CHI_PLAY_AudioBase  
{
	friend void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
public:
	CHI_PLAY_AudioOut();
	virtual ~CHI_PLAY_AudioOut();
	BOOL Start(void);
	void Stop(void);

	void SetAudioDevice(CString strAudioDev){m_strAudioDev = strAudioDev;};
	
	BOOL IsExistDevice(void);
	WAVEOUTCAPS * GetDeviceCap(void);

	// pointer to callback function
	void (*GetDataToAudioOut)(CHI_PLAY_AudioBuffer *buffer, void *pOwner);
	void *m_pOwner;

	CHI_PLAY_RingBuffer m_Audio_RB;
	
private:
	BOOL OpenDevice(void);
	void CloseDevice(void);

    BOOL PerpareBuffer(void);
    void UnperpareBuffer(void);
	void CloseData(void);
	BOOL AddNewHeader(HWAVEOUT hwo, CHI_PLAY_AudioBuffer *buffer);
	BOOL AddNewHeader(HWAVEOUT hwo, CHI_PLAY_AudioBuffer *buffer, WAVEHDR *pWaveHead);
	
	BOOL		m_bDevOpen;
	BOOL		m_bBuffer;
	BOOL		m_bPlaying;
	
	WAVEOUTCAPS	m_waveCaps;
	HWAVEOUT	m_hWaveOut;
	WAVEHDR	    *m_pWaveHead;
	MMRESULT	m_mmr;
	int			m_nIndexWaveHdr;
	UINT		m_nReadLen;

	CString		m_strAudioDev;
};

#endif // !defined(AFX_HI_PLAY_AUDIOOUT_H__F95B3637_F92C_4F66_A8C4_4CB552FDB8E9__INCLUDED_)





















