#ifndef _DT_H_
#define _DT_H_

#include <stdio.h>
//#ifdef __cplusplus

//错误级别
#define DT_ERROR		0x0010
#define DT_WARNNING		0x0100
#define DT_TRACE		0x1000

class _CDT
{
public:
    _CDT(const char* lpszFile, int nLine,int nLevel)
		:m_lpszFile(lpszFile ),m_nLine(nLine),m_nLevel(nLevel)
    {
    }

    int operator()( const char* lpszFormat, ... )
    {
		char szFormat[1024];
		char szMsg[1024];
		sprintf_s(szFormat,"$$@@%s(%d)$$@@%d$$@@%s",m_lpszFile,m_nLine,m_nLevel,lpszFormat);
		lpszFormat = szFormat;
		
		va_list argList;
		va_start(argList, lpszFormat);
		vsprintf_s(szMsg,lpszFormat, argList);
		va_end(argList);

		int nLen = strlen(szMsg);
		for(int i=0;i<nLen;i++)
		{
			if(szMsg[i]=='\n')
			   szMsg[i]= '^';
		}
		
		//send message to dt.exe
		{
			static HWND hwnd = ::FindWindowEx(NULL,NULL,NULL,"DT ");
			if(!IsWindow(hwnd))
			{
				hwnd = ::FindWindowEx(NULL,NULL,NULL,"DT ");
				if(!hwnd)
					hwnd = ::FindWindow(NULL,"DebugHelper ");//向后兼容,旧版名称
			}

			if(hwnd)
			{
				//::SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR)szMsg);
				DWORD dwpRet = 0;
				LRESULT ret = ::SendMessageTimeout(hwnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR)szMsg,
					SMTO_BLOCK,1000*1000,&dwpRet);
				if(!ret)
				{
					DWORD dwErr = GetLastError();
				}
			}
		}
		return 0;
    }
protected:
    const char* m_lpszFile;
    int m_nLine;
	int m_nLevel;
};

//定义_DT启用DT,DW,DE输出
//undef _DT禁用DT,DW,DE输出,禁用后,这些宏不会对程序产生任何影响,也不会向程序增加__FILE__等字符串.
#define _DT
//#undef  _DT
#ifdef _DT
	#define DT	(_CDT( __FILE__, __LINE__,DT_TRACE))
	#define DW	(_CDT( __FILE__, __LINE__,DT_WARNNING))
	#define DE	(_CDT( __FILE__, __LINE__,DT_ERROR))
	#define ONLY_ONCE(x)	\
	do{						\
		static BOOL b(x);	\
	}while(0)
#else
	#define DT	0
	#define DW	0
	#define DE	0
	#define ONLY_ONCE	0
#endif

#ifndef COUNT_OF
	#define COUNT_OF(x) (sizeof(x)/sizeof((x)[0]))
#endif

#endif
