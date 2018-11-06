#include "utility/cmd/ATACommand.h"
#include "utility/device/DeviceUtil.h"

#include "SystemHeader.h"

bool DeviceUtil::GetNameList(vector<string> &nameList)
{
    // Try to open:
    // + \\\\.\\PhysicalDriveX
    // + \\\\.\\scsiX:

    nameList.clear();

    do {
        DeviceUtil::OpenDevice(nameList, 16, "\\\\.\\PhysicalDrive%d", DeviceUtil::DEVNAME_NUMBER);
        if (0 != nameList.size()) break;

        DeviceUtil::OpenDevice(nameList, 16, "\\\\.\\Scsi%d:", DeviceUtil::DEVNAME_NUMBER);
        if (0 != nameList.size()) break;
    } while(0);

    return nameList.size() != 0;
}

bool DeviceUtil::OpenDevice(const string &deviceName, int &handle)
{
    HANDLE handlePtr = CreateFileA(
        deviceName.c_str(),
        GENERIC_READ | GENERIC_WRITE, //IOCTL_ATA_PASS_THROUGH requires read-write
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,            //no security attributes
        OPEN_EXISTING,
        0,               //flags and attributes
        NULL             //no template file
    );

    if (handlePtr == INVALID_HANDLE_VALUE) return false;

    handle = (int) handlePtr;

    return true;
}

void DeviceUtil::CloseDevice(int handle)
{
    CloseHandle((HANDLE) handle);
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
