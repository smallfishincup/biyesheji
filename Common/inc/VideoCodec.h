#ifndef _VIDEO_CODEC_H
#define _VIDEO_CODEC_H

#include <vfw.h>
#pragma comment(lib,"VFW32.LIB")

class CVideoCodec  
{
	COMPVARS m_cv;
	HIC			m_hIC;
	BITMAPINFO  m_BmpU;
	BITMAPINFO  m_BmpC;
public:
	
BOOL InitCompressor()
{
    ZeroMemory(&m_cv,sizeof(m_cv));
	m_cv.cbSize		= sizeof(m_cv);
	m_cv.dwFlags	= ICMF_COMPVARS_VALID;
	m_cv.hic		= m_hIC;
	m_cv.fccType	= ICTYPE_VIDEO;
	m_cv.fccHandler	= 1684633187;
	m_cv.lpbiOut	= 0;
	m_cv.lKey		= 1;
	m_cv.lKeyCount	= 0;
	m_cv.lDataRate	= 4;
	m_cv.lQ			= 9000;
	ZeroMemory(&m_BmpU,sizeof(m_BmpU));
	m_BmpU.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_BmpU.bmiHeader.biWidth		= VIDEO_WIDTH;
	m_BmpU.bmiHeader.biHeight		= VIDEO_HEIGHT;
	m_BmpU.bmiHeader.biPlanes		= 1;	
	m_BmpU.bmiHeader.biBitCount		= 24;	
	m_BmpU.bmiHeader.biSizeImage	= 230400;

	ICINFO icinfo;
	CString tmp;
	char *p = NULL;
	ZeroMemory(&icinfo,sizeof(icinfo));
	if (ICInfo(ICTYPE_VIDEO,1684633187,&icinfo))
	{
		p=(char*)(&(icinfo.fccType));
		tmp.Format ("CVideoCodec::Init ,ICInfo:\ndwSize=%u\nfccType=%C%C%C%C\nfccHandler=%C%C%C%C\ndwFlags=%08X\ndwVersion=%08X\ndwVersionICM=%08X\nszName=[%s]\nszDescription=[%S]\nszDriver=[%S]\n",\
			icinfo.dwSize ,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6),*(p+7),icinfo.dwFlags ,\
			icinfo.dwVersion ,icinfo.dwVersionICM ,icinfo.szName ,icinfo.szDescription ,icinfo.szDriver );
		//MessageBox(tmp);
	}

    //ICCompressorChoose(NULL,ICMF_CHOOSE_ALLCOMPRESSORS,&m_BmpU,NULL,&m_cv,NULL);
	m_hIC=ICOpen(ICTYPE_VIDEO,m_cv.fccHandler,ICMODE_COMPRESS|ICMODE_DECOMPRESS);
	
	ICCompressGetFormat(m_hIC,&m_BmpU,&m_BmpC);
	ICSendMessage(m_hIC,0x60c9,0xf7329ace,0xacdeaea2);

	m_cv.hic=m_hIC;
	m_cv.dwFlags=ICMF_COMPVARS_VALID;

	ICSeqCompressFrameStart(&m_cv,&m_BmpU);
	ICDecompressBegin(m_hIC,&m_BmpC,&m_BmpU);
	return TRUE;
}

BOOL DecodeVideoData(BYTE *pin,int len,BYTE* pout,int *lenr,DWORD flag)
{
	BOOL bRet=FALSE;
	if(!pin||!pout||!m_hIC)		
		goto RET;

	if(ICDecompress(m_hIC,flag,&m_BmpC.bmiHeader,pin,&m_BmpU.bmiHeader,pout)!=ICERR_OK)
		goto RET;
	if(lenr)
		*lenr=m_BmpU.bmiHeader.biSizeImage;
	
	bRet=TRUE;

RET:
	return bRet;	
}

BOOL EncodeVideoData(BYTE* pin,int len,BYTE* pout,int* lenr,BOOL* pKey)
{
	BOOL bRet=FALSE;
	BYTE*p;
	long s=1;
	BOOL k;
	if(!pin||!pout||len!=(int)m_BmpU.bmiHeader.biSizeImage||!m_hIC)
		goto RET;
	
	p=(BYTE*)ICSeqCompressFrame(&m_cv,0,pin,&k,&s);
	if(!p)		goto RET;
	if(lenr)	*lenr=s;
	if(pKey)	*pKey=k;
	CopyMemory(pout,p,s);
	bRet=TRUE;
RET:
	return bRet;	
}

CVideoCodec()
{
}
virtual ~CVideoCodec()
{
}

};

///////////////////////////////////////////////////////////////////////////////
//LZSS Part START--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#define LZSS_N			4096
#define LZSS_F			18
#define LZSS_THRESHOLD	2
#define LZSS_NIL		LZSS_N

class C_LZSS
{
public:
	C_LZSS()
	{
		textsize = 0;
		codesize = 0;
		printcount = 0;
	}
	~C_LZSS()
	{
	}
HGLOBAL C_LZSS::Encode(char *chData, int nSize)
{
	if(!AfxIsValidAddress(chData, nSize))
		return NULL;

//缓冲数据区大小
const int nMaxSize = 65536;

	HGLOBAL hZip = NULL;
	hZip = ::GlobalAlloc(GHND, nMaxSize);
	if(hZip == NULL)
	{
		return NULL;
	}
	LPBYTE lpZipData = (LPBYTE)::GlobalLock(hZip);
	BYTE byMaxBuf[nMaxSize];
	ZeroMemory(byMaxBuf, nMaxSize);

	int nCurPos = 0; //记下数据的位置

	int nZipPos = 0; //记下压缩数据的位置
	int nBufPos = 0; //记下当前存储位置hZip
	int nZipSize = 0; //压缩后数据的大小

	int  i, c, len, r, s, last_match_length, code_buf_ptr;
	unsigned char code_buf[17], mask;
	InitTree();
	code_buf[0] = 0;
	code_buf_ptr = mask = 1;
	s = 0;  r = LZSS_N - LZSS_F;
	for(i = s; i < r; i++)
	{
		text_buf[i] = ' ';
	}
	for(len = 0; len < LZSS_F && (nCurPos < nSize/*getc(infile)*/); len++)
	{
		c = chData[nCurPos]; //add
		nCurPos ++; //add
		text_buf[r + len] = c;
	}
	if((textsize = len) == 0)
	{
		return false;
	}
	for(i = 1; i <= LZSS_F; i++)
	{
		InsertNode(r - i);
	}
	InsertNode(r);
	do
	{
		if(match_length > len)
		{
			match_length = len;
		}
		if(match_length <= LZSS_THRESHOLD)
		{
			match_length = 1;
			code_buf[0] |= mask;
			code_buf[code_buf_ptr++] = text_buf[r];
		}
		else
		{
			code_buf[code_buf_ptr++] = (unsigned char) match_position;
			code_buf[code_buf_ptr++] = (unsigned char)
				(((match_position >> 4) & 0xf0)
			  | (match_length - (LZSS_THRESHOLD + 1)));
		}
		if((mask <<= 1) == 0)
		{
			for(i = 0; i < code_buf_ptr; i++)
			{
				//存储这个压缩字节
				if(nZipPos >= nMaxSize)
				{
					nZipPos = 0;
					memcpy(lpZipData + nBufPos, byMaxBuf, nMaxSize);
					nBufPos += nMaxSize;

					::GlobalUnlock(hZip); //重新分配内存
					hZip = ::GlobalReAlloc(hZip, nBufPos + nMaxSize, 0);
					lpZipData = (LPBYTE)::GlobalLock(hZip);
				}
				byMaxBuf[nZipPos] = code_buf[i];
				nZipPos ++;
				nZipSize ++; //当前压缩数据的大小
			}
			codesize += code_buf_ptr;
			code_buf[0] = 0;
			code_buf_ptr = mask = 1;
		}
		last_match_length = match_length;
		for(i = 0; i < last_match_length && (nCurPos < nSize)/*c = getc(infile)) != EOF*/; i++)
		{
			c = chData[nCurPos]; //add
			nCurPos ++; //add
			DeleteNode(s);
			text_buf[s] = c;
			if(s < LZSS_F - 1)
			{
				text_buf[s + LZSS_N] = c;
			}
			s = (s + 1) & (LZSS_N - 1);
			r = (r + 1) & (LZSS_N - 1);
			InsertNode(r);
		}
		if((textsize += i) > printcount)
		{
			printcount += 1024;
		}
		while(i++ < last_match_length)
		{
			DeleteNode(s);
			s = (s + 1) & (LZSS_N - 1);
			r = (r + 1) & (LZSS_N - 1);
			if(--len)
			{
				InsertNode(r);
			}
		}
	}while (len > 0);
	if(code_buf_ptr > 1)
	{
		for(i = 0; i < code_buf_ptr; i++)
		{
			//存储这个压缩字节
			if(nZipPos >= nMaxSize)
			{
				nZipPos = 0;
				memcpy(lpZipData + nBufPos, byMaxBuf, nMaxSize);
				nBufPos += nMaxSize;

				::GlobalUnlock(hZip); //重新分配内存
				hZip = ::GlobalReAlloc(hZip, nBufPos + nMaxSize, 0);
				lpZipData = (LPBYTE)::GlobalLock(hZip);
			}
			byMaxBuf[nZipPos] = code_buf[i];
			nZipPos ++;
			nZipSize ++; //当前压缩数据的大小
		}
		codesize += code_buf_ptr;
	}
	//存储剩余的压缩数据
	if(nZipPos > 0)
	{
		memcpy(lpZipData + nBufPos, byMaxBuf, nZipPos);
	}
	::GlobalUnlock(hZip); //重新分配内存
	hZip = ::GlobalReAlloc(hZip, nZipSize, 0);
	return hZip;
}

HGLOBAL C_LZSS::Decode(char *chZipData, int nZipSize)
{
	if(!AfxIsValidAddress(chZipData, nZipSize))
		return NULL;

//缓冲数据区大小
const int nMaxSize = 65536;

	HGLOBAL hUnZip = NULL;
	hUnZip = ::GlobalAlloc(GHND, nMaxSize);
	if(hUnZip == NULL)
	{
		return NULL;
	}
	LPBYTE lpUnZipData = (LPBYTE)::GlobalLock(hUnZip);
	BYTE byMaxBuf[nMaxSize];
	ZeroMemory(byMaxBuf, nMaxSize);

	int nCurPos = 0; //记下数据的位置

	int nPos = 0; //记下解压数据的位置
	int nBufPos = 0; //记下当前存储位置hUnZip
	int nSize = 0; //解压后数据的大小

	int  i, j, k, r, c;
	unsigned char cc; //add
	unsigned int  flags;
	for (i = 0; i < LZSS_N - LZSS_F; i++)
	{
		text_buf[i] = ' ';
	}
	r = LZSS_N - LZSS_F;
	flags = 0;
	for( ; ; )
	{
		if(((flags >>= 1) & 256) == 0)
		{
			if(nCurPos >= nZipSize)//(c = getc(infile)) == EOF)
			{
				break;
			}
			c = (unsigned char)chZipData[nCurPos]; //add
			nCurPos ++; //add
			
			flags = c | 0xff00;
		}
		if(flags & 1)
		{
			if(nCurPos >= nZipSize)//(c = getc(infile)) == EOF)
			{
				break;
			}
			c = (unsigned char)chZipData[nCurPos]; //add
			cc = c; //add
			nCurPos ++; //add

			//存储这个压缩字节
			if(nPos >= nMaxSize)
			{
				nPos = 0;
				memcpy(lpUnZipData + nBufPos, byMaxBuf, nMaxSize);
				nBufPos += nMaxSize;
				
				::GlobalUnlock(hUnZip); //重新分配内存
				hUnZip = ::GlobalReAlloc(hUnZip, nBufPos + nMaxSize, 0);
				lpUnZipData = (LPBYTE)::GlobalLock(hUnZip);
			}
			byMaxBuf[nPos] = cc;
			nPos ++;
			nSize ++; //当前压缩数据的大小

			text_buf[r++] = c;
			r &= (LZSS_N - 1);
		}
		else
		{
			if(nCurPos >= nZipSize)//(i = getc(infile)) == EOF)
			{
				break;
			}
			i = (unsigned char)chZipData[nCurPos]; //add
			nCurPos ++; //add

			if(nCurPos >= nZipSize)//(j = getc(infile)) == EOF)
			{
				break;
			}
			j = chZipData[nCurPos]; //add
			nCurPos ++; //add

			i |= ((j & 0xf0) << 4);
			j = (j & 0x0f) + LZSS_THRESHOLD;
			for(k = 0; k <= j; k++)
			{
				c = text_buf[(i + k) & (LZSS_N - 1)];
				cc = c; //add

				//存储这个压缩字节
				if(nPos >= nMaxSize)
				{
					nPos = 0;
					memcpy(lpUnZipData + nBufPos, byMaxBuf, nMaxSize);
					nBufPos += nMaxSize;
					
					::GlobalUnlock(hUnZip); //重新分配内存
					hUnZip = ::GlobalReAlloc(hUnZip, nBufPos + nMaxSize, 0);
					lpUnZipData = (LPBYTE)::GlobalLock(hUnZip);
				}
				byMaxBuf[nPos] = cc;
				nPos ++;
				nSize ++; //当前压缩数据的大小

				text_buf[r++] = c; 
				r &= (LZSS_N - 1);
			}
		}
	}

	//存储剩余的解压数据
	if(nPos > 0)
	{
		memcpy(lpUnZipData + nBufPos, byMaxBuf, nPos);
	}
	::GlobalUnlock(hUnZip); //重新分配内存
	hUnZip = ::GlobalReAlloc(hUnZip, nSize, 0);

	return hUnZip;
}

protected:
	void InitTree(void)
	{
		int i = 0;
		for(i = LZSS_N + 1; i <= LZSS_N + 256; i++)
		{
			rson[i] = LZSS_NIL;
		}
		for(i = 0; i < LZSS_N; i++)
		{
			dad[i] = LZSS_NIL;
		}
	}

	void InsertNode(int r)
	{
		int i = 0;
		int p = 0;
		int cmp = 1;
		unsigned char *key = &text_buf[r];
		p = LZSS_N + 1 + key[0];
		rson[r] = lson[r] = LZSS_NIL;
		match_length = 0;
		for( ; ; )
		{
			if(cmp >= 0)
			{
				if(rson[p] != LZSS_NIL)
				{
					p = rson[p];
				}
				else
				{
					rson[p] = r;
					dad[r] = p;
					return ;
				}
			}
			else
			{
				if(lson[p] != LZSS_NIL)
				{
					p = lson[p];
				}
				else
				{
					lson[p] = r;
					dad[r] = p;
					return ;
				}
			}
			for(i = 1; i < LZSS_F; i++)
			{
				if ((cmp = key[i] - text_buf[p + i]) != 0)
					break;
			}
			if(i > match_length)
			{
				match_position = p;
				if((match_length = i) >= LZSS_F)
					break;
			}
		}
		dad[r] = dad[p];
		lson[r] = lson[p];
		rson[r] = rson[p];
		dad[lson[p]] = r;
		dad[rson[p]] = r;
		if(rson[dad[p]] == p)
		{
			rson[dad[p]] = r;
		}
		else
		{
			lson[dad[p]] = r;
		}
		dad[p] = LZSS_NIL;
	}
	
	void DeleteNode(int p)
	{
		int q = 0;
		if(dad[p] == LZSS_NIL)
			return ;
		if(rson[p] == LZSS_NIL)
		{
			q = lson[p];
		}
		else if(lson[p] == LZSS_NIL)
		{
			q = rson[p];
		}
		else
		{
			q = lson[p];
			if(rson[q] != LZSS_NIL)
			{
				do
				{
					q = rson[q];
				}while (rson[q] != LZSS_NIL);
				rson[dad[q]] = lson[q];
				dad[lson[q]] = dad[q];
				lson[q] = lson[p];
				dad[lson[p]] = q;
			}
			rson[q] = rson[p];
			dad[rson[p]] = q;
		}
		dad[q] = dad[p];
		if(rson[dad[p]] == p)
		{
			rson[dad[p]] = q;
		}
		else
		{
			lson[dad[p]] = q;
		}
		dad[p] = LZSS_NIL;
	}
	
protected:
	unsigned long int textsize;
	unsigned long int codesize;
	unsigned long int printcount;
	unsigned char text_buf[LZSS_N + LZSS_F - 1];
	int match_position;
	int match_length;
	int lson[LZSS_N + 1];
	int rson[LZSS_N + 257];
	int dad[LZSS_N + 1];
//	FILE *infile;
//	FILE *outfile;
};

///////////////////////////////////////////////////////////////////////////////
//LZSS Part END--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//LZW Part START--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
typedef struct LZWEncodeEntry
{
	BYTE bLast;
	WORD wCode;
	struct LZWEncodeEntry far *pChild,*pRightBrother;
}LZWENCODEENTRY,*PLZWENCODEENTRY;     
typedef struct LZWDecodeEntry
{
	BYTE *pbContain;
}LZWDECODEENTRY,*PLZWDECODEENTRY;

class CLZWDecodeTable
{
public:
	CLZWDecodeTable(BOOL fInit=TRUE);
	~CLZWDecodeTable();
	void ClearDecodeTable(void);
	void InitLZWTable(void);
	BYTE* GetMatchData(WORD wCode);
	void AddToChild(WORD wCode,BYTE *pbContain,int iLength);
	DWORD GetTableEntryNumber(void){return m_dwTableEntryNumber;};
protected:
	BYTE** m_pbContain;
	DWORD m_dwTableEntryNumber;
};

class CLZWEncodeTable
{
public:
	CLZWEncodeTable(BOOL fInit=TRUE);
	~CLZWEncodeTable();
	void ClearLZWTable(void);
	void InitLZWTable(void);
	PLZWENCODEENTRY FindMatchChild(BYTE bChileLast,PLZWENCODEENTRY pCurrent);
	// return the find child
	PLZWENCODEENTRY  AddToChild(BYTE bLast,PLZWENCODEENTRY pCurrent);
	//return the add child

public://inline
	PLZWENCODEENTRY GetHead(void){return &m_EntryHead;};
	DWORD GetTableEntryNumber(void){return m_dwTableEntryNumber;};

protected:
	void RemoveFirstChild(void);
	PLZWENCODEENTRY FindRightBrother(PLZWENCODEENTRY pCurrent);
protected:
	DWORD m_dwTableEntryNumber;
	UINT m_uNextCodeForUse;
	LZWENCODEENTRY m_EntryHead;
};



typedef BOOL (*FUN_LZWENCODEGETNEXTBYTE) (BYTE& bGet);// get next byte
typedef BOOL (*FUN_LZWENCODEPUTNEXTBYTES) (BYTE* pbPut,int iLength); // put next byte
typedef void (*FUN_LZWENCODEDBYTES) (void);
typedef BOOL (*FUN_LZWDECODEGETNEXTBYTES) (BYTE* pbGet,int iLength);
typedef BOOL (*FUN_LZWDECODEPUTNEXTBYTE) (BYTE bPut);
typedef void (*FUN_LZWDECODEDBYTES) (void);

#define LZW_MAX_ENTRY             		4096
#define LZW_CLEAR_CODE                  256
#define LZW_END_CODE                    LZW_CLEAR_CODE+1//257
#define LZW_BEGIN_ENTRY                 LZW_END_CODE+1//258

//#define SET_BIT_1(b,i)          (b |= (1<<i))
#define SET_BIT_0(b,i)          (b &= (~(1<<i)))
//#define CHECK_BIT_1(b,i)        (b & (1<<i))
#define CHECK_BIT_0(b,i)        (!(CHECK_BIT_1(b,i))

class CDecodeBitArray
{ 
public:
	CDecodeBitArray(DWORD dwInitWidth=0);// width in bit
	~CDecodeBitArray();
	void ClearBits(void);
	void InitBits(DWORD dwInitWidth);
	void InitBytes(DWORD dwInitWidth);
	DWORD GetLeftBytes(void);
	DWORD GetLeftBits(void){return m_dwTail-m_dwHead;};
	WORD RemoveBits(int iWidth);
	BOOL AddBytes(BYTE* pbAdd,int iLength);
protected:
	void Resort(void);
	WORD RemoveFirstBit(void);
protected:
	BYTE *m_pbBits;
	DWORD m_dwTail,m_dwHead;
	// tail is the first not usable bit
	// head is the first usable bit
	DWORD m_dwWidthInByte;
};

class CEncodeBitArray
{
public:
	CEncodeBitArray(DWORD dwInitWidth=0);// width in bit
	BOOL InitBits(DWORD dwInitWidth);
	BOOL InitBytes(DWORD dwInitWidth){return InitBits(dwInitWidth*8);};
	void ClearBits(void);
	~CEncodeBitArray();
	BOOL AddBits(WORD wAdd,int iWidth);
	DWORD GetBytesWidth(void);// get width in byte
	DWORD GetBitsWidth(void){return m_dwTail;};// get width in bit
	BYTE* GetBits(void){return m_pbBits;};// get the point of contain
	DWORD GetMaxBytes(void){return m_dwWidthInByte;};// get max capability
	int RemoveBytes(BYTE *pbGet,int iWant);
	// get bytes in contain and reset tail
protected:
	BYTE* m_pbBits;
	DWORD m_dwTail;
	DWORD m_dwWidthInByte;
};
class CLZWDecode
{ 
public:
	CLZWDecode():m_LZWTable(FALSE)
	{
		iDeGetPos = 0;
		iDePutPos = 0;
	};
	~CLZWDecode(){};

	HGLOBAL BeginLZWDecode(char *chZipData, int dwLength);

protected:
	CLZWDecodeTable m_LZWTable;
	CDecodeBitArray m_baContain;
	DWORD m_dwDecodedByte;
	int m_iTotalEntry;

	int iDeGetPos;
	int iDePutPos;

};

class CLZWEncode
{
public:
	CLZWEncode():m_LZWTable(FALSE)
	{
		m_wMaxEntry = LZW_MAX_ENTRY;
		iPut = 0;
		iTurn = 0;
	};
	~CLZWEncode(){};
	
	HGLOBAL BeginLZWEncode(char *chData, int dwLength);
	DWORD GetCompressedLength(void){return m_dwCompressedLength;};
protected:
	int GetBitWidth(void);
protected:
	WORD m_wMaxEntry;
	CLZWEncodeTable m_LZWTable;
	CEncodeBitArray m_baContain;
	DWORD m_dwCompressedLength;

	int iPut;
	int iTurn;
};

class C_LZW
{
public:
	C_LZW()
	{
	}
	~C_LZW()
	{
	}

	HGLOBAL Encode(char *chData, int nSize);
	HGLOBAL Decode(char *chZipData, int nZipSize);

protected:
};
///////////////////////////////////////////////////////////////////////////////
//LZW Part END--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#endif