#ifndef HYPCOMMAND_H
#define HYPCOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "BaseCommand.h"

enum eHYPCommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "HYPCommandCode.def"
    #undef MAP_ITEM
};

class HYPCommand : public BaseCommand
{
public:
    virtual bool executeCommand(int handle);
    virtual void setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode);
    virtual eCommandError getErrorStatus();
    virtual string getTaskfileString();

private:
    virtual bool buildCommand();      // setting flags, ..
    virtual void buildBasicCommand(); // Initialize basic data

public:
    string getCommandString() const;
};

#endif // HYPCOMMAND_H

