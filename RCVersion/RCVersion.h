#pragma once


#include <Windows.h>
#include <tchar.h>
#include <xl/Objects/xlString.h>

#define VERSION_TYPE_FILE       0x00000001
#define VERSION_TYPE_PRODUCT    0x00000002

xl::String RCModifyVersion(const xl::String &strRCData, DWORD dwVersionType, WORD wMajor, WORD wMinor, WORD wBuild, WORD wRevision);
xl::String RCModifyVersionString(const xl::String &strRCData, const xl::String &strKey, const xl::String &strValue);

