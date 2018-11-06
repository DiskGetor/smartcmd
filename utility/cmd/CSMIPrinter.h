#ifndef __CSMIPRINTER_H__
#define __CSMIPRINTER_H__

#include "utility/misc/StdHeader.h"

#include "CsmiSas.h"

class CSMIPrinter
{
public:
    static string ToString(const IOCTL_HEADER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_DRIVER_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_DRIVER_INFO_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PCI_BUS_ADDRESS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_IO_BUS_ADDRESS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CNTLR_CONFIG& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CNTLR_CONFIG_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CNTLR_STATUS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CNTLR_STATUS_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_FIRMWARE_DOWNLOAD& info, U32 level = 0);
    static string ToString(const CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_INFO_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_DRIVES& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_DEVICE_ID& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_ADDITIONAL_DATA& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_CONFIG& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_CONFIG_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_TYPE_DESCRIPTION& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_FEATURES& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_FEATURES_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_CONTROL& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_CONTROL_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_DRIVE_EXTENT_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_MODULE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_DRIVE_LOCATION& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_DRIVES_ADDITIONAL_DATA& info, U32 level = 0);
    static string ToString(const CSMI_SAS_DRIVE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_ELEMENT& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_ELEMENT_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_LIST& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_DRIVE_LIST& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_SPARE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_ONLINE_STATE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_CACHE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_WRITE_PROTECT_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_DELETE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_MODIFY_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_TRANSFORM_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_LABEL_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_CREATE_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_OPERATION& info, U32 level = 0);
    static string ToString(const CSMI_SAS_RAID_SET_OPERATION_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_IDENTIFY& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PHY_ENTITY& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PHY_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PHY_INFO_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SET_PHY_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SET_PHY_INFO_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_LINK_ERRORS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_LINK_ERRORS_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SMP_REQUEST& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SMP_RESPONSE& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SMP_PASSTHRU& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SMP_PASSTHRU_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SSP_PASSTHRU& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SSP_PASSTHRU_STATUS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SSP_PASSTHRU_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_STP_PASSTHRU& info, U32 level = 0);
    static string ToString(const CSMI_SAS_STP_PASSTHRU_STATUS& info, U32 level = 0);
    static string ToString(const CSMI_SAS_STP_PASSTHRU_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SATA_SIGNATURE& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SATA_SIGNATURE_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_GET_SCSI_ADDRESS_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SSP_TASK_IU& info, U32 level = 0);
    static string ToString(const CSMI_SAS_SSP_TASK_IU_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CONNECTOR_INFO& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CONNECTOR_INFO_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_LOCATION_IDENTIFIER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_GET_LOCATION_BUFFER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_CHARACTER& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PHY_CONTROL& info, U32 level = 0);
    static string ToString(const CSMI_SAS_PHY_CONTROL_BUFFER& info, U32 level = 0);

public:
    static string ToTypeString(const IOCTL_HEADER& info);
    static string ToTypeString(const CSMI_SAS_DRIVER_INFO& info);
    static string ToTypeString(const CSMI_SAS_DRIVER_INFO_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_PCI_BUS_ADDRESS& info);
    static string ToTypeString(const CSMI_SAS_IO_BUS_ADDRESS& info);
    static string ToTypeString(const CSMI_SAS_CNTLR_CONFIG& info);
    static string ToTypeString(const CSMI_SAS_CNTLR_CONFIG_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_CNTLR_STATUS& info);
    static string ToTypeString(const CSMI_SAS_CNTLR_STATUS_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_FIRMWARE_DOWNLOAD& info);
    static string ToTypeString(const CSMI_SAS_FIRMWARE_DOWNLOAD_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_RAID_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_INFO_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_RAID_DRIVES& info);
    static string ToTypeString(const CSMI_SAS_RAID_DEVICE_ID& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_ADDITIONAL_DATA& info);
    static string ToTypeString(const CSMI_SAS_RAID_CONFIG& info);
    static string ToTypeString(const CSMI_SAS_RAID_CONFIG_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_RAID_TYPE_DESCRIPTION& info);
    static string ToTypeString(const CSMI_SAS_RAID_FEATURES& info);
    static string ToTypeString(const CSMI_SAS_RAID_FEATURES_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_RAID_CONTROL& info);
    static string ToTypeString(const CSMI_SAS_RAID_CONTROL_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_DRIVE_EXTENT_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_MODULE_INFO& info);
    static string ToTypeString(const CSMI_SAS_DRIVE_LOCATION& info);
    static string ToTypeString(const CSMI_SAS_RAID_DRIVES_ADDITIONAL_DATA& info);
    static string ToTypeString(const CSMI_SAS_DRIVE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_ELEMENT& info);
    static string ToTypeString(const CSMI_SAS_RAID_ELEMENT_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_LIST& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_DRIVE_LIST& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_SPARE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_ONLINE_STATE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_CACHE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_WRITE_PROTECT_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_DELETE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_MODIFY_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_TRANSFORM_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_LABEL_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_CREATE_INFO& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_OPERATION& info);
    static string ToTypeString(const CSMI_SAS_RAID_SET_OPERATION_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_IDENTIFY& info);
    static string ToTypeString(const CSMI_SAS_PHY_ENTITY& info);
    static string ToTypeString(const CSMI_SAS_PHY_INFO& info);
    static string ToTypeString(const CSMI_SAS_PHY_INFO_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_SET_PHY_INFO& info);
    static string ToTypeString(const CSMI_SAS_SET_PHY_INFO_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_LINK_ERRORS& info);
    static string ToTypeString(const CSMI_SAS_LINK_ERRORS_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_SMP_REQUEST& info);
    static string ToTypeString(const CSMI_SAS_SMP_RESPONSE& info);
    static string ToTypeString(const CSMI_SAS_SMP_PASSTHRU& info);
    static string ToTypeString(const CSMI_SAS_SMP_PASSTHRU_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_SSP_PASSTHRU& info);
    static string ToTypeString(const CSMI_SAS_SSP_PASSTHRU_STATUS& info);
    static string ToTypeString(const CSMI_SAS_SSP_PASSTHRU_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_STP_PASSTHRU& info);
    static string ToTypeString(const CSMI_SAS_STP_PASSTHRU_STATUS& info);
    static string ToTypeString(const CSMI_SAS_STP_PASSTHRU_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_SATA_SIGNATURE& info);
    static string ToTypeString(const CSMI_SAS_SATA_SIGNATURE_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_GET_SCSI_ADDRESS_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_GET_DEVICE_ADDRESS_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_SSP_TASK_IU& info);
    static string ToTypeString(const CSMI_SAS_SSP_TASK_IU_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_CONNECTOR_INFO& info);
    static string ToTypeString(const CSMI_SAS_CONNECTOR_INFO_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_LOCATION_IDENTIFIER& info);
    static string ToTypeString(const CSMI_SAS_GET_LOCATION_BUFFER& info);
    static string ToTypeString(const CSMI_SAS_CHARACTER& info);
    static string ToTypeString(const CSMI_SAS_PHY_CONTROL& info);
    static string ToTypeString(const CSMI_SAS_PHY_CONTROL_BUFFER& info);

public:
    static string ToDeviceTypeString(U32 value);
    static string ToPortProtocolString(U32 value);
    static string ToLinkRateString(U32 value);
    static string ToSignalClassString(U32 value);
};

#endif // __CSMIPRINTER_H__
