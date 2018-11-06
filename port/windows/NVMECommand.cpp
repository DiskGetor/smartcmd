// Windows implementation

#include "utility/cmd/NVMECommand.h"

#include "SystemHeader.h"

#define SYSTEM_DATA ATA_PASS_THROUGH_EX
#define SYSTEM_CODE IOCTL_ATA_PASS_THROUGH

void NVMECommand::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    p.Length = StructSize;
    p.TimeOutValue = 30;
    p.DataTransferLength = BufferSize;
    p.DataBufferOffset = StructSize + FillerSize;

    p.AtaFlags |= ATA_FLAGS_DRDY_REQUIRED;
    if (true == isDmaMode()) p.AtaFlags |= ATA_FLAGS_USE_DMA;
    if (true == isDataInCommand()) p.AtaFlags |= ATA_FLAGS_DATA_IN;
    if (true == isDataOutCommand()) p.AtaFlags |= ATA_FLAGS_DATA_OUT;

    buildTaskfile(p.CurrentTaskFile);
}

bool NVMECommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string NVMECommand::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::GetTaskfileString(p.CurrentTaskFile, 8);
}

U8* NVMECommand::getOutputTaskfilePtr()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return p.CurrentTaskFile;
}

eCommandError NVMECommand::getErrorStatus()
{
    return CMD_ERROR_NONE;
}

void NVMECommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 4;
    BufferSize = SectorCount * 512;
}

bool NVMECommand::executeCommand(int handle)
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
