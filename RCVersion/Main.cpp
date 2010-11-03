#include <tchar.h>
#include "RCReader.h"

int _tmain(int argc, TCHAR *argv[])
{
    xl::String strRCData;

    ReadRC(_T("..\\Test\\MixedLangages.rc"), &strRCData);
    WriteRC(strRCData, _T("..\\Test\\Unicode.rc"));
    ReadRC(_T("..\\Test\\Unicode.rc"), &strRCData);
    WriteRC(strRCData, _T("..\\Test\\Unicode.rc"));

    return 0;
}