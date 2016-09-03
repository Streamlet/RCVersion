#include "RCVersion.h"
#include <Windows.h>
#include <tchar.h>
#include <xl/Common/String/xlString.h>


xl::String RCModifyStandardVersion(const xl::String &strRCData,
    const xl::String &strKeyword,
    WORD wMajor,
    WORD wMinor,
    WORD wBuild,
    WORD wRevision)
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

        _stprintf_s(BUFFER,
            _T("%u,%u,%u,%u"),
            (DWORD)wMajor,
            (DWORD)wMinor,
            (DWORD)wBuild,
            (DWORD)wRevision);

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

xl::String RCModifyVersion(const xl::String &strRCData,
                           VersionType dwVersionType,
                           WORD wMajor,
                           WORD wMinor,
                           WORD wBuild,
                           WORD wRevision)
{
    xl::String strRet = strRCData;

    switch (dwVersionType)
    {
    case VTFileVersion:
        strRet = RCModifyStandardVersion(strRet, _T("FILEVERSION"), wMajor, wMinor, wBuild, wRevision);
        break;
    case VTProductVersion:
        strRet = RCModifyStandardVersion(strRet, _T("PRODUCTVERSION"), wMajor, wMinor, wBuild, wRevision);
        break;
    default:
        break;
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

xl::String RCModifyVersionString(const xl::String &strRCData,
                                 const xl::String &strKey,
                                 const xl::String &strValue)
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

bool RCReadStandardVersion(const xl::String &strRCData,
                           const xl::String &strKeyword,
                           WORD *pwMajor,
                           WORD *pwMinor,
                           WORD *pwBuild,
                           WORD *pwRevision)
{
    int nPos = strRCData.IndexOf(strKeyword);

    if (nPos == -1)
    {
        return false;
    }

    nPos += strKeyword.Length();

    WORD wVersions[] = { 0, 0, 0, 0 };

    if (_stscanf_s(&strRCData[nPos],
                   _T("%hu,%hu,%hu,%hu"),
                   &wVersions[0],
                   &wVersions[1],
                   &wVersions[2],
                   &wVersions[3]) != 4)
    {
        return false;
    }

    WORD *pwVersions[] = { pwMajor, pwMinor, pwBuild, pwRevision };

    for (int i = 0; i < 4; ++i)
    {
        if (pwVersions[i] != nullptr)
        {
            *pwVersions[i] = wVersions[i];
        }
    }

    return true;
}

xl::String RCIncreaseVersion(const xl::String &strRCData,
                             VersionType dwVersionType,
                             int nDeltaMajor,
                             int nDeltaMinor,
                             int nDeltaBuild,
                             int nDeltaRevision,
                             int nStringFields /*= 4*/,
                             const xl::String &strStringFieldsSplitter /*= _T(".")*/)
{
    xl::String strRet = strRCData;
    xl::String strKeyword;
    xl::String strVersionStringKeyword;

    switch (dwVersionType)
    {
    case VTFileVersion:
        strKeyword = _T("FILEVERSION");
        strVersionStringKeyword = _T("FileVersion");
        break;
    case VTProductVersion:
        strKeyword = _T("PRODUCTVERSION");
        strVersionStringKeyword = _T("ProductVersion");
        break;
    default:
        return strRet;
    }

    WORD wMajor = 0, wMinor = 0, wBuild = 0, wRevision = 0;

    if (!RCReadStandardVersion(strRet, strKeyword, &wMajor, &wMinor, &wBuild, &wRevision))
    {
        return strRet;
    }

    wMajor += nDeltaMajor;
    wMinor += nDeltaMinor;
    wBuild += nDeltaBuild;
    wRevision += nDeltaRevision;

    strRet = RCModifyVersion(strRet, dwVersionType, wMajor, wMinor, wBuild, wRevision);

    WORD wVersions[] = { wMajor, wMinor, wBuild, wRevision };
    TCHAR szBuffer[7] = {};
    xl::String strVersionString;

    for (int i = 0; i < nStringFields && i < 4; ++i)
    {
        if (i != 0)
        {
            strVersionString += strStringFieldsSplitter;
        }

        _stprintf_s(szBuffer, _T("%hu"), wVersions[i]);
        strVersionString += szBuffer;
    }

    strRet = RCModifyVersionString(strRet, strVersionStringKeyword, strVersionString);

    return strRet;
}