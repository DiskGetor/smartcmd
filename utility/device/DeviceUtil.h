#ifndef DEVICEUTIL_H
#define DEVICEUTIL_H

#include "core/CoreData.h"
// #include "core/NVMEStruct.h"
#include "NvmeCmd.h"
#include "utility/cmd/ATACommand.h"
#include "utility/device/VendorCode.h"

enum eScanError
{
    SCAN_ERROR_NONE,
    SCAN_ERROR_SKIPDRIVE,
    SCAN_ERROR_PERMISSION,
    SCAN_ERROR_NODRIVE,
};

enum eSecurityState
{
    SECURITY_STATE_INVALID     = 0x00000001,
    SECURITY_STATE_NOTSUPORTED = 0x00000002,
    SECURITY_STATE_0           = 0x00000010,
    SECURITY_STATE_1           = 0x00000020,
    SECURITY_STATE_2           = 0x00000040,
    SECURITY_STATE_3           = 0x00000080,
    SECURITY_STATE_4           = 0x00000100,
    SECURITY_STATE_5           = 0x00000200,
    SECURITY_STATE_6           = 0x00000400,
};

struct sPhysicalDriveInfo
{
    int DriveNumber;
    int DriveHandle;
    string DriveName;
};

class DeviceUtil
{
public:
    enum eDevNameFormat
    {
        DEVNAME_NUMBER,
        DEVNAME_CHAR,
        DEVNAME_COUNT,
    };

// -------------------------------------------------------------
// Porting to each OS
// -------------------------------------------------------------
public:
    static bool GetNameList(vector<string>& nameList);
    static bool OpenDevice(const string& deviceName, int& handle);
    static void OpenDevice(vector<string>& devNameList, int devCount, const char* devFormat, eDevNameFormat frmt = DEVNAME_NUMBER);
    static void OpenDevice(vector<sPhysicalDriveInfo>& driveList);

    static void CloseDevice(int handle);
    static void CloseDevice(vector<sPhysicalDriveInfo>& driveList);

    static bool ReadIdentifyData(const string& deviceName, U8* identifyBuffer);

    static eScanError ScanDriveByPath(list<sDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = set<string>());
    static eScanError ScanDriveBySerial(list<sDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = set<string>());

    static eScanError ScanDriveByPath(list<sRawDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = set<string>());
    static eScanError ScanDriveBySerial(list<sRawDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = set<string>());

// -------------------------------------------------------------
// Common functions
// -------------------------------------------------------------
public: // Utility functions
    static string GetErrorMessage(eScanError scanError);
    static string ToAsciiString(const unsigned char* buff);
    static eVendorCode GetVendorCode(const U8* idSector);

public:
    static bool IsValidSerial(const string& serialNum, const tSerialSet& serialSet = tSerialSet());

public: // Handle scan drive
    static eScanError ScanByPath(list<sDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = tSerialSet());
    static eScanError ScanBySerial(list<sDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = tSerialSet());

    static eScanError ScanByPath(list<sRawDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = tSerialSet());
    static eScanError ScanBySerial(list<sRawDriveInfo>& infoList, bool readSMART = true, const set<string>& dataSet = tSerialSet());

public: // Handle Identify data
    static void ParseInquiryInfo(const unsigned char* idSector, sInquiryInfo& info);
    static bool ReadIdentifyDataCommon(const string& deviceName, U8* identifyBuffer); // Implement using ATACommand
    static void ParseIdentifyInfo(const unsigned char* idSector, string driveName, sIdentifyInfo& driveInfo);

public: // Support Security Feature
    static eSecurityState ReadSecurrityState(const sIdentifySectorInfo& info);
    static eSecurityState ReadSecurrityState(const string& deviceName);
    static eSecurityState ReadSecurrityState(const U8* identifyBuffer);

    static bool TestPasswordState(eATACommandCode code);
    static bool TestSecurityState(eSecurityState state, eATACommandCode code, string& errorString);
};


#endif // DEVICEUTIL_H

