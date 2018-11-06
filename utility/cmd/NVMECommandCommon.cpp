#include "NVMECommand.h"

string NVMECommand::getCommandString() const
{
    string cmdString = "UnknownCommand";

    switch (CommandCode)
    {
        #define MAP_ITEM(code, val) case code: cmdString = #code; break;
        #include "NVMECommandCode.def"
        #undef MAP_ITEM
    }

    return cmdString;
}
