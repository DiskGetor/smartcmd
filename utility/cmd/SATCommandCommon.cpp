#include "SATCommand.h"

string SATCommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "SATCommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}
