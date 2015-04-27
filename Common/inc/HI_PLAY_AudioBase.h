// **************************************************************
// File		   : hi_play_audiobase.h
// Description : ÒôÆµ²Ù×÷»ùÀà
// Author	   :  
// Date		   : 
// Revisions   :
// **************************************************************

#if !defined(AFX_HI_PLAY_AUDIOBASE_H__8B56D711_FA5F_4AD8_B525_CDFBBF9CB422__INCLUDED_)
#define AFX_HI_PLAY_AUDIOBASE_H__8B56D711_FA5F_4AD8_B525_CDFBBF9CB422__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include "HI_PLAY_AudioBuffer.h"

#pragma comment(lib,"Winmm")

#ifndef PLAY_AV_1K
#define PLAY_AV_1K						1024
#endif

#ifndef PLAY_AUDIO_MAX_RB_SIZE
#define PLAY_AUDIO_MAX_RB_SIZE	        PLAY_AV_1K * 256    //
#endif

class CHI_PLAY_AudioBase  
{
public:
	CHI_PLAY_AudioBase();
	virtual ~CHI_PLAY_AudioBase();

	WAVEFORMATEX*	GetFormat();
	void			SetFormat(WAVEFORMATEX *format); 
	int				GetBufferSize();
	void			SetBufferSize(int NumberOfSamples);
 
	WORD			GetChannels();
	void			SetChannels(WORD wchannels);

	DWORD			GetSamplesPerSecond();
	void			SetSamplesPerSecond(DWORD dwSamples);

	WORD			GetBitsPerSample();
	void			SetBitsPerSample(WORD wBits);

	WORD			GetcbSize();
	void			SetcbSize(WORD wcbSize);
	
	BOOL			IsError(MMRESULT nResult);
	
protected:
	WAVEFORMATEX	m_Format;
	int				m_BufferSize;	// number of samples
	
private:
	void			Update();

};

#endif // !defined(AFX_HI_PLAY_AUDIOBASE_H__8B56D711_FA5F_4AD8_B525_CDFBBF9CB422__INCLUDED_)
