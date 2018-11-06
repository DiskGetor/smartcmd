#include "StringUtil.h"
#include "FileUtil.h"

FileUtil::sUtilData FileUtil::s_Data;

FileUtil::sUtilData::sUtilData()
{
    m_PathDelimiter = string("/\\");
    m_Initialized = true;
}

void SystemInit_FileUtil()
{
    FileUtil::SystemInit();
}

void FileUtil::SystemInit()
{
    s_Data.m_PathDelimiter = string("/\\");

    s_Data.m_Initialized = true;
}

// --------------------------------------------------------------------------------
// Handling FileName
// --------------------------------------------------------------------------------
void FileUtil::GetFileName(const string& absName, string& fileName)
{
    // Get fileName from absolute name

    // Search the last delimiter
    size_t delimpos = absName.find_last_of(s_Data.m_PathDelimiter);

    // Position after delimiter
    size_t filepos = (delimpos == string::npos) ? 0 : delimpos + 1;

    // Extract fileName
    fileName = absName.substr(filepos);
}

void FileUtil::GetPathName(const string& absName, string& pathName)
{
    // Get pathName from absolute name

    // Search the last delimiter
    size_t delimpos = absName.find_last_of(s_Data.m_PathDelimiter);
    
    // Extract pathName
    size_t pathcount = (delimpos == string::npos) ? 0 : delimpos;
    pathName = absName.substr(0, pathcount);
}

void FileUtil::GetDirectory(const string &absName, string &dirName)
{
    dirName = "";

    string pathName;
    FileUtil::GetPathName(absName, pathName);

    // Normalize pathName if needed
    vector<string> items;
    vector<string> final;

    StringUtil::HardSplit(pathName, "/", items);

    U32 count = items.size();
    for (U32 i = 0; i < count; i++)
    {
        vector<string> parts;
        StringUtil::HardSplit(items[i], '\\', parts);
        final.insert(final.end(), parts.begin(), parts.end());
    }

    count = final.size();
    for (U32 i = 0; i < count; i++)
    {
        if (0 != final[i].compare(".")) dirName += final[i] + "\\";
    }
}

void FileUtil::GetBaseName(const string& absName, string& baseName)
{
    // Search the last delimiter
    size_t delimpos = absName.find_last_of(s_Data.m_PathDelimiter);

    // Position after delimiter
    size_t basepos = (delimpos == string::npos) ? 0 : delimpos + 1;

    // Search the last dot
    size_t dotpos = absName.find_last_of(".");

    // Extract baseName
    size_t basecount = ((dotpos == string::npos) ? absName.length() : dotpos) - basepos;
    baseName = absName.substr(basepos, basecount);
}

void FileUtil::GetExtName(const string& absName, string& extName)
{
    // Search the last delimiter
    size_t dotpos = absName.find_last_of(".");
    
    // Position after the dot
    size_t extpos = (dotpos == string::npos) ? string::npos : dotpos + 1;

    // Extract extName
    extName = absName.substr(extpos);
}

void FileUtil::SplitPathName(const string &absName, string &pathName, string &fileName)
{
    // Get fileName from absolute name

    // Search the last delimiter
    size_t delimpos = absName.find_last_of(s_Data.m_PathDelimiter);

    // Position after delimiter
    size_t pathpos = (delimpos == string::npos) ? 0 : delimpos;
    size_t filepos = (delimpos == string::npos) ? 0 : delimpos + 1;

    // Extract pathName
    pathName = absName.substr(0, pathpos);

    // Extract fileName
    fileName = absName.substr(filepos);
}

void FileUtil::GetAbsName (const string& pathName, const string& fileName, string& absName)
{
    absName = pathName + "/" + fileName;
}

void FileUtil::GetFileName(const string& baseName, const string& extName, string& fileName)
{
    fileName = baseName + "." + extName;
}

string FileUtil::GetTempFile()
{
    return string(tmpnam(NULL));
}

bool FileUtil::ReadText(const string &name, string &text)
{
    ifstream fstr;
    fstr.open(name.c_str(), ios::in);

    if (false == fstr.is_open()) return false;

    text = string((istreambuf_iterator<char>(fstr)), istreambuf_iterator<char>());

    return true;
}

bool FileUtil::WriteText(const string &name, const string &text)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out);

    if (false == fstr.is_open()) return false;

    fstr << text;

    return true;
}

U32 FileUtil::GetFileSize(const string &name)
{
    ifstream in(name.c_str(), ios::ate | ios::binary);
    return in.tellg();
}

bool FileUtil::ReadData(const string &name, U32 offset, U8* buffer, U32 size)
{
    ifstream fstr;
    fstr.open(name.c_str(), ios::in | ios::binary);

    if (false == fstr.is_open()) return false;

    fstr.seekg(offset);

    fstr.read((char*) buffer, size);

    fstr.close();

    return true;
}

bool FileUtil::WriteData(const string &name, const U8* buffer, U32 size)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out | ios::binary);

    if (false == fstr.is_open()) return false;

    fstr.write((char*) buffer, size);

    fstr.close();

    return true;
}

bool FileUtil::AppendData(const string &name, const U8* buffer, U32 size)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out | ios::binary | ios::app);

    if (false == fstr.is_open()) return false;

    fstr.write((const char*) buffer, size);

    fstr.close();

    return true;
}

bool FileUtil::ReadSize(const string &name, U32 &size)
{
    size = 0;

    ifstream fstr;
    fstr.open(name.c_str(), ios::in | ios::binary);

    if (false == fstr.is_open()) return false;

    fstr.seekg(0, ios_base::end);
    size = fstr.tellg();

    return true;
}
