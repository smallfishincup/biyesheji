#include "stdafx.h"
#include "HI_PLAY_AudioOut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifndef PLAY_AUDIO_BUF_NUM		
#define PLAY_AUDIO_BUF_NUM		5
#endif

#ifndef PLAY_AUDIO_FRAME_SIZE
#define PLAY_AUDIO_FRAME_SIZE   960
#endif

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(p) { if(p != NULL) { delete[] (p);   (p) = NULL; } }   //Delete Arrary
#endif

CHI_PLAY_AudioOut::CHI_PLAY_AudioOut()
{
	memset(&m_waveCaps,0,sizeof(m_waveCaps));
	
	m_bDevOpen	 = FALSE;
	m_bBuffer	 = FALSE;
	m_bPlaying	 = FALSE;
	m_hWaveOut   = 0;
	
	m_pOwner     = NULL;
	m_pWaveHead  = NULL;
	
	m_Audio_RB.RB_Init(PLAY_AUDIO_MAX_RB_SIZE);
	m_nReadLen   = 0;
	m_strAudioDev.Empty();
}

CHI_PLAY_AudioOut::~CHI_PLAY_AudioOut()
{
	Stop(); 
}

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch ( uMsg ) 
	{
	case WOM_DONE:
		{		
			WAVEHDR *pWaveHdr = (WAVEHDR *)dwParam1;
			CHI_PLAY_AudioOut *pAudioOut = (CHI_PLAY_AudioOut *)(pWaveHdr->dwUser);
			if (TRUE == pAudioOut->m_bPlaying) 
			{
				if (pWaveHdr && hwo) 
				{
					//pWaveHdr->dwFlags = 0;
					if (pAudioOut->IsError(waveOutUnprepareHeader(hwo, pWaveHdr, sizeof(WAVEHDR))))
					{
						break;
					}
					
					//pAudioOut->GetDataToAudioOut(&buff, pAudioOut->m_pOwner);
					CHI_PLAY_AudioBuffer buff(pWaveHdr->lpData, pWaveHdr->dwBufferLength);

					if (0 == pAudioOut->m_Audio_RB.RB_Read_X(buff.ptr.u8, &(pAudioOut->m_nReadLen)))
					{
						buff.iByteLen = pAudioOut->m_nReadLen;

						//如果没有数据就播放上一帧的数据
						pAudioOut->AddNewHeader(hwo, &buff, pWaveHdr);
					}	
				}
			}
		}
		break;
	default:
		break;
	}
	
}

BOOL CHI_PLAY_AudioOut::OpenDevice()
{
	if( m_bDevOpen )
	{
		return FALSE;
	}
	
	UINT nDevice = WAVE_MAPPER;
	WAVEOUTCAPS wic;		
	int nDeviceCount = waveOutGetNumDevs();
	for(int nIndex = 0; nIndex < nDeviceCount; nIndex ++)
	{
		waveOutGetDevCaps(nIndex, &wic, sizeof(wic));
		if(strstr(wic.szPname, m_strAudioDev) != NULL) 
		{
			nDevice = nIndex;
			break;
		} 
	}
	
	m_mmr = waveOutOpen(0,nDevice,&m_Format,0,0,WAVE_FORMAT_QUERY);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return FALSE;
	}
	
	m_mmr = waveOutOpen(&m_hWaveOut,WAVE_MAPPER,&m_Format,(DWORD)waveOutProc,(DWORD)this,CALLBACK_FUNCTION);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return FALSE;
	}
	
	m_bDevOpen = TRUE;
	
	return TRUE;
}

void CHI_PLAY_AudioOut::CloseDevice()
{
	if (!m_bDevOpen)
	{
		return;
	}
	
	if(!m_hWaveOut)
	{
		return;
	}
	
	m_mmr = waveOutClose(m_hWaveOut);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		DW("waveOutClose Error\n");
		return;
	}
	m_hWaveOut = 0;
	m_bDevOpen = FALSE;
}

void CHI_PLAY_AudioOut::CloseData()
{
	if(!m_hWaveOut)
	{
		return;
	}
	
	m_mmr = waveOutReset(m_hWaveOut);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		DW("waveOutReset Error\n");
		return;
	}
}

void CHI_PLAY_AudioOut::UnperpareBuffer()
{
	if (!m_bBuffer)
	{
		return;
	}
	
	for(int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
	{
		m_mmr = waveOutUnprepareHeader(m_hWaveOut,&m_pWaveHead[i],sizeof(WAVEHDR));
		if( MMSYSERR_NOERROR != m_mmr )
		{
			DW("waveOutUnprepareHeader Error\n");
			continue;
		}
		SAFE_DELETEA(m_pWaveHead[i].lpData);
	}
	SAFE_DELETEA(m_pWaveHead);

	m_bBuffer = FALSE;
}

void CHI_PLAY_AudioOut::Stop()
{
	if ( m_bPlaying ) 
	{
		m_bPlaying = FALSE;
		Sleep(50);
		CloseData();
		Sleep(10);
		UnperpareBuffer();
		CloseDevice();
	}
}

BOOL CHI_PLAY_AudioOut::Start()
{
	if( !OpenDevice() )
	{
		return FALSE;
	}

	if ( !PerpareBuffer() ) 
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CHI_PLAY_AudioOut::PerpareBuffer()
{
	if (m_bBuffer)
	{
		return FALSE;
	}
	
	m_mmr = waveOutReset(m_hWaveOut);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return FALSE;
	}
	
	m_nIndexWaveHdr = PLAY_AUDIO_BUF_NUM - 1;
	if( m_pWaveHead == NULL )
	{
		//create buffers
		m_pWaveHead = new WAVEHDR[PLAY_AUDIO_BUF_NUM];
		for (int i = 0; i < PLAY_AUDIO_BUF_NUM; i++) 
		{
			CHI_PLAY_AudioBuffer buff(PLAY_AUDIO_FRAME_SIZE, FALSE);//m_Format.nBlockAlign * m_BufferSize
			//get Audio file data
			if (0 == m_Audio_RB.RB_Read_X(buff.ptr.u8, &m_nReadLen))
			{
				buff.iByteLen = m_nReadLen;
			}
			//GetDataToAudioOut(&buff, m_pOwner);
			if ( !AddNewHeader(m_hWaveOut, &buff) ) 
			{
				return FALSE;
			}
		}
	}

	m_bPlaying = TRUE;
	m_bBuffer = TRUE;
	return TRUE;
}

BOOL CHI_PLAY_AudioOut::AddNewHeader(HWAVEOUT hwo, CHI_PLAY_AudioBuffer *buffer)
{
	m_nIndexWaveHdr = (m_nIndexWaveHdr == PLAY_AUDIO_BUF_NUM - 1) ? 0 : m_nIndexWaveHdr + 1;
	m_pWaveHead[m_nIndexWaveHdr].lpData = buffer->ptr.c;
	m_pWaveHead[m_nIndexWaveHdr].dwBufferLength = buffer->iByteLen;
	m_pWaveHead[m_nIndexWaveHdr].dwFlags = 0;
    m_pWaveHead[m_nIndexWaveHdr].dwLoops = 1;
	m_pWaveHead[m_nIndexWaveHdr].dwUser = (DWORD)(void*)this;
	if (IsError(waveOutPrepareHeader(hwo, &m_pWaveHead[m_nIndexWaveHdr], sizeof(WAVEHDR)))) 
	{
		return FALSE;
	}
	if (IsError(waveOutWrite(hwo, &m_pWaveHead[m_nIndexWaveHdr], sizeof(WAVEHDR)))) 
	{ 
		waveOutUnprepareHeader(hwo, &m_pWaveHead[m_nIndexWaveHdr], sizeof(WAVEHDR));
		m_pWaveHead[m_nIndexWaveHdr].lpData = NULL;
		m_pWaveHead[m_nIndexWaveHdr].dwBufferLength = 0;
		m_pWaveHead[m_nIndexWaveHdr].dwFlags = 0;
		m_pWaveHead[m_nIndexWaveHdr].dwUser = NULL;
		m_nIndexWaveHdr--;
		return FALSE;
	}
	return TRUE;
}

BOOL CHI_PLAY_AudioOut::AddNewHeader(HWAVEOUT hwo, CHI_PLAY_AudioBuffer *buffer, WAVEHDR *pWaveHead)
{
	pWaveHead->lpData = buffer->ptr.c;
	pWaveHead->dwBufferLength = buffer->iByteLen;
	pWaveHead->dwFlags = 0;
	pWaveHead->dwLoops = 1;
	pWaveHead->dwUser = (DWORD)(void*)this;
	if (IsError(waveOutPrepareHeader(hwo, pWaveHead, sizeof(WAVEHDR)))) 
	{
		return FALSE;
	}
	if (IsError(waveOutWrite(hwo, pWaveHead, sizeof(WAVEHDR)))) 
	{ 
		waveOutUnprepareHeader(hwo, pWaveHead, sizeof(WAVEHDR));
		pWaveHead->lpData = NULL;
		pWaveHead->dwBufferLength = 0;
		pWaveHead->dwFlags = 0;
		pWaveHead->dwUser = NULL;
		return FALSE;
	}
	return TRUE;
}

WAVEOUTCAPS* CHI_PLAY_AudioOut::GetDeviceCap()
{
	UINT nDevice = WAVE_MAPPER;
	WAVEOUTCAPS wic;		
	int nDeviceCount = waveOutGetNumDevs();
	for(int nIndex = 0; nIndex < nDeviceCount; nIndex ++)
	{
		waveOutGetDevCaps(nIndex, &wic, sizeof(wic));
		if(strstr(wic.szPname, m_strAudioDev) != NULL) 
		{
			nDevice = nIndex;
			break;
		} 
	}
	
	m_mmr = waveOutGetDevCaps(nDevice,&m_waveCaps,sizeof(m_waveCaps));
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return &m_waveCaps;
	}
	return NULL;
}

BOOL CHI_PLAY_AudioOut::IsExistDevice()
{
	if ( waveOutGetNumDevs() <= 0 )
	{
		return FALSE;
	}
	return TRUE;
}