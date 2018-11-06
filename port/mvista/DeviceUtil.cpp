#include "utility/cmd/ATACommand.h"
#include "utility/device/DeviceUtil.h"

#include "SystemHeader.h"

bool DeviceUtil::GetNameList(vector<string> &nameList)
{
    // Try to open:
    // + /dev/sdX
    // + /dev/sgX

    nameList.clear();

    do {
        DeviceUtil::OpenDevice(nameList, 24, "/dev/hd%c", DeviceUtil::DEVNAME_CHAR);
        if (0 != nameList.size()) break;

        DeviceUtil::OpenDevice(nameList, 24, "/dev/sd%c", DeviceUtil::DEVNAME_CHAR);
        if (0 != nameList.size()) break;

        DeviceUtil::OpenDevice(nameList, 16, "/dev/sg%d", DeviceUtil::DEVNAME_NUMBER);
        if (0 != nameList.size()) break;
    } while(0);

    return nameList.size() != 0;
}

bool DeviceUtil::OpenDevice(const string &deviceName, int &handle)
{
    handle = open(deviceName.c_str(), O_RDONLY | O_NONBLOCK);
    if (-1 == handle) return false;

    return true;
}

void DeviceUtil::CloseDevice(int handle)
{
    close(handle);
}

bool DeviceUtil::ReadIdentifyData(const string &deviceName, U8 *identifyBuffer)
{
    return ReadIdentifyDataCommon(deviceName, identifyBuffer);
}

eScanError DeviceUtil::ScanDriveByPath(list<sDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    return ScanByPath(infoList, readSMART, dataSet);
}

eScanError DeviceUtil::ScanDriveBySerial(list<sDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    return ScanBySerial(infoList, readSMART, dataSet);
}

eScanError DeviceUtil::ScanDriveByPath(list<sRawDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    return ScanByPath(infoList, readSMART, dataSet);
}

eScanError DeviceUtil::ScanDriveBySerial(list<sRawDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    return ScanBySerial(infoList, readSMART, dataSet);
}
