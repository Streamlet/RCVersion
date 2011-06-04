#pragma once


#include <Windows.h>
#include <tchar.h>
#include <xl/String/xlString.h>

enum VersionType
{
    VTFileVersion,
    VTProductVersion
};

xl::String RCModifyVersion(const xl::String &strRCData,
                           VersionType dwVersionType,
                           WORD wMajor,
                           WORD wMinor,
                           WORD wBuild,
                           WORD wRevision);

xl::String RCModifyVersionString(const xl::String &strRCData,
                                 const xl::String &strKey,
                                 const xl::String &strValue);

xl::String RCIncreaseVersion(const xl::String &strRCData,
                             VersionType dwVersionType,
                             int nDeltaMajor,
                             int wDeltaMinor,
                             int wDeltaBuild,
                             int wDeltaRevision,
                             int nStringFields = 4,
                             const xl::String &strStringFieldsSplitter = _T("."));
