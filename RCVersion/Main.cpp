#include <tchar.h>
#include "RCReader.h"
#include "RCVersion.h"
#include <locale.h>
#include <xl/Containers/xlArray.h>
#include <xl/Containers/xlMap.h>
#include <xl/Objects/xlString.h>

#define RC_VERSION_SUCCEESS 0
#define RC_VERSION_FAILURE  -1

struct VersionInfo
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wRevision;
};

struct CommandLineInfo
{
    bool bFileVersion;
    bool bProductVersion;
    VersionInfo viFile;
    VersionInfo viProduct;
    xl::Map<xl::String, xl::String> mapStrings;
    xl::Array<xl::String> arrFiles;
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

    return false;
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
    pCLI->arrFiles.Clear();

    const xl::String strFileVersion = _T("/fileversion:");
    const xl::String strPruductVersion = _T("/productversion:");
    const xl::String strStringProperty = _T("/string:");

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
            xl::Array<xl::String> arrKeyValue = strProperty.Split(_T(":"), 2);

            if (arrKeyValue.Size() != 2)
            {
                return false;
            }

            pCLI->mapStrings.Insert(arrKeyValue[0], arrKeyValue[1]);
        }
        else
        {
            pCLI->arrFiles.PushBack(strCommand);
        }
    }

    return true;
}

bool ModifyRC(const CommandLineInfo &cli)
{
    xl::String strRCData;

    ReadRC(_T("..\\Test\\MixedLangages.rc"), &strRCData);

    strRCData = RCModifyVersion(strRCData, VERSION_TYPE_FILE, 1, 2, 3, 4);
    strRCData = RCModifyVersion(strRCData, VERSION_TYPE_PRODUCT, 11, 22, 33, 44);
    strRCData = RCModifyVersionString(strRCData, _T("FileVersion"), _T("111.222.333.444"));
    strRCData = RCModifyVersionString(strRCData, _T("ProductVersion"), _T("1111.2222.3333.4444"));
    strRCData = RCModifyVersionString(strRCData, _T("FileDescription"), _T("~!@#$%^&*()_+|`-=\\{}[]:\";'<>?,./"));

    WriteRC(strRCData, _T("..\\Test\\Unicode.rc"));

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

