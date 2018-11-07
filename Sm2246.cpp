#include "utility/misc/HexFormatter.h"
#include "utility/device/DeviceManager.h"
#include <bitset>
#include <assert.h>
#include "EventLog.h"

#include "Sm2246.h"

const char Sm2246::SM2246AA_SIGNATURE[8]  = {'S', 'M', '2', '2', '4', '6', 'A', 'A'};

Sm2246::Sm2246()
{
    m_pDevice    = NULL;

    m_pEraseCountTable  = NULL;
    m_pBadBlockList     = NULL;
    m_pMarkBadInfo      = NULL;
    m_pL2PMapping       = NULL;
}

Sm2246::~Sm2246()
{
    if(NULL != m_pMarkBadInfo) delete m_pMarkBadInfo;
    if(NULL != m_pEraseCountTable) delete m_pEraseCountTable;
    if(NULL != m_pBadBlockList) delete m_pBadBlockList;
    if(NULL != m_pL2PMapping) delete m_pL2PMapping;
}

void Sm2246::setDevice(BaseDevice* pDevice)
{
    m_pDevice = pDevice;
}

Sm2246::eCommandStatus Sm2246::vscRead(U8* pCommand, U16 commandSize)
{
    if( NULL == m_pDevice) return CMD_STATUS_ERROR_INVALID_DEVICE;

    U8* pBuffer = m_pDevice->getBufferPtr();

    // ---- VSC sequence
    if( false == m_pDevice->executeCommand(0x00AA, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0xAA00, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x0055, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x5500, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x55AA, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;

    if( false == isVscMode(pBuffer) )
    {
        return CMD_STATUS_ERROR_VSC_MODE;
    }

    // ---- VSC command
    memset(pBuffer, 0, 512);
    memcpy(pBuffer, pCommand, commandSize);
    if( false == m_pDevice->executeCommand(0x55AA, 1, ATA_WRITE_DMA) ) return CMD_STATUS_ERROR_VSC_COMMAND;

    // ---- VSC Read data base on request
    U8 sectorCount = pCommand[11];
    if( false == m_pDevice->executeCommand(0x55AA, sectorCount, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_COMMAND;

    // debug print
    //cout << endl << "Vsc Read Buffer: " << endl << HexFormatter::ToHexString(pBuffer, 512);

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::vscWrite(U8* pCommand, U16 commandSize)
{
    if( NULL == m_pDevice) return CMD_STATUS_ERROR_INVALID_DEVICE;

    U8 backupSectorBuffer[512];
    U8* pBuffer = m_pDevice->getBufferPtr();

    // ---- VSC sequence
    if( false == m_pDevice->executeCommand(0x00AA, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0xAA00, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x0055, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x5500, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;
    if( false == m_pDevice->executeCommand(0x55AA, 1, ATA_READ_DMA) ) return CMD_STATUS_ERROR_VSC_SEQUENCE;

    if( false == isVscMode(pBuffer) )
    {
        return CMD_STATUS_ERROR_VSC_MODE;
    }

    // ---- VSC command
    memcpy(backupSectorBuffer, pBuffer, 512); // back up user data to use buffer for command
    memset(pBuffer, 0, 512);
    memcpy(pBuffer, pCommand, commandSize);
    if( false == m_pDevice->executeCommand(0x55AA, 1, ATA_WRITE_DMA) ) return CMD_STATUS_ERROR_VSC_COMMAND;

    // ---- VSC Write data base on request
    memcpy(pBuffer, backupSectorBuffer, 512); // copy back user data to command buffer
    U8 sectorCount = pCommand[11];
    if( false == m_pDevice->executeCommand(0x55AA, sectorCount, ATA_WRITE_DMA) ) return CMD_STATUS_ERROR_VSC_COMMAND;

    return CMD_STATUS_ERROR_NONE;
}

bool Sm2246::isVscMode(U8* pBuffer)
{
    return ( 0 == memcmp(&pBuffer[0x2E], SM2246AA_SIGNATURE, sizeof(SM2246AA_SIGNATURE)) );
}

Sm2246::eCommandStatus Sm2246::readParameterTable(Sm2246::sParameterTable* pParameterTable)
{
    eCommandStatus cmdStatus;

    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0]  = 0xF0;
    commandBuffer[1]  = 0x04;
    commandBuffer[11] = 0x0E;

    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if( CMD_STATUS_ERROR_NONE != cmdStatus ) return cmdStatus;

    U8* pBuffer = m_pDevice->getBufferPtr();

    memcpy(pParameterTable, pBuffer, sizeof(sParameterTable));

    return cmdStatus;
}

Sm2246::eCommandStatus Sm2246::readCIDTable(Sm2246::sCIDTable* pCIDTable)
{
    eCommandStatus cmdStatus;

    sParameterTable parameterTable;
    cmdStatus = readParameterTable(&parameterTable);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0]  = 0xF0;
    commandBuffer[1]  = 0x0A;
    commandBuffer[2]  = 0x00;
    commandBuffer[3]  = parameterTable.Good_3_Block;

    commandBuffer[8]  = 0x11;
    commandBuffer[11] = 5;

    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if( CMD_STATUS_ERROR_NONE != cmdStatus ) return cmdStatus;

    U8* pBuffer = m_pDevice->getBufferPtr();

    memcpy(pCIDTable, &pBuffer[0x180], sizeof(sCIDTable));

    return cmdStatus;
}

Sm2246::eCommandStatus Sm2246::readFlashID(Sm2246::sFlashID* pFlashID)
{
    eCommandStatus cmdStatus;

    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0]  = 0xF0;
    commandBuffer[1]  = 0x20;
    commandBuffer[11] = 0x02;

    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if( CMD_STATUS_ERROR_NONE != cmdStatus ) return cmdStatus;

    U8* pBuffer = m_pDevice->getBufferPtr();

    memcpy(pFlashID, pBuffer, sizeof(sFlashID));

    // convert byte oder
    U16 endianU16;
    U16 tmpLowU16;

    endianU16 = pFlashID->TotalLog >> 8;
    endianU16 += pFlashID->TotalLog << 8;
    pFlashID->TotalLog = endianU16;

    // get direct from buffer for exectly byte order.
    // read SMI document for this
    pFlashID->PureSpareCount  = pBuffer[0x260]; pFlashID->PureSpareCount <<= 8;
    tmpLowU16 = pBuffer[0x261]; tmpLowU16 &= 0x00FF;
    pFlashID->PureSpareCount |= tmpLowU16;

    pFlashID->TotalSpareCount  = pBuffer[0x262]; pFlashID->TotalSpareCount <<= 8;
    tmpLowU16 = pBuffer[0x263]; tmpLowU16 &= 0x00FF;
    pFlashID->TotalSpareCount |= tmpLowU16;

    return cmdStatus;
}

Sm2246::eCommandStatus Sm2246::readH2FTable(U64 startLba, U32 lbaCount, Sm2246::sL2PMapping** pTable, U32* numberOfEntryRead)
{
    (* numberOfEntryRead) = 0;
    (*pTable) = NULL;

    U64 maxDriveLba = 0;

    eCommandStatus cmdStatus;

    if(NULL != m_pL2PMapping) delete m_pL2PMapping;
    m_pL2PMapping = new sL2PMapping[lbaCount];

    Sm2246::sParameterTable parameterTable;
    cmdStatus = readParameterTable(&parameterTable);
    if( CMD_STATUS_ERROR_NONE != cmdStatus ) return cmdStatus;

    maxDriveLba = parameterTable.g32TotalDatSector;

    std::bitset<8> chipCount(parameterTable.CeMap);
    std::bitset<8> chipSize(chipCount.count() - 1);
    U8 chipSizeInBit = chipSize.count();

    std::bitset<8> channelCount(parameterTable.ChannelMap);
    std::bitset<8> channelSize(channelCount.count() - 1);
    U8 channelSizeInBit = channelSize.count();

    std::bitset<8> planeCount(parameterTable.gbsOpt3.bits.PlaneNum); // PlaneNum is power of 2 rule and already -1
    U8 planeSizeInBit = planeCount.count();

    U16 pageSizeInKB = (((U8)parameterTable.gbsOpt1.all) & 0x0F) * 4;
    std::bitset<8> unitCount((pageSizeInKB / MAPPING_SIZE_IN_KB) - 1);
    U8 unitSizeInBit = unitCount.count();

    U8 commandBuffer[16];
    U8 chunkSizeInsector   = (CHUNK_SIZE_IN_KB * 1024) / 512;
    U8 mappingSizeInSector = (MAPPING_SIZE_IN_KB * 1024) / 512;

    U32 nMappingBufferIndex = 0;

    U64 nLBA = startLba;
    U64 maxLBA = startLba + lbaCount;

    if( maxLBA > maxDriveLba ) maxLBA = maxDriveLba;

    while( nLBA < maxLBA )
    {
        memset(commandBuffer, 0, sizeof(commandBuffer));

        // Fill parameter
        commandBuffer[0]  = 0xF0;
        commandBuffer[1]  = 0x32;

        commandBuffer[2]  = (nLBA >> 24) & 0xFF;
        commandBuffer[3]  = (nLBA >> 16) & 0xFF;
        commandBuffer[4]  = (nLBA >> 8) & 0xFF;
        commandBuffer[5]  =  nLBA & 0xFF;

        commandBuffer[11] = 1;

        cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
        if( CMD_STATUS_ERROR_NONE != cmdStatus ) return cmdStatus;

        uL2PMapping* pL2PMapping;
        pL2PMapping = (uL2PMapping* )m_pDevice->getBufferPtr();

        U32 nH2FTableIndex = 0;
        U32 maxH2FTableIndex = (commandBuffer[11] * 512) / sizeof(uL2PMapping);

        for( ;
                ( nMappingBufferIndex < lbaCount )
             && ( nLBA < maxLBA )
             && ( nH2FTableIndex < maxH2FTableIndex )
             ;
             ++ nMappingBufferIndex,
             ++ nLBA
           )
        {
            // Parse data
            U32 chunkIndex = (nLBA / chunkSizeInsector) % MAPPING_SIZE_IN_KB;


            m_pL2PMapping[nMappingBufferIndex].LBA         = nLBA;

            m_pL2PMapping[nMappingBufferIndex].FBlock      = pL2PMapping->Value.FBlock;
            U32 physicalValue = pL2PMapping->Value.Physical;
            U32 mask;

            // parse unit
            mask = (1 << unitSizeInBit) - 1;
            m_pL2PMapping[nMappingBufferIndex].Unit = physicalValue & mask;
            physicalValue >>= unitSizeInBit;

            // parse plane
            mask = (1 << planeSizeInBit) - 1;
            m_pL2PMapping[nMappingBufferIndex].Plane = physicalValue & mask;
            physicalValue >>= planeSizeInBit;

            // parse channel
            mask = (1 << channelSizeInBit) - 1;
            m_pL2PMapping[nMappingBufferIndex].Channel = physicalValue & mask;
            physicalValue >>= channelSizeInBit;

            // parse chip
            mask = (1 << chipSizeInBit) - 1;
            m_pL2PMapping[nMappingBufferIndex].Chip = physicalValue & mask;
            physicalValue >>= chipSizeInBit;

            // parse page
            m_pL2PMapping[nMappingBufferIndex].Page = physicalValue;


            m_pL2PMapping[nMappingBufferIndex].ChunkInUnit = chunkIndex & 0xFF;

            if( 0 == ( ( nLBA + 1 ) % mappingSizeInSector ) )
            {
                ++ pL2PMapping;
                ++ nH2FTableIndex;
            }
        } // end of for

        if( nMappingBufferIndex >= lbaCount ) break;
    }

    (*numberOfEntryRead) = nMappingBufferIndex;
    (*pTable) = m_pL2PMapping;
    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveH2FTableToFile(string path, sL2PMapping* mappingBuffer, U32 numberOfEntry, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;
    bool saveHeader = false;

    if(NULL == mappingBuffer) numberOfEntry = 0;

    if( true == saveAppend )
    {
        std::ifstream infile(path.c_str());
        saveHeader = (infile.good() == false);

        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
        saveHeader = true;
    }

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if(0 == numberOfEntry)
    {
        std::stringstream strBuffer;

        strBuffer << "There is no record at this time.";
        saveFile << strBuffer.str();
    }
    else if( true == saveHeader )
    {
        saveFile << "LBA,FBlock,Page,Chip,Channel,Plane,Unit,Chunk(4KB),";
    }

    for(U32 nEntry = 0; nEntry < numberOfEntry; ++ nEntry)
    {
        std::stringstream strBuffer;

        strBuffer << endl;
        strBuffer << mappingBuffer[nEntry].LBA << ',';
        strBuffer << mappingBuffer[nEntry].FBlock << ',';
        strBuffer << mappingBuffer[nEntry].Page << ',';
        strBuffer << (U16)mappingBuffer[nEntry].Chip << ',';
        strBuffer << (U16)mappingBuffer[nEntry].Channel << ',';
        strBuffer << (U16)mappingBuffer[nEntry].Plane << ',';
        strBuffer << (U16)mappingBuffer[nEntry].Unit << ',';
        strBuffer << (U16)mappingBuffer[nEntry].ChunkInUnit << ',';

        saveFile << strBuffer.str();
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::readMarkBadBlock(Sm2246::uMarkBadInfo** pMarkBadBlockTable, U16* readEntry)
{
    (*readEntry) = 0;
    (*pMarkBadBlockTable) = 0;

    eCommandStatus cmdStatus;

    U16 maxEntry;
    cmdStatus = getMarkBadBlockCount(&maxEntry);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    U64 totalByte = maxEntry * sizeof(uMarkBadInfo) + 2;
    U16 sectorCount = totalByte / 512;
    if(0 != (totalByte % 512)) sectorCount += 1;

    cmdStatus = vscReadMarkBadBlock(sectorCount);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    if(NULL == m_pMarkBadInfo) delete m_pMarkBadInfo;
    m_pMarkBadInfo = new uMarkBadInfo[maxEntry];

    U8* pBuffer = m_pDevice->getBufferPtr();
    memcpy((U8*)m_pMarkBadInfo, &pBuffer[2], maxEntry * sizeof(uMarkBadInfo));
    (*readEntry) = maxEntry;
    (*pMarkBadBlockTable) = m_pMarkBadInfo;

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::getMarkBadBlockCount(U16* count)
{
    (*count) = 0;

    eCommandStatus cmdStatus;
    cmdStatus = vscReadMarkBadBlock(1);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    U16* pBuffer = (U16*)m_pDevice->getBufferPtr();
    (*count) = pBuffer[0];

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveMarkBadBlock(string path, uMarkBadInfo* badInfoBuffer, U16 numberOfEntry, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;
    bool saveHeader = false;

    if(NULL == badInfoBuffer) numberOfEntry = 0;

    if( true == saveAppend )
    {
        std::ifstream infile(path.c_str());
        saveHeader = (infile.good() == false);

        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
        saveHeader = true;
    }

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if(0 == numberOfEntry)
    {
        std::stringstream strBuffer;

        strBuffer << "There is no record at this time.";
        saveFile << strBuffer.str();
    }
    else if( true == saveHeader )
    {
        std::stringstream strBuffer;

        strBuffer << "Block,Page,Fail Reason,Ecc Error Bits,Chip,Channel,Plane,Erase Counts,Time Stamp(Hrs),";
        saveFile << strBuffer.str();
    }

    for(U32 nEntry = 0; nEntry < numberOfEntry; ++ nEntry)
    {
        std::stringstream strBuffer;

        strBuffer << endl;
        strBuffer << badInfoBuffer[nEntry].Info.Block << ',';
        strBuffer << badInfoBuffer[nEntry].Info.Page << ',';

        U8 failReason = badInfoBuffer[nEntry].Info.FailReason & 0x03;
        switch(failReason)
        {
            case 0:
                strBuffer << "ECC Fail"<< ',';
            break;

            case 3:
                strBuffer << "Early Retire"<< ',';
            break;

            default:
                strBuffer << "Unknow : "<< (U16)failReason << ',';
            break;
        }

        strBuffer << (U16)badInfoBuffer[nEntry].Info.EccBit << ',';
        strBuffer << (U16)badInfoBuffer[nEntry].Info.Chip << ',';
        strBuffer << (U16)badInfoBuffer[nEntry].Info.Channel << ',';
        strBuffer << (U16)(badInfoBuffer[nEntry].Info.Plane & 0x01) << ',';
        strBuffer << badInfoBuffer[nEntry].Info.EraseCount << ',';
        strBuffer << badInfoBuffer[nEntry].Info.TimeStampInHour << ',';

        saveFile << strBuffer.str();
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::vscReadMarkBadBlock(U8 sectorCount)
{
    eCommandStatus cmdStatus;

    sParameterTable parameterTable;
    cmdStatus = readParameterTable(&parameterTable);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0]  = 0xF0;
    commandBuffer[1]  = 0x33;
    // W0
    commandBuffer[2]  = 0x00;
    commandBuffer[3]  = parameterTable.Good_1_Block;;//0x01;
    // W1
    commandBuffer[4]  = 0x00; // Page
    commandBuffer[5]  = 0x00; // Chunk
    // W2
    commandBuffer[6]  = 0x00; //Read retry
    commandBuffer[7]  = 0x00;
    // W3
    commandBuffer[8]  = 0x11; // Option
    commandBuffer[9]  = 0x00;

    commandBuffer[10] = parameterTable.InfoStartPage.gMarkBadInfoStartPage;

    commandBuffer[11] = sectorCount + 1;// Eztool fix at 0x41 for SM-2246

    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    // debug print
    //cout << endl << "Mark Bad Block buffer:" << endl << HexFormatter::ToHexString(m_pDevice->getBufferPtr(), 128);

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::readEraseCountTable(U16** pEraseCountTable, U16* pNumberOfEntry)
{
    (*pNumberOfEntry)   = 0;
    (*pEraseCountTable) = NULL;

    eCommandStatus cmdStatus;

    sParameterTable parameterTable;
    cmdStatus = readParameterTable(&parameterTable);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    U8 sectorCount = parameterTable.g16FBlockPerCE / 512;
    if(0 != (parameterTable.g16FBlockPerCE % 512)) ++ sectorCount;

    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0]  = 0xF0;
    commandBuffer[1]  = 0x06;

    commandBuffer[11] = sectorCount;

    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    if( NULL != m_pEraseCountTable ) delete m_pEraseCountTable;
    m_pEraseCountTable = new U16[parameterTable.g16FBlockPerCE];

    U8* pBuffer = m_pDevice->getBufferPtr();

    U32 eraseCountOffset;
    eraseCountOffset  = pBuffer[parameterTable.g16FBlockPerCE]     << 24;
    eraseCountOffset += pBuffer[parameterTable.g16FBlockPerCE + 1] << 16;
    eraseCountOffset += pBuffer[parameterTable.g16FBlockPerCE + 2] << 8;
    eraseCountOffset += pBuffer[parameterTable.g16FBlockPerCE + 3];

    for(U16 nBlock = 0; nBlock < parameterTable.g16FBlockPerCE; ++ nBlock)
    {
        if(0xFE == pBuffer[nBlock])
        {
            m_pEraseCountTable[nBlock] = ~0; // set max value for bad block
            -- m_pEraseCountTable[nBlock];
        }
        else
        {
            m_pEraseCountTable[nBlock] = eraseCountOffset + pBuffer[nBlock];
        }
    }

    (*pNumberOfEntry)   = parameterTable.g16FBlockPerCE;
    (*pEraseCountTable) = m_pEraseCountTable;
    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveEraseCountTable(string path, U16* pEraseCountBuffer, U16 numberOfEntry, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;
    bool saveHeader = false;

    if(NULL == pEraseCountBuffer) numberOfEntry = 0;

    if( true == saveAppend )
    {
        std::ifstream infile(path.c_str());
        saveHeader = (infile.good() == false);

        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
        saveHeader = true;
    }

    saveFile.open(path.data(), fileMode);
    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if(0 == numberOfEntry)
    {
        std::stringstream strBuffer;

        strBuffer << "There is no record at this time.";
        saveFile << strBuffer.str();
    }
    else if( true == saveHeader )
    {
        std::stringstream strBuffer;

        strBuffer << "FBlock,Erase Count,Status,";
        saveFile << strBuffer.str();
    }

    for(U16 nBlock = 0; nBlock < numberOfEntry; ++ nBlock)
    {
        std::stringstream strBuffer;

        strBuffer << endl;
        strBuffer << nBlock << ',';

        if(0xFFFE == pEraseCountBuffer[nBlock])
        {
            strBuffer << "0," << "Bad Block"<< ',';
        }
        else
        {
            strBuffer << pEraseCountBuffer[nBlock] << ',' << ',';
        }

        saveFile << strBuffer.str();
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::readBadBlock(Sm2246::sBadBlock** pBadBlock, U32* pTotalBlock, bool readOriginalBadBlock)
{
    (*pTotalBlock) = 0;
    (*pBadBlock)   = NULL;

    eCommandStatus cmdStatus;

    sParameterTable parameterTable;
    cmdStatus = readParameterTable(&parameterTable);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    sFlashID flashID;
    cmdStatus = readFlashID(&flashID);
    if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

    std::bitset<8> chipCount(parameterTable.CeMap);
    std::bitset<8> channelCount(parameterTable.ChannelMap);

    U8 numberOfChannel   = channelCount.count();
    U8 numberOfChip      = chipCount.count();
    U32 maxNumberOfEntry = parameterTable.g16FBlockPerCE * numberOfChannel * numberOfChip;

    if(NULL != m_pBadBlockList) delete m_pBadBlockList;
    m_pBadBlockList = new sBadBlock[maxNumberOfEntry];

    U16 pageStart = parameterTable.InfoStartPage.BlockStatusStartPage;
    U16 numberOfPage = (parameterTable.InfoStartPage.BadBlockBitMapStartPage - parameterTable.InfoStartPage.BlockStatusStartPage) / 2;

    if(false == readOriginalBadBlock)
    {
        pageStart += numberOfPage;
    }

    U16 nBlock = 0;
    U32 totalBlock = 0;
    for(U8 nPage = 0; nPage < numberOfPage; ++ nPage)
    {
        U8 commandBuffer[16];

        memset(commandBuffer, 0, sizeof(commandBuffer));

        // Fill command parameter
        commandBuffer[0]  = 0xF0;
        commandBuffer[1]  = 0x33;
        // W0
        commandBuffer[2]  = 0x00;
        commandBuffer[3]  = parameterTable.Good_1_Block;//0x01;
        // W1
        commandBuffer[4]  = 0x00; // Page
        commandBuffer[5]  = 0x00; // Chunk
        // W2
        commandBuffer[6]  = 0x00; //Read retry
        commandBuffer[7]  = 0x00;
        // W3
        commandBuffer[8]  = 0x11; // Option
        commandBuffer[9]  = 0x00;

        commandBuffer[10] = pageStart + nPage;

        commandBuffer[11] = flashID.SectorPerPageH + 1;// Eztool fix at 0x41 for SM-2246

        cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
        if(CMD_STATUS_ERROR_NONE != cmdStatus) return cmdStatus;

        //cout << endl << "Original Bad Block buffer:" << endl << HexFormatter::ToHexString(m_pDevice->getBufferPtr(), commandBuffer[11]*512);

        U8* pBuffer = m_pDevice->getBufferPtr();

        U32 numberOfEntry = ((commandBuffer[11] - 1)*512) / BAD_BLOCK_ENTRY_SIZE;

        for(U32 nEntry = 0; nEntry < numberOfEntry; ++ nEntry)
        {
            U8* pBadInfoBuffer = &pBuffer[nEntry * BAD_BLOCK_ENTRY_SIZE];

            //// debug
            //std::stringstream dbgStrBuffer;
            //bool printDebug = false;
            //dbgStrBuffer << endl << "Block: " << setw(4) << nBlock << " - " << setfill('0') << setw(4) << hex << nBlock;
            //dbgStrBuffer << "  Entry: " << setfill(' ') << setw(4) << dec << nEntry << "  ";
            //dbgStrBuffer << hex;
            //for(U8 nByte = 0; nByte < BAD_BLOCK_ENTRY_SIZE; ++ nByte)
            //{
            //    if(0 == (nByte % 4)) dbgStrBuffer << " -";
            //    dbgStrBuffer << " " << setfill('0') << setw(2) << hex << (U16)pBadInfoBuffer[nByte];
            //    if(0 != pBadInfoBuffer[nByte]) printDebug = true;
            //}
            //if(true == printDebug) cout << dbgStrBuffer.str();
            //// end debug

            for(U8 nByte = 0; nByte < BAD_BLOCK_ENTRY_SIZE; ++ nByte)
            {
                if( 0 != pBadInfoBuffer[nByte] )
                {
                    U8 badInfo = pBadInfoBuffer[nByte];

                    for(U8 nBit = 0; nBit < 8; ++ nBit)
                    {
                        U8 mask = 1 << nBit;
                        if(0 != (badInfo & mask))
                        {
                            U8 plane = nBit % 4;
                            if(plane > parameterTable.gbsOpt3.bits.PlaneNum) continue;

                            m_pBadBlockList[totalBlock].Block = nBlock;

                            // 1 byte = 2 nibble. each nibble is a channel
                            m_pBadBlockList[totalBlock].Channel = (nByte % 4) * 2;
                            if( nBit > 3 ) ++ m_pBadBlockList[totalBlock].Channel;

                            // each bit in a channel is plane
                            m_pBadBlockList[totalBlock].Plane = plane;

                            // group of 4 bytes descript for 4 channel per chip
                            m_pBadBlockList[totalBlock].Chip  = (nByte / 4);

                            ++ totalBlock;

                            assert( totalBlock <= maxNumberOfEntry );
                        }
                    }
                }
            }

            ++ nBlock;
        }
    }

    (*pTotalBlock) = totalBlock;
    (*pBadBlock)   = m_pBadBlockList;
    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveBadBlock(string path, sBadBlock* pBadBlockList, U32 totalBlock, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;
    bool saveHeader = false;

    if(NULL == pBadBlockList) totalBlock = 0;

    if( true == saveAppend )
    {
        std::ifstream infile(path.c_str());
        saveHeader = (infile.good() == false);

        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
        saveHeader = true;
    }

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if(0 == totalBlock)
    {
        std::stringstream strBuffer;

        strBuffer << "There is no record at this time.";
        saveFile << strBuffer.str();
    }
    else if( true == saveHeader )
    {
        std::stringstream strBuffer;

        strBuffer << "FBlock,Chip,Channel,Plane,";
        saveFile << strBuffer.str();
    }

    for(U32 nBlock = 0; nBlock < totalBlock; ++ nBlock)
    {
        std::stringstream strBuffer;

        strBuffer << endl;
        strBuffer << pBadBlockList[nBlock].Block << ',';
        strBuffer << (U16)pBadBlockList[nBlock].Chip << ',';
        strBuffer << (U16)pBadBlockList[nBlock].Channel << ',';
        strBuffer << (U16)pBadBlockList[nBlock].Plane << ',';

        saveFile << strBuffer.str();
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::readLogFromDrive(U16 logIndex, sLogEntry &log)
{
    U8 commandBuffer[16];

    memset(commandBuffer, 0, sizeof(commandBuffer));

    // Fill command parameter
    commandBuffer[0] = 0xF0;
    commandBuffer[1] = 0xEB;

    commandBuffer[2] = logIndex & 0xFF;
    commandBuffer[3] = (logIndex >> 8) & 0xFF;

    commandBuffer[11] = LOG_BUFFER_SIZE_IN_SECTOR;

    eCommandStatus cmdStatus;
    cmdStatus = vscRead(commandBuffer, sizeof(commandBuffer));
    if( CMD_STATUS_ERROR_NONE != cmdStatus )
    {
        return cmdStatus;
    }

    log.Header.Field.Status    = LOG_STATUS_SMI;
    log.Header.Field.LogIndex  = logIndex;

    memcpy(log.RawLog, m_pDevice->getBufferPtr(), LOG_BUFFER_SIZE_IN_BYTE);

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::readLogFromFile(string fileName, U32 logPosition, sLogEntry &log, U32 &totalLog)
{
    ifstream logFile;

    logFile.open(fileName.data(), ios_base::binary);
    if(false == logFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    // get file size
    logFile.seekg(0, ios_base::end);
    U32 fileSize = logFile.tellg();
    logFile.seekg(0, ios_base::beg);

    // read first header to check was file generated by Virtium ?
    logFile.read((char*)log.Header.Buffer, sizeof(uLogHeader));

    totalLog = 0;
    do
    {
        // if YES, read header
        if(LOG_STATUS_VIRTIUM == log.Header.Field.Status)
        {
            U32 logSize = sizeof(log.Header) + sizeof(log.RawLog);
            U32 filePointer = logPosition * logSize;

            totalLog = fileSize / sizeof(sLogEntry);

            if((filePointer + logSize) > fileSize)
            {
                log.Header.Field.Status   = LOG_STATUS_INVALID;
                log.Header.Field.LogIndex = logPosition;
                break;
            }

            logFile.seekg(filePointer);
            logFile.read((char*)log.Header.Buffer, sizeof(uLogHeader));
        }
        else // if NO, manual build header
        {
            U32 logSize = sizeof(log.RawLog);
            U32 filePointer = logPosition * logSize;

            totalLog = fileSize / LOG_BUFFER_SIZE_IN_BYTE;

            if((filePointer + logSize) > fileSize)
            {
                log.Header.Field.Status   = LOG_STATUS_INVALID;
                log.Header.Field.LogIndex = logPosition;
                break;
            }

            logFile.seekg(filePointer);

            string strNumber = fileName.substr(0, 5);
            log.Header.Field.Status = LOG_STATUS_SMI;
            log.Header.Field.LogIndex  = strtoul(strNumber.c_str(), 0, 10);
        }

        logFile.read((char*)log.RawLog, LOG_BUFFER_SIZE_IN_BYTE);
    }while(0);

    logFile.close();
    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveLog(Sm2246::eSaveLogMode saveMode, string fileName, const sLogEntry &log, bool saveAppend)
{
    eCommandStatus cmdStatus = CMD_STATUS_ERROR_NONE;

    switch(saveMode)
    {
        case SAVE_LOG_STATISTIC:
            cmdStatus = saveLogStatistic(saveMode, fileName, log, saveAppend);
        break;

        case SAVE_LOG_VERBOSE:
            cmdStatus = saveLogStatistic(saveMode, fileName, log, saveAppend);
        break;

        case SAVE_LOG_BINARY:
            cmdStatus = saveRawLogBuffer(fileName, log, saveAppend);
        break;

        case SAVE_LOG_EVENT:
            cmdStatus = saveEventLog(fileName, log, saveAppend);
        break;
    }

    return cmdStatus;
}

Sm2246::eCommandStatus Sm2246::saveRawLogBuffer(string path, sLogEntry log, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;

    // make file mode
    if( true == saveAppend )
    {
        fileMode = ios_base::binary | ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::binary | ios_base::out | ios_base::trunc;
    }

    saveFile.open(path.data(), fileMode);
    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    log.Header.Field.Status = LOG_STATUS_VIRTIUM;
    saveFile.write((const char*)log.Header.Buffer, sizeof(uLogHeader));
    saveFile.write((const char*)log.RawLog, LOG_BUFFER_SIZE_IN_BYTE);

    saveFile.close();
    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveLogStatistic(Sm2246::eSaveLogMode saveMode, string path, const sLogEntry &log, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;
    bool saveHeader = false;
    std::stringstream strBuffer;

    // make file mode
    if( true == saveAppend )
    {
        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
    }

    // decide save header
    saveHeader = false;
    // all other value force to SAVE_LOG_MODE_VERBOSE
    if(SAVE_LOG_STATISTIC == saveMode)
    {
        if( true == saveAppend )
        {
            std::ifstream infile(path.c_str());
            saveHeader = (infile.good() == false);
        }
        else
        {
            saveHeader = true;
        }
    }

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if((LOG_STATUS_INVALID == log.Header.Field.Status) && (SAVE_LOG_STATISTIC != saveMode))
    {
        strBuffer.flush();
        strBuffer << "There is no record at this time.";
        saveFile << strBuffer.str();
        strBuffer.flush();
    }

    if( true == saveHeader )
    {
        std::stringstream strHeader;

        strHeader << "Log Index,";
        strHeader << "Save ID,";
        strHeader << "Firmware Version,";
        strHeader << "Firmware Boot Time,";
        strHeader << "Power Cycle Count,";
        strHeader << "Power On Hour,";
        strHeader << "Current Time Stamp,";
        strHeader << "Temperature,";
        strHeader << "Mark Bad Count,";
        strHeader << "Ecc Fail Count,";
        strHeader << "Ecc Fail Count Offline,";
        strHeader << "Program Fail Count,";
        strHeader << "Ecc Correctable Count,";
        strHeader << "Firmware Error Type,";
        strHeader << "Firmware Error Type 2,";
        strHeader << "Protocol Error Type,";
        strHeader << "PSERR,";
        strHeader << "Fatal Error Count,";
        strHeader << "Command Error Flag,";
        strHeader << "PPBCR,";
        strHeader << "Data Abort Count,";
        strHeader << "Sata Error Prd Ptr,";
        strHeader << "Sata Error Lba,";
        strHeader << "Sata Error Xfr Count,";
        strHeader << "NCQ Error Tag,";
        strHeader << "Sata Error Prd Flag,";
        strHeader << "PSSTSR,";
        strHeader << "Sata Error Hour,";
        strHeader << "Sata Error Current Time Stamp,";
        strHeader << "WPRO Block,";
        strHeader << "Act Block 0,";
        strHeader << "Act Block 1,";
        strHeader << "Clean FBlock,";
        strHeader << "Pure Spare Count,";
        strHeader << "Total Spare Count,";
        strHeader << "gEccFailBlockCount,";
        strHeader << "Act0 Pick Mode,";
        strHeader << "Act1 Pick Mode,";
        strHeader << "Empty Block Count,";
        strHeader << "Wear WPro Count,";
        strHeader << "Wear Map Count,";
        strHeader << "Wear Link Count,";
        strHeader << "Wear Act0 Count,";
        strHeader << "Wear Act1 Count,";
        strHeader << "Wear Data Count,";
        strHeader << "Wear Clean Count,";
        strHeader << "Wear Empty Map Count,";
        strHeader << "Wear Log Block Count,";
        strHeader << "g32SwapPickMinValid,";
        strHeader << "g32SwapDataBlockCount,";
        strHeader << "g32SwapActiveBlockCount,";
        strHeader << "g32TailFBlockSerial,";
        strHeader << "g16arDataPagePtr_0,";
        strHeader << "g16arDataPagePtr_1,";
        strHeader << "Last Map Saved Tail Block,";
        strHeader << "Tail FBlock,";
        strHeader << "Link FBlock,";
        strHeader << "Total Data Block,";
        strHeader << "Power Fail Status,";
        strHeader << "gTotalZoneNum,";
        strHeader << "gFastBoot,";
        strHeader << "Map Block,";
        strHeader << "Total Prog Page Count,";
        strHeader << "Total Read Page Count,";
        strHeader << "Host Prog Page Count,";
        strHeader << "FTL Prog Page Count,";
        strHeader << "Host Write Sector Count,";
        strHeader << "Host Read Sector Count,";
        strHeader << "ERET_AUX_0x400,";
        strHeader << "ERBTA_AUX_0x401,";
        strHeader << "ERSTATUS_AUX_0x402,";
        strHeader << "ECR_AUX_0x403,";
        strHeader << "EFA_AUX_0x404,";

        saveFile << strHeader.str();
    }

    sInternalLog* pInternalLog = (sInternalLog*)&log.RawLog[INTERNAL_LOG_OFFSET];

    strBuffer.flush();
    strBuffer << endl;
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "" : "********** Log index : ") << log.Header.Field.LogIndex;

    strBuffer << hex;
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSave ID                       : ") << "0x" << pInternalLog->SaveID.all;// 4

    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFirmware Version              : ");
    for(U8 nChar = 0; nChar < sizeof(pInternalLog->FirmwareVersion); ++ nChar) strBuffer << pInternalLog->FirmwareVersion[nChar];

    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFirmware Boot Time            : ") << "0x" << pInternalLog->FirmwareBootTime;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPower Cycle Count             : ") << "0x" << pInternalLog->PowerCycleCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPower On Hour                 : ") << "0x" << pInternalLog->PowerOnHour;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nCurrent Time Stamp            : ") << "0x" << pInternalLog->CurrentTimeStamp;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTemperature                   : ") << "0x" << (U16)pInternalLog->Temperature;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nMark Bad Count                : ") << "0x" << pInternalLog->MarkBadCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nEcc Fail Count                : ") << "0x" << pInternalLog->EccFailCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nEcc Fail Count Off Line       : ") << "0x" << pInternalLog->EccFailCountOffLine;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nProgram Fail Count            : ") << "0x" << pInternalLog->ProgramFailCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nEcc Correctable Count         : ") << "0x" << pInternalLog->EccCorrectableCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFirmware Error Type           : ") << "0x" << pInternalLog->FirmwareErrorType;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFirmware Error Type 2         : ") << "0x" << pInternalLog->FirmwareErrorType2;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nProtocol Error Type           : ") << "0x" << (U16)pInternalLog->ProtocolErrorType;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPSERR                         : ") << "0x" << pInternalLog->PSERR;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFatal Error Count             : ") << "0x" << pInternalLog->FatalErrorCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nCommand Error Flag            : ") << "0x" << pInternalLog->CommandErrorFlag;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPPBCR                         : ") << "0x" << pInternalLog->PPBCR;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nData Abort Count              : ") << "0x" << pInternalLog->DataAbortCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Prd Ptr            : ") << "0x" << pInternalLog->SataErrorPrdPtr;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Lba                : ") << "0x" << pInternalLog->SataErrorLba;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Xfr Count          : ") << "0x" << pInternalLog->SataErrorXfrCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nNCQ Error Tag                 : ") << "0x" << (U16)pInternalLog->NCQErrorTag;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Prd Flag           : ") << "0x" << pInternalLog->SataErrorPrdFlag;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPSSTSR                        : ") << "0x" << pInternalLog->PSSTSR;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Hour               : ") << "0x" << pInternalLog->SataErrorHour;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nSata Error Current Time Stamp : ") << "0x" << pInternalLog->SataErrorCurrentTimeStamp;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWPROBlock                     : ") << "0x" << pInternalLog->WPROBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nActBlock0                     : ") << "0x" << pInternalLog->ActBlock0;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nActBlock1                     : ") << "0x" << pInternalLog->ActBlock1;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nClean FBlock                  : ") << "0x" << pInternalLog->CleanFBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPure Spare Count              : ") << "0x" << pInternalLog->PureSpareCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTotal Spare COunt             : ") << "0x" << pInternalLog->TotalSpareCOunt;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ngEccFailBlockCount            : ") << "0x" << (U16)pInternalLog->gEccFailBlockCount;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nAct0PickMode                  : ") << "0x" << (U16)pInternalLog->Act0PickMode;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nAct1PickMode                  : ") << "0x" << (U16)pInternalLog->Act1PickMode;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nEmpty Block Count             : ") << "0x" << pInternalLog->EmptyBlockCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearWProCount                 : ") << "0x" << pInternalLog->WearWProCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearMapCount                  : ") << "0x" << pInternalLog->WearMapCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearLinkCount                 : ") << "0x" << pInternalLog->WearLinkCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearAct0Count                 : ") << "0x" << pInternalLog->WearAct0Count;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearAct1Count                 : ") << "0x" << pInternalLog->WearAct1Count;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearDataCount                 : ") << "0x" << pInternalLog->WearDataCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearCleanCount                : ") << "0x" << pInternalLog->WearCleanCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearEmptyMapCount             : ") << "0x" << pInternalLog->WearEmptyMapCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nWearLogBlockCount             : ") << "0x" << pInternalLog->WearLogBlockCount;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng32SwapPickMinValid           : ") << "0x" << pInternalLog->g32SwapPickMinValid;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng32SwapDataBlockCount         : ") << "0x" << pInternalLog->g32SwapDataBlockCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng32SwapActiveBlockCount       : ") << "0x" << pInternalLog->g32SwapActiveBlockCount;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng32TailFBlockSerial           : ") << "0x" << pInternalLog->g32TailFBlockSerial;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng16arDataPagePtr_0            : ") << "0x" << pInternalLog->g16arDataPagePtr_0;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ng16arDataPagePtr_1            : ") << "0x" << pInternalLog->g16arDataPagePtr_1;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nLast Map Saved Tail Block     : ") << "0x" << pInternalLog->LastMapSavedTailBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTail FBlock                   : ") << "0x" << pInternalLog->TailFBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nLink FBlock                   : ") << "0x" << pInternalLog->LinkFBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTotal Data Block              : ") << "0x" << pInternalLog->TotalDataBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nPower Fail Status             : ") << "0x" << pInternalLog->PowerFailStatus;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ngTotalZoneNum                 : ") << "0x" << (U16)pInternalLog->gTotalZoneNum;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\ngFastBoot                     : ") << "0x" << (U16)pInternalLog->gFastBoot;// 1
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nMap Block                     : ") << "0x" << pInternalLog->MapBlock;// 2
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTotal Prog Page Count         : ") << "0x" << pInternalLog->TotalProgPageCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nTotal Read Page Count         : ") << "0x" << pInternalLog->TotalReadPageCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nHost Prog Page Count          : ") << "0x" << pInternalLog->HostProgPageCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nFTLProgPageCount              : ") << "0x" << pInternalLog->FTLProgPageCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nHost Write Sector Count       : ") << "0x" << pInternalLog->HostWriteSectorCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nHost Read Sector Count        : ") << "0x" << pInternalLog->HostReadSectorCount;// 8
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nERET_AUX_0x400                : ") << "0x" << pInternalLog->ERET_AUX_0x400;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nERBTA_AUX_0x401               : ") << "0x" << pInternalLog->ERBTA_AUX_0x401;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nERSTATUS_AUX_0x402            : ") << "0x" << pInternalLog->ERSTATUS_AUX_0x402;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nECR_AUX_0x403                 : ") << "0x" << pInternalLog->ECR_AUX_0x403;// 4
    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "\nEFA_AUX_0x404                 : ") << "0x" << pInternalLog->EFA_AUX_0x404;// 4

    strBuffer << ((SAVE_LOG_STATISTIC == saveMode) ? "," : "");

    strBuffer << dec;

    //cout << endl << " -------------------- " << endl << strBuffer.str();

    if(SAVE_LOG_STATISTIC == saveMode)
    {
        saveFile << strBuffer.str();
    }
    else
    {
        strBuffer << endl << endl;
        saveFile  << strBuffer.str();
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveEventLog(string path, const sLogEntry &log, bool saveAppend)
{
    ofstream saveFile;
    ios_base::openmode fileMode;

    if( true == saveAppend )
    {
        fileMode = ios_base::out | ios_base::app;
    }
    else
    {
        fileMode = ios_base::out | ios_base::trunc;
    }

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    if(LOG_STATUS_INVALID == log.Header.Field.Status)
    {
        saveFile << "There is no record at this time.";
    }
    else
    {
        EventLog eventLog;
        string strEventLog;
        stringstream strLogHeader;

        strLogHeader.flush();
        strLogHeader << endl << "********** Log index: " << log.Header.Field.LogIndex;

        U16 nByte = 0x00;
        U8 *pEventLog = (U8 *)&log.RawLog[ENVENT_LOG_OFFSET];
        while(nByte < ENVENT_LOG_BUFFER_SIZE)
        {
            U64* pU64Val = (U64*)&pEventLog[nByte];
            if(0 != (* pU64Val)) break;
            nByte += sizeof(U64);
        }

        while(nByte < ENVENT_LOG_BUFFER_SIZE)
        {
            U8 logLen;

            eventLog.printEventLog(strEventLog, &pEventLog[nByte], logLen);
            if(0 == logLen)
            {
                logLen = 1;
            }
            else
            {
                strEventLog += "\n";
            }

            nByte += logLen;
        }

        saveFile << endl << strLogHeader.str();

        if(0 == strEventLog.size())
        {
            strEventLog += " There is no event.";
        }

        // debug
        //cout << endl << strEventLog;

        saveFile << endl << strEventLog;
    }

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveParameterTable(string path, sParameterTable* pParameterTable)
{
    ofstream saveFile;
    ios_base::openmode fileMode;

    fileMode = ios_base::out | ios_base::trunc | ios::binary;

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    saveFile.write((char*)pParameterTable, sizeof(sParameterTable));
    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveCIDTable(string path, sCIDTable* pCIDTable)
{
    ofstream saveFile;
    ios_base::openmode fileMode;

    fileMode = ios_base::out | ios_base::trunc | ios::binary;

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    saveFile.write((char*)pCIDTable, sizeof(sCIDTable));
    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

Sm2246::eCommandStatus Sm2246::saveRunningSpareCount(string path, sFlashID* pFlashID)
{
    ofstream saveFile;
    ios_base::openmode fileMode;

    fileMode = ios_base::out | ios_base::app;

    saveFile.open(path.data(), fileMode);

    if(false == saveFile.is_open()) return CMD_STATUS_ERROR_OPEN_FILE;

    stringstream strSpareCount;

    strSpareCount << "Running Spare Count: " << pFlashID->TotalSpareCount << endl;

    saveFile << strSpareCount.str();

    saveFile.close();

    return CMD_STATUS_ERROR_NONE;
}

string Sm2246::GetErrorString(eCommandStatus cmdStatus)
{
    string statusString;

    switch(cmdStatus)
    {
        case CMD_STATUS_ERROR_NONE:
            statusString = "None";
        break;

        case CMD_STATUS_ERROR_INVALID_DEVICE:
            statusString = "Invalid Device";
        break;

        case CMD_STATUS_ERROR_VSC_SEQUENCE:
            statusString = "Can't put device to Vendor mode";
        break;

        case CMD_STATUS_ERROR_VSC_MODE:
            statusString = "SSD is not compatible with this command. The following are compatible SSD:\n"
                           "    - Virtium StorFly (VSFB series)";
        break;

        case CMD_STATUS_ERROR_VSC_COMMAND:
            statusString = "Device did't response Vendor Specific command";
        break;

        case CMD_STATUS_ERROR_OPEN_FILE:
            statusString = "Can't open file";
        break;

        default:
        {
            std::stringstream strBuffer;
            strBuffer << "Unknonw status : " << (U16)cmdStatus;
            statusString = strBuffer.str();
        }
    }

    return statusString;
}
