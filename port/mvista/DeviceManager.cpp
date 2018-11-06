#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"

#include "utility/device/DeviceUtil.h"
#include "utility/device/DeviceManager.h"

eCommandType DeviceManager::GetCommandType(U32 handle)
{
    eCommandType cmdType = CMDTYPE_ATA;

    return cmdType;
}
