#ifndef VSCSM2246_H
#define VSCSM2246_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"
#include "utility/device/Device.h"

#include "Sm2246.h"

class VscSm2246
{
public:
    VscSm2246();
    ~VscSm2246();
    void reset();

public:
    bool setToolInfo(int argc, char* argv[]);
    int  execTool();

public:
    enum eOption
    {
        OPTION_NULL = 0,
        OPTION_LICENSE,
        OPTION_HELP,
        OPTION_SCAN,
        OPTION_PRINT,
        OPTION_READ_MARKED_BAD_BLOCK,
        OPTION_READ_ORIGINAL_BAD_BLOCK,
        OPTION_READ_TOTAL_BAD_BLOCK,
        OPTION_ERASE_COUNT,
        OPTION_L2P_MAPPING,
        OPTION_READ_LOG,
        OPTION_PARSE_LOG,
        OPTION_READ_CARD_INFO,
        OPTION_READ_CID,
        OPTION_READ_RUNNING_SPARE,
        OPTION_TOTAL
    };

    struct sOption
    {
        eOption Id;
        string Name;
        string Parameter;
        U8 MinimumRequireParameter;
        string Title;
        string FileNamePrefix;
    };

public:
    void printHelp(U8 startIndex, U8 count);
    void scanDevice();
    void printDevice();
    void printDevice(const sOption* pOption, BaseDevice* pSmiDev, string fileName);
    void printLicense();

    void l2PMapping(const sOption* pOption, BaseDevice* pSmiDev, string strStartLBA, string strLbaCount, string fileName);
    void readMarkedBadBlock(const sOption* pOption, BaseDevice* pSmiDev, string fileName);
    void readEraseCountTable(const sOption* pOption, BaseDevice* pSmiDev, string fileName);
    void readBadBlockTable(const sOption* pOption, BaseDevice* pSmiDev, string fileName, bool readOriginalBlock = true);
    void readLog(const sOption* pOption, BaseDevice* pSmiDev, string strStartIndex, string strLogCount, string fileName);
    void parseLog(const sOption* pOption, string logFileName, string password);
    void readParameter(const sOption* pOption, BaseDevice* pSmiDev, string fileName);
    void readCID(const sOption* pOption, BaseDevice* pSmiDev, string fileName);
    void readRunningSpareCount(const sOption* pOption, BaseDevice* pSmiDev, string fileName);

private:
    string makeFileName(BaseDevice* pSmiDev, string prefix, string posfix, string extention);
    U32 getPercent(U32 total);

private:
    vector<BaseDevice*> m_DevList;
    string m_AppName;

    static const string virPassword;
    static const sOption m_BuildInOption[ OPTION_TOTAL ];
};

#endif // VSCSM2246_H

