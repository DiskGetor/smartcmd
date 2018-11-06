#include "utility/vsc/virtium/VscVirtiumFunction.h"

VscVirtiumFunction::VscVirtiumFunction(Device *pDevice):VscVirtiumExecutor(pDevice)
{
}

VscVirtiumFunction::~VscVirtiumFunction()
{

}

bool VscVirtiumFunction::getWorkLoadLog(tWorkLoadList &workLoadList,
                                sWorkLoadLogTableInfo &workLoadInfo,
                                void (*progress)(U32 current, U32 total))
{
    // Get WorkLoadLogTable info
    if (false == vscGetWorkLoadLogTableInfo(workLoadInfo)) return false;

    U32 tableSizeInSession = (workLoadInfo.WorkLoadLogTableSizeInByte / workLoadInfo.EntrySizeInBytes);

    U8 *pLogBuffer = readPagingCyclingMobileTable( workLoadInfo.TableID,
                                                   workLoadInfo.WorkLoadLogTableAddress,
                                                   tableSizeInSession,
                                                   workLoadInfo.EntrySizeInBytes,
                                                   progress);
    if(NULL == pLogBuffer)
    {
        return false;
    }

    if (workLoadInfo.MaxLogEntries > workLoadInfo.LogEntriesCount)
    {
        for (U32 i = 0; i < workLoadInfo.LogEntriesCount; ++i)
        {
            decodeWorkLoads(pLogBuffer, workLoadInfo.ItemSizeInBytes, i * workLoadInfo.EntrySizeInBytes, workLoadList);
        }
    }
    else
    {
        // determine the most recent entry
        U32 mostRecentEntry = workLoadInfo.MostRecentLogEntryIndex;

        for (U32 i = mostRecentEntry; i < workLoadInfo.MaxLogEntries; ++i)
        {
            decodeWorkLoads(pLogBuffer, workLoadInfo.ItemSizeInBytes, i * workLoadInfo.EntrySizeInBytes, workLoadList);
        }

        for (U32 i = 0; i < mostRecentEntry; ++i)
        {
            decodeWorkLoads(pLogBuffer, workLoadInfo.ItemSizeInBytes, i * workLoadInfo.EntrySizeInBytes, workLoadList);
        }
    }

    delete pLogBuffer;

    // retrieve the amount of log items in current most recent entry
    pLogBuffer = readFromMemory(workLoadInfo.CurrentWorkloadLogAddress, workLoadInfo.EntrySizeInBytes);
    if (NULL == pLogBuffer)
    {
        return false;
    }

    decodeWorkLoads(pLogBuffer, workLoadInfo.ItemSizeInBytes, 0, workLoadList);
    delete pLogBuffer;

    return true;
}

void VscVirtiumFunction::decodeWorkLoads(U8* pWorkLoadLogBuffer, U32 itemSizeInBytes, U32 startOffset, tWorkLoadList &workLoadList)
{
    U32 itemsCount;

    itemsCount = ByteToU32(pWorkLoadLogBuffer, startOffset);
    startOffset += 4;

    U8* buffer = new U8[itemSizeInBytes];
    for (U32 i = 0; i < itemsCount; ++i)
    {
        U32 offset = (U32)(startOffset + (i * itemSizeInBytes));

        memcpy(buffer, &pWorkLoadLogBuffer[offset], itemSizeInBytes);
        sWorkLoadLogItem workLoadItem;

        workLoadItem.LBA         = ByteToU64(buffer, 0);
        workLoadItem.SectorCount = ByteToU32(buffer, 8);

        workLoadItem.CommandType = ByteToU32(buffer, 12);

        // Cast the last element CommandType in WorkLoadItem to U8 type because this field in FW is U8
        workLoadItem.CommandType = (workLoadItem.CommandType & 0xFF);
        workLoadList.push_back(workLoadItem);
    }

    delete buffer;
}

U8* VscVirtiumFunction::readFromMemory(U64 address, U32 numberOfBytes)
{
    U32 numberOfSectors = (numberOfBytes + VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE - 1) / VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE;

    U8* pBuffer = new U8[numberOfSectors * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE];

    U32 readSectorCount = 0;
    U16 sectorCount;
    U16 limitSector = 256;

    while (0 != numberOfSectors)
    {
        if (limitSector <= numberOfSectors)
        {
            sectorCount = limitSector;
        }
        else
        {
            sectorCount = numberOfSectors;
        }

        numberOfSectors -= sectorCount;

        if (false == vscReadFromMemory(address, sectorCount))
        {
            delete pBuffer;
            return NULL;
        }

        getReadBuffer()->copyTo(&pBuffer[readSectorCount * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE],
                                 sectorCount * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE,
                                 0);

        readSectorCount += sectorCount;

        address += (sectorCount * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE);
    }

    return pBuffer;
}

U8* VscVirtiumFunction::readPagingCyclingMobileTable(U32 tableID,
                                             U32 sessionAddress,
                                             U32 tableSizeInSession,
                                             U32 sessionSizeInByte,
                                             void (*progress)(U32 current, U32 total))
{
    if (false == vscLoadSpecificHeaderTable(tableID)) return NULL;

    U16 sessionSizeInSector = ((sessionSizeInByte + (VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE - 1)) / VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE);
    U32 tableBufferSizeInByte = tableSizeInSession * sessionSizeInByte;
    U8* pBuffer = new U8[tableBufferSizeInByte];

    U32 readBufferIndex = 0;
    for (U32 session = 0; session < tableSizeInSession; ++ session)
    {
        if(NULL != progress)
        {
            progress(session, tableSizeInSession);
        }

        if (false == vscLoadPagingCyclingMobileSession(tableID, session))
        {
            delete pBuffer;
            return NULL;
        }

        if (false == vscReadFromMemory(sessionAddress, sessionSizeInSector))
        {
            delete pBuffer;
            return NULL;
        }

        getReadBuffer()->copyTo(&pBuffer[readBufferIndex], sessionSizeInByte, 0);

        readBufferIndex += sessionSizeInByte;
    }

    return pBuffer;
}

