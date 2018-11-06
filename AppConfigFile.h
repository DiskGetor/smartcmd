#ifndef APPCONFIGFILE_H
#define APPCONFIGFILE_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"
#include "utility/misc/ConfigFile.h"

class AppConfigFile : public ConfigFile
{
public:
    enum eErrorState
    {
        ERROR_NONE,
        ERROR_CONFIG,
        ERROR_LOGFILE,
        ERROR_SERIAL,
        ERROR_DEVPATH,
        ERROR_MODE,
        ERROR_MODENAME,
        ERROR_TIME,
        ERROR_FREQ,
        ERROR_FREQ_VALUE,
    };

public:
    void reset();
    AppConfigFile();
    string toString() const;
    string toErrorString() const;

    virtual bool parseConfig();

    void setErrorState(eErrorState state);

public:
    static string GenerateSampleFileString(bool dMode = false);

public:
    U32 m_UpdateTime;
    U32 m_UpdateCycle;
    bool m_AutoRestart;
    bool m_VerboseMode;
    bool m_RawOutput;

    string m_LogFile;

    set<string> m_SerialSet;
    set<string> m_DeviceSet;

    eErrorState m_ErrorState;
};

#endif // APPCONFIGFILE_H
