#ifndef BASECOMMAND_H
#define BASECOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "CsmiSas.h"

enum eCommandClass
{
    CLASS_CSMI = 0, // Always keep CSMI zero
    CLASS_SCSI,
    CLASS_ATA,
    CLASS_VSC_HYPA1,
    CLASS_VSC_HYPF4,
    CLASS_VSC_HYPCF,
    CLASS_VSC_VRT,
    CLASS_VSC_SMI,
    CLASS_NVME,
    CLASS_NONE,
};

enum eDataCode
{
    DATA_NONE = 0,
    DATA_IN   = 1,
    DATA_OUT  = 2,
};

enum eTransferMode
{
    MODE_NONE = 0,
    MODE_PIO  = 1,
    MODE_DMA  = 2,
};

enum eCommandError
{
#define MAP_ITEM(code) code,
#include "CommandError.def"
#undef MAP_ITEM
};

enum eORGCommandCode
{
#define MAP_ITEM(code, ftr, val, cnt, cls, data, mode) code = ((cls << 28) | (data << 26) | (mode << 24) | (cnt << 16) | (ftr << 8) | val),
#include "ORGCommandCode.def"
#undef MAP_ITEM
};

struct sLBA
{
    U32 Low;
    U32 High;

    sLBA();
    sLBA(U32 low, U32 high = 0);
    sLBA operator+(U32 lba);
    sLBA operator+(const sLBA& lba);
    bool operator==(const sLBA& lba);
};

class BaseCommand
{
public:
    BaseCommand();
    virtual ~BaseCommand();

public:
    sLBA LBA;
    U32 SectorCount;
    U32 CommandCode;
    
    U8* Buffer;
    U32 StructSize;
    U32 FillerSize;
    U32 BufferSize;

    U32 MaxSectorCount;

public:
    U32 getStructSize() const;
    void allocBuffer();
    void releaseBuffer();

public:
    void reset();
    void resetCommandStruct();

public:
    void formatAddress(U32 writeCount);
    void formatAddress(U32 lba, U32 sectorCount, U32 writeCount, U32 sectorOffset = 0); // offset into buffer
    void formatBuffer(const U8 *pattern, U32 size);                                     // format buffer with pattern. don't change address region
    void formatBuffer(const U8 *pattern, U32 size, U32 writeCount);
    void fillBuffer(const U8* pattern, U32 size, U32 sectorCount);                      // fill some sectors in the command buffer

    void randomBuffer(U32 sectorCount);
    bool encryptBuffer(U32 sectorCount);
    bool verifyBuffer(U32 sectorCount);

public:
    bool isDmaMode() const;
    bool isPioMode() const;

    bool hasSectorCode() const;
    U8   getSectorCode() const;
    U32  getSectorCount() const;
    U8   getCommandCode() const;
    U8   getFeatureCode() const;
    bool isDataInCommand() const;
    bool isDataOutCommand() const;
    bool hasDataTransfer() const;
    eCommandClass getCommandClass() const;
    void setCommand(sLBA lba, U32 sectorCount, U32 cmdCode, bool newBuffer = true);

public:
    static U8 GetCommandCode(U8 orgCode);
    static eCommandError ParseSense(U8* sense);
    static eCommandError ParseSATSense(U8* sense);
    static string GetErrorString(eCommandError code);
    static string GetTaskfileString(U8* taskfile, U32 size);
    static string GetCommandString(U32 cmdCode);

public:
    virtual U8* getBufferPtr();
    virtual U8* getStructPtr();
    virtual U8* getOutputTaskfilePtr();

public:
    virtual bool executeCommand(int handle) = 0;
    virtual void setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode) = 0;
    virtual eCommandError getErrorStatus() = 0;
    virtual string getTaskfileString() = 0;

private:
    virtual bool buildCommand() = 0;      // setting flags, ..
    virtual void buildBasicCommand() = 0; // Initialize basic data

public:
    void buildFis(U8*);
    void buildSat12(U8*);
    void buildSat16(U8*);
    void buildTaskfile(U8*);
    void buildCmdblock(U8*);
    void buildTaskstruct(U8*);

private:
    // Utility function for building SCSI command block
    void buildCmdblock_Read10(U8*);
    void buildCmdblock_Write10(U8*);
    void buildCmdblock_Inquiry(U8*);
    void buildCmdblock_ReadCapacity(U8*);
};

#endif // BASECOMMAND_H
