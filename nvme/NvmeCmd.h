#ifndef NVME_CMD
#define NVME_CMD

#ifdef _cplusplus
extern "C" {
#endif

#include "nvme.h"

typedef struct device_list
{
	struct list_item *lst;
	unsigned int cnt;
} device_list_t;

typedef struct smart_log
{
	unsigned int 	id;
	__u8 			err;
	const char 		*dvname;
	__u8			critical_warning;
	__u8			temperature[2];
	__u8			avail_spare;
	__u8			spare_thresh;
	__u8			percent_used;
	__u8			rsvd6[26];
	long double		data_units_read;
	long double		data_units_written;
	long double		host_reads;
	long double		host_writes;
	long double		ctrl_busy_time;
	long double		power_cycles;
	long double		power_on_hours;
	long double		unsafe_shutdowns;
	long double		media_errors;
	long double		num_err_log_entries;
	__u32			warning_temp_time;
	__u32			critical_comp_time;
	__u16			temp_sensor[8];
	__u32			thm_temp1_trans_count;
	__u32			thm_temp2_trans_count;
	__u32			thm_temp1_total_time;
	__u32			thm_temp2_total_time;
	__u8			rsvd232[280];
	long double		nandwrite;
	struct nvme_smart_log smartraw;
} smart_log_t;

typedef struct vendor_log
{
	__u8 err;
	long double	 nandwrite;
	__u8 *data;
} vendor_log_t;

typedef struct firmware_slot
{
	__u8 err;
	struct nvme_firmware_log_page data;
} firmware_slot_t;

#ifdef _cplusplus
}
#endif

#endif
