#ifndef CSMICOMMAND_H
#define CSMICOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "CsmiSas.h"
#include "BaseCommand.h"

enum eCSMICommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "CSMICommandCode.def"
    #undef MAP_ITEM
};

class CSMICommand : public BaseCommand
{
public:
    virtual bool executeCommand(int handle);
    virtual void setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode);
    virtual eCommandError getErrorStatus();
    virtual string getTaskfileString();

private:
    virtual bool buildCommand();      // setting flags, ..
    virtual void buildBasicCommand(); // Initialize basic data

private:
    bool buildIoctlHeader();

public:
    bool buildIoctlContent();
    string getCommandString() const;
    const char* getSignatureString() const;
};

#endif // CSMICOMMAND_H

