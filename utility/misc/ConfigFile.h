#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

class ConfigFile
{
public:
    ConfigFile();
    void reset();

    bool loadConfig(const string& fileName);
    bool loadContent(const string& content);
    bool parseString(const string& config, string& key, string& value);
    string toString() const;
    
public:
    bool readConfig(const string& key, U32& value) const;
    bool readConfig(const string& key, bool& value) const;
    bool readConfig(const string& key, double& value) const;
    bool readConfig(const string& key, string& value) const;

    void addConfig(const string& key, const string& value);

public:
    virtual bool parseConfig() = 0;
    
public:
    string m_FileName;
    map<string, string> m_ConfigMap;
};


#endif // CONFIGFILE_H
