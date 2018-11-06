#ifndef SCSICOMMAND_H
#define SCSICOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "BaseCommand.h"

enum eSCSICommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "SCSICommandCode.def"
    #undef MAP_ITEM
};

class SCSICommand : public BaseCommand
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

#endif // SCSICOMMAND_H

