// Windows implementation

#include "utility/cmd/HYPCommand.h"

#include "SystemHeader.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE IOCTL_SCSI_PASS_THROUGH

struct sHeaderData
{
    SCSI_PASS_THROUGH Spt;
    U8 Filler[4];
    U8 Sense[64];
};

void HYPCommand::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    SCSI_PASS_THROUGH& spt = p.Spt;

    spt.Length = sizeof (p.Spt);
    spt.ScsiStatus = 0;
    spt.CdbLength = sizeof (spt.Cdb);
    spt.SenseInfoLength = sizeof (p.Sense);
    spt.SenseInfoOffset = sizeof (p.Spt) + sizeof (p.Filler);
    spt.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
    spt.DataTransferLength = BufferSize;
    spt.DataBufferOffset = StructSize + FillerSize;
    spt.TimeOutValue = 30;

    if (true == isDataInCommand()) spt.DataIn = SCSI_IOCTL_DATA_IN;
    if (true == isDataOutCommand()) spt.DataIn = SCSI_IOCTL_DATA_OUT;

    switch (getCommandClass())
    {
        case CLASS_ATA: buildSat16(spt.Cdb); break;
        case CLASS_SCSI: buildCmdblock(spt.Cdb); break;
        default: break;
    }
}

bool HYPCommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string HYPCommand::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    SCSI_PASS_THROUGH& spt = p.Spt;

    return BaseCommand::GetTaskfileString(spt.Cdb, 16 + 68);
}

eCommandError HYPCommand::getErrorStatus()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::ParseSense(p.Sense);
}

void HYPCommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool HYPCommand::executeCommand(int handle)
{
    HANDLE deviceHandle = (HANDLE) handle; // In Windows, HANDLE == pvoid

    U32 inputBufferSize = StructSize + FillerSize + (isDataOutCommand() ? BufferSize : 0);
    U32 outputBufferSize = StructSize + FillerSize + BufferSize;

    DWORD byteReturn = 0;
    return DeviceIoControl(
        deviceHandle,
        SYSTEM_CODE,
        getStructPtr(), inputBufferSize,
        getStructPtr(), outputBufferSize,
        &byteReturn,
        NULL);
}
