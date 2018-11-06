// Linux implementation

#include "utility/cmd/NVMECommand.h"
#include "utility/misc/MiscUtil.h"

#include "SystemHeader.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE HDIO_DRIVE_CMD

struct sDriveCmdData
{
	U8 rawbuff[512];
};

struct sDriveTaskfileData
{
	ide_ioctl_request_t ioreq;
	ide_task_request_t taskreq;
};

struct sHeaderData
{
	union {
		sDriveCmdData cmddata;
		sDriveTaskfileData taskdata;
	} header;
};

static U32 GetSystemCode(U32 cmdCode)
{
    switch(cmdCode)
    {
        case ORG_ATA_SMART_ENABLE:
        case ORG_ATA_SMART_DISABLE:
        case ORG_ATA_SMART_READ_DATA:
        case ORG_ATA_SMART_READ_THRESHOLD:
        case ORG_ATA_SMART_READ_LOG:
        case ORG_ATA_IDENTIFY_DEVICE:
            return HDIO_DRIVE_CMD;

        default:
            return HDIO_DRIVE_TASKFILE;
    }
}

void NVMECommand::buildBasicCommand()
{
    resetCommandStruct();
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();

    U8* raw = p.header.cmddata.rawbuff;
    sDriveTaskfileData& hdr = p.header.taskdata;

	switch (CommandCode)
	{
		case ORG_ATA_SMART_ENABLE:
		case ORG_ATA_SMART_DISABLE:
		case ORG_ATA_SMART_READ_DATA:
		case ORG_ATA_SMART_READ_THRESHOLD:
		case ORG_ATA_SMART_READ_LOG:
        {
			raw[0] = getCommandCode();
			raw[2] = getFeatureCode();
			raw[3] = getSectorCount();
			raw[4] = 0x4F;
			raw[5] = 0xC2;
		} break;
		
		case ORG_ATA_IDENTIFY_DEVICE:
        {
			raw[0] = getCommandCode();
			raw[3] = getSectorCount();
		} break;
		
		default:
        {
			hdr.ioreq.task_request = &hdr.taskreq;
			hdr.ioreq.in_buffer = getBufferPtr();
			hdr.ioreq.out_buffer = getBufferPtr();
			
			ide_task_request_t& idereq = hdr.taskreq;
			idereq.data_phase = TASKFILE_NO_DATA;
			idereq.req_cmd = IDE_DRIVE_TASK_NO_DATA;
			
			if (isDataInCommand())
			{
				idereq.data_phase = isPioMode() ? TASKFILE_IN : TASKFILE_IN_DMA;
				idereq.req_cmd = IDE_DRIVE_TASK_IN;
				idereq.in_size = BufferSize;
			}
			
			if (isDataOutCommand())
			{
				idereq.data_phase = isPioMode() ? TASKFILE_OUT : TASKFILE_OUT_DMA;
				idereq.req_cmd = IDE_DRIVE_TASK_OUT;
				idereq.out_size = BufferSize;
			}
			
			buildTaskstruct(idereq.io_ports);
		} break;
	}
}

bool NVMECommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string NVMECommand::getTaskfileString()
{
	return "";
}

U8* NVMECommand::getOutputTaskfilePtr()
{
	return NULL;
}

eCommandError NVMECommand::getErrorStatus()
{
	return CMD_ERROR_NONE;
}

void NVMECommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool NVMECommand::executeCommand(int handle)
{
	if (ORG_ATA_IDENTIFY_DEVICE == CommandCode)
	{
		U16 devid[256];
		if (!ioctl(handle, HDIO_GET_IDENTITY, devid) && (devid[0] & 0x8000))
		{
			SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
			U8* raw = p.header.cmddata.rawbuff;
			
            raw[0] = GetCommandCode(ORG_ATA_IDENTIFY_PDEVICE);
		}
	}
	
    U32 sysCode = GetSystemCode(CommandCode);
    int retCode = ioctl(handle, sysCode, getStructPtr());

    // Shift the buffer if needed
    if (HDIO_DRIVE_CMD == sysCode) memcpy(getBufferPtr(), getStructPtr() + 4, 512);

    return (retCode == 0);
}
