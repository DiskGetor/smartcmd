#ifndef ATADEVICE_H
#define ATADEVICE_H

#include "BaseDevice.h"

class ATADevice : public BaseDevice
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
};

#endif // DEVICE_H

