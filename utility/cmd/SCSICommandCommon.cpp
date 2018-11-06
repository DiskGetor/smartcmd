#include "SCSICommand.h"

string SCSICommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "SCSICommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}
