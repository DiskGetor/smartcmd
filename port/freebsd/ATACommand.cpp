// Linux implementation

#include "utility/cmd/ATACommand.h"
#include "utility/misc/MiscUtil.h"

#include "SystemHeader.h"

#define SYSTEM_DATA sHeaderData
#define SYSTEM_CODE IOCATAREQUEST

struct sHeaderData
{
	struct ata_ioc_request header;
};

#define ATA_CMD_CONTROL                 0x01
#define ATA_CMD_READ                    0x02
#define ATA_CMD_WRITE                   0x04
#define ATA_CMD_ATAPI                   0x08

void ATACommand::buildBasicCommand()
{
    resetCommandStruct();

    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    struct ata_ioc_request& hdr = p.header;
	
	hdr.timeout = 30;
	hdr.u.ata.lba = LBA.Low;
    hdr.u.ata.count = getSectorCount();
    hdr.u.ata.feature = getFeatureCode();
    hdr.u.ata.command = getCommandCode();

	hdr.data = (char*)(void*)getBufferPtr();
	hdr.count = BufferSize;

	hdr.flags = ATA_CMD_CONTROL;
	if (isDataInCommand()) hdr.flags |= ATA_CMD_READ;
	else if (isDataOutCommand()) hdr.flags |= ATA_CMD_WRITE;

	// Overwrite some settings
    switch (CommandCode)
    {
    case ORG_ATA_SMART_READ_DATA:
    case ORG_ATA_SMART_READ_THRESHOLD:
    case ORG_ATA_SMART_READ_LOG:
		hdr.u.ata.lba = (0xC2 << 16) | (0x4F << 8) | LBA.Low;
        break;

    case ORG_ATA_READ_SECTOR: 
    case ORG_ATA_WRITE_SECTOR: break;

    case ORG_ATA_READ_DMA: 
    case ORG_ATA_WRITE_DMA: break;

    // VSC Direct
    case ORG_VSC_HYPCF_PHYSICAL_ERASE:  break;
    case ORG_VSC_HYPCF_DOWNLOAD:        break;
    case ORG_VSC_HYPCF_RESET_CARD:      hdr.u.ata.count = 0x03; break;
    case ORG_VSC_HYPCF_READ_LONG:       hdr.u.ata.count = 0x02; break;
    case ORG_VSC_HYPCF_WRITE_LONG:      hdr.u.ata.count = 0x02; break;

    // VSC via SetFeature
    case ORG_VSC_HYPCF_READ_CONTROLLER: hdr.u.ata.count = 0x44; break;
    case ORG_VSC_HYPCF_SET_FEATURE:     hdr.u.ata.count = 0xE3; break;
    case ORG_VSC_HYPCF_READ_SECTOR:     break;

    default: break;
    }
}

bool ATACommand::buildCommand()
{
    buildBasicCommand();

    // Additional update here

    return true;
}

string ATACommand::getTaskfileString()
{
	return "[FreeBSD] Not supported";
}

U8* ATACommand::getOutputTaskfilePtr()
{
    return NULL;
}

eCommandError ATACommand::getErrorStatus()
{
    SYSTEM_DATA& p = *(SYSTEM_DATA*)(void*)getStructPtr();
    struct ata_ioc_request& hdr = p.header;
	
	return (0 == hdr.error) ? CMD_ERROR_NONE : CMD_ERROR_UNKN;
}

void ATACommand::setCommandBasic(sLBA lba, U32 sectorCount, U32 cmdCode)
{
    LBA = lba;
    SectorCount = sectorCount;
    CommandCode = cmdCode;

    StructSize = sizeof (SYSTEM_DATA);
    FillerSize = 0;
    BufferSize = SectorCount * 512;
}

bool ATACommand::executeCommand(int handle)
{
    int retCode = ioctl(handle, SYSTEM_CODE, getStructPtr());

    return (retCode == 0);
}
