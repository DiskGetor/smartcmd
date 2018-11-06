#ifndef PORTUTIL_H
#define PORTUTIL_H

#include "core/CoreData.h"
#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"
#include "utility/device/DeviceUtil.h"

#include "SystemHeader.h"

// Utility functions for Windows port

typedef bool (*tIssueCommandFunc) (HANDLE, int, unsigned char*);

class PortUtil
{
public:
    static void ScanPhysicalDrive_Test();
    static void IssueCommand_Test(tIssueCommandFunc func);

    static bool IssueCommand_IdentifyDrive(HANDLE handle, int driveNumber, unsigned char* data);
    static bool IssueCommand_ReadSmartValue(HANDLE handle, int driveNumber, unsigned char* data);
    static bool IssueCommand_ReadSmartThreshold(HANDLE handle, int driveNumber, unsigned char* data);
};

#endif // PORTUTIL_H
