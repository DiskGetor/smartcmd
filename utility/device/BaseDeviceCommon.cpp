// #include "core/NVMEStruct.h"
#include "core/SmartUtil.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/Device.h"
#include "utility/device/DeviceUtil.h"

#include "BaseDevice.h"

BaseDevice::BaseDevice()
{
    reset();
}

BaseDevice::~BaseDevice()
{
    if (NULL != m_Command) delete m_Command;
    if (0 != m_Handle) DeviceUtil::CloseDevice(m_Handle);
}

void BaseDevice::reset()
{
    m_Name = "";
    m_Handle = 0;
    m_Command = NULL;
}

U8* BaseDevice::getBufferPtr()
{
    if (NULL == m_Command) return NULL;
    return m_Command->getBufferPtr();
}

eCommandError BaseDevice::getErrorStatus() const
{
    return m_Command->getErrorStatus();
}

U32 BaseDevice::getMaxTransferSize() const
{
    return m_TransferLength;
}

string BaseDevice::toString() const
{
    stringstream sstr;

    sstr << "Name: " << m_Name << endl;
    sstr << "Handle: " << m_Handle << endl;
    sstr << "MaxTransfer: " << m_TransferLength << endl;

    switch(m_CommandType)
    {
        case CMDTYPE_ATA:  sstr << "CommandType: ATA"  << endl; break;
        case CMDTYPE_SAT:  sstr << "CommandType: SAT"  << endl; break;
        case CMDTYPE_STP:  sstr << "CommandType: STP"  << endl; break;
        case CMDTYPE_SCSI: sstr << "CommandType: SCSI" << endl; break;
        case CMDTYPE_CSMI: sstr << "CommandType: CSMI" << endl; break;
        case CMDTYPE_HYP:  sstr << "CommandType: HYP"  << endl; break;
        case CMDTYPE_NVME: sstr << "CommandType: NVME" << endl; break;
        default: break;
    }

    return sstr.str();
}

void BaseDevice::fillWriteBuffer(U8 *buffPtr, U32 buffSizeInByte)
{
    ASSERT (buffSizeInByte <= 256 * 512);
    U8* cmdBuffPtr = m_Command->getBufferPtr();

    memcpy(cmdBuffPtr, buffPtr, buffSizeInByte);
}

void BaseDevice::fillWritePattern(U8 *patPtr, U32 patSizeInByte)
{
    m_Command->formatBuffer(patPtr, patSizeInByte, 0);
}

void BaseDevice::formatBufferHeader(U32 lba, U32 sectorCount, U32 writeCount, U32 sectorOffset)
{
    m_Command->formatAddress(lba, sectorCount, writeCount, sectorOffset);
}

void BaseDevice::randomWriteBuffer(U32 sectorCount)
{
    m_Command->randomBuffer(sectorCount);
}

bool BaseDevice::encryptBuffer(U32 sectorCount)
{
    return m_Command->encryptBuffer(sectorCount);
}

bool BaseDevice::verifyBuffer(U32 sectorCount)
{
    return m_Command->verifyBuffer(sectorCount);
}

bool BaseDevice::prepareResource()
{
    if (NULL == m_Command) return false;

    m_Command->setCommandBasic(0, 256, 0);
    m_Command->allocBuffer();

    return true;
}

bool BaseDevice::executeCommand(U32 lba, U32 sectorCount, U32 cmdCode)
{
    if(NULL == m_Command) return false;

    m_Command->setCommand(lba, sectorCount, cmdCode, false);
    return m_Command->executeCommand(m_Handle);
}

bool BaseDevice::initDevice(const string &driveName)
{
    reset();

    bool status = false;

    do {
        m_Name = driveName;

        if (false == DeviceUtil::OpenDevice(m_Name, m_Handle)) break;

        // Prepare commands, Buffers
        if (false == prepareResource()) break;

        status = true;
    } while(0);

    return status;
}

bool BaseDevice::initDeviceWithSerialNumber(const string &serialNo)
{
    ASSERT(0);

    vector<sPhysicalDriveInfo> driveList;
    DeviceUtil::OpenDevice(driveList);
    DeviceUtil::CloseDevice(driveList);

    bool status = false;
    string activeDriveName;
    U32 driveCount = driveList.size();
    for (U32 i = 0; i < driveCount; i++)
    {
        sPhysicalDriveInfo& physDevice = driveList[i];

        // Read identify information from drive
        Device dev;
        dev.initDevice(physDevice.DriveName);
        dev.readIdentify();

        // Extract SerialNumber
        sIdentifyInfo driveInfo;
        DeviceUtil::ParseIdentifyInfo(dev.getBufferPtr(), physDevice.DriveName, driveInfo);

        // Testing for serialNo
        if (0 == serialNo.compare(driveInfo.SerialNumber))
        {
            status = true;
            activeDriveName = physDevice.DriveName;
            break;
        }
    }

    if (true == status) status = initDevice(activeDriveName);

    return status;
}

bool BaseDevice::readSmart(sSmartInfo &smartInfo)
{
    bool status = false;

    switch(m_CommandType)
    {
    case CMDTYPE_ATA:
    case CMDTYPE_SAT:
    case CMDTYPE_HYP:
    case CMDTYPE_STP:  status = readAtaSmart(smartInfo); break;
    case CMDTYPE_NVME: status = readNvmeSmart(smartInfo); break;
    }

    return status;
}

bool BaseDevice::readIdentify(sIdentifyInfo &driveInfo)
{
    bool status = false;

    switch(m_CommandType)
    {
    case CMDTYPE_ATA:
    case CMDTYPE_SAT:
    case CMDTYPE_HYP:
    case CMDTYPE_STP:  status = readAtaIdentify(driveInfo); break;
    case CMDTYPE_NVME: status = readNvmeIdentify(driveInfo); break;
    }

    return status;
}

bool BaseDevice::readInquiry(sInquiryInfo &driveInfo)
{
    bool status = false;

    do {
        if (false == readIdentify()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        DeviceUtil::ParseInquiryInfo(getBufferPtr(), driveInfo);

        status = true;
    } while(0);

    return status;
}

bool BaseDevice::readAtaSmart(sSmartInfo &smartInfo)
{
    bool status = false;
    U8 valBuffer[512], thrBuffer[512];

    do {
        if (false == readSmartData()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        memcpy(valBuffer, getBufferPtr(), 512);

        if (false == readSmartThreshold()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        memcpy(thrBuffer, getBufferPtr(), 512);

        sSmartData smartData;
        smartData.setVendorCode(eUnknownVendor);
        smartData.setSmartBuffer(valBuffer, thrBuffer);

        SmartUtil::ParseSmartData(smartData, smartInfo);

        status = true;
    } while(0);

    return status;
}

bool BaseDevice::readNvmeSmart(sSmartInfo &smartInfo)
{
    bool status = false;

    do {
        if (false == readSmartData()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        SmartUtil::ParseNvmeSmartInfo(getBufferPtr(), smartInfo);

        status = true;
    } while(0);

    return status;
}

bool BaseDevice::readAtaIdentify(sIdentifyInfo &driveInfo)
{
    bool status = false;

    do {
        if (false == readIdentify()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        DeviceUtil::ParseIdentifyInfo(getBufferPtr(), m_Name, driveInfo);

        status = true;
    } while(0);

    return status;
}

bool BaseDevice::readNvmeIdentify(sIdentifyInfo &driveInfo)
{
    bool status = false;

    do {
        if (false == readIdentify()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        // Parsing NvmeIdentifyData here
        SmartUtil::ParseNvmeIdentifyInfo(getBufferPtr(), driveInfo);

        driveInfo.DriveName = m_Name;

        status = true;
    } while(0);

    return status;
}
