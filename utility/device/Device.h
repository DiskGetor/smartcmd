#ifndef DEVICE_H
#define DEVICE_H

#include "core/CoreData.h"
#include "utility/cmd/ATACommand.h"
#include "utility/cmd/SATCommand.h"
#include "utility/cmd/SCSICommand.h"

#include "BaseDevice.h"

enum eCommonCommandCode
{
    CMDCODE_IDENTIFY,        // Map to ATA_IDENTIFY, SAT_IDENTIFY, SCSI_INQUIRY
    CMDCODE_SMART_DATA,
    CMDCODE_SMART_THRESHOLD,
    CMDCODE_READ,            // Map to ATA_READ_DMA, SAT_READ_DMA, SCSI_READ_10
    CMDCODE_WRITE,           // Map to ATA_WRITE_DMA, SAT_WRITE_DMA, SCSI_WRITE_10
    CMDCODE_READ_SECTOR,     // Map to ATA_READ_SECTOR, SAT_READ_SECTOR, SCSI_READ_10
    CMDCODE_WRITE_SECTOR,    // Map to ATA_WRITE_SECTOR, SAT_WRITE_SECTOR, SCSI_WRITE_10
    CMDCODE_COUNT,
};

class Device
{
public:
    Device();
    ~Device();

    void reset();
    bool initDevice(const string& driveName);
    bool initDeviceWithSerialNumber(const string& serialNo);

public:
    U8* getStructPtr();
    U8* getBufferPtr();
    BaseCommand* getCommandPtr();
    eVendorCode getVendorCode();
    eCommandType getCommandType() const;
    U8* getOutputTaskfilePtr();
    string getDriveName();

    string toString() const;
    U32 getMaxTransferSize() const;
    eCommandError getErrorStatus() const;

public:
    bool readIdentify();                         // general function
    bool readSmartData();                        // general function
    bool readSmartThreshold();                   // general function
    bool read(U32 lba, U32 sectorCount);         // general function
    bool write(U32 lba, U32 sectorCount);        // general function
    bool readCapacity(U32 &lowCap, U32& highCap);// general function
    bool readSector(U32 lba, U32 sectorCount);   // general function
    bool writeSector(U32 lba, U32 sectorCount);  // general function

    bool readSmart(sSmartInfo& smartInfo);
    bool readIdentify(sIdentifyInfo& driveInfo);

    bool readSmart(U8* valBuffer, U8* thrBuffer);
    bool readIdentify(U8* identifyBuffer);

public:
    // Format output buffer (for write command)
    void fillWriteBuffer(const U8* buffPtr, U32 buffSizeInByte); // Fill buffer using user data
    void fillWritePattern(const U8* patPtr, U32 patSizeInByte);  // Fill buffer using pattern data
    void formatBufferHeader(U32 lba, U32 sectorCount, U32 writeCount, U32 offset = 0); // Format header

    void randomWriteBuffer(U32 sectorCount);
    bool encryptBuffer(U32 sectorCount);
    bool verifyBuffer(U32 sectorCount);

public:
    bool executeCommand(U32 lba, U32 sectorCount, U32 code);
    bool executeCommand(U32 lba, U32 sectorCount, eCommonCommandCode cmdCode);

public:
    bool prepareResource();
    bool updateCommandType();
    U32 resolveCommandCode(eCommonCommandCode cmdCode) const;

private:
    string m_Name;
    int m_Handle;

    U32 m_TransferLength;
    eCommandType m_CommandType;
    eVendorCode m_VendorCode;

    BaseCommand* m_Command;
};

#endif // DEVICE_H

