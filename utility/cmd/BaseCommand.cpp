#include "utility/misc/CryptUtil.h"

#include "BaseCommand.h"

#define PROTOCOL_NO_DATA 0x06
#define PROTOCOL_PIO_IN  0x08
#define PROTOCOL_PIO_OUT 0x0A
#define PROTOCOL_DMA     0x0C

#define TLENGTH_NO_DATA    0x00
#define TLENGTH_IN_FEATURE 0x01
#define TLENGTH_IN_SECTOR  0x02
#define TLENGTH_IN_TPSIU   0x03

#define TRANSFER_BYTE      (0 << 2)
#define TRANSFER_BLOCK     (1 << 2)

#define TRANSFER_TO_DEV    (0 << 3)
#define TRANSFER_FROM_DEV  (1 << 3)

#define CHECK_CONDITION    (1 << 5)

sLBA::sLBA() : Low(0), High(0) {}
sLBA::sLBA(U32 lba, U32 high) : Low(lba), High(high) {}

sLBA sLBA::operator+ ( U32 lba )
{
    sLBA sum(lba, 0);
    return *this + sum;
}

sLBA sLBA::operator+ ( const sLBA& iLBA )
{
    sLBA sum(Low  + iLBA.Low, High + iLBA.High);
    if ( ( sum.Low < Low ) || ( sum.Low < iLBA.Low ) ) ++sum.High;
    return sum;
}

bool sLBA::operator== ( const sLBA& iLBA )
{
    return ( Low == iLBA.Low ) && ( High == iLBA.High );
}

void BaseCommand::reset()
{
    LBA = 0;
    SectorCount = 0;
    CommandCode = 0xFFFF;

    Buffer = NULL;
    MaxSectorCount = 0;
}

void BaseCommand::resetCommandStruct()
{
    memset((U8*)(void*)getStructPtr(), 0x00, StructSize);
}

BaseCommand::BaseCommand()
{
    reset();
}

BaseCommand::~BaseCommand()
{
    releaseBuffer();
}

U8* BaseCommand::getStructPtr()
{
    return Buffer;
}

U8* BaseCommand::getOutputTaskfilePtr()
{
    return NULL;
}

U32 BaseCommand::getStructSize() const
{
    return StructSize + FillerSize + BufferSize;
}

U8* BaseCommand::getBufferPtr()
{
    if (NULL == Buffer) return NULL;
    return Buffer + (StructSize + FillerSize);
}

void BaseCommand::allocBuffer()
{
    U32 TotalSize = BufferSize + StructSize + FillerSize;

    if (NULL != Buffer) releaseBuffer();

    MaxSectorCount = BufferSize / 512;

    if (0 == TotalSize) return;

    Buffer = new U8[TotalSize];
    memset(Buffer, 0x00, TotalSize);
}

void BaseCommand::releaseBuffer()
{
    if (NULL != Buffer) delete Buffer;

    MaxSectorCount = 0;

    Buffer = NULL;
}

U8 BaseCommand::getSectorCode() const
{
    return (CommandCode >> 16) & 0xFF;
}

bool BaseCommand::hasSectorCode() const
{
    return (CommandCode) & 0x00FF0000;
}

U32 BaseCommand::getSectorCount() const
{
    return hasSectorCode() ? getSectorCode() : SectorCount;
}

U8 BaseCommand::getFeatureCode() const
{
    return (CommandCode >> 8) & 0xFF;
}

U8 BaseCommand::getCommandCode() const
{
    return GetCommandCode(CommandCode);
}

bool BaseCommand::isDataOutCommand() const // Write
{
    return ((CommandCode >> 26) & 0x3) == DATA_OUT;
}

bool BaseCommand::isDataInCommand() const // Read
{
    return ((CommandCode >> 26) & 0x3) == DATA_IN;
}

bool BaseCommand::hasDataTransfer() const
{
    return ((CommandCode >> 26) & 0x3) != 0;
}

bool BaseCommand::isPioMode() const
{
    return ((CommandCode >> 24) & 0x3) == MODE_PIO;
}

bool BaseCommand::isDmaMode() const
{
    return ((CommandCode >> 24) & 0x3) == MODE_DMA;
}

eCommandClass BaseCommand::getCommandClass() const
{
    return eCommandClass((CommandCode >> 28) & 0xF);
}

U8 BaseCommand::GetCommandCode(U8 orgCode)
{
    return (orgCode) & 0xFF;
}

string BaseCommand::GetErrorString(eCommandError code)
{
    string errorString = "Unknown Error Code";
    switch (code)
    {
        #define MAP_ITEM(code) case code: errorString = #code; break;
        #include "CommandError.def"
        #undef MAP_ITEM
    }

    return errorString;
}

string BaseCommand::GetTaskfileString(U8 *taskfile, U32 size)
{
    stringstream sstr;

    sstr << "CommandBlock: ";

    for (U32 i = 0; i < size; i++)
    {
        U32 value = (U32) taskfile[i];
        // sstr << "[" << FRMT_U32(2, i) << "] ";
        sstr << setw(2) << setfill('0') << hex << uppercase
             << value << " ";
    }

    return sstr.str();
}

string BaseCommand::GetCommandString(U32 cmdCode)
{
    string cmdString = "UnknownCommand";

    switch (cmdCode)
    {
        #define MAP_ITEM(code, ftr, val, cnt, cls, data, mode) case code: cmdString = #code; break;
        #include "ORGCommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}

eCommandError BaseCommand::ParseSense(U8* sense)
{
    U8 responseCode = sense[0] & 0x7F;

    if (0 == responseCode) return CMD_ERROR_NONE;

    U8 senseKey = sense[2] & 0x0F;

    if (senseKey == 0) return CMD_ERROR_NONE;

    U8 senseCode = sense[12];
    U8 senseQualifier = sense[13];

    // Interpret value of senseCode and senseCodeQualifier
    // Temporary return ERROR_UNKN here

    (void) senseCode;
    (void) senseQualifier;

    return CMD_ERROR_UNKN;
}

eCommandError BaseCommand::ParseSATSense(U8* sense)
{
    U8 resCode = sense[0];

    if ((resCode < 0x72) || (resCode > 0x73)) return CMD_ERROR_NONE;

    if ((sense[8] != 0x09) || (sense[9] != 0x0C)) return CMD_ERROR_UNKN;

    if (0 != (sense[21] & 0x01)) return CMD_ERROR_ABORT;

    return CMD_ERROR_NONE;
}

void BaseCommand::formatAddress(U32 writeCount)
{
    formatAddress(LBA.Low, SectorCount, writeCount);
}

void BaseCommand::formatAddress(U32 lbaAddress, U32 sectorCount, U32 writeCount, U32 sectorOffset)
{
    if (sectorOffset >= MaxSectorCount) return;

    sectorCount = MIN2(sectorCount, MaxSectorCount - sectorOffset);

    U8* buffer = getBufferPtr() + sectorOffset * 512;

    for (U32 i = 0; i < sectorCount; i++)
    {
        stringstream sstr;
        sstr << "#LBA:" << FRMT_HEX(8) << lbaAddress << "h" << "  ";
        sstr << "#CNT:" << FRMT_DEC(4) << writeCount << "       ";
        string addrStr = sstr.str();

        memcpy(buffer, addrStr.c_str(), addrStr.length());

        lbaAddress += 1;
        buffer += 512;
    }
}

void BaseCommand::randomBuffer(U32 sectorCount)
{
    U32 bufSize = (rand() % 512) + 1;
    U8* bufPtr = new U8[bufSize];

    for (U32 i = 0; i < bufSize; i++) bufPtr[i] = rand() % 256;

    fillBuffer(bufPtr, bufSize, sectorCount);

    delete[] bufPtr;
}

void BaseCommand::formatBuffer(const U8 *pattern, U32 size, U32 writeCount)
{
    fillBuffer(pattern, size, SectorCount);

    if (0 != writeCount) formatAddress(writeCount);
}

void BaseCommand::formatBuffer(const U8 *pattern, U32 size)
{
    // Don't use this function

    fillBuffer(pattern, size, SectorCount);
}

void BaseCommand::fillBuffer(const U8 *pattern, U32 size, U32 sectorCount)
{
    /* 0000:AB281009h
     * 0020:20
     * 0040:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0060:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0080:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 00A0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 00C0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 00E0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0100:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0120:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0140:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0160:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 0180:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 01A0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 01C0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
     * 01E0:PPPPPPPPPPPPPPPPPPPPPPPPPPP
    */

    sectorCount = MIN2(sectorCount, MaxSectorCount);

    U32 totalByte = sectorCount * 512;
    U8* buffer = getBufferPtr();

    while(totalByte > 0)
    {
        U32 byteCount = MIN2(size, totalByte);
        memcpy(buffer, pattern, byteCount);

        buffer += byteCount;
        totalByte -= byteCount;
    }
}

bool BaseCommand::encryptBuffer(U32 sectorCount)
{
    sectorCount = MIN2(sectorCount, MaxSectorCount);
    return CryptUtil::EncryptSector(getBufferPtr(), sectorCount, CRYPT_MD5);
}

bool BaseCommand::verifyBuffer(U32 sectorCount)
{
    sectorCount = MIN2(sectorCount, MaxSectorCount);
    return CryptUtil::VerifySector(getBufferPtr(), sectorCount, CRYPT_MD5);
}

void BaseCommand::setCommand(sLBA lba, U32 sectorCount, U32 cmdCode, bool newBuffer)
{
    setCommandBasic(lba, sectorCount, cmdCode);
    if (true == newBuffer) allocBuffer();
    buildCommand();
}

// -----------------------------------------------------------------
// Build command block
// -----------------------------------------------------------------

void BaseCommand::buildFis(U8 *fis)
{
    fis[ 0] = 0x27;                       // Type: host-to-device FIS
    fis[ 1] = 0x80;                       // Bit7: Update command register
    fis[ 2] = (CommandCode) & 0xFF;       // lo.command;
    fis[ 3] = (CommandCode >> 8) & 0xFF;  // lo.features;
    fis[ 4] = LBA.Low & 0xFF;             // lo.lba_low;
    fis[ 5] = (LBA.Low >>  8) & 0xFF;     // lo.lba_mid;
    fis[ 6] = (LBA.Low >> 16) & 0xFF;     // lo.lba_high;
    fis[ 7] = 0x40;                       // lo.device;
    fis[ 8] = LBA.High & 0xFF;            // hi.lba_low;
    fis[ 9] = (LBA.High >>  8) & 0xFF;    // hi.lba_mid;
    fis[10] = (LBA.High >> 16) & 0xFF;    // hi.lba_high;
    fis[11] = 0;                          // hi.features;
    fis[12] = SectorCount;                // lo.sector_count;
    fis[13] = 0;                          // hi.sector_count;
    // ------------------------------------------------------
    // Update for specific commands
    // ------------------------------------------------------
    switch (CommandCode)
    {
    case ORG_ATA_SMART_ENABLE:
    case ORG_ATA_SMART_DISABLE:
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
        fis[5] = 0x4F;
        fis[6] = 0xC2;
        break;

    default: break;
    }
}

void BaseCommand::buildSat12(U8 *sat)
{
    // ------------------------------------------------------
    // Initialize sat
    // ------------------------------------------------------
    sat[ 0] = 0xA1;                      // SCSI command code

    // Build sat[1]: MULTIPLE_COUNT | pROTOCOL | EXTEND
    U8 protocol = 0;
    if (false == hasDataTransfer()) protocol = PROTOCOL_NO_DATA;
    else {
        if (true == isDmaMode()) protocol = PROTOCOL_DMA;
        else {
            if (true == isDataInCommand()) protocol = PROTOCOL_PIO_IN;
            if (true == isDataOutCommand()) protocol = PROTOCOL_PIO_OUT;
        }
    }

    sat[ 1] |= protocol;

    if(true == hasDataTransfer())
    {
        sat[2] |= TRANSFER_BLOCK;
        sat[2] |= TLENGTH_IN_SECTOR;
        sat[2] |= isDataInCommand() ? TRANSFER_FROM_DEV : TRANSFER_TO_DEV;
    }
    else
    {
        sat[2] = CHECK_CONDITION;
    }

    sat[ 3] = getFeatureCode(); // Features
    sat[ 4] = getSectorCount(); // SecCount

    sat[ 5] = LBA.Low & 0xFF;

    sat[ 6] = (LBA.Low >>  8) & 0xFF;

    sat[ 7] = (LBA.Low >> 16) & 0xFF;

    sat[ 8] = (LBA.Low >> 24) & 0xF ;     // Device
    sat[ 9] = getCommandCode(); // CommandCode

    // ------------------------------------------------------
    // Update for specific commands
    // ------------------------------------------------------
    switch (CommandCode)
    {
    case ORG_ATA_SMART_ENABLE:
    case ORG_ATA_SMART_DISABLE:
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
        sat[6] = 0x4F;
        sat[7] = 0xC2;
        break;

    case ORG_ATA_READ_SECTOR: sat[8] |= 0x40; break;
    case ORG_ATA_WRITE_SECTOR: sat[8] |= 0x40; break;

    case ORG_ATA_READ_DMA: sat[8] |= 0x40; break;
    case ORG_ATA_WRITE_DMA: sat[8] |= 0x40; break;

    // VSC Direct
    case ORG_VSC_HYPCF_PHYSICAL_ERASE: sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_DOWNLOAD:       sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_RESET_CARD:     sat[4] = 0x03; sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_LONG:      sat[4] = 0x02; sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_WRITE_LONG:     sat[4] = 0x02; sat[8] |= 0xE0; break;

    // VSC via SetFeature
    case ORG_VSC_HYPCF_READ_CONTROLLER:sat[4] = 0x44; sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_SET_FEATURE:    sat[4] = 0xE2; sat[8] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_SECTOR:    sat[8] |= 0xE0; break;

    // VSC SMI - Sanitize
    case ORG_VSC_SMI_SANITIZE_01H:
    case ORG_VSC_SMI_SANITIZE_03H:
    case ORG_VSC_SMI_SANITIZE_80H:
    case ORG_VSC_SMI_SANITIZE_81H:
    case ORG_VSC_SMI_SANITIZE_83H:
        sat[5] = 0x49; sat[6] = 0x4D; sat[7] = 0x53; break;

    default: break;
    }

    switch (getCommandClass())
    {
    case CLASS_VSC_HYPA1:
    case CLASS_VSC_HYPF4: sat[8] |= 0xE0; break;
    case CLASS_VSC_VRT:   sat[8] |= 0xE0; break;
    default: break;
    }
}

void BaseCommand::buildSat16(U8 *sat)
{
    // ------------------------------------------------------
    // Initialize sat
    // ------------------------------------------------------
    sat[ 0] = 0x85;                      // SCSI command code

    // Build sat[1]: MULTIPLE_COUNT | pROTOCOL | EXTEND
    U8 transferMode = 0;
    if (false == hasDataTransfer()) transferMode = PROTOCOL_NO_DATA;
    else {
        if (true == isDmaMode()) transferMode = PROTOCOL_DMA;
        else {
            if (true == isDataInCommand()) transferMode = PROTOCOL_PIO_IN;
            if (true == isDataOutCommand()) transferMode = PROTOCOL_PIO_OUT;
        }
    }

    sat[ 1] |= transferMode;

    if(true == hasDataTransfer())
    {
        sat[2] |= TRANSFER_BLOCK;
        sat[2] |= TLENGTH_IN_SECTOR;
        sat[2] |= isDataInCommand() ? TRANSFER_FROM_DEV : TRANSFER_TO_DEV;
    }
    else
    {
        sat[2] = CHECK_CONDITION;
    }

    sat[ 3] = 0;                         // Feature
    sat[ 4] = getFeatureCode();

    sat[ 5] = 0;                         // Sector Count
    sat[ 6] = getSectorCount();

    sat[ 7] = 0;                         // LBA (7 : 12)
    sat[ 8] = LBA.Low & 0xFF;

    sat[ 9] = 0;
    sat[10] = (LBA.Low >>  8) & 0xFF;

    sat[11] = 0;
    sat[12] = (LBA.Low >> 16) & 0xFF;

    sat[13] = (LBA.Low >> 24) & 0xF ;     // Device
    sat[14] = getCommandCode();           // CommandCode

    // ------------------------------------------------------
    // Update for specific commands
    // ------------------------------------------------------
    switch (CommandCode)
    {
    case ORG_ATA_SMART_ENABLE:
    case ORG_ATA_SMART_DISABLE:
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
        sat[10] = 0x4F;
        sat[12] = 0xC2;
        break;

    case ORG_ATA_READ_SECTOR: sat[13] |= 0x40; break;
    case ORG_ATA_WRITE_SECTOR: sat[13] |= 0x40; break;

    case ORG_ATA_READ_DMA: sat[13] |= 0x40; break;
    case ORG_ATA_WRITE_DMA: sat[13] |= 0x40; break;

    // VSC Direct
    case ORG_VSC_HYPCF_PHYSICAL_ERASE: sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_DOWNLOAD:       sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_RESET_CARD:     sat[6] = 0x03; sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_LONG:      sat[6] = 0x02; sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_WRITE_LONG:     sat[6] = 0x02; sat[13] |= 0xE0; break;

    // VSC via SetFeature
    case ORG_VSC_HYPCF_READ_CONTROLLER:sat[6] = 0x44; sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_SET_FEATURE:    sat[6] = 0xE3; sat[13] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_SECTOR:    sat[13] |= 0xE0; break;

    // VSC SMI - Sanitize
    case ORG_VSC_SMI_SANITIZE_01H:
    case ORG_VSC_SMI_SANITIZE_03H:
    case ORG_VSC_SMI_SANITIZE_80H:
    case ORG_VSC_SMI_SANITIZE_81H:
    case ORG_VSC_SMI_SANITIZE_83H:
        sat[8] = 0x49; sat[10] = 0x4D; sat[12] = 0x53; break;

    default: break;
    }

    switch (getCommandClass())
    {
    case CLASS_VSC_HYPA1:
    case CLASS_VSC_HYPF4: sat[13] |= 0xE0; break;
    default: break;
    }
}

void BaseCommand::buildTaskfile(U8 *task)
{
    // ------------------------------------------------------
    // Initialize TaskFile
    // ------------------------------------------------------
    task[0] = getFeatureCode(); // Features
    task[1] = getSectorCount(); // Count
    task[2] = LBA.Low & 0xFF;
    task[3] = (LBA.Low >>  8) & 0xFF;
    task[4] = (LBA.Low >> 16) & 0xFF;
    task[5] = (LBA.Low >> 24) & 0xF;
    task[6] = getCommandCode(); // Command

    // ------------------------------------------------------
    // Update for specific commands
    // ------------------------------------------------------
    switch (CommandCode)
    {
    case ORG_ATA_SMART_ENABLE:
    case ORG_ATA_SMART_DISABLE:
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
        task[3] |= 0x4F; // LBA Mid
        task[4] |= 0xC2; // LBA High
        break;

    case ORG_ATA_READ_SECTOR: task[5] |= 0x40; break;
    case ORG_ATA_WRITE_SECTOR: task[5] |= 0x40; break;

    case ORG_ATA_READ_DMA: task[5] |= 0x40; break;
    case ORG_ATA_WRITE_DMA: task[5] |= 0x40; break;

    // VSC Direct
    case ORG_VSC_HYPCF_PHYSICAL_ERASE:  task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_DOWNLOAD:        task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_RESET_CARD:      task[1] = 0x03; task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_LONG:       task[1] = 0x02; task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_WRITE_LONG:      task[1] = 0x02; task[5] |= 0xE0; break;

    // VSC via SetFeature
    case ORG_VSC_HYPCF_READ_CONTROLLER: task[1] = 0x44; task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_SET_FEATURE:     task[1] = 0xE3; task[5] |= 0xE0; break;
    case ORG_VSC_HYPCF_READ_SECTOR:     task[5] |= 0xE0; break;

    // VSC SMI - Sanitize
    case ORG_VSC_SMI_SANITIZE_01H:
    case ORG_VSC_SMI_SANITIZE_03H:
    case ORG_VSC_SMI_SANITIZE_80H:
    case ORG_VSC_SMI_SANITIZE_81H:
    case ORG_VSC_SMI_SANITIZE_83H:
        task[2] = 0x49; task[3] = 0x4D; task[4] = 0x53; break;

    default: break;
    }

    switch (getCommandClass())
    {
    case CLASS_VSC_HYPA1:
    case CLASS_VSC_HYPF4: task[5] |= 0xE0; break;
    default: break;
    }
}

void BaseCommand::buildTaskstruct(U8 *task)
{
    // ------------------------------------------------------
    // Initialize TaskFile
    // ------------------------------------------------------
    task[0] = 0;                // Data (unknown)
    task[1] = getFeatureCode(); // Features
    task[2] = getSectorCount(); // Count
    task[3] = LBA.Low & 0xFF;
    task[4] = (LBA.Low >>  8) & 0xFF;
    task[5] = (LBA.Low >> 16) & 0xFF;
    task[6] = (LBA.Low >> 24) & 0xF;
    task[7] = getCommandCode(); // Command

    // ------------------------------------------------------
    // Update for specific commands
    // ------------------------------------------------------
    switch (CommandCode)
    {
    case ORG_ATA_SMART_ENABLE:
    case ORG_ATA_SMART_DISABLE:
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
        task[4] |= 0x4F; // LBA Mid
        task[5] |= 0xC2; // LBA High
        break;

    default: break;
    }

    switch (getCommandClass())
    {
    case CLASS_ATA: task[6] |= 0x40; break;

    case CLASS_VSC_HYPA1:
    case CLASS_VSC_HYPF4: task[5] |= 0xE0; break;
    default: break;
    }
}

void BaseCommand::buildCmdblock(U8 *cmdb)
{
    // ------------------------------------------------------
    // Initialize cmdb
    // ------------------------------------------------------
    switch (CommandCode)
    {
        case ORG_SCSI_INQUIRY:       buildCmdblock_Inquiry(cmdb); break;
        case ORG_SCSI_READ_10:       buildCmdblock_Read10(cmdb); break;
        case ORG_SCSI_WRITE_10:      buildCmdblock_Write10(cmdb); break;
        case ORG_SCSI_READ_CAPACITY: buildCmdblock_ReadCapacity(cmdb); break;
        default: break;
    }
}

void BaseCommand::buildCmdblock_Read10(U8 *cmdb)
{
    cmdb[0] = getCommandCode();
    cmdb[2] = (LBA.Low >> 24) & 0xFF;
    cmdb[3] = (LBA.Low >> 16) & 0xFF;
    cmdb[4] = (LBA.Low >>  8) & 0xFF;
    cmdb[5] = (LBA.Low >>  0) & 0xFF;
    cmdb[7] = 0;
    cmdb[8] = getSectorCount();
}

void BaseCommand::buildCmdblock_Write10(U8 *cmdb)
{
    cmdb[0] = getCommandCode();
    cmdb[2] = (LBA.Low >> 24) & 0xFF;
    cmdb[3] = (LBA.Low >> 16) & 0xFF;
    cmdb[4] = (LBA.Low >>  8) & 0xFF;
    cmdb[5] = (LBA.Low >>  0) & 0xFF;
    cmdb[7] = 0;
    cmdb[8] = getSectorCount();
}

void BaseCommand::buildCmdblock_Inquiry(U8 *cmdb)
{
    FILL_REGION(cmdb, 0x00, 6);
    cmdb[0] = getCommandCode();
}

void BaseCommand::buildCmdblock_ReadCapacity(U8 *cmdb)
{
    FILL_REGION(cmdb, 0x00, 10);
    cmdb[0] = getCommandCode();
}
