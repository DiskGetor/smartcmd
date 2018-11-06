#include "utility/cmd/ATACommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "ATADevice.h"

bool ATADevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new ATACommand();
    BaseDevice::prepareResource();

    return true;
}

bool ATADevice::readIdentify()
{
    return executeCommand(0, 1, ATA_IDENTIFY_DEVICE);
}

bool ATADevice::readSmartData()
{
    return executeCommand(0, 1, ATA_SMART_READ_DATA);
}

bool ATADevice::readSmartThreshold()
{
    return executeCommand(0, 1, ATA_SMART_READ_THRESHOLD);
}

bool ATADevice::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, ATA_READ_DMA);
}

bool ATADevice::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, ATA_WRITE_DMA);
}

bool ATADevice::readInquiry(sInquiryInfo &driveInfo)
{
    UNUSE_NAME(driveInfo);
    return false;
}
