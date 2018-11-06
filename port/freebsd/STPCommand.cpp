// Windows implementation

#include "SystemHeader.h"

#include "utility/cmd/CsmiSas.h"
#include "utility/cmd/STPCommand.h"

void STPCommand::initPhyEntity(const CSMI_SAS_PHY_ENTITY &phy)
{
    ASSERT(0);
}

void STPCommand::buildBasicCommand()
{
    ASSERT(0);
}

bool STPCommand::buildCommand()
{
    ASSERT(0);
    return false;
}

string STPCommand::getTaskfileString()
{
    ASSERT(0);
    return "";
}

eCommandError STPCommand::getErrorStatus()
{
    ASSERT(0);
    return CMD_ERROR_NONE;
}

void STPCommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    ASSERT(0);
}

bool STPCommand::executeCommand(int handle)
{
    ASSERT(0);
    return false;
}

bool STPCommand::buildIoctlHeader()
{
    ASSERT(0);
    return false;
}

U8* STPCommand::getBufferPtr()
{
    ASSERT(0);
    return NULL;
}
