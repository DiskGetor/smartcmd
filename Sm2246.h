#ifndef SM2246_H
#define SM2246_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"
#include "utility/device/Device.h"
#include "utility/device/DeviceUtil.h"

class Sm2246
{
public:
    static const U8  LOG_BUFFER_SIZE_IN_SECTOR = 64;
    static const U32 LOG_BUFFER_SIZE_IN_BYTE   = LOG_BUFFER_SIZE_IN_SECTOR * 512;
    static const U16 ENVENT_LOG_BUFFER_SIZE    = 0x2000;

    static const U16 ENVENT_LOG_OFFSET   = 0;
    static const U16 INTERNAL_LOG_OFFSET = 0x2000;

    static const U64 LOG_STATUS_VIRTIUM = 0x474F4C4E49425456ULL; //"VTBINLOG"
    static const U64 LOG_STATUS_SMI     = 0x474F4C4E49424D53ULL; //"SMBINLOG"
    static const U64 LOG_STATUS_INVALID = 0xFFFFFFFFFFFFFFFFULL;

public:
    enum eCommandStatus
    {
        CMD_STATUS_ERROR_NONE = 0,
        CMD_STATUS_ERROR_INVALID_DEVICE,
        CMD_STATUS_ERROR_VSC_SEQUENCE,
        CMD_STATUS_ERROR_VSC_MODE,
        CMD_STATUS_ERROR_VSC_COMMAND,
        CMD_STATUS_ERROR_OPEN_FILE,
    };

    enum eSaveLogMode
    {
        SAVE_LOG_STATISTIC = 0,
        SAVE_LOG_VERBOSE,
        SAVE_LOG_BINARY,
        SAVE_LOG_EVENT
    };

public:
    union uCardMode0
    {
        U8 all;
        struct
        {
            U8 InternalInterleaveMode : 1;
            U8 ExternalInterleaveMode : 1;
            U8 IntleaveMode : 1;
            U8 Uknown_3     : 1;
            U8 Uknown_4     : 1;
            U8 WholeDriveStrongPage : 1;
            U8 gbForceSLCMode : 1;
            U8 Uknown_7     : 1;
        }bits;
    };

    union uCardMode1
    {
        U8 all;
        struct
        {
            U8 IntlvWay   : 3; // sub 1
            U8 TotalChNum : 3; // sub 1
            U8 Unknow_6   : 2;
        }bits;
    };

    union uCardMode2
    {
        U8 all;
        struct
        {
            U8 DramSize   : 4; // 0:16MB; 1:32MB 2:64MB; 3:128MB 4:256MB; 5:512MB; 6:1GB; 7:2GB
            U8 DramVendor : 3; // 0: Nanya 1: Winbond 2: Samsung; 3: Hynix
            U8 DramClock  : 1; // 0:Dram 330M   1:Dram 380M
        }bits;
    };

    union uFlashOpt0
    {
        U8 all;
        struct
        {
            U8 gbToshibaPlaneLoc0 : 1;
            U8 gbToshibaPlaneLoc1 : 1;
            U8 gbToshibaPlaneLoc2 : 1;
            U8 gbPlane2SeqBlock   : 1;
            U8 gbPlane2TCmd       : 1;
            U8 gbPlane2SAddr      : 1;
            U8 gbMLC              : 1;
            U8 gbSupportTSBA2HCmd : 1;
        }bits;
    };

    union uFlashOpt1
    {
        U8 all;
        struct
        {
            U8 gbPage4k	 : 1;
            U8 gbPage8k	 : 1;
            U8 gbPage16k : 1;
            U8 gbPage32k : 1;
            U8 Page128	 : 1;
            U8 Page256	 : 1;
            U8 Page192	 : 1;
            U8 Unkown    : 1;
        }bits;
    };

    union uFlashOpt2
    {
        U8 all;
        struct
        {
            U8 gbIntelFlash20nmMLC : 1;
            U8 gbIMFlash28nmMLC	   : 1;
            U8 gbHynix4kFlash	   : 1;
            U8 gbIM4kFlash		   : 1;
            U8 gbTSB24nmMLC		   : 1;
            U8 gbSamsung21nmMLC	   : 1;
            U8 gbHynix20nmMLC	   : 1;
            U8 gbSLCReliable	   : 1;
        }bits;
    };

    union uFlashOpt3
    {
        U8 all;
        struct
        {
            U8 DataEccBits : 3;
                //  1/  2/  3/  4/  5/ 6
                // 24/25/40/42/60/66"
            U8 SpareEccSetting : 2;
                // 0: 16B + 24 bit ECC
                // 1: 26B + 20 bit ECC
                // 2: 26B + 24 bit ECC"
            U8 PlaneNum : 3; // Sub -1
        }bits;
    };

    union uFlashOpt4
    {
        U8 all;
        struct
        {
            U8 gbSyncModeFlash	: 1;
            U8 gbToggleMode		: 1;
            U8 gbManualToToggle	: 1;
            U8 gbMPlaneMode		: 1;
            U8 gbEnMPlaneRead	: 1;
            U8 gbCopyBackFunc	: 1;
            U8 gbPlaneAccessCmd	: 1;
            U8 Unknown          : 1;
        }bits;
    };

    struct sInfoStartPage
    {
        U8 BlockStatusStartPage;
        U8 BadBlockBitMapStartPage;
        U8 OrphanEntryStartPage;
        U8 CombinedOrphanStartPage;
        U8 SpareGoodTableStartPage;
        U8 gMarkBadInfoStartPage;
        U8 Unknown;
        U8 gPageOfISP;
    };

    struct sNandCommandSet
    {
        U8 g2PlaneStatusCmd;
        U8 gEraseCmdNum2;
        U8 gRead2PCmdNum1;
        U8 gRead2PCmdNum2;
        U8 gRforCCmdNum1;
        U8 gRforCCmdNum2;
        U8 gCopyP0ProgCmdNum;
        U8 gCopyP1ProgCmdNum;
        U8 gCopyRandomInCmd;
        U8 gP0ProgCmdNum;
        U8 gP1ProgCmdNum;
        U8 gStatusCmd;
        U8 gIntlv2StatusCmd1;
        U8 gIntlv2StatusCmd2;
        U8 gPlaneReadQueuCmdNum;
        U8 gPlaneEraseCmd;
        U8 Unkown[16];
    };

    union uSystemOption
    {
        U8 all;
        struct
        {
            U8 FindInfoBlock0  : 1;
            U8 FindInfoBlock1  : 1;
            U8 FindISPBlock0   : 1;
            U8 FindISPBlock1   : 1;
            U8 FindIndexBlock0 : 1;
            U8 FindIndexBlock1 : 1;
            U8 WPROBlock       : 1;
            U8 RunonISP        : 1;
        }bits;
    };

    struct sNandChipID
    {
        U8 ID[6];
        U8 Unused[2];
    };

    struct sCIDTable
    {
        // Byte [0x00]
        U8 EnableSyncMode           : 1;
        U8 EnableNCQ                : 1;
        U8 EnableThermal            : 1;
        U8 EnableDelaySATASignature : 1;
        U8 EnableEarlyRetire_0x00   : 1;
        U8 ForceGen_1           : 1;
        U8 HangDRAMISR          : 1;
        U8 DebugDRAM            : 1;

        // Byte [0x01]
        U8 EnableScrb                      : 1;
        U8 FlashCacheProg              : 1;
        U8 EnableUnc                       : 1;
        U8 EnableCmdInOrder                : 1;
        U8 DIPM                        : 1;
        U8 HIPM                        : 1;
        U8 SpeedUpHDTune               : 1;
        U8 DisableSamsungAutoReadRetry : 1;

        // Byte [0x02]
        U8 EnableFCTL4setting : 1;
        U8 EnableCache2K      : 1;
        U8 EnableTogFWP       : 1;
        U8 EnableReadEcc      : 1;
        U8 CeDontCare     : 1;
        U8 IgnorProgFail  : 1;
        U8 IgnorStatus    : 1;
        U8 IgnorEraseFail : 1;

        // Byte [0x03] FCTL4 setting
        U8 twhr_ext         : 1;
        U8 fre_1_1          : 1;
        U8 B_03_02_Unuse    : 1;
        U8 fsm_write        : 1;
        U8 fsm_read         : 1;
        U8 wr_wait          : 1;
        U8 rd_wait          : 1;
        U8 pdege            : 1;

        // Byte [0x04]
        U8 SparePoolDiffth; //default = 80. Threshold = (x/100) * WearLeveling different count

        // Byte [0x05]
        U8 NowMaxChildNum  : 4;// "0: NowMaxChildNum = 5 (default) maximal = 12 (unused)"
        U8 LinkTableNumber : 2;// "0: LinkTabNum = 3, default = 3, maximal = 3(unused)"
        U8 B_05_06         : 1;
        U8 B_05_07         : 1;

        // Byte [0x06]
        U8 BackGroundCleanTH;// default = 6. Block background clean threshold: TotalFBlock >> x

        // Byte [0x07]
        U8 Enable2PlaneRead      : 1;// 1: Enable 2-Plane Read
        U8 PreTrigReadPlane1     : 1;// 1: Enable 2 plane write, unreference CID
        U8 EnableRandom1         : 1;// 1:Enable Random1 in/out, 0:Disable Random1 in/out
        U8 EnableRandom2         : 1;// 1:Enable Random2 in/out, 0:Disable Random2 in/out
        U8 EnableCacheWrite      : 1;// 1:Enable ATA write cache
        U8 EnableTempCacheWrite  : 1;// 1:Enable temp cach write
        U8 EnableFATCacheWrite   : 1;// 1:Enable cacahe write
        U8 B_07_07           : 1;

        // Byte [0x08]
        U8 MinSpareBlock; //default:8

        // Byte [0x09]
        U8 WearLevelingDifferentCount; //  default100

        // Byte [0x0A]
        U8 Swap_ISP_Block          : 1; // 1: Pop new ISP block and Info block when start up
        U8 ForceWaitBusyBeforeRead : 1; // 1: Force wait another CE busy before read in interleave mode
        U8 EnableEarlyRetire       : 1; // 1. Enable Early Retire
        U8 B_0A_03                 : 1;
        U8 B_0A_04                 : 1;
        U8 B_0A_05                 : 1;
        U8 Enable_SATA_DASP_in_SATA_Mode : 1; // 1: Enable 0: Disable
        U8 Support_CSS_Vender_Command    : 1; // 1. Enable CSS vender command

        // Byte [0x0B]
        U8 MaximunWearLevelingBlock; // num < 5

        // Byte [0x0C - 0x0F] Could be Big Endian
        U32 CustomerDiskSizeInLBA; // Default:0x0000_0000

        // Byte [0x10]
        U8 OperationSpeedSetting : 4;
            // 0x00 - CPU 30 MHz
            // 0x01 - CPU 35 MHz
            // 0x02 - CPU 40 MHz
            // 0x03 - CPU 44 MHz
            // 0x04 - CPU 49 MHz
            // 0x05 - CPU 53 MHz
            // 0x06 - CPU 56 MHz
            // 0x07 - CPU 60 MHz
            // 0x08 - CPU 64 MHz
            // 0x09 - CPU 68 MHz
            // 0x0A - CPU 71 MHz
            // 0x0B - CPU 75 MHz
            // 0x0C - CPU 79 MHz
            // 0x0D - CPU 82 MHz
            // 0x0E - CPU 85 MHz
            // 0x0F - CPU 88 MHz
        U8 freq_vs : 2;
            // 00 - normal
            // 01 - fast
            // 10 - slowest
            // 11 - slow
        U8 EnableClkDiv1 : 1;// clock division enable 1: CPU clock divide by 1  0: CPU clock divide by 2
        U8 B_10_07       : 1;

        // Byte [0x11]
        U8 MainPllFrequency;
            // 30MHz    : 0x085     31.5MHz   : 0x064     33.3MHz  : 0x043    35MHz     : 0x086
            // 37.5MHz  : 0x022     40MHz     : 0x087     41.6MHz  : 0x044    43.75MHz  : 0x066
            // 45MHz    : 0x088     50MHz     : 0x023     55MHz    : 0x08A    56.25MHz  : 0x068
            // 58.3MHz  : 0x046     60MHz     : 0x08B     62.5MHz  : 0x024    65MHz     : 0x08C
            // 66.6MHz  : 0x047     68.75MHz  : 0x06A     70MHz    : 0x08D    75MHz     : 0x025
            // 80MHz    : 0x08F     81.25MHz  : 0x06C     83.3MHz  : 0x049    85MHz     : 0x090
            // 87.5MHz  : 0x026     90MHz     : 0x091     91.6MHz  : 0x04A    93.75MHz  : 0x06E
            // 95MHz    : 0x092     100MHz    : 0x027     105MHz   : 0x094    106.25MHz : 0x070
            // 108.3MHz : 0x04C     110MHz    : 0x095f    112.5MHz : 0x028    115MHz    : 0x096
            // 116.6MHz : 0x04D     118.75MHz : 0x072     120MHz   : 0x097    125MHz    : 0x029
            // 130MHz   : 0x099     131.25MHz : 0x074     133.3MHz : 0x04F    135MHz    : 0x09A
            // 137.5MHz : 0x02A     140MHz    : 0x09B     141.6MHz : 0x050    143.75MHz : 0x076
            // 145MHz   : 0x09C     150MHz    : 0x02B
            // Default  :  125MHz  0x27(AB)/0x23  (AA)

        // Byte [0x12]
        U8 SDR_FlashPllFrequency; // Default:  80MHz  0x8F(AB)/0x87(AA)

        // Byte [0x13]
        U8 DDR_FlashPllFrequency; // Default:  100MHz   0x27(AB) / 0x23  (AA)

        // Byte [0x14]
        U8 PowerDownTimeSlot; // Set sleep time, 2ms per unit. Ex: set 0x0A, sleep time is 20ms.

        // Byte [0x15]
        U8 CustomerSettingSectorsPerTrack; // Change sector per track of CHS value (must be change with XB_OPT[0x13] simultaneously)

        // Byte [0x16]
        U8 CustomerSettingHeadsPerCylinder; // Change head per cylinder of CHS value (must be change with XB_OPT[0x15] simultaneously)

        // Byte [0x17]	PowerDown WAKEUP_CLK
        U8 wakeup_cnt_1 : 1; // "Clock out after wake up counter: Mask clock count = 2 ^ (wakeup-1)
        U8 wakeup_cnt_2 : 1; // Note : wakeup_cnt equals to zero
        U8 wakeup_cnt_3 : 1; // Unit: clock
        U8 wakeup_cnt_4 : 1; // Ex: WAKEUP_CLK_0 XB_OPT[0x17] -> 8*0, WAKEUP_CLK_1 -> 8*(2^(1-1)), WAKEUP_CLK_2 -> 8*(2^(2-1)), WAKEUP_CLK_4 -> 8*(2^(3-1)), WAKEUP_CLK_8 -> 8*(2^(4-1)), WAKEUP_CLK_16k -> 8* (2^(15-1))"
        U8 wakeup_cnt_5 : 1;
        U8 B_17_05      : 1;
        U8 B_17_06      : 1;
        U8 B_17_07      : 1;

        // Byte [0x18] Host_related_option 1
        U8 DisableUseAutoBuf : 1; // unused
        U8 IORDY_Enable      : 1; // 1: IORDY enable
        U8 SyncClkAtOriginalClearBusyMask : 1;// 0: Enable buffer address increasing asynchronize with clock source
                                              // 1: Enable buffer address increasing synchronize with clock source
        U8 IdTableSelection : 1; // 0: 440A
                                 // 1: 044A
        U8 IgnoreHrstWhenInTrueIdeMode : 1; // 0: Hrst Enable. Hrst will reset all PATA interface registers.
        U8 B_18_05      : 1;
        U8 B_18_06      : 1;
        U8 WholeWriteProtect : 1;// 1: SET_nFlashWPRO. Don't protect Data-out and EraseSector command.

        // Byte [0x19]	Host_related_option 2
        U8 UDMA_ModeSupportted : 3;
            // 000: Enable UDMA mode0
            // 001: Enable UDMA mode1
            // 010: Enable UDMA mode2
            // 011: Enable UDMA mode3
            // 100: Enable UDMA mode4
            // 101: Enable UDMA mode5
            // 110: Enable UDMA mode6
            // 111: Enable UDMA mode6
        U8 DisableUdmaMode         : 1; // 1: Disable all UDMA mode
        U8 Enable_PCMCIA_UDMA_Mode : 1;
        U8 BackToCF14SpecWithPIO_Mode_6_only      : 1;
        U8 BackToCF14SpecWithPIO_Mode_3_4_disable : 1;
        U8 BackToCF14SpecWithPIO_Mode_3_4_enable  : 1;

        // Byte [0x1A]
        U8 TerminationResistorValueInUDMA; // (PCMCIA 5V/PCMCIA 3.3V/IDE 5V/IDE 3.3V) unused

        // Byte [0x1B]
        U8 UDMA_TIMING; // /freq_vs/cf_pad_ctl/f_pad_ctl setting qualifier (0xAA) unused

        // Byte [0x1C]
        U8 UDMA_TIMING_SettingValue; // unused

        // Byte [0x1D]
        U8 EraseRetryCount : 4; // default:3
        U8 B_1D : 4;

        // Byte [0x1E]
        U8 FlashDataPinsDrivingSetting : 3; // 000:4mA 001:8mA ( 4mA/stage )
        U8 B_1E_03 : 1;
        U8 FlashControlPinsDrivingSetting_CE_WE_RE : 3; // (CE,WE,RE) 000:4mA 001:8mA ( 4mA/stage )
        U8 B_1E_07 : 1;

        // Byte [0x1F]
        U8 FlashControlPinsDrivingSetting_ALE_CLE : 3; // (ALE,CLE) 000:4mA 001:8mA ( 4mA/stage )
        U8 B_1F_03 : 1;
        U8 FlashDqsPinDrivingSetting : 3; // 000:4mA  001:8mA ( 4mA/stage )
        U8 B_1F_07 : 1;

        // Byte [0x20]
        U8 LBA_Offset;

        // Byte [0x21]	Judge Copy Back
        U8 ReadFromSource_N : 4; // (n) When executing copy back, every 2 power of n times(2^n), reads from source once.
        U8 ReadFromSource   : 1; // 1: Read from source everytime
        U8 B_21_05          : 3;

        // Byte [0x22]
        U8 EccRetryCount           : 4; // default:3
        U8 EccControlRegister_0x1E : 3; // "Maximun unmatch data in verify mode or correctable ECC criterion of auto stop pre_trig function.
                                        // Note : Don't set the value less then 1."
        U8 ReadEccCorrectable      : 1; // "1: Read ECC correctable 0: Read ECC Fail stop"

        // Byte [0x23]
        U8 PATA_Interface_UDMA_TerminationResistorControl : 1; // 1: Enable Termination Resistor  0: Disable Termination Resistor
        U8 UDMA_TerminationResistorValueSelect            : 1; // 1: Force value  0: decide by pata_drv
        U8 DSTROBE_SignalDelay : 2;
            // 00: No Delay
            // 01: more delay
            // 11: most delay"
        U8 pata_drv : 2;
            // "PATA interface output pins driving strength and termination resistor value control
            // 00: 4 mA (88 Ohm)
            // 01: 8 mA (44 Ohm)
            // 10: 12 mA (33 Ohm)
            // 11: 16 mA (22 Ohm)"
        U8 B_23_06 : 2;

        // Byte [0x24]
        U8 ApacerCommand                 : 1;
        U8 TranscendCommand              : 1;
        U8 EnableSystemBlockWearLeveling : 1; // unused
        U8 EnableSecurityFunction        : 1;
        U8 ReservedForSwWriteProtect     : 1;// 11: Enable Security & Set SecurityUnlockCount
        U8 Enable_SMART_ReadWriteLogFunction : 1; // 1: Enable S.M.A.R.T. Read/Write LOG
        U8 EnableCheckVdtBfReadWrite : 1; // 0:Enable
        U8 Enable_SMART_Function     : 1; // 1: Enable S.M.A.R.T.

        // Byte [0x25]
        U8 CommandWaitCount; // 0x00 (Based on flash clock)

        // Byte [0x26]
        U8 SATA_Tx_Attenuation : 3; //"default:0x03 0:(16/16) 1:(14/16) 2:(12/16) 3:(10/16) 4:(9/16) 5:(8/16) "
        U8 B_26_03             : 1;
        U8 ForceReportSupport48BitAddress : 1; // (device capacity larger than 8GB would Report Support 48-bit Address in default)	1: Report Support
        U8 HwWriteProtect                 : 1; // (dummywrite or Return err)	1: Dummy Write(default)  0: Return error
        U8 GpioForEraseUnit               : 1; // 1: Enable 0: Disable(default)
        U8 EnableHwWriteProtect           : 1;// (dummywrite)	1: Enable 0: Disable(default)

        // Byte [0x27]
        U8 StrongWeakPageTableSelection;
            // 0x00: General
            // 0x01: Samsung 35nm, Toshiba 43nm
            // 0x02: Micron 34nm L63A
            // 0x03: Micron 34nm L63B, L62A

        // Byte [0x28]
        U8 NormalModeCommandTypeBaseOnFlash : 4; // 0x01: Micron/Intel L74A "
        U8 NormalModeFlashIoDrivingLevel    : 4; // Flash IO driving setting in normal mode or SLC of hybrid mode or hybrid MLC mode

        // Byte [0x29]
        U8 CommandTypeBaseOnFlash : 4;
        U8 FlashIoDrivingLevel    : 4; // Flash IO driving setting in MLC of hybrid mode

        // Byte [0x2A]
        U8 EccErrorBitCountForEarlyMove; // Default:0x00
            // ECC Error Bit Count for Early Move (don't markbad) "Valid if EnEarlyRetire(Byte0.4 = 1)
            // Use default setting by ECC engine:
            // = 8, if 13bECC
            // = 18, if 24bECC
            // = 32, if 40bECC)
            // Set 0xFF to disable Early Move function"

        // Byte [0x2B]
        U8 EccErrorBitCountForEarlyRetire; // Default:0x00
            // ECC Error Bit Count for Early Retire (do markbad) "Valid if EnEarlyRetire(Byte0.4 = 1)
            // (Use default setting by ECC engine:
            // = 11, if 13bECC
            // = 22, if 24bECC
            // = 36, if 40bECC)
            // Set 0xFF to disable Early Retire function"

        // Byte [0x2C]
        U8 MinSpareBlockForSmartCommand; // default:2

        // Byte [0x2D] ChkBlockRestECCFail (after power on ) (in readlink)
        U8 ChkBlockRestECCFail_gSLCDiff : 4; // default:3
        U8 ChkBlockRestECCFail_gMLCDiff : 4; // default:8

        // Byte [0x2E] ONFI-SYNC MODE, Toggle Mode
        U8 OnfiFlashDataPinsDrivingSetting : 3; // 000:4mA  001:8mA  ( 4mA/stage )
        U8 B_2E_03 : 1;
        U8 OnfiFlashControlPinsDrivingSetting_CE_WE_RE : 3; // 000:4mA 001:8mA ( 4mA/stage )
        U8 B_2E_07 : 1;

        // Byte [0x2F]
        U8 OnfiFlashControlPinsDrivingSetting_ALE_CLE : 3; // 000:4mA 001:8mA ( 4mA/stage )
        U8 B_2F_03 : 1;
        U8 OnfiFlashDqsPinDrivingSetting : 3; // 000:4mA 001:8mA ( 4mA/stage )
        U8 B_2F_07 : 1;

        // Byte [0x30 - 0x33]
        U32 CustomerMinusSectorOffsetOfCapacity; //	Default:0x0000_0000

        // Byte [0x34 - 0x35]
        U16 HIPMIdleTH; // HIPMIdleTH * 5ms, big-endian	Default:0x0000_0000

        // Byte [0x36]
        U8 MoveWriteRatio : 7; // (clean count if host write  a block)	Default:0x02
        U8 PartialCleanFunction : 1; // Default:1 enable

        // Byte [0x37]
        U8 TotalMoveStep;// (steps for a block clean) Default:0x04

        // Byte [0x38]
        U8 RDTHour; // (unit is 2 hour) Default:0x0A (0x0A*2 = 20 hours)

        // Byte [0x39]
        U8 RWBreakIdleCount; //(unit is 0x10) Default:0x10 (0x10*0x10 = 0x100 count ~ 100us)

        // Byte [0x3A]
        U8 LTS_SetupTime; // (unit is minutes) Default:0x46  (70min)

        // Byte [0x3B]
        U8 LTS_DramTestTime; // (unit is minutes) Default:0x6E (1hr 50min)

        // Byte [0x3C]
        U8 RDTType : 3;
            //"RDT Type:
            //  0:Normal
            //  1:TSB type A    2:TSB type B   3:TSB type C  4:TSB type T"
        U8 B_3C_03 : 1;
        U8 FlashType: 1; // ABL:0  HBL:1
        U8 B_3C_05 : 3;

        // Byte [0x3D]
        U8 gRDTECCFailCntTH; // Default:1 (for TSB)

        // Byte [0x3E]
        U8 gRDTEraseFailCntTH; // Default:0 (for TSB)

        // Byte [0x3F]
        U8 gRDTProgFailCntTH; // Default:6 (for TSB)

        U8 Padding[0x70 - 0x40];

        // Byte [0x70~0x7F]
        U8 CIDIdentifyString[16]; // SM2250AB CIDPAGE
    };

    struct sParameterTable
    {
        // Start at 0x000
        U8 Signature[10];
        uCardMode0 CardMode0;
        uCardMode1 CardMode1;
        uCardMode2 CardMode2;
        U8 CardMode3;
        U8 CardMode4;
        U8 CardMode5;

        // Start at 0x010
        U8 IdVendor[8];
        U8 ChannelMap;
        U8 CeMap;
        uFlashOpt0 gbsOpt0;
        uFlashOpt1 gbsOpt1;
        uFlashOpt2 gbsOpt2;
        uFlashOpt3 gbsOpt3;
        uFlashOpt4 gbsOpt4;
        U8 gbsOpt5;

        // Start at 0x020
        U16 Good_1_Block; // block index that keeps information about bad block
        U16 Good_2_Block;
        U16 Good_3_Block; // block index that keeps information about CID
        U16 Good_4_Block;
        U16 Good_5_Block;
        U16 Good_6_Block;
        U16 Good_7_Block;
        U16 Good_8_Block;

        // Start at 0x030
        U16 g16FirstBlock;
        U16 g16FBlockPerCE;
        U16 g16OrphanEntry;
        U16 B_0x036;
        U16 B_0x038;
        U16 B_0x03A;
        U8  B_0x03C;
        U8  gDDRMode;
        U8  gInterChipNumShiftCnt;

        // Start at 0x040
        U16 g16TotalFBlock;
        U16 g16TotalHBlock;
        U16 g16OrgBadCnt;
        U16 g16PretestBadCnt;
        U16 g16ExtendBlock;
        U16 g16MaxSpareCnt;
        U32 g32TotalDatSector;

        // Start at 0x050
        U8 gBankPerCE[16];

        // Start at 0x060 and 0x070
        sNandCommandSet NandCommandSet;

        // Start at 0x080
        sInfoStartPage InfoStartPage;
        U8 OriginalFWVersion[8];

        // Start at 0x090
        U8 ChannelIODrivingSetting[16];

        // Start at 0x0A0 and 0x0E0
        U8 AvailableForOtherOption[0x0F0 - 0x0A0];

        // Start at 0x0F0
        U8 SubSignature[16];

        // Start at 0x100
        U8 StrongPageTable[512];
    };

    struct sFlashID
    {
        // Start at 0x000
        U8 FlashErrorCode;
            // 0	No Error
            // 2	Total Bank > C_MaxBank
            // 3	cannot find Parameter Page
            // 4	TotalBank = 0
            // 8	ECC on ISP Block
            // 9	Incorrect ISP Block
            // 10	Incorrect ISP Tag
        U8 PretestErrorCode;
            // 0	C_Pret_OK
            // 1	C_Pret_PerformanceBad
            // 2	C_Pret_FlashNoSync
            // 3	C_Pret_DRAMTooSmall
            // 4	C_Pret_DRAMAccessError
            // 5	C_Pret_TooMuchBad
            // 6	C_Pret_2KBFlash
            // 7	C_Pret_ECCLvlError
        U8 KB_PerPlaneF;
        U8 SectorPerPageH;
        U8 IspMode ; // 0:Rom 1:MPISP 2:ISP"
        U8 B_005;
        U8 FirstIspBlock;
        U8 SecondIspBlock;
        U8 B_008;
        uSystemOption SysOption;
        uCardMode0 CardMode0;
        uCardMode1 CardMode1;
        uCardMode2 CardMode2;
        U8 CardMode3;
        U8 CardMode4;
        U8 CardMode5;

        // Start at 0x010
        U16 TotalFBlock_Bigendian;
        U16 TotalHBlock_Bigendian;
        U32 TotalDataSector_Bigendian;
        U16 B_018;
        U16 B_01A;
        U16 B_01C;
        U16 IFSErrorCount;

        // Start at 0x020
        U8 ParamPageId[8];
        U8 ChannelMap;
        U8 CeMap;
        uFlashOpt0 gbsOpt0;
        uFlashOpt1 gbsOpt1;
        uFlashOpt2 gbsOpt2;
        uFlashOpt3 gbsOpt3;
        uFlashOpt4 gbsOpt4;
        U8 gbsOpt5;

        // Start at 0x030
        sNandChipID NandChipID[8][8] ;//  8 chip, 8 channel

        // Start at 0x230
        U16 InfoBlock[2];
        U16 IspBlock[2];
        U16 IndexBlock[2];
        U16 B_23C;
        U16 WProBlock;

        // Start at 0x240
        U16 WproPage[8];

        // Start at 0x250
        U64 HostWriteSectorCountSincePowerOn;
        U32 TotalEraseCountSincePowerOn;
        U32 SectorsPerBlock;

        // Start at 0x260
        U16 PureSpareCount;
        U16 TotalSpareCount;
        U8 B_264[12];
        U8 B_270[44];
        U8 B_29C;
        U8 LogExist;
        U16 TotalLog;
    };

    struct sL2PMapping
    {
        U64 LBA;
        U16 FBlock;
        U16 Page;
        U8  Unit;
        U8  Plane;
        U8  Channel;
        U8  Chip;
        U8  ChunkInUnit;
    };

    struct sMarkBadInfo
    {
        U16 Block;
        U16 Page;

        U8 FailReason;
        U8 EccBit;
        U8 Chip;
        U8 Channel;

        U8 Unused_0[4];
        U8 Plane;
        U8 Unused_1[1];
        U16 EraseCount;

        U8 Unused_2[2];
        U16 TimeStampInHour;
        //U8 Unused_2[12];
    };

    union uMarkBadInfo
    {
        U8 Buffer[32];
        sMarkBadInfo Info;
    };

    struct sBadBlock
    {
        U16 Block;
        U8 Channel;
        U8 Chip;
        U8 Plane;
    };

    union uSaveID
    {
        U32 all;

        struct
        {
            U32 None : 1;
            U32 DevicePowerOnSafeShutdown : 1;
            U32 DevicePowerOnUnsafeShutdown : 1;
            U32 ECCFail : 1;
            U32 ProgramFail : 1;
            U32 EraseFail : 1;
            U32 MarkBad : 1;
            U32 SataTXerror : 1;
            U32 SataRXerror : 1;
            U32 Reserved0 : 1;
            U32 SataLinkLose : 1;
            U32 IceSavedEventLog : 1;
            U32 Reserved1 : 1;
            U32 FirmwareHangsUp : 1;
            U32 EraseUnit : 1;
            U32 OverTemperatureExternalSensorThreshold_70 : 1;
            U32 UnderTemperatureExternalSensorThreshold_0 : 1;
            U32 OverTemperatureInternalSensorThreshold_120 : 1;
            U32 UnderTemperatureInternalSensorThreshold_0 : 1;
            U32 MarkBadByVUCommand : 1;
            U32 VUTage : 1;
            U32 FirmwareDownloadInitiated : 1;
            U32 FirmwareDownloadComplete : 1;
            U32 DriveSanitize : 1;
            U32 LowLevelFormat : 1;
            U32 ROCRecoverSuccess : 1;
            U32 HighVPassRecoverSuccess : 1;
            U32 HighVPassRecoverFail : 1;
            U32 C_SATA_ERR : 1;
            U32 C_FlashRB_NoReady : 1;
            U32 C_BootTime_More5s : 1;
            U32 C_CommandError : 1;
        } bits;
    };

    struct sInternalLog
    {
        uSaveID SaveID;// 4
        U8 FirmwareVersion[8];// 8
        U32 FirmwareBootTime;// 4
        U32 PowerCycleCount;// 4
        U32 PowerOnHour;// 4
        U32 CurrentTimeStamp;// 4
        U8  Temperature;// 1
        U16 MarkBadCount;// 2
        U32 EccFailCount;// 4
        U32 EccFailCountOffLine;// 4
        U32 ProgramFailCount;// 4
        U32 EccCorrectableCount;// 4
        U64 FirmwareErrorType;// 8
        U64 FirmwareErrorType2;// 8
        U8  ProtocolErrorType;// 1
        U8  Padding_0[143];
        U32 PSERR;// 4
        U16 FatalErrorCount;// 2
        U16 CommandErrorFlag;// 2
        U32 PPBCR;// 4
        U16 DataAbortCount;// 2
        U16 SataErrorPrdPtr;// 2
        U32 SataErrorLba;// 4
        U32 SataErrorXfrCount;// 4
        U8  NCQErrorTag;// 1
        U16 SataErrorPrdFlag;// 2
        U32 PSSTSR;// 4
        U32 SataErrorHour;// 4
        U32 SataErrorCurrentTimeStamp;// 4
        U8  Padding_1[8];
        U16 WPROBlock;// 2
        U16 ActBlock0;// 2
        U16 ActBlock1;// 2
        U16 CleanFBlock;// 2
        U16 PureSpareCount;// 2
        U16 TotalSpareCOunt;// 2
        U8  gEccFailBlockCount;// 1
        U8  Act0PickMode;// 1
        U8  Act1PickMode;// 1
        U16 EmptyBlockCount;// 2
        U8 Padding_2[4];
        U16 WearWProCount;// 2
        U16 WearMapCount;// 2
        U16 WearLinkCount;// 2
        U16 WearAct0Count;// 2
        U16 WearAct1Count;// 2
        U16 WearDataCount;// 2
        U16 WearCleanCount;// 2
        U16 WearEmptyMapCount;// 2
        U16 WearLogBlockCount;// 2
        U8 Padding_3[4];
        U32 g32SwapPickMinValid;// 4
        U32 g32SwapDataBlockCount;// 4
        U32 g32SwapActiveBlockCount;// 4
        U32 g32TailFBlockSerial;// 4
        U16 g16arDataPagePtr_0;// 2
        U16 g16arDataPagePtr_1;// 2
        U16 LastMapSavedTailBlock;// 2
        U16 TailFBlock;// 2
        U16 LinkFBlock;// 2
        U16 TotalDataBlock;// 2
        U32 PowerFailStatus;// 4
        U8  gTotalZoneNum;// 1
        U8  gFastBoot;// 1
        U16 MapBlock;// 2
        U8  Padding_4[64];
        U64 TotalProgPageCount;// 8
        U64 TotalReadPageCount;// 8
        U64 HostProgPageCount;// 8
        U64 FTLProgPageCount;// 8
        U64 HostWriteSectorCount;// 8
        U64 HostReadSectorCount;// 8
        U32 ERET_AUX_0x400;// 4
        U32 ERBTA_AUX_0x401;// 4
        U32 ERSTATUS_AUX_0x402;// 4
        U32 ECR_AUX_0x403;// 4
        U32 EFA_AUX_0x404;// 4
    };

    union uLogHeader
    {
        U8 Buffer[64];
        struct
        {
            U64 Status;
            U16 LogIndex;
        }Field;
    };

    struct sLogEntry
    {
        uLogHeader    Header;
        U8            RawLog[ LOG_BUFFER_SIZE_IN_BYTE ];
    };

private:

    struct sL2PMappingValue
    {
        U16 FBlock;
        U16 Unused;
        U32 Physical; // Unit, Channel, Chip, Plane, Page
    };

    union uL2PMapping
    {
        U8 Buffer[8];

        sL2PMappingValue Value;
    };

public :
    Sm2246();
    ~Sm2246();
    void setDevice(BaseDevice* pDevice);

    eCommandStatus readCIDTable(sCIDTable* pCIDTable);
    eCommandStatus readParameterTable(sParameterTable* pParameterTable);
    eCommandStatus readFlashID(sFlashID* pFlashID);

    // Read H2FTable
    Sm2246::eCommandStatus readH2FTable(U64 startLba, U32 lbaCount, sL2PMapping** pTable, U32* numberOfEntryRead);
    Sm2246::eCommandStatus saveH2FTableToFile(string path, sL2PMapping* mappingBuffer, U32 numberOfEntry, bool saveAppend = false);

    // Read Mark Bad Block
    eCommandStatus readMarkBadBlock(uMarkBadInfo** pMarkBadBlockTable, U16* readEntry);
    eCommandStatus saveMarkBadBlock(string path, uMarkBadInfo* badInfoBuffer, U16 numberOfEntry, bool saveAppend = false);

    // Read Original bad block
    eCommandStatus readBadBlock(sBadBlock** pBadBlock, U32* pTotalBadBlock, bool readOriginalBadBlock = true);
    eCommandStatus saveBadBlock(string path, sBadBlock* pBadBlockList, U32 totalBlock, bool saveAppend);

    // Read Erase count
    eCommandStatus readEraseCountTable(U16** pEraseCountTable, U16* pNumberOfEntry);
    eCommandStatus saveEraseCountTable(string path, U16* pEraseCountBuffer, U16 numberOfEntry, bool saveAppend);

    // ReadLog
    eCommandStatus readLogFromDrive(U16 logIndex, sLogEntry &log);
    eCommandStatus readLogFromFile(string fileName, U32 logPosition, sLogEntry &log, U32 &totalLog); // logPosition we don't know log index until read it, so read log by position
    eCommandStatus countLogFromFile(string fileName, U32 &totalLog);
    eCommandStatus saveLog(eSaveLogMode saveMode, string fileName, const sLogEntry &log, bool saveAppend);
    eCommandStatus saveRawLogBuffer(string path, sLogEntry log, bool saveAppend);
    eCommandStatus saveLogStatistic(eSaveLogMode saveMode, string path, const sLogEntry &log, bool saveAppend);
    eCommandStatus saveEventLog(string path, const sLogEntry &log, bool saveAppend);

    //
    eCommandStatus saveParameterTable(string path, sParameterTable* pParameterTable);
    eCommandStatus saveCIDTable(string path, sCIDTable* pCIDTable);
    eCommandStatus saveRunningSpareCount(string path, sFlashID* pFlashID);

public:
    static string GetErrorString(eCommandStatus cmdStatus);

private:
    eCommandStatus vscRead(U8* pCommand, U16 commandSize);
    eCommandStatus vscWrite(U8* pCommand, U16 commandSize);
    bool isVscMode(U8* pBuffer);
    eCommandStatus vscReadMarkBadBlock(U8 sectorCount);
    eCommandStatus getMarkBadBlockCount(U16* count);

public:
    static const U32  MAX_INTERNAL_LOG   = 65536;

private:
    static const U16 MAX_TOTAL_FBLOCK     = 8192;
    static const U8  MAX_INTERLEAVING_WAY = 8;
    static const U8  MAX_CHANNEL_NUMBER   = 4;
    static const U8  MAX_PLANE_NUMBER     = 4;
    static const U8  MAPPING_SIZE_IN_KB   = 4;
    static const U8  CHUNK_SIZE_IN_KB     = 1;

    static const U8  BAD_BLOCK_ENTRY_SIZE = 32;

    static const char SM2246AA_SIGNATURE[8];

    BaseDevice* m_pDevice;
    uMarkBadInfo* m_pMarkBadInfo;
    U16* m_pEraseCountTable;
    sBadBlock* m_pBadBlockList;

    sL2PMapping* m_pL2PMapping;

};

#endif // SM2246_H

