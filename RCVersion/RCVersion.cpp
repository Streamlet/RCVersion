#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "RCVersion.h"
#include <Windows.h>
#include <tchar.h>
#include <xl/Objects/xlString.h>


xl::String RCModifyStandardVersion(const xl::String &strRCData, const xl::String &strKeyword, WORD wMajor, WORD wMinor, WORD wBuild, WORD wRevision)
{
    xl::String strRet;

    int nPos = 0;

    while (true)
    {
        int nPreVersion = strRCData.IndexOf(strKeyword, nPos);

        if (nPreVersion == -1)
        {
            strRet += strRCData.Right(strRCData.Length() - nPos);
            break;
        }

        nPreVersion += strKeyword.Length();

        while (strRCData[nPreVersion] == _T(' '))
        {
            ++nPreVersion;
        }

        strRet += strRCData.SubString(nPos, nPreVersion - nPos);

        const DWORD BUFFER_SIZE = 24;
        TCHAR BUFFER[BUFFER_SIZE];

        _stprintf(BUFFER, _T("%u,%u,%u,%u"), (DWORD)wMajor, (DWORD)wMinor, (DWORD)wBuild, (DWORD)wRevision);

        strRet += BUFFER;

        int nPostVersionCr = strRCData.IndexOf(_T("\r"), nPreVersion);
        int nPostVersionLn = strRCData.IndexOf(_T("\n"), nPreVersion);

        if (nPostVersionCr == -1 && nPostVersionLn == -1)
        {
            break;
        }

        nPos = min(nPostVersionCr, nPostVersionLn);
    }

    return strRet;
}

xl::String RCModifyVersion(const xl::String &strRCData, DWORD dwVersionType, WORD wMajor, WORD wMinor, WORD wBuild, WORD wRevision)
{
    xl::String strRet = strRCData;
         
    if ((dwVersionType & VERSION_TYPE_FILE) != 0)
    {
        strRet = RCModifyStandardVersion(strRet, _T("FILEVERSION"), wMajor, wMinor, wBuild, wRevision);
    }

    if ((dwVersionType & VERSION_TYPE_PRODUCT) != 0)
    {
        strRet = RCModifyStandardVersion(strRet, _T("PRODUCTVERSION"), wMajor, wMinor, wBuild, wRevision);
    }

    return strRet;
}

xl::String RCEscape(const xl::String &strRCString)
{
    xl::String strRet = strRCString;

    strRet = strRet.Replace(_T("\""), _T("\"\""));
    strRet = strRet.Replace(_T("\\"), _T("\\\\"));

    return strRet;
}

xl::String RCModifyVersionString(const xl::String &strRCData, const xl::String &strKey, const xl::String &strValue)
{
    xl::String strRet;

    xl::String strKeyword;
    strKeyword += _T("VALUE \"");
    strKeyword += strKey;
    strKeyword += _T("\", \"");

    int nPos = 0;

    while (true)
    {
        int nPreValue = strRCData.IndexOf(strKeyword, nPos);

        if (nPreValue == -1)
        {
            strRet += strRCData.Right(strRCData.Length() - nPos);
            break;
        }

        nPreValue += strKeyword.Length();

        strRet += strRCData.SubString(nPos, nPreValue - nPos);
        strRet += RCEscape(strValue);

        nPos = strRCData.IndexOf(_T("\""), nPreValue);

        if (nPos == -1)
        {
            break;
        }
    }

    return strRet;
}

