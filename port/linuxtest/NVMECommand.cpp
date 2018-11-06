// Linux implementation

#include "utility/cmd/NVMECommand.h"
#include "utility/misc/MiscUtil.h"

#include "SystemHeader.h"

#include "NVMESystemCode.h"

#define SYSTEM_DATA nvme_passthru_cmd
#define SYSTEM_CODE NVME_IOCTL_ADMIN_CMD

void NVMECommand::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    p.opcode = getCommandCode();
    p.nsid = 0;
    p.addr = (U64) ((void*) getBufferPtr());
    p.data_len = BufferSize;

    p.cdw10 = 0;
    p.cdw11 = 0;
    p.cdw12 = 0;
    p.cdw13 = 0;
    p.cdw14 = 0;
    p.cdw15 = 0;

    p.timeout_ms = 60000;

    switch(CommandCode)
    {
    case ORG_NVME_ADMIN_IDENTIFY:
        p.cdw10 = 0x01; break;
    case ORG_NVME_ADMIN_GLP_ERROR:
        p.nsid = 0xFFFFFFFF; p.cdw10 = 0x01 | (((p.data_len / 4) - 1) << 16); break;
    case ORG_NVME_ADMIN_GLP_SMART:
        p.nsid = 0xFFFFFFFF; p.cdw10 = 0x02 | (((p.data_len / 4) - 1) << 16); break;
    }
}

bool NVMECommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string NVMECommand::getTaskfileString()
{
    return "[nvme] taskfile string not supported";
}

U8* NVMECommand::getOutputTaskfilePtr()
{
    return NULL;
}

eCommandError NVMECommand::getErrorStatus()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return CMD_ERROR_NONE;
}

void NVMECommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool NVMECommand::executeCommand(int handle)
{
    int retCode = ioctl(handle, SYSTEM_CODE, getStructPtr());

    return (retCode == 0);
}
