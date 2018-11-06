#include "utility/misc/StdMacro.h"
#include "utility/misc/MiscUtil.h"

#include "CSMIPrinter.h"
#include "CSMIPrinterMacro.h"

string CSMIPrinter::ToTypeString(const IOCTL_HEADER& info) { UNUSE_NAME(info); RETURN_NAME("IOCTL_HEADER"); }

// CC_CSMI_SAS_DRIVER_INFO

string CSMIPrinter::ToTypeString(const CSMI_SAS_DRIVER_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_DRIVER_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_DRIVER_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRING(info, idx, szName);
    PRINT_VALUE_STRING(info, idx, szDescription);
    PRINT_VALUE_INT(info, idx, usMajorRevision);
    PRINT_VALUE_INT(info, idx, usMinorRevision);
    PRINT_VALUE_INT(info, idx, usBuildRevision);
    PRINT_VALUE_INT(info, idx, usReleaseRevision);
    PRINT_VALUE_INT(info, idx, usCSMIMajorRevision);
    PRINT_VALUE_INT(info, idx, usCSMIMinorRevision);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_DRIVER_INFO_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_DRIVER_INFO_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_DRIVER_INFO_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_CNTLR_CONFIGURATION

string CSMIPrinter::ToTypeString(const CSMI_SAS_PCI_BUS_ADDRESS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PCI_BUS_ADDRESS"); }

string CSMIPrinter::ToString(const CSMI_SAS_PCI_BUS_ADDRESS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bBusNumber);
    PRINT_VALUE_INT(info, idx, bDeviceNumber);
    PRINT_VALUE_INT(info, idx, bFunctionNumber);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_IO_BUS_ADDRESS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_IO_BUS_ADDRESS"); }

string CSMIPrinter::ToString(const CSMI_SAS_IO_BUS_ADDRESS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, PciAddress);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_CNTLR_CONFIG& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CNTLR_CONFIG"); }

string CSMIPrinter::ToString(const CSMI_SAS_CNTLR_CONFIG& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uBaseIoAddress);
    PRINT_VALUE_INT(info, idx, BaseMemoryAddress.uLowPart);
    PRINT_VALUE_INT(info, idx, BaseMemoryAddress.uHighPart);
    PRINT_VALUE_INT(info, idx, uBoardID);
    PRINT_VALUE_INT(info, idx, usSlotNumber);
    PRINT_VALUE_INT(info, idx, bControllerClass);
    PRINT_VALUE_INT(info, idx, bIoBusType);
    PRINT_VALUE_STRUCT(info, idx, BusAddress);
    PRINT_VALUE_STRING(info, idx, szSerialNumber);
    PRINT_VALUE_INT(info, idx, usMajorRevision);
    PRINT_VALUE_INT(info, idx, usMinorRevision);
    PRINT_VALUE_INT(info, idx, usBuildRevision);
    PRINT_VALUE_INT(info, idx, usReleaseRevision);
    PRINT_VALUE_INT(info, idx, usBIOSMajorRevision);
    PRINT_VALUE_INT(info, idx, usBIOSMinorRevision);
    PRINT_VALUE_INT(info, idx, usBIOSBuildRevision);
    PRINT_VALUE_INT(info, idx, usBIOSReleaseRevision);
    PRINT_VALUE_INT(info, idx, uControllerFlags);
    PRINT_VALUE_INT(info, idx, usRromMajorRevision);
    PRINT_VALUE_INT(info, idx, usRromMinorRevision);
    PRINT_VALUE_INT(info, idx, usRromBuildRevision);
    PRINT_VALUE_INT(info, idx, usRromReleaseRevision);
    PRINT_VALUE_INT(info, idx, usRromBIOSMajorRevision);
    PRINT_VALUE_INT(info, idx, usRromBIOSMinorRevision);
    PRINT_VALUE_INT(info, idx, usRromBIOSBuildRevision);
    PRINT_VALUE_INT(info, idx, usRromBIOSReleaseRevision);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_CNTLR_CONFIG_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CNTLR_CONFIG_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_CNTLR_CONFIG_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Configuration);

    return sstr.str();
}

// CC_CSMI_SAS_CNTLR_STATUS

string CSMIPrinter::ToTypeString(const CSMI_SAS_CNTLR_STATUS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CNTLR_STATUS"); }

string CSMIPrinter::ToString(const CSMI_SAS_CNTLR_STATUS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uStatus);
    PRINT_VALUE_INT(info, idx, uOfflineReason);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_CNTLR_STATUS_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CNTLR_STATUS_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_CNTLR_STATUS_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Status);

    return sstr.str();
}

// CC_CSMI_SAS_FIRMWARE_DOWNLOAD

string CSMIPrinter::ToTypeString(const CSMI_SAS_FIRMWARE_DOWNLOAD& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_FIRMWARE_DOWNLOAD"); }

string CSMIPrinter::ToString(const CSMI_SAS_FIRMWARE_DOWNLOAD& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uBufferLength);
    PRINT_VALUE_INT(info, idx, uDownloadFlags);
    PRINT_VALUE_INT(info, idx, usStatus);
    PRINT_VALUE_INT(info, idx, usSeverity);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);
    PRINT_ARRAY_INT(info, idx, bDataBuffer);

    return sstr.str();
}

// CC_CSMI_SAS_RAID_INFO

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uNumRaidSets);
    PRINT_VALUE_INT(info, idx, uMaxDrivesPerSet);
    PRINT_VALUE_INT(info, idx, uMaxRaidSets);
    PRINT_VALUE_INT(info, idx, bMaxRaidTypes);
    PRINT_VALUE_INT(info, idx, ulMinRaidSetBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulMinRaidSetBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, ulMaxRaidSetBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulMaxRaidSetBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, uMaxPhysicalDrives);
    PRINT_VALUE_INT(info, idx, uMaxExtents);
    PRINT_VALUE_INT(info, idx, uMaxModules);
    PRINT_VALUE_INT(info, idx, uMaxTransformationMemory);
    PRINT_VALUE_INT(info, idx, uChangeCount);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_INFO_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_INFO_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_INFO_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_GET_RAID_CONFIG

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_DRIVES& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_DRIVES"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_DRIVES& info, U32 idx) {
    stringstream sstr;

    PRINT_ARRAY_CHAR(info, idx, bModel);
    PRINT_ARRAY_CHAR(info, idx, bFirmware);
    PRINT_ARRAY_CHAR(info, idx, bSerialNumber);
    PRINT_ARRAY_INT(info, idx, bSASAddress);
    PRINT_ARRAY_INT(info, idx, bSASLun);
    PRINT_VALUE_INT(info, idx, bDriveStatus);
    PRINT_VALUE_INT(info, idx, bDriveUsage);
    PRINT_VALUE_INT(info, idx, usBlockSize);
    PRINT_VALUE_INT(info, idx, bDriveType);
    PRINT_VALUE_INT(info, idx, uDriveIndex);
    PRINT_VALUE_INT(info, idx, ulTotalUserBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulTotalUserBlocks.uHighPart);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_DEVICE_ID& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_DEVICE_ID"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_DEVICE_ID& info, U32 idx) {
    stringstream sstr;

    PRINT_ARRAY_INT(info, idx, bDeviceIdentificationVPDPage);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_ADDITIONAL_DATA& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_ADDITIONAL_DATA"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_ADDITIONAL_DATA& info, U32 idx) {
    stringstream sstr;

    PRINT_ARRAY_CHAR(info, idx, bLabel);
    PRINT_ARRAY_INT(info, idx, bRaidSetLun);
    PRINT_VALUE_INT(info, idx, bWriteProtection);
    PRINT_VALUE_INT(info, idx, bCacheSetting);
    PRINT_VALUE_INT(info, idx, bCacheRatio);
    PRINT_VALUE_INT(info, idx, usBlockSize);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uHighPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, uStripeSizeInBlocks);
    PRINT_VALUE_INT(info, idx, uSectorsPerTrack);
    PRINT_ARRAY_INT(info, idx, bApplicationScratchPad);
    PRINT_VALUE_INT(info, idx, uNumberOfHeads);
    PRINT_VALUE_INT(info, idx, uNumberOfTracks);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_CONFIG& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_CONFIG"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_CONFIG& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, uCapacity);
    PRINT_VALUE_INT(info, idx, uStripeSize);
    PRINT_VALUE_INT(info, idx, bRaidType);
    PRINT_VALUE_INT(info, idx, bStatus);
    PRINT_VALUE_INT(info, idx, bInformation);
    PRINT_VALUE_INT(info, idx, bDriveCount);
    PRINT_VALUE_INT(info, idx, bDataType);
    PRINT_VALUE_INT(info, idx, uFailureCode);
    PRINT_VALUE_INT(info, idx, uChangeCount);
    PRINT_ARRAY_STRUCT(info, idx, 0, Drives);
    PRINT_ARRAY_STRUCT(info, idx, 0, DeviceId);
    PRINT_ARRAY_STRUCT(info, idx, 0, Data);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_CONFIG_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_CONFIG_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_CONFIG_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Configuration);

    return sstr.str();
}

// CC_CSMI_SAS_GET_RAID_FEATURES

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_TYPE_DESCRIPTION& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_TYPE_DESCRIPTION"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_TYPE_DESCRIPTION& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bRaidType);
    PRINT_VALUE_INT(info, idx, uSupportedStripeSizeMap);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_FEATURES& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_FEATURES"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_FEATURES& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uFeatures);
    PRINT_VALUE_INT(info, idx, bDefaultTransformPriority);
    PRINT_VALUE_INT(info, idx, bTransformPriority);
    PRINT_VALUE_INT(info, idx, bDefaultRebuildPriority);
    PRINT_VALUE_INT(info, idx, bRebuildPriority);
    PRINT_VALUE_INT(info, idx, bDefaultSurfaceScanPriority);
    PRINT_VALUE_INT(info, idx, bSurfaceScanPriority);
    PRINT_VALUE_INT(info, idx, uRaidSetTransformationRules);
    PRINT_ARRAY_STRUCT(info, idx, 0, RaidType);
    PRINT_ARRAY_INT(info, idx, bCacheRatiosSupported);
    PRINT_VALUE_INT(info, idx, uChangeCount);
    PRINT_VALUE_INT(info, idx, uFailureCode);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_FEATURES_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_FEATURES_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_FEATURES_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_SET_RAID_CONTROL

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_CONTROL& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_CONTROL"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_CONTROL& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bTransformPriority);
    PRINT_VALUE_INT(info, idx, bRebuildPriority);
    PRINT_VALUE_INT(info, idx, bCacheRatioFlag);
    PRINT_VALUE_INT(info, idx, bCacheRatio);
    PRINT_VALUE_INT(info, idx, bSurfaceScanPriority);
    PRINT_ARRAY_INT(info, idx, bClearConfiguration);
    PRINT_VALUE_INT(info, idx, uChangeCount);
    PRINT_VALUE_INT(info, idx, uFailureCode);
    PRINT_ARRAY_INT(info, idx, bFailureDescription);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_CONTROL_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_CONTROL_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_CONTROL_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_GET_RAID_ELEMENT

string CSMIPrinter::ToTypeString(const CSMI_SAS_DRIVE_EXTENT_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_DRIVE_EXTENT_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_DRIVE_EXTENT_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uDriveIndex);
    PRINT_VALUE_INT(info, idx, bExtentType);
    PRINT_VALUE_INT(info, idx, ulExtentOffset.uLowPart);
    PRINT_VALUE_INT(info, idx, ulExtentOffset.uHighPart);
    PRINT_VALUE_INT(info, idx, ulExtentBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulExtentBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, uRaidSetIndex);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_MODULE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_MODULE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_MODULE_INFO& info, U32 idx) {
    stringstream sstr;

    UNUSE_NAME(info);
    UNUSE_NAME(idx);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_DRIVE_LOCATION& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_DRIVE_LOCATION"); }

string CSMIPrinter::ToString(const CSMI_SAS_DRIVE_LOCATION& info, U32 idx) {
    stringstream sstr;

    PRINT_ARRAY_INT(info, idx, bConnector);
    PRINT_ARRAY_INT(info, idx, bBoxName);
    PRINT_VALUE_INT(info, idx, uBay);
    PRINT_ARRAY_INT(info, idx, bAttachedSASAddress);
    PRINT_VALUE_INT(info, idx, bAttachedPhyIdentifier);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_DRIVES_ADDITIONAL_DATA& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_DRIVES_ADDITIONAL_DATA"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_DRIVES_ADDITIONAL_DATA& info, U32 idx) {
    stringstream sstr;

    PRINT_ARRAY_INT(info, idx, bNegotiatedLinkRate);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_DRIVE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_DRIVE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_DRIVE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, Device);
    PRINT_VALUE_STRUCT(info, idx, Data);
    PRINT_VALUE_STRUCT(info, idx, Location);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_ELEMENT& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_ELEMENT"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_ELEMENT& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uEnumerationType);
    PRINT_VALUE_INT(info, idx, uElementIndex);
    PRINT_VALUE_INT(info, idx, uNumElements);
    PRINT_VALUE_INT(info, idx, uChangeCount);
    PRINT_VALUE_INT(info, idx, uSubElementIndex);
    PRINT_VALUE_INT(info, idx, uFailureCode);
    PRINT_ARRAY_INT(info, idx, bFailureDescription);
    PRINT_VALUE_STRUCT(info, idx, Element.Drive);
    PRINT_VALUE_STRUCT(info, idx, Element.Module);
    PRINT_VALUE_STRUCT(info, idx, Element.Extent);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_ELEMENT_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_ELEMENT_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_ELEMENT_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_SET_RAID_OPERATION

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_LIST& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_LIST"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_LIST& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_ARRAY_INT(info, idx, bExistingLun);
    PRINT_ARRAY_INT(info, idx, bNewLun);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_DRIVE_LIST& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_DRIVE_LIST"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_DRIVE_LIST& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uDriveIndex);
    PRINT_VALUE_INT(info, idx, bDriveUsage);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_SPARE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_SPARE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_SPARE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, uDriveCount);
    PRINT_ARRAY_INT(info, idx, bApplicationScratchPad);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_ONLINE_STATE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_ONLINE_STATE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_ONLINE_STATE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, bOnlineState);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_CACHE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_CACHE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_CACHE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, bCacheSetting);
    PRINT_VALUE_INT(info, idx, bCacheRatioFlag);
    PRINT_VALUE_INT(info, idx, bCacheRatio);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_WRITE_PROTECT_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_WRITE_PROTECT_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_WRITE_PROTECT_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, bWriteProtectSetting);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_DELETE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_DELETE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_DELETE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_MODIFY_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_MODIFY_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_MODIFY_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bRaidType);
    PRINT_VALUE_INT(info, idx, uStripeSize);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uHighPart);
    PRINT_VALUE_INT(info, idx, uDriveCount);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_TRANSFORM_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_TRANSFORM_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_TRANSFORM_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bTransformType);
    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_VALUE_INT(info, idx, bRaidType);
    PRINT_VALUE_INT(info, idx, uAdditionalRaidSetIndex);
    PRINT_VALUE_INT(info, idx, uRaidSetCount);
    PRINT_ARRAY_INT(info, idx, bApplicationScratchPad);
    PRINT_VALUE_STRUCT(info, idx, Modify);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_LABEL_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_LABEL_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_LABEL_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_ARRAY_INT(info, idx, bLabel);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_CREATE_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_CREATE_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_CREATE_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bRaidType);
    PRINT_VALUE_INT(info, idx, uStripeSize);
    PRINT_VALUE_INT(info, idx, uTrackSectorCount);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetBlocks.uHighPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uLowPart);
    PRINT_VALUE_INT(info, idx, ulRaidSetExtentOffset.uHighPart);
    PRINT_VALUE_INT(info, idx, uDriveCount);
    PRINT_ARRAY_INT(info, idx, bLabel);
    PRINT_VALUE_INT(info, idx, uRaidSetIndex);
    PRINT_ARRAY_INT(info, idx, bApplicationScratchPad);
    PRINT_VALUE_INT(info, idx, uNumberOfHeads);
    PRINT_VALUE_INT(info, idx, uNumberOfTracks);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_OPERATION& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_OPERATION"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_OPERATION& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uOperationType);
    PRINT_VALUE_INT(info, idx, uChangeCount);
    PRINT_VALUE_INT(info, idx, uFailureCode);
    PRINT_ARRAY_INT(info, idx, bFailureDescription);
    PRINT_VALUE_STRUCT(info, idx, Operation.Create);
    PRINT_VALUE_STRUCT(info, idx, Operation.Label);
    PRINT_VALUE_STRUCT(info, idx, Operation.Transform);
    PRINT_VALUE_STRUCT(info, idx, Operation.Delete);
    PRINT_VALUE_STRUCT(info, idx, Operation.Protect);
    PRINT_VALUE_STRUCT(info, idx, Operation.Cache);
    PRINT_VALUE_STRUCT(info, idx, Operation.State);
    PRINT_VALUE_STRUCT(info, idx, Operation.Spare);
    PRINT_ARRAY_STRUCT(info, idx, 0, Parameters.DriveList);
    PRINT_ARRAY_STRUCT(info, idx, 0, Parameters.RaidSetList);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_RAID_SET_OPERATION_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_RAID_SET_OPERATION_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_RAID_SET_OPERATION_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_GET_PHY_INFO

string CSMIPrinter::ToTypeString(const CSMI_SAS_IDENTIFY& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_IDENTIFY"); }

string CSMIPrinter::ToString(const CSMI_SAS_IDENTIFY& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_ENUM(info, idx, bDeviceType, ToDeviceTypeString);
    PRINT_VALUE_INT(info, idx, bRestricted);
    PRINT_VALUE_ENUM(info, idx, bInitiatorPortProtocol, ToPortProtocolString);
    PRINT_VALUE_ENUM(info, idx, bTargetPortProtocol, ToPortProtocolString);
    PRINT_ARRAY_INT(info, idx, bRestricted2);
    PRINT_ARRAY_INT(info, idx, bSASAddress);
    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_ENUM(info, idx, bSignalClass, ToSignalClassString);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_PHY_ENTITY& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PHY_ENTITY"); }

string CSMIPrinter::ToString(const CSMI_SAS_PHY_ENTITY& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, Identify);
    PRINT_VALUE_INT(info, idx, bPortIdentifier);
    PRINT_VALUE_ENUM(info, idx, bNegotiatedLinkRate, ToLinkRateString);
    PRINT_VALUE_ENUM(info, idx, bMinimumLinkRate, ToLinkRateString);
    PRINT_VALUE_ENUM(info, idx, bMaximumLinkRate, ToLinkRateString);
    PRINT_VALUE_INT(info, idx, bPhyChangeCount);
    PRINT_VALUE_INT(info, idx, bAutoDiscover);
    PRINT_VALUE_INT(info, idx, bPhyFeatures);
    PRINT_VALUE_STRUCT(info, idx, Attached);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_PHY_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PHY_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_PHY_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bNumberOfPhys);
    PRINT_ARRAY_STRUCT(info, idx, info.bNumberOfPhys, Phy);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_PHY_INFO_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PHY_INFO_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_PHY_INFO_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_SET_PHY_INFO

string CSMIPrinter::ToTypeString(const CSMI_SAS_SET_PHY_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SET_PHY_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_SET_PHY_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, bNegotiatedLinkRate);
    PRINT_VALUE_INT(info, idx, bProgrammedMinimumLinkRate);
    PRINT_VALUE_INT(info, idx, bProgrammedMaximumLinkRate);
    PRINT_VALUE_INT(info, idx, bSignalClass);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SET_PHY_INFO_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SET_PHY_INFO_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_SET_PHY_INFO_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_GET_LINK_ERRORS

string CSMIPrinter::ToTypeString(const CSMI_SAS_LINK_ERRORS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_LINK_ERRORS"); }

string CSMIPrinter::ToString(const CSMI_SAS_LINK_ERRORS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, bResetCounts);
    PRINT_VALUE_INT(info, idx, uInvalidDwordCount);
    PRINT_VALUE_INT(info, idx, uRunningDisparityErrorCount);
    PRINT_VALUE_INT(info, idx, uLossOfDwordSyncCount);
    PRINT_VALUE_INT(info, idx, uPhyResetProblemCount);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_LINK_ERRORS_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_LINK_ERRORS_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_LINK_ERRORS_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Information);

    return sstr.str();
}

// CC_CSMI_SAS_SMP_PASSTHRU

string CSMIPrinter::ToTypeString(const CSMI_SAS_SMP_REQUEST& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SMP_REQUEST"); }

string CSMIPrinter::ToString(const CSMI_SAS_SMP_REQUEST& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bFrameType);
    PRINT_VALUE_INT(info, idx, bFunction);
    PRINT_ARRAY_INT(info, idx, bAdditionalRequestBytes);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SMP_RESPONSE& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SMP_RESPONSE"); }

string CSMIPrinter::ToString(const CSMI_SAS_SMP_RESPONSE& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bFrameType);
    PRINT_VALUE_INT(info, idx, bFunction);
    PRINT_VALUE_INT(info, idx, bFunctionResult);
    PRINT_ARRAY_INT(info, idx, bAdditionalResponseBytes);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SMP_PASSTHRU& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SMP_PASSTHRU"); }

string CSMIPrinter::ToString(const CSMI_SAS_SMP_PASSTHRU& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, bPortIdentifier);
    PRINT_VALUE_INT(info, idx, bConnectionRate);
    PRINT_ARRAY_INT(info, idx, bDestinationSASAddress);
    PRINT_VALUE_INT(info, idx, uRequestLength);
    PRINT_VALUE_STRUCT(info, idx, Request);
    PRINT_VALUE_INT(info, idx, bConnectionStatus);
    PRINT_VALUE_INT(info, idx, uResponseBytes);
    PRINT_VALUE_STRUCT(info, idx, Response);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SMP_PASSTHRU_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SMP_PASSTHRU_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_SMP_PASSTHRU_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Parameters);

    return sstr.str();
}

// CC_CSMI_SAS_SSP_PASSTHRU

string CSMIPrinter::ToTypeString(const CSMI_SAS_SSP_PASSTHRU& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SSP_PASSTHRU"); }

string CSMIPrinter::ToString(const CSMI_SAS_SSP_PASSTHRU& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, bPortIdentifier);
    PRINT_VALUE_INT(info, idx, bConnectionRate);
    PRINT_ARRAY_INT(info, idx, bDestinationSASAddress);
    PRINT_ARRAY_INT(info, idx, bLun);
    PRINT_VALUE_INT(info, idx, bCDBLength);
    PRINT_VALUE_INT(info, idx, bAdditionalCDBLength);
    PRINT_ARRAY_INT(info, idx, bCDB);
    PRINT_VALUE_INT(info, idx, uFlags);
    PRINT_ARRAY_INT(info, idx, bAdditionalCDB);
    PRINT_VALUE_INT(info, idx, uDataLength);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SSP_PASSTHRU_STATUS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SSP_PASSTHRU_STATUS"); }

string CSMIPrinter::ToString(const CSMI_SAS_SSP_PASSTHRU_STATUS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bConnectionStatus);
    PRINT_VALUE_INT(info, idx, bSSPStatus);
    PRINT_VALUE_INT(info, idx, bDataPresent);
    PRINT_VALUE_INT(info, idx, bStatus);
    PRINT_ARRAY_INT(info, idx, bResponseLength);
    PRINT_ARRAY_INT(info, idx, bResponse);
    PRINT_VALUE_INT(info, idx, uDataBytes);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SSP_PASSTHRU_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SSP_PASSTHRU_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_SSP_PASSTHRU_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Parameters);
    PRINT_VALUE_STRUCT(info, idx, Status);
    PRINT_ARRAY_INT(info, idx, bDataBuffer);

    return sstr.str();
}

// CC_CSMI_SAS_STP_PASSTHRU

string CSMIPrinter::ToTypeString(const CSMI_SAS_STP_PASSTHRU& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_STP_PASSTHRU"); }

string CSMIPrinter::ToString(const CSMI_SAS_STP_PASSTHRU& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, bPortIdentifier);
    PRINT_VALUE_INT(info, idx, bConnectionRate);
    PRINT_ARRAY_INT(info, idx, bDestinationSASAddress);
    PRINT_ARRAY_INT(info, idx, bCommandFIS);
    PRINT_VALUE_INT(info, idx, uFlags);
    PRINT_VALUE_INT(info, idx, uDataLength);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_STP_PASSTHRU_STATUS& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_STP_PASSTHRU_STATUS"); }

string CSMIPrinter::ToString(const CSMI_SAS_STP_PASSTHRU_STATUS& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bConnectionStatus);
    PRINT_ARRAY_INT(info, idx, bStatusFIS);
    PRINT_ARRAY_INT(info, idx, uSCR);
    PRINT_VALUE_INT(info, idx, uDataBytes);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_STP_PASSTHRU_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_STP_PASSTHRU_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_STP_PASSTHRU_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Parameters);
    PRINT_VALUE_STRUCT(info, idx, Status);
    PRINT_ARRAY_INT(info, idx, bDataBuffer);

    return sstr.str();
}

// CC_CSMI_SAS_GET_SATA_SIGNATURE

string CSMIPrinter::ToTypeString(const CSMI_SAS_SATA_SIGNATURE& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SATA_SIGNATURE"); }

string CSMIPrinter::ToString(const CSMI_SAS_SATA_SIGNATURE& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_ARRAY_INT(info, idx, bSignatureFIS);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SATA_SIGNATURE_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SATA_SIGNATURE_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_SATA_SIGNATURE_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Signature);

    return sstr.str();
}

// CC_CSMI_SAS_GET_SCSI_ADDRESS

string CSMIPrinter::ToTypeString(const CSMI_SAS_GET_SCSI_ADDRESS_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_GET_SCSI_ADDRESS_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_GET_SCSI_ADDRESS_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_ARRAY_INT(info, idx, bSASAddress);
    PRINT_ARRAY_INT(info, idx, bSASLun);
    PRINT_VALUE_INT(info, idx, bHostIndex);
    PRINT_VALUE_INT(info, idx, bPathId);
    PRINT_VALUE_INT(info, idx, bTargetId);
    PRINT_VALUE_INT(info, idx, bLun);

    return sstr.str();
}

// CC_CSMI_SAS_GET_DEVICE_ADDRESS

string CSMIPrinter::ToTypeString(const CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_INT(info, idx, bHostIndex);
    PRINT_VALUE_INT(info, idx, bPathId);
    PRINT_VALUE_INT(info, idx, bTargetId);
    PRINT_VALUE_INT(info, idx, bLun);
    PRINT_ARRAY_INT(info, idx, bSASAddress);
    PRINT_ARRAY_INT(info, idx, bSASLun);

    return sstr.str();
}

// CC_CSMI_SAS_TASK_MANAGEMENT

string CSMIPrinter::ToTypeString(const CSMI_SAS_SSP_TASK_IU& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SSP_TASK_IU"); }

string CSMIPrinter::ToString(const CSMI_SAS_SSP_TASK_IU& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bHostIndex);
    PRINT_VALUE_INT(info, idx, bPathId);
    PRINT_VALUE_INT(info, idx, bTargetId);
    PRINT_VALUE_INT(info, idx, bLun);
    PRINT_VALUE_INT(info, idx, uFlags);
    PRINT_VALUE_INT(info, idx, uQueueTag);
    PRINT_VALUE_INT(info, idx, bTaskManagementFunction);
    PRINT_VALUE_INT(info, idx, uInformation);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_SSP_TASK_IU_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_SSP_TASK_IU_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_SSP_TASK_IU_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_STRUCT(info, idx, Parameters);
    PRINT_VALUE_STRUCT(info, idx, Status);

    return sstr.str();
}

// CC_CSMI_SAS_GET_CONNECTOR_INFO

string CSMIPrinter::ToTypeString(const CSMI_SAS_CONNECTOR_INFO& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CONNECTOR_INFO"); }

string CSMIPrinter::ToString(const CSMI_SAS_CONNECTOR_INFO& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, uPinout);
    PRINT_ARRAY_INT(info, idx, bConnector);
    PRINT_VALUE_INT(info, idx, bLocation);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_CONNECTOR_INFO_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CONNECTOR_INFO_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_CONNECTOR_INFO_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_ARRAY_STRUCT(info, idx, 0, Reference);

    return sstr.str();
}

// CC_CSMI_SAS_GET_LOCATION

string CSMIPrinter::ToTypeString(const CSMI_SAS_LOCATION_IDENTIFIER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_LOCATION_IDENTIFIER"); }

string CSMIPrinter::ToString(const CSMI_SAS_LOCATION_IDENTIFIER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bLocationFlags);
    PRINT_ARRAY_INT(info, idx, bSASAddress);
    PRINT_ARRAY_INT(info, idx, bSASLun);
    PRINT_ARRAY_INT(info, idx, bEnclosureIdentifier);
    PRINT_ARRAY_INT(info, idx, bEnclosureName);
    PRINT_ARRAY_INT(info, idx, bBayPrefix);
    PRINT_VALUE_INT(info, idx, bBayIdentifier);
    PRINT_VALUE_INT(info, idx, bLocationState);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_GET_LOCATION_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_GET_LOCATION_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_GET_LOCATION_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_INT(info, idx, bHostIndex);
    PRINT_VALUE_INT(info, idx, bPathId);
    PRINT_VALUE_INT(info, idx, bTargetId);
    PRINT_VALUE_INT(info, idx, bLun);
    PRINT_VALUE_INT(info, idx, bIdentify);
    PRINT_VALUE_INT(info, idx, bNumberOfLocationIdentifiers);
    PRINT_VALUE_INT(info, idx, bLengthOfLocationIdentifier);
    PRINT_ARRAY_STRUCT(info, idx, 0, Location);

    return sstr.str();
}

// CC_CSMI_SAS_PHY_CONTROL

string CSMIPrinter::ToTypeString(const CSMI_SAS_CHARACTER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_CHARACTER"); }

string CSMIPrinter::ToString(const CSMI_SAS_CHARACTER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bTypeFlags);
    PRINT_VALUE_INT(info, idx, bValue);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_PHY_CONTROL& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PHY_CONTROL"); }

string CSMIPrinter::ToString(const CSMI_SAS_PHY_CONTROL& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, bType);
    PRINT_VALUE_INT(info, idx, bRate);
    PRINT_ARRAY_INT(info, idx, uVendorUnique);
    PRINT_VALUE_INT(info, idx, uTransmitterFlags);
    PRINT_VALUE_INT(info, idx, bTransmitAmplitude);
    PRINT_VALUE_INT(info, idx, bTransmitterPreemphasis);
    PRINT_VALUE_INT(info, idx, bTransmitterSlewRate);
    PRINT_ARRAY_INT(info, idx, bTransmitterVendorUnique);
    PRINT_VALUE_INT(info, idx, uReceiverFlags);
    PRINT_VALUE_INT(info, idx, bReceiverThreshold);
    PRINT_VALUE_INT(info, idx, bReceiverEqualizationGain);
    PRINT_ARRAY_INT(info, idx, bReceiverVendorUnique);
    PRINT_VALUE_INT(info, idx, uPatternFlags);
    PRINT_VALUE_INT(info, idx, bFixedPattern);
    PRINT_VALUE_INT(info, idx, bUserPatternLength);
    PRINT_ARRAY_STRUCT(info, idx, 0, UserPatternBuffer);

    return sstr.str();
}

string CSMIPrinter::ToTypeString(const CSMI_SAS_PHY_CONTROL_BUFFER& info) { UNUSE_NAME(info); RETURN_NAME("CSMI_SAS_PHY_CONTROL_BUFFER"); }

string CSMIPrinter::ToString(const CSMI_SAS_PHY_CONTROL_BUFFER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_STRUCT(info, idx, IoctlHeader);
    PRINT_VALUE_INT(info, idx, uFunction);
    PRINT_VALUE_INT(info, idx, bPhyIdentifier);
    PRINT_VALUE_INT(info, idx, usLengthOfControl);
    PRINT_VALUE_INT(info, idx, bNumberOfControls);
    PRINT_VALUE_INT(info, idx, uLinkFlags);
    PRINT_VALUE_INT(info, idx, bSpinupRate);
    PRINT_ARRAY_INT(info, idx, uVendorUnique);
    PRINT_ARRAY_STRUCT(info, idx, 0, Control);

    return sstr.str();
}

#define MAP_ITEM(name) case name: sstr << #name; break

string CSMIPrinter::ToDeviceTypeString(U32 value) {
    stringstream sstr;
    switch(value) {
        MAP_ITEM(CSMI_SAS_NO_DEVICE_ATTACHED    );
        MAP_ITEM(CSMI_SAS_END_DEVICE            );
        MAP_ITEM(CSMI_SAS_EDGE_EXPANDER_DEVICE  );
        MAP_ITEM(CSMI_SAS_FANOUT_EXPANDER_DEVICE);
    }
    return sstr.str();
}

string CSMIPrinter::ToPortProtocolString(U32 value) {
    stringstream sstr;
    switch(value) {
        MAP_ITEM(CSMI_SAS_PROTOCOL_SATA);
        MAP_ITEM(CSMI_SAS_PROTOCOL_SMP );
        MAP_ITEM(CSMI_SAS_PROTOCOL_STP );
        MAP_ITEM(CSMI_SAS_PROTOCOL_SSP );
    }
    return sstr.str();
}

string CSMIPrinter::ToLinkRateString(U32 value) {
    stringstream sstr;
    switch(value) {
        MAP_ITEM(CSMI_SAS_LINK_RATE_UNKNOWN );
        MAP_ITEM(CSMI_SAS_PHY_DISABLED      );
        MAP_ITEM(CSMI_SAS_LINK_RATE_FAILED  );
        MAP_ITEM(CSMI_SAS_SATA_SPINUP_HOLD  );
        MAP_ITEM(CSMI_SAS_SATA_PORT_SELECTOR);
        MAP_ITEM(CSMI_SAS_LINK_RATE_1_5_GBPS);
        MAP_ITEM(CSMI_SAS_LINK_RATE_3_0_GBPS);
        MAP_ITEM(CSMI_SAS_LINK_VIRTUAL      );
    }
    return sstr.str();
}

string CSMIPrinter::ToSignalClassString(U32 value) {
    stringstream sstr;
    switch(value) {
        MAP_ITEM(CSMI_SAS_SIGNAL_CLASS_UNKNOWN  );
        MAP_ITEM(CSMI_SAS_SIGNAL_CLASS_DIRECT   );
        MAP_ITEM(CSMI_SAS_SIGNAL_CLASS_SERVER   );
        MAP_ITEM(CSMI_SAS_SIGNAL_CLASS_ENCLOSURE);
    }
    return sstr.str();
}

#undef MAP_ITEM
