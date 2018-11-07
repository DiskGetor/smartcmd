#include <stdarg.h>
#include <assert.h>

#include "EventLog.h"

#define PAD_RIGHT     1
#define PAD_ZERO      2
#define PRINT_BUF_LEN 12

const EventLog::sMessageParam EventLog::m_BuiltMessageTable[] =
{
    //{PatternInfo = SMI SSD v0.2, 2013/10/29}
    // ------------------------------------------------------------------------
    //  Format:
    //     Sync pattern, "Message %Param_1_Format ...... %Param_N_Format",
    //                   Param_1_size ...... Param_N_size, Action
    //
    //     %T is reserved as Timer and is not count as Param
    //     Param_x_size can be assigned for first few parameters only(default 1)
    //
    //     Action: math expression
    //             T reserved as timer variable
    //             %1 means Param1, %2 means Param2...
    // ------------------------------------------------------------------------

    //================================================================================
    // Sync pattern ( nk kn xx, ) or ( nk kn xx yy, )
    // n : A ~ F
    // K : 1 ~ 9
    //
    // pattern 3 ( nk kn xx, )
    // SN : xx ( 00 ~ 7F )
    //
    // pattern 4	( nk kn xx yy, )
    // SN : xx ( 00 ~ 7F )
    // SN : yy ( 00 ~ 7F )
    //================================================================================

    //[Message]
    // =============================================================
    // Aging Message
    // =============================================================
    // A1 1A xx, "		"
    // A1 1A xx, "		"
    //
    // A9 9A xx, "		"
    // A9 9A xx yy, "		"
    // ---------------------------------------

    // =============================================================
    // Kernel Message
    // =============================================================
    // B1 1B xx, "		"
    // B1 1B xx yy, "		"
    //
    // B9 9B xx, "		"
    // B9 9B xx yy, "		"
    // ---------------------------------------
    //{SetMsgColor = BLUE}
    {"B1 1B 00",    "KERNEL START", ""},
    {"B1 1B 10",    "KERNEL TEST1", ""},
    {"B1 1B 11",    "KERNEL TEST2", ""},
    {"B1 1B 20",    "MAX LBA=%08X", "4"},
    {"B1 1B 30 01", "Mark1 Error Tag , Tag=%d", "1"},
    {"B1 1B 30 02", "Release Tag=%d with Mark Error Type", "1"},
    {"B1 1B 31 01", "Error Handle Cache Start, System Time : %08X", "4"},
    {"B1 1B 31 02", "Error Handle Cache End,   System Time : %08X", "4"},
    {"B1 1B 40",    "KERNEL TEST3", ""},

    //{SetMsgColor = RED}
    {"B2 2B 00",    "KERNEL TEST5", ""},
    {"B2 2B 01",    "ERROR LBA=%08X", "4"},
    {"B2 2B 10 02", "Mark2 Error Tag , Tag=%d", "1"},
    {"B2 2B 10 03", "Mark3 Error Tag , Tag=%d", "1"},
    {"B2 2B 11 00", "Tag=%d with Mark4 Error Type", "1"},
    {"B2 2B 11 01", "Tag=%d with Mark5 Error Type", "1"},
    {"B2 2B 60",    "KERNEL TEST6", ""},
    {"B2 2B 70",    "KERNEL TEST7", ""},
    //{ResetMsgColor}

    // =============================================================
    // SATA Message
    // =============================================================
    {"82 00 24", "ATA_READ_SECTORS_EXT           24 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 25", "ATA_READ DMA EXT               25 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 27", "ATA_READ_MAX_ADDRESS_EXT       27 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 2F", "READ LOG EXT                   2F Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 34", "ATA_WRITE_SECTORS_EXT          34 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 35", "ATA_WRITE DMA EXT              35 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 37", "ATA_SET_MAX_ADDRESS_EXT        37 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 3D", "ATA_DMA_WRITE_FUA              3D Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 3F", "WRITE LOG EXT                  3F Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 40", "ATA_READ VERIFY SECTORS        40 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 47", "READ LOG DMA EXT               47 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 57", "WRITE LOG DMA EXT              57 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 B0", "ATA_SMART                      B0 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 C6", "ATA_SET_MULTIPLE_MODE          C6 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 C8", "ATA_Read_DMA                   C8 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 CA", "ATA_WRITE_DMA                  CA Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 CE", "ATA_WRITE_MULTI_FUA            CE Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E0", "ATA_STANDBY IMMEDIATE          E0 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E1", "ATA_IDLE IMMEDIATE             E1 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E2", "ATA_STANDBY                    E2 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E3", "ATA_IDLE                       E3 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E4", "ATA_READ_BUFFER                E4 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E5", "ATA_CHECK_POWER_MODE           E5 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E6", "ATA_SET_SLEEP_MODE             E6 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 E7", "ATA_FLUSH_CACHE                E7 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 EA", "FLUSH CACHE EXT                EA Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 EC", "ATA_IDENTIFY                   EC Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 EF", "ATA_SET_FEATURES               EF Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F1", "ATA_SECURITY_SET_PASSWORD      F1 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F2", "ATA_SECURITY_UNLOCK            F2 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F3", "ATA_SECURITY_ERASE PRE         F3 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F4", "ATA_SECURITY_ERASE UNIT        F4 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F5", "ATA_SECURITY_FREEZE_LOCK       F5 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F6", "ATA_SECURITY_DISABLE_PASSWORD  F6 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F8", "ATA_READ_MAX_ADDRESS           F8 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
    {"82 00 F9", "ATA_SET_MAX_ADDRESS            F9 Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},
  //{"82 00 FA",     "ATA_VUCmd                  FA Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1"},

    {"82 00 FA 10", "ATA_VUCmd   VuNandCommunication    SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 20", "ATA_VUCmd   VuGetWProStatus        SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 21", "ATA_VUCmd   VuGetFWRevision        SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 22", "ATA_VUCmd   VuGetSATACmdLog        SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 23", "ATA_VUCmd   VuReadManufactureData  SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 24", "ATA_VUCmd   VuGetErrorLogCount     SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 25", "ATA_VUCmd   VuReadProgramCount     SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 26", "ATA_VUCmd   VuReadUniqueID         SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 29", "ATA_VUCmd   VuGetPhysicalEraseCnt  SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 2A", "ATA_VUCmd   VuGetErrorLog          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 30", "ATA_VUCmd   VuWrManufactureData    SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 31", "ATA_VUCmd   VuSetPhysicalEraseCnt  SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 32", "ATA_VUCmd   VuClrSmartLogs         SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 33", "ATA_VUCmd   VuEnDisBGTask          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 34", "ATA_VUCmd   VuTranLog2PhyAddr      SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 35", "ATA_VUCmd   VuTranPhy2LogAddr      SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 55", "ATA_VUCmd   VuUnlockVU             SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 60", "ATA_VUCmd   VuForceWProMode        SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 61", "ATA_VUCmd   VuClrWProMode          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 62", "ATA_VUCmd   VuEraseAllFlashCmd     SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 63", "ATA_VUCmd   VuLowLevFormat         SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 64", "ATA_VUCmd   VuMarkbadBlock         SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 65", "ATA_VUCmd   VuClrErrorLog          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 66", "ATA_VUCmd   VuHighLevFormat        SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 67", "ATA_VUCmd   VuEraseDirect          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 68", "ATA_VUCmd   VuEnDisFwImageCheck    SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 69", "ATA_VUCmd   VuEraseFwBlks          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 70", "ATA_VUCmd   VuReadDirect           SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA 80", "ATA_VUCmd   VuWriteDirect          SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA AA", "ATA_VUCmd   VulockVU               SecCnt:%02X, L:%02X", "1 1"},
    {"82 00 FA F0", "ATA_VUCmd   VuSaveLog              SecCnt:%02X, L:%02X", "1 1"},
    {"82 00",       "ATA_Command %02X Feature:%02X, SecCnt:%02X, L:%02X", "1 1 1 1"},

    //{SetMsgColor = RED}
    {"82 04 00", "ATA_SRST       %02X", "1"},
    {"82 04 01", "HW_SRST        %02X", "1"},
    {"82 04 02", "SATA_COMRESET  %02X", "1"},
    {"82 04 03", "SATA_INFS      	R8_AtaCmd:%02X       Still Have RW Command:%02X",     "1 2"},
    {"82 04 04", "SATA_FatalError       R8_AtaCmd:%02X       Still Have RW Command:%02X", "1 2"},
    {"82 04 05", "SATA_LSCS     	 R8_AtaCmd:%02X     g32PSSTSR:%02X", "1 4"},
    {"82 04 06", "SATA_Link_Up     Speed:Gen%02X", "1"},
    {"82 04 07", "SATA_Link_lose  ", ""},
  //{"82 04 08", "SATA_Link_lose  ", ""},
    {"82 04 09", "SATA_UnknownFIS ", ""},

  //{"82 01 00", "Complete %02X", "1"}, //(?)TBD
  //{"82 02 00", "ReturnMain ==> %02X", "1"}, //(?)TBD
    {"82 03",    "Power ON", ""},

    {"82 05", "SATA_TX_Error", ""},
    {"82 06", "SATA_RX_Error", ""},

    {"82 07 00 01", "C_Err_DummyCmd",         ""},
    {"82 07 00 02", "C_Err_NonNCQOutOfRange", ""},
    {"82 07 00 04", "C_Err_NCQTageConflict",  ""},
    {"82 07 00 08", "C_Err_NCQOutOfRange",    ""},
    {"82 07 00 10", "C_Err_InsertNonNCQ",     ""},
    {"82 07 00 20", "C_Err_InsertNonRW",      ""},
    {"82 07 00 40", "C_Err_SecurityLockRW",   ""},
    {"82 07 00 80", "C_Err_WaitingReadLog",   ""},
    {"82 07 01 00", "C_Err_UNC",              ""},
    {"82 07 02 00", "C_Err_PIOMultiSecCnt",   ""},
    {"82 07",       "Command Error:%02X ",    "2"},

    //{SetMsgColor = Green}
  //{"82 08", "Lenovo SMART Fail RuntimeBad:%04d OrgBad:%04d PureSpare:%04d", "2 2 2"}, //(?)TBD
  //{"82 09", "SMART Fail TotalSpare:%04d less tnan MinSprBlock:%04d", "2 2"}, //(?)TBD
    {"82 10", "Firmware Download initiated, u8SubCmd = 0x%02X", "2"},
    {"82 11", "Firmware Download complete",    ""},
    {"82 12", "Write Cache has been enabled",  ""},
    {"82 13", "Write Cache has been disabled", ""},
    {"82 14", "Drive Sanitize initiated AtaFeature=0x%02X", "1"},
    {"82 15", "Drive Sanitize Complete", ""},
    //{ResetMsgColor}

    // =============================================================
    // FTL Message
    // =============================================================
    // D1 1D xx, "		"
    // D1 1D xx yy, "		"
    //
    // D9 9D xx, "		"
    // D9 9D xx yy, "		"
    //{SetMsgColor = PURPLE}
    //{SetMsgColor = NAVY}
    //{SetMsgColor = MAROON}
    //{SetMsgColor = BLUE}
    //{SetMsgColor = RED}
    // ---------------------------------------
    //{SetMsgColor = BLUE}
    {"41 01", "      ReadLinkTable() Start", ""},
  //{"41 02", "      ReadLinkTable() End. gInSuspendBackUp=0x%02X, glwbsPwrFailStatus=0x%02X", "2 4"},
    {"41 03", "      ReadIndexBlockInfo() Start", ""},
    {"41 04", "      ReadIndexBlockInfo() End",   ""},
    {"41 05", "      BuildBlockLinkInfo() Start", ""},
    {"41 06", "      BuildBlockLinkInfo() End gFastBoot =0x%02X", "2"},
    {"41 07", "      LoadMapInfo() Start", ""},
    {"41 08", "      LoadMapInfo() End",   ""},
    {"41 09", "      ScanFBlock() Start",  ""},
    {"41 0A", "      ScanFBlock() End",    ""},
    {"41 0B", "      JudgeActiveBlock() Start", ""},
    {"41 0C", "      JudgeActiveBlock() End. ActiveFBlock[0]=0x%02X", "2"},
    {"41 0D", "      ChkUnfullDataBlock() Start", ""},
    {"41 0E", "      ChkUnfullDataBlock() End",   ""},
    //{ResetMsgColor}

    //{SetMsgColor = PURPLE}
    {"41 0F", "      ChkDataBlock() Start. TotalDataBlock=0x%02X, FailReturn=0x%02X", "2 2"},
    {"41 10", "      ChkDataBlock() End", ""},
    {"41 11", "      RebuildAllMap() Start. PartialReBuild=0x%02X", "2"},
    {"41 12", "      RebuildAllMap() End", ""},
    {"41 13", "      BuildH2FTable() Start. ZoneIdx=0x%02X", "2"},
    {"41 14", "      BuildH2FTable() End", ""},
    {"41 15", "      SaveMapGroup() Start. SaveMapID=0x%02X", "2"},
    {"41 16", "      SaveMapGroup() End", ""},
    {"41 17", "      SaveMapToFlash() Start. ZoneIdx=0x%02X", "2"},
    {"41 18", "      SaveMapToFlash() End", ""},
    {"41 19", "      SaveG2FMap() Start", ""},
    {"41 1A", "      SaveG2FMap() End", ""},
    {"41 1B", "      SaveIndexInfo() Start", ""},
    {"41 1C", "      SaveIndexInfo() End", ""},
    {"41 1D", "      SaveBlockLinkTable() Start. SaveLinkID=0x%02X", "2"},
    {"41 1E", "      SaveBlockLinkTable() End", ""},
    {"41 1F", "      JudgeActiveBlock() ===> ExtraActiveBlock Exist", ""},
    {"41 20", "      JudgeActiveBlock() ===> ActiveBlock Exist", ""},
    {"41 21", "      PickCorrectWpro() Start", ""},
    {"41 22", "      PickCorrectWpro() End. WPROBlock=0x%02X", "2"},
    {"41 23", "      ChkWproBlock() ===> WPROBlocks exist", ""},
    {"41 24", "      ChkEraseEmptyMapBlock() Start", ""},
    {"41 25", "      ChkEraseEmptyMapBlock() End", ""},
    {"41 26", "      GenNextMapFBlock() Start. ZoneIdx=0x%02X", "2"},
    {"41 27", "      GenNextMapFBlock() End. NewMapFBlock=0x%02X", "2"},
    {"41 28", "      SaveValidFPageCnt() Start", ""},
    {"41 29", "      SaveValidFPageCnt() End", ""},
    {"41 2A", "      ChkFastBootInfo().gFastBoot=0x%02X", "2"},
    //{ResetMsgColor}

    //{SetMsgColor = RED}
    {"41 2B", "      ChkFastBootInfo().u16LastPage=0x%02X,g16LastMapSavedActivePagePtr=0x%02X mismatch,Check Result NG", "2 2"},
    //{ResetMsgColor}

    //{SetMsgColor = GREEN}
    {"41 2C", "      BuildDevH2FTable().g32LBA=0x%08X,g32HostXfrCnt=0x%08X",      "4 4"},
    {"41 2D", "      BuildDevH2FTable().u32GroupIdx1=0x%08X,u32GroupIdx2=0x%08X", "4 4"},
    {"41 2E", "      BuildDevH2FTable().u8ZoneIdx1=0x%02X,u8ZoneIdx2=0x%02X",     "2 2"},
  //{"41 2F", "      ChkRebuildMap() Get Last zone of read CMD", ""}, //(?)TBD
  //{"41 30", "      BuildDevH2FTable() Move H2F Table u32TargetGHP=0x%08X, u32GroupFPage=0x%08X(1Page=16K,Two plane32K should < 16)", "4 4"}, //(?)TBD
    //{ResetMsgColor}

    {"41 31", "      ChkBlockRestECCFail() ===> BackupDRAM_0", ""},
    {"41 32", "      ChkBlockRestECCFail() ===> BackupDRAM_1", ""},
    {"41 33", "      ReadIndexBlockInfo() ===> Need to change IndexBlock", ""},
    {"41 34", "      BuildBlockLinkInfo() ===> Need to change LinkBlock",  ""},
    {"41 35", "      HandleErrBlock() ===> FindSerial", ""},
    {"41 36", "      LoadMapInfo() ===> Need to change LinkBlock", ""},
    {"41 37", "      MapInfoBlock: LastMapSavedTailFBlock=0x%02X, LastMapSavedActiveFBlock=0x%02X, g16LastMapSavedActivePagePtr=0x%02X", "2 2 2"},
    {"41 38", "      DataBlockCnt=0x%02X", "2"},
    {"41 39", "      ActiveFBlock[gActMode]=0x%02X, ExtraActiveFBlock[gActMode]=0x%02X", "2 2"},
    {"41 3A", "      ActiveFBlock[0]=0x%02X,ActiveFBlock[1]=0x%02X", "2 2"},
  //{"41 3B", "      TotalDataBlock=0x%02X",2"}, // (?)TBD
    {"41 3C", "      ExtraActiveBlock[0]=0x%02X,ExtraActiveBlock[1]=0x%02X", "2 2"},
  //{"41 3D", "      ActiveBlock=0x%02X", "2"}, //(?)TBD
    {"41 3E", "      ReadIndexBlockInfo() ===> Select IndexBlock", ""},
    {"41 3F", "      ReadIndexBlockInfo() ===> IndexBlock Exist",  ""},
    {"41 40", "      WPROBlock=0x%02X", "2"},
    {"41 41", "      WPROBlock Exist",  ""},
    {"41 42", "      LinkBlock Exist",  ""},
    {"41 43", "      ChkEraseUnitState() ===> LoadEC_0", ""},
  //{"41 44", "      ChkEraseUnitState() ===> LoadEC_1", ""}, //(?)TBD
    {"41 45", "      SaveIndexInfo() ===> Change IndexBlock", ""},
    {"41 46", "      HeadFBlock=0x%02X, TailFBlock=0x%02X",   "2 2"},
  //{"41 47", "      FlashErase()     u16FBlock=0x%02X",      "2"}, //(?)TBD
    {"41 48", "      ReBuildActF2HMap() Start u16ActiveBlk=0x%02X,u8BackupDRAM=0x%02X", "2 2"},

    //{SetMsgColor = RED}
    {"41 49", "      ChkF2HAndH2F()   u16FBlock=0x%02X, g32arValidFPageCnt[u16FBlock]=0x%08X", "2 4"},
    //{ResetMsgColor}

    {"41 4A", "      GenNextActiveFBlock()     g16ActiveFBlock=0x%02X,u8GenNew=0x%02X", "2 2"},
    {"41 4B", "      ResetSpareValidPageCnt()     Start", ""},
    {"41 4C", "      ResetSpareValidPageCnt()     END",   ""},
    {"41 4D", "      RemoveVP0Block()     Start",         ""},
    {"41 4E", "      RemoveVP0Block()     END",           ""},
    {"41 4F", "      ChkSaveLinkAndMap()  Start",         ""},
    {"41 50", "      ChkSaveLinkAndMap()  END",           ""},
    //{ResetMsgColor}

    //{SetMsgColor = RED}
    {"41 51", "      ECCFailHandler()  Start",  ""},
    {"41 52", "      ECCFailHandler()  END",    ""},
  //{"41 53", "      ECCFailHandler2()  Start", ""}, //(?)TBD
    {"41 54", "      ECCFailHandler2()  END",   ""},

    //{ResetMsgColor}
    {"41 55", "      EraseAllEmptyBlock()   gEccFailBlockCnt=0x%02X,g16EmptyBlockCnt=0x%02X", "2 2"},
    {"41 56", "      GenNextTailFBlock()", ""},
    {"41 57", "      SaveEOBInfo() g16TailFBlock=0x%02X", "2"},

    //{SetMsgColor = RED}
    {"41 58", "      BuildDevH2FTable().Build Group of H2F Fail", ""},

    //{ResetMsgColor}
    {"41 59", "      IndexBlockInfo: IndexBlock=0x%02X, LinkBlock=0x%02X, WPROBlock=0x%02X, MapInfoFBlock=0x%02X", "2 2 2 2"},
  //{"41 5A", "      LinkBlockInfo: HeadFBlock=0x%02X, TailFBlock=0x%02X, TotalDataBlock=0x%02X", "2 2 2"}, //(?)TBD

    //{SetMsgColor = RED}
    {"41 5B", "      HandleErrBlock() Erase GC Block, u16FBlock=0x%02X,Push Spare Block.", "2"},
    {"41 5C", "      HandleErrBlock() InsertUnfullDataBlock u16FBlock=0x%02X", "2"},
    {"41 5D", "      SetECCFailInfo() u16ECCFailBlock=0x%02X, u8ECCFailSrc=0x%02X, u8MaxErrCnt=0x%02X", "2 2 2"},
    {"41 5E", "      u16ECCFailBlock need to record to ECC fail info table.", ""},
    {"41 5F", "      u16ECCFailBlock already exist ECC fail info table.",     ""},
    {"41 60", "      gp_EccFailInfo->g32ECCFailFlashStatus[gIntlvOddA])=0x%08X,g16ECCFailBlock=0x%02X,g16ECCFail1stPage=0x%02X", "4 2 2"},
    {"41 61", "      gECCFailPageCnt=0x%02X,gECCFailSrc=0x%02X,g16ECCFailDesBlock=0x%02X,gECCErrCnt=0x%02X",     "2 2 2 2"},
  //{"41 62", "      g16ECCFailDesBlock=0x%02X,g16ECCFailDesPage=0x%02X,gECCFailDesCE=0x%02X,gECCErrCnt=0x%02X", "2 2 2 2"}, (?)TBD
    //{ResetMsgColor}

  //{SetMsgColor = GREEN}
  //{"41 63", "      ChkDataBlock()  u16NowFBlock=0x%02X(==0x00 is POR) update to H2F,g16TailFBlock=0x%02X,g32TailFBlockSerial=0x%08X.", "2 2 4"}, //(?)TBD
  //{"41 64", "      LoadEOBInfo()  u16FBlock=0x%02X, u8TableSel=0x%02X, u8InfoPageCnt=0x%02X, u16InfoStartPage=0x%02X.", "2 2 2 2"}, //(?)TBD

  //{SetMsgColor = RED}
  //{"41 65", "      BuildF2HMapInfo()  u16FBlock=0x%02X, u8CleanF2H=0x%02X, u8ForActBlock=0x%02X, u16DataPagePerBlock=0x%02X.", "2 2 2 2"}, //(?)TBD

  //{ResetMsgColor}
  //{"41 66", "      ChkF2HAndH2F()    u16ActiveBlk=0x%02X,g16arValidFPageCnt[u16FBlock]=0x%02X", "2 2"}, //(?)TBD
  //{"41 67", "      UpdateH2FMap_PowerOn()     u16FBlock=0x%02X,u32DesUnitPage=0x%08X,u8ForAct=0x%02X", "2 4 2"}, //(?)TBD
  //{"41 68", "      RestoreEraseCnt() Load EC Table u16FBlock=0x%02X,g32GlobalEraseCntOffset=0x%08X,gbPFEOBEraseCntFail=0x%02X", "2 4 2"}, //(?)TBD
  //{"41 69", "      ReschedulingSparePool(),Push spare block to High EC or Low EC Pool depend on Erase Count,u16SpareCnt=0x%02X,g16arSpareCnt[0]=0x%02X,g16arSpareCnt[1]=0x%02X,", "2 2 2"}, //(?)TBD

    //{SetMsgColor = GREEN}
  //{"41 6A", "      LoadMapInfo(),g16arMapFBlock[0]=0x%02X,g16arMapFBlock[1]=0x%02X,g16arMapFBlock[2]=0x%02X,,g16arMapFBlock[3]=0x%02X", "2 2 2 2"}, //(?)TBD
  //{"41 6B", "      LoadMapInfo(),g16arMapFBlock[4]=0x%02X,g16arMapFBlock[5]=0x%02X,g16arMapFBlock[6]=0x%02X,,g16arMapFBlock[7]=0x%02X", "2 2 2 2"}, //(?)TBD
  //{"41 6C", "      LoadMapInfo(),g32LastMapSavedTailFBlockSerial=0x%08X", "4"}, //(?)TBD
    {"41 6D", "      Secure Erase initiated", ""},
    {"41 6E", "      Secure Erase Complete",  ""},
    {"41 6F", "      ChkLogBlock() start",    ""},
    {"41 70", "      ChkLogBlock() end",      ""},
    {"41 71", "      ReadLinkTable() End.  Safe shutdown: BootTime=0x%08X ms", "4"},
    {"41 72", "      ReadLinkTable() End.  Unsafe shutdown BootTime=0x%08X",   "4"},
    {"41 73", "      Read ECC Fail!!",     ""},
    {"41 74", "      Program Fail!!",      ""},
    {"41 75", "      Erase Fail!!",        ""},
    {"41 76", "      Firmware hangs up!!", ""},
    {"41 77", "			 Over temperature!! (Internal Sensor)(Threshold: 120'C)", ""},
    {"41 78", "			 Under temperature!!(Internal Sensor)(Threshold: 0'C)",   ""},
    {"41 79", "			 Over temperature!! (External Sensor)(Threshold: 70'C)",  ""},
    {"41 7A", "			 Under temperature!! (External Sensor)(Threshold: 0'C)",  ""},
    {"41 7B", "      MarkBadBlock() Start. ErrFBlock=0x%02X", "2"},
    {"41 7C", "      MarkBadBlock() End. NewFBlock=0x%02X",   "2"},
    {"41 7D", "      RwCmdQueueService()  Start",             ""},
    {"41 7E", "      RwCmdQueueService()  End",               ""},
    {"41 7F", "      MarkBadBlock by VU command. ErrFBlock=0x%02X Ch=%02X Ce=%02X Lun=%02X Plane=%02X ", "2 1 1 2 2"},
  //{"41 80", "      MarkBadBlock by VU command. ErrFBlock@CH=0x%02X, ErrFBlock@CE=0x%02X", "1 1"},
    {"41 81", "      An low level format command has been issued by VU command R8_AtaSecCnt=0x%02X", "1"},
    {"41 82", "      An low level format command has been completed by VU command", ""},
    {"41 83", "      RLC last trim recovery fail",    ""},
    {"41 84", "      RLC last trim recovery success", ""},
    {"41 85", "      RLC VPASS recovery success",     ""},
    {"41 86", "      RLC VPASS recovery fail",        ""},
    {"41 87", "      RLC ROC recovery success",       ""},
    {"41 88", "      RLC ROC  recovery fail",         ""},
    {"41 89", "      FlashRB no ready",               ""},
    {"41 8A", "      Boot time great than 5s",        ""},
    {"41 8B", "      Enter Write protect Mode",       ""},
    {"41 8C", "      MicronRDT_FailPointTag",         ""},

    //{SetMsgColor = RED}
  //{"44 00", "      MarkBadBlock() Start. ErrFBlock=0x%02X", "2"},
  //{"44 01", "      MarkBadBlock() End. NewFBlock=0x%02X",   "2"},
    {"44 02", "      Block ReCombine!", ""},
    {"44 03", "      Block MarkBad!",   ""},
    {"44 04", "      ChkBackUpPage() Error. u16BKPage=0x%02X, gMaxErrCnt=0x%02X", "2 2"},
    {"44 05", "      MapErrHandle===>Erase all map block",        ""},
    {"44 06", "      ChkSaveLinkAndMap() ===> Miss all map info", ""},
    {"44 07", "      ChkDataBlock() ===> DataEOBFail",            ""},
    {"44 08", "      ChkDataBlock() ===> BuildF2HMapInfo Fail",   ""},
    {"44 09", "      ReadIndexBlockInfo() ===> IndexEndPageFail", ""},
    {"44 0A", "      ReadIndexBlockInfo() ===> IndexRestFail",    ""},
    {"44 0B", "      BuildBlockLinkPrevTable() ===> PrevBlock MarkBad", ""},
    {"44 0C", "      BuildBlockLinkInfo() ===> Error LinkBlock",  ""},
    {"44 0D", "      BuildBlockLinkInfo() ===> LinkDataFail",     ""},
    {"44 0E", "      HandleErrBlock() ===> FindSerial",           ""},
    {"44 0F", "      ScanFBlock() ===> Block read EOB Fail. NowFBlock=0x%02X", "2"},
    {"44 10", "      ScanFBlock() ===> Is LinkBlock!. NowFBlock=0x%02X", "2"},
  //{"44 11", "      ScanFBlock() ===> Block Error!", ""}, //(?)TBD
  //{"44 12", "      ScanFBlock() ===> Handle error IndexBlock. gIndexBlockIndex=0x%02X", "1"}, //(?)TBD
    {"44 13", "      JudgeWproPage() ===> ECCFail",           ""},
    {"44 14", "      JudgeWproPage() ===> WProSprFail",       ""},
    {"44 15", "      JudgeWproPage() ===> WProRest/DataFail", ""},
  //{"44 16", "      ChkEraseUnitState() ===> WProDataFail",  ""}, //(?)TBD
    {"44 17", "      ChkF2HAndH2F() ===> F2HMismatchH2F. ActMode=0x%02X, FBlock=0x%02X", "1 2"},
    {"44 18", "      GHP=0x%02X0x%02X, F2HMap=0x%02X0x%02X", "2 2 2 2"},
    {"44 19", "      H2FPage=0x%02X",                 "2"},
    {"44 1A", "      ChkF2HAndH2F() Fail",            ""},
    {"44 1B", "      LoadMapInfo() ===> LoadMapFail", ""},
    {"44 1C", "      SaveIndexInfo() ===> ECCFail",   ""},
    {"44 1D", "      BuildDevH2FTable() G2F Error u8EccFail=0x%02X, (R8_FCTRSpr[0] &0x0F)=0x%02X, R8_FCTRSpr[8]=0x%02X, gRestG2F=0x%02X.", "2 2 2 2"},
    {"44 1E", "      gIndexBlockIndex =0x%02X", "2"},
  //{"44 1F", "      BuildDevH2FTable() MapBlock Error g16arMapFBlock[u8ZoneIdx1]=0x%02X, g16arFreePtrOfMapFBlock[u8ZoneIdx1]=0x%02X, u8ZoneIdx1=0x%02X.", "2 2 2"}, //(?)TBD
  //{"44 20", "      BuildDevH2FTable() H2F Error need to check!!", ""}, //(?)TBD
    {"44 21", "      SetErrBlock() u16FBlock = 0x%02X,ID=0x%02X,g16ErrorBlock1=0x%02X!!", "2 2 2"},
    {"44 22", "      SetPFInfo() u8PFID = 0x%02X,u16PFInfo=0x%02X,gPFInfoIdx=0x%02X!!",   "2 2 2"},
    {"44 23", "      AddPowerOnFailBlock() u16Block = 0x%02X,u8ECCFailSrc=0x%02X,u8EccErrCnt=0x%02X,gEccFailBlockCnt=0x%02X!!", "2 2 2 2"},
  //{"44 24", "      RLNK_WHILE u32Info = 0x%08X,gReadLinkDebug=0x%02X, gPowerOnHandleInt=0x%02X!!", "4 2 2"}, //(?)TBD
  //{"44 25", "      LoadEOBInfo() ECC Fail !!,u8EccFail=0x%02X, garLinkId[0]=0x%02X, g16FPage=0x%02X, u8ForceWait=0x%02X", "2 2 2 2"}, //(?)TBD
  //{"44 26", "      BuildF2HMapInfo() ECC Fail !!,u8DataEccFail=0x%02X,gbPFSavedActEccFail=0x%02X, u8DiscardPage=0x%02X(=1,lost data),g16ChkActivePagePtr=0x%02X", "2 2 2 2"}, //(?)TBD

    //{ResetMsgColor}
    // =============================================================
    // PC Message
    // =============================================================
    // E1 1E xx, "		"
    // E1 1E xx yy, "		"
    //
    // E9 9E xx, "		"
    // E9 9E xx yy, "		"
    // ---------------------------------------

    //{SetMsgColor = BLUE}
    {"11 00", "   StandByEntry()",    ""},
    {"11 01", "   StandByResume()",   ""},
    {"11 02", "   Pw1_Shutdown()",    ""},
    {"11 03", "   Pw1_Resume()",      ""},
    {"11 04", "   Pw1Pw2_Shutdown()", ""},
    {"11 05", "   Pw1Pw2_Resume()",   ""},
    {"11 06", "   SuspendMode() Start ,gInPowerMode=0x%02X,R32_SataMac[O32_PISR]=0x%02X ", "2 2"},
    {"11 07", "   DEVSLP interrupt,prepare into DEVSLP mode ", ""},

    {"12 00", "   SYS_VDT40_Powerdown()", ""},
    {"12 01", "   SYS_VDT27_Powerdown()", ""},
    {"12 02", "   SYS_VDT18_Powerdown()", ""},

    //{ResetMsgColor}

    // =============================================================
    // GC Message
    // =============================================================
    // F1 1F xx, "		"
    // F1 1F xx yy, "		"
    //
    // F9 9F xx, "		"
    // F9 9F xx yy, "		"
    // ---------------------------------------
    //{SetMsgColor = BLUE}
    {"22 01 00", "    SwapDataBlock() start, PickForceMode, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",  "1 2"},
    {"22 01 04", "    SwapDataBlock() start, PickActiveMode, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X", "1 2"},
    {"22 01 08", "    SwapDataBlock() start, PickZeroErCnt, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",  "1 2"},
    {"22 01 0C", "    SwapDataBlock() start, PickMinValid, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",   "1 2"},
    {"22 01 10", "    SwapDataBlock() start, PickHeadBlock, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",  "1 2"},
    {"22 01 14", "    SwapDataBlock() start, PickErrBlock, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",   "1 2"},
    {"22 01 18", "    SwapDataBlock() start, PickTail, gEnBGClean=0x%02X , g16TotalSpareCnt+g16EmptyBlockCnt = 0x%02X",       "1 2"},
  //{"22 02", "    SwapDataBlock() end, g16UnsavedCnt=0x%02x", "2"}, (?)TBD
  //{"22 20", "    The previous Log page offset = 0x%04X",     "2"}, (?)TBD
  //{"22 21", "    Key Parameter Sector Start", ""}, //(?)TBD
  //{"22 22", "    Key Parameter Sector End",   ""}, //(?)TBD
  //{"22 24", "    SetPartialMovePara g16PureSpareCnt = 0x%02x, g32MoveWriteRatio =0x%08x", "2 4"}, //(?)TBD
    //{ResetMsgColor}

    //{SetMsgColor = Block}
  //{"22 07", "    AssignCleanWrFifo() start", ""},
  //{"22 08", "    AssignCleanWrFifo() end",   ""},
  //{"22 0A", "    FinishPartialMove()",       ""}, //(?)TBD
    {"22 0C", "    FinishCleanCache(), g16CleanGHPCnt= 0x%02X", "2"},
    {"22 0D", "    FinishPickErrBlock()",    ""},
    {"22 0E", "    ReleaseAllCleanList()",   ""},
    {"22 0F", "    SetupProtectPage()",      ""},
    {"22 10", "    ProgFinishPage() start",  ""},
    {"22 11", "    ProgFinishPage() end",    ""},
    {"22 12", "    SwapActiveBlock() start", ""},
    {"22 13", "    SwapActiveBlock() end",   ""},
    {"22 14", "    DecAllWLCnt() start",     ""},
    {"22 15", "    DecAllWLCnt() end u8MaxEraseCnt = 0x%02X", "2"},
    {"22 16", "    WearLeveling() start ", ""},
    {"22 17 00 00", "    WearLeveling() end WearWProBlock,  WearWProBockCnt = 0x%02X, g32WearLevelingCnt=0x%08x",  "2 4"},
    {"22 17 00 01", "    WearLeveling() end WearMapBlock,   WearMapBlockCnt = 0x%02X, g32WearLevelingCnt=0x%08x",  "2 4"},
    {"22 17 00 02", "    WearLeveling() end WearLinkBlock,  WearLinkBlock = 0x%02X, g32WearLevelingCnt=0x%08x",    "2 4"},
    {"22 17 00 03", "    WearLeveling() end WearAct0Block,  WearAct0BlockCnt = 0x%02X, g32WearLevelingCnt=0x%08x", "2 4"},
    {"22 17 00 04", "    WearLeveling() end WearDataBlock,  WearDataBlock = 0x%02X, g32WearLevelingCnt=0x%08x",    "2 4"},
    {"22 17 00 05", "    WearLeveling() end WearCleanBlock, WearCleanBlock = 0x%02X, g32WearLevelingCnt=0x%08x",   "2 4"},
    {"22 17 00 06", "    WearLeveling() end WearEmptyMapBlock, WearEmptyMapBlock = 0x%02X, g32WearLevelingCnt=0x%08x", "2 4"},
    {"22 18", "    InitCandidateSetting, u8PickMode = 0x%02X ", "2"},
  //{"22 28", "    FindMinErasedCnt EC = 0x%02x",               "2"}, //(?)TBD

    //{ResetMsgColor}
    //{SetMsgColor = GREEN}
    {"22 03", "    FindNextCandidate() start u8PickMode = 0x%02X, u8First = 0x%02X",           "1 1"},
    {"22 04", "    FindNextCandidate() end u16FoundFBlock = %02X, g16arValidFPageCnt= 0x%02X", "2 2"},
  //{"22 05", "    MoveOutSwapBlock() start, u16DesBlock = 0x%02X, g16arCleanFBlock=0x%02x, g16arValidFPageCnt = 0x%02X", "2 2 2"},
  //{"22 06", "    MoveOutSwapBlock() end, gStopSwapping = 0x%02X", "2"},
    {"22 0B", "    ContinueSwap()", ""},
    {"22 50", "    SwapDataBlock, g32arValidFPageCnt[u16SrcBlock]=0",  ""},
    {"22 51", "    SwapDataBlock, g32arValidFPageCnt[u16SrcBlock]!=0", ""},
    {"22 52", "    SwapDataBlock, Generate new active block", ""},
  //{"22 53", "    SwapDataBlock, Hot block", ""}, //(?)TBD
  //{"22 54", "    SwapDataBlock, Cold block", ""}, //(?)TBD
    {"22 55", "    SwapDataBlock, No Clean Src Block", ""},
  //{"22 56", "    SwapDataBlock, Start Copy", ""}, //(?)TBD
  //{"22 19", "    PopSpareBlock,  Block Already Erased , g16ZeroSpareCnt = 0x%02x, g16TotalSpareCnt=0x%02x, g16arFBlock =0x%02x",  "2 2 2"}, //(?)TBD
  //{"22 25", "    PushSpareBlock,  Block Already Erased , g16ZeroSpareCnt = 0x%02x, g16TotalSpareCnt=0x%02x, g16arFBlock =0x%02x", "2 2 2"}, //(?)TBD
  //{"22 26", "    PrepareFastBoot,  Block Already Erased , g16ZeroSpareCnt = 0x%02x, g16TotalSpareCnt=0x%02x", "2 2"}, //(?)TBD
  //{"22 27", "    Check ErasedBlockCnt OK!!, wErasedSparCnt=0x%02x, g16ZeroSpareCnt=0x%02x, wSpareBlockCnt=0x%02x, g16TotalSpareCnt=0x%02x", "2 2 2 2"}, //(?)TBD

    //=============================================================

    //{ResetMsgColor}
    //{SetMsgColor = RED}
  //{"22 57", "    Add ValidFpagcnt on GC over limit = 0x%04x ,g16CurrentBlock = 0x%04x, g16arValidFPageCnt = 0x%04x", "2 2 2"}, //(?)TBD
  //{"22 58", "    Dec ValidFpagcnt on GC over limit = 0x%04x ,g16SrcBlock = 0x%04x, g16arValidFPageCnt = 0x%04x",     "2 2 2"}, //(?)TBD
  //{"22 59", "    Add ValidFpagcnt on AllocateWrite over limit = 0x%04x ,g16CurrentBlock = 0x%04x, g16arValidFPageCnt = 0x%04x", "2 2 2"}, //(?)TBD
  //{"22 5A", "    Dec ValidFpagcnt on AllocateWrite over limit = 0x%04x ,g16SrcBlock = 0x%04x, g16arValidFPageCnt = 0x%04x",     "2 2 2"}, //(?)TBD
  //{"22 5B", "    PopSpareBlock Error No SpareCnt , g16UnsavedCnt = 0x%02x, g16EmptyBlockCnt= 0x%02x", "2 2"}, //(?)TBD
  //{"22 5C", "    This is not a Error Log Page¡Aplease enter check OFFSET~", ""}, //(?)TBD
  //{"22 3F", "    Cannot find previous Log page", ""}, //(?)TBD
  //{"22 23", "    Load WPro Log Page Error.", ""}, //(?)TBD

    {"FD DF 00 00", "WHILE Error ID = %d", "4"},
    {"FD DF 00 01", "WHILE Error ID = %d Para0 = 0x%04X", "4 2"},
    {"FD DF 00 02", "WHILE Error ID = %d Para0 = 0x%04X Para1 = 0x%04X", "4 2 2"},
    {"FD DF 00 03", "WHILE Error ID = %d Para0 = 0x%04X Para2 = 0x%04X Para2 = 0x%04X", "4 2 2 2"},

    {"FD DF 00 04 00 A1", "Flash Controller Err",""},
    {"FD DF 00 04 00 A2", "BOP Controller Err",  ""},
    {"FD DF 00 04 00 A3", "UART Err",            ""},
    {"FD DF 00 04 00 A4", "CPU Other Errs",      ""},
    {"FD DF 00 04 00 A5", "Instruction Err",     ""},
    {"FD DF 00 04 00 A6", "Memory Err",          ""},

    //{SetMsgColor = BLUE}
    {"FC CF", " Max EC = %d; Min EC = %d; Avg EC = %d; Blk count = %04d.", "4 4 4 2"},

    //{ResetMsgColor}
    //{SetMsgColor = RED}
    //{ResetMsgColor}

    {STRING_END, STRING_END, STRING_END},
};


EventLog::EventLog()
{
    m_NumberOfBuiltMessage = getTotalBuiltMessage();
}

U16 EventLog::getTotalBuiltMessage()
{
    U16 count = 0;

    while(0 != m_BuiltMessageTable[count].Parameter.compare(STRING_END)) ++ count;

    return count;
}

bool EventLog::syncPattern(U8 *pBuffer, U16 &syncPatternIndex, U8 &syncLen, vector<S64> &argsList)
{
    argsList.clear();
    syncLen          = 0;

    stringstream pattern;
    pattern << setfill('0') << setw(2) << hex << uppercase << (U16)pBuffer[ syncLen ];
    ++ syncLen;

    bool syncFlag = false;
    U16 firstSync = 0;
    U16 lastSync  = m_NumberOfBuiltMessage;

    syncPatternIndex = m_NumberOfBuiltMessage;

    do
    {
        while(firstSync < lastSync)
        {
            int cmpResult = m_BuiltMessageTable[ firstSync ].SyncPattern.compare(0, pattern.str().length(), pattern.str());

            if(0 == cmpResult) break;

            ++ firstSync;
        }

        if(lastSync == firstSync) break;

        lastSync = firstSync;

        while(lastSync < m_NumberOfBuiltMessage)
        {
            int cmpResult = m_BuiltMessageTable[ lastSync ].SyncPattern.compare(0, pattern.str().length(), pattern.str());

            if(0 != cmpResult) break;

            if(pattern.str().length() == m_BuiltMessageTable[ lastSync ].SyncPattern.length())
            {
                syncPatternIndex = lastSync;
                syncFlag = true;
            }

            ++ lastSync;
        }

        if(firstSync == syncPatternIndex)
        {
            break;
        }

        pattern << " " << setfill('0') << setw(2) << hex << uppercase << (U16)pBuffer[ syncLen ];
        ++ syncLen;

    } while(firstSync < m_NumberOfBuiltMessage);

    if(false == syncFlag)
    {
        syncLen          = 0;
        syncPatternIndex = 0;
        return false;
    }

    // parse parameter
    if(0 != m_BuiltMessageTable[syncPatternIndex].Parameter.length())
    {
        size_t startPos = 0;
        size_t nextPos  = 0;

        do
        {
            nextPos = m_BuiltMessageTable[syncPatternIndex].Parameter.find(' ', startPos);

            string paramStr;
            if(string::npos != nextPos)
            {
                paramStr = m_BuiltMessageTable[syncPatternIndex].Parameter.substr(startPos, nextPos - startPos);
                startPos = nextPos + 1;
            }
            else
            {
                nextPos  = m_BuiltMessageTable[syncPatternIndex].Parameter.length();
                paramStr = m_BuiltMessageTable[syncPatternIndex].Parameter.substr(startPos, nextPos - startPos);
            }

            if(paramStr.length() != 0)
            {
                U8 paramLen = atoi(paramStr.c_str());
                S64 paramValue = 0;

                if(4 == paramLen)
                {
                    paramValue = pBuffer[syncLen + 0];
                    paramValue <<= 8;

                    paramValue += pBuffer[syncLen + 1];
                    paramValue += pBuffer[syncLen + 2];
                    paramValue <<= 8;

                    paramValue += pBuffer[syncLen + 3];
                }
                else
                {
                    // big edian or little edian
                    for(U8 nParam = 0; nParam < paramLen; ++ nParam)
                    {
                        paramValue <<= 8;
                        paramValue += pBuffer[syncLen];
                        ++ syncLen;
                    }
                }

                argsList.push_back(paramValue);
            }
            else
            {
                cout << endl << "Error parsing parameter: empty number string at built message table index : " << syncPatternIndex << endl;
                assert(0);
            }
        }while(nextPos < m_BuiltMessageTable[syncPatternIndex].Parameter.length());
    }

    return true;
}

bool EventLog::printEventLog(string &eventLogString, U8* pLogBuffer, U8 &logLen)
{
    bool status;
    vector<S64> args;
    U8 syncLen;
    U16 syncPatternIndex;

    logLen = 0;

    status = syncPattern(pLogBuffer, syncPatternIndex, syncLen, args);

    if(false == status)
    {
        return false;
    }

    print(eventLogString, m_BuiltMessageTable[syncPatternIndex].Message.c_str(), args );

    logLen = syncLen;
    return true;
}

int EventLog::print(string &outputStr, const char *format, vector<S64> args )
{
    register int width, pad;
    register int pc = 0;
    int argsIndex = 0;

    for (; *format != 0; ++ format)
    {
        if (*format == '%')
        {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-')
            {
                ++ format;
                pad = PAD_RIGHT;
            }

            while (*format == '0')
            {
                ++ format;
                pad |= PAD_ZERO;
            }

            for ( ; *format >= '0' && *format <= '9'; ++ format)
            {
                width *= 10;
                width += *format - '0';
            }

            // unsupport for now
            if( *format == 's' )
            {
                //register char *s = (char *)va_arg( args, int );
                //pc += printString (outputStr, s ? s: "(null)", width, pad);
                continue;
            }

            if( *format == 'd' )
            {
                pc += printInterger (outputStr, args[argsIndex ++], 10, 1, width, pad, 'a');
                continue;
            }

            if( *format == 'x' )
            {
                pc += printInterger (outputStr, args[argsIndex ++], 16, 0, width, pad, 'a');
                continue;
            }

            if( *format == 'X' )
            {
                pc += printInterger (outputStr, args[argsIndex ++], 16, 0, width, pad, 'A');
                continue;
            }

            if( *format == 'u' )
            {
                pc += printInterger (outputStr, args[argsIndex ++], 10, 0, width, pad, 'a');
                continue;
            }

            // unsupport for now
            if( *format == 'c' )
            {
                /* char are converted to int then pushed on the stack */
                //scr[0] = (char)va_arg( args, int );
                //scr[1] = '\0';
                //pc += printString (outputStr, scr, width, pad);
                continue;
            }
        }
        else
        {
        out:
            printChar (outputStr, *format);
            ++pc;
        }
    }

    return pc;
}

int EventLog::printInterger(string &outputStr, S64 i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned int u = i;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return printString (outputStr, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0)
    {
        neg = 1;
        u   = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u)
    {
        t = u % b;

        if( t >= 10 )
            t += letbase - '0' - 10;

        *--s = t + '0';
        u /= b;
    }

    if (neg)
    {
        if( width && (pad & PAD_ZERO) )
        {
            printChar (outputStr, '-');
            ++pc;
            --width;
        }
        else
        {
            *--s = '-';
        }
    }

    return pc + printString (outputStr, s, width, pad);
}

int EventLog::printString(string &outputStr, const char *string, int width, int pad)
{
    int pc = 0;
    int padchar = ' ';

    if (width > 0)
    {
        int len = 0;
        const char *ptr;

        for (ptr = string; *ptr; ++ ptr) ++ len;

        if (len >= width) width = 0;
        else width -= len;

        if (pad & PAD_ZERO) padchar = '0';
    }

    if (!(pad & PAD_RIGHT))
    {
        for ( ; width > 0; -- width)
        {
            printChar (outputStr, padchar);
            ++ pc;
        }
    }

    for ( ; *string ; ++ string)
    {
        printChar (outputStr, *string);
        ++ pc;
    }

    for ( ; width > 0; -- width)
    {
        printChar (outputStr, padchar);
        ++ pc;
    }

    return pc;
}

void EventLog::printChar(string &outputStr, int c)
{
    outputStr += (char)c;
}
