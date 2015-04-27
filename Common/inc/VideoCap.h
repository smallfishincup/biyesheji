#ifndef _VIDEO_CAP_H
#define _VIDEO_CAP_H

//-----------------------------------------------------------------------

#include <vfw.h>
#pragma comment(lib,"VFW32.LIB")
#pragma comment(lib, "winmm.lib")

//-----------------------------------------------------------------------

LRESULT CALLBACK FrameCallBack(HWND hWnd, LPVIDEOHDR lpVHdr);

class CVideoCap : public CWnd
{
public:
	CVideoCap();
	virtual ~CVideoCap();
	
	static LRESULT PASCAL WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	BOOL				Create();
	void				SetImageData(LPVOID pData,DWORD dwData);
	LPBYTE				GetImageData();
	DWORD				GetImageDataSize();

	void				Destroy();

public:
	BITMAPINFO			m_BitmapInfo;
	DWORD				m_dwLastCallback;

	PBYTE				m_pImageData;
	DWORD				m_dwImageNew;
	DWORD				m_dwImageDataSize;

	HWND				m_hWndCapture;
	HWND				m_hWndCap;

	BOOL				m_bValidCallHint;
};

#endif