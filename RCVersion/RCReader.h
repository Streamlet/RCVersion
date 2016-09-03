#pragma once

#include <Windows.h>
#include <tchar.h>
#include <xl/Common/String/xlString.h>

bool ReadRC(LPCTSTR lpFileName, xl::String *pstrRCData);
bool WriteRC(const xl::String &strRCData, LPCTSTR lpFileName);
