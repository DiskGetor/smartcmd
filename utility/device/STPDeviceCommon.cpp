#include "core/CoreData.h"
#include "utility/cmd/STPCommand.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/DeviceUtil.h"

#include "STPDevice.h"

bool STPDevice::prepareResource()
{
    if (NULL != m_Command) delete m_Command;

    m_Command = new STPCommand();
    return BaseDevice::prepareResource();
}

bool STPDevice::readIdentify()
{
    return executeCommand(0, 1, STP_IDENTIFY_DEVICE);
}

bool STPDevice::readSmartData()
{
    return executeCommand(0, 1, STP_SMART_READ_DATA);
}

bool STPDevice::readSmartThreshold()
{
    return executeCommand(0, 1, STP_SMART_READ_THRESHOLD);
}

bool STPDevice::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, STP_READ_DMA);
}

bool STPDevice::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, STP_WRITE_DMA);
}

bool STPDevice::readInquiry(sInquiryInfo &driveInfo)
{
    UNUSE_NAME(driveInfo);
    return false;
}

bool STPDevice::initPhyEntity(const CSMI_SAS_PHY_ENTITY &phy)
{
    if (NULL == m_Command) return false;

    STPCommand* cmd = (STPCommand*) m_Command;
    cmd->initPhyEntity(phy);

    return true;
}
