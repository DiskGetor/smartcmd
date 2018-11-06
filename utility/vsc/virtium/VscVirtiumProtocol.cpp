#include "utility/misc/SystemUtil.h"
#include "VscVirtiumProtocol.h"

VscVirtiumProtocol::VscVirtiumProtocol(Device* pDevice)
{
    ASSERT(NULL != pDevice);

    m_pDevice = pDevice;
}

VscVirtiumProtocol::~VscVirtiumProtocol()
{

}

sBuffer* VscVirtiumProtocol::getReadBuffer()
{
    return &m_ReadBuffer;
}

sBuffer* VscVirtiumProtocol::requestWriteBuffer(U32 sectorCount)
{
    if(true == m_WriteBuffer.allocateSector(sectorCount)) return &m_WriteBuffer;
    return NULL;
}

// protocol function
bool VscVirtiumProtocol::open()
{
    VscVirtiumConstant::uVscDataStructure_1_0* pVscCmdBuffer;

    pVscCmdBuffer = (VscVirtiumConstant::uVscDataStructure_1_0*)m_pDevice->getBufferPtr();

    memcpy(pVscCmdBuffer->Sequence.Signature, VscVirtiumConstant::BUFFER_SIGNATURE_1, VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);
    pVscCmdBuffer->Sequence.SequenceNumber = VscVirtiumConstant::m_NumberSequences_0;

    // Write Sequence1    --    Signature1 + sequence1
    if( false == m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER)) return false;

    memcpy(pVscCmdBuffer->Sequence.Signature, VscVirtiumConstant::BUFFER_SIGNATURE_2, VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);
    pVscCmdBuffer->Sequence.SequenceNumber = VscVirtiumConstant::m_NumberSequences_1;

    // Write Sequence2    --    Signature2 + sequence2
    return m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER);
}

bool VscVirtiumProtocol::close()
{
    VscVirtiumConstant::uVscDataStructure_1_0* pVscCmdBuffer;

    pVscCmdBuffer = (VscVirtiumConstant::uVscDataStructure_1_0*)m_pDevice->getBufferPtr();

    memcpy(pVscCmdBuffer->Sequence.Signature, VscVirtiumConstant::BUFFER_SIGNATURE_4, VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);
    pVscCmdBuffer->Sequence.SequenceNumber = VscVirtiumConstant::m_NumberSequences_3;

    // Write Sequence4    --    Signature4 + sequence4
    return m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER);
}

bool VscVirtiumProtocol::waitReady(U8 cmdRevision)
{
    time_t start_time, cur_time;
    time(&start_time);

    // loop until status equal ready
    // 1: ready
    // 0: busy
    U32 numberReady = 0;
    while(1)
    {
        U8* pBuffer = m_pDevice->getBufferPtr();
        memset(pBuffer, 0, 512);

        if( false == m_pDevice->executeCommand(0, 1, ORG_ATA_READ_BUFFER)) return false;

        U8* pBufferTemp = m_pDevice->getBufferPtr();
        numberReady = pBufferTemp[0];// | pBufferTemp[1] << 8 | pBufferTemp[2] << 16 | pBufferTemp[3] << 24;
        if(0 != numberReady) break;

        time(&cur_time);
        if (VscVirtiumConstant::TIME_OUT_VALUE_IN_SECONDS <= (cur_time - start_time)) break;

        // Set time delay for each read status command
        SystemUtil::SystemSleepMillisecond(50);
    }

    if     (1 == cmdRevision) return(1 == numberReady); // version 1.0 require numberReady == 1
    else if(2 == cmdRevision) return(0 != numberReady);
    else return false;
}

bool VscVirtiumProtocol::startTransferData()
{
    VscVirtiumConstant::uVscDataStructure_1_0* pVscCmdBuffer;

    pVscCmdBuffer = (VscVirtiumConstant::uVscDataStructure_1_0*)m_pDevice->getBufferPtr();

    memcpy(pVscCmdBuffer->Sequence.Signature,
           VscVirtiumConstant::BUFFER_SIGNATURE_3,
           VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);

    pVscCmdBuffer->Sequence.SequenceNumber = VscVirtiumConstant::m_NumberSequences_2;

    // Write Sequence4    --    Signature4 + sequence4
    return m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER);
}

bool VscVirtiumProtocol::readDataProcess(U32 sectorCount)
{
    m_ReadBuffer.allocateSector(sectorCount);
    for (U32 sectorIndex = 0; sectorIndex < sectorCount; ++ sectorIndex)
    {
        if( false == m_pDevice->executeCommand(0, 1, ORG_ATA_READ_BUFFER)) return false;

        memcpy(&m_ReadBuffer.BufferPtr[sectorIndex * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE],
                m_pDevice->getBufferPtr(),
                VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE);
    }

    return true;
}

bool VscVirtiumProtocol::writeDataProcess(U32 sectorCount)
{
    U8 *pDriveBuffer = m_pDevice->getBufferPtr();

    ASSERT(m_WriteBuffer.BuferSizeInSector == sectorCount); // Buffer could be not allocated

    for (U32 sectorIndex = 0; sectorIndex < sectorCount; ++ sectorIndex)
    {
        // Load proper data to the buffer temp
        memcpy( pDriveBuffer,
               &m_WriteBuffer.BufferPtr[sectorIndex * VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE],
                VscVirtiumConstant::VSC_SECTOR_SIZE_IN_BYTE);

        if( false == m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER)) return false;
    }

    return true;
}

// VSC version 1.0
bool VscVirtiumProtocol::issueCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd, U32 commandType)
{
    bool cmdStatus = false;
    switch (commandType)
    {
        case VscVirtiumConstant::VSC_READ:
        {
            cmdStatus = processReadGroupCommand_1_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_WRITE:
        {
            cmdStatus = processWriteGroupCommand_1_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_CONTROL:
        {
            cmdStatus = processControlGroupCommand_1_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_NO_STATUS:
        {
            cmdStatus = processNoStatusGroupCommand_1_0(vscCmd);
            break;
        }
    }
    return cmdStatus;
}

bool VscVirtiumProtocol::sendCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd)
{
    VscVirtiumConstant::uVscDataStructure_1_0 *pVscCmdBuffer = (VscVirtiumConstant::uVscDataStructure_1_0 *)m_pDevice->getBufferPtr();

    memcpy(pVscCmdBuffer, &vscCmd, sizeof(VscVirtiumConstant::uVscDataStructure_1_0));

    memcpy(pVscCmdBuffer->CommandVersion.Signature,
           VscVirtiumConstant::VSC_COMMAND_1_0_SIGNATURE,
           VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);

    // Write VSC_Command + Params
    return m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER);
}

bool VscVirtiumProtocol::processReadGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_1_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(1)) return false;

    // Start transfer data
    // Write Buffer    --    Signature3 + sequence3
    if (false == startTransferData()) return false;

    // Transfer data
    // Write Data Process. Using Write Buffer, Read Buffer command to handle and transfer data

    if (false == readDataProcess(vscCmd.VSCCommand.SectorCount)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processWriteGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_1_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(1)) return false;

    // Start transfer data
    // Write Buffer    --    Signature3 + sequence3
    if (false == startTransferData()) return false;

    // Transfer data
    // Write Data Process. Using Write Buffer, Read Buffer command to handle and transfer data
    if (false == writeDataProcess(vscCmd.VSCCommand.SectorCount)) return false;

    // Read Status
    if (false == waitReady(1)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processControlGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_1_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(1)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processNoStatusGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_1_0(vscCmd)) return false;

    // delay 1 second
    SystemUtil::SystemSleep(2);

    return close();
}

// Vsc command version 2.0 function
bool VscVirtiumProtocol::issueCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    bool cmdStatus = false;
    switch (vscCmd.VSCCommand.Command)
    {
        case VscVirtiumConstant::VSC_READ:
        {
            cmdStatus = processReadGroupCommand_2_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_WRITE:
        {
            cmdStatus = processWriteGroupCommand_2_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_CONTROL:
        {
            cmdStatus = processControlGroupCommand_2_0(vscCmd);
            break;
        }

        case VscVirtiumConstant::VSC_NO_STATUS:
        {
            cmdStatus = processNoStatusGroupCommand_2_0(vscCmd);
            break;
        }
    }
    return cmdStatus;
}

bool VscVirtiumProtocol::sendCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    VscVirtiumConstant::uVscDataStructure_2_0 *pVscCmdBuffer = (VscVirtiumConstant::uVscDataStructure_2_0 *)m_pDevice->getBufferPtr();

    memcpy(pVscCmdBuffer, &vscCmd, sizeof(VscVirtiumConstant::uVscDataStructure_2_0));

    memcpy(pVscCmdBuffer->CommandVersion.Signature,
           VscVirtiumConstant::VSC_COMMAND_2_0_SIGNATURE,
           VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE);

    // Write VSC_Command + Params
    return m_pDevice->executeCommand(0, 1, ORG_ATA_WRITE_BUFFER);
}

bool VscVirtiumProtocol::processReadGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_2_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(2)) return false;

    // Start transfer data
    // Write Buffer    --    Signature3 + sequence3
    if (false == startTransferData()) return false;

    // Transfer data
    if (false == readDataProcess(vscCmd.VSCCommand.SectorCount)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processWriteGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_2_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(2)) return false;

    // Start transfer data
    // Write Buffer    --    Signature3 + sequence3
    if (false == startTransferData()) return false;

    // Transfer data
    // Write Data Process. Using Write Buffer, Read Buffer command to handle and transfer data
    if (false == writeDataProcess(vscCmd.VSCCommand.SectorCount)) return false;

    // Read Status
    if (false == waitReady(2)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processControlGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_2_0(vscCmd)) return false;

    // Read Status
    if (false == waitReady(2)) return false;

    // End of frame
    // Write Buffer    --    Signature4 + sequence4
    return close();
}

bool VscVirtiumProtocol::processNoStatusGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd)
{
    // Open
    // Write Sequence1    --    Signature1 + sequence1
    // Write Sequence2    --    Signature2 + sequence2
    if (false == open()) return false;

    // Send Command
    // Write VSC_Command + Params
    if (false == sendCommand_2_0(vscCmd)) return false;

    // delay 1 second
    SystemUtil::SystemSleep(2);

    return close();
}
