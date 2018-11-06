#ifndef SATCOMMAND_H
#define SATCOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "BaseCommand.h"

enum eSATCommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "SATCommandCode.def"
    #undef MAP_ITEM
};

class SATCommand : public BaseCommand
{
public:
    virtual bool executeCommand(int handle);
    virtual void setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode);
    virtual eCommandError getErrorStatus();
    virtual string getTaskfileString();
    virtual U8* getOutputTaskfilePtr();

private:
    virtual bool buildCommand();      // setting flags, ..
    virtual void buildBasicCommand(); // Initialize basic data

public:
    string getCommandString() const;
};

#endif // SATCOMMAND_H

