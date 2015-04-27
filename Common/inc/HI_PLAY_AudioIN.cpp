#include "stdafx.h"
#include "HI_PLAY_AudioIN.h"

#ifndef PLAY_AUDIO_BUF_NUM
#define PLAY_AUDIO_BUF_NUM		10
#endif

#ifndef PLAY_AUDIO_FRAME_SIZE
#define PLAY_AUDIO_FRAME_SIZE   960
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//DWORD CHI_PLAY_AudioIn::s_dwInstance = 0;

CHI_PLAY_AudioIn::CHI_PLAY_AudioIn(void)
{
	memset(&m_waveCaps,0,sizeof(m_waveCaps));

	m_bDevOpen	 = FALSE;
	m_bStartData = FALSE;
	m_bBuffer	 = FALSE;
	m_hWaveIn	 = 0;

	m_pWaveHead  = NULL;
	m_pOwner     = NULL;

	m_Audio_RB.RB_Init(PLAY_AUDIO_MAX_RB_SIZE);
	m_u32Len = 0;
	m_strAudioDev.Empty();
}

CHI_PLAY_AudioIn::~CHI_PLAY_AudioIn(void)
{
	Stop();//	FreeBuffer();
}

/************************************************ 
* Function Name	     : AudioInProc
* Description	     : Audio Callback Function 
* Return Type        : void CALLBACK 
* Parameters         : HWAVEIN hwi
* Parameters         : UINT uMsg
* Parameters         : DWORD dwInstance
* Parameters         : DWORD dwParam1
* Parameters         : DWORD dwParam2
* Last Modified      : 2006-4-30 9:17:15
************************************************/
void CALLBACK AudioInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch ( uMsg ) 
	{
	case MM_WIM_DATA:
		{		
			WAVEHDR *pWaveHdr = (WAVEHDR *)dwParam1;
			CHI_PLAY_AudioIn *pAudioIn = (CHI_PLAY_AudioIn *)(pWaveHdr->dwUser);

			if (pAudioIn->m_bStartData) 
			{
				if (pWaveHdr && hwi) 
				{
					if (pWaveHdr->dwFlags & WHDR_DONE == WHDR_DONE) 
					{
						pWaveHdr->dwFlags = 0;
						if (pAudioIn->IsError(waveInUnprepareHeader(hwi, pWaveHdr, sizeof(WAVEHDR))))
						{
							break;
						}
						if (pWaveHdr->dwBytesRecorded > 0) 
						{
							CHI_PLAY_AudioBuffer buff(pWaveHdr->lpData, pWaveHdr->dwBytesRecorded);
							//pAudioIn->DataFromAudioIn(&buff, pAudioIn->m_pOwner);
							pAudioIn->m_Audio_RB.RB_Write_X(buff.ptr.u8, buff.iByteLen, &(pAudioIn->m_u32Len));
							//Add new buffer
							waveInPrepareHeader(hwi, pWaveHdr, sizeof(WAVEHDR));
							waveInAddBuffer(hwi, pWaveHdr, sizeof(WAVEHDR));				
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

}

BOOL CHI_PLAY_AudioIn::OpenDevice(void)
{
	if ( m_bDevOpen )
	{
		return FALSE;
	}

	UINT nDevice = WAVE_MAPPER;
	WAVEINCAPS wic;		
	int nDeviceCount = waveInGetNumDevs();
	for(int nIndex = 0; nIndex < nDeviceCount; nIndex ++)
	{
		waveInGetDevCaps(nIndex, &wic, sizeof(wic));
		if(strstr(wic.szPname, m_strAudioDev) != NULL) 
		{
			nDevice = nIndex;
			break;
		} 
	}
	
	m_mmr = waveInOpen(0, nDevice, &m_Format, 0, 0, WAVE_FORMAT_QUERY);
	if ( MMSYSERR_NOERROR != m_mmr )
	{
		return FALSE;
	}
	
	m_mmr = waveInOpen(&m_hWaveIn, nDevice, &m_Format, (DWORD)AudioInProc, 0, CALLBACK_FUNCTION);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return FALSE;
	}
	
	m_bDevOpen = TRUE;

	return TRUE;
}

void CHI_PLAY_AudioIn::CloseDevice(void)
{
	if (!m_bDevOpen)
	{
		return;
	}
	
	if(!m_hWaveIn)
	{
		return;
	}

	m_mmr = waveInClose(m_hWaveIn);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		TRACE("waveInClose Error\n");
		return;
	}
	
	m_hWaveIn = 0;
	m_bDevOpen = FALSE;
}

BOOL CHI_PLAY_AudioIn::PerpareBuffer(void)
{
	if (m_bBuffer)
	{
		return FALSE;
	}
	
	m_mmr = waveInReset(m_hWaveIn);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		DT("waveInReset Error");
		return FALSE;
	}
	
	if( m_pWaveHead == NULL )
	{
		m_pWaveHead = new WAVEHDR[PLAY_AUDIO_BUF_NUM];
		
		for(int i=0; i < PLAY_AUDIO_BUF_NUM; i++ )
		{
			ZeroMemory(&m_pWaveHead[i],sizeof(WAVEHDR));
			
			CHI_PLAY_AudioBuffer buff(PLAY_AUDIO_FRAME_SIZE, FALSE);//m_Format.nBlockAlign * m_BufferSize
			m_pWaveHead[i].lpData = buff.ptr.c;
			m_pWaveHead[i].dwBufferLength = buff.iByteLen;
			m_pWaveHead[i].dwFlags = 0;
			m_pWaveHead[i].dwUser = (DWORD)(void *)this;
			waveInPrepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
			waveInAddBuffer(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
		}
	}
	else
	{
		for(int i=0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
		{
			waveInPrepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
			waveInAddBuffer(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
		}
	}
	m_bBuffer = TRUE;
	return TRUE;
}

void CHI_PLAY_AudioIn::UnperpareBuffer(void)
{
	if (!m_bBuffer)
	{
		return;
	}
	
	if( m_pWaveHead == NULL )
	{
		return;
	}
	
	for(int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
	{
		m_mmr = waveInUnprepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
		if( MMSYSERR_NOERROR != m_mmr )
		{
			DT("waveInUnprepareHeader Error\n");
			continue;
		}		
	}

	m_bBuffer = FALSE;
}

void CHI_PLAY_AudioIn::FreeBuffer(void)
{
	if( m_pWaveHead == NULL )
	{
		return;
	}
	
	for(int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
	{
		SAFE_DELETEA(m_pWaveHead[i].lpData);
	}
	SAFE_DELETEA(m_pWaveHead);
}

BOOL CHI_PLAY_AudioIn::Start(void)
{
	BOOL bRet=FALSE;
	if( !OpenDevice() )
	{
		goto Exit;
	}
	if ( !PerpareBuffer() )
	{
		goto Exit1;
	}

	if ( !StartData() ) 
	{
		goto Exit2;
	}
	
	bRet = TRUE;
	goto Exit;
Exit2:
	UnperpareBuffer();
Exit1:
	CloseDevice ();
Exit:
	return bRet;
}

void CHI_PLAY_AudioIn::Stop(void)
{
	CloseData();
	UnperpareBuffer();
	CloseDevice();
	FreeBuffer();
}

BOOL CHI_PLAY_AudioIn::StartData(void)
{
	if (m_bStartData)
	{
		return FALSE;
	}
	
	if(!m_hWaveIn)
	{
		return FALSE;
	}
	
	m_mmr = waveInStart(m_hWaveIn);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		DT("waveInStart Error");
		return FALSE;
	}
	
	m_bStartData = TRUE;
	return TRUE;
}

void CHI_PLAY_AudioIn::CloseData(void)
{
	if ( !m_bStartData )
	{
		return;
	}
	
	if( !m_hWaveIn )
	{
		return;
	}
	
	m_bStartData = FALSE;			 //important	
	m_mmr = waveInReset(m_hWaveIn);  //m_mmr = waveInStop(m_hWaveIn);
	if( MMSYSERR_NOERROR != m_mmr )
	{
		DT("waveInReset Error");
	}
}

BOOL CHI_PLAY_AudioIn::IsExistDevice(void)
{
	if( waveInGetNumDevs() <= 0 )
	{
		return FALSE;
	}
	return TRUE;
}

WAVEINCAPS* CHI_PLAY_AudioIn::GetDeviceCap(void)
{
	UINT nDevice = WAVE_MAPPER;
	WAVEINCAPS wic;		
	int nDeviceCount = waveInGetNumDevs();
	for(int nIndex = 0; nIndex < nDeviceCount; nIndex ++)
	{
		waveInGetDevCaps(nIndex, &wic, sizeof(wic));
		if(strstr(wic.szPname, m_strAudioDev) != NULL) 
		{
			nDevice = nIndex;
			break;
		} 
	}

	m_mmr = waveInGetDevCaps(nDevice,&m_waveCaps,sizeof(m_waveCaps));
	if( MMSYSERR_NOERROR != m_mmr )
	{
		return &m_waveCaps;
	}
	return NULL;
}


BOOL CHI_PLAY_AudioIn::IsOpen(void)
{
	return m_bDevOpen;
}


