#ifndef VSC_VIRTIUM_DEFINE_H
#define VSC_VIRTIUM_DEFINE_H

#include "utility/misc/StdMacro.h"

class VscVirtiumConstant
{
public:
    // VSC command null
    static const U8 VSC_NO_COMMAND = 0x00;

    // NAND access command
    static const U32 VSC_NAND_ACCESS_COMMAND = 0x00000001;
    static const U32 VSC_SUB_ERASE_PHYSICAL_BLOCK = 0x00000001;
    static const U32 VSC_SUB_WRITE_PHYSICAL_PAGE = 0x00000002;
    static const U32 VSC_SUB_READ_PHYSICAL_PAGE = 0x00000003;
    static const U32 VSC_SUB_WRITE_PHYSICAL_PAGE_WITH_SPARE = 0x00000004;
    static const U32 VSC_SUB_READ_PHYSICAL_PAGE_WITH_SPARE = 0x00000005;
    static const U32 VSC_SUB_READ_SPARE = 0x00000006;
    static const U32 VSC_SUB_READ_SECTORS = 0x00000007;
    static const U32 VSC_SUB_WRITE_SPARE = 0x00000008;
    static const U32 VSC_SUB_READ_PHYSICAL_PAGE_WITH_UECC = 0x00000009;
    static const U32 VSC_SUB_ERASE_PHYSICAL_BLOCK_DUAL_PLANE = 0x0000000A;
    static const U32 VSC_SUB_READ_PHYSICAL_PAGE_DUAL_PLANE = 0x0000000B;
    static const U32 VSC_SUB_WRITE_PHYSICAL_PAGE_WITH_SPARE_DUAL_PLANE = 0x0000000C;
    // TODO: Nand interface currently has not supported this operation yet
    static const U32 VSC_SUB_WRITE_PHYSICAL_PAGE_DUAL_PLANE = 0x0000000D;
    static const U32 VSC_SUB_READ_PHYSICAL_PAGE_WITH_SPARE_DUAL_PLANE = 0x0000000E;

    // Get firmware mode
    static const U32 VSC_GET_FIRMWARE_MODE = 0x00000002;

    // System table information
    static const U32 VSC_SYSTEM_TABLE_INFORMATION = 0x00000003;

    // Read ID
    static const U32 VSC_READ_FROM_MEMORY = 0x00000004;
    static const U32 VSC_SUB_READ_ADDRESS = 0x00000001;
    static const U32 VSC_SUB_READ_IDENTIFICATION_TABLE = 0x00000002;
    static const U32 VSC_SUB_READ_SMART_TABLE = 0x00000005;
    static const U32 VSC_SUB_GET_SWL_CONFIG = 0x00000006;
    static const U32 VSC_SUB_READ_ICCM = 0x00000007;        // obsolete in g2
    static const U32 VSC_SUB_READ_PROD_EXT = 0x00000008;        // obsolete in g2
    static const U32 VSC_SUB_READ_ACTIVE_WRITE_CMD = 0x00000009;
    static const U32 VSC_SUB_READ_ACTIVE_READ_CMD = 0x0000000A;
    static const U32 VSC_SUB_READ_NAND_CMD = 0x0000000B;
    static const U32 VSC_SUB_READ_CHIP_MAP = 0x0000000C;
    static const U32 VSC_SUB_READ_REVISION_CMD = 0x0000000D;
    static const U32 VSC_SUB_GET_ASSERT_LOG = 0x0000000E;
    static const U32 VSC_SUB_GET_ASSERT_PARAMETER = 0x0000000F;
    static const U32 VSC_SUB_READ_DEFECT_TABLE = 0x00000010;
    static const U32 VSC_SUB_GET_DEFECT_TABLE_SECTOR_SIZE = 0x00000011;
    static const U32 VSC_SUB_GET_DEFECT_BLOCK_LIST = 0x00000012;
    static const U32 VSC_SUB_GET_DEFECT_BLOCK_LIST_SECTOR_SIZE = 0x00000013;
    static const U32 VSC_SUB_GET_TABLEID_COUNTS_INFO = 0x00000014;
    static const U32 VSC_SUB_GET_NAND_FIFO = 0x00000015;
    static const U32 VSC_SUB_GET_NAND_FIFO_STATUS = 0x00000016;
    static const U32 VSC_SUB_GET_NAND_FIFO_TSB = 0x00000017;
    static const U32 VSC_SUB_GET_GPIO_EMULATOR_INFO = 0x00000018;
    static const U32 VSC_SUB_GET_CURRENT_WRITE_PAGE_SYSTEM_TABLES = 0x00000019;
    static const U32 VSC_SUB_GET_DRIVE_CONFIG = 0x0000001A;
    static const U32 VSC_SUB_GET_UNUSED_BLOCK = 0x0000001B;
    static const U32 VSC_SUB_CHECK_IDM_IDLE_STATE = 0x0000001C;
    static const U32 VSC_SUB_GET_TABLEID = 0x0000001D;
    static const U32 VSC_SUB_GET_EVENT_LOGGER_CIRCULAR_LOG_ENTRY_INFO = 0x0000001E;
    static const U32 VSC_SUB_GET_EVENT_LOGGER_MEMORY_CONFIG_INFO = 0x0000001F;
    static const U32 VSC_SUB_GET_ERASE_COUNT_INFO = 0x00000020;
    static const U32 VSC_SUB_GET_LAST_NAND_COMMAND_STATUS = 0x00000021;
    static const U32 VSC_SUB_READ_NAND_ID = 0x00000022;
    static const U32 VSC_SUB_READ_NAND_LOG = 0x00000023;
    static const U32 VSC_SUB_GET_USER_CAPACITY_MAX = 0x00000024;
    static const U32 VSC_SUB_GET_TOTAL_CAPACITY = 0x00000025;
    static const U32 VSC_SUB_GET_WRITE_DUAL_PLANE_TEST_RESULT = 0x00000026;
    static const U32 VSC_SUB_GET_ERASE_COUNT_LOG_TABLE_INFO = 0x00000027;
    static const U32 VSC_SUB_GET_WORK_LOAD_LOG_TABLE_INFO = 0x00000028;
    static const U32 VSC_SUB_GET_INITIAL_DATE = 0x00000029;
    static const U32 VSC_SUB_GET_INTERLEAVING_ADDRESS = 0x0000002A;
    static const U32 VSC_SUB_CHECK_ENABLE_SCRAMBLER = 0x0000002B;
    static const U32 VSC_SUB_GET_CURRENT_LINKED_BLOCK_HDM_INFO = 0x0000002D;
    static const U32 VSC_SUB_CHIP_MAP_DISCOVERY = 0x0000002E;

    // Write ID
    static const U32 VSC_WRITE_TO_MEMORY = 0x00000005;
    static const U32 VSC_SUB_WRITE_ADDRESS = 0x00000001;
    static const U32 VSC_SUB_WRITE_IDENTIFICATION_TABLE = 0x00000002;
    static const U32 VSC_SUB_WRITE_SMART_TABLE = 0x00000003;
    static const U32 VSC_SUB_WRITE_ICCM = 0x00000004;        // obsolete in g2
    static const U32 VSC_SUB_WRITE_PROD_EXT = 0x00000005;        // obsolete in g2
    static const U32 VSC_SUB_WRITE_CURRENT_USER_CAPACITY = 0x00000006;
    static const U32 VSC_SUB_UPDATE_DEFECT_BLOCK_STATUS = 0x00000007;
    static const U32 VSC_SUB_UPDATE_INDENTIFY_DEVICE_INFO = 0x00000008;
    static const U32 VSC_SUB_SET_STALE_PAGES_FOR_LINKED_BLOCK = 0x00000009;

    // Control ID
    static const U32 VSC_CONTROL_DEVICE = 0x00000006;
    static const U32 VSC_SUB_RESET_DEVICE = 0x00000001;
    static const U32 VSC_SUB_ERASE_ALL_BLOCKS = 0x00000002;
    static const U32 VSC_SUB_SCAN_DEFECT_BLOCK = 0x00000003;
    static const U32 VSC_SUB_RESCAN_DEFECT_BLOCK = 0x00000004;
    static const U32 VSC_SUB_INITIALIZE_TABLES = 0x00000005;
    static const U32 VSC_SUB_WRITE_ALL_TABLES_TO_NAND = 0x00000006;
    static const U32 VSC_SUB_SET_SWL = 0x00000007;
    static const U32 VSC_SUB_SET_SWL_ERASE_COUNT_DELTA = 0x00000008;
    static const U32 VSC_SUB_SET_COMPRESS = 0X0000000B;
    static const U32 VSC_SUB_ADD_NAND_FIFO_COMMAND = 0x0000000C;
    static const U32 VSC_SUB_START_NAND_FIFO = 0x0000000E;
    static const U32 VSC_SUB_SET_ECC_ENABLE_FLAG = 0x0000000F;
    static const U32 VSC_SUB_SET_SCRAMBLER_ENABLE_FLAG = 0x00000010;
    static const U32 VSC_SUB_SET_SCRAMBLER_AFTER_ECC_ENABLE_FLAG = 0x00000011;
    static const U32 VSC_SUB_RESET_NAND_CHANNELS = 0x00000012;
    static const U32 VSC_SUB_STOP_NAND_FIFO = 0x00000013;
    static const U32 VSC_SUB_ASYN_WRITE_SEGMENTS_TABLE = 0x00000014;
    static const U32 VSC_SUB_WRITE_UNCOMPLETED_SESSION_TABLE = 0x00000015;
    static const U32 VSC_SUB_SET_DRIVE_INTO_END_OF_LIFE = 0x00000016;
    static const U32 VSC_SUB_PERFORM_WRITE_DUAL_PLANE_TEST = 0x00000017;
    static const U32 VSC_SUB_LOAD_TABLE_FROM_NAND = 0x00000018;
    static const U32 VSC_SUB_SET_INITIAL_DATE = 0x00000019;
    static const U32 VSC_SUB_CHECK_PAIR_DUAL_PLANE = 0x0000001A;
    static const U32 VSC_SUB_SET_ERASE_FOR_LINKED_BLOCK = 0x0000001C;
    static const U32 VSC_SUB_IMPOSE_DATA_COLLECTOR_RUN = 0x0000001D;

    static const U32 VSC_SUB_SET_ASSERT = 0x000000FE; // Debug only. Keep at end.

    // Get compile platform
    static const U32 VSC_GET_FIRMWARE_COMPILE_PLATFORM = 0x00000007;

    // Specific header table information of system table
    static const U32 VSC_SYSTEM_TABLE_SPECIFIC_HEADER_INFORMATION = 0x00000008;

    // Download Operation Code
    static const U32 VSC_DOWNLOAD_OPERATION_CODE = 0x00000009;
    static const U32 VSC_SUB_DOWNLOAD_AND_ACTIVATE = 0x00000001;
    static const U32 VSC_SUB_DOWNLOAD_BOOT_LOADER = 0x00000002;

    static const U32 VSC_READ      = 0x0000000A;
    static const U32 VSC_WRITE     = 0x0000000B;
    static const U32 VSC_CONTROL   = 0x0000000C;
    static const U32 VSC_NO_STATUS = 0x0000000D;

    static const U32 VSC_INVALID_CODE = 0xFFFFFFFF;

    // VSC constan defined by VSC service
    static const U32 VSC_SUB_VSC_LOADSPECIFICHEADERTABLE_12_128 = 128;
    static const U32 VSC_SUB_VSC_LOADPAGINGCYCLINGMOBILESESSION_12_82 = 82;


    // SIGNATUREs
    static const U16 SIGNATURE_SIZE_IN_BYTE = 16;
    static const U16 SEQUENCE_SIZE_IN_BYTE = 4;

    static const U8 BUFFER_SIGNATURE_1[SIGNATURE_SIZE_IN_BYTE];
    static const U8 BUFFER_SIGNATURE_2[SIGNATURE_SIZE_IN_BYTE];
    static const U8 BUFFER_SIGNATURE_3[SIGNATURE_SIZE_IN_BYTE];
    static const U8 BUFFER_SIGNATURE_4[SIGNATURE_SIZE_IN_BYTE];
    static const U8 VSC_COMMAND_1_0_SIGNATURE[SIGNATURE_SIZE_IN_BYTE];
    static const U8 VSC_COMMAND_2_0_SIGNATURE[SIGNATURE_SIZE_IN_BYTE];

    static const U16 VSC_SECTOR_SIZE_IN_BYTE = 512;
    static const U32 TIME_OUT_VALUE_IN_SECONDS = 120;

    static const U16 MAX_SECTOR_COUNT_28_BIT = 256;
    static const U32 MAX_SECTOR_COUNT_48_BIT = 65536;

public:
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 4 byte boundary */

    union uVSCSubParameter
    {
        struct
        {
            U32 TableID;
        } SystemTableInformation;

        union
        {
            U8* Address;
            struct
            {
                U32 Low;
                U32 High;
            } Address64;
        } AccessMemory;

        struct
        {
            U8 Handle;
        } AccessNANDCommand;

        struct
        {
            U8  Channel;
            U8  Chip;
            U16 Block;
            U16 Page;
        } NANDAccessCommand;

        struct
        {
            U16 SegmentID;
        } LinkedBlockEraseCountsCmd;

        struct
        {
            U8  Enable;
            U16 EraseCountDelta;
        } SWLCmd;

        struct
        {
            U8 AddressID1;
            U8 AddressIDLength1;
            U8 AddressID2;
            U8 AddressIDLength2;
            U8 NANDDeviceID0;
            U8 NANDDeviceID1;
            U8 NANDDeviceID2;
            U8 NANDDeviceID3;
            U8 NANDDeviceID4;
            U8 NANDDeviceID5;
            U8 NANDDeviceID6;
            U8 NANDDeviceID7;
            U8 NANDDeviceID8;
            U8 NANDDeviceID9;
        } NANDDeviceID;

        struct
        {
            U8 Enable;
        } CompressCmd;

        struct
        {
            U16 TotalSegments;
            U32 TableID;
        } SaveSegmentsCmd;

        struct
        {
            U32 TableID;
            U8  EnableMark;
            U8  StepJump;
        } SimWriteSessionCmd;

        struct
        {
            U32 Signature;
        } TableSignature;

        struct
        {
            U8  Enable;
        }  ECCCmd;

        struct
        {
            U8 Channel;
            U8 Chip;
            U8 Plane;
        } RetrieveFreeBlockCmd;

        struct
        {
            U8  Enable;
        } ScramblerCmd;

        struct
        {
            U8 Enable;
        } WriteProtectCmd;

        struct
        {
            U32  TableID;
            U8   LogCount;
            U8   ReadContinue;
        } ReadLog;

        struct
        {
            U16 StartLinkedBlock;
            U16 NumberOfPages;
            U8  TestType;
        } WriteDualPlaneTest;

        struct
        {
            U32 TableID;
        } LoadTableCmd;

        struct
        {
            U64 Value;
        } InitialDate;
    };

    union uVscDataStructure_1_0
    {
        U8 Data[VSC_SECTOR_SIZE_IN_BYTE];

        struct
        {
            U8  Signature[SIGNATURE_SIZE_IN_BYTE];
            U32 SequenceNumber;
        } Sequence;

        struct
        {
            U32 Status;
            U32 Command;
        } Status;

        struct
        {
            U8                  Command;
            U8                  SubCmd;
            U16                 SectorCount;
            uVSCSubParameter    SubParameter;
        } VSCCommand;

        struct
        {
            U8 Padding[VSC_SECTOR_SIZE_IN_BYTE - SIGNATURE_SIZE_IN_BYTE];
            U8 Signature[SIGNATURE_SIZE_IN_BYTE];
        } CommandVersion;
    };
#pragma pack(pop)   /* restore original alignment from stack */

    union uVscDataStructure_2_0 // fit in a sector
    {
        U8 Data[VSC_SECTOR_SIZE_IN_BYTE];

        struct
        {
            U32 Command;
            U32 SubCmd;
            U32 SectorCount;
            U32 SubParameter[(VSC_SECTOR_SIZE_IN_BYTE - (SIGNATURE_SIZE_IN_BYTE + 3*sizeof(U32))) / sizeof(U32)];
        }VSCCommand;

        struct
        {
            U8 Padding[VSC_SECTOR_SIZE_IN_BYTE - SIGNATURE_SIZE_IN_BYTE];
            U8 Signature[SIGNATURE_SIZE_IN_BYTE];
        } CommandVersion;
    };

    static const U32 m_NumberSequences_0;
    static const U32 m_NumberSequences_1;
    static const U32 m_NumberSequences_2;
    static const U32 m_NumberSequences_3;
};

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 4 byte boundary */

struct sBuffer
{
    U8* BufferPtr;
    U32 BuferSizeInSector;

    sBuffer();
    ~sBuffer();
    void reset();
    bool allocateSector(U32 sizeInSector);
    void copyTo(U8* pDestMem, const U32 &bytesCount, const U32 startIndex);
    void free();
};

struct sWorkLoadLogTableInfo
{
    U64 WorkLoadLogTableInfoId;
    U64 WorkLoadLogTableAddress;

    // This member doesn't need to be stored to database
    //[NotMapped]
    U64 CurrentWorkloadLogAddress;
    U32 WorkLoadLogTableSizeInByte;
    U32 EntrySizeInBytes;
    U32 ItemSizeInBytes;
    U32 MaxLogEntries;
    U32 LogEntriesCount;
    U32 MostRecentLogEntryIndex;
    U32 CurrentLogEntryIndex;
    U32 MaxLogItems;
    U32 CurrentLogItemIndex;
    U32 TableID;
    U64 NumberOfRandomReadCommands;
    U64 NumberOfRandomWriteCommands;
    U64 NumberOfSequentialReadCommands;
    U64 NumberOfSequentialWriteCommands;
};

// The oder of fields is not map to FW because the exist SW tool use this oder to save to file.
struct sWorkLoadLogItem
{
    U32 CommandType;
    U64 LBA;
    U32 SectorCount;
};

#pragma pack(pop)   /* restore original alignment from stack */

typedef list<sWorkLoadLogItem> tWorkLoadList;

U16 ByteToU16(const U8* buff, U32 index);
U32 ByteToU32(const U8* buff, U32 index);
U64 ByteToU64(const U8* buff, U32 index);

void consoleProgressPercent(U32 current, U32 total);

#endif // VSC_VIRTIUM_DEFINE_H

