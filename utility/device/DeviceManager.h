#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "BaseDevice.h"

class DeviceManager
{
public:
    static BaseDevice* InitDevice(const string& serialNumber);
    static bool InitDevice(const string& devName, vector<BaseDevice*>& devList);

    static bool InitDeviceList(vector<BaseDevice*>& devList);
    static bool InitDeviceList(const vector<string>& devName, vector<BaseDevice*>& devList);
    static BaseDevice* SearchDevice(const string& devName, const vector<BaseDevice*>& devList);

    static void DeleteDeviceList(vector<BaseDevice*>& devList);

public:
    static eCommandType GetCommandType(U32 handle);
};

#endif // DEVICEMANAGER_H

