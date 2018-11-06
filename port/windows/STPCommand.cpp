// Windows implementation

#include "SystemHeader.h"

#include "utility/cmd/CsmiSas.h"
#include "utility/cmd/STPCommand.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE IOCTL_SCSI_MINIPORT

struct sHeaderData
{
    CSMI_SAS_STP_PASSTHRU_BUFFER Stp;
};

void STPCommand::initPhyEntity(const CSMI_SAS_PHY_ENTITY &phy)
{
    memcpy((void*)&PhyEntity, (void*)&phy, sizeof (PhyEntity));
}

void STPCommand::buildBasicCommand()
{
    // Init IoctlHeader member
    buildIoctlHeader();

    // Init Parameters member
    CSMI_SAS_STP_PASSTHRU_BUFFER& stp = *(CSMI_SAS_STP_PASSTHRU_BUFFER*) getStructPtr();
    CSMI_SAS_PHY_ENTITY& phyEntity = PhyEntity;
    CSMI_SAS_STP_PASSTHRU& pthru = stp.Parameters;

    pthru.bPortIdentifier = phyEntity.bPortIdentifier;
    pthru.bPhyIdentifier = phyEntity.Identify.bPhyIdentifier;
    pthru.bConnectionRate = CSMI_SAS_LINK_RATE_NEGOTIATED;
    memcpy(pthru.bDestinationSASAddress, phyEntity.Attached.bSASAddress, sizeof (pthru.bDestinationSASAddress));

    pthru.uDataLength = SectorCount * 512;
    pthru.uFlags = CSMI_SAS_STP_PIO | CSMI_SAS_STP_READ;

    if (true == isDataInCommand()) pthru.uFlags |= CSMI_SAS_STP_READ;
    if (true == isDataOutCommand()) pthru.uFlags |= CSMI_SAS_STP_WRITE;

    if (true == isPioMode()) pthru.uFlags |= CSMI_SAS_STP_PIO;
    if (true == isDmaMode()) pthru.uFlags |= CSMI_SAS_STP_DMA;

    buildFis(pthru.bCommandFIS);
}

bool STPCommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string STPCommand::getTaskfileString()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    CSMI_SAS_STP_PASSTHRU& pthru = p.Stp.Parameters;

    return BaseCommand::GetTaskfileString(pthru.bCommandFIS, 20);
}

eCommandError STPCommand::getErrorStatus()
{
    return CMD_ERROR_NONE;
}

void STPCommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool STPCommand::executeCommand(int handle)
{
    HANDLE deviceHandle = (HANDLE) handle; // In Windows, HANDLE == pvoid

    U8* structPtr = getStructPtr();
    U32 structSize = getStructSize();

    DWORD byteReturn = 0;
    return DeviceIoControl(
        deviceHandle,
        SYSTEM_CODE,
        structPtr, structSize,
        structPtr, structSize,
        &byteReturn,
        NULL);
}

bool STPCommand::buildIoctlHeader()
{
    IOCTL_HEADER& header = *(IOCTL_HEADER*) (void*) getStructPtr();
    U32 structSize = getStructSize();

    const char* signature = getSignatureString();
    if (NULL == signature) return false;

    header.HeaderLength = sizeof(IOCTL_HEADER);
    header.Timeout = CSMI_SAS_TIMEOUT;
    header.ControlCode = CC_CSMI_SAS_STP_PASSTHRU;
    header.Length = structSize - sizeof(IOCTL_HEADER);
    memcpy(&header.Signature, signature, strlen(signature));

    return true;
}

U8* STPCommand::getBufferPtr()
{
    // For STP Command, the buffer start from bDataBuffer

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    return p.Stp.bDataBuffer;
}
