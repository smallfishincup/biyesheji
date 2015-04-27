// ValidFunction.cpp: implementation of the CValidFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ValidFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CValidFunction
CString CValidFunction::GetPathFileName(CString strPath)
{
	CString strFileName;
	strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);//ȥ���ַ������"\\"
	return strFileName;
}

BOOL CValidFunction::GetFileExt(CString strFilename,CString &strExt)
{
	if(strFilename.IsEmpty() || strFilename.Find('.') == -1)
		return FALSE;

	strExt = strFilename.Right(strFilename.GetLength() - strFilename.ReverseFind('.') - 1);

	return TRUE;
}

BOOL CValidFunction::IsFileExist(LPCTSTR strFileName)
{
	if(strFileName == NULL)
		return FALSE;

	DWORD dwAttr = ::GetFileAttributes(strFileName);
	return (dwAttr!=-1 && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY) );
}

BOOL CValidFunction::IsPathExist(LPCTSTR strPath)
{
	if(strPath == NULL)
		return FALSE;

	CString t_PathName = strPath;
	t_PathName.Replace('/','\\');
	DWORD dwAttr = ::GetFileAttributes(t_PathName);

	return (dwAttr!=-1 && (dwAttr&FILE_ATTRIBUTE_DIRECTORY) );
}

void CValidFunction::GetWorkPath(CString &strPath)
{
	char FilePath[_MAX_PATH];
	DWORD dwSize = GetModuleFileName(NULL,FilePath,_MAX_PATH);
	strPath = FilePath;
	strPath = strPath.Left(strPath.ReverseFind('\\'));//ȥ���ַ������"\\"
}

CString CValidFunction::ParseFileName(CString strPathFullName)
{
	CString strFileName;
	
	strFileName = strPathFullName.Right(strPathFullName.GetLength() - strPathFullName.ReverseFind('\\') - 1);

	return strFileName;
}

BOOL CValidFunction::CreatePath(LPCTSTR lpszDir)
{
	if(lpszDir==NULL || _tcslen(lpszDir)==0)
		return FALSE;
	
	// base case . . .if directory exists
	if(IsPathExist(lpszDir))
		return TRUE;

	CString strDir(lpszDir);
	if(strDir.Right(1) == "\\")
		strDir = strDir.Left(strDir.GetLength()-1); 

	 // recursive call, one less directory
	int nFound = strDir.ReverseFind('\\');
	CString strSubDir = strDir.Left(nFound);

	if(! CreatePath(strSubDir) )
		return FALSE;

	return ::CreateDirectory(strDir,NULL);
}


int CValidFunction::IsInStringArray(const CStringArray &strArr, const CString &str)
{
	int nIndex = -1;
	for (int i=0; i< strArr.GetSize(); i++)
	{
		if (str.Compare(strArr[i]) == 0)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

int CValidFunction::IsInStringArrayNoCase(const CStringArray &strArr, const CString &str)
{
	int nIndex = -1;
	for (int i=0; i< strArr.GetSize(); i++)
	{
		if (str.CompareNoCase(strArr[i]) == 0)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

void CValidFunction::GetWindowDisk(CString &strDisk)
{
	CString strExe;
	TCHAR pBuf[MAX_PATH];
	::GetWindowsDirectory(pBuf,MAX_PATH);
	strExe = pBuf;
	strDisk = strExe.Left(3);	
}

void CValidFunction::GetWindowDir(CString &strDir)
{
	CString strExe;
	TCHAR pBuf[MAX_PATH];
	::GetWindowsDirectory(pBuf,MAX_PATH);
	strExe = pBuf;
	strDir = strExe;
}

void CValidFunction::GetProgramFileDir(CString &strDir)
{
	CString strExe;
	TCHAR pBuf[MAX_PATH];
	::GetWindowsDirectory(pBuf,MAX_PATH);
	strExe = pBuf;
	strExe = strExe.Left(3);
	strDir = strExe + _T("Program Files");
}

CString CValidFunction::LongPathToShort(LPCTSTR strPath)
{
	CString strShort;
	TCHAR  pBuf[MAX_PATH];
	DWORD dwLen = ::GetShortPathName(strPath,pBuf,MAX_PATH);
	if (dwLen > 0)
		strShort = pBuf;
	return strShort;
}

CString CValidFunction::ShortPathToLong(LPCTSTR strPath)
{
	CString strLong;
	TCHAR pBuf[MAX_PATH];
	DWORD dwLen = ::GetLongPathName(strPath,pBuf,MAX_PATH);
	if (dwLen > 0)
		strLong = pBuf;
	return strLong;
}

CString CValidFunction::GetLONGLONGString(LONGLONG llValue)
{
	CString strRes = "";
	char strTemp[_MAX_PATH];
	LONGLONG llTemp = llValue;
	CString strValue = "";
	BYTE bTemp = 0;
	while(llTemp > 0)
	{
		bTemp = (BYTE)(llTemp % 10);
		llTemp = llTemp / 10;
		sprintf_s(strTemp, "%x", bTemp);
		strValue = strTemp;
		strRes = strValue + strRes;
	}
	return strRes;
}

CString CValidFunction::CombineFilePath(LPCTSTR lpszPathName, LPCTSTR lpszFileName)
{
	CString str(lpszPathName);
	if(str.Right(1) != '\\' && *lpszFileName!='\\')
		str += '\\';
	str += lpszFileName;
	return str;
}
