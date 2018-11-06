// Linux implementation

#include "utility/cmd/ATACommand.h"
#include "utility/misc/MiscUtil.h"

#include "SystemHeader.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE SG_IO

struct sHeaderData
{
    struct sg_io_hdr header;
    U8 cmdblk[16]; // ATACommand16
    U8 sense[32];
};

void ATACommand::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    struct sg_io_hdr& hdr = p.header;

    hdr.interface_id = 'S';

    hdr.cmdp = p.cmdblk;
    hdr.cmd_len = sizeof (p.cmdblk);

    hdr.sbp = p.sense;
    hdr.mx_sb_len = sizeof (p.sense);

    hdr.timeout = 30000;
    hdr.dxfer_len = BufferSize;
    hdr.dxferp = getBufferPtr();

    hdr.dxfer_direction = SG_DXFER_NONE;
    if (true == isDataInCommand()) hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    if (true == isDataOutCommand()) hdr.dxfer_direction = SG_DXFER_TO_DEV;

    buildSat16(p.cmdblk);
}

bool ATACommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string ATACommand::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::GetTaskfileString(p.cmdblk, 16);
}

U8* ATACommand::getOutputTaskfilePtr()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return p.sense;
}

eCommandError ATACommand::getErrorStatus()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::ParseSense(p.sense);
}

void ATACommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool ATACommand::executeCommand(int handle)
{
    int retCode = ioctl(handle, SYSTEM_CODE, getStructPtr());

    return (retCode == 0);
}
