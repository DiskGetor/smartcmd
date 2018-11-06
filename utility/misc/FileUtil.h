#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "StdMacro.h"
#include "StdHeader.h"

class FileUtil
{
public:
    struct sUtilData
    {
        bool m_Initialized;

        string m_PathDelimiter;

        sUtilData();
    };

public:
    static void SystemInit();

public:
    // Group of functions for handling FileName
    static void GetFileName(const string& absName, string& fileName);
    static void GetPathName(const string& absName, string& pathName); // Exclude last delimiter
    static void GetBaseName(const string& absName, string& baseName);
    static void GetExtName (const string& absName, string& extName ); // Exclude "dot"

    static void GetAbsName (const string& pathName, const string& fileName, string& absName);
    static void GetFileName(const string& baseName, const string& extName, string& fileName);

    static void GetDirectory(const string& absName, string& dirName);

    static void SplitPathName(const string& absName, string& pathName, string& fileName);

public:
    static string GetTempFile();

    static U32 GetFileSize(const string& name);

    static bool ReadText(const string& name, string& text);
    static bool WriteText(const string& name, const string& text);

    static bool ReadData(const string& name, U32 offset, U8* buffer, U32 size);
    static bool WriteData(const string& name, const U8* buffer, U32 size);
    static bool AppendData(const string& name, const U8* buffer, U32 size);
    static bool ReadSize(const string& name, U32& size);

private:
    static sUtilData s_Data;
};

#endif //FILEUTIL_H
