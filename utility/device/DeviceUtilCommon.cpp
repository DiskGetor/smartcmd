#include "core/CoreData.h"
#include "core/CoreUtil.h"
#include "utility/cmd/ATACommand.h"
#include "utility/misc/StringUtil.h"
#include "utility/misc/MiscUtil.h"

#include "Device.h"
#include "DeviceUtil.h"

void DeviceUtil::CloseDevice(vector<sPhysicalDriveInfo> &driveList)
{
    int driveCount = driveList.size();
    for (int i = 0; i < driveCount; i++)
    {
        CloseDevice(driveList[i].DriveHandle);
    }
}

eScanError DeviceUtil::ScanBySerial(list<sDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    // -------------------------------------------------------------
    // Scan drive using ATACommand
    // -------------------------------------------------------------

    infoList.clear();

    vector<sPhysicalDriveInfo> driveList;
    OpenDevice(driveList);

    int driveCount = driveList.size();
    for (int i = 0; i < driveCount; i++)
    {
        sDriveInfo driveInfo;
        sPhysicalDriveInfo& physDrive = driveList[i];
        string driveName = physDrive.DriveName;

        // Read time
        driveInfo.CaptureTime = time(NULL);

        Device dev;
        if(false == dev.initDevice(driveName)) continue;
        if (false == dev.readIdentify(driveInfo.IdentifyInfo)) continue;
        if (false == IsValidSerial(driveInfo.IdentifyInfo.SerialNumber, dataSet)) continue;
        if((true == readSMART) && (false == dev.readSmart(driveInfo.SmartInfo))) continue;

        infoList.push_back(driveInfo);
    }

    CloseDevice(driveList);

    return (infoList.size() != 0) ? SCAN_ERROR_NONE :
                ((driveCount != 0) ? SCAN_ERROR_SKIPDRIVE : SCAN_ERROR_PERMISSION);
}

eScanError DeviceUtil::ScanByPath(list<sDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    // -------------------------------------------------------------
    // Scan drive using ATACommand
    // -------------------------------------------------------------

    infoList.clear();

    U32 i = 0;
    U32 driveCount = dataSet.size();
    set<string>::const_iterator iter;

    for (iter = dataSet.begin(); iter != dataSet.end(); iter++, i++)
    {
        string driveName = *iter;

        sDriveInfo driveInfo;

        // Read time
        driveInfo.CaptureTime = time(NULL);

        Device dev;
        if (false == dev.initDevice(driveName)) continue;
        if (false == dev.readIdentify(driveInfo.IdentifyInfo)) continue;
        if (false == IsValidSerial(driveInfo.IdentifyInfo.SerialNumber)) continue;
        if((true == readSMART) && (false == dev.readSmart(driveInfo.SmartInfo))) continue;

        infoList.push_back(driveInfo);
    }

    return (infoList.size() != 0) ? SCAN_ERROR_NONE :
                ((driveCount != 0) ? SCAN_ERROR_SKIPDRIVE : SCAN_ERROR_NODRIVE);
}

eScanError DeviceUtil::ScanBySerial(list<sRawDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    // -------------------------------------------------------------
    // Scan drive using ATACommand
    // -------------------------------------------------------------

    infoList.clear();

    vector<sPhysicalDriveInfo> driveList;
    OpenDevice(driveList);

    int driveCount = driveList.size();
    for (int i = 0; i < driveCount; i++)
    {
        sRawDriveInfo driveInfo;
        sIdentifyInfo identifyInfo;
        sPhysicalDriveInfo& physDrive = driveList[i];
        string driveName = physDrive.DriveName;

        // Read time
        driveInfo.DriveName = driveName;
        driveInfo.CaptureTime = time(NULL);

        Device dev;
        if(false == dev.initDevice(driveName)) continue;
        if (false == dev.readIdentify(identifyInfo)) continue;
        if (false == IsValidSerial(identifyInfo.SerialNumber, dataSet)) continue;

        memcpy(driveInfo.IdentifySector, dev.getBufferPtr(), 512);

        if ((true == readSMART) && (false == dev.readSmart(driveInfo.ValueSector, driveInfo.ThresholdSector))) continue;

        infoList.push_back(driveInfo);
    }

    CloseDevice(driveList);

    return (infoList.size() != 0) ? SCAN_ERROR_NONE :
                ((driveCount != 0) ? SCAN_ERROR_SKIPDRIVE : SCAN_ERROR_PERMISSION);
}

eScanError DeviceUtil::ScanByPath(list<sRawDriveInfo>& infoList, bool readSMART, const set<string>& dataSet)
{
    // -------------------------------------------------------------
    // Scan drive using ATACommand
    // -------------------------------------------------------------

    infoList.clear();

    U32 i = 0;
    U32 driveCount = dataSet.size();
    set<string>::const_iterator iter;

    for (iter = dataSet.begin(); iter != dataSet.end(); iter++, i++)
    {
        string driveName = *iter;

        sRawDriveInfo driveInfo;
        sIdentifyInfo identifyInfo;

        // Read time
        driveInfo.DriveName = driveName;
        driveInfo.CaptureTime = time(NULL);

        Device dev;
        if (false == dev.initDevice(driveName)) continue;
        if (false == dev.readIdentify(identifyInfo)) continue;
        if (false == IsValidSerial(identifyInfo.SerialNumber)) continue;

        memcpy(driveInfo.IdentifySector, dev.getBufferPtr(), 512);

        if ((true == readSMART) && (false == dev.readSmart(driveInfo.ValueSector, driveInfo.ThresholdSector))) continue;

        infoList.push_back(driveInfo);
    }

    return (infoList.size() != 0) ? SCAN_ERROR_NONE :
                ((driveCount != 0) ? SCAN_ERROR_SKIPDRIVE : SCAN_ERROR_NODRIVE);
}

void DeviceUtil::OpenDevice(vector<string>& devNameList, int devCount, const char *devFormat, eDevNameFormat frmt)
{
    devNameList.clear();

    int handle;
    char buffer[128];
    string devName;

    for (int i = 0; i < devCount; i++)
    {
        switch (frmt)
        {
            case DEVNAME_NUMBER: sprintf(buffer, devFormat, i); break;
            case DEVNAME_CHAR: sprintf(buffer, devFormat, i + 'a'); break;
            default: ASSERT(0); break;
        }

        devName = string(buffer);

        if (true == DeviceUtil::OpenDevice(devName, handle))
        {
            devNameList.push_back(devName);
        }
    }
}

void DeviceUtil::OpenDevice(vector<sPhysicalDriveInfo>& driveList)
{
    driveList.clear();

    // Get list of drive name
    vector<string> nameList;
    DeviceUtil::GetNameList(nameList);

    // Try to open drive in nameList
    int handle;
    int maxDriveCount = nameList.size();
    for (int i = 0; i < maxDriveCount; i++)
    {
        string driveName = nameList[i];
        if (true == OpenDevice(driveName, handle))
        {
            sPhysicalDriveInfo driveInfo;
            driveInfo.DriveHandle = (int) handle;
            driveInfo.DriveNumber = i;
            driveInfo.DriveName = driveName;
            driveList.push_back(driveInfo);
        }
    }
}

bool DeviceUtil::ReadIdentifyDataCommon(const string &deviceName, U8 *identifyBuffer)
{
    bool status = false;

    do {
        Device dev;
        if (false == dev.initDevice(deviceName)) break;

        if (false == dev.readIdentify()) break;

        memcpy(identifyBuffer, dev.getBufferPtr(), 512);

        status = true;
    } while(0);

    return status;
}

eSecurityState DeviceUtil::ReadSecurrityState(const sIdentifySectorInfo& info)
{
    eSecurityState state = SECURITY_STATE_INVALID;

    do {
        if (false == info.IsSecuritySupported) { state = SECURITY_STATE_NOTSUPORTED; break; }

        if (!info.IsUserPasswordPresent &&
            !info.IsDeviceLocked &&
            !info.IsDeviceFrozen &&
            !info.IsMasterPasswordMaximum) { state = SECURITY_STATE_1; break; }

        if (!info.IsUserPasswordPresent &&
            !info.IsDeviceLocked &&
             info.IsDeviceFrozen) { state = SECURITY_STATE_2; break; }

        if ( info.IsUserPasswordPresent &&
             info.IsDeviceLocked &&
            !info.IsDeviceFrozen) { state = SECURITY_STATE_4; break; }

        if ( info.IsUserPasswordPresent &&
            !info.IsDeviceLocked &&
            !info.IsDeviceFrozen) { state = SECURITY_STATE_5; break; }

        if ( info.IsUserPasswordPresent &&
            !info.IsDeviceLocked &&
             info.IsDeviceFrozen) { state = SECURITY_STATE_6; break; }

    } while(0);

    return state;
}

eSecurityState DeviceUtil::ReadSecurrityState(const string &deviceName)
{
    eSecurityState state = SECURITY_STATE_INVALID;

    // Read the Identify buffer
    U8 identifyBuffer[512];

    if (true == ReadIdentifyData(deviceName, identifyBuffer))
    {
        state = ReadSecurrityState(identifyBuffer);
    }

    return state;
}

eSecurityState DeviceUtil::ReadSecurrityState(const U8 *identifyBuffer)
{
    eSecurityState state = SECURITY_STATE_INVALID;

    do {
        if (NULL == identifyBuffer) break;

        U16 w82 = GET_WORD(identifyBuffer, 82);
        U16 w85 = GET_WORD(identifyBuffer, 85);
        U16 w128 = GET_WORD(identifyBuffer, 128);

        // Testing for SecurityFeatureSet w82.b1 == 1
        if (0 == GET_BIT(w82, 1)) { state = SECURITY_STATE_NOTSUPORTED; break; }

        // Consistent to values in w82
        if (GET_BIT(w128, 0) != GET_BIT(w82, 1)) break;
        if (GET_BIT(w128, 1) != GET_BIT(w85, 1)) break;

        bool activeUser   = (0 != GET_BIT(w85,  1)); // bit 1
        bool deviceLocked = (0 != GET_BIT(w128, 2)); // bit 2
        bool deviceFrozen = (0 != GET_BIT(w128, 3)); // bit 3
        bool maxMaster    = (0 != GET_BIT(w128, 8)); // bit 8

        if (!activeUser && !deviceLocked && !deviceFrozen && !maxMaster) { state = SECURITY_STATE_1; break; }
        if (!activeUser && !deviceLocked &&  deviceFrozen              ) { state = SECURITY_STATE_2; break; }
        if ( activeUser &&  deviceLocked && !deviceFrozen              ) { state = SECURITY_STATE_4; break; }
        if ( activeUser && !deviceLocked && !deviceFrozen              ) { state = SECURITY_STATE_5; break; }
        if ( activeUser && !deviceLocked &&  deviceFrozen              ) { state = SECURITY_STATE_6; break; }

    } while(0);

    return state;
}

bool DeviceUtil::TestPasswordState(eATACommandCode code)
{
    bool requestPassword = false;

    switch(code)
    {
        case ATA_UNLOCK:
        case ATA_ERASE_UNIT:
        case ATA_SET_PASSWORD:
        case ATA_DISABLE_PASSWORD: requestPassword = true; break;

        case ATA_FREEZE_LOCK:
        case ATA_ERASE_PREPARE:    requestPassword = false; break;

        default: break;
    }

    return requestPassword;
}

bool DeviceUtil::TestSecurityState(eSecurityState state, eATACommandCode code, string &errorString)
{
    unsigned int validState = 0;

    // See Table 8, ATA Spec

    switch(code)
    {
        case ATA_DISABLE_PASSWORD: validState |= SECURITY_STATE_1 | SECURITY_STATE_5; break;
        case ATA_ERASE_PREPARE:    validState |= SECURITY_STATE_4 | SECURITY_STATE_1 | SECURITY_STATE_5; break;
        case ATA_ERASE_UNIT:       validState |= SECURITY_STATE_4 | SECURITY_STATE_1 | SECURITY_STATE_5; break;
        case ATA_FREEZE_LOCK:      validState |= SECURITY_STATE_1 | SECURITY_STATE_5 | SECURITY_STATE_2 | SECURITY_STATE_6; break;
        case ATA_SET_PASSWORD:     validState |= SECURITY_STATE_1 | SECURITY_STATE_5; break;
        case ATA_UNLOCK:           validState |= SECURITY_STATE_4 | SECURITY_STATE_1 | SECURITY_STATE_5; break;
        default: break;
    }

    if (state & validState) return true;

    switch (state)
    {
        case SECURITY_STATE_4:           errorString = "Drive is locked. Unlock drive first and try again"; break;
        case SECURITY_STATE_2:
        case SECURITY_STATE_6:           errorString = "Drive is frozen. Unfrozen drive first and try again"; break;
        case SECURITY_STATE_INVALID:     errorString = "Invalid Security state"; break;
        case SECURITY_STATE_NOTSUPORTED: errorString = "Security Feature not supported."; break;
        default: break;
    }

    return false;
}

bool DeviceUtil::IsValidSerial(const string& serial, const tSerialSet& serialSet)
{
    bool status = false;

    do {
        if (0 != serialSet.size())
        {
            status = (serialSet.end() != serialSet.find(serial));
            break;
        }

        // serialSet is empty
        // Check for validation of serial value

        // Serial value should not be ""
        if (0 == serial.length()) break;

        // Serial should have some ascii characters
        // ???

        status = true;
    } while(0);

    return status;
}

string DeviceUtil::GetErrorMessage(eScanError scanError)
{
    const string errorMessage[] = {
        "",
        "Some valid drives are skipped",
        "Please run this program as administrator",
        "Please specify device name",
    };

    U32 messageCount = sizeof(errorMessage) / sizeof (errorMessage[0]);

    ASSERT(scanError < messageCount);

    return errorMessage[scanError];
}

// convert 512byte-sector to string for testing
string DeviceUtil::ToAsciiString(const unsigned char* buff)
{
    stringstream sstr;

    int rowCount = 32;
    int colCount = 512 / rowCount;

    for (int i = 0; i < rowCount; i++)
    {
        stringstream textstr;
        int index = i * colCount;

        sstr << hex << uppercase << setw(4) << setfill('0') << i << " | ";

        for (int j = 0; j < colCount; j++)
        {
            // int k = index + j + 1;
            // k = (k % 2) ? k : k - 2;
            int k = index + j;

            unsigned char val = buff[k];
            sstr << hex << uppercase << setw(2) << setfill('0') << (int) val << " ";

            char ascVal = ((val >= 32) && (val < 127)) ? val : '.';

            textstr << ascVal;
        }

        sstr << " | " << textstr.str() << endl;
    }

    return sstr.str();
}

void DeviceUtil::ParseIdentifyInfo(const unsigned char* idSector, string driveName, sIdentifyInfo& driveInfo)
{
    // Parse identify info and store in driveInfo
    if(1) {
        driveInfo.DriveName = driveName;
    }

    if(1) {
        const int wordPos = 27;   // Word Position of ModelNumber
        const int wordCount = 20; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, true);
        driveInfo.DeviceModel = string((char*)buffer);
        StringUtil::Trim(driveInfo.DeviceModel);
    }

    if(1) {
        const int wordPos = 10;   // Word Position of SerialNumber
        const int wordCount = 10; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, true);
        driveInfo.SerialNumber = string((char*)buffer);
        StringUtil::Trim(driveInfo.SerialNumber);
    }

    if(1) {
        const int wordPos = 23;   // Word Position of FirmwareVersion
        const int wordCount =  4; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, true);
        driveInfo.FirmwareVersion = string((char*)buffer);
        StringUtil::Trim(driveInfo.FirmwareVersion);
    }

    if (1) {
        unsigned int usercap_low = 0, usercap_high = 0;

        U16 w83 = GET_WORD(idSector, 83); // Check for 48b supported

        if((w83 >> 10) & 0x1) {
            // Read Capacity (48b) (word 100 -> 103)
            U16 w100 = GET_WORD(idSector, 100);
            U16 w101 = GET_WORD(idSector, 101);
            U16 w102 = GET_WORD(idSector, 102);
            U16 w103 = GET_WORD(idSector, 103);
            usercap_low = (w101 << 16) | w100;
            usercap_high = (w103 << 16) | w102;
        }
        else
        {
            // Normal capacity info (word 60 -> 61)
            U16 w60 = GET_WORD(idSector, 60);
            U16 w61 = GET_WORD(idSector, 61);
            usercap_low = (w61 << 16) | w60;
        }

        // driveInfo.UserCapacity = (double) usercap / 10;
        driveInfo.LowCapacity = usercap_low;
        driveInfo.HighCapacity = usercap_high;

        // UINT64 usercap = ((((UINT64) usercap_high << 32) | usercap_low) << 9 ) / 2^30;
        driveInfo.UserCapacity = 2048.0 * usercap_high + usercap_low / 2097152.0;

        // UINT64 usercap = ((((UINT64) usercap_high << 32) | usercap_low) << 9 ) / 100000000;
        // driveInfo.UserCapacity = 32 * usercap_high * pow(1.6, 9) + usercap_low * pow(0.2, 9);
    }

    if (1) {
        // Read Security Feature:

        U16 w82 = GET_WORD(idSector, 82);
        U16 w83 = GET_WORD(idSector, 83);
        U16 w85 = GET_WORD(idSector, 85);
        U16 w86 = GET_WORD(idSector, 86);
        U16 w89 = GET_WORD(idSector, 89);
        U16 w90 = GET_WORD(idSector, 90);
        U16 w119 = GET_WORD(idSector, 119);
        U16 w120 = GET_WORD(idSector, 120);
        U16 w128 = GET_WORD(idSector, 128);
        U16 w234 = GET_WORD(idSector, 234);
        U16 w235 = GET_WORD(idSector, 235);

        sIdentifySectorInfo &p = driveInfo.SectorInfo;

        // SMART Feature
        p.IsSMARTSupported = GET_BIT(w82, 0);
        p.IsSMARTEnabled = GET_BIT(w85, 0);

        // SecureErase Feature
        p.IsSecuritySupported = GET_BIT(w82, 1);
        p.IsUserPasswordPresent = GET_BIT(w85, 1);
        p.IsDeviceLocked = GET_BIT(w128, 2);
        p.IsDeviceFrozen = GET_BIT(w128, 3);
        p.IsPasswordAttemptExceeded = GET_BIT(w128, 4);
        p.IsMasterPasswordMaximum = GET_BIT(w128, 8);
        p.SecureEraseTime = w89;
        p.EnhancedSecureEraseTime = w90;

        // RemovableMedia Feature
        p.IsRemovableSupported = GET_BIT(w82, 2);
        p.IsRemovableEnabled = GET_BIT(w85, 2);


        // PowerManagement Feature
        p.IsPowerSupported = GET_BIT(w82, 3);
        p.IsPowerEnabled = GET_BIT(w85, 3);


        // PacketCommand Feature
        p.IsPacketSupported = GET_BIT(w82, 4);
        p.IsPacketEnabled = GET_BIT(w85, 4);

        // DownloadMicrocode
        p.IsDownloadCodeSupported = GET_BIT(w83, 0) && GET_BIT(w86, 0);
        p.IsTransferMode3Supported = GET_BIT(w119, 4) && GET_BIT(w120, 4);

        p.DownloadSizeMin = w234;
        p.DownloadSizeMax = w235;
    }
}

void DeviceUtil::ParseInquiryInfo(const unsigned char* idSector, sInquiryInfo& info)
{
    if(1) {
        const int wordPos = 4;    // Word Position of VendorName
        const int wordCount = 4;  // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, false);
        info.VendorName = string((char*)buffer);
        StringUtil::Trim(info.VendorName);
    }

    if(1) {
        const int wordPos = 8;   // Word Position of ProductName
        const int wordCount = 8; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, false);
        info.ProductName = string((char*)buffer);
        StringUtil::Trim(info.ProductName);
    }

    if(1) {
        const int wordPos = 16;   // Word Position of RevisionLevel
        const int wordCount = 2; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, false);
        info.RevisionLevel = string((char*)buffer);
        StringUtil::Trim(info.RevisionLevel);
    }

    if(1) {
        const int wordPos = 18;   // Word Position of SerialNumber
        const int wordCount = 4; // Word Count
        unsigned char buffer[wordCount*2 + 1] = {0};
        CoreUtil::CopyWord(buffer, idSector + wordPos * 2, wordCount, false);
        info.SerialNumber = string((char*)buffer);
        StringUtil::Trim(info.SerialNumber);
    }
}

eVendorCode DeviceUtil::GetVendorCode(const U8* idSector)
{
    const U8* pVendor = &idSector[108*2];
    //const U8* pVendorSpecific = &idSector[129*2];
    
    // Testing Virtium device
    /*bool isVirtiumDevice = (    (0x69 == pVendorSpecific[0]) && (0x56 == pVendorSpecific[1])
                             && (0x74 == pVendorSpecific[2]) && (0x72 == pVendorSpecific[3])
                             && (0x75 == pVendorSpecific[4]) && (0x69 == pVendorSpecific[5])
                             && (0x6D == pVendorSpecific[7]));*/

    // Testing Realtek device
    bool isRealtekDevice = (    (0x51 == pVendor[2]) && (0x07 == pVendor[3])
                             && (0x10 == pVendor[4]) && (0x09 == pVendor[5])
                             && (0x10 == pVendor[6]) && (0x4E == pVendor[7]));

    // Testing Toshiba device
    bool isToshibaDevice = (    (0xCE == pVendor[0]) && (0x58 == pVendor[1])
                             && (0x38 == pVendor[3]));

    if (true == isRealtekDevice) { return eRealtekDevice; }
    else if (true == isToshibaDevice) {return eToshibaDevice; }

    return eUnknownVendor;
}
