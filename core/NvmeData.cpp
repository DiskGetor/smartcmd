#include "NvmeData.h"
#include "StringUtil.h"
#include "HexFormatter.h"


device_list_t NvmeData::GetNvmeListDevice(void)
{
	device_list_t lists;
    char *a;
    size_t pos;
    a = "-list";

    lists = ListDevice(1, &a);

    return lists;
}

smart_log_t NvmeData::GetNvmeSmartInfo(const string &driveName)
{
	char *a[2];
	smart_log_t smartinfo;

	a[0] = const_cast<char*>("-smart-log");
	a[1] = const_cast<char*>(driveName.c_str());

	smartinfo = GetSmartLog(2, a);

	return smartinfo;
}

NvmeDevice NvmeData::NvmeDiviceScan(const string &driveName)
{
	device_list_t lists;
	smart_log_t sm;
	vendor_log_t vd;
	string temp;
	NvmeDevice ret;
	vector<string> strlist;

	lists = GetNvmeListDevice();

	ret.err = 1;

	if(0 == lists.cnt)
		return ret;
	ret.iden = GetIdentifyInfo(driveName, lists);

	sm = GetNvmeSmartInfo(driveName);
	if(0 != sm.err)
		return ret;
	vd = GetNvmeVendorSpecLog(driveName);
	if(0 != vd.err)
		sm.nandwrite = 0;
	else
		sm.nandwrite = vd.nandwrite;
	
	ret.timestamp = time(0);
	ret.smart = sm;
	ret.err = 0;
	
	free(lists.lst);
	free(vd.data);

	return ret;
}

vendor_log_t NvmeData::GetNvmeVendorSpecLog(const string &driveName)
{
	char *a[2];
    vendor_log_t ret;
    a[0] = const_cast<char*>("-smart-log");
	a[1] = const_cast<char*>(driveName.c_str());

    ret = GetVendorSpecLog(2, a);

    return ret;
}

firmware_slot_t NvmeData::GetNvmeFwSlot(const string &driveName)
{
	char *a[2];
	firmware_slot_t ret;
	a[0] = const_cast<char*>("-smart-log");
	a[1] = const_cast<char*>(driveName.c_str());

	ret = GetFirmwareSlot(2, a);
	return ret;
}

NvmeIdenStr NvmeData::GetIdentifyInfo(const string &driveName, const device_list_t &lists)
{
	U32 i, pos;
	NvmeIdenStr ret;
	string temp;
	long long int lba;

	for(i = 0; i < lists.cnt; i++)
	{
		temp = lists.lst[i].node;
		if(0 == temp.compare(driveName))
		{
			lba = 1 << lists.lst[i].ns.lbaf[(lists.lst[i].ns.flbas & 0x0f)].ds;
        	ret.nsze = le64_to_cpu(lists.lst[i].ns.nsze) * lba;
        	ret.nuse = le64_to_cpu(lists.lst[i].ns.nuse) * lba;
			pos = sizeof(lists.lst[i].ctrl.sn) - 1;
			lists.lst[i].ctrl.sn[pos] = '\0';
			ret.sn = lists.lst[i].ctrl.sn;
			ret.sn = StringUtil::RightTrim(ret.sn);
			pos = sizeof(lists.lst[i].ctrl.mn) - 1;
			lists.lst[i].ctrl.mn[pos] = '\0';
			ret.mn = lists.lst[i].ctrl.mn;
			ret.mn = StringUtil::RightTrim(ret.mn);
			ret.fr = lists.lst[i].ctrl.fr;
			ret.fr = ret.fr.substr(0, 8);
			ret.path = temp;
			ret.hexbuf = HexFormatter::BuffToHexString((U8*)(&lists.lst[i].ctrl), sizeof(lists.lst[i].ctrl), false);
			ret.devpos = i;
			break;
		}
	}

	return ret;
}
