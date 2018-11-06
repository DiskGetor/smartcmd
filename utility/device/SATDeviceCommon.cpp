#include "core/CoreData.h"
#include "utility/cmd/ATACommand.h"
#include "utility/cmd/SATCommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "SATDevice.h"

bool SATDevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new SATCommand();
    BaseDevice::prepareResource();

    return true;
}

bool SATDevice::readIdentify()
{
    return executeCommand(0, 1, SAT_IDENTIFY_DEVICE);
}

bool SATDevice::readSmartData()
{
    return executeCommand(0, 1, SAT_SMART_READ_DATA);
}

bool SATDevice::readSmartThreshold()
{
    return executeCommand(0, 1, SAT_SMART_READ_THRESHOLD);
}

bool SATDevice::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, SAT_READ_DMA);
}

bool SATDevice::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, SAT_WRITE_DMA);
}

bool SATDevice::readInquiry(sInquiryInfo &driveInfo)
{
    UNUSE_NAME(driveInfo);
    return false;
}
