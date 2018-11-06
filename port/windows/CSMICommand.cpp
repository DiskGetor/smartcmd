// Windows implementation

#include "SystemHeader.h"

#include "utility/cmd/CsmiSas.h"
#include "utility/cmd/CSMICommand.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE IOCTL_SCSI_MINIPORT

struct sHeaderData
{
    union {
        CSMI_SAS_DRIVER_INFO_BUFFER        DriverInfo;
        CSMI_SAS_CNTLR_CONFIG_BUFFER       CntrlConfig;
        CSMI_SAS_CNTLR_STATUS_BUFFER       CntrlStatus;
        CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER  FirmwareDownload;
        CSMI_SAS_RAID_INFO_BUFFER          RaidInfo;
        CSMI_SAS_RAID_CONFIG_BUFFER        RaidConfig;
        CSMI_SAS_RAID_FEATURES_BUFFER      RaidFeatures;
        CSMI_SAS_RAID_CONTROL_BUFFER       RaidControl;
        CSMI_SAS_RAID_ELEMENT_BUFFER       RaidElement;
        CSMI_SAS_RAID_SET_OPERATION_BUFFER RaidOperation;
        CSMI_SAS_PHY_INFO_BUFFER           PhyInfo;
        CSMI_SAS_SET_PHY_INFO_BUFFER       SetPhyInfo;
        CSMI_SAS_LINK_ERRORS_BUFFER        LinkErrors;
        CSMI_SAS_SATA_SIGNATURE_BUFFER     SataSignature;
        CSMI_SAS_GET_SCSI_ADDRESS_BUFFER   ScsiAddress;
        CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER DeviceAddress;
        CSMI_SAS_SSP_TASK_IU_BUFFER        TaskManagement;
        CSMI_SAS_CONNECTOR_INFO_BUFFER     ConnectorInfo;
        CSMI_SAS_GET_LOCATION_BUFFER       Location;
        CSMI_SAS_PHY_CONTROL_BUFFER        PhyControl;
    };
};

void CSMICommand::buildBasicCommand()
{
    // Init IoctlHeader member
    buildIoctlHeader();

    // Init other members
    buildIoctlContent();
}

bool CSMICommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string CSMICommand::getTaskfileString()
{
    IOCTL_HEADER& header = *(IOCTL_HEADER*)(void*)getStructPtr();

    return BaseCommand::GetTaskfileString((U8*)(void*)&header, sizeof (header));
}

eCommandError CSMICommand::getErrorStatus()
{
    return CMD_ERROR_NONE;
}

void CSMICommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;

    // Fix StructSize:
    #define MAP_ITEM(code, buffer) case code: StructSize = sizeof(buffer); break;
    switch (CommandCode)
    {
        MAP_ITEM(CSMI_GET_DRIVER_INFO   , CSMI_SAS_DRIVER_INFO_BUFFER       );
        MAP_ITEM(CSMI_GET_CNTLR_CONFIG  , CSMI_SAS_CNTLR_CONFIG_BUFFER      );
        MAP_ITEM(CSMI_GET_CNTLR_STATUS  , CSMI_SAS_CNTLR_STATUS_BUFFER      );
        MAP_ITEM(CSMI_FIRMWARE_DOWNLOAD , CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER );
        MAP_ITEM(CSMI_GET_RAID_INFO     , CSMI_SAS_RAID_INFO_BUFFER         );
        MAP_ITEM(CSMI_GET_RAID_CONFIG   , CSMI_SAS_RAID_CONFIG_BUFFER       );
        MAP_ITEM(CSMI_GET_RAID_FEATURES , CSMI_SAS_RAID_FEATURES_BUFFER     );
        MAP_ITEM(CSMI_SET_RAID_CONTROL  , CSMI_SAS_RAID_CONTROL_BUFFER      );
        MAP_ITEM(CSMI_GET_RAID_ELEMENT  , CSMI_SAS_RAID_ELEMENT_BUFFER      );
        MAP_ITEM(CSMI_SET_RAID_OPERATION, CSMI_SAS_RAID_SET_OPERATION_BUFFER);
        MAP_ITEM(CSMI_GET_PHY_INFO      , CSMI_SAS_PHY_INFO_BUFFER          );
        MAP_ITEM(CSMI_SET_PHY_INFO      , CSMI_SAS_SET_PHY_INFO_BUFFER      );
        MAP_ITEM(CSMI_GET_LINK_ERRORS   , CSMI_SAS_LINK_ERRORS_BUFFER       );
        MAP_ITEM(CSMI_GET_SATA_SIGNATURE, CSMI_SAS_SATA_SIGNATURE_BUFFER    );
        MAP_ITEM(CSMI_GET_SCSI_ADDRESS  , CSMI_SAS_GET_SCSI_ADDRESS_BUFFER  );
        MAP_ITEM(CSMI_GET_DEVICE_ADDRESS, CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER);
        MAP_ITEM(CSMI_TASK_MANAGEMENT   , CSMI_SAS_SSP_TASK_IU_BUFFER       );
        MAP_ITEM(CSMI_GET_CONNECTOR_INFO, CSMI_SAS_CONNECTOR_INFO_BUFFER    );
        MAP_ITEM(CSMI_GET_LOCATION      , CSMI_SAS_GET_LOCATION_BUFFER      );
        MAP_ITEM(CSMI_PHY_CONTROL       , CSMI_SAS_PHY_CONTROL_BUFFER       );

        default: break;
    }
    #undef MAP_ITEM
}

bool CSMICommand::executeCommand(int handle)
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

bool CSMICommand::buildIoctlHeader()
{
    IOCTL_HEADER& header = *(IOCTL_HEADER*) (void*) getStructPtr();
    U32 structSize = getStructSize();

    const char* signature = getSignatureString();
    if (NULL == signature) return false;

    header.HeaderLength = sizeof(IOCTL_HEADER);
    header.Timeout = CSMI_SAS_TIMEOUT;
    header.ControlCode = CommandCode;
    header.Length = structSize - sizeof(IOCTL_HEADER);
    memcpy(&header.Signature, signature, strlen(signature));

    return true;
}
