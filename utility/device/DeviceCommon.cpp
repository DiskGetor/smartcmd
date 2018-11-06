#include "core/SmartUtil.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"

#include "Device.h"
#include "DeviceUtil.h"

Device::Device()
{
    reset();
}

Device::~Device()
{
    if (NULL != m_Command) delete m_Command;
    if (0 != m_Handle) DeviceUtil::CloseDevice(m_Handle);
}

void Device::reset()
{
    m_Name = "";
    m_Handle = 0;
    m_Command = NULL;
    m_CommandType = CMDTYPE_COUNT;
    m_VendorCode = eUnknownVendor;
}

U8* Device::getStructPtr()
{
    if (NULL == m_Command) return NULL;
    return m_Command->getStructPtr();
}

U8* Device::getBufferPtr()
{
    if (NULL == m_Command) return NULL;
    return m_Command->getBufferPtr();
}

BaseCommand* Device::getCommandPtr()
{
    return m_Command;
}

eVendorCode Device::getVendorCode()
{
    return m_VendorCode;
}

eCommandType Device::getCommandType() const
{
    return m_CommandType;
}

U8* Device::getOutputTaskfilePtr()
{
    return (NULL == m_Command) ? NULL : m_Command->getOutputTaskfilePtr();
}

eCommandError Device::getErrorStatus() const
{
    return m_Command->getErrorStatus();
}

U32 Device::getMaxTransferSize() const
{
    return m_TransferLength;
}

U32 Device::resolveCommandCode(eCommonCommandCode cmdCode) const
{
    const U32 mapTable[] = {
        (U32) ATA_IDENTIFY_DEVICE,
        (U32) ATA_SMART_READ_DATA,
        (U32) ATA_SMART_READ_THRESHOLD,
        (U32) ATA_READ_DMA,
        (U32) ATA_WRITE_DMA,
        (U32) ATA_READ_SECTOR,
        (U32) ATA_WRITE_SECTOR,

        (U32) SAT_IDENTIFY_DEVICE,
        (U32) SAT_SMART_READ_DATA,
        (U32) SAT_SMART_READ_THRESHOLD,
        (U32) SAT_READ_DMA,
        (U32) SAT_WRITE_DMA,
        (U32) SAT_READ_SECTOR,
        (U32) SAT_WRITE_SECTOR,

        // Using ATA instead of STP (dummy code)
        (U32) ATA_IDENTIFY_DEVICE,
        (U32) ATA_SMART_READ_DATA,
        (U32) ATA_SMART_READ_THRESHOLD,
        (U32) ATA_READ_DMA,
        (U32) ATA_WRITE_DMA,
        (U32) ATA_READ_SECTOR,
        (U32) ATA_WRITE_SECTOR,

        (U32) SCSI_INQUIRY,
        (U32) SCSI_INVALID_COMMAND,
        (U32) SCSI_INVALID_COMMAND,
        (U32) SCSI_READ_10,
        (U32) SCSI_WRITE_10,
        (U32) SCSI_READ_10,
        (U32) SCSI_WRITE_10,

        // Using ATA instead of CSMI (dummy code)
        (U32) ATA_IDENTIFY_DEVICE,
        (U32) ATA_SMART_READ_DATA,
        (U32) ATA_SMART_READ_THRESHOLD,
        (U32) ATA_READ_DMA,
        (U32) ATA_WRITE_DMA,
        (U32) ATA_READ_SECTOR,
        (U32) ATA_WRITE_SECTOR,
    };

    int cmdCount = sizeof (mapTable) / sizeof (mapTable[0]);

    int cmdIndex = m_CommandType * CMDCODE_COUNT + cmdCode;

    ASSERT(cmdIndex < cmdCount);

    return mapTable[cmdIndex];
}

bool Device::prepareResource()
{
    if(1) { // Prepare Command
        switch(m_CommandType)
        {
        case CMDTYPE_ATA: m_Command = new ATACommand(); break;
        case CMDTYPE_SAT: m_Command = new SATCommand(); break;
        case CMDTYPE_SCSI: m_Command = new SCSICommand(); break;
        default: ASSERT(0); break;
        }
    }

    m_Command->setCommandBasic(0, 256, CMDCODE_IDENTIFY);
    m_Command->allocBuffer();

    return true;
}

bool Device::initDevice(const string &driveName)
{
    // Open device to get handle
    // Scan BusType.
    // Read Identify/SMART info. Also checking for SAT mode
    // Prepare commands, buffers

    reset();

    bool status = false;

    do {
        m_Name = driveName;

        if (false == DeviceUtil::OpenDevice(m_Name, m_Handle)) break;

        // Update BusType, MaxTransfer, checking SAT mode if needed
        if (false == updateCommandType()) break;

        // Prepare commands, Buffers
        if (false == prepareResource()) break;
        
        // Prepare other info (optional)
        if (true == readIdentify())
        {
            m_VendorCode = DeviceUtil::GetVendorCode(getBufferPtr());
        }
        
        status = true;
    } while(0);

    return status;
}

bool Device::initDeviceWithSerialNumber(const string &serialNo)
{
    vector<sPhysicalDriveInfo> driveList;
    DeviceUtil::OpenDevice(driveList);
    DeviceUtil::CloseDevice(driveList);

    bool status = false;
    string activeDriveName;
    int driveCount = driveList.size();
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

string Device::toString() const
{
    stringstream sstr;

    sstr << "Name: " << m_Name << endl;
    sstr << "Handle: " << m_Handle << endl;
    sstr << "MaxTransfer(sector): " << m_TransferLength / 512 << endl;

    switch (m_CommandType)
    {
        case CMDTYPE_ATA:  sstr << "CommandType: ATA" << endl; break;
        case CMDTYPE_SAT:  sstr << "CommandType: SAT" << endl; break;
        case CMDTYPE_STP:  sstr << "CommandType: STP" << endl; break;
        case CMDTYPE_HYP:  sstr << "CommandType: HYP" << endl; break;
        case CMDTYPE_SCSI: sstr << "CommandType: SCSI" << endl; break;
        case CMDTYPE_CSMI: sstr << "CommandType: CSMI" << endl; break;
    }

    return sstr.str();
}

bool Device::executeCommand(U32 lba, U32 sectorCount, U32 cmdCode)
{
    m_Command->setCommand(lba, sectorCount, cmdCode, false);
    return m_Command->executeCommand(m_Handle);
}

bool Device::executeCommand(U32 lba, U32 sectorCount, eCommonCommandCode cmdCode)
{
    // resolve command code from commonCommandCode
    U32 concreteCode = resolveCommandCode(cmdCode);

    return executeCommand(lba, sectorCount, concreteCode);
}

bool Device::readSmart(U8* valBuffer, U8* thrBuffer)
{
    bool status = false;

    do {
        if (false == readSmartData()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        memcpy(valBuffer, getBufferPtr(), 512);

        if (false == readSmartThreshold()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        memcpy(thrBuffer, getBufferPtr(), 512);

        status = true;
    } while(0);

    return status;
}

bool Device::readSmart(sSmartInfo &smartInfo)
{
    bool status = false;
    U8 valBuffer[512], thrBuffer[512];

    do {
        if (false == readSmart(valBuffer, thrBuffer)) break;

        struct sSmartData smartData;
        smartData.setVendorCode(m_VendorCode);
        smartData.setSmartBuffer(valBuffer, thrBuffer);

        SmartUtil::ParseSmartData(smartData, smartInfo);

        status = true;
    } while(0);

    return status;
}

bool Device::readIdentify(U8 *identifyBuffer)
{
    bool status = false;

    do {
        if (false == readIdentify()) break;
        if (CMD_ERROR_NONE != getErrorStatus()) break;

        memcpy(identifyBuffer, getBufferPtr(), 512);

        status = true;
    } while(0);

    return status;
}

bool Device::readIdentify(sIdentifyInfo &driveInfo)
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

bool Device::readIdentify()
{
    return executeCommand(0, 1, CMDCODE_IDENTIFY);
}

bool Device::readSmartData()
{
    return executeCommand(0, 1, CMDCODE_SMART_DATA);
}

bool Device::readSmartThreshold()
{
    return executeCommand(0, 1, CMDCODE_SMART_THRESHOLD);
}

bool Device::read(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, CMDCODE_READ);
}

bool Device::write(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, CMDCODE_WRITE);
}

bool Device::readSector(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, CMDCODE_READ_SECTOR);
}

bool Device::writeSector(U32 lba, U32 sectorCount)
{
    return executeCommand(lba, sectorCount, CMDCODE_WRITE_SECTOR);
}

bool Device::readCapacity(U32 &lowCap, U32 &highCap)
{
    switch(m_CommandType)
    {
        case CMDTYPE_ATA:
        case CMDTYPE_SAT:
        {
            // read capacity from IdentifySector
            if (false == readIdentify()) return false;

            sIdentifyInfo driveInfo;
            DeviceUtil::ParseIdentifyInfo(getBufferPtr(), m_Name, driveInfo);

            lowCap = driveInfo.LowCapacity;
            highCap = driveInfo.HighCapacity;
        } break;

        case CMDTYPE_SCSI:
        {
            // read capacity from command
            m_Command->setCommand(0, 1, SCSI_READ_CAPACITY);
            if (false == m_Command->executeCommand(m_Handle)) return false;

            const U8* buffPtr = m_Command->getBufferPtr();

            U32 blockCount = GetDWord(buffPtr, 0, false);
            U32 blockSize = GetDWord(buffPtr, 1, false);

            if (512 != blockSize) return false;

            // U32 m = blockCount >> 23;
            // U32 n = blockCount & 0x007FFFFF;
            // lowCap = n << 9;
            // highCap = m;
			
			lowCap = blockCount;
			highCap = 0;
        } break;

        default: return false;
    }

    return true;
}

void Device::fillWriteBuffer(const U8 *buffPtr, U32 buffSizeInByte)
{
    ASSERT (buffSizeInByte <= 256 * 512);
    U8* cmdBuffPtr = m_Command->getBufferPtr();

    memcpy(cmdBuffPtr, buffPtr, buffSizeInByte);
}

void Device::fillWritePattern(const U8 *patPtr, U32 patSizeInByte)
{
    m_Command->formatBuffer(patPtr, patSizeInByte, 0);
}

void Device::formatBufferHeader(U32 lba, U32 sectorCount, U32 writeCount, U32 offset)
{
    m_Command->formatAddress(lba, sectorCount, writeCount, offset);
}

void Device::randomWriteBuffer(U32 sectorCount)
{
    m_Command->randomBuffer(sectorCount);
}

bool Device::encryptBuffer(U32 sectorCount)
{
    return m_Command->encryptBuffer(sectorCount);
}

bool Device::verifyBuffer(U32 sectorCount)
{
    return m_Command->verifyBuffer(sectorCount);
}

string Device::getDriveName()
{
    return m_Name;
}
