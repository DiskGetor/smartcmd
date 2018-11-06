#ifndef SCSIDEVICE_H
#define SCSIDEVICE_H

#include "BaseDevice.h"

class SCSIDevice : public BaseDevice
{
public:
    virtual bool readIdentify();                  // general function
    virtual bool readSmartData();                 // general function
    virtual bool readSmartThreshold();            // general function
    virtual bool read(U32 lba, U32 sectorCount);  // general function
    virtual bool write(U32 lba, U32 sectorCount); // general function

    virtual bool readIdentify(sIdentifyInfo& driveInfo);

public:
    bool prepareResource();
};

#endif // SCSIDEVICE_H

