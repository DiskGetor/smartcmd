#ifndef NVMECOMMAND_H
#define NVMECOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "BaseCommand.h"

enum eNVMECommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "NVMECommandCode.def"
    #undef MAP_ITEM
};

class NVMECommand : public BaseCommand
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
    bool execReadLogPage();
    bool execReadIdentify();

public:
    string getCommandString() const;
};

#endif // ATACOMMAND_H

