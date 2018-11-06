#include "HYPCommand.h"

string HYPCommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "HYPCommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}
