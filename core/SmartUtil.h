#ifndef SMARTUTIL_H
#define SMARTUTIL_H

#include "CoreData.h"
#include "CoreUtil.h"
#include "utility/device/VendorCode.h"

struct sSmartData
{
    const U8* ValueSector;
    const U8* ThresholdSector;
    eVendorCode VendorCode;

    sSmartData();
    sSmartData(U8* val, U8* thr, eVendorCode code = eUnknownVendor);
    void reset();
    void setVendorCode(eVendorCode code);
    void setSmartBuffer(const U8* val, const U8* thr);
};

class SmartUtil
{
public: // Handle SMART data
    static void CorrectAttribute(sAttribute& attr);
    static bool ParseSmartAttribute(const unsigned char* valSector, const tThresholdMap& thresholdMap, sAttribute& attr, const eVendorCode vendor);
    static void ParseSmartThreshold(const unsigned char* thrSector, tThresholdMap& thresholdMap);
    static void ParseSmartData(const sSmartData& smartData, sSmartInfo& smartInfo, bool autoCorrect = true);

public:
    static void CorrectSmartInfo(const sSmartData& smartData, sSmartInfo& info);
    static bool CorrectSmartInfo_HyperstoneUSB(const U8* valSector, const U8* thrSector, sSmartInfo& info);
    static bool CorrectSmartInfo_Realtek(sSmartInfo& info);

public:
	static void ParseNvmeSmartInfo(const U8* logSector, sSmartInfo& info);
	static void ParseNvmeIdentifyInfo(const U8* logSector, sIdentifyInfo& info);
	
	static void BuildNvmeSmartInfo(const U8* logSector, nvme_smart_log& nvmeInfo);
	static void BuildNvmeIdentifyInfo(const U8* logSector, nvme_id_ctrl& nvmeInfo);

public:
    static void BuildIdentifySector(unsigned char* buff, const struct sIdentifyInfo* id);
    static void BuildSmartSector(unsigned char* sector, const struct sSmartInfo* smartInfo);
    static void BuildThresholdSector(unsigned char* sector, const struct sSmartInfo* id);
};

#endif // SMARTUTIL_H

