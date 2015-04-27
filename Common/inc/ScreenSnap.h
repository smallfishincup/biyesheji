// ScreenSnap.h: interface for the CScreenSnap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENSNAP_H__E9A7BBF4_AA0C_4EBA_81A9_5AC52C1FB88E__INCLUDED_)
#define AFX_SCREENSNAP_H__E9A7BBF4_AA0C_4EBA_81A9_5AC52C1FB88E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScreenSnap  
{
public:
	CScreenSnap();
	virtual ~CScreenSnap();

	BOOL						GetScreenDataSize(DWORD &dwScreenDataSize);
	
	BOOL						GetScreenData(BYTE* pScreenData,DWORD &dwScreenData);
	
	HBITMAP						ChangeDataToBitmap(BYTE* pData);
		

	PSCREENCAP_SCREENBITMAP		ScrCapGetFullScreen();
	PSCREENCAP_SCREENBITMAP		ScrCapGetPartScreen(PSCREENCAP_SCREENBITMAP pPrevScreen);
	DWORD						ScrCapGetChanged(PSCREEN_HANDLE psh);
	DWORD						ScrCapLineScan(PSCREEN_HANDLE psh, DWORD y, DWORD height, BYTE *lpBuffer, DWORD *lpCount);
	DWORD						ScrCapGetDifferences(BYTE *src, BYTE *dst, BYTE *buf, DWORD len);
	void						ScrCapGetLines(PSCREEN_HANDLE psh, int y, int height);
	
		
};


#endif // !defined(AFX_SCREENSNAP_H__E9A7BBF4_AA0C_4EBA_81A9_5AC52C1FB88E__INCLUDED_)
