#include "utility/misc/SystemUtil.h"

#include "SystemHeader.h"

bool SystemUtil::IsAdministrator()
{
    return IsUserAnAdmin ();
}

U32 SystemUtil::GetBootTime()
{
    return time(0) - (GetTickCount() / 1000);
}

void SystemUtil::SystemSleep(U32 sec)
{
    // Sleep the calling process in some seconds
    Sleep(sec * 1000);
}

void SystemUtil::SystemSleepMillisecond(U32 milisecond)
{
    Sleep(milisecond);
}

void SystemUtil::MakeDir(const string &dirPath)
{
    ASSERT(0);
    UNUSE_NAME(dirPath);
    // QDir dir(QFileInfo(dirPath.c_str()).absolutePath());
    // if (false == dir.exists())
    // {
    //     dir.mkpath(".");
    // }

    // if (!PathFileExistsA(dirPath.c_str()))
    // {
    //     CreateDirectoryA(dirPath.c_str(), NULL);
    // }
}

void SystemUtil::GetSystemTime(U32 &sec, U32 &msec)
{
    SYSTEMTIME systime;
    ::GetSystemTime(&systime);

    sec = time(0);
    msec = systime.wMilliseconds;
}

string SystemUtil::GetCmdRemoveFile(const string& fileName)
{
    stringstream sstr;
    sstr << "DEL /F /Q " << fileName;

    return sstr.str();
}

string SystemUtil::GetCmdMakeDir(const string& dirPath)
{
    stringstream sstr;
    sstr << "mkdir " << dirPath;

    return sstr.str();
}
