#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"

#include "ArgParser.h"

ArgParser::ArgParser()
{
    reset();
}

void ArgParser::reset()
{

}

bool ArgParser::searchKey(const string &key, vector<string> &value) const
{
    tArgMap::const_iterator iter = m_ArgMap.find(key);
    if ( m_ArgMap.end() != iter ) return false;

    const vector<string>& original = iter->second;
    value.reserve(original.size());
    copy(original.begin(),original.end(),back_inserter(value));

    return true;
}

void ArgParser::setArg(int argc, char **argv, int offset)
{
    // debug code
    m_Input = "";
    for (int i = 0; i < argc; i++) { m_Input += argv[i]; m_Input += " "; }

    enum eSearchState {
        SEARCH_KEY,
        SEARCH_VALUE,
    };

    int index = offset;
    eSearchState state = SEARCH_KEY;

    string key;
    vector<string> value;

    while(index <= argc)
    {
        if(index < argc)
        {
            string curarg = argv[index];

            switch(state)
            {
                case SEARCH_KEY:
                {
                    if('-' == curarg[0])
                    {
                        key = curarg;
                        state = SEARCH_VALUE;
                    }

                    index++;
                } break;

                case SEARCH_VALUE:
                {
                    if('-' != curarg[0])
                    {
                        value.push_back(curarg);
                        index++;
                    }
                    else
                    {
                        // complete current key
                        m_ArgMap.insert(pair<string, vector<string> >(key, value));

                        key = "";
                        value.clear();

                        // move to next key
                        state = SEARCH_KEY;
                    }
                } break;

                default: break;
            }
        }
        else
        {
            // Flush the last key:value pair

            if (0 == key.length()) break;

            m_ArgMap.insert(pair<string, vector<string> >(key, value));

            break;
        }
    }
}

string ArgParser::toString() const
{
    stringstream sstr;

    sstr << "Input: " << m_Input << endl;

    tArgMap::const_iterator iter;

    for (iter = m_ArgMap.begin(); iter != m_ArgMap.end(); iter++)
    {
        const string& key = iter->first;
        const vector<string>& value = iter->second;

        sstr << "Key: " << key << endl;
        sstr << "Val: ";

        U32 valueCount = value.size();
        for (U32 i = 0; i < valueCount; i++)
        {
            sstr << value[i] << " ";
        }

        sstr << endl;
    }

    return sstr.str();
}

bool ArgParser::readInt(const string &key, int &val0) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (1 != value.size()) return false;

    val0 = atoi(value[0].c_str());
    return true;
}

bool ArgParser::readInt(const string &key, int &val0, int& val1) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (2 != value.size()) return false;

    val0 = atoi(value[0].c_str());
    val1 = atoi(value[1].c_str());
    return true;
}

bool ArgParser::readInt(const string &key, int &val0, int& val1, int& val2) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (3 != value.size()) return false;

    val0 = atoi(value[0].c_str());
    val1 = atoi(value[1].c_str());
    val2 = atoi(value[2].c_str());
    return true;
}

bool ArgParser::readString(const string &key, string &val0) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (1 != value.size()) return false;

    val0 = value[0];
    return true;
}

bool ArgParser::readString(const string &key, string &val0, string& val1) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (2 != value.size()) return false;

    val0 = value[0];
    val1 = value[1];
    return true;
}

bool ArgParser::readString(const string &key, string &val0, string& val1, string& val2) const
{
    vector<string> value;
    if (false == searchKey(key, value)) return false;

    if (3 != value.size()) return false;

    val0 = value[0];
    val1 = value[1];
    val2 = value[2];
    return true;
}
