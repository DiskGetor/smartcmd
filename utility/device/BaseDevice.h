#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "core/CoreData.h"
#include "utility/misc/StdHeader.h"
#include "utility/misc/StdMacro.h"
#include "utility/cmd/BaseCommand.h"
#include "utility/device/VendorCode.h"

enum eCommandType
{
    CMDTYPE_ATA,
    CMDTYPE_SAT,
    CMDTYPE_STP,
    CMDTYPE_SCSI,
    CMDTYPE_CSMI,
    CMDTYPE_HYP,
    CMDTYPE_NVME,
    CMDTYPE_COUNT,
};

class BaseDevice
{
public:
    BaseDevice();
    ~BaseDevice();

public:
    virtual void reset();
    virtual bool initDevice(const string& driveName);
    virtual bool initDeviceWithSerialNumber(const string& serialNo);

public:
    virtual U8* getBufferPtr();
    virtual string toString() const;
    virtual U32 getMaxTransferSize() const;
    virtual eCommandError getErrorStatus() const;

public:
    virtual bool readSmart(sSmartInfo& smartInfo);
    virtual bool readInquiry(sInquiryInfo& driveInfo);
    virtual bool readIdentify(sIdentifyInfo& driveInfo);

public:
    virtual bool readIdentify() = 0;                  // general function
    virtual bool readSmartData() = 0;                 // general function
    virtual bool readSmartThreshold() = 0;            // general function
    virtual bool read(U32 lba, U32 sectorCount) = 0;  // general function
    virtual bool write(U32 lba, U32 sectorCount) = 0; // general function

public:
    virtual bool executeCommand(U32 lba, U32 sectorCount, U32 code);

public:
    virtual bool prepareResource();
    virtual bool updateCommandType();

public:
    // Utility functions for each command class: ATA, MMC, NVME
    bool readAtaSmart(sSmartInfo& smartInfo);
    bool readNvmeSmart(sSmartInfo& smartInfo);

    bool readAtaIdentify(sIdentifyInfo& driveInfo);
    bool readNvmeIdentify(sIdentifyInfo& driveInfo);

public:
    // Format output buffer (for write command)
    void fillWriteBuffer(U8* buffPtr, U32 buffSizeInByte);             // Fill buffer using user data
    void fillWritePattern(U8* patPtr, U32 patSizeInByte);              // Fill buffer using pattern data
    void formatBufferHeader(U32 lba, U32 sectorCount, U32 writeCount, U32 sectorOffset = 0); // Format header

    void randomWriteBuffer(U32 sectorCount);
    bool encryptBuffer(U32 sectorCount);
    bool verifyBuffer(U32 sectorCount);

public:
    string m_Name;
    int m_Handle;
    U32 m_TransferLength;
    eCommandType m_CommandType;

    BaseCommand* m_Command;
};

#endif // SMARTDEVICE_H

