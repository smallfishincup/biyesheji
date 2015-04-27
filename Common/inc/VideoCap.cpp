// VideoCap.cpp: implementation of the CVideoCap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoCap.h"

CVideoCap* g_pGrabber = NULL;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVideoCap::CVideoCap()
{
	m_dwLastCallback	= 0;
	m_dwImageDataSize	= 0;
	m_pImageData		= NULL;
	m_dwImageNew		= VIDEO_SIZE;
	m_hWndCapture		= NULL;
	m_hWndCap			= NULL;
}

CVideoCap::~CVideoCap()
{
	Destroy();
}

void CVideoCap::Destroy()
{
	try
	{
		capCaptureAbort(m_hWndCapture);
		capSetCallbackOnVideoStream(m_hWndCapture, NULL);
		
		//	capPreview(m_hWndCapture, FALSE);	
		capDriverDisconnect(m_hWndCapture);
		//	capCaptureStop(m_hWndCapture);
		//	capCaptureSingleFrameClose(m_hWndCapture);
		
		::SendMessage(m_hWndCapture, WM_CLOSE, NULL, NULL);
		::SendMessage(m_hWndCapture, WM_DESTROY, NULL, NULL);
		
		SAFE_DELETE(m_pImageData);
	}
	catch(...)
	{
		DW("Catch Camera Destroy");
	}

	g_pGrabber		= NULL;
	m_hWndCapture	= NULL;
}

void CVideoCap::SetImageData(LPVOID pData,DWORD dwData)
{
	if(!m_bValidCallHint || m_pImageData == NULL || dwData == 0)
		return;
	
	if(dwData > m_dwImageNew)
	{
		m_dwImageNew = dwData;
		SAFE_DELETE(m_pImageData);

		m_pImageData = new BYTE[m_dwImageNew];
		memset(m_pImageData,0,m_dwImageNew);
	}

	memcpy(m_pImageData,pData,dwData);
	m_dwImageDataSize	= dwData;

	m_dwLastCallback	= timeGetTime();
}

LPBYTE CVideoCap::GetImageData()
{
	CheckPointer(m_pImageData,NULL);
	
	DWORD dwTick = (timeGetTime() - m_dwLastCallback);
	if(dwTick > 20)
		capGrabFrameNoStop(m_hWndCapture);

	return (m_pImageData);
}

DWORD CVideoCap::GetImageDataSize()
{
	return m_dwImageDataSize;
}

LRESULT PASCAL CVideoCap::WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return ::DefWindowProc (hwnd, uMsg, wParam, lParam) ;
}

BOOL CVideoCap::Create()
{
	if(g_pGrabber)
		return FALSE;
	
	try
	{
		WCHAR szClassName[] = L"_capWindow_";
		WNDCLASS wc			= {0};
		wc.style			= CS_HREDRAW | CS_VREDRAW ;
		wc.lpfnWndProc		= CVideoCap::WindowProc;
		wc.cbClsExtra		= 0 ;
		wc.cbWndExtra		= 0 ;
		wc.hInstance		= GetModuleHandle(NULL);
		wc.hIcon			= LoadIcon (NULL, IDI_APPLICATION) ;
		wc.hCursor			= LoadCursor (NULL, IDC_ARROW) ;
		wc.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH) ;
		wc.lpszMenuName		= NULL ;
		wc.lpszClassName	= (LPCSTR)szClassName;
		
		RegisterClass(&wc);
		m_hWndCap = CreateWindowEx(0, (LPCSTR)szClassName, "Camera", NULL, 0, 0, 320, 240, NULL, NULL, NULL, NULL);
		
		if(!IsWindow(m_hWndCap))
			return FALSE;
		
		m_hWndCapture = ::capCreateCaptureWindow(
			_T("Capture Window"),
			WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			0, 0, 320, 240,
			(HWND) m_hWndCap,
			(int) 1);
		
		CheckPointer(m_hWndCapture,FALSE);
		
		::SetWindowLong(m_hWndCapture,GWL_EXSTYLE,WS_EX_TOOLWINDOW | WS_EX_STATICEDGE);	
		::MoveWindow(m_hWndCapture,0,0,0,0,TRUE);
		
		if (!capDriverConnect(m_hWndCapture,0))
		{
			DW("capDriverConnect Failed.");
			m_hWndCapture = NULL;
			capDriverDisconnect(m_hWndCapture);
			return FALSE;
		}
		
		if (!capGetVideoFormat(m_hWndCapture,&m_BitmapInfo,sizeof(m_BitmapInfo)))
		{
			DW("capGetVideoFormat Failed.");
			m_hWndCapture = NULL;
			capDriverDisconnect(m_hWndCapture);
			return FALSE;
		}
		
		m_BitmapInfo.bmiHeader.biHeight	= VIDEO_HEIGHT;
		m_BitmapInfo.bmiHeader.biWidth	= VIDEO_WIDTH;
		if (!capSetVideoFormat(m_hWndCapture,&m_BitmapInfo,sizeof(BITMAPINFO)))
		{
			DW("capSetVideoFormat Failed.");
			m_hWndCapture = NULL;
			capDriverDisconnect(m_hWndCapture);
			return FALSE;
		}
		
		if (!capPreviewRate(m_hWndCapture,10))
		{
			DW("capPreviewRate Failed.");
			m_hWndCapture = NULL;
			capDriverDisconnect(m_hWndCapture);
			return FALSE;
		}
		
		g_pGrabber		= this;
		m_pImageData	= new BYTE[VIDEO_SIZE];
		memset(m_pImageData,0,VIDEO_SIZE);
		
		if (!capSetCallbackOnFrame(m_hWndCapture,FrameCallBack))
		{
			DW("capSetCallbackOnFrame Failed.");
			m_hWndCapture = NULL;
			capDriverDisconnect(m_hWndCapture);
			return FALSE;
		}
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

static LRESULT CALLBACK FrameCallBack(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	CheckPointer(g_pGrabber,FALSE);

	try
	{
		g_pGrabber->m_bValidCallHint = TRUE;
		g_pGrabber->SetImageData(lpVHdr->lpData,lpVHdr->dwBufferLength);
		g_pGrabber->m_bValidCallHint = FALSE;		
	}
	catch(...)
	{
		DW("Catch Camera FrameCallBack");
	}

	return TRUE;
}
