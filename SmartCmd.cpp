#include "utility/cmd/ATACommand.h"
#include "utility/misc/StdMacro.h"
#include "utility/misc/MiscUtil.h"
#include "utility/misc/FileUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/misc/SystemUtil.h"
#include "utility/misc/HexFormatter.h"
#include "utility/vsc/virtium/VscVirtiumFunction.h"

#include "AppData.h"
#include "SmartCmd.h"
#include "NvmeData.h"

SmartCmd::SmartCmd()
{
    reset();

}

SmartCmd::~SmartCmd()
{

}

void SmartCmd::reset()
{
    m_TestName = "DefaultName";
    
    const char* usbModelList[] = {
        "VTDU",
    };

    for (U32 i = 0, serialCount = sizeof(usbModelList) / sizeof(usbModelList[0]); i < serialCount; i++) {
        m_ValidVirtiumUsbModel.push_back(usbModelList[i]);
    }

    const char* cfModelList[] = {
        "VTDC",
    };

    for (U32 i = 0, serialCount = sizeof(cfModelList) / sizeof(cfModelList[0]); i < serialCount; i++) {
        m_ValidVirtiumCfModel.push_back(cfModelList[i]);
    }
}

void SmartCmd::setToolInfo(int argc, char *argv[])
{
    reset();

    FileUtil::GetFileName(argv[0], m_AppName);

    if (argc < 2){ printHelp(); exit(1); }

    U32 operation = 0;
    string flag = argv[1];

    if     ((0 == flag.compare("-f" )) && ((argc  == 3) || (argc >= 5))) operation = 1; // Run with config
    else if((0 == flag.compare("-g" )) && (argc  == 3)) operation = 2; // Generate sample config
    else if((0 == flag.compare("-s" )) && (argc  == 2)) operation = 3; // Scan
    else if((0 == flag.compare("-r" )) && (argc  == 3)) operation = 4; // Dump Life Percent/Retention info
    else if((0 == flag.compare("-m" )) && (argc  == 3)) operation = 5; // Dump SMART

    else if(0 == flag.compare("--get-vtview-prime-log" ))   operation = 6; // Read SMART Log
    else if(0 == flag.compare("--get-workload-stat" ))      operation = 7; // Retrieve vtView Prime workload statistics
    else if(0 == flag.compare("--get-sampling-rate-info" )) operation = 8; // Get sampling rate drive record SMART log
    else if(0 == flag.compare("--set-sampling-rate" ))      operation = 9; // Set sampling rate drive record SMART log

    else if((0 == flag.compare("-v" )) && (argc  == 2)) operation = 17;// Version
    else if((0 == flag.compare("-l" )) && (argc  == 2)) operation = 18;// License
    else if((0 == flag.compare("-h" )) && (argc  == 2)) operation = 19;// Show help
    else if((0 == flag.compare("-dg")) && (argc  == 3)) operation = 20;// Debug. Generate sample config, debug mode
    else if((0 == flag.compare("-di")) && (argc  == 3)) operation = 21;// Debug. Dump identify info
    else if((0 == flag.compare("-dm")) && (argc  == 3)) operation = 22;// Debug. Dump smart info
    else if((0 == flag.compare("-ph")) && (argc  == 2)) operation = 23;// show vtView Prime commands
    else if((0 == flag.compare("-i")) && (argc  == 3)) operation = 24;// show identify
    else if((0 == flag.compare("-sv")) && (argc  == 3)) operation = 25;
    else if((0 == flag.compare("-dfs")) && (argc  == 3)) operation = 26;
    else if((0 == flag.compare("-fs")) && (argc  == 3)) operation = 27;
    else if((0 == flag.compare("-sm")) && (argc  == 3)) operation = 28;

    switch (operation)
    {
    case 0:
        cerr << "[Error] Invalid arguments" << endl << endl;
        printHelp(); exit(1); break;

    case 1: // run with config
    {
        bool status = false;
        string check, temp;
        string otcheck;
        int i, tnlen;

        if(argc >= 5)
        {
            m_TestName = "";
            check = argv[3];
            if(0 == check.compare("-tn"))
            {
                tnlen = 0;
                for(i = 4; i < argc; i++)
                {
                    temp = argv[i];
                    otcheck = temp.substr(0,1);
                    if(0 != otcheck.compare("-"))
                        m_TestName += temp + " ";
                    else break;
                }
            }
            else
            {
                printHelp(); exit(1);
            }
        }
        m_TestName = StringUtil::RightTrim(m_TestName);

        do {
            status = m_Config.loadConfig(argv[2]);
            if (false == status) { m_Config.setErrorState(AppConfigFile::ERROR_CONFIG); break; }

            status = m_Config.parseConfig();
            if (false == status) break;

            status = true;
        } while(0);

        if (false == status)
        {
            cerr << "[Error] " << m_Config.toErrorString() << endl << endl;
            exit(1);
        }
    } break;

    case 2: printConfig(argv[2], false); cout << endl; exit(0); break;
    case 3: printDevice(); cout << endl; exit(0); break;
    case 4: printUserIdentify(argv[2]); cout << endl; exit(0); break;
    case 5: printUserSmart(argv[2]); cout << endl; exit(0); break;

    case 6:
        if(argc != 3)
        {
            cerr << "[Error] Invalid arguments" << endl << endl;
            printPrimeHelp();
            exit(1);
            break;
        }

        saveVirtiumPrimeLog(argv[2]);
        exit(0);
        break;

    case 7:
        if(argc != 3)
        {
            cerr << "[Error] Invalid arguments" << endl << endl;
            printPrimeHelp();
            exit(1);
            break;
        }

        saveWorkloadLog(argv[2]);
        exit(0);
        break;

    case 8:
        if(argc != 3)
        {
            cerr << "[Error] Invalid arguments" << endl << endl;
            printPrimeHelp();
            exit(1);
            break;
        }

        getVirtiumFwSamplingRateInfo(argv[2]);
        exit(0);
        break;

    case 9:
        if(argc != 4)
        {
            cerr << "[Error] Invalid arguments" << endl << endl;
            printPrimeHelp();
            exit(1);
            break;
        }

        setVirtiumFwSamplingRateSmartLog(argv[3], argv[2]);
        exit(0);
        break;


    case 17: printVersion(); cout << endl; exit(0); break;
    case 18: printLicense(); exit(0); break;
    case 19: printHelp(); exit(0); break;

    case 20: printConfig(argv[2], true); exit(0); break;
    case 21: printDebugIdentify(argv[2]); exit(0); break;
    case 22: printDebugSmart(argv[2]); exit(0); break;
    case 23: printPrimeHelp(); exit(0); break;
    case 24: PrintDetailIdentify(argv[2]); exit(0); break;
    case 25: PrintNvmeVendorLog(argv[2]); exit(0); break;
    case 26: PrintNvmeFwSlot(argv[2]); exit(0); break;
    case 27: PrintDetailNvmeFwSlot(argv[2]); exit(0); break;
    case 28: PrintDetailSataSmart(argv[2]); exit(0); break;
    default: exit(1); break;
    }
}

void SmartCmd::printHelp()
{
    stringstream hstr;
    hstr << "1st use, please read:" << endl;
    hstr << "  Step 1: " << m_AppName << " -s to list devices and their paths (/dev/nvme0n1, etc)" << endl;
    hstr << "  Step 2: " << m_AppName << " -g config.txt to generate config file and to setup recording condition" << endl;
    hstr << "  Step 3: use text editor to edit config.txt and set recording condition" << endl;
    hstr << "  Step 4: " << m_AppName << " -f config.txt to start recording" << endl;
    hstr << endl;

    hstr << "Usage: " << m_AppName << " [option]" << endl;
    hstr << endl;

    hstr << "Option: " << endl;
    hstr << "  -s                        List SMART devices" << endl;
    hstr << "  -g  file                  Generate sample config file" << endl;
    hstr << "  -f  file                  Run program with config file" << endl;
    hstr << "  -f  file -tn testname     Run program with config file and name the test session" << endl;
    hstr << "  -r  dev                   Show Life Percent and Power off Data Retention Estimate" << endl;
    hstr << "  -i  dev                   Show Identify information" << endl;
    hstr << "  -m  dev                   Show SMART information about device" << endl;
    hstr << "  -h                        Show this help" << endl;
    hstr << "  -v                        Show version information" << endl;
    hstr << "  -l                        Show license information" << endl;
    hstr << "  -sm dev                   Show detail smart information" << endl;

    hstr << endl;
    cout << hstr.str();
}

void SmartCmd::printLicense()
{
    cout << MiscUtil::GetLicenseString() << endl;
}

void SmartCmd::printVersion()
{
    cout << MiscUtil::GetAboutString(m_AppName, true) << endl;
}

void SmartCmd::printUserIdentify(const string &driveName)
{
    if(isNvmeDevice(driveName))
        printUserNvmeIdentify(driveName);
    else
        printUserSataIdentify(driveName);
}

void SmartCmd::printUserNvmeIdentify(const string &driveName)
{
    device_list_t lists;
    NvmeIdenStr iden;

    lists = NvmeData::GetNvmeListDevice();

    if(0 == lists.cnt)
    {
        cout << "[Error] Cannot read device " << driveName << endl;
        return;
    }

    iden = NvmeData::GetIdentifyInfo(driveName, lists);
    if(!iden.mn.empty())
    {
        /*cout << "NVMe Identify Information" << endl;
        cout << "Model                  : " << iden.mn << endl;
        cout << "Serial Number          : " << iden.sn << endl;
        cout << "Firmware Version       : " << iden.fr << endl;
        cout << "Total NVMe Capacity    : " << toPrecisionString(iden.nsze / 1000000000, 0) + " GB" << endl;
        // cout << "Unallocated NVMe Capacity \t\t: " << toPrecisionString((iden.nsze - iden.nuse), 0) << endl;*/
        
        U32 remLife = 0;
        U32 percentageUsed = 0;
        
        smart_log_t smartinfo = NvmeData::GetNvmeSmartInfo(driveName);
        if(0 != smartinfo.err)
        {
            cout << "[Error] Cannot read device" << driveName << endl;
            return;
        }
        percentageUsed = smartinfo.percent_used;
        
        if (100 - percentageUsed < 0) {remLife = 0;}
        else { remLife = 100 - percentageUsed;}
        
        stringstream sstr;
        sstr << "Below information is only accurate for Virtium StorFly SSD" << endl;
        sstr << "Drive Information" << endl;
        sstr << "Model: " << iden.mn << endl;
        sstr << "Serial: " << iden.sn << endl;
        sstr << "Life%: " << remLife << " %" << endl;
        sstr << endl;
        
        //TODO: Calculate Retention for Nvme later
        
        // Parse data retention info from smartInfo
        sDataRetentionInfo retInfo;

        //eNandType nandType = GetNandType(smartInfo, driveInfo.SerialNumber);

        //U32 eraseCount = 0;
        //ReadAttribute(smartInfo, ATTR_MAX_ERASE, eraseCount);

        //GetPowerOffDataRetentionEstimate(nandType, eraseCount, retInfo);
        GetPowerOffDataRetentionEstimate(NAND_MLC, 0, retInfo);   //hardcode, will be remove later
        
        char todaystr[80];
        time_t curtime = time(0);
        strftime(todaystr, sizeof(todaystr) / sizeof(todaystr[0]), "%m/%d/%Y", localtime(&curtime));

        #define DUMP_TEMP_INFO(ctmp) do { \
            U32 ftmp = ctmp * 9 / 5 + 32; \
            string timestr = MiscUtil::ToTimeString(retInfo.RemainingDay_##ctmp##C, true); \
            sstr << setw(3) << setfill(' ') \
                 << ftmp <<" F" << " (" << ctmp << " C)    " \
                 << timestr << " from " << todaystr << endl; \
        } while(0)

        sstr << "Estimate Power off Data Retention " << endl;
        DUMP_TEMP_INFO(25);
        DUMP_TEMP_INFO(40);
        DUMP_TEMP_INFO(55);
        DUMP_TEMP_INFO(70);
        DUMP_TEMP_INFO(85);

        #undef DUMP_TEMP_INFO
        
        cout << sstr.str();
    }

}

void SmartCmd::printUserSataIdentify(const string &driveName)
{
    do {
        Device dev;
        if (false == dev.initDevice(driveName))
        {
            cerr << "[Error] Cannot read device " << driveName << endl;
            break;
        }

        sIdentifyInfo driveInfo;
        if (false == dev.readIdentify(driveInfo))
        {
            cerr << "[Error] Cannot read identify information on device " << driveName << endl;
            break;
        }

        sSmartInfo smartInfo;
        if (false == dev.readSmart(smartInfo))
        {
            cerr << "[Error] Cannot read Data Retention information on device " << driveName << endl;
            break;
        }

        // Parse data retention info from smartInfo
        sDataRetentionInfo retInfo;

        eNandType nandType = GetNandType(smartInfo, driveInfo.SerialNumber);

        U32 eraseCount = 0;
        ReadAttribute(smartInfo, ATTR_MAX_ERASE, eraseCount);

        GetPowerOffDataRetentionEstimate (nandType, eraseCount, retInfo);

        stringstream sstr;

        U32 remLife = 0;
        eVendorCode mVendorCode = dev.getVendorCode();
        switch(mVendorCode) {
            case eToshibaDevice: {
                U32 nomalizedMediaWearOut = 0;
                ReadAttributeValue(smartInfo, TSB_ATTR_WEAR_OUT, nomalizedMediaWearOut);
                remLife = nomalizedMediaWearOut / 2;
            } break;
            default: {
                ReadAttribute(smartInfo, ATTR_LIFE_LEFT, remLife);
            }
        }

        sstr << "Below information is only accurate for Virtium StorFly SSD" << endl;
        sstr << "Drive Information" << endl;
        sstr << "Model: " << driveInfo.DeviceModel << endl;
        sstr << "Serial: " << driveInfo.SerialNumber << endl;
        sstr << "Life%: " << remLife << " %" << endl;
        sstr << endl;

        char todaystr[80];
        time_t curtime = time(0);
        strftime(todaystr, sizeof(todaystr) / sizeof(todaystr[0]), "%m/%d/%Y", localtime(&curtime));

        #define DUMP_TEMP_INFO(ctmp) do { \
            U32 ftmp = ctmp * 9 / 5 + 32; \
            string timestr = MiscUtil::ToTimeString(retInfo.RemainingDay_##ctmp##C, true); \
            sstr << setw(3) << setfill(' ') \
                 << ftmp <<" F" << " (" << ctmp << " C)    " \
                 << timestr << " from " << todaystr << endl; \
        } while(0)

        sstr << "Estimate Power off Data Retention " << endl;
        DUMP_TEMP_INFO(25);
        DUMP_TEMP_INFO(40);
        DUMP_TEMP_INFO(55);
        DUMP_TEMP_INFO(70);
        DUMP_TEMP_INFO(85);

        #undef DUMP_TEMP_INFO

        cout << sstr.str();
    } while(0);
}

void SmartCmd::printUserSmart(const string &driveName)
{
    if(isNvmeDevice(driveName))
        printUserNvmeSmart(driveName);
    else
        printUserSataSmart(driveName);
}

void SmartCmd::printUserSataSmart(const string &driveName)
{
    do {
        Device dev;
        if (false == dev.initDevice(driveName))
        {
            cerr << "[Error] Cannot read device " << driveName << endl;
            break;
        }

        sSmartInfo smartInfo;
        if (false == dev.readSmart(smartInfo))
        {
            cerr << "[Error] Cannot read SMART information on device " << driveName << endl;
            break;
        }

        cout << "SMART attributes " << endl;
        cout << ToVerboseString(smartInfo);
    } while(0);
}

// void SmartCmd::printUserNvmeSmart(const string &driveName)
// {
//     smart_log_t smartinfo;
//     U32 i;
//     int temperature;

//     smartinfo = NvmeData::GetNvmeSmartInfo(driveName);
//     if(0 != smartinfo.err)
//     {
//         cout << "[Error] Cannot read device" << driveName << endl;
//         return;
//     }

//     temperature = ((smartinfo.temperature[1] << 8) | smartinfo.temperature[0]) - 273;

//     cout << FRMT_STR(38, "Critical Warning") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.critical_warning, 0) << endl;
//     cout << FRMT_STR(38, "Temperature") << FRMT_STR(4, ":  ") << (toPrecisionString(temperature, 0) + " C") << endl;
//     cout << FRMT_STR(38, "Available Spare") << FRMT_STR(4, ":  ") << (toPrecisionString(smartinfo.avail_spare, 0) + " %") << endl;
//     cout << FRMT_STR(38, "Available Spare Threshold") << FRMT_STR(4, ":  ") << (toPrecisionString(smartinfo.spare_thresh, 0) + " %") << endl;
//     cout << FRMT_STR(38, "Percentage Used") << FRMT_STR(4, ":  ") << (toPrecisionString(smartinfo.percent_used, 0) + " %") << endl;
//     cout << FRMT_STR(38, "Data Units Read") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.data_units_read, 0) << endl;
//     cout << FRMT_STR(38, "Data Units Written") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.data_units_written, 0) << endl;
//     cout << FRMT_STR(38, "Host Read Commands") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.host_reads, 0) << endl;
//     cout << FRMT_STR(38, "Host Write Commands") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.host_writes, 0) << endl;
//     cout << FRMT_STR(38, "Controller Busy Time") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.ctrl_busy_time, 0) << endl;
//     cout << FRMT_STR(38, "Power Cycles") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.power_cycles, 0) << endl;
//     cout << FRMT_STR(38, "Power On Hours") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.power_on_hours, 0) << endl;
//     cout << FRMT_STR(38, "Unsafe Shutdowns") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.unsafe_shutdowns, 0) << endl;
//     cout << FRMT_STR(38, "Media Errors") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.media_errors, 0) << endl;
//     cout << FRMT_STR(38, "Num Error Log Entries") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.num_err_log_entries, 0) << endl;
//     cout << FRMT_STR(38, "Warning Temperature Time") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.percent_used, 0) << endl;
//     cout << FRMT_STR(38, "Critical Composite Temperature Time") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.critical_comp_time, 0) << endl;
//     for(i = 0; i < 8; i++)
//     {
//         __s32 temp = smartinfo.temp_sensor[i];
//         if(0 == temp)
//             continue;
//         cout << FRMT_STR(38, "Temperature Sensor " + toPrecisionString(i, 0)) << FRMT_STR(4, ":  ") << (toPrecisionString((temp - 273), 0) + " C") << endl;
//     }
//     cout << FRMT_STR(38, "Thermal Management T1 Trans Count") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.thm_temp1_trans_count, 0) << endl;
//     cout << FRMT_STR(38, "Thermal Management T2 Trans Count") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.thm_temp2_trans_count, 0) << endl;
//     cout << FRMT_STR(38, "Thermal Management T1 Total Time") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.thm_temp1_total_time, 0) << endl;
//     cout << FRMT_STR(38, "Thermal Management T2 Total Time") << FRMT_STR(4, ":  ") << toPrecisionString(smartinfo.thm_temp2_total_time, 0) << endl;
// }

void SmartCmd::printUserNvmeSmart(const string &driveName)
{
    smart_log_t smartinfo;
    U32 i;
    int temperature;

    smartinfo = NvmeData::GetNvmeSmartInfo(driveName);
    if(0 != smartinfo.err)
    {
        cout << "[Error] Cannot read device" << driveName << endl;
        return;
    }

    cout << "Smart Data information" << endl;
    temperature = ((smartinfo.temperature[1] << 8) | smartinfo.temperature[0]) - 273;

    cout << "Critical Warning                       " << ":  " << toPrecisionString(smartinfo.critical_warning, 0) << endl;
    cout << "Temperature                            " << ":  " << (toPrecisionString(temperature, 0) + " C") << endl;
    cout << "Available Spare                        " << ":  " << (toPrecisionString(smartinfo.avail_spare, 0) + " %") << endl;
    cout << "Available Spare Threshold              " << ":  " << (toPrecisionString(smartinfo.spare_thresh, 0) + " %") << endl;
    cout << "Percentage Used                        " << ":  " << (toPrecisionString(smartinfo.percent_used, 0) + " %") << endl;
    cout << "Data Units Read                        " << ":  " << toPrecisionString(smartinfo.data_units_read, 0) << endl;
    cout << "Data Units Written                     " << ":  " << toPrecisionString(smartinfo.data_units_written, 0) << endl;
    cout << "Host Read Commands                     " << ":  " << toPrecisionString(smartinfo.host_reads, 0) << endl;
    cout << "Host Write Commands                    " << ":  " << toPrecisionString(smartinfo.host_writes, 0) << endl;
    cout << "Controller Busy Time                   " << ":  " << toPrecisionString(smartinfo.ctrl_busy_time, 0) << endl;
    cout << "Power Cycles                           " << ":  " << toPrecisionString(smartinfo.power_cycles, 0) << endl;
    cout << "Power On Hours                         " << ":  " << toPrecisionString(smartinfo.power_on_hours, 0) << endl;
    cout << "Unsafe Shutdowns                       " << ":  " << toPrecisionString(smartinfo.unsafe_shutdowns, 0) << endl;
    cout << "Media Errors                           " << ":  " << toPrecisionString(smartinfo.media_errors, 0) << endl;
    cout << "Num Error Log Entries                  " << ":  " << toPrecisionString(smartinfo.num_err_log_entries, 0) << endl;
    cout << "Warning Temperature Time               " << ":  " << toPrecisionString(smartinfo.percent_used, 0) << endl;
    cout << "Critical Composite Temperature Time    " << ":  " << toPrecisionString(smartinfo.critical_comp_time, 0) << endl;
    for(i = 0; i < 8; i++)
    {
        __s32 temp = smartinfo.temp_sensor[i];
        if(0 == temp)
        {
            cout << "Temperature Sensor " + toPrecisionString(i, 0) << "                   :  " << "NC" << endl;
            continue;
        }
        cout << "Temperature Sensor " + toPrecisionString(i, 0) << "                   :  " << (toPrecisionString((temp - 273), 0) + " C") << endl;

    }
    cout << "Thermal Management T1 Trans Count      " << ":  " << toPrecisionString(smartinfo.thm_temp1_trans_count, 0) << endl;
    cout << "Thermal Management T2 Trans Count      " << ":  " << toPrecisionString(smartinfo.thm_temp2_trans_count, 0) << endl;
    cout << "Thermal Management T1 Total Time       " << ":  " << toPrecisionString(smartinfo.thm_temp1_total_time, 0) << endl;
    cout << "Thermal Management T2 Total Time       " << ":  " << toPrecisionString(smartinfo.thm_temp2_total_time, 0) << endl;
}

bool SmartCmd::isVirtiumUsbDevice(const string &model) {
    bool status = false;

    for (U32 i = 0, serialCount = m_ValidVirtiumUsbModel.size(); (i < serialCount) && !status; i++)
    {
        status |= (string::npos != model.find(m_ValidVirtiumUsbModel[i]));
    }

    return status;
}

bool SmartCmd::isVirtiumCfDevice(const string &model) {
    bool status = false;

    for (U32 i = 0, serialCount = m_ValidVirtiumCfModel.size(); (i < serialCount) && !status; i++)
    {
        status |= (string::npos != model.find(m_ValidVirtiumCfModel[i]));
    }

    return status;
}

void SmartCmd::PrintDetailIdentify(const string &driveName)
{
    if(isNvmeDevice(driveName)) {
        device_list_t lists;
        NvmeIdenStr iden;
        
        lists = NvmeData::GetNvmeListDevice();
        iden = NvmeData::GetIdentifyInfo(driveName, lists);
        
        if(!iden.mn.empty())
        {
            cout << ParseNvmeIdenity(lists, iden);
        }
    } else {
        do {
            Device dev;
            if (false == dev.initDevice(driveName))
            {
                cerr << "[Error] Cannot read device " << driveName << endl;
                break;
            }    

            sIdentifyInfo driveInfo;
            if (false == dev.readIdentify(driveInfo))
            {
                cerr << "[Error] Cannot read identify information on device " << driveName << endl;
                break;
            }    
        
            if (true == isVirtiumUsbDevice(driveInfo.DeviceModel)) {
                printUserIdentify(driveName);
            } else if (true == isVirtiumCfDevice(driveInfo.DeviceModel)) {
                printUserIdentify(driveName);
            } else {
                cout << ParseIdenity(driveInfo, dev.getBufferPtr());
            }
        } while(0);
    }
}

void SmartCmd::PrintNvmeVendorLog(const string &driveName)
{
    vendor_log_t dat;

    dat = NvmeData::GetNvmeVendorSpecLog(driveName);

    if(dat.err)
    {
        cout << "[ERROR]: Cannot read device " << driveName << endl;
        return;
    }

    cout << "Vendor Specific log data:" << endl;
    cout << HexFormatter::ToHexString(dat.data, 512) << endl;

    free(dat.data);
}

void SmartCmd::PrintNvmeFwSlot(const string &driveName)
{
    firmware_slot_t dat;

    dat = NvmeData::GetNvmeFwSlot(driveName);

    if(dat.err)
    {
        cout << "[ERROR]: Cannot read device " << driveName << endl;
        return;
    }

    cout << "Firmware Slot information:" << endl;
    cout << HexFormatter::ToHexString((U8 *)&dat.data, 512) << endl;
}

void SmartCmd::PrintDetailNvmeFwSlot(const string &driveName)
{
    firmware_slot_t dat;

    dat = NvmeData::GetNvmeFwSlot(driveName);

    if(dat.err)
    {
        cout << "[ERROR]: Cannot read device " << driveName << endl;
        return;
    }

    cout << ParseNvmeFirmwareSlot(dat);
}

void SmartCmd::PrintDetailSataSmart(const string &driveName)
{
    do {
        Device dev;
        if (false == dev.initDevice(driveName))
        {
            cerr << "[Error] Cannot read device " << driveName << endl;
            break;
        }

        sSmartInfo smartInfo;
        if (false == dev.readSmart(smartInfo))
        {
            cerr << "[Error] Cannot read SMART information on device " << driveName << endl;
            break;
        }

        cout << "Detail SMART attributes " << endl;
        cout << parseDetailSataSmart(smartInfo, dev.getVendorCode());
    } while(0);
}

void SmartCmd::printDebugIdentify(const string &driveName)
{
    if(isNvmeDevice(driveName)) 
        printDebugNvmeIdentify(driveName);
    else
        printDebugSataIdentify(driveName);
}

void SmartCmd::printDebugNvmeIdentify(const string& driveName)
{
    device_list_t lists;
    U8 check = 0;
    U32 i;
    string temp;

    lists = NvmeData::GetNvmeListDevice();
    cout << "Identify sector:" << endl;
    if(0 != lists.cnt)
    {
        for(i = 0; i < lists.cnt; i++)
        {
            temp = lists.lst[i].node;
            if(0 == temp.compare(driveName))
            {
                cout << HexFormatter::ToHexString((U8*)(&lists.lst[i].ctrl), sizeof(lists.lst[i].ctrl)) << endl;
                check = 1;
                break;
            }
        }
    }

    if(0 == check) cout << "[Error] Cannot read device " << driveName << endl;
}

void SmartCmd::printDebugSataIdentify(const string& driveName)
{
    do {
        Device dev;
        if (false == dev.initDevice(driveName))
        {
            cerr << "[Error] Cannot read device " << driveName << endl;
            break;
        }

        if (false == dev.readIdentify())
        {
            cerr << "[Error] Cannot read identify information on device " << driveName << endl;
            break;
        }

        cout << "Identify sector: " << endl;
        cout << HexFormatter::ToHexString(dev.getBufferPtr(), 512) << endl;
    } while(0);
}

void SmartCmd::printDebugSmart(const string &driveName)
{
    if(isNvmeDevice(driveName))
        printDebugNvmeSmart(driveName);
    else
        printDebugSataSmart(driveName);
}

void SmartCmd::printDebugSataSmart(const string &driveName)
{
    do {
        Device dev;
        if (false == dev.initDevice(driveName))
        {
            cerr << "[Error] Cannot read device " << driveName << endl;
            break;
        }

        U8 valBuffer[512], thrBuffer[512];

        if (false == dev.readSmart(valBuffer, thrBuffer))
        {
            cerr << "[Error] Cannot read SMART information on device " << driveName << endl;
            break;
        }

        cout << "SMART Data sector: " << endl;
        cout << HexFormatter::ToHexString(valBuffer, 512) << endl;

        cout << "SMART Threshold sector: " << endl;
        cout << HexFormatter::ToHexString(thrBuffer, 512) << endl;

    } while(0);
}

void SmartCmd::printDebugNvmeSmart(const string &driveName)
{
    smart_log_t sm;
    sm = NvmeData::GetNvmeSmartInfo(driveName);

    if(0 != sm.err)
    {
        cout << "[Error] Cannot read device" << driveName << endl;
        return;
    }

    cout << "SMART Data sector: " << endl;
    cout << HexFormatter::ToHexString((U8*)(&sm.smartraw), sizeof(sm.smartraw)) << endl;
}

void SmartCmd::printDevice()
{
    tDriveList driveList;
    U32 i = 1, j, pos;
    device_list_t nvmelists;
    eScanError scanError = DeviceUtil::ScanDriveBySerial(driveList, true);
    if(SCAN_ERROR_NONE == scanError)
    {
        cout << "List of SMART devices: " << endl;

        tDriveList::const_iterator iter;

        for (iter = driveList.begin(); iter != driveList.end(); iter++, i++)
        {
            const sDriveInfo& driveInfo = *iter;
            const sIdentifyInfo& idInfo = driveInfo.IdentifyInfo;

            // check Virtium firmware
            bool virtiumFw = false;
            Device smartDevice;
            if(true == smartDevice.initDevice(driveInfo.IdentifyInfo.DriveName))
            {
                if (true == smartDevice.Device::readSmartData())
                {
                    virtiumFw = isSupportSelfSmartLog(smartDevice.getBufferPtr());
                }
            }

            cout
                << FRMT_U32(2, i) << ". "
                << "Path: " << FRMT_STR(16, idInfo.DriveName) << " "
                << "Model: " << idInfo.DeviceModel
                << ((true == virtiumFw) ? "    * This SSD supports vtView Prime. Use \"vtSmartCmd -ph\" to list vtView Prime usage." : "")
                << endl;
        }
    }
    // else cout << "No device found!" << endl;
    // scan nvme device
    nvmelists = NvmeData::GetNvmeListDevice();
    if(0 != nvmelists.cnt)
    {
        for(j = 0; j < nvmelists.cnt; j++)
        {
            pos = sizeof(nvmelists.lst[j].ctrl.mn) - 1;
            nvmelists.lst[j].ctrl.mn[pos] = '\0';
            cout
                << FRMT_U32(2, i + j) << ". "
                << "Path: " << FRMT_STR(16, nvmelists.lst[j].node) << " "
                << "Model: " << nvmelists.lst[j].ctrl.mn
                << endl;
        }
    }
    
    if(1 == i)  cout << "No device found!" << endl;
}

void SmartCmd::printConfig(const string &fileName, bool dMode)
{
    ofstream fstr;
    fstr.open(fileName.c_str(), ios::out);

    if (!fstr.is_open())
    {
        cerr << "[Error] Cannot open file " << fileName << " for writting." << endl;
    }
    else
    {
        cout << "Generated sample config file successfully (" << fileName << ")." << endl;
        cout << "Please open " << fileName << " with a text editor to set recording condition." << endl;
        fstr << AppConfigFile::GenerateSampleFileString(dMode);
    }

    fstr.close();
}

void SmartCmd::printError(eScanError scanError, bool verboseMode)
{
    switch (scanError)
    {
    case SCAN_ERROR_NODRIVE:   cerr << "[Error] Invalid device names" << endl; break;
    case SCAN_ERROR_SKIPDRIVE: cerr << "[Error] Cannot read information from devices" << endl; break;
    default: break;
    }
}

void SmartCmd::printWarning(const set<string> &hiddenSet, bool verboseMode)
{
    if (0 != hiddenSet.size())
    {
        stringstream sstr;
        set<string>::const_iterator iter;
        for (iter = hiddenSet.begin(); iter != hiddenSet.end(); iter++) sstr << *iter << " ";
        cerr << "[Warning] Missing drive(s) " << sstr.str() << endl;
    }
}

int SmartCmd::execTool()
{
    syncConfig();
    scanDrive();

    return 0;
}

void SmartCmd::syncConfig()
{
    // get information from m_Config and save to confptr;

    sAppData* dataptr = GetAppDataPtr();
    sAppConfig* confptr = GetAppConfigPtr();

    dataptr->HistoryFile = m_Config.m_LogFile;
    dataptr->UpdateTime = m_Config.m_UpdateTime;
    dataptr->UpdateCycle = m_Config.m_UpdateCycle;
    dataptr->RawOutput = m_Config.m_RawOutput;
    dataptr->VerboseMode = m_Config.m_VerboseMode;
    dataptr->SerialSet.insert(m_Config.m_SerialSet.begin(), m_Config.m_SerialSet.end());
    dataptr->DeviceSet.insert(m_Config.m_DeviceSet.begin(), m_Config.m_DeviceSet.end());

    stringstream sstr;
    sstr << m_AppName << " is running ... " << endl;
    sstr << dataptr->toVerboseString() << endl;
    if (false == dataptr->VerboseMode)
    {
        sstr << "To see more detail, set VERBOSE_MODE = TRUE in config file" << endl;
    }

    cout << sstr.str();

    // cout << "Program Config: " << endl << dataptr->toString() << endl;
}

void SmartCmd::updateRawState()
{
    sAppData* dataptr = GetAppDataPtr();

    // Refresh system using DeviceSet
    eScanError scanError = DeviceUtil::ScanDriveByPath(dataptr->RawLiveInfo, true, dataptr->DeviceSet);

    if(SCAN_ERROR_NONE != scanError)
    {
        printError(scanError, dataptr->VerboseMode);
        cout << endl;
        exit(1); // No device found. Exit program
    }

    set<string> hiddenSet;
    if (true == dataptr->getHiddenDevice(hiddenSet)) printWarning(hiddenSet, dataptr->VerboseMode);

    // Save drive info
    AppendHistory(dataptr->RawLiveInfo, dataptr->HistoryFile);
}

void SmartCmd::updateDriveState()
{
    sAppData* dataptr = GetAppDataPtr();
    string devpath;

    for (set<string>::const_iterator iter = dataptr->DeviceSet.begin(); iter != dataptr->DeviceSet.end(); iter++)
    {
        devpath  = *iter;
        if(isNvmeDevice(devpath))
            updateNvmeSate(devpath);
        else
            updateSATAState(devpath);
    }
}

void SmartCmd::updateSATAState(const string &devpath)
{
    sAppData* dataptr = GetAppDataPtr();
    // Refresh system using SerialSet
    // eScanError scanError = DeviceUtil::ScanDriveBySerial(dataptr->LiveInfo, true, dataptr->SerialSet);

    // Refresh system using DeviceSet
    set<string> dev;

    dev.clear();
    dev.insert(devpath);
    eScanError scanError = DeviceUtil::ScanDriveByPath(dataptr->LiveInfo, true, dev);

    if(SCAN_ERROR_NONE != scanError)
    {
        printError(scanError, dataptr->VerboseMode);
        cout << endl;
        exit(1); // No device found. Exit program
    }

    // set<string> hiddenSet;
    // if (true == dataptr->getHiddenDevice(hiddenSet)) printWarning(hiddenSet, dataptr->VerboseMode);

    // Save drive info
    AppendHistory(dataptr->LiveInfo, dataptr->HistoryFile);
}

void SmartCmd::updateNvmeSate(const string &devpath)
{
    sAppData* dataptr = GetAppDataPtr();
    NvmeDevice devinfo;

    devinfo = NvmeData::NvmeDiviceScan(devpath);
    if(0 != devinfo.err)
    {
        cout << "[Error] Cannot read information from: " << devpath << endl;
        exit(1);
    }
    // Save drive info
    AppendHistory(devinfo, dataptr->HistoryFile);
}

void SmartCmd::updateDriverHeader()
{
    device_list_t lists;
    sHeaderLogData head;
    string devpath;
    string temp;
    U32 curtime;
    U32 i;
    sAppData* dataptr = GetAppDataPtr();

    lists = NvmeData::GetNvmeListDevice();
    // if(0 == lists.cnt)
    //     return;

    head.testname = m_TestName;

    for(i = 0; i < 4; i++)
    {
        head.device[i].clear();
    }

    for (set<string>::const_iterator iter = dataptr->DeviceSet.begin(); iter != dataptr->DeviceSet.end(); iter++)
    {
        NvmeIdenStr iden;
        firmware_slot_t fw;
        Device dev;
        sIdentifyInfo driveInfo;
        devpath  = *iter;
        if(isNvmeDevice(devpath))
        {
            iden = NvmeData::GetIdentifyInfo(devpath, lists);
            if(iden.mn.empty())
            {
                cout << "[Error] Cannot read identify information from devices" << endl;
                exit(1);
            }
            fw = NvmeData::GetNvmeFwSlot(devpath);
            if(0 != fw.err)
            {
                cout << "[Error] Cannot read firmware slot information from devices" << endl;
                exit(1);
            }

            // push port to list
            head.device[1].push_back(iden.path);
            // push serial number to list
            head.device[2].push_back(iden.sn);
            // push model to list
            head.device[0].push_back(iden.mn);
            // push type to list
            head.device[3].push_back("NVMe");
            // push identify to list
            head.device[4].push_back(iden.hexbuf);
            // push firmware slot to list
            head.device[5].push_back(HexFormatter::BuffToHexString((U8 *)&fw.data, 512, false));
        }
        else
        {
            if (false == dev.initDevice(devpath))
            {
                cout << "[Error] Cannot read device " << devpath << endl;
                exit(1);
            }
            if (false == dev.readIdentify(driveInfo))
            {
                cout << "[Error] Cannot read identify information on device " << devpath << endl;
                exit(1);
            }

            // push port to list
            head.device[1].push_back(driveInfo.DriveName);
            // push serial number to list
            head.device[2].push_back(driveInfo.SerialNumber);
            // push model to list
            head.device[0].push_back(driveInfo.DeviceModel);
            // push type to list
            head.device[3].push_back("SATA");
            // push identify to list
            head.device[4].push_back(HexFormatter::BuffToHexString(dev.getBufferPtr(), 512, false));
            // push firmware slot to list
            head.device[5].push_back("");
        }
    }

    head.timestamp = time(0);

    AppendHeader(head, dataptr->HistoryFile);
    free(lists.lst);
}

void SmartCmd::scanDrive()
{
    sAppData* dataptr = GetAppDataPtr();

    // General checking:
    bool validHistory = dataptr->isValidHistory();
    if (false == validHistory) return;

    U32 updateCycleInSec = dataptr->UpdateCycle;
    U32 updateTimeInSec = dataptr->UpdateTime;

    U32 startTime = time(0);
    U32 endTime = startTime + updateTimeInSec;
    U32 intervalTime = updateCycleInSec;

    U32 recordCount = 0;

    // updateNvmeHeader();
    updateDriverHeader();

    while(1)
    {
        U32 currentTime = time(0);
        recordCount++;
        if (true == dataptr->RawOutput) updateRawState();
        else                            updateDriveState();

        if (true == dataptr->VerboseMode)
        {
            cout << "[Last Update] " << MiscUtil::FormatTime("%s", "%Y-%m-%d %X", currentTime) << " ";
            cout << "[Record] " << FRMT_LEFT(4, ' ', recordCount) << " ";
            cout << endl;
        }

        // Checking for enable sleeping:
        if (0 != updateTimeInSec)
        {
            if (currentTime >= endTime) break;

            U32 remainingTime = endTime - currentTime;
            intervalTime = MIN2(intervalTime, remainingTime);
        }

        // Goes sleep for some secs
        SystemUtil::SystemSleep(intervalTime);
    }

    cout << endl;
}

void SmartCmd::saveVirtiumPrimeLog(const string& driveName)
{
    Device dev;

    if (false == dev.initDevice(driveName))
    {
        cerr << "[Error] Cannot open device " << driveName << endl << endl;
        return;
    }

    // Build drive info
    sDriveInfo driveInfo;
    if (false == dev.readIdentify(driveInfo.IdentifyInfo))
    {
        cerr << "[Error] Cannot read identify information on device " << driveName << endl << endl;
        return;
    }

    if (false == dev.Device::readSmartData())
    {
        cerr << "[Error] Cannot read SMART information on device " << driveName << endl << endl;
        return;
    }

    U8 totalSector         = dev.getBufferPtr()[401];// 133;
    U8 sectionSizeInSector = dev.getBufferPtr()[402];// 32;

    if (false == isSupportSelfSmartLog(dev.getBufferPtr()))
    {
        cerr << "[Error] --get-vtview-prime-log is not supported on this SSD" << endl << endl;
        return;
    }

    // read SMART data to get some field worst, threshold ... SMART log only transfer raw data
    sSmartInfo refSmartInfo;
    if (false == dev.readSmart(refSmartInfo))
    {
        cerr << "[Error] Cannot read SMART information on device " << driveName << endl << endl;
        return;
    }

    U8* pSmartLogBuffer = new U8[totalSector*512];
    U8 numberOfSection = (totalSector + (sectionSizeInSector - 1)) / sectionSizeInSector;

    U8 totalReadSector = 0;
    for(U8 nSection = 0; nSection < numberOfSection; ++ nSection)
    {
        U32 lba = (nSection << 24) + 0xA0;
        U8 sectorCount;

        sectorCount = totalSector - totalReadSector;
        if(sectorCount > sectionSizeInSector) sectorCount = sectionSizeInSector;

        if(false == dev.executeCommand(lba, sectorCount, ORG_ATA_SMART_READ_LOG))
        {
            cerr << "[Error] Cannot execute command Read SMART Log on device " << driveName << endl << endl;
            delete pSmartLogBuffer;
            return;
        }

        memcpy(&pSmartLogBuffer[totalReadSector*512], dev.getBufferPtr(), sectorCount*512);
        totalReadSector += sectorCount;
    }

    list<sDriveInfo> listDriveInfo;
    list<sDriveInfo>::iterator insertPos;

    U32 lastIndex = 0;
    bool insertMode = false;
    U8* pSmartEntry = pSmartLogBuffer;

    for(U16 nEntry = 0; nEntry < sVirtiumSmartLog::NUMBER_OF_ENTRY; ++ nEntry)
    {
        sVirtiumSmartLogEntryUnmapped smartLog;
        pSmartEntry = convertSmartLogEntry(pSmartEntry, smartLog);

        if(0 == smartLog.Index) continue;

        if((false == insertMode) && (smartLog.Index < lastIndex))
        {
            insertMode = true;
            insertPos = listDriveInfo.begin();
        }

        lastIndex = smartLog.Index;

        driveInfo.SmartInfo.reset();
        driveInfo.CaptureTime = smartLog.TimeStamp;

        for(U8 nId = 0; nId < 30; ++ nId)
        {
            U8 smartID = smartLog.SmartData[nId].SmartId;
            sAttribute smartAtt = refSmartInfo.AttrMap[smartID];

            ASSERT(smartAtt.ID == smartLog.SmartData[nId].SmartId);
            if(0 == smartID) continue;

            smartAtt.Value   = smartLog.SmartData[nId].CurrentValue;
            smartAtt.LowRaw  = smartLog.SmartData[nId].PrimaryRawData;

            if(false == CoreUtil::LookupAttributeName(smartAtt.ID, smartAtt.Name))
            {
                smartAtt.Name = "Reserved_Attribute";
            }

            driveInfo.SmartInfo.AttrMap[smartID] = smartAtt;
        }

        if(true == insertMode)
        {
            listDriveInfo.insert(insertPos, 1, driveInfo);
        }
        else
        {
            listDriveInfo.push_back(driveInfo);
        }
    }

    delete pSmartLogBuffer;

    if(0 == listDriveInfo.size())
    {
        cout << "There is no vtView Prime log at this time" << endl << endl;
        return;
    }

    // adjust time
    time_t captureTime = time(NULL);
    list<sDriveInfo>::iterator smartSample = listDriveInfo.end();
    U32 totalTime = 0;

    do
    {
        -- smartSample;
        U16 timeStamp = smartSample->CaptureTime;
        smartSample->CaptureTime = captureTime - totalTime;
        totalTime += timeStamp * 60;
    }while(smartSample != listDriveInfo.begin());

    time_t rawtime;
    time (&rawtime);

    struct tm * timeinfo;
    timeinfo = localtime (&rawtime);

    stringstream smartLogFile;
    smartLogFile << "vtViewPrimeLog_";
    smartLogFile << (1900 + timeinfo->tm_year);
    smartLogFile << setfill('0') << setw(2) << (timeinfo->tm_mon + 1);
    smartLogFile << setfill('0') << setw(2) << timeinfo->tm_mday;
    smartLogFile << "_";
    smartLogFile << setfill('0') << setw(2) << timeinfo->tm_hour;
    smartLogFile << setfill('0') << setw(2) << timeinfo->tm_min;
    smartLogFile << setfill('0') << setw(2) << timeinfo->tm_sec;
    smartLogFile << ".txt";

    cout << "vtView Prime log is saved to file " << smartLogFile.str();
    if(true == AppendHistory(listDriveInfo, smartLogFile.str()))
    {
        cout << " successfully";
    }
    else
    {
        cout << " fail";
    }

    cout << endl << endl;
}

void SmartCmd::setVirtiumFwSamplingRateSmartLog(const string& driveName, const char* sampleRate)
{
    Device dev;

    if (false == dev.initDevice(driveName))
    {
        cerr << "[Error] Cannot open device " << driveName << endl << endl;
        return;
    }

    if (false == dev.Device::readSmartData())
    {
        cerr << "[Error] Cannot read SMART information on device " << driveName << endl << endl;
        return;
    }

    if (false == isSupportSelfSmartLog(dev.getBufferPtr()))
    {
        cerr << "[Error] --set-sampling-rate is not supported on this SSD" << endl << endl;
        return;
    }

    if(false == StringUtil::IsDecimal(sampleRate))
    {
        cerr << "[Error] Parameter \"" << sampleRate << "\" isn't decimal number" << endl << endl;
        return;
    }

    U32 minutes = strtoul(sampleRate, NULL, 10);

    if(0 != minutes)
    {
        if((minutes < MIN_SAMPLING_RATE) || (minutes > MAX_SAMPLING_RATE))
        {
            cerr << "Set sampling rate failed" << endl;
            cerr << "  * Minimum = " << MIN_SAMPLING_RATE <<", Maximum = " << MAX_SAMPLING_RATE << endl;
            cerr << "  * Set 0 to disable" << endl;
            cerr << endl;
            return;
        }
    }

    if( false == dev.executeCommand(minutes, 0, ORG_VSC_VRT_SET_SAMPLING_RATE))
    {
        cerr << "[Error] Cannot set sampling rate to device " << driveName << endl << endl;
        return;
    }

    cout << "Set sampling rate to ";

    if(minutes > 1) cout << minutes << " minutes";
    else cout << minutes << " minute";

    cout << " successfully";
    if(0 == minutes)
    {
        cout << endl << "vtView Prime is disabled";
    }
    cout << endl << endl;
}

void SmartCmd::getVirtiumFwSamplingRateInfo(const string& driveName)
{
    Device dev;

    if (false == dev.initDevice(driveName))
    {
        cerr << "[Error] Cannot open device " << driveName << endl << endl;
        return;
    }

    if (false == dev.Device::readSmartData())
    {
        cerr << "[Error] Cannot read SMART information on device " << driveName << endl << endl;
        return;
    }

    if (false == isSupportSelfSmartLog(dev.getBufferPtr()))
    {
        cerr << "[Error] --get-sampling-rate-info is not supported on this SSD" << endl << endl;
        return;
    }

    U16 sampleRate;

    if( false == getSmartLogSampleRate(dev, sampleRate))
    {
        cerr << "[Error] Cannot retrieve sampling rate on device " << driveName << endl << endl;
        return;
    }

    if(0 == sampleRate)
    {
        cout <<         "vtView Prime = disabled";
        cout << endl << "Sampling Rate = 0 minute";
        cout << endl << "To enable vtView Prime, set Sampling Rate to value between " << MIN_SAMPLING_RATE << " to " << MAX_SAMPLING_RATE << " minutes";
    }
    else
    {
        cout <<         "vtView Prime = enabled";
        cout << endl << "Sampling Rate = " << sampleRate << " minutes";
        cout << endl << "To disable vtView Prime, set Sampling Rate = 0";
    }

    cout << endl << endl;
}

bool SmartCmd::getSmartLogSampleRate(Device& smartDev, U16& sampleRate)
{
    if (false == smartDev.readSmartData()) return false;

    U8* pBuffer = smartDev.getBufferPtr();
    sampleRate = (U16)((pBuffer[396] << 8) + pBuffer[395]);

    return true;
}

bool SmartCmd::isSupportSelfSmartLog(U8* pSmartData)
{
    const char signature[] = "Grizz";

    return (0 == memcmp(&pSmartData[0x182], signature, 5));
}

U8* SmartCmd::convertSmartLogEntry(const U8* pBuffer, sVirtiumSmartLogEntryUnmapped& entry)
{
    entry.Index = ((pBuffer[3] << 24) | (pBuffer[2] << 16) | (pBuffer[1] << 8) | pBuffer[0]);
    pBuffer += 4;

    entry.TimeStamp = ((pBuffer[1] << 8) | pBuffer[0]);
    pBuffer += 2;

    for(U8 nSmart = 0; nSmart < 30; ++ nSmart)
    {
        entry.SmartData[nSmart].SmartId          = pBuffer[0];
        pBuffer += 1;

        entry.SmartData[nSmart].CurrentValue     = pBuffer[0];
        pBuffer += 1;

        entry.SmartData[nSmart].PrimaryRawData   = ((pBuffer[3] << 24) | (pBuffer[2] << 16) | (pBuffer[1] << 8) | pBuffer[0]);
        pBuffer += 4;
    }

    return (U8*)pBuffer;
}

void SmartCmd::printPrimeHelp()
{
    stringstream hstr;

    hstr << "vtView Prime includes all features of vtView 1.0 and the following features:" << endl;
    hstr << "  * Self-record vtView log" << endl;
    hstr << "  * Workload statistics (access type, access size, access distribution, aligned/non-aligned accesses, etc.)" << endl;
    hstr << "  * Enhanced Smart attributes for failure analysis" << endl;
    hstr << "  * Visit vtView.virtium.com for full list of features and use cases" << endl;

    hstr << endl;
    hstr << "vtView Prime commands:" << endl;
    hstr << "  --get-vtview-prime-log          dev  Retrieve vtView Prime log" << endl;
    hstr << "  --get-workload-stat             dev  Retrieve vtView Prime workload statistics" << endl;
    hstr << "  --get-sampling-rate-info        dev  Get current vtView Prime sampling rate information" << endl;
    hstr << "  --set-sampling-rate       rate  dev  Set vtView Prime sampling rate in minute" << endl;
    hstr << "                                         * Minimum = " << MIN_SAMPLING_RATE <<", Maximum = " << MAX_SAMPLING_RATE << endl;
    hstr << "                                         * Set 0 to disable" << endl;
    hstr << endl;
    cout << hstr.str();
}

void SmartCmd::saveWorkloadLog(const string& driveName)
{
    string functionTitle = "Save workload statistics";

    Device dev;

    if (false == dev.initDevice(driveName))
    {
        cerr << "[Error] Cannot open device " << driveName << endl << endl;
        return;
    }

    tWorkLoadList         workLoadList;
    sWorkLoadLogTableInfo workLoadInfo;
    VscVirtiumFunction    vscExec(&dev);

    cout << "Reading workload statistics ...";

    bool readLogStatus = vscExec.getWorkLoadLog(workLoadList, workLoadInfo, &consoleProgressPercent);

    if(false == readLogStatus)
    {
        cerr << endl << endl;
        cerr << "[Error] Read workload statistics fail on device " << driveName << endl << endl;
        return;
    }

    cout << endl << endl << "Saving workload statistics ..." << endl;

    string fileName;
    if(false == makeFileName(&dev, "WS_Debug", "", "bin", fileName))
    {
        cerr << "[Error] Create file name" << endl << endl;
        return;
    }

    do
    {
        ofstream saveFile;
        saveFile.open(fileName.data(), (ios_base::binary | ios_base::out | ios_base::trunc));
        if(false == saveFile.is_open())
        {
            cerr << "[Error] Open file " << fileName << endl << endl;
            break;
        }

        // parameter for header
        U64 workLoadStatistic[4];
        workLoadStatistic[0] = workLoadInfo.NumberOfRandomReadCommands;
        workLoadStatistic[1] = workLoadInfo.NumberOfRandomWriteCommands;
        workLoadStatistic[2] = workLoadInfo.NumberOfSequentialReadCommands;
        workLoadStatistic[3] = workLoadInfo.NumberOfSequentialWriteCommands;

        // Reference this array in XOject <Object Name="TransferSizeStatisticInKB" Type="string" Serialize="Package">
        #define HARD_CODE_TransferSizeStatisticInKBConfligListCount 12
        U32 transferSizeStatisticInKBConfligListCount = HARD_CODE_TransferSizeStatisticInKBConfligListCount;
        double transferSizeList[HARD_CODE_TransferSizeStatisticInKBConfligListCount] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0};

        U32 workLoadItemCount = workLoadList.size();

        // save header
        saveFile.write((const char*)workLoadStatistic, sizeof(workLoadStatistic));
        saveFile.write((const char*)&transferSizeStatisticInKBConfligListCount, sizeof(transferSizeStatisticInKBConfligListCount));
        saveFile.write((const char*)transferSizeList, sizeof(transferSizeList));
        saveFile.write((const char*)&workLoadItemCount, sizeof(workLoadItemCount));

        // save data
        for (tWorkLoadList::iterator workLoadItem = workLoadList.begin(); workLoadItem != workLoadList.end(); workLoadItem ++)
        {
            sWorkLoadLogItem* pWorkLoadItem = &(*workLoadItem);

            saveFile.write((const char*)pWorkLoadItem, sizeof(sWorkLoadLogItem));
        }

        saveFile.close();

        cout << endl << functionTitle << " to file " << fileName << " successfully";
        cout << endl << "Please send " << fileName << " to Virtium FAE for analysis";
        cout << endl << endl;

    }while(0);
}

bool SmartCmd::makeFileName(Device* pSmiDev, string prefix, string posfix, string extention, string& fileName)
{
    if(false == pSmiDev->readIdentify())
    {
        return false;
    }

    fileName = prefix + "_";
    U8* idBuffer = pSmiDev->getBufferPtr();
    if (pSmiDev->getCommandType() != CMDTYPE_SCSI)
    {
        sIdentifyInfo driveInfo;
        DeviceUtil::ParseIdentifyInfo(idBuffer, pSmiDev->getDriveName(), driveInfo);
        fileName += driveInfo.SerialNumber;
    }
    else
    {
        sInquiryInfo driveInfo;
        DeviceUtil::ParseInquiryInfo(idBuffer, driveInfo);
        fileName += driveInfo.SerialNumber;
    }

    fileName += posfix;
    if(0 != extention.length())
    {
        fileName += "." + extention;
    }

    return true;
}

bool SmartCmd::isNvmeDevice(const string &driveName)
{
    string check = driveName.substr(0,8);

    if(0 == check.compare("/dev/nvm"))
        return true;

    return false;
}
