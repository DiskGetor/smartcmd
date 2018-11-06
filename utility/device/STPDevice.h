#ifndef CSMIDEVICE_H
#define CSMIDEVICE_H

#include "utility/cmd/CsmiSas.h"

#include "BaseDevice.h"

class STPDevice : public BaseDevice
{
public:
    virtual bool readIdentify();                  // general function
    virtual bool readSmartData();                 // general function
    virtual bool readSmartThreshold();            // general function
    virtual bool read(U32 lba, U32 sectorCount);  // general function
    virtual bool write(U32 lba, U32 sectorCount); // general function

    virtual bool readInquiry(sInquiryInfo& driveInfo);

public:
    bool prepareResource();
    bool initPhyEntity(const CSMI_SAS_PHY_ENTITY& phy);
};

#endif // CSMIDEVICE_H

