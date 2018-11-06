#include <unistd.h>
#include "utility/misc/SystemUtil.h"

#include "SystemHeader.h"
       
bool SystemUtil::IsAdministrator()
{
    uid_t user = getuid();
    return user == 0;
}

U32 SystemUtil::GetBootTime()
{
    // using "cat /proc/uptime" to get running time of system
    // 1st value: seconds from boot-time
    // 2nd value: idle time in seconds

    double bootsec;
    string fileName = "/proc/uptime";
    ifstream fstr(fileName.c_str(), ios::in);

    fstr >> bootsec;

    fstr.close();

    return time(0) - (U32) bootsec;
}

void SystemUtil::SystemSleep(U32 sec)
{
    // Sleep the calling process in some seconds
    sleep(sec);
}

void SystemUtil::SystemSleepMillisecond(U32 milisecond)
{
    usleep(milisecond * 1000);
}

void SystemUtil::GetSystemTime(U32 &sec, U32 &msec)
{
    // Use gettimeofday here.
    // Temporarily set msec to zero.

    msec = 0;
    sec = time(0);
}

void SystemUtil::MakeDir(const string &dirPath)
{
    struct stat st = {0};

    if (stat(dirPath.c_str(), &st) == -1)
    {
        mkdir(dirPath.c_str(), 0755);
    }

    // Qt way ---------------------------------------------
    // QDir dir(QFileInfo(dirPath.c_str()).absolutePath());
    // if (false == dir.exists())
    // {
    //     dir.mkpath(".");
    // }
    // Qt way ---------------------------------------------
}

string SystemUtil::GetCmdRemoveFile(const string& fileName)
{
    stringstream sstr;
    sstr << "rm -rf " << fileName;

    return sstr.str();
}

string SystemUtil::GetCmdMakeDir(const string &dirPath)
{
    stringstream sstr;
    sstr << "mkdir -p " << dirPath;

    return sstr.str();
}
