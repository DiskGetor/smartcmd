#include "AppMain.h"
#include "VscSm2246.h"
#include "utility/misc/MiscUtil.h"

int main(int argc, char *argv[])
{
    MiscUtil::RequestAdminPermission();

    VscSm2246 tool;

    tool.setToolInfo(argc, argv);
    return tool.execTool();
}
