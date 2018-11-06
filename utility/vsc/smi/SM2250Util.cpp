#include "utility/misc/HexFormatter.h"
#include "SM2250Util.h"

namespace SM2250
{
    bool Read(Device& dev, U32 lba, U32 sector) { return dev.read(lba, sector); }
    bool Write(Device& dev, U32 lba, U32 sector) { return dev.write(lba, sector); }

    bool ReadDMA(Device& dev, U32 lba, U32 sector) { return dev.read(lba, sector); }
    bool WriteDMA(Device& dev, U32 lba, U32 sector) { return dev.write(lba, sector); }
    bool ReadSEC(Device& dev, U32 lba, U32 sector) { return dev.readSector(lba, sector); }
    bool WriteSEC(Device& dev, U32 lba, U32 sector) { return dev.writeSector(lba, sector); }

    bool ExecSequence(Device &dev)
    {
        return Read(dev, 0x00AA, 1) && Read(dev, 0xAA00, 1) &&
               Read(dev, 0x0055, 1) && Read(dev, 0x5500, 1) &&
               Read(dev, 0x55AA, 1);
    }

    U8 FillChannel(U8 chnl, U8 ce)
    {
        return (chnl & 0x7) | ((ce & 0x7) << 3);
    }

    U8 FillWriteOption(U8 plane, U8 strong)
    {
        return ((plane & 0x1) << 7) | ((strong & 0x1) << 4);
    }

    U8 FillReadOption(U8 plane, U8 strong, U8 ecc, U8 scramble)
    {
        return ((plane & 0x1) << 7) | ((strong & 0x1) << 4) | ((ecc & 0x1) << 1) | (scramble & 0x1);
    }

    bool FillResetCpuCommand(U8 *buffer)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF0;
        buffer[ 1] = 0x2C;
        buffer[11] = 0x01;

        return true;
    }

    bool FillReadStrongPageCommand(U8 *buffer, U8 scnt)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF0;
        buffer[ 1] = 0x1B;
        buffer[11] = scnt;

        return true;
    }

    bool FillReadFlashIdCommand(U8 *buffer, U8 scnt)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF0;
        buffer[ 1] = 0x20;
        buffer[11] = scnt;

        return true;
    }

    bool FillEraseBlockCommand(U8 *buffer, U8 blkh, U8 blkl)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF0;
        buffer[ 1] = 0x0C;
        buffer[ 2] = blkh;
        buffer[ 3] = blkl;
        buffer[11] = 0x01;

        return true;
    }

    bool FillReadFlashPageCommand(U8 *buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF0;
        buffer[ 1] = 0x0A;
        buffer[ 2] = blkh;
        buffer[ 3] = blkl;
        buffer[ 4] = page;
        buffer[ 8] = option;
        buffer[10] = chnl;
        buffer[11] = scnt;

        return true;
    }

    bool FillWriteFlashPageCommand(U8 *buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt, bool fillspare)
    {
        if (NULL == buffer) return false;

        memset(buffer, 0x00, 512);
        buffer[ 0] = 0xF1;
        buffer[ 1] = 0x0B;
        buffer[ 2] = blkh;
        buffer[ 3] = blkl;
        buffer[ 4] = page;
        buffer[ 8] = option;
        buffer[10] = chnl;
        buffer[11] = scnt;

        if (true == fillspare) memset(buffer + 0x100, 0xE4, 12);

        return true;
    }

    bool ExecReadCommand(Device &dev, U8 scnt)
    {
        if (false == Write(dev, 0x55AA, 1)) return false;

        return Read(dev, 0x55AA, scnt);
    }

    bool ExecWriteCommand(Device &dev, const U8* buffer, U8 scnt)
    {
        if (false == Write(dev, 0x55AA, 1)) return false;

        dev.fillWriteBuffer(buffer, scnt * 512);

        return Write(dev, 0x55AA, scnt);
    }

    bool ExecReadFlashId(Device &dev)
    {
        if (false == ExecSequence(dev)) return false;
        FillReadFlashIdCommand(dev.getBufferPtr(), 2);
        return ExecReadCommand(dev, 2);
    }

    bool ExecReadFlashPage(Device &dev, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt)
    {
        if (false == ExecSequence(dev)) return false;
        FillReadFlashPageCommand(dev.getBufferPtr(), blkh, blkl, page, option, chnl, scnt);
        return ExecReadCommand(dev, scnt);
    }

    bool ExecWriteFlashPage(Device &dev, const U8 *buffer, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt, bool fillspare)
    {
        if (false == ExecSequence(dev)) return false;
        FillWriteFlashPageCommand(dev.getBufferPtr(), blkh, blkl, page, option, chnl, scnt, fillspare);
        return ExecWriteCommand(dev, buffer, scnt);
    }

    bool ExecWriteRandomPage(Device &dev, U8 blkh, U8 blkl, U8 page, U8 option, U8 chnl, U8 scnt)
    {
        if (false == ExecSequence(dev)) return false;
        FillWriteFlashPageCommand(dev.getBufferPtr(), blkh, blkl, page, option, chnl, scnt, true);

        U32 buffsize = scnt * 512;
        U8* buffer = new U8[buffsize]; memset(buffer, 'Z', buffsize);
        bool status = ExecWriteCommand(dev, buffer, scnt);

        delete buffer;
        return status;
    }

    bool ExecReadLogicalPage(Device &dev, const sFlashIdInfo &flash, U8 ropt, U16 blk, U16 pidx)
    {
        U8 ce, cn, page;
        U8 blkh = (blk >> 8) & 0xFF, blkl = blk & 0xFF;

        if (false == SM2250::ReadPageAddress(flash, pidx, ce, cn, page)) return false;

        U8 channel = SM2250::FillChannel(cn, ce);
        U8 rcnt = flash.TotalSectorsPerPhysPage + 1;

        if (false == SM2250::ExecReadFlashPage(dev, blkh, blkl, page, ropt, channel, rcnt)) return false;

        return true;
    }

    bool ExecWriteLogicalPage(Device &dev, const U8* buffer, const sFlashIdInfo &flash, U8 wopt, U16 blk, U16 pidx, bool fillspare)
    {
        U8 ce, cn, page;
        U8 blkh = (blk >> 8) & 0xFF, blkl = blk & 0xFF;

        if (false == SM2250::ReadPageAddress(flash, pidx, ce, cn, page)) return false;

        U8 channel = SM2250::FillChannel(cn, ce);
        U8 wcnt = flash.TotalSectorsPerPhysPage;

        if (false == SM2250::ExecWriteFlashPage(dev, buffer, blkh, blkl, page, wopt, channel, wcnt, fillspare)) return false;

        return true;
    }

    bool ExecResetCPU(Device &dev)
    {
        if (false == ExecSequence(dev)) return false;
        FillResetCpuCommand(dev.getBufferPtr());
        return ExecReadCommand(dev, 1);
    }

    bool ExecReadStrongPageTable(Device &dev, U8 scnt)
    {
        if (false == ExecSequence(dev)) return false;
        FillReadStrongPageCommand(dev.getBufferPtr(), scnt);
        return ExecReadCommand(dev, scnt);
    }

    bool ExecEraseBlock(Device& dev, U8 blkh, U8 blkl)
    {
        if (false == ExecSequence(dev)) return false;
        FillEraseBlockCommand(dev.getBufferPtr(), blkh, blkl);
        return ExecReadCommand(dev, 1);
    }

    U32 CalculateChecksum(const U8 *buffer, U32 buffsize)
    {
        U32 sum = 0;
        for (U32 i = 0; i < buffsize; i++) sum += buffer[i];

        return sum;
    }

    bool UpdateChecksum(U8 *buffer, U32 size)
    {
        if ((NULL == buffer) || (size < 0x400)) return false;

        U32 sum = CalculateChecksum(buffer + 0x400, size - 0x400);

        buffer[0x3FC] = (sum >> 24) & 0xFF;
        buffer[0x3FD] = (sum >> 16) & 0xFF;
        buffer[0x3FE] = (sum >>  8) & 0xFF;
        buffer[0x3FF] = (sum      ) & 0xFF;

        return true;
    }

    bool UpdateSerialNumber(U8 *buffer, U32 buffsize, const char *serial)
    {
        if ((NULL == buffer) || (buffsize <= 0x4527)) return false;
        if ((NULL == serial) || (strlen(serial) > 20)) return false;

        U8 serbuff[20]; memset (serbuff, 0x00, sizeof(serbuff));
        memcpy(serbuff, serial, strlen(serial));

        return UpdateSerialNumber(buffer, buffsize, serbuff);
    }

    bool UpdateSerialNumber(U8 *buffer, U32 buffsize, const U8 *serial)
    {
        if ((NULL == buffer) || (buffsize <= 0x4527)) return false;
        if ((NULL == serial)) return false;

        memcpy(buffer + 0x10C, serial, 20);
        memcpy(buffer + 0x4514, serial, 20);

        return true;
    }

    bool VerifyInfoBlock(const U8 *buffer, U32 buffsize, const char* sign)
    {
        if (NULL == sign) return false;

        U32 signlen = strlen(sign);
        if ((NULL == buffer) || (buffsize < signlen)) return false;

        return (0 == memcmp(buffer, sign, signlen));
    }

    bool ReadPageAddress(const sFlashIdInfo &flash, U32 pidx, U8 &ce, U8 &cn, U8 &page)
    {
        U8 ce_max = flash.TotalInterleave;
        U8 cn_max = flash.TotalChannels;
        U8 pl_max = 1;

        U32 pmax = flash.TotalPagesPerPhysBlock * ce_max * cn_max * pl_max;
        if (pidx >= pmax) return false;

        U8 ppce = cn_max * pl_max; // page-per-ce
        U8 ppln = ppce * ce_max;   // page-per-line
        U8 ppcn = pl_max;          // page-per-cn

        U8 ci = pidx % ppln;       // column index

        page = pidx / ppln;
        ce = ci / ppce;
        cn = (ci % ppce) / ppcn;

        return true;
    }

    bool ReadPageAddress(const sFlashIdInfo &flash, U32 pidx, U8 &ce, U8 &cn, U8& page, U8 &plane)
    {
        U8 ce_max = flash.TotalInterleave;
        U8 cn_max = flash.TotalChannels;
        U8 pl_max = flash.TotalPlanes;

        U32 pmax = flash.TotalPagesPerPhysBlock * ce_max * cn_max * pl_max;
        if (pidx >= pmax) return false;

        U8 ppce = cn_max * pl_max; // page-per-ce
        U8 ppln = ppce * ce_max;   // page-per-line
        U8 ppcn = pl_max;          // page-per-cn

        U8 ci = pidx % ppln;       // column index

        page = pidx / ppln;
        ce = ci / ppce;
        cn = (ci % ppce) / ppcn;
        plane = (ci % ppcn);

        return true;
    }

    void ReadInfo(const U8 *buffer, sFlashIdInfo &info)
    {
        memset((void*)&info, 0x00, sizeof(info));

        U8 b4 = buffer[0x04];
        U8 bA = buffer[0x0A];
        U8 bB = buffer[0x0B];

        info.PageSizeCode = bB & 0x7;

        switch(info.PageSizeCode)
        {
            case 1: info.TotalSectorsPerPage = 4;  info.PageSizeInKB = 2; break;
            case 2: info.TotalSectorsPerPage = 8;  info.PageSizeInKB = 4; break;
            case 4: info.TotalSectorsPerPage = 16; info.PageSizeInKB = 8; break;
            default: break;
        }

        switch((bB >> 3) & 0x7) {
            case 0: info.TotalPagesPerPhysBlock = 64; break;
            case 1: info.TotalPagesPerPhysBlock = 128; break;
            case 2: info.TotalPagesPerPhysBlock = 256; break;
            case 4: info.TotalPagesPerPhysBlock = 192; break; }

        switch((bB >> 6) & 0x1) {
            case 0: info.TotalPlanes = 1; break;
            case 1: info.TotalPlanes = 2; break; }

        info.FlashType = (bA >> 6) & 0x01;

        switch(b4 & 0x3) {
            case 0: info.TotalInterleave = 1; break;
            case 1: info.TotalInterleave = 2; break;
            case 2: info.TotalInterleave = 4; break;
            case 3: info.TotalInterleave = 8; break; }

        switch((b4 >> 2) & 0x7) {
            case 0: info.TotalChannels = 1; break;
            case 1: info.TotalChannels = 2; break;
            case 2: info.TotalChannels = 3; break;
            case 3: info.TotalChannels = 4; break;
            case 4: info.TotalChannels = 5; break;
            case 5: info.TotalChannels = 6; break;
            case 6: info.TotalChannels = 7; break;
            case 7: info.TotalChannels = 8; break; }

        info.TotalSectorsPerPhysPage = info.TotalPlanes * info.TotalSectorsPerPage;

        info.InfoBlockAddr = (buffer[0x230] << 8) | buffer[0x231];
        info.Isp1BlockAddr = (buffer[0x234] << 8) | buffer[0x235];
        info.Isp2BlockAddr = (buffer[0x236] << 8) | buffer[0x237];
    }

    string ToString(const sFlashIdInfo& info)
    {
        stringstream sstr;

        string flash = (0 == info.FlashType) ? "SLC" : "MLC";
        string plane = (1 == info.TotalPlanes) ? "1-Plane" : "2-Plane";

        sstr << "Total sectors per page: " << (U32)info.TotalSectorsPerPage
             << " (code " << (U32) info.PageSizeCode << ")" << endl;
        sstr << "Total pages per physical block: " << (U32)info.TotalPagesPerPhysBlock << endl;
        sstr << "Two plane mode: " << (U32)info.TotalPlanes << " (" << plane << ")" << endl;
        sstr << "SLC/MLC Flash type: " << (U32)info.FlashType << " (" << flash << ")" << endl;
        sstr << "Total interleave: " << (U32)info.TotalInterleave << endl;
        sstr << "Total channels: " << (U32)info.TotalChannels << endl;
        sstr << "Total sectors per one physical page: " << (U32)info.TotalSectorsPerPhysPage << endl;
        sstr << "INFO block address: " << (U32)info.InfoBlockAddr << endl;
        sstr << "ISP1 block address: " << (U32)info.Isp1BlockAddr << endl;
        sstr << "ISP2 block address: " << (U32)info.Isp2BlockAddr << endl;

        return sstr.str();
    }

    string ToString(U8 blkh, U8 blkl, U8 page, U8 channel)
    {
        stringstream sstr;
        U32 block = (blkh << 8) | blkl;
        U32 ce = (channel >> 3) & 0x7;
        U32 cn = channel & 0x7;

        sstr << "(B" << block << "/P" << (U32) page
             << "/CE" << ce << "/CN" << cn << ")";

        return sstr.str();
    }

    string ToString(U8 blkh, U8 blkl, U8 page, U8 channel, U8 plane)
    {
        U8 ce = (channel >> 3) & 0x7;
        U8 cn = channel & 0x7;
        return ToString(blkh, blkl, page, ce, cn, plane);
    }

    string ToString(U8 blkh, U8 blkl, U8 page, U8 ce, U8 cn, U8 plane)
    {
        stringstream sstr;
        U32 block = (blkh << 8) | blkl;

        sstr << "(B" << block << "/P" << (U32) page
             << "/E" << (U32) ce << "/N" << (U32) cn
             << "/L" << (U32) plane << ")";

        return sstr.str();
    }

    string ToString(const U8* buffer, U32 datasize, U32 sparesize, const string& addr)
    {
        stringstream sstr;

        sstr << "Data section: " << endl
             << HexFormatter::ToHexString(buffer, datasize) << endl;

        sstr << "Spare section: " << endl
             << HexFormatter::ToHexString(buffer + datasize, sparesize) << endl;

        sstr << "PageAddress " << addr
             << " PageSize " << datasize << "(bytes)"
             << " SpareSize " << sparesize << "(bytes)" << endl;

        return sstr.str();
    }
}
