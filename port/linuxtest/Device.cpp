#include "utility/device/Device.h"
#include "utility/cmd/ATACommand.h"

bool Device::updateCommandType()
{
    // update bus-type, max-transfer-length
    bool status = false;

    do {
        m_CommandType = CMDTYPE_ATA;
        m_TransferLength = 128; // in byte units

        status = true;
    } while(0);

    return status;
}
