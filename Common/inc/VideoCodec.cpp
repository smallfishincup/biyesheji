
#include "stdafx.h"
#include "VideoCodec.h"
///////////////////////////////////////////////////////////////////////////////
//LZW Part START--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

CLZWDecodeTable::CLZWDecodeTable(BOOL fInit)
{
	m_pbContain=NULL;
	m_dwTableEntryNumber=0;
	if(fInit)
		InitLZWTable();
}
CLZWDecodeTable::~CLZWDecodeTable()
{
	ClearDecodeTable();
}
void CLZWDecodeTable::ClearDecodeTable(void)
{   
	if(m_pbContain==NULL)
		return;
	for(int i=0;i<4096;i++)
	{
		if(m_pbContain[i])
			delete (m_pbContain[i]);
	}
	delete m_pbContain;
	m_pbContain=NULL;
	m_dwTableEntryNumber=0;
}
void CLZWDecodeTable::InitLZWTable(void)
{
	ClearDecodeTable();
	m_pbContain=new BYTE*[4096];
	int iSize=sizeof(m_pbContain);
	//if(NULL==m_pbContain)
	//	AfxMessageBox("error new m_pbContain=BYTE*[4096]");
	for(int i=0;i<4096;i++)
	{
		m_pbContain[i]=NULL;
	}
	for(i=0;i<=255;i++)
	{
		m_pbContain[i]=new BYTE[1+2];
		*((WORD*)m_pbContain[i])=1;
		m_pbContain[i][2]=(BYTE)i;
	}
	m_dwTableEntryNumber=LZW_BEGIN_ENTRY;
}
BYTE* CLZWDecodeTable::GetMatchData(WORD wCode)
{
	return m_pbContain[wCode];
}
void CLZWDecodeTable::AddToChild(WORD wCode,BYTE *pbContain,int iLength)
{
	ASSERT(wCode<4096);
	if(m_pbContain[wCode])
		delete m_pbContain[wCode];
	m_pbContain[wCode]=new BYTE[iLength+2];
	*((WORD*)m_pbContain[wCode])=(WORD)iLength;
	memcpy(m_pbContain[wCode]+2,pbContain,iLength);
}

/*
class CLZWEncodeTable
*/
CLZWEncodeTable::CLZWEncodeTable(BOOL fInit)
{
	if(fInit)
		InitLZWTable();
	else
	{
		m_dwTableEntryNumber=0;
		m_EntryHead.pRightBrother=NULL;
		m_EntryHead.pChild=NULL;
	}
}
CLZWEncodeTable::~CLZWEncodeTable()
{
	ClearLZWTable();
}
void CLZWEncodeTable::ClearLZWTable(void)
{
	if(m_EntryHead.pChild==NULL)
		return;
	m_dwTableEntryNumber=0;
	int iRe=0;
	while(m_EntryHead.pChild)
	{
		RemoveFirstChild();
		iRe++;
		//printf("remove %d\n",++iRe);;
	}
}
void CLZWEncodeTable::RemoveFirstChild(void)
{
	PLZWENCODEENTRY pFirstChild=m_EntryHead.pChild;// this child will be removed
	if(pFirstChild->pChild)
	{
		m_EntryHead.pChild=pFirstChild->pChild;
		if(m_EntryHead.pChild->pRightBrother)
		{
			PLZWENCODEENTRY pRightBrother=FindRightBrother(m_EntryHead.pChild);
			pRightBrother->pRightBrother=pFirstChild->pRightBrother;
		}
		else
			(m_EntryHead.pChild)->pRightBrother=pFirstChild->pRightBrother;
		//delete pFirstChild;
	}
	else
		m_EntryHead.pChild=pFirstChild->pRightBrother;
	delete pFirstChild;
}
PLZWENCODEENTRY CLZWEncodeTable::FindRightBrother(PLZWENCODEENTRY pCurrent)
{
	PLZWENCODEENTRY pFind;
	pFind=pCurrent;
	while(pFind->pRightBrother)
	{
		pFind=pFind->pRightBrother;
	}
	return pFind;
}
void CLZWEncodeTable::InitLZWTable(void)
{// init the table ,it has 256 items code from 0 to 255
	ClearLZWTable();
	PLZWENCODEENTRY pEntryFirst=new LZWENCODEENTRY;
	pEntryFirst->wCode=(WORD)0;
	pEntryFirst->bLast=(BYTE)0;
	pEntryFirst->pRightBrother=pEntryFirst->pChild=NULL;
	m_EntryHead.pChild=pEntryFirst;
	m_EntryHead.pRightBrother=NULL;
	PLZWENCODEENTRY pPrev=pEntryFirst;
	for(int i=1;i<=255;i++)
	{// set the brother nodes
		PLZWENCODEENTRY pEntry=new LZWENCODEENTRY;
		pEntry->wCode=(WORD)i;
		pEntry->bLast=(BYTE)i;
		pEntry->pChild=pEntry->pRightBrother=NULL;
		pPrev->pRightBrother=pEntry;
		pPrev=pEntry;
	}
	m_dwTableEntryNumber=258;
	m_uNextCodeForUse=LZW_BEGIN_ENTRY;
}
PLZWENCODEENTRY CLZWEncodeTable::FindMatchChild(BYTE bChildLast,PLZWENCODEENTRY pCurrent)
{// return the find child entry
	if(pCurrent->pChild==NULL)
		return NULL;
	PLZWENCODEENTRY pChild=pCurrent->pChild;
	// pChild is the current's child
	// and all pChild's brother is the current's child
	while(pChild!=NULL)
	{
		if(pChild->bLast==bChildLast)
			return pChild;
		pChild=pChild->pRightBrother;
	}
	return NULL;
}
PLZWENCODEENTRY CLZWEncodeTable::AddToChild(BYTE bLast,PLZWENCODEENTRY pCurrent)
{
	ASSERT(pCurrent);
	PLZWENCODEENTRY pChild=new LZWENCODEENTRY;
	if(pChild==NULL)
	{
		int _j=0;
	}
	pChild->pChild=pChild->pRightBrother=NULL;
	pChild->bLast=bLast;
	pChild->wCode=(WORD)m_uNextCodeForUse;
	if(pChild->wCode==LZW_CLEAR_CODE)
	{
		int _i=0;
	}
	m_uNextCodeForUse++;
	m_dwTableEntryNumber++;
	pChild->pRightBrother=pCurrent->pChild;
	pCurrent->pChild=pChild;
	return pChild;
}

CDecodeBitArray::CDecodeBitArray(DWORD dwInitWidth)// width in bit
{
	m_pbBits=NULL;
	m_dwWidthInByte=0;
	m_dwTail=m_dwHead=0;
	if(dwInitWidth)
		InitBits(dwInitWidth);
}
CDecodeBitArray::~CDecodeBitArray()
{
	ClearBits();
}
void CDecodeBitArray::ClearBits(void)
{
	if(m_pbBits)
		delete m_pbBits;
	m_dwTail=m_dwHead=0;
	m_dwWidthInByte=0;
}
void CDecodeBitArray::InitBits(DWORD dwInitWidth)
{
	ClearBits();
	DWORD dwLength=dwInitWidth/8;
	dwLength+=(dwInitWidth%8)?1:0;
	m_pbBits=new BYTE[dwLength];
	m_dwHead=m_dwTail=0;
	m_dwWidthInByte=dwLength;
}
void CDecodeBitArray::InitBytes(DWORD dwInitWidth)
{
	InitBits(dwInitWidth*8);
}
DWORD CDecodeBitArray::GetLeftBytes(void)
{
	DWORD dwLeftBytes;
	DWORD dwLeftBits=GetLeftBits();
	dwLeftBytes=dwLeftBits/8;
	dwLeftBytes+=(dwLeftBits%8)?1:0;
	return dwLeftBytes;
}
WORD CDecodeBitArray::RemoveBits(int iWidth)
{
	WORD wGet=0;
	for(int i=iWidth-1;i>=0;i--)
	{
		if(RemoveFirstBit())
		{
			(wGet |= (1<<i));
		}
////			SET_BIT_1(wGet,i);
	}
	return wGet;
}
WORD CDecodeBitArray::RemoveFirstBit(void)
{
	BYTE* pbFirstByte=m_pbBits+m_dwHead/8;
	BYTE bFirstByte=*pbFirstByte;
//	 (b & (1<<i))
	WORD wGet= bFirstByte&(1<<(7-m_dwHead%8))?1:0;
	m_dwHead++;
	return wGet;
}
BOOL CDecodeBitArray::AddBytes(BYTE* pbAdd,int iLength)
{
	if(m_pbBits==NULL)
		return FALSE;
	Resort();
	memcpy(m_pbBits+m_dwTail/8,pbAdd,iLength);
	m_dwTail+=8*(DWORD)iLength;
	return TRUE;
}
void CDecodeBitArray::Resort(void)
{// because m_dwTail%8 always equal 0
	if(m_dwHead<8)
		return;
	if(m_dwTail==m_dwHead)
	{
		m_dwTail=m_dwHead=0;
		return;
	}
	DWORD dwLength=GetLeftBytes();
	DWORD dwHead=m_dwHead%8;
	DWORD dwMove=m_dwHead-dwHead;
	memcpy(m_pbBits,m_pbBits+(m_dwHead/8),(int)dwLength);
	m_dwHead=dwHead;
	m_dwTail-=dwMove;
}

/*
class CEncodeBitArray
*/
CEncodeBitArray::CEncodeBitArray(DWORD dwInitWidth)
{
	if(dwInitWidth==0)
		m_pbBits=NULL;
	else
		InitBits(dwInitWidth);
}
CEncodeBitArray::~CEncodeBitArray()
{
	ClearBits();
}
BOOL CEncodeBitArray::InitBits(DWORD dwInitWidth)
{
	ClearBits();
	ASSERT(dwInitWidth);
	m_dwWidthInByte=dwInitWidth/8;
	m_dwWidthInByte+=(dwInitWidth%8)?1:0;
	m_pbBits=new BYTE[m_dwWidthInByte];
	m_dwTail=0;
	return TRUE;
}
void CEncodeBitArray::ClearBits(void)
{
	if(m_pbBits)
		delete m_pbBits;
	m_pbBits=NULL;
}
BOOL CEncodeBitArray::AddBits(WORD wAdd,int iWidth)
{
	if(m_pbBits==NULL)
		return FALSE;
	for(int i=iWidth-1;i>=0;i--)
	{
		BYTE* pbByte=m_pbBits+m_dwTail/8;
		if((wAdd & (1<<i)))//  CHECK_BIT_1(wAdd,i))
			((*pbByte) |= (1<<(7-m_dwTail%8)));//SET_BIT_1(*pbByte,7-m_dwTail%8);
		else
			((*pbByte) &= (~(1<<(7-m_dwTail%8))));//SET_BIT_0(*pbByte,7-m_dwTail%8);
		m_dwTail++;
	}
	return TRUE;
}
DWORD CEncodeBitArray::GetBytesWidth(void)
{
	DWORD dwBytes=m_dwTail/8;
	dwBytes+=(m_dwTail%8)?1:0;
	return dwBytes;
}
int CEncodeBitArray::RemoveBytes(BYTE *pbGet,int iWant)
{
	if(m_pbBits==NULL)
		return -1;
	int iTotal=(int)GetBytesWidth();
	if(iWant>iTotal)
		iWant=iTotal;
	if(pbGet!=NULL)
		memcpy(pbGet,m_pbBits,iWant);
	memcpy(m_pbBits,m_pbBits+iWant,iTotal-iWant);
	int iTail=(int)m_dwTail;
	iTail-=iWant*8;
	if(iTail<0)
		iTail=0;
	m_dwTail=iTail;
	return iWant;
}

HGLOBAL CLZWDecode::BeginLZWDecode(char *chZipData, int dwLength)
{
	if(!AfxIsValidAddress(chZipData, dwLength))
		return NULL;


//缓冲数据区大小
const int nMaxSize = 65536;
	HGLOBAL hUnZip = NULL;
	hUnZip = ::GlobalAlloc(GHND, nMaxSize);
	if(hUnZip == NULL)
	{
		return NULL;
	}
	LPBYTE lpZipData = (LPBYTE)::GlobalLock(hUnZip);
	BYTE byMaxBuf[nMaxSize];
	ZeroMemory(byMaxBuf, nMaxSize);
	int nZipPos = 0; //记下压缩数据的位置
	int nBufPos = 0; //记下当前存储位置hZip
	int nZipSize = 0; //压缩后数据的大小

	iDePutPos=iDeGetPos=0;
	
	DWORD dwBytesOnce = 1;
	DWORD wBuffer = 1024;
	m_dwDecodedByte=0;
	BYTE *pbNewData=new BYTE[4000],*pbOutData=new BYTE[4000];
	BYTE *pbBuffer=new BYTE[wBuffer];
	BYTE bFirst;
	m_LZWTable.InitLZWTable();
	int iBitWidth=9;
	m_iTotalEntry=LZW_BEGIN_ENTRY;
	BYTE* pbDecodedData;
	WORD wOld,wLastLen;
	m_baContain.InitBits((wBuffer+20)*8);
	int iR=0;
	DWORD dwRead=0;
	while(1)
	{
		if(m_baContain.GetLeftBytes()<5)
		{
			WORD wGetBytes= (WORD)wBuffer;
			if((DWORD)wGetBytes+dwRead>(DWORD)dwLength)
				wGetBytes=(WORD)(dwLength-dwRead);
			if(wGetBytes!=0)
			{
///////////////////////////////////				
				int iGet = wGetBytes;
				if(iDeGetPos + iGet > dwLength)
					iGet = dwLength - iDeGetPos;
				memcpy(pbBuffer, chZipData + iDeGetPos, iGet);
				iDeGetPos += iGet;
///////////////////////////////////				
				m_baContain.AddBytes(pbBuffer,wBuffer);
				dwRead+=wGetBytes;
			}
		}
		int iT=m_iTotalEntry+1;
		iT>>=9;
		iBitWidth=9;
		while(iT>0)
		{
			iT>>=1;
			iBitWidth++;
		}
		WORD wGet=m_baContain.RemoveBits(iBitWidth);
		if(wGet==LZW_END_CODE)
		{
			break;
		}
		if(wGet==LZW_CLEAR_CODE)
		{
			m_LZWTable.InitLZWTable();
			wGet=m_baContain.RemoveBits(9);
			if(wGet==LZW_END_CODE)
				break;
			pbDecodedData=m_LZWTable.GetMatchData(wGet);

//////////////////////////////
			BYTE *pbWrite = pbDecodedData;
			if(pbWrite != NULL)
			{
				WORD wL=*((WORD*)pbWrite);
				pbWrite+=sizeof(WORD);
				for(DWORD i=0;i<wL;i++)
				{
					//存储这个压缩
					if(nZipPos >= nMaxSize)
					{
						memcpy(lpZipData + nBufPos, byMaxBuf, nMaxSize);
						nBufPos += nMaxSize;
						nZipPos = 0;
						::GlobalUnlock(hUnZip); //重新分配内存
						hUnZip = ::GlobalReAlloc(hUnZip, nBufPos + nMaxSize, 0);
						lpZipData = (LPBYTE)::GlobalLock(hUnZip);
					}
					byMaxBuf[nZipPos] = *pbWrite;
					nZipPos ++;//
					nZipSize ++; //add


					pbWrite++;
				}
				m_dwDecodedByte+=wL;
			}

//////////////////////////////
			wOld=wGet;
			m_iTotalEntry=258;
		}
		else
		{// not clear
			pbDecodedData=m_LZWTable.GetMatchData(wGet);
			if(NULL!=pbDecodedData)
			{// in table
				bFirst=pbDecodedData[2];
//////////////////////////////
			BYTE *pbWrite = pbDecodedData;
			if(pbWrite != NULL)
			{
				WORD wL=*((WORD*)pbWrite);
				pbWrite+=sizeof(WORD);
				for(DWORD i=0;i<wL;i++)
				{
					//存储这个压缩
					if(nZipPos >= nMaxSize)
					{
						memcpy(lpZipData + nBufPos, byMaxBuf, nMaxSize);
						nBufPos += nMaxSize;
						nZipPos = 0;
						::GlobalUnlock(hUnZip); //重新分配内存
						hUnZip = ::GlobalReAlloc(hUnZip, nBufPos + nMaxSize, 0);
						lpZipData = (LPBYTE)::GlobalLock(hUnZip);
					}
					byMaxBuf[nZipPos] = *pbWrite;
					nZipPos ++;//
					nZipSize ++; //add


					pbWrite++;
				}
				m_dwDecodedByte+=wL;
			}

//////////////////////////////
				if(wOld!=LZW_CLEAR_CODE)
				{// not the first code be read in
					pbDecodedData=m_LZWTable.GetMatchData(wOld);
					wLastLen=*((WORD*)pbDecodedData);
					memcpy(pbNewData,pbDecodedData+2,wLastLen);
					pbNewData[wLastLen]=bFirst;
					m_LZWTable.AddToChild((WORD)m_iTotalEntry,pbNewData,wLastLen+1);
					m_iTotalEntry+=1;
				}
				wOld=wGet;
			}
			else
			{
				pbDecodedData=m_LZWTable.GetMatchData(wOld);
				bFirst=pbDecodedData[2];
				wLastLen=*((WORD*)pbDecodedData);
				memcpy(pbOutData+2,pbDecodedData+2,wLastLen);
				pbOutData[wLastLen+2]=bFirst;
				*((WORD*)pbOutData)=wLastLen+1;
//////////////////////////////
			BYTE *pbWrite = pbOutData;
			if(pbWrite != NULL)
			{
				WORD wL=*((WORD*)pbWrite);
				pbWrite+=sizeof(WORD);
				for(DWORD i=0;i<wL;i++)
				{
					//存储这个压缩
					if(nZipPos >= nMaxSize)
					{
						memcpy(lpZipData + nBufPos, byMaxBuf, nMaxSize);
						nBufPos += nMaxSize;
						nZipPos = 0;
						::GlobalUnlock(hUnZip); //重新分配内存
						hUnZip = ::GlobalReAlloc(hUnZip, nBufPos + nMaxSize, 0);
						lpZipData = (LPBYTE)::GlobalLock(hUnZip);
					}
					byMaxBuf[nZipPos] = *pbWrite;
					nZipPos ++;//
					nZipSize ++; //add


					pbWrite++;
				}
				m_dwDecodedByte+=wL;
			}

//////////////////////////////
				if(m_iTotalEntry>=4096)
				{
					int _j=0;
				}
				m_LZWTable.AddToChild((WORD)m_iTotalEntry,pbOutData+2,wLastLen+1);
				m_iTotalEntry+=1;
				wOld=wGet;
			}
		}
	}
	delete pbNewData;
	delete pbOutData;
	delete pbBuffer;
	
	if(dwRead != (DWORD)dwLength)
	{
		GlobalFree(hUnZip);
		hUnZip = NULL;
	}

	//存储剩余的压缩数据
	if(nZipPos > 0)
	{
		memcpy(lpZipData + nBufPos, byMaxBuf, nZipPos);
	}
	::GlobalUnlock(hUnZip); //重新分配内存
	hUnZip = ::GlobalReAlloc(hUnZip, nZipSize, 0);

	return hUnZip;
}								

/*
CLZWEncode
*/
HGLOBAL CLZWEncode::BeginLZWEncode(char *chData, int dwLength)
{
	if(!AfxIsValidAddress(chData, dwLength))
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
	int nZipPos = 0; //记下压缩数据的位置
	int nBufPos = 0; //记下当前存储位置hZip
	int nZipSize = 0; //压缩后数据的大小


	m_dwCompressedLength = 0;
	WORD wBufferLen = 1024;
	int iBufferLen = wBufferLen = 1024;
	BYTE bGet;
	int nCurPos = 0;
	m_LZWTable.InitLZWTable();// init the entry table
	m_baContain.InitBits((iBufferLen+8)*8);// init the bit array
	m_baContain.AddBits(LZW_CLEAR_CODE,9);// add the first clear code
	/// below : begin the algorithm
	PLZWENCODEENTRY pCurrent = m_LZWTable.GetHead();
	int iBitWidth;
	DWORD dwEncoded=0;
	for(DWORD i = 0; i < (DWORD)dwLength; i ++)
	{// for each byte
		bGet = chData[nCurPos]; // add
		nCurPos ++;

		PLZWENCODEENTRY pChild=m_LZWTable.FindMatchChild(bGet,pCurrent);
		if(pChild)
		{// has found the continue
			pCurrent=pChild;
		}
		else
		{// not find write last code & add new entry 
			iBitWidth=GetBitWidth();
			WORD wW=pCurrent->wCode;
			m_baContain.AddBits(wW,iBitWidth);// add last code to buffer
			m_LZWTable.AddToChild(bGet,pCurrent);// add last code to table
			if(m_baContain.GetBytesWidth()>(DWORD)iBufferLen)
			{
				m_dwCompressedLength+=(DWORD)iBufferLen;

				//存储这个压缩
				if(nZipPos + iBufferLen >= nMaxSize)
				{
					memcpy(lpZipData + nBufPos, byMaxBuf, nZipPos);
					nBufPos += nZipPos;
					nZipPos = 0;
					::GlobalUnlock(hZip); //重新分配内存
					hZip = ::GlobalReAlloc(hZip, nBufPos + nMaxSize, 0);
					lpZipData = (LPBYTE)::GlobalLock(hZip);
				}

				memcpy(byMaxBuf + nZipPos, m_baContain.GetBits(), iBufferLen);
				nZipPos += iBufferLen;
				nZipSize += iBufferLen; //add

				m_baContain.RemoveBytes(NULL,iBufferLen);
			}
			if(m_LZWTable.GetTableEntryNumber()>= (DWORD)(m_wMaxEntry-3))
			{
				iBitWidth=GetBitWidth();
				m_baContain.AddBits(LZW_CLEAR_CODE,iBitWidth);
				m_LZWTable.InitLZWTable();
			}
			pCurrent=m_LZWTable.FindMatchChild(bGet,m_LZWTable.GetHead());
		}
		dwEncoded++;
	}
	iBitWidth=GetBitWidth();
	m_baContain.AddBits(pCurrent->wCode,iBitWidth);// add last code to buffer


	iBitWidth=GetBitWidth();
	m_baContain.AddBits(LZW_END_CODE,iBitWidth);
	m_dwCompressedLength+=m_baContain.GetBytesWidth();
	iBufferLen = m_baContain.GetBytesWidth();
	//存储这个压缩
	if(nZipPos + iBufferLen >= nMaxSize)
	{
		memcpy(lpZipData + nBufPos, byMaxBuf, nZipPos);
		nBufPos += nZipPos;
		nZipPos = 0;
		::GlobalUnlock(hZip); //重新分配内存
		hZip = ::GlobalReAlloc(hZip, nBufPos + nMaxSize, 0);
		lpZipData = (LPBYTE)::GlobalLock(hZip);
	}

	memcpy(byMaxBuf + nZipPos, m_baContain.GetBits(), iBufferLen);
	nZipPos += iBufferLen;
	nZipSize += iBufferLen; //add
	

	m_LZWTable.ClearLZWTable();
	m_baContain.ClearBits();

	//存储剩余的压缩数据
	if(nZipPos > 0)
	{
		memcpy(lpZipData + nBufPos, byMaxBuf, nZipPos);
	}
	::GlobalUnlock(hZip); //重新分配内存
	hZip = ::GlobalReAlloc(hZip, nZipSize, 0);


	return hZip;
}

int CLZWEncode::GetBitWidth(void)
{
	int iTotal=(int)m_LZWTable.GetTableEntryNumber();
	iTotal>>=9;
	int iBitWidth=9;
	while(iTotal>0)
	{
		iTotal>>=1;
		iBitWidth+=1;
	}
	return iBitWidth;
}

HGLOBAL C_LZW::Encode(char *chData, int nSize)
{
	CLZWEncode cl;
	return cl.BeginLZWEncode(chData, nSize);
}

HGLOBAL C_LZW::Decode(char *chZipData, int nZipSize)
{
	CLZWDecode cl;
	return cl.BeginLZWDecode(chZipData, nZipSize);
}

///////////////////////////////////////////////////////////////////////////////
//LZW Part END--------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

