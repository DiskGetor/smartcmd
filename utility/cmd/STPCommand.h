#ifndef STPCOMMAND_H
#define STPCOMMAND_H

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

#include "CsmiSas.h"
#include "BaseCommand.h"

enum eSTPCommandCode
{
    #define MAP_ITEM(code, val) code = val,
    #include "STPCommandCode.def"
    #undef MAP_ITEM
};

class STPCommand : public BaseCommand
{
public:
    virtual U8* getBufferPtr();
    virtual bool executeCommand(int handle);
    virtual void setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode);
    virtual eCommandError getErrorStatus();
    virtual string getTaskfileString();

private:
    virtual bool buildCommand();      // setting flags, ..
    virtual void buildBasicCommand(); // Initialize basic data

public:
    bool buildIoctlHeader();
    void initPhyEntity(const CSMI_SAS_PHY_ENTITY& phy);

public:
    string getCommandString() const;
    const char* getSignatureString() const;

private:
    CSMI_SAS_PHY_ENTITY PhyEntity;
};

#endif // STPCOMMAND_H

