#ifndef APPDATA_H
#define APPDATA_H

#include "core/CoreData.h"

typedef list<sDriveInfo> tDriveList;
typedef list<sRawDriveInfo> tRawDriveList;

struct sAppData
{
public:
    string HistoryFile;
    tDriveList LiveInfo;
    tRawDriveList RawLiveInfo;
    set<string> DeviceSet; // List of valid device path
    set<string> SerialSet; // List of valid serial number

    U32 UpdateTime;
    U32 UpdateCycle;

    bool RawOutput;
    bool VerboseMode;

    sDeviceData HistoryData;
    string ActiveSerial;
    sDeviceInfo* ActiveDevicePtr;

public:
    sAppData();
    void save();
    void reset();

public:
    string toString() const;
    string toVerboseString() const;
    bool isValidHistory() const;
    bool getHiddenDevice(set<string>& hiddenSet) const;
};

struct sAppConfig
{
public:
    sAppConfig();
    void reset();
};

struct sApplicationData
{
public:
    sAppData* Data;
    sAppConfig* Config;

public:
    void setApplicationInfo(sAppConfig* config, sAppData* data);
};

extern sApplicationData AppData;

sAppData* GetAppDataPtr();
sAppConfig* GetAppConfigPtr();
sApplicationData* GetApplicationDataPtr();

void LoadCoreData();

#endif // APPDATA_H
