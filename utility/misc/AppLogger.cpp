#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"

#include "AppLogger.h"

AppLogger::AppLogger()
{
    reset();
}

AppLogger::AppLogger(const string &logName)
{
    reset();
    LogName = logName;
}

void AppLogger::reset()
{
    LogName = "";
    CurrentLogLine = 0;
    CurrentLogPart = 0;
    MaxStreamPartition = 0;
}

void AppLogger::setLogName(const string &logName)
{
    LogName = logName;
}

string AppLogger::GetTimeString()
{
    // Get current time in hour:min:sec format
    string timeString = MiscUtil::FormatTime("[%s]", "%Y%m%d %H:%M:%S", time(0));

    return timeString;
}

string AppLogger::getStreamName()
{
    stringstream sstr;
    sstr
        << "AppLogger."
        << LogName
        << ".Part"
        << FRMT_U32(2, CurrentLogPart)
        << ".txt";

    return sstr.str();
}

void AppLogger::createLogStream()
{
    std::fstream &lstr = LogStream;
    if (!lstr.is_open())
    {
        lstr.open(getStreamName().c_str(), std::ios::out);
    }
}

void AppLogger::openLogStream()
{
    std::fstream &lstr = LogStream;
    if (!lstr.is_open())
    {
        lstr.open(getStreamName().c_str(), std::ios::app);
    }
}

void AppLogger::closeLogStream()
{
    std::fstream &lstr = LogStream;
    if (lstr.is_open())
    {
        lstr.close();
    }
}

void AppLogger::flushLogStream()
{
    std::fstream &lstr = LogStream;
    if (lstr.is_open())
    {
        lstr.flush();
    }
}

fstream& AppLogger::getStreamForLogging()
{
    fstream &lstr = LogStream;

    bool isOpened = false;
    if (false == lstr.is_open())
    {
        ASSERT (CurrentLogPart == 0);
        ASSERT (CurrentLogLine == 0);
    }
    else
    {
        CurrentLogLine++;
        // Already opening. checking for switching to next part
        if (CurrentLogLine >= MAX_LOG_LINE)
        {
            CurrentLogPart++;
            if (0 != MaxStreamPartition)
            {
                // Rolling the partition-file
                CurrentLogPart %= MaxStreamPartition;
            }

            CurrentLogLine = 0;

            lstr << "# End of stream partition " << endl;

            lstr.flush();
            lstr.close();
        }
        else
        {
            isOpened = true;
        }
    }

    if (false == isOpened)
    {
        lstr.open(getStreamName().c_str(), ios::out | ios::app);
        lstr << endl << string(100, '#') << endl;
    }

    lstr << GetTimeString() << " ";

    return lstr;
}

void AppLogger::dumpLog(const string& textLog, const string& prefixLog, const string& postfixLog)
{
    if (0 == LogName.length()) return;

    fstream &lstr = getStreamForLogging();

    lstr << prefixLog << textLog;

    if (0 != postfixLog.length())
    {
        lstr << " # " << postfixLog;
    }

    lstr << endl;
    flushLogStream();
}

void AppLogger::dumpLineLog(const string& textLog, const string& postfixLog)
{
    dumpLog(textLog, "TextLog: ", postfixLog);
}

void AppLogger::dumpBlockLog(const string& textLog, const string& postfixLog)
{
    dumpLog(textLog, "TextLog: \n", postfixLog);
}
