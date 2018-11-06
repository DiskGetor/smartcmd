#include "utility/cmd/NVMECommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "NVMEDevice.h"

bool NVMEDevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new NVMECommand();
    BaseDevice::prepareResource();

    // Additional updates:
    m_CommandType = CMDTYPE_NVME;
    m_TransferLength = 256;

    return true;
}

bool NVMEDevice::readIdentify()
{
    return executeCommand(0, 8, NVME_ADMIN_IDENTIFY); // NVME uses 8 sectors
}

bool NVMEDevice::readSmartData()
{
    return executeCommand(0, 1, ORG_NVME_ADMIN_GLP_SMART);
}

bool NVMEDevice::readSmartThreshold()
{
    return false;
}

bool NVMEDevice::read(U32 lba, U32 sectorCount)
{
    UNUSE_NAME(lba);
    UNUSE_NAME(sectorCount);
    return false;
}

bool NVMEDevice::write(U32 lba, U32 sectorCount)
{
    UNUSE_NAME(lba);
    UNUSE_NAME(sectorCount);
    return false;
}

bool NVMEDevice::readInquiry(sInquiryInfo &driveInfo)
{
    UNUSE_NAME(driveInfo);
    return false;
}
