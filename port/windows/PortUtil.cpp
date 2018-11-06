#include "core/CoreUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/misc/MiscUtil.h"

#include "PortUtil.h"

bool PortUtil::IssueCommand_IdentifyDrive(HANDLE handle, int driveNumber, unsigned char* data)
{
    // Identify device using SENDCMDINPARAMS

    DWORD retValue = 0;

    const DWORD outParamSize = sizeof(SENDCMDOUTPARAMS);
    const DWORD outBufferSize = outParamSize + 512 - 1;

    unsigned char outBuffer[outBufferSize];

    SENDCMDINPARAMS inParam;
    memset (&inParam, 0x00, sizeof (inParam));

    inParam.cBufferSize = 512;
    inParam.bDriveNumber = driveNumber;
    inParam.irDriveRegs.bFeaturesReg = 0;
    inParam.irDriveRegs.bSectorCountReg = 1;
    inParam.irDriveRegs.bSectorNumberReg = 1;
    inParam.irDriveRegs.bCommandReg = ID_CMD;

    BOOL status = DeviceIoControl(
        handle,
        SMART_RCV_DRIVE_DATA,
        &inParam, sizeof(inParam),
        outBuffer, outBufferSize,
        &retValue,
        NULL);

    if (FALSE == status) return false;

    memcpy(data, outBuffer + outParamSize - 1, 512);

    return true;
}

bool PortUtil::IssueCommand_ReadSmartValue(HANDLE handle, int driveNumber, unsigned char* data)
{
    // Read SMART attributes using SENDCMDINPARAMS

    DWORD retValue = 0;

    const DWORD outParamSize = sizeof(SENDCMDOUTPARAMS);
    const DWORD outBufferSize = outParamSize + 512 - 1;

    unsigned char outBuffer[outBufferSize];
    SENDCMDINPARAMS inParam;
    memset (&inParam, 0x00, sizeof (inParam));

    inParam.cBufferSize = 512;
    inParam.bDriveNumber = driveNumber;
    inParam.irDriveRegs.bFeaturesReg = READ_ATTRIBUTES;
    inParam.irDriveRegs.bSectorCountReg = 1;
    inParam.irDriveRegs.bSectorNumberReg = 1;
    inParam.irDriveRegs.bCylLowReg = SMART_CYL_LOW;
    inParam.irDriveRegs.bCylHighReg = SMART_CYL_HI;
    inParam.irDriveRegs.bDriveHeadReg = 0xA0;
    inParam.irDriveRegs.bCommandReg = SMART_CMD;

    BOOL status = DeviceIoControl(
        handle,
        SMART_RCV_DRIVE_DATA,
        &inParam, sizeof(inParam),
        outBuffer, outBufferSize,
        &retValue,
        NULL);

    if (FALSE == status) return false;

    memcpy(data, outBuffer + outParamSize - 1, 512);

    return true;
}

bool PortUtil::IssueCommand_ReadSmartThreshold(HANDLE handle, int driveNumber, unsigned char* data)
{
    // Read SMART attributes using SENDCMDINPARAMS

    DWORD retValue = 0;

    const DWORD outParamSize = sizeof(SENDCMDOUTPARAMS);
    const DWORD outBufferSize = outParamSize + 512 - 1;

    unsigned char outBuffer[outBufferSize];
    SENDCMDINPARAMS inParam;
    memset (&inParam, 0x00, sizeof (inParam));

    inParam.cBufferSize = 512;
    inParam.bDriveNumber = driveNumber;
    inParam.irDriveRegs.bFeaturesReg = READ_THRESHOLDS;
    inParam.irDriveRegs.bSectorCountReg = 1;
    inParam.irDriveRegs.bSectorNumberReg = 1;
    inParam.irDriveRegs.bCylLowReg = SMART_CYL_LOW;
    inParam.irDriveRegs.bCylHighReg = SMART_CYL_HI;
    inParam.irDriveRegs.bDriveHeadReg = 0xA0;
    inParam.irDriveRegs.bCommandReg = SMART_CMD;

    BOOL status = DeviceIoControl(
        handle,
        SMART_RCV_DRIVE_DATA,
        &inParam, sizeof(inParam),
        outBuffer, outBufferSize,
        &retValue,
        NULL);

    if (FALSE == status) return false;

    memcpy(data, outBuffer + outParamSize - 1, 512);

    return true;
}

void PortUtil::IssueCommand_Test(tIssueCommandFunc func)
{
    vector<sPhysicalDriveInfo> driveList;
    DeviceUtil::OpenDevice(driveList);

    int driveCount = driveList.size();
    if (driveCount != 0)
    {
        for (int i = 0; i < driveCount; i++)
        {
            stringstream sstr;
            sstr << "Name: " << driveList[i].DriveName << " ";
            sstr << "Hndl: " << driveList[i].DriveHandle << " ";
            sstr << endl;

            // Read Identify info
            unsigned char outBuffer[512];
            func((HANDLE)driveList[i].DriveHandle, i, outBuffer);

            sstr << DeviceUtil::ToAsciiString(outBuffer);

            vector<string> nameList;
            nameList.push_back(sstr.str());
        }
    }

    DeviceUtil::CloseDevice(driveList);
}

void PortUtil::ScanPhysicalDrive_Test()
{
    vector<sPhysicalDriveInfo> driveList;
    DeviceUtil::OpenDevice(driveList);

    int driveCount = driveList.size();
    if (driveCount != 0)
    {
        vector<string> nameList;
        for (int i = 0; i < driveCount; i++)
        {
            stringstream sstr;
            sstr << "Name: " << driveList[i].DriveName << " Handle: " << driveList[i].DriveHandle;
            nameList.push_back(sstr.str());
        }
    }

    DeviceUtil::CloseDevice(driveList);
}
