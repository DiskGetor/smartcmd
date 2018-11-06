#include "AppMain.h"
#include "AppData.h"
#include "SmartCmd.h"
#include "utility/misc/MiscUtil.h"

int main(int argc, char *argv[])
{
    MiscUtil::RequestAdminPermission();

    LoadCoreData();

    SmartCmd tool;
    tool.setToolInfo(argc, argv);
    return tool.execTool();
}
