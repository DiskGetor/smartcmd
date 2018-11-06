#include "utility/cmd/ATACommand.h"
#include "utility/misc/StringUtil.h"
#include "utility/device/BaseDevice.h"

bool BaseDevice::updateCommandType()
{
    // update bus-type, max-transfer-length
    bool status = false;

    do {
        if (true == status) break;

        if (false == StringUtil::StartWith(m_Name, "/dev/nvme", false)) break;
        m_CommandType = CMDTYPE_NVME;
        m_TransferLength = 128;

        status = true;
    } while(0);

    do {
        if (true == status) break;

        m_CommandType = CMDTYPE_ATA;
        m_TransferLength = 128; // in byte units

        status = true;
    } while(0);

    return status;
}
