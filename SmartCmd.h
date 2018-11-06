#ifndef SMARTCMD_H
#define SMARTCMD_H

#include "utility/misc/StdHeader.h"
#include "utility/device/Device.h"
#include "utility/device/DeviceUtil.h"
#include "NvmeData.h"
#include "AppConfigFile.h"

class SmartCmd
{
public:
    SmartCmd();
    ~SmartCmd();
    void reset();

public:
    string m_AppName;
    string m_TestName;

    AppConfigFile m_Config;
    
    vector<string> m_ValidVirtiumUsbModel;
    vector<string> m_ValidVirtiumCfModel;

public:
    static const U32 MIN_SAMPLING_RATE = 15;
    static const U32 MAX_SAMPLING_RATE = 1440;

public:
    int execTool();
    void setToolInfo(int argc, char* argv[]);

public:
    void scanDrive();
    void syncConfig();
    void updateRawState();
    void updateDriveState();
    void updateDriverHeader();
    void updateSATAState(const string &devpath);
    void updateNvmeSate(const string &devpath);

    void printHelp();
    void printDevice();
    void printLicense();
    void printVersion();
    void printDebugSmart(const string& driveName);
    void printDebugSataSmart(const string &driveName);
    void printDebugNvmeSmart(const string &driveName);
    void printDebugIdentify(const string& driveName);
    void printDebugNvmeIdentify(const string& driveName);
    void printDebugSataIdentify(const string& driveName);
    void printUserSmart(const string& driveName);
    void printUserNvmeSmart(const string &driveName);
    void printUserSataSmart(const string &driveName);
    void printUserIdentify(const string& driveName);
    void printUserNvmeIdentify(const string &driveName);
    void printUserSataIdentify(const string &driveName);
    void printError(eScanError error, bool verboseMode);
    void printWarning(const set<string>& hiddenSet, bool verboseMode);
    void printConfig(const string& fileName, bool dMode = false);
    void PrintDetailIdentify(const string &driveName);
    void PrintNvmeVendorLog(const string &driveName);
    void PrintNvmeFwSlot(const string &driveName);
    void PrintDetailNvmeFwSlot(const string &driveName);
    void PrintDetailSataSmart(const string &driveName);

    void printPrimeHelp();
    void saveVirtiumPrimeLog(const string& driveName);
    void setVirtiumFwSamplingRateSmartLog(const string& driveName, const char* sampleRate);
    void getVirtiumFwSamplingRateInfo(const string& driveName);
    void saveWorkloadLog(const string& driveName);


private:
    bool isSupportSelfSmartLog(U8* pSmartData);
    bool isVirtiumUsbDevice(const string &model);
    bool isVirtiumCfDevice(const string &model);
    U8* convertSmartLogEntry(const U8* pBuffer, sVirtiumSmartLogEntryUnmapped& entry);
    bool getSmartLogSampleRate(Device& smartDev, U16& sampleRate);
    bool makeFileName(Device* pSmiDev, string prefix, string posfix, string extention, string& fileName);
    bool isNvmeDevice(const string &driveName);
};

#endif // SMARTCMD_H
