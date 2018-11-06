#include "CSMICommand.h"

string CSMICommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "CSMICommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}

const char* CSMICommand::getSignatureString() const
{
    const char* signature = NULL;

    switch(CommandCode)
    {
        case CSMI_GET_DRIVER_INFO:
        case CSMI_GET_CNTLR_CONFIG:
        case CSMI_GET_CNTLR_STATUS:
        case CSMI_FIRMWARE_DOWNLOAD:
            signature = CSMI_ALL_SIGNATURE; break;

        case CSMI_GET_RAID_INFO     :
        case CSMI_GET_RAID_CONFIG   :
        case CSMI_GET_RAID_FEATURES :
        case CSMI_SET_RAID_CONTROL  :
        case CSMI_GET_RAID_ELEMENT  :
        case CSMI_SET_RAID_OPERATION:
            signature = CSMI_RAID_SIGNATURE; break;

        case CSMI_GET_PHY_INFO      :
        case CSMI_SET_PHY_INFO      :
        case CSMI_GET_LINK_ERRORS   :
        case CSMI_GET_SATA_SIGNATURE:
        case CSMI_GET_SCSI_ADDRESS  :
        case CSMI_GET_DEVICE_ADDRESS:
        case CSMI_TASK_MANAGEMENT   :
        case CSMI_GET_CONNECTOR_INFO:
        case CSMI_GET_LOCATION      :
            signature = CSMI_SAS_SIGNATURE; break;

        case CSMI_PHY_CONTROL       :
            signature = CSMI_PHY_SIGNATURE; break;

        default: break;
    }

    return signature;
}

bool CSMICommand::buildIoctlContent()
{
    U8* buffer = getStructPtr();

    switch (CommandCode)
    {
        case CSMI_GET_DRIVER_INFO   : // do nothing
        case CSMI_GET_CNTLR_CONFIG  : // do nothing
        case CSMI_GET_CNTLR_STATUS  : // do nothing
        case CSMI_GET_RAID_INFO     : // do nothing
        case CSMI_GET_PHY_INFO      : // do nothing
        case CSMI_GET_CONNECTOR_INFO: // do nothing
        case CSMI_GET_LOCATION      : // do nothing
            break;

        case CSMI_GET_RAID_CONFIG   :
        {
            CSMI_SAS_RAID_CONFIG& config = *(CSMI_SAS_RAID_CONFIG*)(void*)(buffer + sizeof (IOCTL_HEADER));

            config.uRaidSetIndex = 0;
        } break;

        case CSMI_SET_PHY_INFO      :
        {
            CSMI_SAS_SET_PHY_INFO& config = *(CSMI_SAS_SET_PHY_INFO*)(void*)(buffer + sizeof (IOCTL_HEADER));

            config.bPhyIdentifier = 0;
            config.bSignalClass = CSMI_SAS_SIGNAL_CLASS_UNKNOWN;
            config.bNegotiatedLinkRate = CSMI_SAS_LINK_RATE_NEGOTIATE;
            config.bProgrammedMinimumLinkRate = CSMI_SAS_PROGRAMMED_LINK_RATE_UNCHANGED;
            config.bProgrammedMaximumLinkRate = CSMI_SAS_PROGRAMMED_LINK_RATE_UNCHANGED;
        } break;

        case CSMI_GET_LINK_ERRORS   :
        {
            CSMI_SAS_LINK_ERRORS& config = *(CSMI_SAS_LINK_ERRORS*)(void*)(buffer + sizeof (IOCTL_HEADER));

            config.bPhyIdentifier = 0;
            config.bResetCounts = CSMI_SAS_LINK_ERROR_DONT_RESET_COUNTS;
        } break;

        case CSMI_GET_SATA_SIGNATURE:
        {
            CSMI_SAS_SATA_SIGNATURE& config = *(CSMI_SAS_SATA_SIGNATURE*)(void*)(buffer + sizeof (IOCTL_HEADER));

            config.bPhyIdentifier = 0;
        } break;

        case CSMI_GET_SCSI_ADDRESS  :
        {
            CSMI_SAS_GET_SCSI_ADDRESS_BUFFER& config = *(CSMI_SAS_GET_SCSI_ADDRESS_BUFFER*)(void*)(buffer);
            UNUSE_NAME(config.bSASLun);
            UNUSE_NAME(config.bSASAddress);
        } break;

        case CSMI_GET_DEVICE_ADDRESS  :
        {
            CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER& config = *(CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER*)(void*)(buffer);
            config.bHostIndex = 0;
            config.bPathId = 0;
            config.bTargetId = 0;
            config.bLun = 0;
            UNUSE_NAME(config.bSASLun);
            UNUSE_NAME(config.bSASAddress);
        } break;

        case CSMI_FIRMWARE_DOWNLOAD : // not supported
        case CSMI_GET_RAID_FEATURES : // not supported
        case CSMI_SET_RAID_CONTROL  : // not supported
        case CSMI_GET_RAID_ELEMENT  : // not supported
        case CSMI_SET_RAID_OPERATION: // not supported
        case CSMI_TASK_MANAGEMENT   : // not supported
        case CSMI_PHY_CONTROL       : // not supported
            ASSERT(0); break;
    }

    return true;
}
