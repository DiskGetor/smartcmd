#ifndef VSC_VIRTIUM_FUNCTION_H
#define VSC_VIRTIUM_FUNCTION_H

#include "VscVirtiumExecutor.h"

class VscVirtiumFunction : VscVirtiumExecutor
{
public:
    VscVirtiumFunction(Device *pDevice);
    ~VscVirtiumFunction();

public:
    U8* readFromMemory(U64 address, U32 numberOfBytes);
    U8* readPagingCyclingMobileTable(U32 tableID,
                                     U32 sessionAddress,
                                     U32 tableSizeInSession,
                                     U32 sessionSizeInByte,
                                     void (*progress)(U32 current, U32 total));

    bool getWorkLoadLog(tWorkLoadList &workLoadList,
                        sWorkLoadLogTableInfo &workLoadInfo,
                        void (*progress)(U32 current, U32 total));

private:
    void decodeWorkLoads(U8* pWorkLoadLogBuffer, U32 itemSizeInBytes, U32 startOffset, tWorkLoadList &workLoadList);

private:
    sBuffer m_Buffer;
};

#endif
