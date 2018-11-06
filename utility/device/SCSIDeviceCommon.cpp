#include "core/CoreData.h"
#include "utility/cmd/SCSICommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "SCSIDevice.h"

bool SCSIDevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new SCSICommand();
    return BaseDevice::prepareResource();
}

bool SCSIDevice::readIdentify()
{
    return executeCommand(0, 1, SCSI_INQUIRY);
}

bool SCSIDevice::readSmartData()
{
    return false;
}

bool SCSIDevice::readSmartThreshold()
{
    return false;
}

bool SCSIDevice::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, SCSI_READ_10);
}

bool SCSIDevice::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, SCSI_WRITE_10);
}

bool SCSIDevice::readIdentify(sIdentifyInfo &driveInfo)
{
    UNUSE_NAME(driveInfo);
    return false;
}
