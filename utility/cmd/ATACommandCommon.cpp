#include "ATACommand.h"

string ATACommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "ATACommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}

string ATACommandDirect::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "ATACommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}
