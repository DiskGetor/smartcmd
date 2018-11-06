#include "STPCommand.h"

#include "CsmiSas.h"

string STPCommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "STPCommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}

const char* STPCommand::getSignatureString() const
{
    const char* signature = NULL;

    switch(CommandCode)
    {
        case STP_DEVICE_RESET        :
        case STP_DOWNLOAD_MICROCODE  :
        case STP_IDENTIFY_DEVICE     :
        case STP_SMART_DISABLE       :
        case STP_SMART_ENABLE        :
        case STP_SMART_READ_DATA     :
        case STP_SMART_READ_THRESHOLD:
        case STP_SMART_READ_LOG      :
        case STP_SMART_WRITE_LOG     :
        case STP_READ_BUFFER         :
        case STP_READ_DMA            :
        case STP_READ_DMA_EXT        :
        case STP_READ_SECTOR         :
        case STP_READ_SECTOR_EXT     :
        case STP_WRITE_BUFFER        :
        case STP_WRITE_DMA           :
        case STP_WRITE_DMA_EXT       :
        case STP_WRITE_SECTOR        :
        case STP_WRITE_SECTOR_EXT    :
        case STP_SET_PASSWORD        :
        case STP_DISABLE_PASSWORD    :
        case STP_ERASE_PREPARE       :
        case STP_ERASE_UNIT          :
        case STP_UNLOCK              :
        case STP_FREEZE_LOCK         :
            signature = CSMI_SAS_SIGNATURE; break;

        default: break;
    }

    return signature;
}
