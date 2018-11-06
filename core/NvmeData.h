#ifndef _NVME_DATA_H_
#define _NVME_DATA_H_


#include "StdHeader.h"
#include <time.h>
extern "C" {
    #include "NvmeCmd.h"
    device_list_t ListDevice(int argc, char **argv);
    smart_log_t GetSmartLog(int argc, char **argv);
    vendor_log_t GetVendorSpecLog(int argc, char **argv);
    firmware_slot_t GetFirmwareSlot(int argc, char **argv);
}

struct NvmeIdenStr
{
	string sn;
	string mn;
	string fr;
	string path;
	string hexbuf;
	double nsze;
    double nuse;
	__u32 devpos;
};

struct NvmeDevice
{
	__u8 err;
	__u32 timestamp;
	NvmeIdenStr iden;
	smart_log_t smart;
};

class NvmeData
{
public:
	static device_list_t GetNvmeListDevice(void);
	static smart_log_t GetNvmeSmartInfo(const string &driveName);
	static NvmeDevice NvmeDiviceScan(const string &driveName);
	static NvmeIdenStr GetIdentifyInfo(const string &driveName, const device_list_t &lists);
	static vendor_log_t GetNvmeVendorSpecLog(const string &driveName);
	static firmware_slot_t GetNvmeFwSlot(const string &driveName);
};

#endif