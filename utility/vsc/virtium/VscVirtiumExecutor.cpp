#include "utility/misc/SystemUtil.h"
#include "VscVirtiumDefine.h"
#include "VscVirtiumExecutor.h"

VscVirtiumExecutor::VscVirtiumExecutor(Device *pDevice):VscVirtiumProtocol(pDevice)
{
}

VscVirtiumExecutor::~VscVirtiumExecutor()
{
}

sBuffer* VscVirtiumExecutor::getReadBuffer()
{
    return VscVirtiumProtocol::getReadBuffer();
}

sBuffer* VscVirtiumExecutor::requestWriteBuffer(U32 sectorCount)
{
    return VscVirtiumProtocol::requestWriteBuffer(sectorCount);
}

// Vsc executor
bool VscVirtiumExecutor::vscGetWorkLoadLogTableInfo(sWorkLoadLogTableInfo &workLoadLogTableInfo)
{
    VscVirtiumConstant::uVscDataStructure_1_0 vscCmd;

    vscCmd.VSCCommand.Command     = VscVirtiumConstant::VSC_READ_FROM_MEMORY;
    vscCmd.VSCCommand.SubCmd      = VscVirtiumConstant::VSC_SUB_GET_WORK_LOAD_LOG_TABLE_INFO;
    vscCmd.VSCCommand.SectorCount = 1;

    bool cmdStatus = issueCommand_1_0(vscCmd, VscVirtiumConstant::VSC_READ);

    if(true == cmdStatus)
    {
        U8* buffer = getReadBuffer()->BufferPtr;

        workLoadLogTableInfo.WorkLoadLogTableAddress         = ByteToU64(buffer, 0);
        workLoadLogTableInfo.WorkLoadLogTableSizeInByte      = ByteToU32(buffer, 8);
        workLoadLogTableInfo.EntrySizeInBytes                = ByteToU32(buffer, 12);
        workLoadLogTableInfo.ItemSizeInBytes                 = ByteToU32(buffer, 16);
        workLoadLogTableInfo.MaxLogEntries                   = ByteToU32(buffer, 20);
        workLoadLogTableInfo.LogEntriesCount                 = ByteToU32(buffer, 24);
        workLoadLogTableInfo.MostRecentLogEntryIndex         = ByteToU32(buffer, 28);
        workLoadLogTableInfo.CurrentLogEntryIndex            = ByteToU32(buffer, 32);
        workLoadLogTableInfo.MaxLogItems                     = ByteToU32(buffer, 36);
        workLoadLogTableInfo.CurrentLogItemIndex             = ByteToU32(buffer, 40);
        workLoadLogTableInfo.TableID                         = ByteToU32(buffer, 44);
        workLoadLogTableInfo.NumberOfRandomReadCommands      = ByteToU64(buffer, 48);
        workLoadLogTableInfo.NumberOfRandomWriteCommands     = ByteToU64(buffer, 56);
        workLoadLogTableInfo.NumberOfSequentialReadCommands  = ByteToU64(buffer, 64);
        workLoadLogTableInfo.NumberOfSequentialWriteCommands = ByteToU64(buffer, 72);
        workLoadLogTableInfo.CurrentWorkloadLogAddress       = ByteToU64(buffer, 80);
    }

    return cmdStatus;
}

bool VscVirtiumExecutor::vscLoadSpecificHeaderTable(U32 tableID)
{
    VscVirtiumConstant::uVscDataStructure_2_0 vscCmd;

    vscCmd.VSCCommand.Command         = VscVirtiumConstant::VSC_CONTROL;
    vscCmd.VSCCommand.SubCmd          = VscVirtiumConstant::VSC_SUB_VSC_LOADSPECIFICHEADERTABLE_12_128;
    vscCmd.VSCCommand.SectorCount     = 0;
    vscCmd.VSCCommand.SubParameter[0] = tableID;

    return issueCommand_2_0(vscCmd);
}

bool VscVirtiumExecutor::vscLoadPagingCyclingMobileSession(U32 tableID, U32 sessionIndex)
{
    VscVirtiumConstant::uVscDataStructure_2_0 vscCmd;

    vscCmd.VSCCommand.Command         = VscVirtiumConstant::VSC_CONTROL;
    vscCmd.VSCCommand.SubCmd          = VscVirtiumConstant::VSC_SUB_VSC_LOADPAGINGCYCLINGMOBILESESSION_12_82;
    vscCmd.VSCCommand.SectorCount     = 0;
    vscCmd.VSCCommand.SubParameter[0] = tableID;
    vscCmd.VSCCommand.SubParameter[1] = sessionIndex;

    return issueCommand_2_0(vscCmd);
}

bool VscVirtiumExecutor::vscReadFromMemory(U64 memoryAddress, U16 sectorCount)
{
    VscVirtiumConstant::uVscDataStructure_1_0 vscCmd;

    vscCmd.VSCCommand.Command     = VscVirtiumConstant::VSC_READ_FROM_MEMORY;
    vscCmd.VSCCommand.SubCmd      = VscVirtiumConstant::VSC_SUB_READ_ADDRESS;
    vscCmd.VSCCommand.SectorCount = sectorCount;

    vscCmd.VSCCommand.SubParameter.AccessMemory.Address64.Low  = (U32)(memoryAddress & 0xFFFFFFFF);
    vscCmd.VSCCommand.SubParameter.AccessMemory.Address64.High = (U32)(memoryAddress >> 32);

    return issueCommand_1_0(vscCmd, VscVirtiumConstant::VSC_READ);
}
