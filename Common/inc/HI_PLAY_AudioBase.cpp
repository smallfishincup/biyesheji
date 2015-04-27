#include "stdafx.h"
#include "HI_PLAY_AudioBase.h"

#define PLAY_AUDIO_SAMPLE_POOR		8000
#define PLAY_AUDIO_SAMPLE_LOW		11025
#define PLAY_AUDIO_SAMPLE_NORMAL	22050
#define PLAY_AUDIO_SAMPLE_HIGH		44100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHI_PLAY_AudioBase::CHI_PLAY_AudioBase()
{
	m_BufferSize			= 480;  // samples per callback
	m_Format.wFormatTag		= WAVE_FORMAT_PCM;
	m_Format.wBitsPerSample = 16;
    m_Format.nSamplesPerSec = PLAY_AUDIO_SAMPLE_POOR;
	m_Format.nChannels		= 1;
	m_Format.cbSize			= 0;
	
	Update();
}

CHI_PLAY_AudioBase::~CHI_PLAY_AudioBase()
{
}

void CHI_PLAY_AudioBase::SetBitsPerSample(WORD wBits)
{
	m_Format.wBitsPerSample = wBits;
	Update();
}

WORD CHI_PLAY_AudioBase::GetBitsPerSample()
{
	return m_Format.wBitsPerSample;	
}

void CHI_PLAY_AudioBase::SetSamplesPerSecond(DWORD dwSamples)
{
	m_Format.nSamplesPerSec = dwSamples;
	Update();
}

DWORD CHI_PLAY_AudioBase::GetSamplesPerSecond()
{
	return m_Format.nSamplesPerSec;
}

void CHI_PLAY_AudioBase::SetChannels(WORD wchannels)
{
	m_Format.nChannels = wchannels;
	Update();
}

WORD CHI_PLAY_AudioBase::GetChannels()
{
	return m_Format.nChannels;
}

void CHI_PLAY_AudioBase::SetcbSize(WORD wcbSize)
{
	m_Format.cbSize = wcbSize;
	Update();
}

WORD CHI_PLAY_AudioBase::GetcbSize()
{
	return m_Format.cbSize;
}

void CHI_PLAY_AudioBase::Update()
{
	m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * (m_Format.wBitsPerSample / 8);
	m_Format.nBlockAlign	 = m_Format.nChannels      * (m_Format.wBitsPerSample / 8);
}

void CHI_PLAY_AudioBase::SetBufferSize(int NumberOfSamples)
{
	m_BufferSize = NumberOfSamples;
}

int CHI_PLAY_AudioBase::GetBufferSize()
{
	return m_BufferSize;
}

WAVEFORMATEX* CHI_PLAY_AudioBase::GetFormat()
{
	return &m_Format;
}

void CHI_PLAY_AudioBase::SetFormat(WAVEFORMATEX *format)
{
	m_Format = *format;
}

BOOL CHI_PLAY_AudioBase::IsError(MMRESULT nResult)
{
	UINT m_nError = nResult;
	
	if ( MMSYSERR_NOERROR == m_nError ) 
	{
		return FALSE;
	} 
	else 
	{
		return TRUE;
	}
}
