//***************************************************************************
//	Description		: Read and Write Ini File Class
//  File Name		: SkinIniFile.h
//***************************************************************************
#if !defined(AFX_SKININIFILE_H__A5B8049E_EE8B_40F1_A9E0_AB25D589F30E__INCLUDED_)
#define AFX_SKININIFILE_H__A5B8049E_EE8B_40F1_A9E0_AB25D589F30E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CSkinSection
{
public:
	BOOL ParseStringToTime(const CString& str,CTime& tm);
	BOOL GetKeyValue(LPCTSTR strKey,CTime& time);
	BOOL SetKeyValue(LPCTSTR strKey,const CTime& time);
	BOOL ParseStringToColor(const CString& str,COLORREF& color);
	BOOL ParseStringToPoint(const CString& str,CPoint& point);
	BOOL ParseStringToRect(const CString& str,CRect& rect);
	BOOL ParseStringToStringArray(const CString& str,CStringArray& strArr,TCHAR ch);
	BOOL GetKeyValue(LPCTSTR strKey,CStringArray &strArr,TCHAR ch=',');
	BOOL SetKeyValue(LPCTSTR strKey,const CStringArray& strArr,TCHAR ch=',');
	BOOL SetKeyValue(LPCTSTR strKey,double fVal);
	BOOL GetKeyValue(LPCTSTR strKey,double& fVal);
	BOOL SetKeyValue(LPCTSTR strKey,FLOAT fVal);
	BOOL GetKeyValue(LPCTSTR strKey,FLOAT& fVal);
	BOOL GetKeyValue(LPCTSTR strKey,long& lVal);
	BOOL SetKeyValue(LPCTSTR strKey,long  lVal);

	BOOL SetKeyValue(LPCTSTR strKey,const CRect& rect);
	BOOL SetKeyValue(LPCTSTR strKey,const CSize& size);
	BOOL SetKeyValue(LPCTSTR strKey,const CPoint& point);
	BOOL SetKeyValue(LPCTSTR strKey,DWORD dwVal);
	BOOL SetKeyValue(LPCTSTR strKey,UINT nVal);
	BOOL SetKeyValue(LPCTSTR strKey,int nVal);
	BOOL SetKeyValue(LPCTSTR strKey, LPCTSTR strVal);

	BOOL GetKeyValue(LPCTSTR strKey,CString& strVal);
	
	BOOL GetKeyValue(LPCTSTR strKey,CRect& rect);
	BOOL GetKeyValue(LPCTSTR strKey,CSize& size);
	BOOL GetKeyValue(LPCTSTR strKey,CPoint& point);
	
	BOOL GetKeyValue(LPCTSTR strKey,DWORD& dwVal);
	BOOL GetKeyValue(LPCTSTR strKey,UINT& nVal);
	BOOL GetKeyValue(LPCTSTR strKey,int& nVal);
	friend class CSkinIniFile;
	
protected:
	CString				m_strSecName;
	CMapStringToString	m_mapKey;
	
};

class CSkinIniFile  
{
public:
	CList <	CSkinSection*,CSkinSection*>	m_listSection;
public:
	CSkinIniFile();
	virtual ~CSkinIniFile();
protected:
	CString		m_strError;		
	CString		m_strFilePath;	
	
public:
	BOOL SetValue(LPCTSTR strSec,LPCTSTR strKey, CStringArray &strArr, TCHAR ch);
	BOOL GetValue(LPCTSTR strSec,LPCTSTR strKey, CStringArray &strArr, TCHAR ch);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,const CRect& rect);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,const CSize& size);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,const CPoint& point);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,double fVal);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,DWORD dwVal);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,UINT nVal);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,int nVal);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,CRect& rect);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,CSize& size);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,CPoint& point);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,double& fVal);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,DWORD& dwVal);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,UINT& nVal);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,int& nVal);
	BOOL SetValue(LPCTSTR strSec, LPCTSTR strKey,FLOAT fVal);
	BOOL GetValue(LPCTSTR strSec, LPCTSTR strKey,FLOAT& fVal);

	int GetKeyCount(LPCTSTR strSec);
	int GetSectionCount();
	BOOL SetValue(LPCTSTR strSec,LPCTSTR strKey,LPCTSTR strVal);
	BOOL GetValue(LPCTSTR strSec,LPCTSTR strKey,CString& strValue);
	BOOL IsSectionExist(LPCTSTR strSec);
	CString GetErrorString();
	BOOL WriteFile();
	BOOL ReadFile();
	CSkinIniFile(LPCTSTR strPath);
	void SetFilePath(LPCTSTR strPath);
	CString GetFilePath();		
	void DeleteAllSection();
	CSkinSection* GetSectionPtr(LPCTSTR strSec);

protected:
	POSITION GetSectionPos(LPCTSTR strSec);
	
};

#endif // !defined(AFX_SKININIFILE_H__A5B8049E_EE8B_40F1_A9E0_AB25D589F30E__INCLUDED_)
