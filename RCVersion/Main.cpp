#include <tchar.h>
#include "RCReader.h"
#include "RCVersion.h"
#include <locale.h>
#include <xl/Containers/xlArray.h>
#include <xl/Containers/xlMap.h>
#include <xl/Objects/xlString.h>
#include <Loki/ScopeGuard.h>

#define RC_VERSION_SUCCEESS 0
#define RC_VERSION_FAILURE  -1

struct VersionInfo
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wRevision;
};

struct PathFile
{
    xl::String strPattern;
    bool bRecursion;
};

struct CommandLineInfo
{
    bool bFileVersion;
    bool bProductVersion;
    VersionInfo viFile;
    VersionInfo viProduct;
    xl::Map<xl::String, xl::String> mapStrings;
    xl::Array<PathFile> arrPaths;
};

void ShowCopyright()
{

}

void ShowHelp()
{
    _T("Comments")
    _T("CompanyName")
    _T("FileDescription")
    _T("FileVersion")
    _T("InternalName")
    _T("LegalCopyright")
    _T("LegalTrademarks")
    _T("OriginalFilename")
    _T("PrivateBuild")
    _T("ProductName")
    _T("ProductVersion")
    _T("SpecialBuild");
}

bool ParseVersion(const xl::String &strVerison, VersionInfo *pVI)
{
    xl::Array<xl::String> arrVersion = strVerison.Split(_T(","));

    if (arrVersion.Size() != 4)
    {
        return false;
    }

    pVI->wMajor = _ttoi(arrVersion[0].GetAddress());
    pVI->wMinor = _ttoi(arrVersion[1].GetAddress());
    pVI->wBuild = _ttoi(arrVersion[2].GetAddress());
    pVI->wRevision = _ttoi(arrVersion[3].GetAddress());

    return true;
}

bool ParseCommandLine(int argc, TCHAR *argv[], CommandLineInfo *pCLI)
{
    if (argc <= 1)
    {
        ShowHelp();
        return false;
    }

    pCLI->bFileVersion = false;
    pCLI->bProductVersion = false;
    pCLI->mapStrings.Clear();
    pCLI->arrPaths.Clear();

    const xl::String strFileVersion = _T("/fileversion:");
    const xl::String strPruductVersion = _T("/productversion:");
    const xl::String strStringProperty = _T("/string:");
    const xl::String strRecursion = _T("/r");

    for (int i = 1; i < argc; ++i)
    {
        xl::String strCommand = argv[i];
        xl::String strCommandLower = strCommand.ToLower();

        if (strCommandLower.IndexOf(strFileVersion) == 0)
        {
            xl::String strVersion = strCommand.Right(strCommand.Length() - strFileVersion.Length());

            if (!ParseVersion(strVersion, &pCLI->viFile))
            {
                return false;
            }

            pCLI->bFileVersion = true;
        }
        else if (strCommandLower.IndexOf(strPruductVersion) == 0)
        {
            xl::String strVersion = strCommand.Right(strCommand.Length() - strPruductVersion.Length());

            if (!ParseVersion(strVersion, &pCLI->viProduct))
            {
                return false;
            }

            pCLI->bProductVersion = true;
        }
        else if (strCommandLower.IndexOf(strStringProperty) == 0)
        {
            xl::String strProperty = strCommand.Right(strCommand.Length() - strStringProperty.Length());
            xl::Array<xl::String> arrKeyValue = strProperty.Split(_T("="), 2);

            if (arrKeyValue.Size() != 2)
            {
                return false;
            }

            pCLI->mapStrings.Insert(arrKeyValue[0], arrKeyValue[1]);
        }
        else
        {
            PathFile pf;
            pf.strPattern = strCommand;
            pf.bRecursion = false;

            if (i + 1 < argc)
            {
                xl::String strExtra = argv[i + 1];
                strExtra.MakeLower();

                if (strExtra == strRecursion)
                {
                    pf.bRecursion = true;
                    ++i;
                }
            }

            pCLI->arrPaths.PushBack(pf);
        }
    }

    return true;
}

bool ModifyRCFile(const CommandLineInfo &cli, const xl::String strFile)
{
    _tprintf(_T("Modifying file %s...\n"), strFile.GetAddress());

    xl::String strRCData;

    if (!ReadRC(strFile.GetAddress(), &strRCData))
    {
        _tprintf(_T("Error: Failed to read RC file.\n"));
        return false;
    }

    if (cli.bFileVersion)
    {
        strRCData = RCModifyVersion(strRCData, VERSION_TYPE_FILE,
            cli.viFile.wMajor, cli.viFile.wMinor, cli.viFile.wBuild, cli.viFile.wRevision);
    }

    if (cli.bProductVersion)
    {
        strRCData = RCModifyVersion(strRCData, VERSION_TYPE_PRODUCT,
            cli.viProduct.wMajor, cli.viProduct.wMinor, cli.viProduct.wBuild, cli.viProduct.wRevision);
    }

    for (auto it = cli.mapStrings.Begin(); it != cli.mapStrings.End(); ++it)
    {
        strRCData = RCModifyVersionString(strRCData, it->Key.GetAddress(), it->Value.GetAddress());
    }

    if (!WriteRC(strRCData, strFile.GetAddress()))
    {
        _tprintf(_T("Error: Failed to write RC file.\n"));
        return false;
    }

    return true;
}

bool ModifyRCFiles(const CommandLineInfo &cli, const xl::String strSearch, bool bRecursion)
{
    WIN32_FIND_DATA wfd = {};

    xl::String strPath;
    xl::String strPattern;

    int nPos = strSearch.LastIndexOf(_T("\\"));

    if (nPos == -1)
    {
        strPattern = strSearch;
    }
    else
    {
        strPath = strSearch.Left(nPos + 1);
        strPattern = strSearch.Right(strSearch.Length() - nPos - 1);
    }

    bool bSuccess = true;

    while (true)
    {
        HANDLE hFind = FindFirstFile(strSearch.GetAddress(), &wfd);

        if (hFind == INVALID_HANDLE_VALUE)
        {
            break;
        }

        LOKI_ON_BLOCK_EXIT(FindClose, hFind);

        do 
        {
            xl::String strFile = wfd.cFileName;

            if (strFile == _T(".") || strFile == _T(".."))
            {
                continue;
            }

            if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                continue;
            }

            if (!ModifyRCFile(cli, strPath + strFile))
            {
                bSuccess = false;
            }

        } while (FindNextFile(hFind, &wfd));

        break;
    }
    
    if (bRecursion)
    {
        HANDLE hFind = FindFirstFile((strPath + _T("*")).GetAddress(), &wfd);

        if (hFind == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        LOKI_ON_BLOCK_EXIT(FindClose, hFind);
        
        bool bSuccess = true;

        do 
        {
            xl::String strFile = wfd.cFileName;

            if (strFile == _T(".") || strFile == _T(".."))
            {
                continue;
            }

            if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                continue;
            }
            
            ModifyRCFiles(cli, strPath + strFile + _T("\\") + strPattern, true);

        } while (FindNextFile(hFind, &wfd));
    }

    return bSuccess;
}

bool ModifyRC(const CommandLineInfo &cli)
{
    bool bSuccess = true;

    for (auto it = cli.arrPaths.Begin(); it != cli.arrPaths.End(); ++it)
    {
        bSuccess = ModifyRCFiles(cli, it->strPattern, it->bRecursion) && bSuccess;
    }

    return true;
}

int _tmain(int argc, TCHAR *argv[])
{
    setlocale(LC_ALL, "");

    ShowCopyright();

    CommandLineInfo cli = {};

    if (!ParseCommandLine(argc, argv, &cli))
    {
        return RC_VERSION_FAILURE;
    }

    if (!ModifyRC(cli))
    {
        return RC_VERSION_FAILURE;
    }
    
    return RC_VERSION_SUCCEESS;
}

