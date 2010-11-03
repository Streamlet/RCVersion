#include <tchar.h>
#include "RCReader.h"
#include "RCVersion.h"

int _tmain(int argc, TCHAR *argv[])
{
    xl::String strRCData;

    ReadRC(_T("..\\Test\\MixedLangages.rc"), &strRCData);

    strRCData = RCModifyVersion(strRCData, VERSION_TYPE_FILE, 1, 2, 3, 4);
    strRCData = RCModifyVersion(strRCData, VERSION_TYPE_PRODUCT, 11, 22, 33, 44);
    strRCData = RCModifyVersionString(strRCData, _T("FileVersion"), _T("111.222.333.444"));
    strRCData = RCModifyVersionString(strRCData, _T("ProductVersion"), _T("1111.2222.3333.4444"));
    strRCData = RCModifyVersionString(strRCData, _T("FileDescription"), _T("~!@#$%^&*()_+|`-=\\{}[]:\";'<>?,./"));

    WriteRC(strRCData, _T("..\\Test\\Unicode.rc"));
    
    return 0;
}