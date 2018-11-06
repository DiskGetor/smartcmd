#ifndef VSC_VIRTIUM_EXECUTOR_H
#define VSC_VIRTIUM_EXECUTOR_H

#include "VscVirtiumProtocol.h"

class VscVirtiumExecutor : private VscVirtiumProtocol
{
public:
    VscVirtiumExecutor(Device *pDevice);
    ~VscVirtiumExecutor();

    sBuffer* getReadBuffer();
    sBuffer* requestWriteBuffer(U32 sectorCount);

public:
    // Vsc command
    bool vscGetWorkLoadLogTableInfo(sWorkLoadLogTableInfo &workLoadLogTableInfo);
    bool vscLoadSpecificHeaderTable(U32 tableID);
    bool vscLoadPagingCyclingMobileSession(U32 tableID, U32 sessionIndex);
    bool vscReadFromMemory(U64 memoryAddress, U16 sectorCount);
};

#endif // VSC_VIRTIUM_EXECUTOR_H
