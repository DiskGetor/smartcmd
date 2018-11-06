#include "FileUtil.h"
#include "StringUtil.h"
#include "SystemUtil.h"

bool SystemUtil::ExecCommand(const string& cmd, const vector<string>& param, string& output)
{
    // Create tmpfile
    string tmpfile = FileUtil::GetTempFile();

    string cmdstr = cmd + " ";
    for (int i = 0, count = param.size(); i< count; i++) cmdstr += param[i] + " ";

    // Redirect to tmpfile
    cmdstr += " > " + tmpfile;

    system(cmdstr.c_str());

    bool readStatus = FileUtil::ReadText(tmpfile, output);

    // Remove temp file
    cmdstr = SystemUtil::GetCmdRemoveFile(tmpfile);
    system(cmdstr.c_str());

    return readStatus;
}

void SystemUtil::GetSystemTime(U32 &msec)
{
    U32 high, low;
    GetSystemTime(high, low);

    msec = (high % 1000000) * 1000 + low;
}
