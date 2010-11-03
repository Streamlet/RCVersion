#pragma once

#include <Windows.h>
#include <tchar.h>
#include <xl/Objects/xlString.h>

bool ReadRC(LPCTSTR lpFileName, xl::String *pstrRCData);
bool WriteRC(const xl::String &strRCData, LPCTSTR lpFileName);
