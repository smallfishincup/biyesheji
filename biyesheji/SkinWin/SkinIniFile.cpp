// SkinIniFile.cpp: implementation of the CSkinIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkinIniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinIniFile::CSkinIniFile()
{
	m_strError = "No error";
}

CSkinIniFile::~CSkinIniFile()
{
	DeleteAllSection();
}

CString CSkinIniFile::GetFilePath()
{
	return m_strFilePath;
}

//设置新的文件路径,读写文件均可
void CSkinIniFile::SetFilePath(LPCTSTR strPath)
{
	m_strFilePath = strPath;
}

CSkinIniFile::CSkinIniFile(LPCTSTR strPath)
{
	m_strFilePath = strPath;
	m_strError = "No error";
}

//从ini文件读取
BOOL CSkinIniFile::ReadFile()
{
	//是否存在该文件
	CFile	fl;
	CFileStatus flStatus;
	if (!CFile::GetStatus(m_strFilePath,flStatus))
	{
		m_strError = "File not exist!";
		return false;
	}
	CStdioFile	iniFile;
	if (!iniFile.Open(m_strFilePath,CFile::modeRead|CFile::typeText))
	{
		m_strError = "Unable to open ini File!";
		return false;
	}
	CString strLine;
	CString str;
	int nIndex = 0;
	int nKeySel = 0;
	int nLen =0;
	CSkinSection* pSec = NULL;
	try
	{
		while (iniFile.ReadString(strLine))	//读一行
		{
			nLen = strLine.GetLength();
			if ( nLen <= 0)
				continue;
			if (strLine[0] == '[')				//是区段头部
			{
				str = strLine.Mid(1,nLen - 2);
				pSec = new CSkinSection;
				pSec->m_strSecName = str;
				m_listSection.AddTail(pSec);
				nIndex ++;
				nKeySel = 0;
			}
			else if(pSec)		//键值对
			{
				int nPos = strLine.Find('=');
				if (nPos < 0)
					continue;
				str = strLine.Left(nPos);
				pSec->m_mapKey.SetAt(str,strLine.Mid(nPos + 1));
				nKeySel ++;
			}
		}
	}
	catch(...)
	{
		m_strError = "Unable to read file!";
	}
	iniFile.Close();
	return true;
}

//写到文件中
BOOL CSkinIniFile::WriteFile()
{
	CStdioFile	iniFile;
	if (!iniFile.Open(m_strFilePath,CFile::modeCreate|CFile::modeWrite|CFile::typeText))
	{
		m_strError = "Unable to open ini File!";
		return FALSE;
	}

	CString strLine;
	CSkinSection* pSec = NULL;

	try
	{
		POSITION pos = m_listSection.GetHeadPosition();
		while(pos)
		{
			pSec = m_listSection.GetNext(pos);
			strLine = "[" + pSec->m_strSecName + "]";	//写头部
			iniFile.WriteString(strLine);
			iniFile.WriteString("\n");
			
			POSITION posMap = pSec->m_mapKey.GetStartPosition();
			CString strKey,strVal;
			while (posMap)								//区段数据
			{
				pSec->m_mapKey.GetNextAssoc(posMap,strKey,strVal);
				strLine = strKey + "=" + strVal;
				iniFile.WriteString(strLine);
				iniFile.WriteString("\n");
			}
		}
	}
	catch (...)
	{
		m_strError = "Unable to write file!";
	}
	iniFile.Close();

	return TRUE;
}

//删除所有区段
void CSkinIniFile::DeleteAllSection()
{
	POSITION pos = m_listSection.GetHeadPosition();
	while(pos)
	{
		delete m_listSection.GetNext(pos);
	}
	m_listSection.RemoveAll();
}

//返回出错字符串
CString CSkinIniFile::GetErrorString()
{
	return m_strError;	
}

//该区段是否存在
BOOL CSkinIniFile::IsSectionExist(LPCTSTR strSec)
{
	POSITION pos = m_listSection.GetHeadPosition();
	CSkinSection* pSec = NULL;
	while (pos)
	{
		pSec = m_listSection.GetNext(pos);
		if (pSec->m_strSecName.Compare(strSec) == 0)
			return TRUE;
	}
	return FALSE;
}

//取得键值
BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, CString &strValue)
{
	POSITION pos = GetSectionPos(strSec);
	if (pos == NULL)
		return FALSE;
	CSkinSection* pSec = m_listSection.GetAt(pos);
	if (!pSec->m_mapKey.Lookup(strKey,strValue))
		return FALSE;
	return TRUE;
}

//设置键值
BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, LPCTSTR strVal)
{
	POSITION pos = GetSectionPos(strSec);
	CSkinSection* pSec = NULL;
	if (pos == NULL)	//不存在就创建一个新的区段
	{
		pSec = new CSkinSection;
		pSec->m_strSecName = strSec;
		m_listSection.AddTail(pSec);
	}
	else
	{
		pSec = m_listSection.GetAt(pos);		
	}
	pSec->m_mapKey.SetAt(strKey,strVal);
	return TRUE;
}

//取得区段的位置
POSITION CSkinIniFile::GetSectionPos(LPCTSTR strSec)
{
	POSITION pos = m_listSection.GetHeadPosition();
	POSITION oldPos = pos;
	CSkinSection* pSec = NULL;
	while (pos)
	{
		oldPos = pos;
		pSec = m_listSection.GetNext(pos);
		if (pSec->m_strSecName.Compare(strSec) == 0)
			return oldPos;
	}
	return NULL;
}

//取得区段的对象指针
CSkinSection* CSkinIniFile::GetSectionPtr(LPCTSTR strSec)
{
	POSITION pos = m_listSection.GetHeadPosition();
	CSkinSection* pSec = NULL;
	while (pos)
	{
		pSec = m_listSection.GetNext(pos);
		if (pSec->m_strSecName.Compare(strSec) == 0)
			return pSec;
	}
	pSec = new CSkinSection;
	pSec->m_strSecName = strSec;
	m_listSection.AddTail(pSec);

	return pSec;
}

int CSkinIniFile::GetSectionCount()
{
	return m_listSection.GetCount();
}

int CSkinIniFile::GetKeyCount(LPCTSTR strSec)
{
	
	CSkinSection* pSec = (CSkinSection*)GetSectionPtr(strSec);

	if (pSec == NULL)
		return 0;

	return pSec->m_mapKey.GetCount();
}

//将字符串解析为CRect
BOOL CSkinSection::ParseStringToRect(const CString &str, CRect &rect)
{
	int nLen = str.GetLength();
	if ( nLen < 7)
		return FALSE;

	int nHead = -1;
	int nTail = -1;
	int nVal[4] = {0,0,0,0};
	for (int i=0; i<4; i++)
	{
		nHead = nTail + 1;
		nTail = str.Find(',',nHead + 1);
		if (nTail < 0)
			nTail = nLen;
		nVal[i] = atoi(str.Mid(nHead,nTail - nHead));
	}

	rect.left = nVal[0];
	rect.top  = nVal[1];
	rect.right = nVal[2];
	rect.bottom = nVal[3];

	return TRUE;
}

//将字符串解析为CPoint
BOOL CSkinSection::ParseStringToPoint(const CString &str, CPoint &point)
{
	int nLen = str.GetLength();
	if (nLen < 3)
		return false;
	int nPos = str.Find(',');
	if (nPos < 0)
		return false;
	point.x = atoi(str.Mid(0,nPos));
	point.y = atoi(str.Mid(nPos+1,nLen));

	return TRUE;
}

BOOL CSkinSection::ParseStringToColor(const CString &str, COLORREF &color)
{
	if (str.GetLength() < 5)
		return false;
	int nPos1 = str.Find(',',0);
	if (nPos1 == -1)
		return false;
	int nPos2 = str.Find(',',nPos1+1);
	if (nPos2 == -1)
		return false;
	int	nRed = atoi(str.Mid(0,nPos1));
	int nGreen = atoi(str.Mid(nPos1+1,nPos2 - nPos1 -1));
	int nBlue = atoi(str.Mid(nPos2+1));
	color = RGB(nRed,nGreen,nBlue);

	return TRUE;
}

BOOL CSkinSection::ParseStringToStringArray(const CString &str, CStringArray &strArr, TCHAR ch)
{
	int nLength = str.GetLength();
	if (nLength <= 0)
		return FALSE;
	int nPos = str.Find(ch,0);
	int nHead = 0;
	while (nPos > 0)
	{
		strArr.Add(str.Mid(nHead,nPos - nHead));
		nHead = nPos + 1;
		nPos = str.Find(ch,nHead);
	}
	strArr.Add(str.Mid(nHead,nLength - nHead));

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, int &nVal)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	nVal = atoi(strVal);

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, UINT &nVal)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	nVal = atoi(strVal);

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, DWORD &dwVal)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;
	dwVal = atol(strVal);

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CPoint &point)
{
	CString strVal;
	
	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	if (!ParseStringToPoint(strVal,point))
		return FALSE;

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CSize &size)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	CPoint point(size.cx,size.cy);

	if (!ParseStringToPoint(strVal,point))
		return FALSE;

	size.cx = point.x;
	size.cy = point.y;

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CRect &rect)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	if (!ParseStringToRect(strVal,rect))
		return FALSE;

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CString &strVal)
{
	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CStringArray &strArr, TCHAR ch)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	return ParseStringToStringArray(strVal,strArr,ch);
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey,LPCTSTR strVal)
{
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, int nVal)
{
	CString strVal;
	strVal.Format(_T("%d"),nVal);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;	
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, UINT nVal)
{
	CString strVal;
	strVal.Format(_T("%d"),nVal);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, DWORD dwVal)
{
	CString strVal;
	strVal.Format(_T("%d"),dwVal);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, const CPoint &point)
{
	CString strVal;
	strVal.Format(_T("%d,%d"),point.x,point.y);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, const CSize &size)
{
	CString strVal;
	strVal.Format(_T("%d,%d"),size.cx,size.cy);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, const CRect &rect)
{
	CString strVal;
	strVal.Format(_T("%d,%d,%d,%d"),rect.left,rect.top,rect.right,rect.bottom);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, const CStringArray &strArr,TCHAR ch)
{
	BOOL bRes = FALSE;
	CString	strVal;
	int nSize = strArr.GetSize();
	if (nSize == 0)
		bRes = TRUE;
	else
	{
		for (int i=0; i<nSize - 1; i++)
		{
			strVal += strArr[i];
			strVal += ch;
		}
		strVal += strArr[nSize - 1];
	}
	m_mapKey.SetAt(strKey,strVal);
	
	return bRes;
}


BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, FLOAT &fVal)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	fVal = (FLOAT)atof(strVal);

	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, FLOAT fVal)
{
	CString strVal;
	strVal.Format(_T("%f"),fVal);
	m_mapKey.SetAt(strKey,strVal);
	return TRUE;
}
	
BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, double &fVal)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;

	fVal = atof(strVal);

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey,long& lVal)
{
	CString strVal;
	
	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;
	
	lVal = atol(strVal);
	
	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey,long  lVal)
{
	CString strVal;
	strVal.Format(_T("%d"),lVal);
	m_mapKey.SetAt(strKey,strVal);
	
	return TRUE;
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, double fVal)
{
	CString strVal;
	strVal.Format(_T("%f"),fVal);
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, int &nVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,nVal);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, UINT &nVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;
	
	return pSec->GetKeyValue(strKey,nVal);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, DWORD &dwVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,dwVal);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, double &fVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,fVal);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, CPoint &point)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,point);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, CSize &size)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,size);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, CRect &rect)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,rect);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, int nVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,nVal);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, UINT nVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,nVal);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, DWORD dwVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,dwVal);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, double fVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;
	
	return pSec->SetKeyValue(strKey,fVal);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, const CPoint &point)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,point);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, const CSize &size)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;
	
	return pSec->SetKeyValue(strKey,size);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, const CRect &rect)
{	
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,rect);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, CStringArray &strArr, TCHAR ch)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,strArr,ch);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, CStringArray &strArr, TCHAR ch)
{
	CSkinSection* pSec = GetSectionPtr(strSec);
	
	if (pSec == NULL)
		return FALSE;
	
	return pSec->SetKeyValue(strKey,strArr,ch);
}

BOOL CSkinIniFile::GetValue(LPCTSTR strSec, LPCTSTR strKey, FLOAT &fVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->GetKeyValue(strKey,fVal);
}

BOOL CSkinIniFile::SetValue(LPCTSTR strSec, LPCTSTR strKey, FLOAT fVal)
{
	CSkinSection* pSec = GetSectionPtr(strSec);

	if (pSec == NULL)
		return FALSE;

	return pSec->SetKeyValue(strKey,fVal);
}

BOOL CSkinSection::SetKeyValue(LPCTSTR strKey, const CTime &time)
{
	CString strVal;
	strVal = time.Format(_T("%Y,%m,%d,%H,%M,%S"));
	m_mapKey.SetAt(strKey,strVal);

	return TRUE;
}

BOOL CSkinSection::GetKeyValue(LPCTSTR strKey, CTime &time)
{
	CString strVal;

	if (!m_mapKey.Lookup(strKey,strVal))
		return FALSE;
	
	return ParseStringToTime(strVal,time);
}

BOOL CSkinSection::ParseStringToTime(const CString &str, CTime &tm)
{
	int nLen = str.GetLength();
	if ( nLen < 10)
		return FALSE;

	int nHead = -1;
	int nTail = -1;
	CTime tmNow = CTime::GetCurrentTime();
	int nVal[6] = {tmNow.GetYear(),tmNow.GetMonth(),tmNow.GetDay(),
					tmNow.GetHour(),tmNow.GetMinute(),tmNow.GetSecond()};
	
	for (int i=0; i<6; i++)
	{
		nHead = nTail + 1;
		nTail = str.Find(',',nHead + 1);
		
		if (nTail < 0)
			nTail = nLen;

		nVal[i] = atoi(str.Mid(nHead,nTail - nHead));
	}
	tm = CTime(nVal[0],nVal[1],nVal[2],nVal[3],nVal[4],nVal[5]);
	
	return TRUE;
}
