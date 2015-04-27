// **************************************************************
// File		   : hi_play_audiobuffer.h
// Description : ÒôÆµ»º³åÀà
// Author	   :  
// Date		   : 
// Revisions   :
// **************************************************************

#if !defined(AFX_HI_PLAY_AUDIOBUFFER_H__9DF05CBE_CD37_4912_A48F_CB410087E089__INCLUDED_)
#define AFX_HI_PLAY_AUDIOBUFFER_H__9DF05CBE_CD37_4912_A48F_CB410087E089__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHI_PLAY_AudioBuffer  
{
public:
	CHI_PLAY_AudioBuffer(DWORD dwSize, BOOL bAutoDelete = TRUE);
	CHI_PLAY_AudioBuffer(void *buffer, DWORD dwlength);

	virtual ~CHI_PLAY_AudioBuffer();

	void	Erase();
	short	GetUsedLength();
	void	GetBuffer(void *buffer, DWORD dwlength);
	
	union PointerUnion
	{
		BYTE		*b;
		BYTE		*u8;
		char		*c;
		WORD		*w;
		short int	*s;
		DWORD		*d;
		int			*i;
		float		*f;
		double		*r;	// real
		void		*v;
	}ptr;
	
	UINT	iByteLen;	// length in bytes
	
private:
	BOOL	m_bAutoDelete;
	
};

#endif // !defined(AFX_HI_PLAY_AUDIOBUFFER_H__9DF05CBE_CD37_4912_A48F_CB410087E089__INCLUDED_)
