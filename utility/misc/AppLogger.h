#ifndef APPLOGGER_H
#define APPLOGGER_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

class AppLogger
{
public:
    static const U32 MAX_LOG_LINE = 100000;

public:
    AppLogger();
    AppLogger(const string& logName);
    void reset();

public:
    void createLogStream();
    void setLogName(const string& logName);

public:
    // Logging a message
    void dumpLineLog(const string& textLog, const string& postfixLog = string());
    void dumpBlockLog(const string& textLog, const string& postfixLog = string());

private:
    void dumpLog(const string& textLog, const string& prefixLog, const string& postfixLog);

private:
    void openLogStream();
    void closeLogStream();
    void flushLogStream();

    string getStreamName();
    fstream& getStreamForLogging();

    static string GetTimeString();

private:
    string LogName;
    fstream LogStream;
    U32 CurrentLogLine;
    U32 CurrentLogPart;
    U32  MaxStreamPartition;
};

#endif // APPLOGGER_H
