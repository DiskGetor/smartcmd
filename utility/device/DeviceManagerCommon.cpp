#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/cmd/CSMICommand.h"

#include "DeviceUtil.h"
#include "DeviceManager.h"

#include "ATADevice.h"
#include "SATDevice.h"
#include "HYPDevice.h"
#include "STPDevice.h"
#include "SCSIDevice.h"

bool DeviceManager::InitDevice(const string &driveName, vector<BaseDevice*>& devList)
{
    // Open device name
    // Check bus type
    // Return the correspondign object type

    devList.clear();

    BaseDevice* devPtr = NULL;

    int handle;

    do {
        if (false == DeviceUtil::OpenDevice(driveName, handle)) break;

        // Check bus type
        eCommandType cmdType = DeviceManager::GetCommandType(handle);

        switch(cmdType)
        {
            case CMDTYPE_STP: break;
            case CMDTYPE_ATA: devPtr = new ATADevice; break;
            case CMDTYPE_SAT: devPtr = new SATDevice; break;
            case CMDTYPE_HYP: devPtr = new HYPDevice; break;
            case CMDTYPE_SCSI: devPtr = new SCSIDevice; break;
            default: break;
        }

        if (cmdType == CMDTYPE_STP)
        {
            CSMICommand phycmd;
            phycmd.setCommand(0, 0, CSMI_GET_PHY_INFO);
            if (false == phycmd.executeCommand(handle)) break;

            CSMI_SAS_PHY_INFO_BUFFER* phyBuffer = (CSMI_SAS_PHY_INFO_BUFFER*)phycmd.getStructPtr();
            U32 phyCount = phyBuffer->Information.bNumberOfPhys;
            if (0 == phyCount) break;

            for (U32 i = 0; i < phyCount; i++)
            {
                CSMI_SAS_PHY_ENTITY& phy = phyBuffer->Information.Phy[i];

                STPDevice* stpDevPtr= new STPDevice;

                bool initStatus =
                        (true == stpDevPtr->initDevice(driveName)) &&
                        (true == stpDevPtr->initPhyEntity(phy));

                if (false == initStatus)
                {
                    delete stpDevPtr;
                    continue;
                }

                devList.push_back((BaseDevice*)stpDevPtr);
            }
        }
        else
        {
            if (false == devPtr->initDevice(driveName))
            {
                delete devPtr;
                break;
            }

            devList.push_back(devPtr);
        }
    } while(0);

    return devList.size() != 0;
}

bool DeviceManager::InitDeviceList(const vector<string> &devNameList, vector<BaseDevice *> &devList)
{
    devList.clear();

    U32 devCount = devNameList.size();
    for (U32 i = 0; i < devCount; i++)
    {
        vector<BaseDevice*> subList;

        if(false == DeviceManager::InitDevice(devNameList[i], subList)) continue;

        U32 subCount = subList.size();
        for (U32 j = 0; j < subCount; j++) devList.push_back(subList[j]);
    }

    return devList.size() != 0;
}

bool DeviceManager::InitDeviceList(vector<BaseDevice *> &devList)
{
    // Get list of deviceName
    vector<string> devNameList;

    if (false == DeviceUtil::GetNameList(devNameList)) return false;

    return InitDeviceList(devNameList, devList);
}

void DeviceManager::DeleteDeviceList(vector<BaseDevice *> &devList)
{
    U32 devCount = devList.size();
    for (U32 i = 0; i < devCount; i++)
    {
        BaseDevice* devPtr = devList[i];
        if (NULL != devPtr) delete devPtr;
    }

    devList.clear();
}


BaseDevice* DeviceManager::InitDevice(const string &serialNo)
{
    vector<string> devNameList;
    DeviceUtil::GetNameList(devNameList);

    U32 nameCount = devNameList.size();
    for (U32 i = 0; i < nameCount; i++)
    {
        vector<BaseDevice*> subList;
        if(false == DeviceManager::InitDevice(devNameList[i], subList)) continue;

        // Reading Identify of all devices in devSubList
        U32 subCount = subList.size();
        for (U32 j = 0; j < subCount; j++)
        {
            BaseDevice* devPtr = subList[j];
            if (false == devPtr->readIdentify()) continue;

            string devSerial;

            // Checking for SerialNumber
            if (CMDTYPE_SCSI == devPtr->m_CommandType)
            {
                sInquiryInfo driveInfo;
                DeviceUtil::ParseInquiryInfo(devPtr->getBufferPtr(), driveInfo);
                devSerial = driveInfo.SerialNumber;
            }
            else
            {
                sIdentifyInfo driveInfo;
                DeviceUtil::ParseIdentifyInfo(devPtr->getBufferPtr(), "DriveName", driveInfo);
                devSerial = driveInfo.SerialNumber;
            }

            // Matching SerialNumber
            if (0 != devSerial.compare(serialNo)) continue;

            // Found matched serial number
            // Delete all remaining devices
            for (U32 k = 0; k < subCount; k++) if (k != j) delete subList[k];

            return devPtr;
        }
    }

    return NULL;
}

BaseDevice* DeviceManager::SearchDevice(const string& devName, const vector<BaseDevice*>& devList)
{
    U16 devCount = devList.size();

    for(U16 nDevice = 0; nDevice < devCount; ++ nDevice)
    {
        BaseDevice* pDev = devList[nDevice];
        if( 0 == pDev->m_Name.compare(devName) )
        {
            return pDev;
        }
    }

    return NULL;
}
