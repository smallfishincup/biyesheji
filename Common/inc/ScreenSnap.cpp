// ScreenSnap.cpp: implementation of the CScreenSnap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScreenSnap.h"
#include <windowsx.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef DIBSCANLINE_WIDTHBYTES
#define DIBSCANLINE_WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenSnap::CScreenSnap()
{

}

CScreenSnap::~CScreenSnap()
{

}

BOOL CScreenSnap::GetScreenDataSize(DWORD &dwScreenDataSize)
{
	CDC dc;
	dc.CreateDC("DISPLAY",NULL,NULL,NULL);
	CheckPointer(dc.GetSafeHdc(),FALSE);

	CBitmap bm;
	int nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int nHeight	= GetSystemMetrics(SM_CYSCREEN);
	if(!bm.CreateCompatibleBitmap(&dc,nWidth,nHeight))
	{
		dc.DeleteDC();
		return FALSE;
	}

	BITMAP btm;
	bm.GetBitmap(&btm);

	dwScreenDataSize  = btm.bmWidthBytes * btm.bmHeight;
	dwScreenDataSize += sizeof(BITMAPFILEHEADER);
	dwScreenDataSize += sizeof(BITMAPINFOHEADER);
	
	dc.DeleteDC();

	return TRUE;
}

BOOL CScreenSnap::GetScreenData(BYTE* pScreenData,DWORD &dwScreenData)
{
	CDC dc;
	dc.CreateDC("DISPLAY",NULL,NULL,NULL);
	CheckPointer(dc.GetSafeHdc(),FALSE);
	
	CBitmap bm;
	int nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int nHeight	= GetSystemMetrics(SM_CYSCREEN);
	if(!bm.CreateCompatibleBitmap(&dc,nWidth,nHeight))
	{
		dc.DeleteDC();
		return FALSE;
	}

	CDC tdc;
	if(!tdc.CreateCompatibleDC(&dc))
	{
		dc.DeleteDC();
		return FALSE;
	}

	CBitmap*pOld = tdc.SelectObject(&bm);
	if(pOld == NULL)
	{
		tdc.DeleteDC();
		dc.DeleteDC();
		return FALSE;
	}

	tdc.BitBlt(0,0,nWidth,nHeight,&dc,0,0,SRCCOPY);
	tdc.SelectObject(pOld);
	
	BITMAP btm;
	bm.GetBitmap(&btm);
	DWORD nSize  = btm.bmWidthBytes * btm.bmHeight;
//	LPSTR lpData = (LPSTR)GlobalAllocPtr(GPTR,nSize);
//	if(lpData == NULL)
//	{
//		tdc.DeleteDC();
//		dc.DeleteDC();
//		return FALSE;
//	}

	//--------------------------------------------------------

	BITMAPINFOHEADER bih;
	bih.biBitCount		= btm.bmBitsPixel;
	bih.biClrImportant	= 0;
	bih.biClrUsed		= 0;
	bih.biCompression	= 0;
	bih.biHeight		= btm.bmHeight;
	bih.biPlanes		= 1;
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biSizeImage		= nSize;
	bih.biWidth			= btm.bmWidth;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter	= 0;

	//--------------------------------------------------------
	//得到DIB色彩表
	if(!GetDIBits(dc,bm,0,bih.biHeight,(pScreenData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)),(BITMAPINFO*)&bih,DIB_RGB_COLORS))
	{
		tdc.DeleteDC();
		dc.DeleteDC();
		return FALSE;
	}
		
	//--------------------------------------------------------
	
	BITMAPFILEHEADER bfh;
	bfh.bfReserved1	= bfh.bfReserved2 = 0;
	bfh.bfType		= ((WORD)('M'<< 8)|'B');
	bfh.bfSize		= 54 + nSize;
	bfh.bfOffBits	= 54;
	
	memcpy(pScreenData,&bfh,sizeof(BITMAPFILEHEADER));
	dwScreenData = sizeof(BITMAPFILEHEADER);
	
	memcpy(pScreenData + dwScreenData,&bih,sizeof(BITMAPINFOHEADER));
	dwScreenData += sizeof(BITMAPINFOHEADER);
	
//	memcpy(pScreenData,lpData,nSize);
	dwScreenData += nSize;

//	GlobalFreePtr(lpData);

	tdc.DeleteDC();
	dc.DeleteDC();

	return TRUE;
}

HBITMAP CScreenSnap::ChangeDataToBitmap(BYTE* pData)
{
	BITMAPFILEHEADER* pbmfh ;     // DIB位图文件头
	HBITMAP           hBitmap ;
	
	pbmfh = (BITMAPFILEHEADER *)pData;
	if (pbmfh->bfType != * (WORD *) "BM") 
		return NULL ;
	
	HDC hdc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	if(hdc == NULL)
		return NULL;

	// 创建DDB位图
	hBitmap = CreateDIBitmap(hdc,              
							(BITMAPINFOHEADER *) (pbmfh + 1),
							CBM_INIT,
							(BYTE *) pbmfh + pbmfh->bfOffBits,
							(BITMAPINFO *) (pbmfh + 1),
							DIB_RGB_COLORS) ;

	DeleteDC(hdc);

	return hBitmap ;
}


PSCREENCAP_SCREENBITMAP CScreenSnap::ScrCapGetPartScreen(PSCREENCAP_SCREENBITMAP pPrevScreen)
{
	PSCREENCAP_SCREENBITMAP psb;
	SCREEN_HANDLE sh;

	sh.hScrDC	= CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	sh.nWidth	= GetDeviceCaps(sh.hScrDC, HORZRES);
	sh.nHeight	= GetDeviceCaps(sh.hScrDC, VERTRES);
	sh.nDepth	= GetDeviceCaps(sh.hScrDC, BITSPIXEL);
	sh.nSize1	= sh.nWidth * sh.nDepth / 8;
	sh.nSize2	= sh.nWidth * 2;

	if (sh.nWidth != pPrevScreen->Width ||
		sh.nHeight != pPrevScreen->Height ||
		sh.nDepth < pPrevScreen->Depth)
	{
		DeleteDC(sh.hScrDC);
		return NULL;
	}

	sh.hMemDC = CreateCompatibleDC(sh.hScrDC);
	sh.hMemBitmap = CreateCompatibleBitmap(sh.hScrDC, sh.nWidth, 5);
	SelectObject(sh.hMemDC, sh.hMemBitmap);

	sh.pTempBuffer = HeapAlloc(GetProcessHeap(), 0, sh.nWidth * sh.nDepth / 8 * 5);
	psb = (PSCREENCAP_SCREENBITMAP)HeapAlloc(GetProcessHeap(), 0, sizeof(SCREENCAP_SCREENBITMAP) + sh.nWidth * sh.nHeight * 2 + 0x10000);

	if (sh.pTempBuffer != NULL && psb != NULL)
	{
		psb->Width = sh.nWidth;
		psb->Height = sh.nHeight;
		psb->Depth = 16;
		psb->Method = SCREEN_DIFFERENCE;
		sh.pScreenBuffer = pPrevScreen->Bits;
		sh.pOutputBuffer = psb->Bits;

		psb->Size = ScrCapGetChanged(&sh);
	}

	if (sh.pTempBuffer)
		HeapFree(GetProcessHeap(), 0, sh.pTempBuffer);

	if (sh.hScrDC)
		DeleteDC(sh.hScrDC);

	if (sh.hMemDC)
		DeleteDC(sh.hMemDC);

	if (sh.hMemBitmap)
		DeleteObject(sh.hMemBitmap);

	return psb;
}

PSCREENCAP_SCREENBITMAP CScreenSnap::ScrCapGetFullScreen()
{
	PSCREENCAP_SCREENBITMAP psb;
	BITMAPINFOHEADER bih;
	HDC hScrDC, hMemDC;
	HBITMAP hBitmap;
	int nWidth, nHeight, nDepth;

	hScrDC	= CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	nWidth	= GetDeviceCaps(hScrDC, HORZRES);
	nHeight	= GetDeviceCaps(hScrDC, VERTRES);
	nDepth	= GetDeviceCaps(hScrDC, BITSPIXEL);

	hMemDC	= CreateCompatibleDC(hScrDC);
	hBitmap	= CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	SelectObject(hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, 0, 0, SRCCOPY);

	bih.biSize = sizeof(BITMAPINFOHEADER); 
	bih.biWidth = nWidth;
	bih.biHeight = -nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 16;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	psb = (PSCREENCAP_SCREENBITMAP)HeapAlloc(GetProcessHeap(), 0, sizeof(SCREENCAP_SCREENBITMAP) + nWidth * nHeight * 2);

	if (psb != NULL)
	{
		psb->Width = nWidth;
		psb->Height = nHeight;
		psb->Depth = 16;
		psb->Method = SCREEN_FULL;
		psb->Size = nWidth * nHeight * 2;

		GetDIBits(hMemDC, hBitmap, 0, nHeight, psb->Bits, (PBITMAPINFO)&bih, DIB_RGB_COLORS);
	}

	DeleteDC(hMemDC);
	DeleteDC(hScrDC);
	DeleteObject(hBitmap);

	return psb;
}

DWORD CScreenSnap::ScrCapGetChanged(PSCREEN_HANDLE psh)
{
	DWORD h, y, lasty, count;
	DWORD mode, scan;
	BYTE *p;
	
	mode = 0;
	y = 0;
	p = (BYTE *)psh->pOutputBuffer;
	
	do
	{
		switch (mode)
		{
		case 0:
			if (y + 5 >= psh->nHeight)
			{
				mode = 3;
				break;
			}
			
			p += ScrCapLineScan(psh, y, 5, p, &count);		//扫描5行
			y += 5;
			
			if (count)
			{
				scan = y;
				mode = 1;
				y += 7;					//最后一行相同是以7行的跨度扫描
				
				if (count > 1)
					y += 8;				//最后有多行相同时以15行的跨度扫描
				
				if (y >= psh->nHeight)
					y = psh->nHeight - 1;
			}
			break;
			
		case 1:
			p += ScrCapLineScan(psh, y, 1, p, &count);		//加速扫描，一次1行
			
			if (count)
			{
				scan = y;
				y += 15 < psh->nHeight - y ? 15 : psh->nHeight - y;	//没有修改，跳过15行
			}
			else
			{
				mode = 2;
				lasty = y + 1;			//发现有不同的区域，回滚前15行
			}
			break;
			
		case 2:
			h = y - scan > 2 ? 2 : y - scan;
			p += ScrCapLineScan(psh, y -= h, h, p, &count);
			
			if (count == h || h == 1 || y - 1 == scan)
			{
				mode = 0;
				y = lasty;
			}
			break;
			
		case 3:
			h = psh->nHeight - y;
			p += ScrCapLineScan(psh, y, h, p, &count);
			y += h;
		}
	} while (y < psh->nHeight);
	
	return p - (BYTE *)psh->pOutputBuffer;
}


DWORD CScreenSnap::ScrCapLineScan(PSCREEN_HANDLE psh, DWORD y, DWORD height, BYTE *lpBuffer, DWORD *lpCount)
{
	DWORD i, total, width = psh->nWidth;
	PBYTE buffer1 = (PBYTE)psh->pScreenBuffer + width * 2 * y;
	PBYTE buffer2 = (PBYTE)psh->pTempBuffer;
	
	*lpCount = 0;
	ScrCapGetLines(psh, y, height);
	
	for (i = 0, total = 0; i < height; i++)
	{
		PBYTE src = buffer2 + width * 2 * i;
		PBYTE dst = buffer1 + width * 2 * i;
		
		DWORD temp = ScrCapGetDifferences(src, dst, lpBuffer + total + 2, width * 2);
		
		if (temp == 0)
		{
			(*lpCount)++;
			continue;
		}
		
		if (temp >= width * 2)
		{
			*(WORD *)(lpBuffer + total) = (WORD)(0xf8 | ((y + i) << 8));
			memcpy(lpBuffer + total + 2, src, width * 2);
			total += width * 2 + 2;
			memcpy(dst, src, width * 2);
			*lpCount = 0;
		}
		else
		{
			*(WORD *)(lpBuffer + total) = (WORD)(0xf0 | ((y + i) << 8));
			total += temp + 2;
			memcpy(dst, src, width * 2);
			*lpCount = 0;
		}
	}
	
	return total;
}

void CScreenSnap::ScrCapGetLines(PSCREEN_HANDLE psh, int y, int height)
{
	BitBlt(psh->hMemDC, 0, 0, psh->nWidth, psh->nHeight, psh->hScrDC, 0, y, SRCCOPY);
	GetBitmapBits(psh->hMemBitmap, psh->nSize1 * psh->nHeight, psh->pTempBuffer);//获取位数据
	
	if (psh->nDepth <= 16)
		return;
	
	__asm
	{
		mov		eax, psh
			mov		esi, [eax].pTempBuffer
			mov		edi, esi
			mov		ebx, [eax].nSize2
			mov		eax, height
			imul	ebx, eax
			shr		ebx, 1
in1:
		mov		eax, [esi]
			mov		ecx, eax
			mov		edx, eax
			and		eax, 0xf8
			and		ecx, 0xf800
			and		edx, 0xf80000
			shr		eax, 3
			shr		ecx, 6
			shr		edx, 9
			or		eax, ecx		;32位色转16位色（555模式）
			or		eax, edx
			mov		[edi], ax
			add		esi, 4
			add		edi, 2
			dec		ebx
			jnz		in1
	}
}

DWORD CScreenSnap::ScrCapGetDifferences(BYTE *src, BYTE *dst, BYTE *buf, DWORD len)
{
	__asm
	{
		mov		esi, src
			mov		edi, dst
			mov		ebx, buf
			mov		ecx, len
			mov		edx, esi
			cld
			
LoopSame:
		repe	cmpsb			;循环比较相同的象素
			je		FindEnd
			
			lea		eax, [esi - 1]
			sub		eax, edx		;偏移相对于上一个偏移加上一个数据长度
			cmp		eax, 224		;偏移超出0-223时用两字节记录偏移
			jb		WriteOff
			or		ah, 0xe0		;两字节偏移标记
			mov		[ebx], ah
			inc		ebx
			
WriteOff:
		mov		[ebx], al
			inc		ebx
			lea		eax, [esi - 1]	;edx暂存偏移
			jmp		LoopDiffEx
			
StepTwo:
		dec		ecx
			inc		esi
			inc		edi
			
StepOne:
		sub		ecx, 2
			add		esi, 2
			add		edi, 2
			
LoopDiffEx:
		cmp		ecx, 3
			jb		LoopDiff
			
			mov		al, [esi + 2]	;优化比较，适应性强
			cmp		[edi + 2], al
			jne		StepTwo
			
			mov		al, [esi + 1]	;优化比较，减少长度
			cmp		[edi + 1], al
			jne		StepOne
			
LoopDiff:
		repne	cmpsb			;循环比较不同的象素
			jne		WriteLenEx
			dec		ecx
			inc		esi
			inc		edi
			
WriteLenEx:
		mov		eax, esi
			sub		eax, edx
			cmp		eax, 240		;长度超出0-239时用两字节记录长度
			jb		WriteLen
			or		ah, 0xf0		;两字节长度标记
			mov		[ebx], ah
			inc		ebx
			
WriteLen:
		mov		[ebx], al
			inc		ebx
			
WriteDiff:
		mov		al, [edx]
			mov		[ebx], al
			inc		edx
			inc		ebx
			cmp		edx, esi
			jne		WriteDiff
			
			test	ecx, ecx
			jnz		LoopSame
			
FindEnd:
		mov		eax, ebx
			sub		eax, buf
	}

	return 0;
}