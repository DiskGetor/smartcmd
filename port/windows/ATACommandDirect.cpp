#include "utility/cmd/ATACommand.h"

#include "SystemHeader.h"

#define SYSTEM_DATA ATA_PASS_THROUGH_DIRECT
#define SYSTEM_CODE IOCTL_ATA_PASS_THROUGH_DIRECT

void ATACommandDirect::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    p.Length = StructSize;
    p.TimeOutValue = 30;
    p.DataTransferLength = BufferSize;
    p.DataBuffer = Buffer + (StructSize + FillerSize);

    p.AtaFlags |= ATA_FLAGS_DRDY_REQUIRED;
    if (true == isDmaMode()) p.AtaFlags |= ATA_FLAGS_USE_DMA;
    if (true == isDataInCommand()) p.AtaFlags |= ATA_FLAGS_DATA_IN;
    if (true == isDataOutCommand()) p.AtaFlags |= ATA_FLAGS_DATA_OUT;

    buildTaskfile(p.CurrentTaskFile);
}

bool ATACommandDirect::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string ATACommandDirect::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return BaseCommand::GetTaskfileString(p.CurrentTaskFile, 8);
}

eCommandError ATACommandDirect::getErrorStatus()
{
    #define ACC_ERROR(e, bit, code) ((e & bit) ? code : CMD_ERROR_NONE);

    const SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*) getStructPtr();
    const U8* t = p.CurrentTaskFile;

    if (0 == (t[6] & 0x1)) return CMD_ERROR_NONE;            // Error bit is zero
    if ((CommandCode & 0xFF) == t[6]) return CMD_ERROR_EXEC; // CommandCode not changed

    U8 errorByte = t[0];
    eCommandError errorCode = CMD_ERROR_NONE;

    switch (CommandCode)
    {
    // Security Feature. ErrorCode: Abort | InterfaceCRC
    case ATA_DISABLE_PASSWORD    :
    case ATA_SET_PASSWORD        :
    case ATA_ERASE_PREPARE       :
    case ATA_ERASE_UNIT          :
    case ATA_UNLOCK              :
    case ATA_FREEZE_LOCK         :
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0004, CMD_ERROR_ABORT);
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0080, CMD_ERROR_ICRC );
        break;

    // SMART feature. ErrorCode:
    case ATA_SMART_READ_DATA:
    case ATA_SMART_READ_THRESHOLD:
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0004, CMD_ERROR_ABORT);
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0010, CMD_ERROR_IDNF );
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0040, CMD_ERROR_UNC  );
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0080, CMD_ERROR_ICRC );
        break;

    case ATA_READ_DMA:
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0004, CMD_ERROR_ABORT);
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0010, CMD_ERROR_IDNF );
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0040, CMD_ERROR_UNC  );
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0080, CMD_ERROR_ICRC );
        break;

    case ATA_WRITE_DMA:
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0004, CMD_ERROR_ABORT);
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0010, CMD_ERROR_IDNF );
        if (CMD_ERROR_NONE == errorCode) errorCode = ACC_ERROR(errorByte, 0x0080, CMD_ERROR_ICRC );
        break;

    // General FeatureSet
    case ATA_IDENTIFY_DEVICE:
        break;

    default:
        ASSERT(0); break;
    }

    return errorCode;
}

void ATACommandDirect::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool ATACommandDirect::executeCommand(int handle)
{
    HANDLE deviceHandle = (HANDLE) handle; // In Windows, HANDLE == pvoid

    U32 inputBufferSize = StructSize;
    U32 outputBufferSize = StructSize;

    DWORD byteReturn = 0;
    return DeviceIoControl(
        deviceHandle,
        SYSTEM_CODE,
        getStructPtr(), inputBufferSize,
        getStructPtr(), outputBufferSize,
        &byteReturn,
        NULL);
}
