// **************************************************************
// File		   : hi_play_ringbuffer.h
// Description : 音频缓冲类
// Author	   :  
// Date		   : 
// Revisions   :
// **************************************************************
#if !defined(AFX_HI_PLAY_RINGBUFFER_H__B935304D_CA29_4CE9_A292_EAD05A0F4364__INCLUDED_)
#define AFX_HI_PLAY_RINGBUFFER_H__B935304D_CA29_4CE9_A292_EAD05A0F4364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HI_PLAY_AudioBase.h"

typedef enum RB_STAT_ENUM
{
	E_RB_TotalReadError,
	E_RB_TotalWriteError,
		
	E_RB_TotalWriteSize,
	E_RB_TotalReadSize,
		
	E_RB_TotalReadTimes,
	E_RB_TotalWriteTimes,
		
	E_RB_STAT_END
}RB_STAT_E;

class CHI_PLAY_RingBuffer : public CHI_PLAY_AudioBase   
{
public:
	CHI_PLAY_RingBuffer();
	virtual ~CHI_PLAY_RingBuffer();

	VOID RB_Init_Stat();
	VOID RB_Do_Stat(RB_STAT_E, unsigned __int64 value);
	VOID RB_Disp_Stat();
	
	UINT RB_MaxReadSize();
	UINT RB_MaxWriteSize();
	UINT RB_MAXSize();

	int RB_Init (UINT ulSize);
	
	VOID RB_Clear();
	VOID RB_Destroy();
	
	int RB_Write(unsigned char *pData, UINT ulLen, UINT *pulWriteLen);
	int RB_Read(unsigned char *pData, UINT ulLen, UINT *pulRead);
	
	int RB_Write_X(unsigned char *pData, UINT ulDataLen, UINT *pulWriteLen);
	int RB_Read_X(unsigned char *pData, UINT *pulData);
	
	UINT RB_GetLen_X();
	unsigned char RB_GetFirstHI_U8();   /*从Read指针读取值*/
	unsigned short RB_GetFirstHI_U16(); /*2Byte*/
	UINT RB_GetFirstHI_U32();

	UINT RB_GetMaxWritePercent();
	UINT RB_GetMaxReadPercent();

private:
    UINT  m_ulSize;			/*Buffer 的总长度*/    
    unsigned char   *m_pRB;				/*Buffer的开始地址*/
    UINT  m_ulWritePos;		/*写的位置, 偏移位*/
    UINT  m_ulReadPos;		/*读的位置, 偏移位*/
	unsigned short	  m_usMediaType; 	    /*媒体类型*/ 
	unsigned __int64  m_stat[E_RB_STAT_END];
	
};

#endif // !defined(AFX_HI_PLAY_RINGBUFFER_H__B935304D_CA29_4CE9_A292_EAD05A0F4364__INCLUDED_)
