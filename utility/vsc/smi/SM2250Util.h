#ifndef SM2250_UTIL_H
#define SM2250_UTIL_H

#include "utility/misc/StdHeader.h"
#include "utility/misc/StdMacro.h"
#include "utility/device/Device.h"

namespace SM2250 
{   
    enum eCommandCode
    {
        CMD_READ_FLASH_PAGE,
        CMD_WRITE_FLASH_PAGE,
        CMD_ERASE_BLOCK,
        CMD_READ_STRONG_PAGE,
        CmD_READ_FLASH_ID,
        CMD_RESET_CPU,
    };

    struct sFlashIdInfo
    {
         U8 PageSizeCode;
         U8 PageSizeInKB;
         U8 TotalSectorsPerPage;
         U8 TotalPagesPerPhysBlock;
         U8 TotalPlanes;
         U8 FlashType;
         U8 TotalInterleave;
         U8 TotalChannels;
         U8 TotalSectorsPerPhysPage;
         U16 InfoBlockAddr;
         U16 Isp1BlockAddr;
         U16 Isp2BlockAddr;
    };

    U8 FillChannel(U8 chnl, U8 ce);
    U8 FillWriteOption(U8 plane, U8 strong);
    U8 FillReadOption(U8 plane, U8 strong, U8 ecc, U8 scramble);

    // Fill VSC command buffers
    bool FillResetCpuCommand(U8* buffer);
    bool FillReadStrongPageCommand(U8* buffer, U8 scnt);
    bool FillEraseBlockCommand(U8* buffer, U8 blkh, U8 blkl);
    bool FillReadFlashIdCommand(U8* buffer, U8 scnt);
    bool FillReadFlashPageCommand(U8* buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt);
    bool FillWriteFlashPageCommand(U8* buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt, bool fillspare = false);

    // Execute prefix commands
    bool ExecSequence(Device& dev);

    // Execute read/write commands
    bool ExecReadCommand(Device& dev, U8 scnt);
    bool ExecWriteCommand(Device& dev, const U8 *buffer, U8 scnt);

    bool ExecReadFlashId(Device& dev);
    bool ExecReadFlashPage(Device& dev, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt);
    bool ExecWriteFlashPage(Device& dev, const U8* buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt, bool fillspare);
    bool ExecWriteRandomPage(Device& dev, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt);
    bool ExecReadLogicalPage(Device &dev, const sFlashIdInfo &flash, U8 ropt, U16 blk, U16 pidx);
    bool ExecWriteLogicalPage(Device &dev, const U8* buffer, const sFlashIdInfo &flash, U8 wopt, U16 blk, U16 pidx, bool fillspare);

    bool ExecResetCPU(Device& dev);
    bool ExecReadStrongPageTable(Device& dev, U8 scnt);
    bool ExecEraseBlock(Device& dev, U8 blkh, U8 blkl);

    // Utilities
    void ReadInfo(const U8* buffer, sFlashIdInfo& info);
    bool ReadPageAddress(const sFlashIdInfo& info, U32 pageidx, U8& ce, U8& cn, U8& pg);
    bool ReadPageAddress(const sFlashIdInfo& info, U32 pageidx, U8& ce, U8& cn, U8& pg, U8& plane);
    U32  CalculateChecksum(const U8* buffer, U32 buffsize);
    bool UpdateChecksum(U8* buffer, U32 size);
    bool UpdateSerialNumber(U8* buffer, U32 buffsize, const char* serial);
    bool UpdateSerialNumber(U8* buffer, U32 buffsize, const U8* serial);
    bool VerifyInfoBlock(const U8 *buffer, U32 buffsize, const char *sign);

    bool Read(Device& dev, U32 lba, U32 sector);
    bool Write(Device& dev, U32 lba, U32 sector);
    bool ReadDMA(Device& dev, U32 lba, U32 sector);
    bool WriteDMA(Device& dev, U32 lba, U32 sector);
    bool ReadSEC(Device& dev, U32 lba, U32 sector);
    bool WriteSEC(Device& dev, U32 lba, U32 sector);

    // String utilities
    string ToString(const sFlashIdInfo& info);
    string ToString(U8 blkh, U8 blkl, U8 page, U8 channel);
    string ToString(U8 blkh, U8 blkl, U8 page, U8 channel, U8 plane);
    string ToString(U8 blkh, U8 blkl, U8 page, U8 ce, U8 cn, U8 plane);
    string ToString(const U8* buffer, U32 datasize, U32 sparesize, const string &addr);
}

#endif // SM2250_UTIL_H
