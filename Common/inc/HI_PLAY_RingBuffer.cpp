#include "stdafx.h"
#include "HI_PLAY_RingBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p != NULL) { delete (p);     (p) = NULL; } }   //Delete object by New create 
#endif

#define RB_DO_STAT(idx, v)	 RB_Do_Stat(idx, v)
#define RB_INIT_STAT()       RB_Init_Stat()
#define RB_DISP_STAT()       RB_Disp_Stat()

#define RB_DO_STAT_TotalReadError(v) RB_DO_STAT(E_RB_TotalReadError, v)
#define RB_DO_STAT_TotalWriteError(v) RB_DO_STAT(E_RB_TotalWriteError, v)

#define RB_DO_STAT_TotalWriteSize(v) RB_DO_STAT(E_RB_TotalWriteSize, v)
#define RB_DO_STAT_TotalReadSize(v) RB_DO_STAT(E_RB_TotalReadSize, v)

#define RB_DO_STAT_TotalReadTimes(v) RB_DO_STAT(E_RB_TotalReadTimes, v)
#define RB_DO_STAT_TotalWriteTimes(v) RB_DO_STAT(E_RB_TotalWriteTimes, v)

#define RB_LEN_BYTES 4

typedef enum tag_MODULE
{
    MODULE_GENERAL = 0,
	MODULE_SYS     = 1 ,  /*系统模块*/           
	MODULE_DRV     = 2 ,  /*驱动模块*/
	MODULE_MMF     = 3,   /*MultiMedia Frame*/
	MODULE_APP     = 4,   /*应用模块*/
	MODULE_BUTT         /*结束标志*/
}MODULE_E;

typedef enum tag_SYS_SUB_MODULE
{
    SYS_SUB_MODULE_AZ   = 1, /*Arm ZSP Message Communication*/
	SYS_SUB_MODULE_RB   = 2, /*Ring Buffer */
	SYS_SUB_MODULE_BM   = 3, /*Buffer Manager*/
	SYS_SUB_MODULE_MQ   = 4, /*Message Queue*/
	SYS_SUB_MODULE_THREAD = 5,
	SYS_SUB_MODULE_BUTT
}SYS_SUB_MODULE;

/*
系统错误码的定义
31       24 23      16 15                   0
-----------------------------------------------
|    模块   |  子模块  |   错误码(子模块定义) |
-----------------------------------------------
*/


#define ERR_SUB_MODULE_BASE(module, submodule)    \
(( module << 24 ) | ( submodule << 16 )) 

#define ERR_NUMBER(err, base)          ( (base) | (err) ) 


/*****************************************************************************
通用模块错误代码定义
****************************************************************************/

/*0x00XX*/
#define ERR_GENERAL_BASE      ERR_SUB_MODULE_BASE(MODULE_GENERAL, 0)

#define ERR_GEN_NOT_ENOUGH_MEMORY ERR_NUMBER(2, ERR_GENERAL_BASE) /*0x0002*/
#define ERR_GEN_INVALID_POINTER   ERR_NUMBER(3, ERR_GENERAL_BASE) /*0x0003*/
#define ERR_GEN_OVERFLOW          ERR_NUMBER(4, ERR_GENERAL_BASE) /*0x0004*/
#define ERR_GEN_OUT_OF_BOUND      ERR_NUMBER(5, ERR_GENERAL_BASE) /*0x0004*/
#define ERR_GEN_INVALID_FP        ERR_NUMBER(6, ERR_GENERAL_BASE) /*0x0004*/

#define ERR_GEN_TIMEOUT           ERR_NUMBER(7, ERR_GENERAL_BASE) /*0x0007*/
#define ERR_GEN_INVALID_PARAMETER ERR_NUMBER(8, ERR_GENERAL_BASE) /*0x0008*/


/*****************************************************************************
驱动模块, 子模块错误代码定义
****************************************************************************/
#define ERR_DRV_VIU_BASE    ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_VIU)
#define ERR_DRV_VOU_BASE    ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_VOU)
#define ERR_DRV_DSU_BASE    ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_DSU)
#define ERR_DRV_USB_BASE    ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_USB)
#define ERR_DRV_SF_BASE     ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_SF)
#define ERR_DRV_TDE_BASE    ERR_SUB_MODULE_BASE(MODULE_DRV,  DRV_SUB_MODULE_TDE)

/* ERR_DRV_VOU_BASE */
#define ERR_DRV_VOU_ERROR         ERR_NUMBER(0, ERR_DRV_VOU_BASE)
#define ERR_DRV_VOU_NOT_INITED    ERR_NUMBER(1, ERR_DRV_VOU_BASE)
#define ERR_DRV_VOU_INIT_FAILED   ERR_NUMBER(2, ERR_DRV_VOU_BASE)
#define ERR_DRV_VOU_OUTPUT_MODE_NOT_SUPPORT  ERR_NUMBER(3, ERR_DRV_VOU_BASE)
#define ERR_DRV_VOU_LAYER_INVALID ERR_NUMBER(4, ERR_DRV_VOU_BASE)

/* ERR_DRV_VIU_BASE */
#define ERR_DRV_VIU_ERROR        ERR_NUMBER(0, ERR_DRV_VIU_BASE)
#define ERR_DRV_VIU_NOT_INITED   ERR_NUMBER(1, ERR_DRV_VIU_BASE)
#define ERR_DRV_VIU_INIT_FAILED  ERR_NUMBER(2, ERR_DRV_VIU_BASE)
#define ERR_DRV_VIU_MODE_NOT_SUPPORT   ERR_NUMBER(3, ERR_DRV_VIU_BASE)

/* ERR_DRV_DSU_BASE */
#define ERR_DRV_DSU_ERROR        ERR_NUMBER(0, ERR_DRV_DSU_BASE)
#define ERR_DRV_DSU_NOT_INITED   ERR_NUMBER(1, ERR_DRV_DSU_BASE)
#define ERR_DRV_DSU_INIT_FAILED  ERR_NUMBER(2, ERR_DRV_DSU_BASE)
#define ERR_DRV_DSU_FILTER_NOT_SUPPORT   ERR_NUMBER(3, ERR_DRV_DSU_BASE)

/* ERR_DRV_TDE_BASE */
#define ERR_DRV_TDE_ERROR        ERR_NUMBER(0, ERR_DRV_TDE_BASE)
#define ERR_DRV_TDE_NOT_INITED   ERR_NUMBER(1, ERR_DRV_TDE_BASE)
#define ERR_DRV_TDE_INIT_FAILED  ERR_NUMBER(2, ERR_DRV_TDE_BASE)
#define ERR_DRV_TDE_FIFO_TIMEOUT ERR_NUMBER(3, ERR_DRV_TDE_BASE)

/*****************************************************************************
系统模块,ARM ZSP 通讯子模块错误代码定义
****************************************************************************/
/*00000001-00000001-xxxxxxxx-xxxxxxxx  0x11XX*/
#define ERR_SYS_AZ_BASE ERR_SUB_MODULE_BASE(MODULE_SYS, SYS_SUB_MODULE_AZ)


#define ERR_AZMSG_INVALID_MSGPOOLS  ERR_NUMBER(1, ERR_SYS_AZ_BASE) /*0x1101*/
#define ERR_AZMSG_INVALID_MSG      ERR_NUMBER(2, ERR_SYS_AZ_BASE) /*0x1102*/
#define ERR_AZMSG_NO_SPACE_TOSEND  ERR_NUMBER(3, ERR_SYS_AZ_BASE) /*0x1103*/
#define ERR_AZMSG_NO_MSG_TORECV    ERR_NUMBER(4, ERR_SYS_AZ_BASE)
#define ERR_AZMSG_INVALID_MSGDATA  ERR_NUMBER(5, ERR_SYS_AZ_BASE) /*0x1105*/

#define ERR_AZMSG_OVERFLOW         ERR_NUMBER(6, ERR_SYS_AZ_BASE) /*0x1106*/
#define ERR_AZMSG_EMPTY            ERR_NUMBER(7, ERR_SYS_AZ_BASE) /*0x1107*/
#define ERR_AZMSG_READY_TIMEOUT    ERR_NUMBER(8, ERR_SYS_AZ_BASE) /*0x1108*/
#define ERR_AZMSG_POOL_OFFREAD     ERR_NUMBER(9, ERR_SYS_AZ_BASE) /*0x1109*/
#define ERR_AZMSG_POOL_OFFWRITE    ERR_NUMBER(10, ERR_SYS_AZ_BASE) /*0x1110*/

#define ERR_AZMSG_MSG_MAGIC        ERR_NUMBER(11, ERR_SYS_AZ_BASE) /*0x1111*/
#define ERR_AZMSG_MSG_LENGTH       ERR_NUMBER(12, ERR_SYS_AZ_BASE) /*0x1112*/
#define ERR_AZMSG_MSG_PID          ERR_NUMBER(13, ERR_SYS_AZ_BASE) /*0x1113*/
/*Ring Buffer*/
/*00000001-00000002-xxxxxxxx-xxxxxxxx  0x12XX*/
#define ERR_SYS_RB_BASE ERR_SUB_MODULE_BASE(MODULE_SYS, SYS_SUB_MODULE_RB)

#define ERR_RB_ERROR               ERR_NUMBER(0, ERR_SYS_RB_BASE) /*0x1201*/
#define ERR_RB_INIT                ERR_NUMBER(1, ERR_SYS_RB_BASE) /*0x1201*/
#define ERR_RB_OVERFLOW            ERR_NUMBER(2, ERR_SYS_RB_BASE) /*0x1202*/
#define ERR_RB_READ_NOTENOGH       ERR_NUMBER(3, ERR_SYS_RB_BASE) /*0x1203*/
#define ERR_RB_WRITE_NOTENOGH      ERR_NUMBER(4, ERR_SYS_RB_BASE) /*0x1204*/
#define ERR_RB_NULL_BUF            ERR_NUMBER(5, ERR_SYS_RB_BASE) /*0x1205*/
#define ERR_RB_DAMAGE              ERR_NUMBER(6, ERR_SYS_RB_BASE) /*0x1206*/

/*Buffer Manager*/
/*00000001-00000003-xxxxxxxx-xxxxxxxx  0x13XX*/
#define ERR_SYS_BM_BASE ERR_SUB_MODULE_BASE(MODULE_SYS, SYS_SUB_MODULE_BM)

#define ERR_BM_ERROR               ERR_NUMBER(0, ERR_SYS_BM_BASE) /*0x1301*/
#define ERR_BM_INIT                ERR_NUMBER(1, ERR_SYS_BM_BASE) /*0x1301*/
#define ERR_BM_OVERFLOW            ERR_NUMBER(2, ERR_SYS_BM_BASE) /*0x1302*/
#define ERR_BM_READ_NOTENOGH       ERR_NUMBER(3, ERR_SYS_BM_BASE) /*0x1303*/
#define ERR_BM_WRITE_NOTENOGH      ERR_NUMBER(4, ERR_SYS_BM_BASE) /*0x1304*/
#define ERR_BM_INVALIDLEN          ERR_NUMBER(5, ERR_SYS_BM_BASE) /*0x1304*/

/*Message Queue*/
/*00000001-00000004-xxxxxxxx-xxxxxxxx  0x14XX*/
#define ERR_SYS_MQ_BASE ERR_SUB_MODULE_BASE(MODULE_SYS, SYS_SUB_MODULE_MQ)
/*Message Queue's Normal Error*/
#define ERR_MQ_ERROR           ERR_NUMBER(0, ERR_SYS_MQ_BASE)
/*the Queue is invalid*/
#define ERR_MQ_QUEUE_INVALUD   ERR_NUMBER(1, ERR_SYS_MQ_BASE)
/*no enough space to write to queue*/
#define ERR_MQ_QUEUE_OVERFLOW  ERR_NUMBER(2, ERR_SYS_MQ_BASE)
/*the queue is empty to read */
#define ERR_MQ_QUEUE_EMPTY     ERR_NUMBER(3, ERR_SYS_MQ_BASE)
/*the message is invalid*/
#define ERR_MQ_MSG_INVALID     ERR_NUMBER(4, ERR_SYS_MQ_BASE)
/*the message length is null*/
#define ERR_MQ_MSG_LEN_INVALID ERR_NUMBER(5, ERR_SYS_MQ_BASE)
/*the buffer is invalid*/
#define ERR_MQ_BUFFER_INVALID  ERR_NUMBER(6, ERR_SYS_MQ_BASE)

/*thread manager*/
#define ERR_SYS_THREAD_BASE ERR_SUB_MODULE_BASE(MODULE_SYS, SYS_SUB_MODULE_THREAD)

#define ERR_SYS_THREAD_MUTEX_ERROR     ERR_NUMBER(0, ERR_SYS_THREAD_BASE)

/*------------- MMF Module Error Number Define  ---------------0x31xx----------*/
#define ERR_MMF_INPUT_BASE \
                ERR_SUB_MODULE_BASE(MODULE_MMF, MMF_SUB_MODULE_INPUT)

#define ERR_INPUT_STREAM_ERROR \
                ERR_NUMBER(1, ERR_MMF_INPUT_BASE) /*0x3102*/
#define ERR_INPUT_STREAM_NOTSUPPORT \
                ERR_NUMBER(2, ERR_MMF_INPUT_BASE) /*0x3102*/
#define ERR_INPUT_FILE_OPEN_ERROR  \
                ERR_NUMBER(3, ERR_MMF_INPUT_BASE) /*0x3102*/
                
/*------------- DEMUX Module Error Number Define  ---------------0x35xx--------*/
#define ERR_MMF_DEMUX_BASE \
                ERR_SUB_MODULE_BASE(MODULE_MMF, MMF_SUB_MODULE_DEMUX)

#define ERR_DEMUX_STREAM_END \
                ERR_NUMBER(1, ERR_MMF_DEMUX_BASE) 
#define ERR_DEMUX_STREAM_SEEK_FAILED \
                ERR_NUMBER(2, ERR_MMF_DEMUX_BASE) 
#define ERR_DEMUX_STREAM_SEEK_NOSUPPORT  \
                ERR_NUMBER(3, ERR_MMF_DEMUX_BASE) 
#define ERR_DEMUX_NO_DEMUX_INFO  \
                ERR_NUMBER(4, ERR_MMF_DEMUX_BASE)                 



#define ERR_MMF_STREAM_BASE \
                ERR_SUB_MODULE_BASE(MODULE_MMF, MMF_SUB_MODULE_STREAM)
                
#define ERR_STREAM_EMPTY    \
                ERR_NUMBER(1, MMF_SUB_MODULE_STREAM) /*0x3601*/

/*************** video decode error code define *******************/
#define ERR_MMF_VDEC_BASE \
                ERR_SUB_MODULE_BASE(MODULE_MMF, MMF_SUB_MODULE_VDEC)

#define ERR_MMF_VDEC_ERROR \
                ERR_NUMBER(0, ERR_MMF_VDEC_BASE)    /*0x3701*/

#define ERR_MMF_VDEC_HANDLE_INVALID \
                ERR_NUMBER(1, ERR_MMF_VDEC_BASE)

#define ERR_MMF_VDEC_DATA_INVALID \
                ERR_NUMBER(2, ERR_MMF_VDEC_BASE)

#define ERR_MMF_VDEC_DATATYPE_INVALID \
                ERR_NUMBER(3, ERR_MMF_VDEC_BASE)

#define ERR_MMF_VDEC_PAYTYPE_INVALID \
                ERR_NUMBER(4, ERR_MMF_VDEC_BASE)    

#define ERR_MMF_VDEC_FIRST_NALU_INVALID \
                ERR_NUMBER(5, ERR_MMF_VDEC_BASE)

/*************** jitter buffer error code define *******************/
#define ERR_MMF_JB_BASE \
                ERR_SUB_MODULE_BASE(MODULE_MMF, MMF_SUB_MODULE_JB)
/* #define ERR_MMF_JB_... */
/* 由于jitterbuffer中定义的错误代码较多，暂时先放在hi_jb_errorcode.h中 */


/*****************************************************************************
APP模块, 子模块错误代码定义
****************************************************************************/
#define ERR_APP_VPAI_BASE ERR_SUB_MODULE_BASE(MODULE_APP,  APP_SUB_MODULE_VAPI)

/* ERR_APP_VPAI_BASE */
#define ERR_APP_VAPI_ERROR         ERR_NUMBER(0, ERR_APP_VPAI_BASE)
#define ERR_APP_VAPI_NOT_INITED    ERR_NUMBER(1, ERR_APP_VPAI_BASE)
#define ERR_APP_VAPI_INIT_FAILED   ERR_NUMBER(2, ERR_APP_VPAI_BASE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHI_PLAY_RingBuffer::CHI_PLAY_RingBuffer()
{
	m_pRB = NULL;
    m_ulSize     = 0;
    m_ulReadPos  = 0; 
    m_ulWritePos = 0;
	RB_Init_Stat();
}

CHI_PLAY_RingBuffer::~CHI_PLAY_RingBuffer()
{
	RB_Destroy();
}

void CHI_PLAY_RingBuffer::RB_Init_Stat()
{
    memset(m_stat, 0, sizeof(m_stat));
}

void CHI_PLAY_RingBuffer::RB_Do_Stat(RB_STAT_E stattype, unsigned __int64 value)
{
	if (stattype >= 0  && stattype < E_RB_STAT_END)
	{
		if (m_stat[stattype] < 0xFFFFFFFFFFFFFFFF)
		{
			m_stat[stattype] += value;
		}
		else
		{
			m_stat[stattype] = 0;
		}
	}
	else
	{
		DW("stattype :%d out of bounds", stattype);
	}
}

void CHI_PLAY_RingBuffer::RB_Disp_Stat()
{
	DW("\nTotalReadError: %llu, TotalWriteError: %llu \n", m_stat[E_RB_TotalReadError], m_stat[E_RB_TotalWriteError]);
    DW("TotalWriteSize: %llu, TotalReadSize: %llu \n", m_stat[E_RB_TotalWriteSize], m_stat[E_RB_TotalReadSize]); 
	DW("TotalReadTimes: %llu, TotalWriteTimes: %llu \n", m_stat[E_RB_TotalReadTimes], m_stat[E_RB_TotalWriteTimes]);
}

/************************************************ 
* Function Name	     : CHI_PLAY_RingBuffer::RB_MaxWriteSize
* Description	     : 最大的可以写的缓冲区长度
* Return Type        : UINT 
* Parameters         : void
* Last Modified      : 2006-5-21 15:53:53
************************************************/
UINT CHI_PLAY_RingBuffer::RB_MaxWriteSize()
{
    UINT ulMaxWriteSize = 0;

    if(NULL == m_pRB)
    {
        return ERR_GEN_INVALID_POINTER;
    }

    /*READ == WRITE 认为是空缓冲*/
    if (m_ulReadPos == m_ulWritePos)
    {
        ulMaxWriteSize = m_ulSize ; 
    }

    if (m_ulReadPos > m_ulWritePos)
    {
        ulMaxWriteSize = m_ulReadPos - m_ulWritePos;
    }
    else
    {
        ulMaxWriteSize  = m_ulSize - m_ulWritePos + m_ulReadPos;
    }
    
    return ulMaxWriteSize;
}

/************************************************ 
* Function Name	     : CHI_PLAY_RingBuffer::RB_MaxReadSize
* Description	     : 最大的可以读出的缓冲区长度
* Return Type        : UINT 
* Parameters         : void
* Last Modified      : 2006-5-21 15:53:39
************************************************/
UINT CHI_PLAY_RingBuffer::RB_MaxReadSize()
{
    UINT  ulMaxReadSize = 0;

    if( NULL == m_pRB )
    {
        return ERR_GEN_INVALID_POINTER;
    }

    /*认为缓冲是空的, 所以, 可以读出的数据为0*/
    if( m_ulReadPos == m_ulWritePos)
    {
        ulMaxReadSize = 0;
    }
    else if( m_ulReadPos < m_ulWritePos)
    {
        ulMaxReadSize = m_ulWritePos - m_ulReadPos;
    }
    else if( m_ulReadPos > m_ulWritePos)
    {
        ulMaxReadSize = m_ulSize - m_ulReadPos + m_ulWritePos;
    }
    return ulMaxReadSize;
}

UINT CHI_PLAY_RingBuffer::RB_MAXSize()
{
	return m_ulSize;
}

int CHI_PLAY_RingBuffer::RB_Init(IN  UINT ulSize)
{
	if (ulSize <= 0) 
	{
		return ERR_RB_INIT;
	}

	RB_Destroy();
	m_pRB = new BYTE[ ulSize ];
		
    m_ulSize     = ulSize;
    m_ulReadPos  = 0; 
    m_ulWritePos = 0;
    
    memset(m_pRB, 0, ulSize);

    RB_INIT_STAT();
 
	return 0;
}

void CHI_PLAY_RingBuffer::RB_Clear()
{
    m_ulReadPos = m_ulWritePos ; 
}

void CHI_PLAY_RingBuffer::RB_Destroy()
{
    if (NULL != m_pRB)
	{
		SAFE_DELETE(m_pRB);
	}
}

int CHI_PLAY_RingBuffer::RB_Write(BYTE *pData, UINT ulLen, UINT *pulWriteLen)
{
    UINT ulMaxWriteSize = 0;
    UINT ulWriteSize = ulLen; /*真实写入的数据*/

    /*写需要分段*/
    UINT ulFirstChunkSize = 0;
    UINT ulSecondChunkSize = 0;

    RB_DO_STAT_TotalWriteTimes(1);
    
    ulMaxWriteSize = RB_MaxWriteSize();
    
    if (ulWriteSize >=ulMaxWriteSize )
    {
        RB_DO_STAT_TotalWriteError(1);
        return ERR_RB_WRITE_NOTENOGH;
    }
    
    // easy case, no wrapping
    if( m_ulWritePos + ulWriteSize < m_ulSize )
    {
        memcpy( (m_pRB + m_ulWritePos), pData, ulWriteSize);
        m_ulWritePos += ulWriteSize;
    }
    else // harder case we need to wrap
    {
        ulFirstChunkSize  = m_ulSize - m_ulWritePos;
        ulSecondChunkSize = ulWriteSize - ulFirstChunkSize;

        memcpy( (m_pRB + m_ulWritePos), pData, ulFirstChunkSize );
        memcpy( m_pRB, (pData + ulFirstChunkSize), ulSecondChunkSize );
		
        m_ulWritePos = ulSecondChunkSize;
    }

    if (pulWriteLen != NULL)
    {
        *pulWriteLen = ulWriteSize;
    }

    RB_DO_STAT_TotalWriteSize(ulWriteSize);
    return 0;
}

int CHI_PLAY_RingBuffer::RB_Read(BYTE *pData, UINT ulLen, UINT *pulRead)
{
    UINT ulMaxReadSize = 0;
    UINT ulReadSize = 0; /*真实写入的数据*/

    /*写需要分段*/
    UINT ulFirstChunkSize = 0;
    UINT ulSecondChunkSize = 0;
    
    ulMaxReadSize = RB_MaxReadSize();

    RB_DO_STAT_TotalReadTimes(1);
    ulReadSize = ulLen;

    if (ulReadSize > ulMaxReadSize)
    {
        RB_DO_STAT_TotalReadError(1);
        return ERR_RB_READ_NOTENOGH;
    }
    
    // easy case, no wrapping
    if( m_ulReadPos + ulReadSize < m_ulSize)
    {
        memcpy( pData, (m_pRB + m_ulReadPos), ulReadSize);
        m_ulReadPos += ulReadSize;
    }
    else // harder case, buffer wraps
    {
        ulFirstChunkSize  = m_ulSize - m_ulReadPos;
        ulSecondChunkSize = ulReadSize - ulFirstChunkSize;

        memcpy( pData, (m_pRB + m_ulReadPos), ulFirstChunkSize );
        memcpy( pData + ulFirstChunkSize, m_pRB, ulSecondChunkSize );

        m_ulReadPos = ulSecondChunkSize;
    }

    if (pulRead)
    {
        *pulRead = ulReadSize;
    }
    RB_DO_STAT_TotalReadSize(ulReadSize);
    
    return 0;
}

#ifndef ALIGN_NEXT
#define ALIGN_NEXT(v,a) ((((v) + ((a)-1)) & (~((a)-1))))
#endif

#ifndef ALIGN_LENGTH
#define ALIGN_LENGTH(l, a) ALIGN_NEXT(l, a)
#endif

/*往RB写pData, 会在前面附加4Bytes的长度指示, 以4Byte为单位对齐*/
int CHI_PLAY_RingBuffer::RB_Write_X(BYTE *pData, UINT ulDataLen, UINT *pulWriteLen)
{
    UINT ulMaxWriteSize = 0;

    /*Data对齐后的长度*/
    UINT lenAlign = ALIGN_LENGTH(ulDataLen, RB_LEN_BYTES);
    
    /*真实写入的长度*/
    UINT ulWriteSize =  lenAlign + RB_LEN_BYTES ; 
    
    if(ulDataLen == 0)
	{
        CString str;
        RB_DO_STAT_TotalWriteError(1);
        str.Format("RB WR_ERR : ulDataLen=%d.", ulDataLen);
		DW(str);
        return ERR_RB_NULL_BUF;
	}

    /*写需要分段*/
    UINT ulFirstChunkSize = 0;
    UINT ulSecondChunkSize = 0;

    RB_DO_STAT_TotalWriteTimes(1);
    
    ulMaxWriteSize = RB_MaxWriteSize();
    if (ulWriteSize >= ulMaxWriteSize )
    {
        RB_DO_STAT_TotalWriteError(1);
        return ERR_RB_WRITE_NOTENOGH;
    }
    
    // easy case, no wrapping
    if( m_ulWritePos + ulWriteSize < m_ulSize )
    {
        memcpy( (m_pRB + m_ulWritePos ), (&ulDataLen), RB_LEN_BYTES);        
        memcpy( (m_pRB + m_ulWritePos + RB_LEN_BYTES), pData, ulDataLen);
        m_ulWritePos += ulWriteSize;
    }
    else // harder case we need to wrap
    {
        ulFirstChunkSize  = m_ulSize - m_ulWritePos;
        ulSecondChunkSize = ulWriteSize - ulFirstChunkSize;

        memcpy( (m_pRB + m_ulWritePos), ( &ulDataLen ), RB_LEN_BYTES);        
        if (ulFirstChunkSize == RB_LEN_BYTES)
        {
            memcpy( m_pRB, pData, ulSecondChunkSize );
        }
        else
        {
            memcpy( (m_pRB + m_ulWritePos + RB_LEN_BYTES), pData, ulFirstChunkSize - RB_LEN_BYTES);
            memcpy( m_pRB, (pData + ulFirstChunkSize - RB_LEN_BYTES), ulSecondChunkSize);
        }
        
        m_ulWritePos = ulSecondChunkSize;
    }

    if (pulWriteLen != NULL)
    {
        *pulWriteLen = ulWriteSize;
    }
    
    RB_DO_STAT_TotalWriteSize(ulWriteSize);
    return 0;
}

int CHI_PLAY_RingBuffer::RB_Read_X(BYTE *pData, UINT *pulData)
{
	UINT ulMaxReadSize = 0;
    UINT ulReadSize	 = 0;				/*真实读出的数据*/

    /*读需要分段*/
    UINT ulFirstChunkSize = 0;
    UINT ulSecondChunkSize = 0;
    
    RB_DO_STAT_TotalReadTimes(1);

    ulMaxReadSize = RB_MaxReadSize();
	//增加保护代码
    if (ulMaxReadSize == 0)
	{
        return ERR_RB_READ_NOTENOGH;
	}

    *pulData   = RB_GetLen_X();
    ulReadSize = ALIGN_LENGTH(*pulData, RB_LEN_BYTES) + RB_LEN_BYTES;

	//增加保护代码
	//if (ulReadSize <= 0 || ulReadSize >= m_ulSize)
    if (ulReadSize == 0 
        || ulReadSize > m_ulSize
        || ulReadSize > ulMaxReadSize)
	{

		CString str;
		str.Format("RB RD_Error: ulMaxReadSize=%d, ulReadSize=%d, m_ulReadPos=%d, m_ulWritePos=%d, m_ulSize=%d >>\n", 
			 ulMaxReadSize, ulReadSize, m_ulReadPos, m_ulWritePos, m_ulSize);
		DT(str);

        RB_DO_STAT_TotalReadError(1);

        return ERR_RB_READ_NOTENOGH;
	}

    if (ulReadSize > ulMaxReadSize)
    {
        RB_DO_STAT_TotalReadError(1);
        return ERR_RB_READ_NOTENOGH;
    }

    // easy case, no wrapping
    if( m_ulReadPos + ulReadSize < m_ulSize)
    {
        memcpy( pData, (m_pRB + m_ulReadPos + RB_LEN_BYTES), ulReadSize - RB_LEN_BYTES );
        m_ulReadPos += ulReadSize;
    }
    else // harder case, buffer wraps
    {
        ulFirstChunkSize  = m_ulSize - m_ulReadPos;
        ulSecondChunkSize = ulReadSize - ulFirstChunkSize;
        if (ulFirstChunkSize == RB_LEN_BYTES)
        {
            memcpy( pData , m_pRB, ulSecondChunkSize );
        }
        else
        {
            memcpy( pData, (m_pRB + m_ulReadPos  + RB_LEN_BYTES), ulFirstChunkSize  - RB_LEN_BYTES);
            memcpy( pData + ulFirstChunkSize  - RB_LEN_BYTES, m_pRB, ulSecondChunkSize);
        }
        
        m_ulReadPos = ulSecondChunkSize;
    }

    RB_DO_STAT_TotalReadSize(ulReadSize);
    
    return 0;
}

UINT CHI_PLAY_RingBuffer::RB_GetLen_X()
{
    UINT len;
    memcpy((VOID *)(&len), (VOID *)(m_pRB + m_ulReadPos), RB_LEN_BYTES);
    return len;
}

BYTE CHI_PLAY_RingBuffer::RB_GetFirstHI_U8()
{
    return (BYTE)(*(BYTE *)(m_pRB + m_ulReadPos));
}

/*2Byte*/
unsigned short CHI_PLAY_RingBuffer::RB_GetFirstHI_U16()
{
    return (unsigned short)(*(unsigned short *)(m_pRB + m_ulReadPos));
}

UINT CHI_PLAY_RingBuffer::RB_GetFirstHI_U32()
{
    return (UINT)(*(UINT *)(m_pRB + m_ulReadPos));
}

UINT CHI_PLAY_RingBuffer::RB_GetMaxWritePercent()
{
    UINT ulMaxWriteSize = RB_MaxWriteSize();
	return ulMaxWriteSize * 100 / m_ulSize * 100;
}

UINT CHI_PLAY_RingBuffer::RB_GetMaxReadPercent()
{
    UINT ulMaxReadSize = RB_MaxReadSize();
	return ulMaxReadSize * 100 / m_ulSize * 100;
}
