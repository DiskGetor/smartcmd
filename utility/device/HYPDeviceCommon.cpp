#include "core/CoreData.h"
#include "utility/cmd/HYPCommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "HYPDevice.h"

bool HYPDevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new HYPCommand();
    BaseDevice::prepareResource();

    return true;
}

bool HYPDevice::readInquiry(sInquiryInfo &driveInfo)
{
    return executeCommand(0, 1, HYP_INQUIRY);
}

bool HYPDevice::readIdentify()
{
    return executeCommand(0, 1, HYP_IDENTIFY_DEVICE);
}

bool HYPDevice::readSmartData()
{
    return executeCommand(0, 1, HYP_SMART_READ_DATA);
}

bool HYPDevice::readSmartThreshold()
{
    return executeCommand(0, 1, HYP_SMART_READ_THRESHOLD);
}

bool HYPDevice::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, HYP_READ_10);
}

bool HYPDevice::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, HYP_WRITE_10);
}
