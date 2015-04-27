//***************************************************************************
//	Description		: Valid Function Class
//  File Name		: ValidFunction.h
//***************************************************************************

#if !defined(AFX_VALIDFUNCTION_H__F23994E6_9038_47E3_8716_9E907ADEC71E__INCLUDED_)
#define AFX_VALIDFUNCTION_H__F23994E6_9038_47E3_8716_9E907ADEC71E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CValidFunction  
{

public:
	static void GetProgramFileDir(CString& strDir);
	static void GetWindowDir(CString& strDir);
	static void GetWindowDisk(CString& strDisk);
	static void SetStaticText(CDialog* pDlg,UINT nID,LPCTSTR strText);
	static void GetWorkPath(CString& strPath);

	static int IsInStringArrayNoCase(const CStringArray &strArr, const CString &str);
	static int IsInStringArray(const CStringArray & strArr,const CString& str);
	
	static BOOL IsPathExist(LPCTSTR strPath);
	static BOOL IsFileExist(LPCTSTR strFileName);
	static BOOL CreatePath(LPCTSTR strPath);
	static BOOL GetFileExt(CString strFilename,CString &strExt);		
	
	static CString GetLONGLONGString(LONGLONG llValue);
	static CString CombineFilePath(LPCTSTR lpszPathName, LPCTSTR lpszFileName);
	static CString ParseFileName(CString strPathFullName);
	static CString ShortPathToLong(LPCTSTR strPath);
	static CString LongPathToShort(LPCTSTR strPath);
	static CString GetPathFileName(CString strPath);
	
};

#endif // !defined(AFX_VALIDFUNCTION_H__F23994E6_9038_47E3_8716_9E907ADEC71E__INCLUDED_)
