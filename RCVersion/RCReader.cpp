#include "RCReader.h"
#include "Encoding.h"
#include <xl/Containers/xlArray.h>
#include <Loki/ScopeGuard.h>
#include <cassert>
#include <Windows.h>

bool ReadRCRaw(LPCTSTR lpFileName, xl::Array<BYTE> *parrRCData)
{
    assert(parrRCData != nullptr);

    parrRCData->Clear();

    HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);

    LARGE_INTEGER li;

    if (!GetFileSizeEx(hFile, &li) || li.HighPart != 0)
    {
        return false;
    }

    parrRCData->Resize(li.LowPart);

    DWORD dwBytesRead = 0;

    if (!ReadFile(hFile, &(*parrRCData)[0], li.LowPart, &dwBytesRead, NULL) || dwBytesRead != li.LowPart)
    {
        return false;
    }

    return true;
}

bool RCMixedToUnicode(const xl::Array<BYTE> &arrRCData, xl::String *pstrRCData)
{
    assert(pstrRCData != nullptr);

    pstrRCData->Clear();

    xl::StringA strPreCodePage = "#pragma code_page(";
    xl::StringA strPostCodePage = ")\r\n";

    xl::StringA strAnsi = xl::StringA((LPCSTR)&arrRCData[0], arrRCData.Size());

    int nPos = 0;
    DWORD dwCodePage = CP_ACP;

    while (true)
    {
        int nPreCodePage = strAnsi.IndexOf(strPreCodePage, nPos);

        if (nPreCodePage == -1)
        {
            *pstrRCData += Encoding::AnsiToString(strAnsi.Right(strAnsi.Length() - nPos), dwCodePage);
            break;
        }
        
        *pstrRCData += Encoding::AnsiToString(strAnsi.SubString(nPos, nPreCodePage - nPos), dwCodePage);
        
        int nPostCodePage = strAnsi.IndexOf(strPostCodePage, nPreCodePage);

        if (nPostCodePage == -1)
        {
            pstrRCData->Clear();
            return false;
        }
        
        nPreCodePage += strPreCodePage.Length();
        dwCodePage = (DWORD)atoi(strAnsi.SubString(nPreCodePage, nPostCodePage -nPreCodePage).GetAddress());

        nPos = nPostCodePage + strPostCodePage.Length();
    }

    return true;
}

bool RCRawToUnicode(const xl::Array<BYTE> &arrRCData, xl::String *pstrRCData)
{
    assert(pstrRCData != nullptr);

    if (arrRCData.Empty())
    {
        pstrRCData->Clear();
        return true;
    }

    WORD wUnicodeBOM = *(WORD *)&arrRCData[0];

    if (wUnicodeBOM == 0xfeff)
    {
        *pstrRCData = xl::String((LPCTSTR)&arrRCData[2], arrRCData.Size() / sizeof(xl::Char) - 1);
        return true;
    }

    return RCMixedToUnicode(arrRCData, pstrRCData);
}

bool ReadRC(LPCTSTR lpFileName, xl::String *pstrRCData)
{
    if (lpFileName == nullptr || lpFileName[0] == _T('\0') || pstrRCData == nullptr)
    {
        return FALSE;
    }

    xl::Array<BYTE> arrRCData;

    if (!ReadRCRaw(lpFileName, &arrRCData))
    {
        return false;
    }

    if (!RCRawToUnicode(arrRCData, pstrRCData))
    {
        return false;
    }

    return true;
}

bool WriteRC(const xl::String &strRCData, LPCTSTR lpFileName)
{
    if (lpFileName == nullptr || lpFileName[0] == _T('\0'))
    {
        return FALSE;
    }

    Loki::ScopeGuard sgDeleteFile = Loki::MakeGuard(DeleteFile, lpFileName);

    HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);

    DWORD dwBytesWritten = 0;
    WORD wUnicodeBOM = 0xfeff;

    if (!WriteFile(hFile, &wUnicodeBOM, sizeof(WORD), &dwBytesWritten, NULL) || dwBytesWritten != dwBytesWritten)
    {
        return false;
    }

    if (!strRCData.Empty())
    {
        DWORD dwBytesToWrite = strRCData.Length() * sizeof(xl::Char);

        if (!WriteFile(hFile, (LPVOID)strRCData.GetAddress(), dwBytesToWrite, &dwBytesWritten, NULL) || dwBytesWritten != dwBytesToWrite)
        {
            return false;
        }
    }

    sgDeleteFile.Dismiss();
    
    return true;
}
