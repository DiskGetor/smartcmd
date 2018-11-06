#include "utility/misc/StringUtil.h"

#include "ConfigFile.h"

ConfigFile::ConfigFile()
{
    reset();
}

void ConfigFile::reset()
{
    m_FileName = "";
    m_ConfigMap.clear();
}

bool ConfigFile::parseString(const string& config, string& key, string& value)
{
    bool status = false;
    do {
        string line = config;
        StringUtil::LeftTrim(line);

        if (0 == line.length()) break;
        if ('#' == line[0]) break;

        vector<string> subList;
        StringUtil::HardSplit(line, '=', subList);
        if (2 != subList.size()) break;

        key = StringUtil::Trim(subList[0]);
        value = StringUtil::Trim(subList[1]);

        status = true;
    } while(0);

    return status;
}

bool ConfigFile::loadConfig(const string& fileName)
{
    reset();

    m_FileName = fileName;

    string line, key, value;
    fstream fstr(m_FileName.c_str());
    if (!fstr.is_open()) return false;

    while(getline(fstr, line, '\n'))
    {
        if (true == parseString(line, key, value)) addConfig(key, value);
    }

    return true;
}

bool ConfigFile::loadContent(const string& content)
{
    reset();

    string line, key, value;
    stringstream fstr(content);

    while(getline(fstr, line, '\n'))
    {
        if (true == parseString(line, key, value)) addConfig(key, value);
    }

    return true;
}

string ConfigFile::toString() const
{
    stringstream sstr;

    map<string, string>::const_iterator iter;
    for (iter = m_ConfigMap.begin(); iter != m_ConfigMap.end(); iter++)
    {
        sstr << "Key: " << FRMT_STR(16, iter->first) << " ";
        sstr << "Value: " << iter->second << " " << endl;
    }

    return sstr.str();
}

bool ConfigFile::readConfig(const string &key, bool &value) const
{
    map<string, string>::const_iterator iter = m_ConfigMap.find(key);

    if (iter == m_ConfigMap.end()) return false;

    const string& valueString = iter->second;

    value = false;
    if (0 == strcmp(valueString.c_str(), "TRUE")) value = true;

    return true;
}

bool ConfigFile::readConfig(const string &key, U32 &value) const
{
    map<string, string>::const_iterator iter = m_ConfigMap.find(key);

    if (iter == m_ConfigMap.end()) return false;

    const string& valueString = iter->second;

    value = atoi(valueString.c_str());

    return true;
}

bool ConfigFile::readConfig(const string &key, double &value) const
{
    map<string, string>::const_iterator iter = m_ConfigMap.find(key);

    if (iter == m_ConfigMap.end()) return false;

    const string& valueString = iter->second;

    value = atof(valueString.c_str());

    return true;
}

bool ConfigFile::readConfig(const string &key, string &value) const
{
    map<string, string>::const_iterator iter = m_ConfigMap.find(key);

    if (iter == m_ConfigMap.end()) return false;

    const string& valueString = iter->second;

    value = valueString;

    return true;
}

void ConfigFile::addConfig(const string &key, const string &value)
{
    m_ConfigMap[key] = value;
}
