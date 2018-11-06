#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/cmd/SATCommand.h"
#include "utility/device/DeviceUtil.h"
#include "utility/device/DeviceManager.h"

#include "SystemHeader.h"

eCommandType DeviceManager::GetCommandType(U32 handle)
{
    // update bus-type, max-transfer-length
    // in windows, using control code storage-adapter-descriptor

    bool status = false;
    eCommandType cmdType = CMDTYPE_COUNT;

    do {
        STORAGE_PROPERTY_QUERY query;
        memset (&query, 0x00, sizeof (query));
        query.PropertyId = StorageAdapterProperty;
        query.QueryType = PropertyStandardQuery;

        // Output data structure
        STORAGE_ADAPTER_DESCRIPTOR buffer;
        memset (&buffer, 0x00, sizeof(buffer));

        DWORD returnData;
        status = DeviceIoControl(
                        (HANDLE) handle,
                        IOCTL_STORAGE_QUERY_PROPERTY ,
                        &query, sizeof (query),
                        &buffer, sizeof(buffer),
                        &returnData,
                        NULL
                        );

        if (false == status) break;

        STORAGE_ADAPTER_DESCRIPTOR desc;
        memcpy(&desc, &buffer, sizeof(desc));

        // Extract bus type, max data transfer,
        switch(desc.BusType)
        {
            case BusTypeAta: cmdType = CMDTYPE_ATA; break;
            case BusTypeUsb: cmdType = CMDTYPE_SCSI; break;
            case BusTypeRAID: cmdType = CMDTYPE_STP; break;
            default: cmdType = CMDTYPE_ATA; break;
        }

        // Here we try the SAT command for all bus type.
        // If the device cannot execute the SAT_IDENTIFY_DEVICE, the commandtype will not be changed

        bool cmdStatus;
        SATCommand cmd;
        cmd.setCommand(0, 1, SAT_IDENTIFY_DEVICE);

        cmdStatus =
            (true == cmd.executeCommand(handle)) &&
            (CMD_ERROR_NONE == cmd.getErrorStatus());

        if (false == cmdStatus) break;

        // Try other ATA-based commands
        cmd.setCommand(0, 1, SAT_READ_DMA);

        cmdStatus =
            (true == cmd.executeCommand(handle)) &&
            (CMD_ERROR_NONE == cmd.getErrorStatus());

        cmdType = cmdStatus ? CMDTYPE_SAT : CMDTYPE_HYP;

    } while(0);

    return cmdType;
}
