// Windows implementation

#include "SystemHeader.h"

#include "utility/cmd/CsmiSas.h"
#include "utility/cmd/CSMICommand.h"

void CSMICommand::buildBasicCommand()
{
    ASSERT(0);
}

bool CSMICommand::buildCommand()
{
    ASSERT(0);
    return false;
}

string CSMICommand::getTaskfileString()
{
    ASSERT(0);
    return "";
}

eCommandError CSMICommand::getErrorStatus()
{
    ASSERT(0);
    return CMD_ERROR_NONE;
}

void CSMICommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    ASSERT(0);
}

bool CSMICommand::executeCommand(int handle)
{
    ASSERT(0);
    return false;
}

bool CSMICommand::buildIoctlHeader()
{
    ASSERT(0);
    return false;
}
