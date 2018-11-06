#ifndef SYSTEMUTIL_H
#define SYSTEMUTIL_H

#include "StdMacro.h"
#include "StdHeader.h"

#include "ThreadUtil.h"

class SystemUtil
{
public:
    // Common functions
    static bool ExecCommand(const string& cmd, const vector<string>& param, string& output);

public:
    static U32 GetBootTime();
    static void SystemSleep(U32 sec);
    static void SystemSleepMillisecond(U32 milisecond);
    static bool IsAdministrator();
    static void MakeDir(const string& dirPath);

    static void GetSystemTime(U32& msec);           // Low resolution. Use this for measuring short time
    static void GetSystemTime(U32& sec, U32& msec);

public:
    // System commands
    static string GetCmdMakeDir(const string& dirPath);
    static string GetCmdRemoveFile(const string& fileName);
};

#endif //SYSTEMUTIL_H
