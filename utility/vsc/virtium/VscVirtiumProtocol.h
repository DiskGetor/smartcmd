#ifndef VSC_VIRTIUM_PROTOCOL_H
#define VSC_VIRTIUM_PROTOCOL_H

#include "utility/device/Device.h"
#include "VscVirtiumDefine.h"

class VscVirtiumProtocol
{
public:
    VscVirtiumProtocol(Device* pDevice);
    ~VscVirtiumProtocol();

    bool issueCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd, U32 commandType);
    bool issueCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);
    sBuffer* getReadBuffer();
    sBuffer* requestWriteBuffer(U32 sectorCount);

private:

    // Vsc command version 1.0 function
    bool sendCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd);
    bool processReadGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd);
    bool processWriteGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd);
    bool processControlGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd);
    bool processNoStatusGroupCommand_1_0(const VscVirtiumConstant::uVscDataStructure_1_0 &vscCmd);

    // Vsc command version 2.0 function
    bool sendCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);
    bool processReadGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);
    bool processWriteGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);
    bool processControlGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);
    bool processNoStatusGroupCommand_2_0(const VscVirtiumConstant::uVscDataStructure_2_0 &vscCmd);

    // common
    bool open();
    bool close();
    bool waitReady(U8 cmdRevision);// readStatus
    bool startTransferData();
    bool readDataProcess(U32 sectorCount);
    bool writeDataProcess(U32 sectorCount);

private:
    Device *m_pDevice;
    sBuffer m_ReadBuffer;
    sBuffer m_WriteBuffer;
};

#endif // VSC_VIRTIUM_PROTOCOL_H

