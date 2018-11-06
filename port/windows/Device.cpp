#include "utility/cmd/ATACommand.h"
#include "utility/device/Device.h"

#include "SystemHeader.h"

bool Device::updateCommandType()
{
    // update bus-type, max-transfer-length
    // in windows, using control code storage-adapter-descriptor

    bool status = false;

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
                        (HANDLE) m_Handle,
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
            case BusTypeUsb: m_CommandType = CMDTYPE_SCSI; break;
            case BusTypeAta: m_CommandType = CMDTYPE_ATA; break;
            default: m_CommandType = CMDTYPE_ATA; break;
        }

        m_TransferLength = desc.MaximumTransferLength; // in byte units

        // Checking for SAT in all cases
        SATCommand cmd;
        cmd.setCommand(0, 1, SAT_IDENTIFY_DEVICE);
        if (true == cmd.executeCommand(m_Handle))
        {
            if (CMD_ERROR_NONE == cmd.getErrorStatus())
            {
                m_CommandType = CMDTYPE_SAT;
            }
        }

        status = true;
    } while(0);

    return status;
}
