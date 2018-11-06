#include "utility/misc/StringUtil.h"

#include "AppConfigFile.h"

AppConfigFile::AppConfigFile()
{
    reset();
}

void AppConfigFile::reset()
{
    m_UpdateTime = 0;
    m_UpdateCycle = 0;
    m_AutoRestart = false;
    m_VerboseMode = false;
    m_RawOutput = false;

    m_LogFile = "";
    m_SerialSet.clear();

    m_ErrorState = ERROR_NONE;
}

void AppConfigFile::setErrorState(eErrorState state)
{
    m_ErrorState = state;
}

string AppConfigFile::toString() const
{
    stringstream sstr;

    sstr << "LogFile: " << m_LogFile << endl;
    sstr << "UpdateTime: " << m_UpdateTime << endl;
    sstr << "UpdateCycle: " << m_UpdateCycle << endl;
    sstr << "AutoRestart: " << BOOL_STR(m_AutoRestart);

    int serialCount = m_SerialSet.size();

    if (0 != serialCount)
    {
        sstr << "SerialNumber:" << endl;
        set<string>::const_iterator iter;
        for (iter = m_SerialSet.begin(); iter != m_SerialSet.end(); iter++)
        {
            sstr << "  S/N " << *iter << endl;
        }
    }

    return sstr.str();
}

#define DUMP_LINE(text) sstr << text << endl

string AppConfigFile::GenerateSampleFileString(bool dMode)
{
    string sampleConfig =
        #include "SmartCmd_SampleConfig.txt"
    ;

    string debugConfig =
        #include "SmartCmd_DebugConfig.txt"
    ;

    return dMode ? debugConfig : sampleConfig;
}

string AppConfigFile::toErrorString() const
{
    stringstream sstr;

    switch(m_ErrorState)
    {
        case ERROR_NONE: break;
        case ERROR_CONFIG:  sstr << "Cannot open config file"; break;
        case ERROR_MODE:    sstr << "Lacking Working-Mode parameter"; break;
        case ERROR_TIME:    sstr << "Lacking Total-Testing-Time parameter"; break;
        case ERROR_FREQ:    sstr << "Lacking Frequency parameter"; break;
        case ERROR_LOGFILE: sstr << "Lacking Log-File parameter"; break;
        case ERROR_DEVPATH: sstr << "Lacking Device-Path parameter"; break;
        case ERROR_SERIAL:  sstr << "Lacking Device-Serial parameter"; break;
        case ERROR_MODENAME:sstr << "Invalid Name of Working-Mode"; break;

        case ERROR_FREQ_VALUE:
            sstr << "Frequency value should be greater than 0.0003 (at least one second)"; break;
        default: break;
    }

    return sstr.str();
}

bool AppConfigFile::parseConfig()
{
    #define SET_ERROR(code) { m_ErrorState = code; break; }

    bool status = false;

    reset();

    do
    {
        if(false == readConfig("CONFIG_LOGFILE", m_LogFile)) SET_ERROR(ERROR_LOGFILE)

        string mode;
        if(false == readConfig("CONFIG_MODE", mode)) SET_ERROR(ERROR_MODE)

        bool autoMode  = (0 == strcmp(mode.c_str(), "MANUAL"));
        bool jedecMode = (0 == strcmp(mode.c_str(), "JEDEC"));

        if (false == (autoMode || jedecMode))  SET_ERROR(ERROR_MODENAME)

        if (true == jedecMode)
        {
            m_UpdateTime = 100 * 3600; // 100 hours
            m_UpdateCycle = 24 * 3600; //  24 hours
        }

        if (true == autoMode)
        {
            double updateTime = 0;
            double updateCycle = 0;

            if(false == readConfig("CONFIG_TIME", updateTime)) SET_ERROR(ERROR_TIME)
            if(false == readConfig("CONFIG_FREQ", updateCycle)) SET_ERROR(ERROR_FREQ)

            m_UpdateTime = updateTime * 3600;
            m_UpdateCycle = updateCycle * 3600;
            m_UpdateCycle = MIN2(m_UpdateCycle, 86400);
            m_UpdateTime = MAX2(m_UpdateCycle + 1, m_UpdateTime);

            if (0 == m_UpdateCycle) SET_ERROR(ERROR_FREQ_VALUE);
        }

        string serial;
        if (true == readConfig("CONFIG_SERIAL", serial))
        {
            vector<string> serialList;
            StringUtil::HardSplit(serial, ';', serialList);

            for (int i = 0, serialCount = serialList.size(); i < serialCount; i++)
            {
                m_SerialSet.insert(StringUtil::Trim(serialList[i]));
            }
        }

        string devpath;
        if (true == readConfig("CONFIG_DEVPATH", devpath))
        {
            vector<string> devList;
            StringUtil::HardSplit(devpath, ';', devList);

            for (int i = 0, devCount = devList.size(); i < devCount; i++)
            {
                m_DeviceSet.insert(StringUtil::Trim(devList[i]));
            }
        }
        else SET_ERROR(ERROR_DEVPATH);

        readConfig("CONFIG_CONTINUE", m_AutoRestart);

        readConfig("CONFIG_VERBOSE", m_VerboseMode);

        readConfig("CONFIG_RAWOUTPUT", m_RawOutput);

        status = true;
    } while(0);

    return status;
}
