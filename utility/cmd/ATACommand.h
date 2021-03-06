#ifndef ATACOMMAND_H
#define ATACOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "BaseCommand.h"

enum eATACommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "ATACommandCode.def"
    #undef MAP_ITEM
};

class ATACommand : public BaseCommand
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

class ATACommandDirect : public BaseCommand
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

#endif // ATACOMMAND_H

