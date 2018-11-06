#include "utility/misc/StringUtil.h"

#include "AppData.h"

sApplicationData AppData;

// ----------------------------------------------------------------------
// Implementation of common functions
// ----------------------------------------------------------------------

void sApplicationData::setApplicationInfo(sAppConfig *config, sAppData *data)
{
    Config = config;
    Data = data;
}

sAppData* GetAppDataPtr()
{
    return AppData.Data;
}

sAppConfig* GetAppConfigPtr()
{
    return AppData.Config;
}

sApplicationData* GetApplicationDataPtr()
{
    return &AppData;
}

void LoadCoreData()
{
    sApplicationData* applptr = GetApplicationDataPtr();

    applptr->setApplicationInfo(new sAppConfig, new sAppData);
}

// ----------------------------------------------------------------------
// Implementation of AppData
// ----------------------------------------------------------------------

sAppData::sAppData()
{
    reset();
}

void sAppData::reset()
{
    UpdateCycle = 0;
    HistoryFile = "";
    ActiveSerial = "";
    ActiveDevicePtr = NULL;
    VerboseMode = false;
    RawOutput = false;

    HistoryData.reset();
    LiveInfo.clear();
    SerialSet.clear();
    DeviceSet.clear();
}

void sAppData::save()
{
    // Save history data
    SaveHistory(HistoryData, HistoryFile);
}

string sAppData::toString() const
{
    stringstream sstr;
    sstr << "HistoryFile: " << HistoryFile << endl;

    if(1) {
        sstr << "LiveInfo: " << endl;
        tDriveList::const_iterator iter;
        for (iter = LiveInfo.begin(); iter != LiveInfo.end(); iter++)
        {
            const sDriveInfo& info = *iter;
            sstr << ToVerboseString(info) << endl;
        }
    }

    if(1) {
        sstr << "ValidDevice: " << endl;
        tSerialSet::const_iterator iter;
        for (iter = SerialSet.begin(); iter != SerialSet.end(); iter++)
        {
            sstr << "- " << *iter << endl;
        }
    }

    if(1) {
        sstr << "DeviceSet: " << endl;
        set<string>::const_iterator iter;
        for (iter = DeviceSet.begin(); iter != DeviceSet.end(); iter++)
        {
            sstr << "- " << *iter << endl;
        }
    }

    if(1) {
        sstr << "UpdateTime: " << UpdateTime << endl;
        sstr << "UpdateCycle: " << UpdateCycle << endl;
    }

    if(1) {
        sstr << "VerboseMode: " << FRMT_BLN(4, VerboseMode) << endl;
    }

    return sstr.str();
}

string sAppData::toVerboseString() const
{
    stringstream sstr;

    sstr << "Drives list: ";
    for (set<string>::const_iterator iter = DeviceSet.begin(); iter != DeviceSet.end(); iter++) sstr << *iter << " ";
    sstr << endl;

    sstr << "Time to run: " << UpdateTime / 3600.0 << " hour(s)" << endl;

    sstr << "Log: " << HistoryFile << endl;

    return sstr.str();
}

bool sAppData::isValidHistory() const
{
    return HistoryFile.length() != 0;
}

bool sAppData::getHiddenDevice(set<string> &hiddenSet) const
{
    // Test LiveInfo vs DeviceSet;

    hiddenSet.clear();

    if (LiveInfo.size() == DeviceSet.size()) return false; // No hidden device found

    set<string> liveSet;

    // Build set of devPath in LiveInfo
    for (list<sDriveInfo>::const_iterator iter = LiveInfo.begin(); iter != LiveInfo.end(); iter++)
    {
        const sDriveInfo& driveInfo = *iter;
        liveSet.insert(driveInfo.IdentifyInfo.DriveName);
    }

    // Checking for lacking device in liveInfo
    for (set<string>::const_iterator iter = DeviceSet.begin(); iter != DeviceSet.end(); iter++)
    {
        const string& driveName = *iter;
        if(liveSet.end() == liveSet.find(driveName)) hiddenSet.insert(driveName);
    }

    return 0 != hiddenSet.size();
}

// ----------------------------------------------------------------------
// Implementation of AppConfig
// ----------------------------------------------------------------------

sAppConfig::sAppConfig()
{
    reset();
}

void sAppConfig::reset()
{

}

// ----------------------------------------------------------------------
// End
// ----------------------------------------------------------------------
