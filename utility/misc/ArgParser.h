#ifndef ARGPARSER_H
#define ARGPARSER_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

typedef map<string, vector<string> > tArgMap;

class ArgParser
{
public:
    ArgParser();

    void reset();
    void setArg(int argc, char** argv, int offset = 0);

public:
    bool testParam(const string& key, string& value);

    bool readInt(const string& key, int& val0) const;
    bool readInt(const string& key, int& val0, int& val1) const;
    bool readInt(const string& key, int& val0, int& val1, int& val2) const;

    bool readString(const string& key, string& val0) const;
    bool readString(const string& key, string& val0, string& val1) const;
    bool readString(const string& key, string& val0, string& val1, string& val2) const;

public:
    string toString() const;
    bool searchKey(const string& key, vector<string>& value) const;

private:
    string m_Input;
    tArgMap m_ArgMap;
};

#endif // ARGPARSER_H
