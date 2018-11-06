#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/misc/HexFormatter.h"

#include "CoreData.h"
#include "CoreUtil.h"
#include "SmartUtil.h"
#include "NvmeData.h"
#include "CommonStruct.h"

map<U8, string> sHiddenAttribute;
map<string, eIdentifyKey> sIdentifyKeyMap;

sAttribute::sAttribute()
{
    reset();
}

void sAttribute::reset()
{
    ID = 0;
    Value = Worst = Threshold = 0;
    LowRaw = HighRaw = 0;
    Name = "Reserved_Attribute";
    Note = "";
}

sIdentifySectorInfo::sIdentifySectorInfo()
{
    reset();
}

void sIdentifySectorInfo::reset()
{
    memset((void*) this, 0x00, sizeof(*this));
}

sIdentifyInfo::sIdentifyInfo()
{
    reset();
}

void sIdentifyInfo::reset()
{
    DriveName = "";
    DeviceModel = "";
    SerialNumber = "";
    FirmwareVersion = "";
    UserCapacity = 0;

    SectorInfo.reset();
}

string sIdentifyInfo::toString() const
{
    stringstream sstr;

    sstr << "Model: " << DeviceModel << endl;
    sstr << "Serial: " << SerialNumber << endl;
    sstr << "Capacity: " << UserCapacity << " GB" << endl;
    sstr << "Firmware: " << FirmwareVersion;

    return sstr.str();
}

string sIdentifyInfo::toFeatureString() const
{
    stringstream sstr;

    const sIdentifySectorInfo &p = SectorInfo;
    sstr << "SMART: "
         << (p.IsSMARTSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsSMARTEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "SECURITY: "
         << (p.IsSecuritySupported ? "Supported" : "Unsupported") << " / "
         << (p.IsUserPasswordPresent ? "Enabled" : "Disabled") << endl;

    sstr << "PACKET COMMAND: "
         << (p.IsPacketSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsPacketEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "REMOVABLE MEDIA: "
         << (p.IsRemovableSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsRemovableEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "POWER MANAGEMENT: "
         << (p.IsPowerSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsPowerEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "DOWNLOAD MICROCODE: "
         << (p.IsDownloadCodeSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsTransferMode3Supported ? "Mode3" : "Mode7") << " / "
         << "MinSize(sector) " << p.DownloadSizeMin << " / "
         << "MaxSize(sector) " << p.DownloadSizeMax << endl;

    return sstr.str();
}

sInquiryInfo::sInquiryInfo()
{
    reset();
}

void sInquiryInfo::reset()
{
    VendorName = "";
    ProductName = "";
    RevisionLevel = "";
    SerialNumber = "";
}

string sInquiryInfo::toString() const
{
    stringstream sstr;

    sstr << "Vendor: " << VendorName << endl;
    sstr << "Serial: " << SerialNumber << endl;
    sstr << "Product: " << ProductName << endl;
    sstr << "Revision: " << RevisionLevel;

    return sstr.str();
}

void sSmartInfo::reset()
{
    AttrMap.clear();
}

sDriveInfo::sDriveInfo()
{
    reset();
}

void sDriveInfo::reset()
{
    CaptureTime = 0;
    SmartInfo.reset();
    IdentifyInfo.reset();
    DriveErrorState = 0;
}

void sDriveInfo::setError(eDriveErrorState state) const
{
    DriveErrorState |= state;
}

sRawDriveInfo::sRawDriveInfo()
{
    reset();
}

void sRawDriveInfo::reset()
{
    DriveName = "";
    CaptureTime = 0;

    FILL_OBJECT(ValueSector, 0x00);
    FILL_OBJECT(ThresholdSector, 0x00);
    FILL_OBJECT(IdentifySector, 0x00);
}

sSlotLifeInfo::sSlotLifeInfo()
{
    reset();
}

void sSlotLifeInfo::reset()
{
    TeraByteWritten = 0;
    AverageLoad = 0;
    EstimateEOL = 0;
    RemainingLife = 0;
}

string sSlotLifeInfo::toString() const
{
    stringstream sstr;

    sstr << "Data Write: " << AverageLoad << " GB/hour" << endl;
    sstr << "Remaining Life: " << RemainingLife << " %" << endl;
    sstr << "Estimate Remaining TBW: " << TeraByteWritten << " TB" << endl;
    sstr << "Estimate Remaining Time: " << MiscUtil::ToTimeString(EstimateEOL, true) << endl;

    return sstr.str();
}

sSlotInfo::sSlotInfo()
{
    reset();
}

void sSlotInfo::reset()
{
    SlotTime = 0;
    Duration = 0;
    LBARead = 0;
    LBAWritten = 0;
    AveEraseCount = 0;
    MaxEraseCount = 0;
    SlotEraseCount = 0;
    RawCapacity = 0;
    Temperature = 0;
    LifeLeft = 0;
    SpareBlock = 0;
    ECCError = 0;
    CRCError = 0;
    ProgramFail = 0;
    EraseFail = 0;
    NandEndurance = 0;

    WriteAmp = 0;
    WearEfficiency = 0;

    ItemState = 0;

    SlotLifeInfo.reset();
}

void sSlotInfo::updateLifeInfo()
{
    sSlotLifeInfo& lifeInfo = SlotLifeInfo;

    U32 roundDuration = Duration;
    // U32 roundDuration = ((Duration + 86400 - 1) / 86400) * 86400;

    double nandEndurance = NandEndurance * LifeLeft / 100.0;

    // Writable capacity:
    double totalCapacityInGB = nandEndurance * RawCapacity;
    // Using WearEfficiency in calculating totalWritten
    // if ( (0 != MaxEraseCount) && (0 != LBAWritten))  totalCapacityInGB = (nandEndurance * LBAWritten) / (MaxEraseCount * 32.0);
    
    // Skip WearEfficiency in calculating totalWritten
    if ( (0 != SlotEraseCount) && (0 != LBAWritten)) totalCapacityInGB = (nandEndurance * LBAWritten) / (SlotEraseCount * 32.0);

    lifeInfo.TeraByteWritten = MiscUtil::RoundPrecision(totalCapacityInGB / 1024);

    lifeInfo.AverageLoad = (((double)LBAWritten * 112.5)) / (roundDuration); // AverageLoad per hour
    lifeInfo.AverageLoad = ((int) (lifeInfo.AverageLoad * 100)) / 100.0;

    // Calculate lifetime:
    U32 validLBAWritten = (0 == LBAWritten) ? 1 : LBAWritten;
    double durationInDay = (double) roundDuration / (3600 * 24);
    double writtenInGB = (double) validLBAWritten / 32;
    double writtenInDay = writtenInGB / durationInDay;
    lifeInfo.EstimateEOL = (int) (totalCapacityInGB /writtenInDay);

    // RemainingLife read from SMART
    lifeInfo.RemainingLife = LifeLeft;
}

bool sSlotInfo::getData(eDataKey key, pair<U32, U32>& value) const
{
    switch(key)
    {
        #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v0; value.second = w0; break;
        #include "DataKey.def"
        #undef MAP_ITEM
        default: return false;
    }

    return true;
}

bool sSlotInfo::getData(eDataKey key, U32 dataSetIndex, pair<U32, U32>& value) const
{
    if (dataSetIndex == 0)
    {
        switch(key)
        {
            #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v0; value.second = w0; break;
            #include "DataKey.def"
            #undef MAP_ITEM
            default: return false;
        }
    }

    if (dataSetIndex == 1)
    {
        switch(key)
        {
            #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v1; value.second = w1; break;
            #include "DataKey.def"
            #undef MAP_ITEM
            default: return false;
        }
    }

    return true;
}

sLifeInfo::sLifeInfo()
{
    reset();
}

void sLifeInfo::reset()
{
    SlotArray.clear();

    LastLifeInfo.reset();
}

void sLifeInfo::calcSlotInfo(int minIndex, int maxIndex, sSlotInfo &diffSlot) const
{
    #define SET_ITEM(diff, curr, field) diff.field  = curr.field
    #define ADD_ITEM(diff, curr, field) diff.field += curr.field
    #define SUB_ITEM(diff, curr, field) diff.field -= curr.field

    diffSlot.reset();

    if (minIndex == maxIndex)
    {
        diffSlot = SlotArray[minIndex];
    }
    else
    {
        // Accumulate all sSlotInfo in ranges [minIndex, maxIndex]

        ASSERT (minIndex < maxIndex);

        const sSlotInfo& maxSlot = SlotArray[maxIndex];

        SET_ITEM(diffSlot, maxSlot, SlotTime);
        SET_ITEM(diffSlot, maxSlot, LifeLeft);
        SET_ITEM(diffSlot, maxSlot, SpareBlock);
        SET_ITEM(diffSlot, maxSlot, RawCapacity);
        SET_ITEM(diffSlot, maxSlot, NandEndurance);
        SET_ITEM(diffSlot, maxSlot, AveEraseCount);
        SET_ITEM(diffSlot, maxSlot, MaxEraseCount);
        SET_ITEM(diffSlot, maxSlot, WearEfficiency);

        for (int i = minIndex; i <= maxIndex; i++)
        {
            const sSlotInfo& currSlot = SlotArray[i];

            // Accumulate these fields
            ADD_ITEM(diffSlot, currSlot, Duration);
            ADD_ITEM(diffSlot, currSlot, LBARead);
            ADD_ITEM(diffSlot, currSlot, LBAWritten);
            ADD_ITEM(diffSlot, currSlot, SlotEraseCount);
            ADD_ITEM(diffSlot, currSlot, CRCError);
            ADD_ITEM(diffSlot, currSlot, ECCError);
            ADD_ITEM(diffSlot, currSlot, ProgramFail);
            ADD_ITEM(diffSlot, currSlot, EraseFail);
        }

        diffSlot.WriteAmp = (U32) (((double)diffSlot.RawCapacity * 1024 * diffSlot.SlotEraseCount * 100) / (diffSlot.LBAWritten * 32));

        diffSlot.updateLifeInfo();
    }

    #undef SET_ITEM
    #undef ADD_ITEM
    #undef SUB_ITEM
}

void sLifeInfo::calcLifeInfo(int minIndex, int maxIndex, sSlotLifeInfo &lifeInfo) const
{
    sSlotInfo diffSlot;

    calcSlotInfo(minIndex, maxIndex, diffSlot);
    lifeInfo = diffSlot.SlotLifeInfo;
}

void sLifeInfo::getData(eDataKey key, vector<pair<double, double> > &valueArray) const
{
    valueArray.clear();

    int slotCount = SlotArray.size();
    for (int i = 0; i < slotCount; i++)
    {
        pair<U32, U32> unsignedData;
        pair<double, double> doubleData;
        const sSlotInfo& slot = SlotArray[i];

        if (true == slot.getData(key, unsignedData))
        {
            doubleData.first = (int) unsignedData.first;
            doubleData.second = (int) unsignedData.second;
        }

        valueArray.push_back(doubleData);
    }
}

void sLifeInfo::getData(eDataKey key, vector<vector<pair<double, double> > >&valueArray) const
{
    // for each dataset in key, create an data_array

    valueArray.clear();

    int dataSetCount = DataKeyUtil::GetDataSetCount(key);

    for (int dataSet = 0; dataSet < dataSetCount; dataSet++)
    {
        vector<pair<double, double> > dataArray;

        int slotCount = SlotArray.size();
        for (int i = 0; i < slotCount; i++)
        {
            pair<U32, U32> unsignedData;
            pair<double, double> doubleData;
            const sSlotInfo& slot = SlotArray[i];

            if (true == slot.getData(key, dataSet, unsignedData))
            {
                doubleData.first = (int) unsignedData.first;
                doubleData.second = (int) unsignedData.second;
            }

            dataArray.push_back(doubleData);
        }

        valueArray.push_back(dataArray);
    }
}

sDiffInfo::sDiffInfo()
{
    reset();
}

void sDiffInfo::reset()
{
    SlotArray.clear();
}

void sDiffInfo::getData(eDataKey key, vector<pair<double, double> > &valueArray) const
{
    valueArray.clear();

    int slotCount = SlotArray.size();
    for (int i = 0; i < slotCount; i++)
    {
        pair<U32, U32> unsignedData;
        pair<double, double> doubleData;
        const sSlotInfo& slot = SlotArray[i];

        if (true == slot.getData(key, unsignedData))
        {
            doubleData.first = (int) unsignedData.first;
            doubleData.second = (int) unsignedData.second;
        }

        valueArray.push_back(doubleData);
    }
}

void sDiffInfo::getData(eDataKey key, vector<vector<pair<double, double> > >&valueArray) const
{
    // for each dataset in key, create an data_array

    valueArray.clear();

    int dataSetCount = DataKeyUtil::GetDataSetCount(key);

    for (int dataSet = 0; dataSet < dataSetCount; dataSet++)
    {
        vector<pair<double, double> > dataArray;

        int slotCount = SlotArray.size();
        for (int i = 0; i < slotCount; i++)
        {
            pair<U32, U32> unsignedData;
            pair<double, double> doubleData;
            const sSlotInfo& slot = SlotArray[i];

            if (true == slot.getData(key, dataSet, unsignedData))
            {
                doubleData.first = (int) unsignedData.first;
                doubleData.second = (int) unsignedData.second;
            }

            dataArray.push_back(doubleData);
        }

        valueArray.push_back(dataArray);
    }
}

void sDeviceInfo::getSlotData(eDataKey code, vector<pair<double, double> > &valueArray) const
{
    LifeInfo.getData(code, valueArray);
}

void sDeviceInfo::getDiffData(eDataKey code, vector<pair<double, double> > &valueArray) const
{
    DiffInfo.getData(code, valueArray);
}

void sDeviceInfo::getSlotData(eDataKey code, vector<vector<pair<double, double> > >&valueArray) const
{
    LifeInfo.getData(code, valueArray);
}

void sDeviceInfo::getDiffData(eDataKey code, vector<vector<pair<double, double> > >&valueArray) const
{
    DiffInfo.getData(code, valueArray);
}

void sDeviceData::reset()
{
    DeviceData.clear();

    SerialMap.clear();
    SerialIndex = 9999;
}

// -----------------------------------------------------------------
// Function for handling core data
// -----------------------------------------------------------------

// Load one drive info from history file
bool LoadDrive(const string& str, sDriveInfo& drive, bool autoCorrect)
{
    string line;
    stringstream sstr(str);

    const char sep = ';';

    bool status = false;

    drive.reset();
    sSmartInfo& smart = drive.SmartInfo;
    sIdentifyInfo& identify = drive.IdentifyInfo;

    do {
        // SerialNumber
        if (getline(sstr, line, sep).eof()) break;
        identify.SerialNumber = line;

        // CaptureTime
        if (getline(sstr, line, sep).eof()) break;
        drive.CaptureTime = atoi(line.c_str());

        // DriveName
        if (getline(sstr, line, sep).eof()) break;
        identify.DriveName = line;

        // DeviceModel
        if (getline(sstr, line, sep).eof()) break;
        identify.DeviceModel = line;

        // 2nd SerialNumber
        if (getline(sstr, line, sep).eof()) break;
        if(0 != identify.SerialNumber.compare(line)) break;

        // FirmwareVersion
        if (getline(sstr, line, sep).eof()) break;
        identify.FirmwareVersion = line;

        // UserCapacity
        if (getline(sstr, line, sep).eof()) break;
        identify.UserCapacity = atof(line.c_str());
        identify.UserCapacity = ((int) (identify.UserCapacity * 100)) / 100.0; // minor truncating

        // Array of attributes
        while(getline(sstr, line, sep))
        {
            sAttribute attr;

            attr.ID = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Name = line;

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.LowRaw = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Value = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Worst = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Threshold = atoi(line.c_str());

            // Change name if needed
            // string name, note;
            // if (true == CoreUtil::LookupAttributeText(attr.ID, name, note))
            // {
            //     attr.Name = name;
            //     attr.Note = note;
            // }

            // Validate ID after scanning all necessary attributes
            if (false == CoreUtil::ValidateAttributeID(attr.ID)) continue;

            // Correct value of some attributes (temperature, ..)
            SmartUtil::CorrectAttribute(attr);

            smart.AttrMap[attr.ID] = attr;
        }

        if(true == autoCorrect) 
        {
            SmartUtil::ParseSmartData(sSmartData(), smart);
        }

        status = true;
    } while(0);

    if(true == status) AdaptDrive(drive);

    return status;
}

void AdaptDrive(sDriveInfo& info)
{
    // Change attribute names if needed
    tAttributeMap& smartMap = info.SmartInfo.AttrMap;

    string name, note;
    tAttributeMap::iterator iter;
    for (iter = smartMap.begin(); iter != smartMap.end(); iter++)
    {
        sAttribute& attr = iter->second;

        attr.Name = "Reserved_Attribute";
        if (true == CoreUtil::LookupAttributeText(attr.ID, name, note))
        {
            attr.Name = name;
            attr.Note = note;
        }
    }
}

// Add one drive into DeviceList
void AddDrive(sDeviceData& data, const sDriveInfo& inputInfo)
{
    sDriveInfo info = inputInfo;
    AdaptDrive(info);

    // Add drive into device
    string serialNum = info.IdentifyInfo.SerialNumber;
    map<string, sDeviceInfo>::iterator iter = data.DeviceData.find(serialNum);

    if (iter != data.DeviceData.end())
    {
        sDeviceInfo& device = data.DeviceData[serialNum];
        device.DriveSet.insert(info);
    }
    else
    {
        sDeviceInfo device;
        device.SerialNumber = serialNum;
        device.DriveSet.insert(info);

        data.DeviceData[serialNum] = device;
    }
}

// Refine and Add one drive into DeviceList
void RefineDrive(sDeviceData& data, const sDriveInfo& inputInfo)
{
    sDriveInfo info = inputInfo;
    AdaptDrive(info);

    string currSerialNumber = info.IdentifyInfo.SerialNumber;
    int currCapacity = (int) info.IdentifyInfo.UserCapacity;

    tSerialMap::iterator serialIter = data.SerialMap.find(currSerialNumber);
    if (serialIter == data.SerialMap.end())
    {
        // First time found this serial in input data
        tCapacityMap capMap;
        capMap[currCapacity] = currSerialNumber;

        data.SerialMap[currSerialNumber] = capMap;
    }
    else
    {
        tCapacityMap& capMap = serialIter->second;
        tCapacityMap::iterator capIter = capMap.find(currCapacity);
        if (capIter == capMap.end())
        {
            // Create new serial
            string newSerialNumber = currSerialNumber;
            stringstream sstr; sstr << setw(4) << setfill('0') << data.SerialIndex;
            string newTail = sstr.str();
            data.SerialIndex--;

            // Modify the serial number
            int currSerialLength = newSerialNumber.length();
            int tailLength = newTail.length();
            currSerialLength = currSerialLength - tailLength;
            for (int i = 0; i < tailLength; i++)
            {
                newSerialNumber[currSerialLength + i] = newTail[i];
            }

            // Add new pair to capMap
            capMap[currCapacity] = newSerialNumber;
            info.IdentifyInfo.SerialNumber = newSerialNumber;
        }
        else
        {
            // Use existing SerialNumber
            info.IdentifyInfo.SerialNumber = capIter->second;
        }
    }

    // Add drive into device
    string serialNumber = info.IdentifyInfo.SerialNumber;
    map<string, sDeviceInfo>::iterator iter = data.DeviceData.find(serialNumber);

    if (iter != data.DeviceData.end())
    {
        sDeviceInfo& device = data.DeviceData[serialNumber];
        device.DriveSet.insert(info);
    }
    else
    {
        sDeviceInfo device;
        device.SerialNumber = serialNumber;
        device.DriveSet.insert(info);

        data.DeviceData[serialNumber] = device;
    }
}

// Update LifeInfo of single device
void UpdateLifeInfo(sDeviceInfo& devInfo)
{
    sLifeInfo& lifeInfo = devInfo.LifeInfo;
    tDriveSet& driveSet = devInfo.DriveSet;

    lifeInfo.reset();

    sSlotInfo prevSmart, currSmart, lastSmart, diffSlot;
    U32 defaultNandEndurance = CoreUtil::LookupNandEndurance(devInfo.SerialNumber);

    tDriveSet::iterator iter;
    for (iter = driveSet.begin(); iter != driveSet.end(); iter++)
    {
        const sDriveInfo& driveInfo = *iter;
        const sSmartInfo& smartInfo = driveInfo.SmartInfo;

        currSmart.reset();

        // Read attribute from currSmart;

        // Mandatory attributes
        #define READ_MANDATORY_ATTRIBUTE(attrID, attrValue, errCode) if (false == ReadAttribute(smartInfo, attrID, attrValue)) { driveInfo.setError(errCode); continue; }
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_READ, currSmart.LBARead, DRIVE_LACK_LBA_READ);
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_WRITTEN, currSmart.LBAWritten, DRIVE_LACK_LBA_WRITTEN);
        READ_MANDATORY_ATTRIBUTE(ATTR_AVE_ERASE, currSmart.AveEraseCount, DRIVE_LACK_AVE_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_MAX_ERASE, currSmart.MaxEraseCount, DRIVE_LACK_MAX_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_TEMPERATURE, currSmart.Temperature, DRIVE_LACK_TEMPERATURE);
        #undef READ_MANDATORY_ATTRIBUTE

        currSmart.SlotTime = driveInfo.CaptureTime;

        // double userCapInGB = (driveInfo.IdentifyInfo.UserCapacity * 93.13) / 100.0; // (convert from 1000 to 1024 unit)
        double userCapInGB = driveInfo.IdentifyInfo.UserCapacity;                     // Dont need to convert to 1024 unit
        currSmart.RawCapacity = MiscUtil::RoundUp(userCapInGB);                       // Rounding to next Power_Of_2

        // Verify attributes
        #define VERIFY_MANDATORY_ATTRIBUTE(errCond, errCode) if (errCond) { driveInfo.setError(errCode); continue; }
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.SlotTime >= currSmart.SlotTime, DRIVE_INVALID_SLOT_TIME);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBARead > currSmart.LBARead, DRIVE_INVALID_LBA_READ);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBAWritten > currSmart.LBAWritten, DRIVE_INVALID_LBA_WRITTEN);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.AveEraseCount > currSmart.AveEraseCount, DRIVE_INVALID_AVE_ERASE);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.MaxEraseCount >  currSmart.MaxEraseCount, DRIVE_INVALID_MAX_ERASE);
        #undef VERIFY_MANDATORY_ATTRIBUTE

        currSmart.ItemState |= MANDATORY_ITEM;

        // Optional attributes
        #define READ_OPTIONAL_ATTRIBUTE(attrID, attrValue, optionCode) if (true == ReadAttribute(smartInfo, attrID, attrValue)) { currSmart.ItemState |= optionCode; }
        READ_OPTIONAL_ATTRIBUTE(ATTR_LIFE_LEFT,      currSmart.LifeLeft     , OPTIONAL_ITEM_LIFELEFT      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_SPARE_BLOCK,    currSmart.SpareBlock   , OPTIONAL_ITEM_SPAREBLOCK    );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ECC_ERROR,      currSmart.ECCError     , OPTIONAL_ITEM_ECCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_CRC_ERROR,      currSmart.CRCError     , OPTIONAL_ITEM_CRCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_PROGRAM_FAIL,   currSmart.ProgramFail  , OPTIONAL_ITEM_PROGRAMFAIL   );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ERASE_FAIL,     currSmart.EraseFail    , OPTIONAL_ITEM_ERASEFAIL     );
        READ_OPTIONAL_ATTRIBUTE(ATTR_NAND_ENDURANCE, currSmart.NandEndurance, OPTIONAL_ITEM_NANDENDURANCE );
        #undef READ_OPTIONAL_ATTRIBUTE

        int optionState = currSmart.ItemState & prevSmart.ItemState;

        #define VERIFY_OPTIONAL_ATTRIBUTE(errCond, optionCode, errCode) if ((optionState & optionCode) && (errCond)) { driveInfo.setError(errCode); continue; }
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.NandEndurance != currSmart.NandEndurance, OPTIONAL_ITEM_NANDENDURANCE, DRIVE_INVALID_NANDENDURANCE);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.CRCError > currSmart.CRCError, OPTIONAL_ITEM_CRCERROR, DRIVE_INVALID_CRCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ECCError > currSmart.ECCError, OPTIONAL_ITEM_ECCERROR, DRIVE_INVALID_ECCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ProgramFail > currSmart.ProgramFail, OPTIONAL_ITEM_PROGRAMFAIL, DRIVE_INVALID_PROGRAMFAIL);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.EraseFail > currSmart.EraseFail, OPTIONAL_ITEM_ERASEFAIL, DRIVE_INVALID_ERASEFAIL);
        #undef VERIFY_OPTIONAL_ATTRIBUTE

        // Calculate different between currSmart and prevSmart
        do {
            if (0 == (prevSmart.ItemState & MANDATORY_ITEM)) break; // Skip the first record
            if(currSmart.ItemState != prevSmart.ItemState) break;   // Different in optional fields

            diffSlot = currSmart;
            diffSlot.Duration = currSmart.SlotTime - prevSmart.SlotTime;
            diffSlot.LBARead -= prevSmart.LBARead;
            diffSlot.LBAWritten -= prevSmart.LBAWritten;
            diffSlot.SlotEraseCount = currSmart.AveEraseCount - prevSmart.AveEraseCount;

            if( optionState & OPTIONAL_ITEM_CRCERROR ) diffSlot.CRCError -= prevSmart.CRCError;
            if( optionState & OPTIONAL_ITEM_ECCERROR ) diffSlot.ECCError -= prevSmart.ECCError;
            if( optionState & OPTIONAL_ITEM_PROGRAMFAIL) diffSlot.ProgramFail -= prevSmart.ProgramFail;
            if( optionState & OPTIONAL_ITEM_ERASEFAIL) diffSlot.EraseFail -= prevSmart.EraseFail;

            // Set default nandEndurance
            if( !(optionState & OPTIONAL_ITEM_NANDENDURANCE)) diffSlot.NandEndurance = defaultNandEndurance;

            diffSlot.WearEfficiency = 0;
            if (0 != currSmart.MaxEraseCount)
            {
                diffSlot.WearEfficiency = (double) currSmart.AveEraseCount / (double) currSmart.MaxEraseCount;
            }

            diffSlot.WriteAmp = 0;
            if (0 != diffSlot.LBAWritten)
            {
                diffSlot.WriteAmp = (U32) (((double)diffSlot.RawCapacity * 1024 * diffSlot.SlotEraseCount * 100) / (diffSlot.LBAWritten * 32));
            }

            diffSlot.updateLifeInfo();

            lifeInfo.SlotArray.push_back(diffSlot);
        } while(0);

        prevSmart = currSmart;
    }

    int slotCount = lifeInfo.SlotArray.size();
    if (0 != slotCount)
    {
        sSlotInfo& lastSlot = lifeInfo.SlotArray[slotCount - 1];        // Last slot
        lifeInfo.LastLifeInfo = lastSlot.SlotLifeInfo;
    }
}
// Update DiffInfo of single device
void UpdateDiffInfo(sDeviceInfo& devInfo)
{
    sDiffInfo& diffInfo = devInfo.DiffInfo;
    tDriveSet& driveSet = devInfo.DriveSet;

    diffInfo.reset();

    sSlotInfo prevSmart, currSmart, lastSmart, diffSlot;
    U32 defaultNandEndurance = CoreUtil::LookupNandEndurance(devInfo.SerialNumber);

    tDriveSet::iterator iter;
    for (iter = driveSet.begin(); iter != driveSet.end(); iter++)
    {
        const sDriveInfo& driveInfo = *iter;
        const sSmartInfo& smartInfo = driveInfo.SmartInfo;

        currSmart.reset();

        // Read attribute from currSmart;

        // Mandatory attributes
        #define READ_MANDATORY_ATTRIBUTE(attrID, attrValue, errCode) if (false == ReadAttribute(smartInfo, attrID, attrValue)) { driveInfo.setError(errCode); continue; }
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_READ, currSmart.LBARead, DRIVE_LACK_LBA_READ);
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_WRITTEN, currSmart.LBAWritten, DRIVE_LACK_LBA_WRITTEN);
        READ_MANDATORY_ATTRIBUTE(ATTR_AVE_ERASE, currSmart.AveEraseCount, DRIVE_LACK_AVE_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_MAX_ERASE, currSmart.MaxEraseCount, DRIVE_LACK_MAX_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_TEMPERATURE, currSmart.Temperature, DRIVE_LACK_TEMPERATURE);
        #undef READ_MANDATORY_ATTRIBUTE

        currSmart.SlotTime = driveInfo.CaptureTime;

        // double userCapInGB = (driveInfo.IdentifyInfo.UserCapacity * 93.13) / 100.0; // (convert from 1000 to 1024 unit)
        double userCapInGB = driveInfo.IdentifyInfo.UserCapacity;                     // Dont need to convert to 1024 unit
        currSmart.RawCapacity = MiscUtil::RoundUp(userCapInGB);                       // Rounding to next Power_Of_2

        // Verify attributes
        #define VERIFY_MANDATORY_ATTRIBUTE(errCond, errCode) if (errCond) { driveInfo.setError(errCode); continue; }
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.SlotTime >= currSmart.SlotTime, DRIVE_INVALID_SLOT_TIME);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBARead > currSmart.LBARead, DRIVE_INVALID_LBA_READ);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBAWritten > currSmart.LBAWritten, DRIVE_INVALID_LBA_WRITTEN);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.AveEraseCount > currSmart.AveEraseCount, DRIVE_INVALID_AVE_ERASE);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.MaxEraseCount >  currSmart.MaxEraseCount, DRIVE_INVALID_MAX_ERASE);
        #undef VERIFY_MANDATORY_ATTRIBUTE

        currSmart.ItemState |= MANDATORY_ITEM;

        // Optional attributes
        #define READ_OPTIONAL_ATTRIBUTE(attrID, attrValue, optionCode) if (true == ReadAttribute(smartInfo, attrID, attrValue)) { currSmart.ItemState |= optionCode; }
        READ_OPTIONAL_ATTRIBUTE(ATTR_LIFE_LEFT,      currSmart.LifeLeft     , OPTIONAL_ITEM_LIFELEFT      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_SPARE_BLOCK,    currSmart.SpareBlock   , OPTIONAL_ITEM_SPAREBLOCK    );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ECC_ERROR,      currSmart.ECCError     , OPTIONAL_ITEM_ECCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_CRC_ERROR,      currSmart.CRCError     , OPTIONAL_ITEM_CRCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_PROGRAM_FAIL,   currSmart.ProgramFail  , OPTIONAL_ITEM_PROGRAMFAIL   );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ERASE_FAIL,     currSmart.EraseFail    , OPTIONAL_ITEM_ERASEFAIL     );
        #undef READ_OPTIONAL_ATTRIBUTE

        int optionState = currSmart.ItemState & prevSmart.ItemState;

        #define VERIFY_OPTIONAL_ATTRIBUTE(errCond, optionCode, errCode) if ((optionState & optionCode) && (errCond)) { driveInfo.setError(errCode); continue; }
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.CRCError > currSmart.CRCError, OPTIONAL_ITEM_CRCERROR, DRIVE_INVALID_CRCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ECCError > currSmart.ECCError, OPTIONAL_ITEM_ECCERROR, DRIVE_INVALID_ECCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ProgramFail > currSmart.ProgramFail, OPTIONAL_ITEM_PROGRAMFAIL, DRIVE_INVALID_PROGRAMFAIL);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.EraseFail > currSmart.EraseFail, OPTIONAL_ITEM_ERASEFAIL, DRIVE_INVALID_ERASEFAIL);
        #undef VERIFY_OPTIONAL_ATTRIBUTE

        // Calculate different between currSmart and prevSmart
        do {
            if (0 == (prevSmart.ItemState & MANDATORY_ITEM)) break; // Skip the first record
            if(currSmart.ItemState != prevSmart.ItemState) break;   // Different in optional fields

            diffSlot = currSmart;
            diffSlot.Duration = currSmart.SlotTime - prevSmart.SlotTime;
            diffSlot.LBARead -= prevSmart.LBARead;
            diffSlot.LBAWritten -= prevSmart.LBAWritten;
            diffSlot.SlotEraseCount = currSmart.AveEraseCount - prevSmart.AveEraseCount;

            if( optionState & OPTIONAL_ITEM_CRCERROR ) diffSlot.CRCError -= prevSmart.CRCError;
            if( optionState & OPTIONAL_ITEM_ECCERROR ) diffSlot.ECCError -= prevSmart.ECCError;
            if( optionState & OPTIONAL_ITEM_PROGRAMFAIL) diffSlot.ProgramFail -= prevSmart.ProgramFail;
            if( optionState & OPTIONAL_ITEM_ERASEFAIL) diffSlot.EraseFail -= prevSmart.EraseFail;

            diffInfo.SlotArray.push_back(diffSlot);
        } while(0);

        prevSmart = currSmart;
    }
}

// Update Slot info on sDeviceData
void UpdateSlotInfo(sDeviceData& data)
{
    tDeviceMap::iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++)
    {
        UpdateLifeInfo(iter->second);
        UpdateDiffInfo(iter->second);
    }
}

void UpdateDrive(sDeviceData &deviceData, const sDriveInfo& drive)
{
    AddDrive(deviceData, drive);
    UpdateSlotInfo(deviceData);
}

void UpdateDrive(sDeviceData &deviceData, const list<sDriveInfo> &driveList)
{
    list<sDriveInfo>::const_iterator iter;
    for (iter = driveList.begin(); iter != driveList.end(); iter++)
    {
        AddDrive(deviceData, *iter);
    }

    UpdateSlotInfo(deviceData);
}

// Load all drives from history file
bool LoadHistory(sDeviceData& data, const string& filename)
{
    data.DeviceData.clear();

    string line;
    fstream fstr(filename.c_str());
    if(false == fstr.is_open()) return false;

    while(getline(fstr, line, '\n'))
    {
        sDriveInfo info;
        if(true == LoadDrive(line, info)) AddDrive(data, info);
    }

    // Update LifeInfo of devices
    UpdateSlotInfo(data);

    return true;
}

// Refine drives from history file. Change SerialNum if needed
bool RefineHistory(sDeviceData& data, const string& filename)
{
    string line;
    ifstream fstr(filename.c_str());

    data.DeviceData.clear();

    while(getline(fstr, line, '\n'))
    {
        sDriveInfo info;
        if(true == LoadDrive(line, info)) RefineDrive(data, info);
    }

    // Update LifeInfo of devices
    UpdateSlotInfo(data);

    return true;
}

bool SaveHistory(const sDeviceData& data, const string& filename)
{
    ofstream fstr(filename.c_str());
    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHistory(const list<sDriveInfo>& data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHistory(const NvmeDevice &data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHeader(const sHeaderLogData &data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHistory(const list<sRawDriveInfo>& data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool SaveLifeInfo(const sDeviceInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    // Common information:
    fstr << "Device: " << info.SerialNumber << endl;
    fstr << ToString(info.LifeInfo);
    fstr.close();

    return true;
}

bool SaveDeviceInfo(const sDeviceInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    fstr << ToVerboseString(info);
    fstr.close();

    return true;
}

bool SaveDriveInfo(const sDriveInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    fstr << ToVerboseString(info);
    fstr.close();

    return true;
}

U8 GetSubKey(U8 mainKey)
{
    U8 subKey = 0;

    switch (mainKey)
    {
    case 169: subKey = 248; break; // Remaining-Life-Left
    case 248: subKey = 169; break;
    default: break;
    }

    return subKey;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, double& value)
{
    U32 lowRaw = 0, highRaw = 0;
    if (false == ReadAttribute(info, key, lowRaw, highRaw)) return false;

    value = (double) lowRaw;

    return true;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, U32& value)
{
    U32 lowRaw = 0, highRaw = 0;
    if (false == ReadAttribute(info, key, lowRaw, highRaw)) return false;

    value = lowRaw;

    return true;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, U32& low, U32& high)
{
    map<U8, sAttribute>::const_iterator iter = info.AttrMap.find(key);
    if(info.AttrMap.end() == iter)
    {
        U8 subkey = GetSubKey(key);
        if(0 != subkey) iter = info.AttrMap.find(subkey); // second chance on this key:

        if(info.AttrMap.end() == iter) return false;
    }

    const sAttribute& attr = iter->second;

    low = attr.LowRaw;
    high = attr.HighRaw;

    return true;
}

bool ReadAttributeValue(const sSmartInfo& info, U8 key, U32& value)
{
    map<U8, sAttribute>::const_iterator iter = info.AttrMap.find(key);
    if(info.AttrMap.end() == iter)
    {
        U8 subkey = GetSubKey(key);
        if(0 != subkey) iter = info.AttrMap.find(subkey); // second chance on this key:

        if(info.AttrMap.end() == iter) return false;
    }

    const sAttribute& attr = iter->second;

    value = attr.Value;

    return true;
}

eNandType GetNandType(const sSmartInfo &info, const string &serialNo)
{
    U32 nandEndurance = 0;

    if (false == ReadAttribute(info, ATTR_NAND_ENDURANCE, nandEndurance))
    {
        // Using default value from device model:
        nandEndurance = CoreUtil::LookupNandEndurance(serialNo);
    }

    eNandType nandType = NAND_SLC;

    if (nandEndurance <= 10000) nandType = NAND_MLC;

    return nandType;
}

sDeviceInfo* GetDevicePtr(sDeviceData &data, unsigned int offset)
{
    unsigned int index = 0;
    tDeviceMap::iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++, index++)
    {
        if(index == offset)
        {
            return &iter->second;
        }
    }

    return NULL;
}

sDeviceInfo* GetDevicePtr(sDeviceData &data, const string& serialNum)
{
    tDeviceMap::iterator iter = data.DeviceData.find(serialNum);
    return (iter != data.DeviceData.end()) ? &iter->second : NULL;
}

const sDriveInfo* GetDrivePtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sDriveInfo* info = NULL;

    unsigned int driveCount = device.DriveSet.size();
    if (driveCount != 0)
    {
        tDriveSet::iterator iter;
        if(offset < driveCount)
        {
            iter = device.DriveSet.begin();
            for (unsigned int i = 0; i < offset; i++, iter++);
        }
        else
        {
            iter = device.DriveSet.end();
            iter--;
        }

        info = &(*iter);
    }

    return info;
}

const sSmartInfo* GetSmartPtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sSmartInfo* info = NULL;

    const sDriveInfo* drivePtr = GetDrivePtr(device, offset);
    if (NULL != drivePtr) info = &drivePtr->SmartInfo;

    return info;
}

const sIdentifyInfo* GetIdentifyPtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sIdentifyInfo* info = NULL;

    const sDriveInfo* drivePtr = GetDrivePtr(device, offset);
    if (NULL != drivePtr) info = &drivePtr->IdentifyInfo;

    return info;
}

const sSmartInfo* GetLastSmartPtr(const sDeviceInfo& device)
{
    return GetSmartPtr(device, device.DriveSet.size());
}

const sIdentifyInfo* GetLastIdentifyPtr(const sDeviceInfo& device)
{
    return GetIdentifyPtr(device, device.DriveSet.size());
}

const sDriveInfo* GetLastDrivePtr(const sDeviceInfo& device)
{
    return GetDrivePtr(device, device.DriveSet.size());
}

string ToString(const sSlotInfo& slot, bool showHeader)
{
    stringstream sstr;

    const char* sep = ";";

    tHeaderItem itemList[] = {
        tHeaderItem("Time", 0),
        tHeaderItem("Duration", 0),
        tHeaderItem("RawCap(G)", 0),
        tHeaderItem("LBA(32M)", 0),
        tHeaderItem("AveEr", 0),
        tHeaderItem("MaxEr", 0),
        tHeaderItem("Temp(C)", 0),
        tHeaderItem("Life(%)", 0),
        tHeaderItem("Spare(%)", 0),
        tHeaderItem("WriteAmp", 0),
    };

    MakeHeaderList(itemList, header);
    if (true == showHeader) sstr << header.ToString(";") << endl;

    // Dump contents
    if (slot.ItemState & MANDATORY_ITEM)
    {
        sstr << FRMT_U32(header.Length(0), slot.SlotTime) << sep;
        sstr << FRMT_U32(header.Length(1), slot.Duration) << sep;
        sstr << FRMT_U32(header.Length(2), slot.RawCapacity) << sep;
        sstr << FRMT_U32(header.Length(3), slot.LBAWritten) << sep;
        sstr << FRMT_U32(header.Length(4), slot.AveEraseCount) << sep;
        sstr << FRMT_U32(header.Length(5), slot.MaxEraseCount) << sep;
        sstr << FRMT_U32(header.Length(6), slot.Temperature) << sep;

        if (slot.ItemState & OPTIONAL_ITEM_LIFELEFT) sstr << FRMT_U32(header.Length(7), slot.LifeLeft);
        sstr << sep;

        if (slot.ItemState & OPTIONAL_ITEM_SPAREBLOCK) sstr << FRMT_U32(header.Length(8), slot.SpareBlock);
        sstr << sep;

        sstr << FRMT_DBL(header.Length(9), slot.WriteAmp / 100.0) << sep;
    }

    return sstr.str();
}

string ToString(const sLifeInfo& info)
{
    stringstream sstr;

    sSlotLifeInfo life = info.LastLifeInfo;

    sstr << "TBW: " << life.TeraByteWritten << endl;
    sstr << "EstimateEOL: " << life.EstimateEOL << endl;

    // Dump contents
    U32 slotCount = info.SlotArray.size();
    for (U32 i = 0; i < slotCount; i++)
    {
        const sSlotInfo& slot = info.SlotArray[i];
        sstr << ToString(slot, (i == 0)) << endl;
    }

    return sstr.str();
}

string ToString(const sSmartInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    tAttributeMap::const_iterator iter;
    for (iter = info.AttrMap.begin(); iter != info.AttrMap.end(); ++iter)
    {
        const sAttribute& attr = iter->second;

        ASSERT (iter->first == attr.ID);

        sstr
            << (U32) attr.ID  << sep
            << attr.Name      << sep
            << (U32) attr.HighRaw << sep
            << (U32) attr.LowRaw    << sep
            << (U32) attr.Value     << sep
            << (U32) attr.Worst     << sep
            << (U32) attr.Threshold << sep;
    }

    return sstr.str();
}

string ToString(const sIdentifyInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr
        << info.DriveName       << sep
        << info.DeviceModel     << sep
        << info.SerialNumber    << sep
        << info.FirmwareVersion << sep
        << info.UserCapacity    << sep;

    return sstr.str();
}

string ToString(const sDriveInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr << "log;";
    sstr
        << info.IdentifyInfo.SerialNumber << sep
        << info.CaptureTime << sep;

    sstr << ToString(info.IdentifyInfo);
    sstr << ToString(info.SmartInfo);

    return sstr.str();
}

string ToString(const sRawDriveInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr
        << info.CaptureTime << sep
        << info.DriveName << sep;

    sstr << endl;

    // Identify sector
    // HexFormatter frmt;
    // frmt.setSize(512, 1);
    // frmt.setState(true, false, false);
    // sstr << frmt.toString(info.IdentifySector, 512);
    // sstr << frmt.toString(info.ValueSector, 512);
    // sstr << frmt.toString(info.ThresholdSector, 512);

    sstr << HexFormatter::ToHexString(info.IdentifySector, 512) << endl;
    sstr << HexFormatter::ToHexString(info.ValueSector, 512) << endl;
    sstr << HexFormatter::ToHexString(info.ThresholdSector, 512) << endl;

    return sstr.str();
}

string ToString(const list<sDriveInfo>& info)
{
    stringstream sstr;

    list<sDriveInfo>::const_iterator iter;
    for (iter = info.begin(); iter != info.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const list<sRawDriveInfo>& info)
{
    stringstream sstr;

    list<sRawDriveInfo>::const_iterator iter;
    for (iter = info.begin(); iter != info.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const sDeviceInfo& info)
{
    stringstream sstr;

    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const sDeviceData& data)
{
    stringstream sstr;

    tDeviceMap::const_iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++)
    {
        sstr << ToString(iter->second);
    }

    return sstr.str();
}

string ToString(const NvmeDevice &data)
{
    stringstream sstr;
    int i;
    int temperature = ((data.smart.temperature[1] << 8) | data.smart.temperature[0]) - 273;

    sstr.precision(0);
    sstr << "log;" << data.iden.sn << ";" << data.timestamp << ";" << data.iden.path << ";" << data.iden.mn << ";"  << data.iden.fr << ";" \
         << "Capacity" << ";" << fixed << (double)(data.iden.nsze / 1000000000) << ";" \
         << "Critical_Warning" << ";" << fixed << U16(data.smart.critical_warning) << ";" \
         << "Temperature" << ";" << fixed << temperature << ";" \
         << "Available_Spare" << ";" << fixed << U16(data.smart.avail_spare) << ";" \
         << "Available_Spare_Threshold" << ";" << fixed << U16(data.smart.spare_thresh) << ";" \
         << "Percentage_Used" << ";" << fixed << U16(data.smart.percent_used) << ";" \
         << "Data_Units_Read" << ";" << fixed << (long double)(data.smart.data_units_read) << ";" \
         << "Data_Units_Written" << ";" << fixed << (long double)(data.smart.data_units_written) << ";" \
         << "Host_Read_Commands" << ";" << fixed << (long double)(data.smart.host_reads) << ";"\
         << "Host_Write_Commands" << ";" << fixed << (long double)(data.smart.host_writes) << ";" \
         << "Controller_Busy_Time" << ";" << fixed << (long double)(data.smart.ctrl_busy_time) << ";" \
         << "Power_Cycles" << ";" << fixed << (long double)(data.smart.power_cycles) << ";" \
         << "Power_On_Hours" << ";" << fixed << (long double)(data.smart.power_on_hours) << ";" \
         << "Unsafe_Shutdowns" << ";" << fixed << (long double)(data.smart.unsafe_shutdowns) << ";" \
         << "Media_Errors" << ";" << fixed << (long double)(data.smart.media_errors) << ";" \
         << "Num_Err_Log_Entries" << ";" << fixed << (long double)(data.smart.num_err_log_entries) << ";" \
         << "Warning_Temperature_Time" << ";" << fixed << U32(data.smart.warning_temp_time) << ";" \
         << "Critical_Composite_Temperature_Time" << ";" << fixed << U32(data.smart.critical_comp_time) << ";";
    for(i = 0; i < 8; i++)
    {
        __s32 temp = data.smart.temp_sensor[i];
        if(0 == temp)
        {
            sstr << "Temperature_Sensor_" << i << ";" << "NC" << ";";
            continue;
        }
        sstr << "Temperature_Sensor_" << i << ";" << temp - 273 << ";";
    }

    sstr << "Thermal_Management_T1_Trans_Count" << ";" << fixed << U32(data.smart.thm_temp1_trans_count) << ";" \
         << "Thermal_Management_T2_Trans_Count" << ";" << fixed << U32(data.smart.thm_temp2_trans_count) << ";" \
         << "Thermal_Management_T1_Total_Time" << ";" << fixed << U32(data.smart.thm_temp1_total_time) << ";" \
         << "Thermal_Management_T2_Total_Time" << ";" << fixed << U32(data.smart.thm_temp2_total_time) << ";" \
         << "Reversed_1" << ";" << fixed << (long double)(data.smart.nandwrite) << ";";

    sstr << endl;

    return sstr.str();
}

string ToString(const sHeaderLogData &data)
{
    stringstream sstr;
    U16 i, j, len;
    const U16 fields = 6;
    const char *a[fields] = {"model", "port", "SN", "type", "identify", "firmwareSlot"};

    // fill session
    sstr << "header;{\"session\":{\"testName\":";
    sstr << "\"" << data.testname << "\",\"dateTime\":";
    sstr << "\"" << MiscUtil::FormatTime("%s", "%Y-%m-%d %X", data.timestamp) << "\"";
    sstr << "},";

    // fill device
    len = data.device[0].size();
    sstr << "\"devices\":[";
    for(i = 0; i < len; i++)
    {
        sstr << "{";
        for(j = 0; j < fields; j ++)
        {
            sstr << "\"" << a[j] << "\":\"" << data.device[j][i] << "\"";
            if(j != (fields - 1))
                sstr << ",";
        }
        if(i != (len - 1))
            sstr << "},";
        else
            sstr << "}";
    }
    sstr << "]}" << endl;
    
    return sstr.str();
}

string ToVerboseString(const sIdentifyInfo& info)
{
    stringstream sstr;

    sstr << "Name: " << info.DriveName << endl;
    sstr << "Model: " << info.DeviceModel << endl;
    sstr << "Serial: " << info.SerialNumber << endl;
    sstr << "Version: " << info.FirmwareVersion << endl;
    sstr << "UserCap: " << info.UserCapacity << endl;

    return sstr.str();
}

string ToVerboseString(const sSmartInfo& info)
{
    #define APP_FRMT_U32(w, value) setw(w) << setfill(' ') << (U32) value

    stringstream sstr;
    const char* sep = " ";

    tHeaderItem itemList[] = {
        tHeaderItem("ID", 3),
        tHeaderItem("Attribute", 30),
        tHeaderItem("High Raw", 10),
        tHeaderItem("Low Raw", 10),
        tHeaderItem("Value", 5),
        tHeaderItem("Worst", 5),
        tHeaderItem("Threshold", 9),
    };

    MakeHeaderList(itemList, header);
    sstr << header.ToString(" ") << endl;

    tAttributeMap::const_iterator iter;
    for (iter = info.AttrMap.begin(); iter != info.AttrMap.end(); ++iter)
    {
        const sAttribute& attr = iter->second;

        ASSERT (iter->first == attr.ID);

        sstr
            << APP_FRMT_U32(header.Length(0), attr.ID)        << sep
            << FRMT_STR(header.Length(1), attr.Name)          << sep
            << APP_FRMT_U32(header.Length(2), attr.HighRaw)    << sep
            << APP_FRMT_U32(header.Length(3), attr.LowRaw)    << sep
            << APP_FRMT_U32(header.Length(4), attr.Value)     << sep
            << APP_FRMT_U32(header.Length(5), attr.Worst)     << sep
            << APP_FRMT_U32(header.Length(6), attr.Threshold) << sep
            << endl;
    }

    return sstr.str();

    #undef APP_FRMT_U32
}

string ToVerboseString(const sDriveInfo& info)
{
    stringstream sstr;

    string sep(55, '-');

    char timeSlot[80];
    strftime(
        timeSlot, sizeof(timeSlot) / sizeof(timeSlot[0]),
        "%Y-%m-%d %H:%M:%S", localtime(&info.CaptureTime));

    string errorString = (0 == info.DriveErrorState) ? "This is a valid record" : "This is an invalid record";

    sstr << "Capture Time: " << timeSlot << endl;
    sstr << "Status: " << errorString << endl << sep << endl;
    sstr << ToVerboseString(info.IdentifyInfo) << sep << endl;
    sstr << ToVerboseString(info.SmartInfo) << sep << endl;

    return sstr.str();
}

string ToVerboseString(const sDeviceInfo& info)
{
    stringstream sstr;

    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++)
    {
        sstr << ToVerboseString(*iter) << endl;
    }

    return sstr.str();
}

string ToVerboseString(const sDataRetentionInfo& info)
{
    stringstream sstr;

    sstr << "Remaining days at 25 celcius " << info.RemainingDay_25C << endl;
    sstr << "Remaining days at 40 celcius " << info.RemainingDay_40C << endl;
    sstr << "Remaining days at 55 celcius " << info.RemainingDay_55C << endl;
    sstr << "Remaining days at 70 celcius " << info.RemainingDay_70C << endl;
    sstr << "Remaining days at 85 celcius " << info.RemainingDay_85C << endl;

    return sstr.str();
}

string ToShortString(const tSerialMap& serialMap)
{
    stringstream sstr;

    tSerialMap::const_iterator serialIter;
    for (serialIter = serialMap.begin(); serialIter != serialMap.end(); serialIter++)
    {
        string oldSerial = serialIter->first;

        const tCapacityMap& capMap = serialIter->second;
        tCapacityMap::const_iterator capIter;

        int mapCount = capMap.size();
        sstr << "OldSerial: " << left << setw(21) << oldSerial << "MapCount: " << mapCount << endl;
        for (capIter = capMap.begin(); capIter != capMap.end(); capIter++)
        {
            int capacity = capIter->first;
            string newSerial = capIter->second;

            sstr << "NewSerial: " << left << setw(21) << newSerial << "Capacity: " << capacity << endl;
        }

        sstr << endl;
    }

    return sstr.str();
}

string ToShortString(const sDeviceInfo& info)
{
    stringstream sstr;

    string serialNumber;
    string deviceModel;
    string updateTime;

    int driveCount = info.DriveSet.size();

    if (0 != driveCount)
    {
        const sDriveInfo& drive = *(info.DriveSet.rbegin());

        serialNumber = drive.IdentifyInfo.SerialNumber;
        deviceModel = drive.IdentifyInfo.DeviceModel;

        char timeBuff[80]; int buffSize = sizeof(timeBuff) / sizeof(timeBuff[0]);
        strftime( timeBuff, buffSize, "%Y-%m-%d %H:%M:%S", localtime(&drive.CaptureTime));
        updateTime = timeBuff;
    }

    sstr << "Last Update: " << updateTime << endl;
    sstr << "Device Model: " << deviceModel << endl;
    sstr << "Serial Number: " << serialNumber << endl;
    sstr << "Number of Records: " << driveCount << endl;
    sstr << endl;

    sstr << "No CaptureTime         RecordStatus\n";
    sstr << "-- ------------------- --------------------------------\n";

    int recordIndex = 0;
    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++, recordIndex++)
    {
        const sDriveInfo& drive = *iter;

        char timeBuff[80]; int buffSize = sizeof(timeBuff) / sizeof(timeBuff[0]);
        strftime( timeBuff, buffSize, "%Y-%m-%d %H:%M:%S", localtime(&drive.CaptureTime));

        sstr << setw(2)  << left << recordIndex << " ";
        sstr << setw(19) << left << timeBuff << " ";

        if(0 != drive.DriveErrorState) sstr << left << "Invalid record" << " ";
        sstr << endl;
    }

    return sstr.str();
}

void GetPowerOffDataRetentionEstimate (eNandType nandType, U32 eraseCount, sDataRetentionInfo &info)
{
    double MLCFactor[] = {
        18, -1,            // Deg85, A, B
        77.944, -0.994346, // Deg70, A, B
        450, -1,           // Deg55, A, B
        3000, -1,          // Deg40, A, B
        24000, -1,         // Deg25, A, B
    };

    double SLCFactor[] = {
        600, -1,           // Deg85, A, B
        2113.69, -0.957078,// Deg70, A, B
        12287.3, -0.975084,// Deg55, A, B
        100000, -1,        // Deg40, A, B
        800000, -1,        // Deg25, A, B
    };

    double* ptr = SLCFactor;

    switch (nandType)
    {
    case NAND_MLC: ptr = MLCFactor; break;
    case NAND_SLC: ptr = SLCFactor; break;
    default: break;
    }

    // Set threshold to 10 years
    #define MAP_ITEM(name, index) do { \
        double remainingYear = ptr[index * 2 + 0] * pow(eraseCount, ptr[index * 2 + 1]); \
        if (remainingYear > 10) remainingYear = 10; \
        info.name = (int)(remainingYear * 365); \
    } while(0)

    MAP_ITEM(RemainingDay_85C, 0);
    MAP_ITEM(RemainingDay_70C, 1);
    MAP_ITEM(RemainingDay_55C, 2);
    MAP_ITEM(RemainingDay_40C, 3);
    MAP_ITEM(RemainingDay_25C, 4);

    #undef MAP_ITEM
}

string ParseIdenity(sIdentifyInfo &driveInfo, const U8 *buf)
{
    stringstream sstr;
    U32 temp, temp2, pos;
    string s;

    const IdentifyDeviceData& iden = *((IdentifyDeviceData*) buf);

    const string Word0[32] = {
                                                  " 0 = Reserved", \
                                                  " 1 = Reserved", \
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  " 0 = Response complete", \
                                                  " 1 = Response incomplete", \
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  " 0 = Removable controller and/or device", \
                                                  " 1 = Not removable controller and/or device", \
                                                  " 0 = Not removable media device", \
                                                  " 1 = Removable media device",
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  " 0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = Retired", \
                                                  " 1 = Retired", \
                                                  "0 = ATA device", \
                                                  " 1 = Reserved"
                                                 };

    const string Word48[32] = {
                               " 0 = Trusted Computing feature set is not supported", \
                               " 1 = Trusted Computing feature set is supported", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Shall be set to one", \
                               "0 = Shall be cleared to zero", \
                               " 1 = Reserved"
                              };

    const string Word49[32] = {
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired", \
                                          " 0 = Reserved", \
                                          " 1 = Retired", \
                                          " 0 = Retired", \
                                          " 1 = Retired",
                                          " 0 = DMA not supported", \
                                          " 1 = DMA supported", \
                                          " 0 = LBA not supported", \
                                          " 1 = LBA supported", \
                                          "0 = Reserved", \
                                          " 1 = IORDY may be disabled, shall be set to one", \
                                          "0 = IORDY may be supported", \
                                          " 1 = IORDY supported", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Standby timer values shall be managed by the device", \
                                          " 1 = Standby timer values as specified in this standard are supported", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Reserved"
                                         };

    const string Word50[32] = {
                                          " 0 = There is no minimum Standby timer value", \
                                          " 1 = There is a minimum Standby time value and it is vendor specific", \
                                          " 0 = Obsolete", \
                                          " 1 = Obsolete", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved",
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          " 0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Reserved", \
                                          "0 = Reserved", \
                                          " 1 = Shall be set to one", \
                                          "0 = Shall be cleared to zero", \
                                          " 1 = Reserved"
                                         };

    const string Word53[32] = {
                               " 0 = The fields reported in words 54-58 are not valid", \
                               " 1 = The fields reported in words 54-58 are valid", \
                               " 0 = The fields reported in words 64-70 are not valid", \
                               " 1 = The fields reported in words 64-70 are valid", \
                               " 0 = The fields reported in word 88 are not valid", \
                               " 1 = The fields reported in word 88 are valid", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word59[32] = {
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit", \
                               " 0 = Group bit", \
                               " 1 = Group bit",
                               " 0 = Multiple logical sector setting is not valid", \
                               " 1 = Multiple logical sector setting is valid", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = The SANITIZE ANTIFREEZE LOCK EXT command is not supported", \
                               " 1 = The SANITIZE ANTIFREEZE LOCK EXT command is supported", \
                               "0 = The commands allowed during a sanitize operation are as specified by ACS-2", \
                               " 1 = The commands allowed during a sanitize operation are as specified by ACS-8", \
                               "0 = The Sanitize feature set is not supported", \
                               " 1 = The Sanitize feature set is supported", \
                               "0 = The CRYPTO SCRAMBLE EXT command is not supported", \
                               " 1 = The CRYPTO SCRAMBLE EXT command is supported", \
                               "0 = The OVERWRITE EXT command is not supported", \
                               " 1 = The OVERWRITE EXT command is supported", \
                               "0 = The BLOCK ERASE EXT command is not supported", \
                               " 1 = The BLOCK ERASE EXT command is supported"
                              };

    const string Word63[32] = {
                               " 0 = Multiword DMA mode 0 is not supported", \
                               " 1 = Multiword DMA mode 0 is supported", \
                               " 0 = Reserved", \
                               " 1 = Multiword DMA mode 1 and below are supported", \
                               " 0 = Reserved", \
                               " 1 = Multiword DMA mode 2 and below are supported", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Multiword DMA mode 0 is not selected", \
                               " 1 = Multiword DMA mode 0 is selected", \
                               " 0 = Multiword DMA mode 1 is not selected", \
                               " 1 = Multiword DMA mode 1 is selected", \
                               "0 = Multiword DMA mode 2 is not selected", \
                               " 1 = Multiword DMA mode 2 is selected", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word69[32] = {
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Write cache may be volatile", \
                               " 1 = All write cache is non-volatile", \
                               " 0 = Extended Number of User Addressable Sectors is not supported (words 230-233)", \
                               " 1 = Extended Number of User Addressable Sectors is supported (words 230-233)", \
                               " 0 = Device may not Encrypts All User Data on the device", \
                               " 1 = Device Encrypts All User Data on the device", \
                               " 0 = Trimmed LBA range(s) returning zeroed data is not supported", \
                               " 1 = Trimmed LBA range(s) returning zeroed data is supported", \
                               " 0 = Optional ATA device 28-bit commands is supported", \
                               " 1 = Optional ATA device 28-bit commands is not supported", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = DOWNLOAD MICROCODE DMA is not supported", \
                               " 1 = DOWNLOAD MICROCODE DMA is supported", \
                               " 0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = WRITE BUFFER DMA is not supported", \
                               " 1 = WRITE BUFFER DMA is supported", \
                               "0 = READ BUFFER DMA is not supported", \
                               " 1 = READ BUFFER DMA is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = Long Physical Sector Alignment Error Reporting Control is not supported", \
                               " 1 = Long Physical Sector Alignment Error Reporting Control is supported", \
                               "0 = Deterministic data in trimmed LBA range(s) is not supported", \
                               " 1 = Deterministic data in trimmed LBA range(s) is supported", \
                               "0 = Reserved for CFA", \
                               " 1 = Reserved for CFA"
                              };

    const string Word76[32] = {
                               " 0 = Shall be cleared to zero", \
                               " 1 = Reserved", \
                               " 0 = Not supports SATA Gen1 Signaling Speed (1.5Gb/s)", \
                               " 1 = Supports SATA Gen1 Signaling Speed (1.5Gb/s)", \
                               " 0 = Not supports SATA Gen2 Signaling Speed (3.0Gb/s)", \
                               " 1 = Supports SATA Gen2 Signaling Speed (3.0Gb/s)", \
                               " 0 = Not supports SATA Gen3 Signaling Speed (6.0Gb/s)", \
                               " 1 = Supports SATA Gen3 Signaling Speed (6.0Gb/s)", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Not supports the NCQ feature set", \
                               " 1 = Supports the NCQ feature set", \
                               " 0 = Not supports receipt of host initiated power management requests", \
                               " 1 = Supports receipt of host initiated power management requests", \
                               "0 = Not supports the SATA Phy Event Counters log", \
                               " 1 = Supports the SATA Phy Event Counters log", \
                               "0 = Not supports Unload while NCQ commands are outstanding", \
                               " 1 = Supports Unload while NCQ commands are outstanding", \
                               "0 = Not supports NCQ priority information", \
                               " 1 = Supports NCQ priority information", \
                               "0 = Not supports Host Automatic Partial to Slumber transitions", \
                               " 1 = Supports Host Automatic Partial to Slumber transitions", \
                               "0 = Not supports Device Automatic Partial to Slumber transitions", \
                               " 1 = Supports Device Automatic Partial to Slumber transitions", \
                               "0 = Not supports READ LOG DMA EXT as equivalent to READ LOG EXT", \
                               " 1 = Supports READ LOG DMA EXT as equivalent to READ LOG EXT"
                              };

    const string Word77[32] = {
                               " 0 = Shall be cleared to zero", \
                               " 1 = Reserved", \
                               " 0 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 1 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 0 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 1 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 0 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 1 = Coded value indicating current negotiated Serial ATA signal speed", \
                               " 0 = Not supports NCQ Streaming", \
                               " 1 = Supports NCQ Streaming", \
                               " 0 = not supports NCQ Queue Management Command", \
                               " 1 = Supports NCQ Queue Management Command", \
                               " 0 = Not supports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands", \
                               " 1 = Supports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word78[32] = {
                               " 0 = Shall be cleared to zero", \
                               " 1 = Reserved", \
                               " 0 = Device not supports non-zero buffer offsets", \
                               " 1 = Device supports non-zero buffer offsets", \
                               " 0 = Device not supports DMA Setup auto-activation", \
                               " 1 = Device supports DMA Setup auto-activation", \
                               " 0 = Device not supports initiating power management", \
                               " 1 = Device supports initiating power management", \
                               " 0 = Device not supports in-order data delivery", \
                               " 1 = Device supports in-order data delivery", \
                               " 0 = Device not supports Hardware Feature Control", \
                               " 1 = Device supports Hardware Feature Control", \
                               " 0 = Device not supports Software Settings Preservation", \
                               " 1 = Device supports Software Settings Preservation", \
                               " 0 = Device not supports NCQ Autosense", \
                               " 1 = Device supports NCQ Autosense",
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word79[32] = {
                               " 0 = Shall be cleared to zero", \
                               " 1 = Reserved", \
                               " 0 = Non-zero buffer offsets disabled", \
                               " 1 = Non-zero buffer offsets enabled", \
                               " 0 = DMA Setup auto-activation disabled", \
                               " 1 = DMA Setup auto-activation enabled", \
                               " 0 = Device initiated power management disabled", \
                               " 1 = Device initiated power management enabled", \
                               " 0 = In-order data delivery disabled", \
                               " 1 = In-order data delivery enabled", \
                               " 0 = Hardware Feature Control is disabled", \
                               " 1 = Hardware Feature Control is enabled", \
                               " 0 = Software Settings Preservation disabled", \
                               " 1 = Software Settings Preservation enabled", \
                               " 0 = Automatic Partial to Slumber transitions disabled", \
                               " 1 = Automatic Partial to Slumber transitions enabled",
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word80[32] = {
                               " 0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Obsolete", \
                               " 1 = Obsolete", \
                               " 0 = Not supports ATA-2", \
                               " 1 = Supports ATA-2", \
                               " 0 = Not supports ATA-3", \
                               " 1 = Supports ATA-3", \
                               " 0 = Not supports ATA/ATAPI-4", \
                               " 1 = Supports ATA/ATAPI-4", \
                               " 0 = Not supports ATA/ATAPI-5", \
                               " 1 = Supports ATA/ATAPI-5", \
                               " 0 = Not supports ATA/ATAPI-6", \
                               " 1 = Supports ATA/ATAPI-6", \
                               " 0 = Not supports ATA/ATAPI-7", \
                               " 1 = Supports ATA/ATAPI-7", \
                               " 0 = Not supports ATA8-ACS", \
                               " 1 = Supports ATA8-ACS", \
                               " 0 = Not supports ACS-2", \
                               " 1 = Supports ACS-2", \
                               "0 = Not supports ACS-3", \
                               " 1 = Supports ACS-3", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word82[32] = {
                               " 0 = The SMART feature set is not supported", \
                               " 1 = The SMART feature set is supported", \
                               " 0 = The Security feature set is not supported", \
                               " 1 = The Security feature set is supported", \
                               " 0 = Removable Media feature set is not supported", \
                               " 1 = Removable Media feature set is supported", \
                               " 0 = Power Management feature set is not supported", \
                               " 1 = Power Management feature set is supported", \
                               " 0 = PACKET feature set is not supported", \
                               " 1 = PACKET feature set is supported", \
                               " 0 = The volatile write cache is not supported", \
                               " 1 = The volatile write cache is supported", \
                               " 0 = Read look-ahead is not supported", \
                               " 1 = Read look-ahead is supported", \
                               " 0 = Release interrupt is not supported", \
                               " 1 = Release interrupt is supported",
                               " 0 = SERVICE interrupt is not supported", \
                               " 1 = SERVICE interrupt is supported", \
                               " 0 = DEVICE RESET command is not supported", \
                               " 1 = DEVICE RESET command is supported", \
                               "0 = Host Protected Area feature set is not supported", \
                               " 1 = Host Protected Area feature set is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = The WRITE BUFFER command is not supported", \
                               " 1 = The WRITE BUFFER command is supported", \
                               "0 = The READ BUFFER command is not supported", \
                               " 1 = The READ BUFFER command is supported", \
                               "0 = The NOP command is not supported", \
                               " 1 = The NOP command is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete"
                              };

    const string Word83[32] = {
                               " 0 = DOWNLOAD MICROCODE command is not supported", \
                               " 1 = DOWNLOAD MICROCODE command is supported", \
                               " 0 = READ/WRITE DMA QUEUED is not supported", \
                               " 1 = READ/WRITE DMA QUEUED is supported", \
                               " 0 = CFA feature set is not supported", \
                               " 1 = CFA feature set is supported", \
                               " 0 = Advanced Power Management feature set is not supported", \
                               " 1 = Advanced Power Management feature set is supported", \
                               " 0 = Removable Media Status Notification feature set is not supported", \
                               " 1 = Removable Media Status Notification feature set is supported", \
                               " 0 = Power-Up In Standby feature set is not supported", \
                               " 1 = Power-Up In Standby feature set is supported", \
                               " 0 = SET FEATURES subcommand not required to spinup after power-up", \
                               " 1 = SET FEATURES subcommand required to spinup after power-up", \
                               " 0 = Reserved for the Address Offset Reserved Area Boot Method", \
                               " 1 = Reserved for the Address Offset Reserved Area Boot Method", \
                               " 0 = SET MAX security extension is not supported",
                               " 1 = SET MAX security extension is supported", \
                               " 0 = The AAM feature set is not supported", \
                               " 1 = The AAM feature set is supported", \
                               "0 = The 48-bit Address feature set is not supported", \
                               " 1 = The 48-bit Address feature set is supported", \
                               "0 = The DCO feature set is not supported", \
                               " 1 = The DCO feature set is supported", \
                               "0 = FLUSH CACHE command is not supported", \
                               " 1 = FLUSH CACHE command is supported", \
                               "0 = FLUSH CACHE EXT command is not supported", \
                               " 1 = FLUSH CACHE EXT command is supported", \
                               "0 = Reserved", \
                               " 1 = Shall be set to one", \
                               "0 = Shall be cleared to zero", \
                               " 1 = Reserved"
                              };

    const string Word84[32] = {
                               " 0 = SMART error logging is not supported", \
                               " 1 = SMART error logging is supported", \
                               " 0 = The SMART self-test is not supported", \
                               " 1 = The SMART self-test is supported", \
                               " 0 = Media serial number is not supported", \
                               " 1 = Media serial number is supported", \
                               " 0 = The Media Card Pass Through Command feature set is not supported", \
                               " 1 = The Media Card Pass Through Command feature set is supported", \
                               " 0 = The Streaming feature set is not supported", \
                               " 1 = The Streaming feature set is supported", \
                               " 0 = The GPL feature set is not supported", \
                               " 1 = The GPL feature set is supported", \
                               " 0 = The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are not supported", \
                               " 1 = The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are supported", \
                               " 0 = The WRITE DMA QUEUED FUA EXT command is not supported", \
                               " 1 = The WRITE DMA QUEUED FUA EXT command is supported",
                               " 0 = The 64-bit World wide name is not supported", \
                               " 1 = The 64-bit World wide name is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = Reserved for TLC", \
                               " 1 = Reserved for TLC", \
                               "0 = Reserved for TLC", \
                               " 1 = Reserved for TLC", \
                               "0 = The IDLE IMMEDIATE command with UNLOAD feature is not supported", \
                               " 1 = The IDLE IMMEDIATE command with UNLOAD feature is supported", \
                               "0 = Reserved", \
                               " 1 = Shall be set to one", \
                               "0 = Shall be cleared to zero", \
                               " 1 = Reserved"
                              };

    const string Word85[32] = {
                               " 0 = SMART feature set is disabled", \
                               " 1 = SMART feature set is enabled", \
                               " 0 = Security Mode feature set is disabled", \
                               " 1 = Security Mode feature set is enabled", \
                               " 0 = Removable Media feature set is disabled", \
                               " 1 = Removable Media feature set is enabled", \
                               " 0 = Power Management feature set is not supported", \
                               " 1 = Power Management feature set is supported", \
                               " 0 = PACKET feature set is not supported", \
                               " 1 = PACKET feature set is supported", \
                               " 0 = The volatile write cache is disabled", \
                               " 1 = The volatile write cache is enabled", \
                               " 0 = Read look-ahead is disabled", \
                               " 1 = Read look-ahead is enabled", \
                               " 0 = The release interrupt is disabled", \
                               " 1 = The release interrupt is enabled",
                               " 0 = The SERVICE interrupt is disabled", \
                               " 1 = The SERVICE interrupt is enabled", \
                               " 0 = DEVICE RESET command is not supported", \
                               " 1 = DEVICE RESET command is supported", \
                               "0 = Host Protected Area feature set is not supported", \
                               " 1 = Host Protected Area feature set is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = The WRITE BUFFER command is not supported", \
                               " 1 = The WRITE BUFFER command is supported", \
                               "0 = The READ BUFFER command is not supported", \
                               " 1 = The READ BUFFER command is supported", \
                               "0 = The NOP command is not supported", \
                               " 1 = The NOP command is supported", \
                               "0 = Obsolete", \
                               " 1 = Obsolete"
                              };

    const string Word86[32] = {
                               " 0 = The DOWNLOAD MICROCODE command is not supported", \
                               " 1 = The DOWNLOAD MICROCODE command is supported", \
                               " 0 = The TCQ feature set is not supported", \
                               " 1 = The TCQ feature set is supported", \
                               " 0 = The CFA feature set is not supported", \
                               " 1 = The CFA feature set is supported", \
                               " 0 = The Advanced Power Management feature set is disabled", \
                               " 1 = The Advanced Power Management feature set is enabled", \
                               " 0 = Removable Media Status Notification feature set is disabled", \
                               " 1 = Removable Media Status Notification feature set is enabled", \
                               " 0 = The Power-Up In Standby feature set is disabled", \
                               " 1 = The Power-Up In Standby feature set is enabled", \
                               " 0 = SET FEATURES subcommand is not required to spin-up after power-up", \
                               " 1 = SET FEATURES subcommand is required to spin-up after power-up", \
                               " 0 = Reserved for Address Offset Reserved Area Boot Method", \
                               " 1 = Reserved for Address Offset Reserved Area Boot Method",
                               " 0 = SET MAX security extension is disabled by SET MAX SET PASSWORD", \
                               " 1 = SET MAX security extension is enabled by SET MAX SET PASSWORD", \
                               " 0 = The AAM feature set is disabled", \
                               " 1 = The AAM feature set is enabled", \
                               "0 = The 48-bit Address features set is not supported", \
                               " 1 = The 48-bit Address features set is supported", \
                               "0 = The DCO feature set is not supported", \
                               " 1 = The DCO feature set is supported", \
                               "0 = FLUSH CACHE command is not supported", \
                               " 1 = FLUSH CACHE command is supported", \
                               "0 = FLUSH CACHE EXT command is not supported", \
                               " 1 = FLUSH CACHE EXT command is supported", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = Words 119-120 are not valid", \
                               " 1 = Words 119-120 are valid"
                              };

    const string Word87[32] = {
                               " 0 = SMART error logging is not supported", \
                               " 1 = SMART error logging is supported", \
                               " 0 = SMART self-test is not supported", \
                               " 1 = SMART self-test is supported", \
                               " 0 = Media serial number is not valid", \
                               " 1 = Media serial number is valid", \
                               " 0 = The Media Card Pass Through Command feature set is not supported", \
                               " 1 = The Media Card Pass Through Command feature set is supported", \
                               " 0 = Obsolete", \
                               " 1 = Obsolete", \
                               " 0 = The GPL feature set is not supported", \
                               " 1 = The GPL feature set is supported", \
                               " 0 = The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are not supported", \
                               " 1 = The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are supported", \
                               " 0 = The WRITE DMA QUEUED FUA EXT command is not supported", \
                               " 1 = The WRITE DMA QUEUED FUA EXT command is supported",
                               " 0 = The 64-bit World wide name is not supported", \
                               " 1 = The 64-bit World wide name is supported", \
                               " 0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = Obsolete", \
                               " 1 = Obsolete", \
                               "0 = Reserved for TLC", \
                               " 1 = Reserved for TLC", \
                               "0 = Reserved for TLC", \
                               " 1 = Reserved for TLC", \
                               "0 = The IDLE IMMEDIATE command with UNLOAD FEATURE is not supported", \
                               " 1 = The IDLE IMMEDIATE command with UNLOAD FEATURE is supported", \
                               "0 = Reserved", \
                               " 1 = Shall be set to one", \
                               "0 = Shall be cleared to zero", \
                               " 1 = Reserved"
                              };

    const string Word88[32] = {
                               " 0 = Ultra DMA mode 0 is not supported", \
                               " 1 = Ultra DMA mode 0 is supported", \
                               " 0 = Ultra DMA mode 1 and below are not supported", \
                               " 1 = Ultra DMA mode 1 and below are supported", \
                               " 0 = Ultra DMA mode 2 and below are not supported", \
                               " 1 = Ultra DMA mode 2 and below are supported", \
                               " 0 = Ultra DMA mode 3 and below are not supported", \
                               " 1 = Ultra DMA mode 3 and below are supported", \
                               " 0 = Ultra DMA mode 4 and below are not supported", \
                               " 1 = Ultra DMA mode 4 and below are supported", \
                               " 0 = Ultra DMA mode 5 and below are not supported", \
                               " 1 = Ultra DMA mode 5 and below are supported", \
                               " 0 = Ultra DMA mode 6 and below are not supported", \
                               " 1 = Ultra DMA mode 6 and below are supported", \
                               " 0 = Reserved", \
                               " 1 = Reserved",
                               " 0 = Ultra DMA mode 0 is not selected", \
                               " 1 = Ultra DMA mode 0 is selected", \
                               " 0 = Ultra DMA mode 1 is not selected", \
                               " 1 = Ultra DMA mode 1 is selected", \
                               "0 = Ultra DMA mode 2 is not selected", \
                               " 1 = Ultra DMA mode 2 is selected", \
                               "0 = Ultra DMA mode 3 is not selected", \
                               " 1 = Ultra DMA mode 3 is selected", \
                               "0 = Ultra DMA mode 4 is not selected", \
                               " 1 = Ultra DMA mode 4 is selected", \
                               "0 = Ultra DMA mode 5 is not selected", \
                               " 1 = Ultra DMA mode 5 is selected", \
                               "0 = Ultra DMA mode 6 is not selected", \
                               " 1 = Ultra DMA mode 6 is selected", \
                               "0 = Reserved", \
                               " 1 = Reserved"
                              };

    const string Word93[32] = {
                               " 0 = Reserved", \
                               " 1 = Shall be set to one for PATA devices", \
                               " 0 = Group bits indicate how Device 0 determined the device number", \
                               " 1 = Group bits indicate how Device 0 determined the device number", \
                               " 0 = Group bits indicate how Device 0 determined the device number", \
                               " 1 = Group bits indicate how Device 0 determined the device number", \
                               " 0 = Device 0 failed diagnostics", \
                               " 1 = Device 0 passed diagnostics", \
                               " 0 = Device 0 did not detect the assertion of PDIAG", \
                               " 1 = Device 0 detected the assertion of PDIAG", \
                               " 0 = Device 0 did not detect the assertion of DASP", \
                               " 1 = Device 0 detected the assertion of DASP", \
                               " 0 = Device 0 does not respond when Device 1 is selected", \
                               " 1 = Device 0 responds when Device 1 is selected", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               " 0 = Reserved", \
                               " 1 = Shall be set to one",
                               " 0 = Group bits indicate how Device 1 determined the device number", \
                               " 1 = Group bits indicate how Device 1 determined the device number", \
                               " 0 = Group bits indicate how Device 1 determined the device number", \
                               " 1 = Group bits indicate how Device 1 determined the device number", \
                               "0 = Device 1 did not assert PDIAG", \
                               " 1 = Device 1 asserted PDIAG", \
                               "0 = Reserved", \
                               " 1 = Reserved", \
                               "0 = device detected CBLID- below ViL", \
                               " 1 = device detected CBLID- above ViH", \
                               "0 = Reserved", \
                               " 1 = Shall be set to one", \
                               "0 = Shall be cleared to zero", \
                               " 1 = Reserved"
                              };

    const string Word106[32] = {
                                " 0 = Group bits 2X logical sectors per physical sector", \
                                " 1 = Group bits 2X logical sectors per physical sector", \
                                " 0 = Group bits 2X logical sectors per physical sector", \
                                " 1 = Group bits 2X logical sectors per physical sector", \
                                " 0 = Group bits 2X logical sectors per physical sector", \
                                " 1 = Group bits 2X logical sectors per physical sector", \
                                " 0 = Group bits 2X logical sectors per physical sector", \
                                " 1 = Group bits 2X logical sectors per physical sector", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Device Logical Sector longer than 256 Words", \
                                "0 = Reserved", \
                                " 1 = Device has multiple logical sectors per physical sector", \
                                "0 = Reserved", \
                                " 1 = Shall be set to one", \
                                "0 = Shall be cleared to zero", \
                                " 1 = Reserved"
                               };

    const string Word119[32] = {
                                " 0 = Reserved for DDT", \
                                " 1 = Reserved for DDT", \
                                " 0 = The Write-Read-Verify feature set is not supported", \
                                " 1 = The Write-Read-Verify feature set is supported", \
                                " 0 = The WRITE UNCORRECTABLE EXT command is not supported", \
                                " 1 = The WRITE UNCORRECTABLE EXT command is supported", \
                                " 0 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are not supported", \
                                " 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported", \
                                " 0 = The DOWNLOAD MICROCODE command with mode 3 is not supported", \
                                " 1 = The DOWNLOAD MICROCODE command with mode 3 is supported", \
                                " 0 = The Free-fall Control feature set is not supported", \
                                " 1 = The Free-fall Control feature set is supported", \
                                " 0 = Sense Data Reporting feature set is not supported", \
                                " 1 = Sense Data Reporting feature set is supported", \
                                " 0 = EPC feature set is not supported", \
                                " 1 = EPC feature set is supported",
                                " 0 = Accessible Max Address Configuration feature set is not supported", \
                                " 1 = Accessible Max Address Configuration feature set is supported", \
                                " 0 = DSN feature set is not supported", \
                                " 1 = DSN feature set is supported", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Shall be set to one", \
                                "0 = Shall be cleared to zero", \
                                " 1 = Reserved"
                               };

    const string Word120[32] = {
                                " 0 = Reserved for DDT", \
                                " 1 = Reserved for DDT", \
                                " 0 = The Write-Read-Verify feature set is disabled", \
                                " 1 = The Write-Read-Verify feature set is enabled", \
                                " 0 = The WRITE UNCORRECTABLE EXT command is not supported", \
                                " 1 = The WRITE UNCORRECTABLE EXT command is supported", \
                                " 0 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are not supported", \
                                " 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported", \
                                " 0 = The DOWNLOAD MICROCODE command with mode 3 is not supported", \
                                " 1 = The DOWNLOAD MICROCODE command with mode 3 is supported", \
                                " 0 = The Free-fall Control feature set is disabled", \
                                " 1 = The Free-fall Control feature set is enabled", \
                                " 0 = Sense Data Reporting feature set is disabled", \
                                " 1 = Sense Data Reporting feature set is enabled", \
                                " 0 = EPC feature set is disabled", \
                                " 1 = EPC feature set is enabled",
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = DSN feature set is disabled", \
                                " 1 = DSN feature set is enabled", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Shall be set to one", \
                                "0 = Shall be cleared to zero", \
                                " 1 = Reserved"
                               };

    const string Word128[32] = {
                                " 0 = Security is not supported", \
                                " 1 = Security is supported", \
                                " 0 = Security is disabled", \
                                " 1 = Security is enabled", \
                                " 0 = Security is not locked", \
                                " 1 = Security is locked", \
                                " 0 = Security is not frozen", \
                                " 1 = Security is frozen", \
                                " 0 = Security count is not expired", \
                                " 1 = Security count is expired", \
                                " 0 = Enhanced security erase is not supported", \
                                " 1 = Enhanced security erase is supported", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = Mater Password High Capability", \
                                " 1 = Mater Password Maximum Capability", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved"
                               };

    const string Word160[32] = {
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma",
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                " 0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                "0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                "0 = Maximum current in ma", \
                                " 1 = Maximum current in ma", \
                                "0 = CFA power mode 1 enabled", \
                                " 1 = CFA power mode 1 disabled", \
                                "0 = CFA power mode 1 is not required for one or more commands implemented by the device", \
                                " 1 = CFA power mode 1 is required for one or more commands implemented by the device", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Word 160 is not supported", \
                                " 1 = Word 160 is supported"
                               };

    const string Word169[32] = {
                                " 0 = The TRIM bit in the DATA SET MANAGEMENT command is not supported", \
                                " 1 = The TRIM bit in the DATA SET MANAGEMENT command is supported", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved"
                               };

    const string Word206[32] = {
                                " 0 = The SCT Command Transport is not supported", \
                                " 1 = The SCT Command Transport is supported", \
                                " 0 = The SCT Read/Write Long command is not supported", \
                                " 1 = The SCT Read/Write Long command is supported", \
                                " 0 = The SCT Write Same command is not supported", \
                                " 1 = The SCT Write Same command is supported", \
                                " 0 = The SCT Error Recovery Control command is not supported", \
                                " 1 = The SCT Error Recovery Control command is supported", \
                                " 0 = The SCT Feature Control command is not supported", \
                                " 1 = The SCT Feature Control command is supported", \
                                " 0 = The SCT Data Tables command is not supported", \
                                " 1 = The SCT Data Tables command is supported", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Vendor Specific", \
                                " 1 = Vendor Specific", \
                                "0 = Vendor Specific", \
                                " 1 = Vendor Specific", \
                                "0 = Vendor Specific", \
                                " 1 = Vendor Specific", \
                                "0 = Vendor Specific", \
                                " 1 = Vendor Specific"
                               };

    const string Word209[32] = {
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed",
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                "0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                "0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                "0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                "0 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                " 1 = Logical sector offset within the first physical sector where the first logical sector is placed", \
                                "0 = Reserved", \
                                " 1 = Shall be set to one", \
                                "0 = Shall be cleared to zero", \
                                " 1 = Reserved"
                               };

    const string Word214[32] = {
                                " 0 = NV Cache Power Mode feature set not supported", \
                                " 1 = NV Cache Power Mode feature set supported", \
                                " 0 = NV Cache Power Mode feature set disabled", \
                                " 1 = NV Cache Power Mode feature set enabled", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = NV Cache feature set disabled", \
                                " 1 = NV Cache feature set enabled", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = NV Cache Power Mode feature set version", \
                                " 1 = NV Cache Power Mode feature set version", \
                                " 0 = NV Cache Power Mode feature set version", \
                                " 1 = NV Cache Power Mode feature set version", \
                                "0 = NV Cache Power Mode feature set version", \
                                " 1 = NV Cache Power Mode feature set version", \
                                "0 = NV Cache Power Mode feature set version", \
                                " 1 = NV Cache Power Mode feature set version", \
                                "0 = NV Cache feature set version", \
                                " 1 = NV Cache feature set version", \
                                "0 = NV Cache feature set version", \
                                " 1 = NV Cache feature set version", \
                                "0 = NV Cache feature set version", \
                                " 1 = NV Cache feature set version", \
                                "0 = NV Cache feature set version", \
                                " 1 = NV Cache feature set version"
                               };

    const string Word222[32] = {
                                " 0 = Reserved", \
                                " 1 = ATA8-AST", \
                                " 0 = Reserved", \
                                " 1 = SATA 1.0a", \
                                " 0 = Reserved", \
                                " 1 = SATA II: Extensions", \
                                " 0 = Reserved", \
                                " 1 = SATA Rev 2.5", \
                                " 0 = Reserved", \
                                " 1 = SATA Rev 2.6", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved",
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                " 0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved", \
                                "0 = Reserved", \
                                " 1 = Reserved"
                               };

    char strBuf[100];

    sstr << "{" << endl;

    READ_U16(temp, &buf[0*2]);
    sstr << "    \"Word 0: General configuration\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[0*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 2, 1, Word0, false);
    sstr << BuildIdentifylevel2(temp, 6, 2, Word0, false);
    sstr << BuildIdentifylevel2(temp, 15, 1, Word0, true);

    sstr << "    \"Word 1: Number of logical cylinders\":" << "\"" << HexFormatter::BuffToHexString(&buf[1*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 2: Specific configuration\":" << "\"" << HexFormatter::BuffToHexString(&buf[2*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 3: Number of logical heads\":" << "\"" << HexFormatter::BuffToHexString(&buf[3*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 4: Retired\":" << "\"" << HexFormatter::BuffToHexString(&buf[4*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 5: Retired\":" << "\"" << HexFormatter::BuffToHexString(&buf[5*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 6: Number of logical sectors per logical track\":" << "\"" << HexFormatter::BuffToHexString(&buf[6*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 7: Reserved for CFA\":" << "\"" << HexFormatter::BuffToHexString(&buf[7*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 8: Reserved for CFA\":" << "\"" << HexFormatter::BuffToHexString(&buf[8*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 9: Retired\":" << "\"" << HexFormatter::BuffToHexString(&buf[9*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 10-19: Serial number\":" << "\"" << driveInfo.SerialNumber << "\"," << endl;
    sstr << "    \"Word 20: Retired\":" << "\"" << HexFormatter::BuffToHexString(&buf[20*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 21: Retired\":" << "\"" << HexFormatter::BuffToHexString(&buf[21*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 22: Obsolete\":" << "\"" << HexFormatter::BuffToHexString(&buf[22*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 23-26: Firmware revision\":" << "\"" << driveInfo.FirmwareVersion << "\"," << endl;
    sstr << "    \"Word 27-46: Model number\":" << "\"" << driveInfo.DeviceModel << "\"," << endl;
    sstr << "    \"Word 47: READ/WRITE MULTIPLE support\":" << "\"" << HexFormatter::BuffToHexString(&buf[47*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[48*2]);
    sstr << "    \"Word 48: Trusted Computing feature set options\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[48*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, Word48, false);
    sstr << BuildIdentifylevel2(temp, 14, 2, Word48, true);

    READ_U16(temp, &buf[49*2]);
    sstr << "    \"Word 49: Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[49*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 8, 4, Word49, false);
    sstr << BuildIdentifylevel2(temp, 13, 1, Word49, true);

    READ_U16(temp, &buf[50*2]);
    sstr << "    \"Word 50: Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[50*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, Word50, false);
    sstr << BuildIdentifylevel2(temp, 14, 2, Word50, true);

    sstr << "    \"Word 51: Obsolete\":" << "\"" << HexFormatter::BuffToHexString(&buf[51*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 52: Obsolete\":" << "\"" << HexFormatter::BuffToHexString(&buf[52*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[53*2]);
    sstr << "    \"Word 53: Field validity\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[53*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 3, Word53, true);

    sstr << "    \"Word 54: Number of current logical cylinders\":" << "\"" << HexFormatter::BuffToHexString(&buf[54*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 55: Number of current logical heads\":" << "\"" << HexFormatter::BuffToHexString(&buf[55*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 56: Number of current logical sectors per track\":" << "\"" << HexFormatter::BuffToHexString(&buf[56*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 57-58: Current capacity in sectors\":" << "\"" << HexFormatter::BuffToHexString(&buf[57*2], 2*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[59*2]);
    sstr << "    \"Word 59: Multiple sector setting\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[59*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Number of sectors that shall be transferred per interrupt on R/W Multiple command" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 8, 1, Word59, false);
    sstr << BuildIdentifylevel2(temp, 10, 6, Word59, true);

    sstr << "    \"Word 60-61: Total number of user addressable sectors\":" << "\"" << HexFormatter::BuffToHexString(&buf[60*2], 2*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 62: Obsolete\":" << "\"" << HexFormatter::BuffToHexString(&buf[62*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[63*2]);
    sstr << "    \"Word 63: Multiword DMA transfer\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[63*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 3, Word63, false);
    sstr << BuildIdentifylevel2(temp, 8, 3, Word63, true);

    READ_U16(temp, &buf[64*2]);
    sstr << "    \"Word 64: PIO transfer modes supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[64*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    PIO modes supported" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[65*2]);
    sstr << "    \"Word 65: Minimum Multiword DMA transfer cycle time per word\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[65*2], 2, true) << "h" << "\"," << endl;
    sstr << "        \"bit 0-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp, 2, false) << "h" << " Cycle time in nanoseconds" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[66*2]);
    sstr << "    \"Word 66: Manufacturer's recommended Multiword DMA transfer cycle time\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[66*2], 2, true) << "h" << "\"," << endl;
    sstr << "        \"bit 0-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp, 2, false) << "h" << " Cycle time in nanoseconds" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[67*2]);
    sstr << "    \"Word 67: Minimum PIO transfer cycle time without flow control\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[67*2], 2, true) << "h" << "\"," << endl;
    sstr << "        \"bit 0-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp, 2, false) << "h" << " Cycle time in nanoseconds" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[68*2]);
    sstr << "    \"Word 68: Minimum PIO transfer cycle time with IORDY flow control\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[68*2], 2, true) << "h" << "\"," << endl;
    sstr << "        \"bit 0-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp, 2, false) << "h" << " Cycle time in nanoseconds" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[69*2]);
    sstr << "    \"Word 69: Additional Supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[69*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 2, 5, Word69, false);
    sstr << BuildIdentifylevel2(temp, 8, 1, Word69, false);
    sstr << BuildIdentifylevel2(temp, 10, 2, Word69, false);
    sstr << BuildIdentifylevel2(temp, 13, 2, Word69, true);

    sstr << "    \"Word 70: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[70*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 71: Reserved for IDENTIFY PACKET DEVICE command\":" << "\"" << HexFormatter::BuffToHexString(&buf[71*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 72: Reserved for IDENTIFY PACKET DEVICE command\":" << "\"" << HexFormatter::BuffToHexString(&buf[72*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 73: Reserved for IDENTIFY PACKET DEVICE command\":" << "\"" << HexFormatter::BuffToHexString(&buf[73*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 74: Reserved for IDENTIFY PACKET DEVICE command\":" << "\"" << HexFormatter::BuffToHexString(&buf[74*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[75*2]);
    sstr << "    \"Word 75: Queue depth\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[75*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x001F;
    sstr << "        \"bit 0-4\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Maximum queue depth - 1" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[76*2]);
    sstr << "    \"Word 76: Serial ATA Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[76*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 4, Word76, false);
    sstr << BuildIdentifylevel2(temp, 8, 8, Word76, true);

    READ_U16(temp, &buf[77*2]);
    sstr << "    \"Word 77: Serial ATA Additional Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[77*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, Word77, false);
    temp2 = temp & 0x000E;
    sstr << "        \"bit 1-3\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Current negotiated Serial ATA signal speed" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 4, 3, Word77, true);

    READ_U16(temp, &buf[78*2]);
    sstr << "    \"Word 78: Serial ATA features supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[78*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 8, Word78, true);

    READ_U16(temp, &buf[79*2]);
    sstr << "    \"Word 79: Serial ATA features enabled\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[79*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 8, Word79, true);

    READ_U16(temp, &buf[80*2]);
    sstr << "    \"Word 80: Major version number\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[80*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 2, 9, Word80, true);

    sstr << "    \"Word 81: Minor version number\":" << "\"" << HexFormatter::BuffToHexString(&buf[81*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[82*2]);
    sstr << "    \"Word 82: Commands and feature sets supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[82*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 11, Word82, false);
    sstr << BuildIdentifylevel2(temp, 12, 3, Word82, true);

    READ_U16(temp, &buf[83*2]);
    sstr << "    \"Word 83: Commands and feature sets supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[83*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 16, Word83, true);

    READ_U16(temp, &buf[84*2]);
    sstr << "    \"Word 84: Commands and feature sets supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[84*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 9, Word84, false);
    sstr << BuildIdentifylevel2(temp, 13, 3, Word84, true);

    READ_U16(temp, &buf[85*2]);
    sstr << "    \"Word 85: Commands and feature sets supported or enabled\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[85*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 11, Word85, false);
    sstr << BuildIdentifylevel2(temp, 12, 3, Word85, true);

    READ_U16(temp, &buf[86*2]);
    sstr << "    \"Word 86: Commands and feature sets supported or enabled\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[86*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 14, Word86, false);
    sstr << BuildIdentifylevel2(temp, 15, 1, Word86, true);

    READ_U16(temp, &buf[87*2]);
    sstr << "    \"Word 87: Commands and feature sets supported or enabled\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[87*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 4, Word87, false);
    sstr << BuildIdentifylevel2(temp, 5, 4, Word87, false);
    sstr << BuildIdentifylevel2(temp, 11, 5, Word87, true);

    READ_U16(temp, &buf[88*2]);
    sstr << "    \"Word 88: Ultra DMA modes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[88*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 7, Word88, false);
    sstr << BuildIdentifylevel2(temp, 8, 7, Word88, true);

    sstr << "    \"Word 89: Time required for security erase unit completion\":" << "\"" << HexFormatter::BuffToHexString(&buf[89*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 90: Time required for Enhanced security erase completion\":" << "\"" << HexFormatter::BuffToHexString(&buf[90*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 91: Current advanced power management value\":" << "\"" << HexFormatter::BuffToHexString(&buf[91*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 92: Master Password Identifier\":" << "\"" << HexFormatter::BuffToHexString(&buf[92*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[93*2]);
    sstr << "    \"Word 93: Hardware reset result\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[93*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, Word93, false);
    temp2 = temp & 0b0000000000000110;
    sstr << "        \"bit 1-2\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Group bits indicate how Device 0 determined the device number" << endl;
    sstr << "                          00 = Reserved" << endl;
    sstr << "                          01 = A jumper was used" << endl;
    sstr << "                          10 = The CSEL signal was used" << endl;
    sstr << "                          11 = Some other method was used or the method is unknown" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 3, 4, Word93, false);
    sstr << BuildIdentifylevel2(temp, 8, 1, Word93, false);
    temp2 = temp & 0b0000011000000000;
    sstr << "        \"bit 9-10\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "   Group bits indicate how Device 1 determined the device number" << endl;
    sstr << "                          00 = Reserved" << endl;
    sstr << "                          01 = A jumper was used" << endl;
    sstr << "                          10 = The CSEL signal was used" << endl;
    sstr << "                          11 = Some other method was used or the method is unknown" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 11, 1, Word93, false);
    sstr << BuildIdentifylevel2(temp, 13, 3, Word93, true);

    READ_U16(temp, &buf[94*2]);
    sstr << "    \"Word 94: Current AAM value\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[94*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Current AAM value" << "\"," << endl;
    temp2 = temp & 0xFF00;
    sstr << "        \"bit 8-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "   Vendor's recommended AAM value" << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Word 95: Stream Minimum Request Size\":" << "\"" << HexFormatter::BuffToHexString(&buf[95*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 96: Streaming Transfer Time - DMA\":" << "\"" << HexFormatter::BuffToHexString(&buf[96*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 97: Streaming Access Latency - DMA and PIO\":" << "\"" << HexFormatter::BuffToHexString(&buf[97*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 98-99: Streaming Performance Granularity (DWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[98*2], 2*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 100-103: Total Number of User Addressable Logical Sectors (QWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[100*2], 4*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 104: Streaming Transfer Time - PIO\":" << "\"" << HexFormatter::BuffToHexString(&buf[104*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 105: Maximum number of 512-byte blocks per DATA SET MANAGEMENT command\":" << "\"" << HexFormatter::BuffToHexString(&buf[105*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[106*2]);
    sstr << "    \"Word 106: Physical sector size / logical sector size\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[106*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x000F;
    sstr << "        \"bit 0-4\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    2X logical sectors per physical sector" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 12, 4, Word106, true);

    sstr << "    \"Word 107: Inter-seek delay for ISO 7779 standard acoustic testing\":" << "\"" << HexFormatter::BuffToHexString(&buf[107*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 108-111: World wide name\":" << "\"" << HexFormatter::BuffToHexString(&buf[108*2], 4*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 112: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[112*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 113: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[113*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 114: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[114*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 115: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[115*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 116: Reserved for TLC\":" << "\"" << HexFormatter::BuffToHexString(&buf[116*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 117-118: Logical sector size (DWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[117*2], 2*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[119*2]);
    sstr << "    \"Word 119: Commands and feature sets supported (Continued from words 82..84)\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[119*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 10, Word119, false);
    sstr << BuildIdentifylevel2(temp, 14, 2, Word119, true);

    READ_U16(temp, &buf[120*2]);
    sstr << "    \"Word 120: Commands and feature sets supported or enabled (Continued from words 85..87)\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[120*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 8, Word120, false);
    sstr << BuildIdentifylevel2(temp, 9, 1, Word120, false);
    sstr << BuildIdentifylevel2(temp, 14, 2, Word120, true);

    sstr << "    \"Word 121: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[121*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 122: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[122*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 123: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[123*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 124: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[124*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 125: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[125*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 126: Reserved for expanded supported and enabled settings\":" << "\"" << HexFormatter::BuffToHexString(&buf[126*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[127*2]);
    sstr << "    \"Word 127: Removable Media Status Notification feature set support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[127*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0b0000000000000011;
    sstr << "        \"bit 0-1\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Removable Media Status Notification feature set support" << endl;
    sstr << "                          00 = Removable Media Status Notification feature set not supported" << endl;
    sstr << "                          01 = Removable Media Status Notification feature supported" << endl;
    sstr << "                          10 = Reserved" << endl;
    sstr << "                          11 = Reserved" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[128*2]);
    sstr << "    \"Word 128: Security status\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[128*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 6, Word128, false);
    sstr << BuildIdentifylevel2(temp, 8, 1, Word128, true);

    sstr << "    \"Word 129-159: Vendor Specific\":" << "\"" << HexFormatter::BuffToHexString(&buf[129*2], 31*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[160*2]);
    sstr << "    \"Word 160: CFA power mode 1\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[160*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x0FFF;
    sstr << "        \"bit 0-11\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "   Maximum current in ma" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 12, 2, Word160, false);
    sstr << BuildIdentifylevel2(temp, 15, 1, Word160, true);

    sstr << "    \"Word 161: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[161*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 162: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[162*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 163: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[163*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 164: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[164*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 165: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[165*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 166: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[166*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 167: Reserved for the CompactFlash Association\":" << "\"" << HexFormatter::BuffToHexString(&buf[167*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[168*2]);
    sstr << "    \"Word 168: Device Nominal Form Factor\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[168*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x0FFF;
    sstr << "        \"bit 0-3\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Device Nominal Form Factor" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[169*2]);
    sstr << "    \"Word 169: DATA SET MANAGEMENT command support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[169*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, Word169, true);

    sstr << "    \"Word 170-173: Additional Product Identifier\":" << "\"" << HexFormatter::BuffToHexString(&buf[170*2], 4*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 174-175: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[174*2], 2*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 176-205: Current media serial number\":" << "\"" << HexFormatter::BuffToHexString(&buf[176*2], 30*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[206*2]);
    sstr << "    \"Word 206: SCT Command Transport\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[206*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 6, Word206, false);
    temp2 = temp & 0xF000;
    sstr << "        \"bit 12-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "  Vendor Specific" << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Word 207-208: Reserved for CE-ATA\":" << "\"" << HexFormatter::BuffToHexString(&buf[207*2], 2*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[209*2]);
    sstr << "    \"Word 209: Alignment of logical blocks within a physical block\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[209*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x3FFF;
    sstr << "        \"bit 0-13\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 2, false) << "h" << " Logical sector offset within the first physical sector where the first logical sector is placed" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 14, 2, Word209, true);

    sstr << "    \"Word 210-211: Write-Read-Verify Sector Mode 3 Count (DWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[210*2], 2*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 212-213: Write-Read-Verify Sector Mode 2 Count (DWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[212*2], 2*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[214*2]);
    sstr << "    \"Word 214: NV Cache Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[214*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 2, Word214, false);
    sstr << BuildIdentifylevel2(temp, 4, 1, Word214, false);
    temp2 = temp & 0x0F00;
    sstr << "        \"bit 8-11\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "   NV Cache Power Mode feature set version" << "\"," << endl;
    temp2 = temp & 0xF000;
    sstr << "        \"bit 12-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "  NV Cache feature set version" << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Word 215-216: NV Cache Size in Logical Blocks (DWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[215*2], 2*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 217: Nominal media rotation rate\":" << "\"" << HexFormatter::BuffToHexString(&buf[217*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 218: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[218*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[219*2]);
    sstr << "    \"Word 219: NV Cache Options\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[219*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Device Estimated Time to Spin Up in Seconds" << "\"" << endl;
    sstr << "    }," << endl;

    READ_U16(temp, &buf[220*2]);
    sstr << "    \"Word 220: Write-Read-Verify feature set current mode\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[220*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Write-Read-Verify feature set current mode" << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Word 221: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[221*2], 2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[222*2]);
    sstr << "    \"Word 222: Transport major version number\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[222*2], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 5, Word222, false);
    temp2 = temp & 0xF000;
    sstr << "        \"bit 12-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "  Transport Type" << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Word 223: Transport minor version number\":" << "\"" << HexFormatter::BuffToHexString(&buf[223*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 224-229: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[224*2], 6*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 230-233: Extended Number of User Addressable Sectors (QWord)\":" << "\"" << HexFormatter::BuffToHexString(&buf[230*2], 4*2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 234: Minimum number of 512-byte data blocks per DOWNLOAD MICROCODE command for mode 03h\":" << "\"" << HexFormatter::BuffToHexString(&buf[234*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 235: Maximum number of 512-byte data blocks per DOWNLOAD MICROCODE command for mode 03h\":" << "\"" << HexFormatter::BuffToHexString(&buf[235*2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Word 236-254: Reserved\":" << "\"" << HexFormatter::BuffToHexString(&buf[236*2], 19*2, true) << "h" << "\"," << endl;

    READ_U16(temp, &buf[255*2]);
    sstr << "    \"Word 255: Integrity word\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[255*2], 2, true) << "h" << "\"," << endl;
    temp2 = temp & 0x00FF;
    sstr << "        \"bit 0-7\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "    Checksum Validity Indicator" << "\"," << endl;
    temp2 = temp & 0xFF00;
    sstr << "        \"bit 8-15\":" << "\"" << HexFormatter::BuffToHexString((U8*)&temp2, 1, false) << "h" << "   Checksum" << "\"" << endl;
    sstr << "    }" << endl;

    sstr << "}" << endl;

    sstr << endl;
    sstr << "{" << endl;
    sstr << "\"Word 129-159: Vendor Specific\":\"" << endl;
    sstr << HexFormatter::ToHexString(&buf[129*2], 31*2);
    sstr << "\"," << endl;
    sstr << "\"Word 176-205: Current media serial number\":\"" << endl;
    sstr << HexFormatter::ToHexString(&buf[176*2], 30*2);
    sstr << "\"" << endl;
    sstr << "}" << endl;

    string str = sstr.str();
    return str;
}

string ParseNvmeIdenity(const device_list_t &lists, const NvmeIdenStr &iden)
{
    stringstream sstr;
    U8 *buf;
    U32 temp, pos;
    string s;
    const string CMICtable[6] = {"0 = the NVM subsystem contains only a single PCIExpress port", \
                                 "1 = the NVM subsystem may contain two or more physical PCIExpress ports", \
                                 "0 = the NVM subsystem contains only a single controller", \
                                 "1 = the NVM subsystem may contain two or more controllers",\
                                 "0 = is associated with a PCI Function", \
                                 "1 = is associated with an SR-IOV Virtual Function"};

    const string OAEStable[18] = {"Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "0 = not support sending the Namespace Attribute Notices", \
                                 "1 = supports sending the Namespace Attribute Notices", \
                                 "0 = not support sending sending Firmware Activation Notices", \
                                 "1 = supports sending sending Firmware Activation Notices"};

    const string CTRATTtable[4] = {"0 = not support a 128-bit Host Identifier", \
                                   "1 = supports a 128-bit Host Identifier", \
                                   "0 = not support Non-Operational Power State Permissive Mode", \
                                   "1 = supports Non-Operational Power State Permissive Mode"};

    const string OACStable[18] = {"0 = not support the Security Send and Security Receive commands", \
                                  "1 = supports the Security Send and Security Receive commands", \
                                  "0 = not support the Format NVM command", \
                                  "1 = supports the Format NVM command", \
                                  "0 = not support the Firmware Activate and Firmware Download commands", \
                                  "1 = supports the Firmware Activate and Firmware Download commands",\
                                  "0 = not support the Namespace Management and Namespace Attachment commands",\
                                  "1 = supports the Namespace Management and Namespace Attachment commands", \
                                  "0 = not support the Device Self-test command", \
                                  "1 = supports the Device Self-test command", \
                                  "0 = not support Directives", \
                                  "1 = supports Directives", \
                                  "0 = not support the NVMe-MI Send and NVMe-MI Receive commands", \
                                  "1 = supports the NVMe-MI Send and NVMe-MI Receive commands", \
                                  "0 = not support the Virtualization Management command", \
                                  "1 = supports the Virtualization Management command", \
                                  "0 = not support the Doorbell Buffer Config command", \
                                  "1 = supports the Doorbell Buffer Config command"};

    const string FRMWtable[10] = {"0 = the first firmware slot (slot 1) is read/write", \
                                 "1 = the first firmware slot (slot 1) is read only", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "Reverse", \
                                 "0 = requires a reset for firmware to be activated", \
                                 "1 = supports firmware activation without a reset"};

    const string LPAtable[8] = {"0 = not support the SMART/Health information logpage on a per namespace basis", \
                                "1 = supports the SMART/Health information logpage on a per namespace basis", \
                                "0 = not support the Command Effects log page", \
                                "1 = supports the Command Effects log page", \
                                "0 = not support extended data for Get Log Page (including extended Number of Dwords and Log Page Offset fields)", \
                                "1 = supports extended data for Get Log Page (including extended Number of Dwords and Log Page Offset fields)", \
                                "0 = not support the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices", \
                                "1 = supports the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices"
                                };

    const string AVSCCtable[2] = {"0 = the format of all Admin Vendor Specific Commands are vendor specific", \
                                  "1 = all Admin Vendor Specific Commands use the format defined in this spec"};

    const string APSTAtable[2] = {"0 = not support autonomous power state transitions", \
                                  "1 = supports autonomous power state transitions"};

    const string DSTOtable[2] = {"0 = the NVM subsystem supports one device self-test operation per controller at a time", \
                                 "1 = the NVM subsystem supports only one device self-test operation in progress at a time"};

    const string HCTMAtable[2] = {"0 = not support host controlled thermal management", \
                                  "1 = supports host controlled thermal management"};

    const string SANICAPtable[6] = {"0 = not support the Crypto Erase sanitize operation", \
                                    "1 = supports the Crypto Erase sanitize operation", \
                                    "0 = not support the Block Erase sanitize operation", \
                                    "1 = supports the Block Erase sanitize operation", \
                                    "0 = not support the Overwrite sanitize operation", \
                                    "1 = supports the Overwrite sanitize operation"};

    const string ONCStable[14] = {"0 = not support the Compare command", \
                                  "1 = supports the Compare command", \
                                  "0 = not support the Write Uncorrectable command", \
                                  "1 = supports the Write Uncorrectable command", \
                                  "0 = not support the Dataset Management command", \
                                  "1 = supports the Dataset Management command", \
                                  "0 = not support the Write Zeroes command", \
                                  "1 = supports the Write Zeroes command", \
                                  "0 = not support the Save field in the Set Features command and the Select field in the Get Features command", \
                                  "1 = supports the Save field in the Set Features command and the Select field in the Get Features command", \
                                  "0 = not support reservations", \
                                  "1 = supports reservations", \
                                  "0 = not support the Timestamp feature", \
                                  "1 = supports the Timestamp feature"};

    const string FUSEStable[2] = {"0 =  supports the Compare and Write fused operation", \
                                  "1 =  supports the Compare and Write fused operation"};

    const string FNAtable[6] = {"0 = supports format on a per namespace basis", \
                                "1 = all namespaces shall be configured with the same attributes and a format of any namespace results in a format of all namespaces", \
                                "0 = a cryptographic erase or user data erase as part of a format is performed on a per namespace basis", \
                                "1 = a cryptographic erase or user data erase of a particular namespace as part of a format results in a cryptographic erase or user data erase of all namespaces", \
                                "0 = cryptographic erase is not supported", \
                                "1 = cryptographic erase is supported"};

    const string VWCtable[2] = {"0 = a volatile write cache is not present", \
                                "1 = a volatile write cache is present"};

    const string NVSCCtable[2] = {"0 = the format of all NVM Vendor Specific Commands are vendor specific", \
                                  "1 = all NVM Vendor Specific Commands use the format defined in this spec"};

    const string SGLSSubtable[4] = {"00b = SGLs are not supported", \
                                    "01b = SGLs are supported. There is no alignment nor granularity requirement for Data Blocks", \
                                    "10b = SGLs are supported. There is a Dword alignment and granularity requirement for Data Blocks", \
                                    "11b = Reserved"};

    const string SGLStable[42] = {"Used", \
                                  "Used", \
                                  "Used", \
                                  "Used", \
                                  "0 = not support the Keyed SGL Data Block descriptor", \
                                  "1 = supports the Keyed SGL Data Block descriptor", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "Reverse", \
                                  "0 = the SGL Bit Bucket descriptor is not supported", \
                                  " 1 = the SGL Bit Bucket descriptor is supported", \
                                  "0 = use of a byte aligned contiguous physical buffer of metadata is not supported", \
                                  " 1 = use of a byte aligned contiguous physical buffer of metadata is supported", \
                                  "0 = the SGL length shall be equal to the amount of data to be transferred", \
                                  " 1 = supports commands that contain a data or metadata SGL of a length larger than the amount of data to be transferred", \
                                  "0 = use of Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned is not supported", \
                                  " 1 = use of Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned is supported", \
                                  "0 = then the Address fieldspecifying an offset is not supported", \
                                  " 1 = the Address field in SGL Data Block, SGL Segment, and SGL Last Segment descriptor types specifying an offset"};

    buf = (U8*) (&lists.lst[iden.devpos].ctrl);

    // sstr << ">>>>>>>>>>>>>>>>>>>>>>>>>> IDENTIFY Controller Info" << endl;
    sstr << "{" << endl;
    sstr << "    \"PCI Vendor ID\":" << "\"" << HexFormatter::BuffToHexString(buf, 2, true) << "h" << "\"," << endl;
    sstr << "    \"PCI Subsystem Vendor ID\":" << "\"" << HexFormatter::BuffToHexString(&buf[2], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Serial Number\":" << "\"" << iden.sn << "\"," << endl;
    sstr << "    \"Model Number\":" << "\"" << iden.mn << "\"," << endl;
    sstr << "    \"Firmware Revision\":" << "\"" << iden.fr << "\"," << endl;
    sstr << "    \"Recommended Arbitration Burst\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[72], 1, true) << "h" << "\"," << endl;
    sstr << "    \"IEEE OUI Identifier\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[73], 3, true) << "h" << "\"," << endl;

    temp = lists.lst[iden.devpos].ctrl.cmic;
    sstr << "    \"Controller Multi-Path I/O and Namespace Sharing Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[76], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 2, CMICtable, true);

    sstr << "    \"Maximum Data Transfer Size\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[77], 1, true) << "h" << "\"," << endl;
    sstr << "    \"Controller ID\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[78], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Version\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[80], 4, true) << "h" << "\"," << endl;
    sstr << "    \"RTD3 Resume Latency\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[84], 4, true) << "h" << "\"," << endl;
    sstr << "    \"RTD3 Entry Latency\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[88], 4, true) << "h" << "\"," << endl;

    temp = le32_to_cpu(lists.lst[iden.devpos].ctrl.oaes);
    sstr << "    \"Optional Asynchronous Events Supported\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[92], 4, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 8, 2, OAEStable, true);

    temp = le32_to_cpu(lists.lst[iden.devpos].ctrl.ctratt);
    sstr << "    \"Controller Attributes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[96], 4, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 2, CTRATTtable, true);

    sstr << "    \"FRU Globally Unique Identifier\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[122], 16, true) << "h" << "\"," << endl;
    sstr << "    \"NVMe Management Interface Specification\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[240], 16, true) << "h" << "\"," << endl;

    temp = le16_to_cpu(lists.lst[iden.devpos].ctrl.oacs);
    sstr << "    \"Optional Admin Command Support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[256], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 9, OACStable, true);

    sstr << "    \"Abort Command Limit\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[258], 1, true) << "h" << "\"," << endl;
    sstr << "    \"Asynchronous Event Request Limit\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[259], 1, true) << "h" << "\"," << endl;

    temp = lists.lst[iden.devpos].ctrl.frmw;
    sstr << "    \"Firmware Updates\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[260], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, FRMWtable, false);
    sstr << "        \"Firmware Slot\":" << "\"" << ((lists.lst[iden.devpos].ctrl.frmw >> 1) & 0x07) << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 4, 1, FRMWtable, true);

    temp = lists.lst[iden.devpos].ctrl.lpa;
    sstr << "    \"Log Page Attributes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[261], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 4, LPAtable, true);

    sstr << "    \"Error Log Page Entries\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[262], 1, true) << "h" << "\"," << endl;
    sstr << "    \"Number of Power States Support\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[263], 1, true) << "h" << "\"," << endl;

    temp = lists.lst[iden.devpos].ctrl.avscc;
    sstr << "    \"Admin Vendor Specific Command Configuration\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[264], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, AVSCCtable, true);

    temp = lists.lst[iden.devpos].ctrl.apsta;
    sstr << "    \"Autonomous Power State Transition Attributes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[265], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, APSTAtable, true);

    sstr << "    \"Warning Composite Temperature Threshold\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[266], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Critical Composite Temperature Threshold\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[268], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Maximum Time for Firmware Activation\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[270], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Host Memory Buffer Preferred Size\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[272], 4, true) << "h" << "\"," << endl;
    sstr << "    \"Host Memory Buffer Minimum Size\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[276], 4, true) << "h" << "\"," << endl;
    sstr << "    \"Total NVM Capacity\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[280], 16, true) << "h" << "\"," << endl;
    sstr << "    \"Unallocated NVM Capacity\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[296], 16, true) << "h" << "\"," << endl;

    temp = le32_to_cpu(temp = lists.lst[iden.devpos].ctrl.rpmbs); 
    sstr << "    \"Replay Protected Memory Block Support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[312], 4, true) << "h" << "\"," << endl;
    sstr << "        \"Number of RPMB Units\":" << "\"" << (temp &0x00000003) << "\"," << endl;
    s = ((temp >> 3) & 0x00000007)? "Reserved" : "HMAC SHA-256";
    sstr << "        \"Authentication Method\":" << "\"" << ((temp >> 3) & 0x00000007) << ": " << s << "\"," << endl;
    sstr << "        \"Total Size\":" << "\"" << ((temp >> 16) & 0x000000FF) << "\"," << endl;
    sstr << "        \"Access Size\":" << "\"" << ((temp >> 24) & 0x000000FF) << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Extended Device Self-test Time\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[316], 2, true) << "h" << "\"," << endl;

    temp = lists.lst[iden.devpos].ctrl.dsto;
    sstr << "    \"Device Self-test Options\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[318], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, DSTOtable, true);

    sstr << "    \"Firmware Update Granularity\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[319], 1, true) << "h" << "\"," << endl;
    sstr << "    \"Keep Alive Support\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[320], 1, true) << "h" << "\"," << endl;

    temp = le16_to_cpu(lists.lst[iden.devpos].ctrl.hctma);
    sstr << "    \"Host Controlled Thermal Management Attributes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[322], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, HCTMAtable, true);

    sstr << "    \"Minimum Thermal Management Temperature\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[324], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Maximum Thermal Management Temperature\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[326], 2, true) << "h" << "\"," << endl;

    temp = le16_to_cpu(lists.lst[iden.devpos].ctrl.sanicap);
    sstr << "    \"Sanitize Capabilities\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[328], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 3, SANICAPtable, true);

    temp = lists.lst[iden.devpos].ctrl.sqes;
    sstr << "    \"Submission Queue Entry Size\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[512], 1, true) << "h" << "\"," << endl;
    sstr << "        \"Maximum Size\":" << "\"" << (temp & 0x0000000F) << "\"," << endl;
    sstr << "        \"Required Size\":" << "\"" << ((temp >> 4) & 0x0000000F) << "\"" << endl;
    sstr << "    }," << endl;

    temp = lists.lst[iden.devpos].ctrl.cqes;
    sstr << "    \"Completion Queue Entry Size\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[513], 1, true) << "h" << "\"," << endl;
    sstr << "        \"Maximum Size\":" << "\"" << (temp & 0x0000000F) << "\"," << endl;
    sstr << "        \"Required Size\":" << "\"" << ((temp >> 4) & 0x0000000F) << "\"" << endl;
    sstr << "    }," << endl;

    sstr << "    \"Maximum Outstanding Commands\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[514], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Number of Namespaces\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[516], 4, true) << "h" << "\"," << endl;

    temp = le16_to_cpu(lists.lst[iden.devpos].ctrl.oncs);
    sstr << "    \"Optional NVM Command Support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[520], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 7, ONCStable, true);

    temp = le16_to_cpu(lists.lst[iden.devpos].ctrl.fuses);
    sstr << "    \"Fused Operation Support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[522], 2, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, FUSEStable, true);

    temp = lists.lst[iden.devpos].ctrl.fna;
    sstr << "    \"Format NVM Attributes\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[524], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 3, FNAtable, true);

    temp = lists.lst[iden.devpos].ctrl.vwc;
    sstr << "    \"Volatile Write Cache\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[525], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, VWCtable, true);

    sstr << "    \"Atomic Write Unit Normal\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[526], 2, true) << "h" << "\"," << endl;
    sstr << "    \"Atomic Write Unit Power Fail\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[528], 2, true) << "h" << "\"," << endl;

    temp = lists.lst[iden.devpos].ctrl.nvscc;
    sstr << "    \"VNVM Vendor Specific Command Configuration\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[530], 1, true) << "h" << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 0, 1, NVSCCtable, true);

    sstr << "    \"Atomic Compare 0 Write Unit\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[532], 2, true) << "h" << "\"," << endl;

    temp = le32_to_cpu(lists.lst[iden.devpos].ctrl.sgls);
    sstr << "    \"SGL Support\":{" << endl;
    sstr << "        \"Value\":" << "\"" << HexFormatter::BuffToHexString(&buf[536], 4, true) << "h" << "\"," << endl;
    pos = (temp & 0x00000003); 
    sstr << "        \"bit 1:0\":" << "\"" << SGLSSubtable[pos] << "\"," << endl;
    sstr << BuildIdentifylevel2(temp, 2, 1, SGLStable, false);
    sstr << BuildIdentifylevel2(temp, 16, 5, SGLStable, true);

    sstr << "    \"NVM Subsystem NVMe Qualified Name\":" << "\"" <<  HexFormatter::BuffToHexString(&buf[768], 256, false) << "\"" << endl;
    sstr << "}" << endl;

    sstr << BuildPowerDescriptor(lists, iden) << endl;

    sstr << "{" << endl << "\"Vendor Specific\":\"" << endl ;
    sstr << HexFormatter::ToHexString(&buf[3072], 1024);
    sstr << "\"" << endl << "}" << endl;

    return sstr.str();
}

// string BuildIdentifylevel2(U32 data, U16 start, U16 count, const string *table, bool isend)
// {
//     stringstream sstr;
//     U32 i, end, pos, sh = 1;

//     end = start + count;
//     for(i = start; i < end; i++)
//     {
//         pos = ((data & (sh << i)) >> i) + i * 2;
//         sstr << "        \"bit" << i << "\":" << "\"" << table[pos] << "\"";
//         if((end - 1) != i || !isend)
//             sstr << "," << endl;
//         else
//             sstr << endl;
//     }
//     if(isend)
//         sstr << "    }," << endl;

//     return sstr.str();
// }

string BuildIdentifylevel2(U32 data, U16 start, U16 count, const string *table, bool isend)
{
    stringstream sstr;
    U32 i, end, pos, sh = 1;
    U32 temp;

    end = start + count;
    for(i = start; i < end; i++)
    {
        temp = ((data & (sh << i)) >> i);
        pos = i * 2;
        sstr << "        \"bit " << i << "\":" << "\"" << temp << "b  " << table[pos] << endl;
        sstr << "                     " << table[pos + 1] << "\"";
        if((end - 1) != i || !isend)
            sstr << "," << endl;
        else
            sstr << endl;
    }
    if(isend)
        sstr << "    }," << endl;

    return sstr.str();
}

string BuildPowerDescriptor(const device_list_t &lists, const NvmeIdenStr &iden)
{
    stringstream sstr;
    U32 i, pos , temp;
    U8 *buf;

    sstr << endl << "{" << endl;
    sstr << "\"Power State Descriptors\":" << "{" << endl;

    sstr << "    \"NOPS\":" << "\"Non-Operational State\"" << "," << endl;
    sstr << "    \"MPS\":"  << "\"Max Power Scale (0: in 0.01 Watts; 1: in 0.0001 Watts)\"" << "," << endl;
    sstr << "    \"ENLAT\":"  << "\"Entry Latency in microseconds\"" << "," << endl;
    sstr << "    \"RWL\":"  << "\"Relative Write Latency\"" << "," << endl;
    sstr << "    \"RRL\":"  << "\"Relative Read Latency\"" << "," << endl;
    sstr << "    \"IPS\":"  << "\"Idle Power Scale (00b: Not reported; 01b: 0.0001 W; 10b: 0.01 W; 11b: Reserved)\"" << "," << endl;
    sstr << "    \"APS\":"  << "\"Active Power Scale (00b: Not reported; 01b: 0.0001 W; 10b: 0.01 W; 11b: Reserved)\"" << "," << endl;
    sstr << "    \"ACTP\":"  << "\"Active Power\"" << "," << endl;
    sstr << "    \"MP\":"  << "\"Maximum Power\"" << "," << endl;
    sstr << "    \"EXLAT\":"  << "  Exit Latency in microseconds\"" << "," << endl;
    sstr << "    \"RWT\":"  << "\"Relative Write Throughput\"" << "," << endl;
    sstr << "    \"RRT\":"  << "\"Relative Read Throughput\"" << "," << endl;
    sstr << "    \"IDLP\":" << "\"Idle Power\"" << "," << endl;
    sstr << "    \"APW\":"  << "\"Active Power Workload\"" << "," << endl;
    sstr << "    \"Ofs\":"  << "\"BYTE Offset\"" << "," << endl;

    sstr << "    \"Power State Descriptors Details\":\"" << endl;

    // with BYTE #
    // sstr << FRMT_STR(9, "BYTE # ") << FRMT_STR(6, "Entry") << FRMT_STR(10, "0fs 00-03") << FRMT_STR(5, "NOPS") << FRMT_STR(4, "MPS") << FRMT_STR(6, "MP") \
          << FRMT_STR(10, "ENLAT") << FRMT_STR(10, "EXLAT") << FRMT_STR(10, "0fs 12-15") << FRMT_STR(4, "RWL") <<FRMT_STR(4, "RWT") << FRMT_STR(4, "RRL") \
          << FRMT_STR(4, "RRT") << FRMT_STR(10, "0fs 16-19") << FRMT_STR(4, "IPS") << FRMT_STR(6, "IDLP") <<FRMT_STR(10, "0fs 20-23") << FRMT_STR(4, "APS") \
          << FRMT_STR(4, "APW") << FRMT_STR(6, "ACTP") << endl;

    // sstr << FRMT_STR(9, "=========") << FRMT_STR(6, "=====") << FRMT_STR(10, "=========") << FRMT_STR(5, "====") << FRMT_STR(4, "===") << FRMT_STR(6, "=====") \
          << FRMT_STR(10, "=========") << FRMT_STR(10, "=========") << FRMT_STR(10, "=========") << FRMT_STR(4, "===") <<FRMT_STR(4, "===") << FRMT_STR(4, "===") \
          << FRMT_STR(4, "===") << FRMT_STR(10, "=========") << FRMT_STR(4, "===") << FRMT_STR(6, "=====") <<FRMT_STR(10, "=========") << FRMT_STR(4, "===") \
          << FRMT_STR(5, "====") << FRMT_STR(6, "=====") << endl;

    // without BYTE #
    sstr << FRMT_STR(6, "Entry") << FRMT_STR(10, "0fs 00-03") << FRMT_STR(5, "NOPS") << FRMT_STR(4, "MPS") << FRMT_STR(6, "MP") \
         << FRMT_STR(10, "ENLAT") << FRMT_STR(10, "EXLAT") << FRMT_STR(10, "0fs 12-15") << FRMT_STR(4, "RWL") <<FRMT_STR(4, "RWT") << FRMT_STR(4, "RRL") \
         << FRMT_STR(4, "RRT") << FRMT_STR(10, "0fs 16-19") << FRMT_STR(4, "IPS") << FRMT_STR(6, "IDLP") <<FRMT_STR(10, "0fs 20-23") << FRMT_STR(4, "APS") \
         << FRMT_STR(5, "APW") << FRMT_STR(6, "ACTP") << endl;

    sstr << FRMT_STR(6, "=====") << FRMT_STR(10, "=========") << FRMT_STR(5, "====") << FRMT_STR(4, "===") << FRMT_STR(6, "=====") \
         << FRMT_STR(10, "=========") << FRMT_STR(10, "=========") << FRMT_STR(10, "=========") << FRMT_STR(4, "===") <<FRMT_STR(4, "===") << FRMT_STR(4, "===") \
         << FRMT_STR(4, "===") << FRMT_STR(10, "=========") << FRMT_STR(4, "===") << FRMT_STR(6, "=====") <<FRMT_STR(10, "=========") << FRMT_STR(4, "===") \
         << FRMT_STR(5, "====") << FRMT_STR(6, "=====") << endl;

    for(i = 0; i < 32; i++)
    {
        stringstream sst;
        pos = 2080 + i * 32 - 1;
        sst << pos - 31 << "-" << pos;
        // without BYTE #
        // sstr << FRMT_STR(9, sst.str()); 
        sstr << FRMT_STR(6, to_string(i));
        sst.str("");
        buf = (U8*) (&lists.lst[iden.devpos].ctrl.psd[i]);
        sst << HexFormatter::BuffToHexString(&buf[0], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        temp = lists.lst[iden.devpos].ctrl.psd[i].flags;
        sst.str("");
        sst << ((U8)temp & 0x02) << "b"; 
        sstr << FRMT_STR(5, sst.str());
        sst.str("");
        sst << ((U8)temp & 0x01) << "b";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[0], 2, true) << "h";
        sstr << FRMT_STR(6, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[4], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[8], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[12], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[15], 1, true) << "h";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[14], 1, true) << "h";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[13], 1, true) << "h";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[12], 1, true) << "h";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[16], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        sst.str("");
        temp = lists.lst[iden.devpos].ctrl.psd[i].idle_scale;
        sst << (((U8)temp >> 6) & 0x01) << (((U8)temp >> 7) & 0x01) << "b";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[16], 2, true) << "h";
        sstr << FRMT_STR(6, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[20], 4, true) << "h";
        sstr << FRMT_STR(10, sst.str());
        sst.str("");
        temp = lists.lst[iden.devpos].ctrl.psd[i].active_work_scale;
        sst << (((U8)temp >> 6) & 0x01) << (((U8)temp >> 7) & 0x01) << "b";
        sstr << FRMT_STR(4, sst.str());
        sst.str("");
        temp = lists.lst[iden.devpos].ctrl.psd[i].active_work_scale;
        sst << (((U8)temp) & 0x01) << (((U8)temp >> 1) & 0x01) << (((U8)temp >> 2) & 0x01) << "b";
        sstr << FRMT_STR(5, sst.str());
        sst.str("");
        sst << HexFormatter::BuffToHexString(&buf[20], 2, true) << "h";
        sstr << FRMT_STR(6, sst.str()) << endl;
    }

    sstr << "\"" << endl << "}" << endl << "}" << endl; 

    return sstr.str();
}

string ParseNvmeFirmwareSlot(const firmware_slot_t &fs)
{
    stringstream sstr;
    U8 afi = fs.data.afi;
    string temp;
    int i;

    sstr << "Firmware Slot Infomation \n";
    sstr << "Firmware slot from which the actively running firmware revision was loaded     :  " << (afi & 0x07) << endl;
    sstr << "Firmware slot that is going to be activated at the next controller reset       :  " << ((afi >> 4) & 0x07) << "h (0h indicate this field is not activated)" << endl;

    for(i = 0; i < 7; i++)
    {
        temp = (char *)(&fs.data.frs[i]);
        sstr << "Firmware Revision for Slot " << i << "                                                   :  " << temp.substr(0, 8) << endl;
    }

    return sstr.str();

}

string toPrecisionString(const long double &number, const U16 &pre)
{
    stringstream sstr;
    sstr.precision(pre);
    sstr << fixed << number;

    return sstr.str();
}

string parseDetailSataSmart(const sSmartInfo &info, const eVendorCode code)
{
    stringstream sstr;

    if(eToshibaDevice != code)
    {
        sstr << "[ERROR] This drive has not supported detail smart infomation yet\n";
        return sstr.str();
    } 
    else
    {
        tAttributeMap::const_iterator iter;
        U32 count = 0;
        sstr << "{" << endl;
        for (iter = info.AttrMap.begin(); iter != info.AttrMap.end(); ++iter)
        {
            const sAttribute& attr = iter->second;

            ASSERT (iter->first == attr.ID);
            count ++;
            bool isend = (count == info.AttrMap.size())? true : false;
            sstr << mapDetailSataSmart(attr, code, isend);
        }
        sstr << "}" << endl;
    }

    return sstr.str();
}

string mapDetailSataSmart(const sAttribute& attr, const eVendorCode code, const bool isend)
{
    stringstream sstr;
    switch(code)
    {
        case eToshibaDevice:
        {
            U8 pos = getDetailSataPos(attr.ID, code);
            sstr << toToshibaDetailString(attr, pos, isend);
            break;
        }
        default:
            break;
    }

    return sstr.str();
}

string toToshibaDetailString(const sAttribute &attr, const U8 pos, const bool isend)
{
    stringstream sstr;
    const U8 mapsmtype[20] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 3, 0, 0, 0};
    // 0 normal
    // 1 SSD Failure Mode Status
    // 2 Temperature
    // 3 Thermal Throttle Status
    const string SSDfailuretable[4] = {"0 : Good",\
                                       "1 : 9PLP Fail", \
                                       "2 : No Free block", \
                                       "3 : Exeption fail"};
    const string mapunit[20] = {"", " hours", " cycles", "",\
                         "", " %", " times", " times",\
                         " times", "", "", "",\
                         "", "", "", "",\
                         "", " (x65536 sectors written)", " (x65536 sectors read)", " GiB"};

    sstr << "    \"" << attr.Name << "\":{" << endl;
    sstr << "        \"Worst\":\"" << (U32)attr.Worst << "\"," << endl;
    sstr << "        \"Threshold\":\"" << (U32)attr.Threshold << "\"," << endl;

    switch(mapsmtype[pos])
    {
        case 0:
        {
            U64 temp = ((U64) attr.HighRaw << 32) | attr.LowRaw;
            sstr << "        \"Value\":\"" << temp << mapunit[pos] << "\"" << endl;
            break;
        }
        case 1:
        {
            sstr << "        \"Value\":{" << endl;
            U32 temp = (attr.LowRaw >> 16) & 0xFFFF;
            sstr << "            \"Detection of failure mode\":\"" << SSDfailuretable[temp] << "\"," << endl;
            temp = attr.HighRaw & 0xFFFF;
            sstr << "            \"Lifetime number of test\":\"" << temp << "\"" << endl;
            sstr << "    }" << endl;
            break;
        }
        case 2:
        {
            sstr << "        \"Value\":{" << endl;
            U32 temp = attr.LowRaw & 0xFFFF;
            sstr << "            \"Current temperature\":\"" << temp << " C\"," << endl;
            temp = (attr.LowRaw >> 16) & 0xFFFF;
            sstr << "            \"Minimum temperature\":\"" << temp << " C\"," << endl;
            temp = attr.HighRaw & 0xFFFF;
            sstr << "            \"Maximum temperature\":\"" << temp << " C\"" << endl;
            sstr << "        }" << endl;
            break;
        }
        case 3:
        {
            sstr << "        \"Value\":{" << endl;
            U8 temp = (U8)attr.LowRaw;
            sstr << "            \"Throttling status\":\"" << ((U32)temp & 0xFF) << " %\"," << endl;
            temp = attr.HighRaw & 0xFF;
            U32 temp64 = ((U32)temp << 24) |  ((attr.LowRaw >> 8) & 0xFFFF);
            sstr << "            \"Number of times throttling activated\":\"" <<  temp64 << " times\"" << endl;
            sstr << "        }" << endl;
        }
        default:
            break;
    }

    if(isend)
        sstr << "    }" << endl;
    else
        sstr << "    }," << endl;

    return sstr.str();
}

U8 getDetailSataPos(const U8 id, const eVendorCode code)
{
    const U8 maptoshiba[20] = {5, 9, 12, 173, 175, 179, 181, 182, 183, 184, 187, 190, 192, 194, 197, 199, 234, 241, 242, 249};
    U8 ret = 0;

    switch(code)
    {
        case eToshibaDevice:
        {
            for(int i = 0; i < 20; i++)
                if(id == maptoshiba[i]) { ret = i; break; }
            break;
        }
        default:
            break;
    }

    return ret;
}
