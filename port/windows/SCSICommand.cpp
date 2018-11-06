// Windows implementation

#include "utility/cmd/SCSICommand.h"

#include "SystemHeader.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE IOCTL_SCSI_PASS_THROUGH

struct sHeaderData
{
    SCSI_PASS_THROUGH Spt;
    U8 Filler[4];
    U8 Sense[64];
};

void SCSICommand::buildBasicCommand()
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

    buildCmdblock(spt.Cdb);
}

bool SCSICommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string SCSICommand::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    SCSI_PASS_THROUGH& spt = p.Spt;

    return BaseCommand::GetTaskfileString(spt.Cdb, 16 + 68);
}

eCommandError SCSICommand::getErrorStatus()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::ParseSense(p.Sense);
}

void SCSICommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool SCSICommand::executeCommand(int handle)
{
    HANDLE deviceHandle = (HANDLE) handle; // In Windows, HANDLE == pvoid

    U32 inputBufferSize = StructSize + FillerSize + (isDataOutCommand() ? BufferSize : 0);
    // U32 inputBufferSize = StructSize + FillerSize + BufferSize;
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
