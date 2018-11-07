#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#define STRING_END "End Built Meesage"

class EventLog
{
public:

    struct sMessageParam
    {
        string SyncPattern;
        string Message;
        string Parameter;
    };

public:
    EventLog();

    // return false if don't sync a pattern.
    bool printEventLog(string &eventLogString, U8* pLogBuffer, U8 &logLen);

private:
    U16 getTotalBuiltMessage();
    bool syncPattern(U8 *pBuffer, U16 &syncPatternIndex, U8 &syncLen, vector<S64> &argsList);

    // Print library
    // http://www.menie.org/georges/embedded/printf-stdarg.html
    void printChar(string &outputStr, int c);
    int printString(string &outputStr, const char *string, int width, int pad);
    int printInterger(string &outputStr, S64 i, int b, int sg, int width, int pad, int letbase);
    int print(string &outputStr, const char *format, vector<S64> args);

private:
    static const sMessageParam m_BuiltMessageTable[];
    U16 m_NumberOfBuiltMessage;
};

#endif
