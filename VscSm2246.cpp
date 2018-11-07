#include "utility/misc/MiscUtil.h"
#include "utility/misc/StringUtil.h"
#include "utility/misc/HexFormatter.h"
#include "utility/device/DeviceManager.h"
#include <bitset>
#include <sys/stat.h>

#include "VscSm2246.h"

const VscSm2246::sOption VscSm2246::m_BuildInOption[ VscSm2246::OPTION_TOTAL ] =
    {
        {OPTION_NULL,
            "",
            "",
            0,
            "",
            ""
        },

        {OPTION_LICENSE,
            "-l",
            "",
            0,
            "Print license",
            ""
        },

        {OPTION_HELP,
            "-h",
            "",
            0,
            "Print this help",
            ""
        },

        {OPTION_SCAN,
            "-s",
            "",
            0,
            "Scan devices",
            ""
        },

        {OPTION_PRINT,
            "-p",
            "device_name",
            1,
            "Print device information to file",
            "DeviceInfo"
        },

        {OPTION_READ_MARKED_BAD_BLOCK,
            "-mbad",
            "device_name",
            1,
            "Read marked bad block",
            "MarkedBadBlock"
        },

        {OPTION_READ_ORIGINAL_BAD_BLOCK,
            "-obad",
            "device_name",
            1,
            "Read original bad block",
            "OriginalBadBlock"
        },

        {OPTION_READ_TOTAL_BAD_BLOCK,
            "-tbad",
            "device_name",
            1,
            "Read total bad block",
            "TotalBadBlock"
        },

        {OPTION_ERASE_COUNT,
            "-ecount",
            "device_name",
            1,
            "Read erase count",
            "EraseCount"
        },

        {OPTION_L2P_MAPPING,
            "-l2p",
            "start_index  count  device_name",
            3,
            "Convert LBA to physical",
            "LbaToPhysical"
        },

        {OPTION_READ_LOG,
            "-log",
            "start_index  count  device_name    \n"
            "last         count  device_name    \n"
            "all                 device_name    Read all could take up to a few hours.",
            2,
            "Read internal log",
            "Log"
        },

        {OPTION_PARSE_LOG,
            "-plog",
            "1. \"vtFA -log ...\" to retrieve log from SSD\n"
            "2. \"vtFA -plog Log.bin password\" to display events",
            2,
            "Retrieve and display event log from binary log file",
            ""
        },

        {OPTION_READ_CARD_INFO,
            "-card",
            "device_name",
            1,
            "Read card info",
            "CardInfo"
        },

        {OPTION_READ_CID,
            "-cid",
            "device_name",
            1,
            "Read CID",
            "CID"
        },

        {OPTION_READ_RUNNING_SPARE,
            "-rsc",
            "device_name",
            1,
            "Display running spare count",
            "RunningSpareCount"
        },
    };


const string VscSm2246::virPassword = "virtiumeyesonly";


VscSm2246::VscSm2246()
{
    reset();
}

VscSm2246::~VscSm2246()
{
    DeviceManager::DeleteDeviceList(m_DevList);
}

void VscSm2246::reset()
{
    m_AppName = "";
}

bool VscSm2246::setToolInfo(int argc, char* argv[])
{
    reset();

    scanDevice();

    m_AppName = argv[0];
    size_t nPos = m_AppName.find_last_of('\\');
    if(nPos < m_AppName.length() )
    {
        m_AppName.erase(0, nPos + 1);
    }

    if (argc < 2)
    {
        printHelp(0, OPTION_TOTAL);
        return true;
    }

    // search option
    eOption optionCode = OPTION_NULL;
    const sOption *pSelectedOption = NULL;
    for(U8 nSelectedOption = 0; nSelectedOption < OPTION_TOTAL; nSelectedOption ++)
    {
        if(0 == m_BuildInOption[nSelectedOption].Name.compare(argv[1]))
        {
            if(argc >= (m_BuildInOption[nSelectedOption].MinimumRequireParameter + 2))
            {
                pSelectedOption = &m_BuildInOption[nSelectedOption];
                optionCode = m_BuildInOption[nSelectedOption].Id;
                break;
            }
            else
            {
                cout << endl << m_BuildInOption[nSelectedOption].Title << " failed." << endl;
                cout << endl << "ERROR: Missed parameter." << endl;
                printHelp(nSelectedOption, 1);
                return true;
            }
        }
    }

    // Get device name base on option
    bool needOpenDevice = true;
    string devName;
    switch( optionCode )
    {
        case OPTION_LICENSE:
        case OPTION_NULL:
        case OPTION_HELP:
        case OPTION_SCAN:
        case OPTION_PARSE_LOG:
        {
            needOpenDevice = false;
        }break;

        case OPTION_PRINT:
        {
            devName = argv[2];
        }break;

        case OPTION_READ_MARKED_BAD_BLOCK:
        {
            devName = argv[2];
        }break;

        case OPTION_READ_ORIGINAL_BAD_BLOCK:
        case OPTION_READ_TOTAL_BAD_BLOCK:
        {
            devName = argv[2];
        }break;

        case OPTION_ERASE_COUNT:
        {
            devName = argv[2];
        }break;

        case OPTION_L2P_MAPPING:
        {
            devName = argv[4];
        }break;

        case OPTION_READ_LOG:
        {
            if(5 == argc) devName = argv[4];
            else devName = argv[3];
        }break;

        case OPTION_READ_CARD_INFO:
        {
            devName = argv[2];
        }break;

        case OPTION_READ_CID:
        {
            devName = argv[2];
        }break;

        case OPTION_READ_RUNNING_SPARE:
        {
            devName = argv[2];
        }break;

        default:
        break;
    }

    // open device
    BaseDevice* pSmiDev = NULL;
    if(true == needOpenDevice)
    {
        if(0 == devName.length())
        {
            cout << endl << pSelectedOption->Title << " failed." << endl;
            cout << endl << "ERROR: Missed device name." << endl << endl;
            return true;
        }

        pSmiDev = DeviceManager::SearchDevice(devName, m_DevList);
        if(NULL == pSmiDev)
        {
            cout << endl << pSelectedOption->Title << " failed." << endl;
            cout << endl << "ERROR: Invalid device name " << devName << endl << endl;
            return true;
        }
    }

    // response command
    switch( optionCode )
    {
        case OPTION_LICENSE:
        {
            printLicense();
        }break;

        case OPTION_NULL:
        case OPTION_HELP:
        {
            printHelp(0, OPTION_TOTAL);
        }break;

        case OPTION_SCAN:
        {
            printDevice();
        }break;

        case OPTION_PRINT:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "txt");
            printDevice(pSelectedOption, pSmiDev, fileName);
        }break;

        case OPTION_READ_MARKED_BAD_BLOCK:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "csv");
            readMarkedBadBlock(pSelectedOption, pSmiDev, fileName);
        }break;

        case OPTION_READ_ORIGINAL_BAD_BLOCK:
        case OPTION_READ_TOTAL_BAD_BLOCK:
        {
            bool readOriginalBad = (OPTION_READ_ORIGINAL_BAD_BLOCK == optionCode);

            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "csv");
            readBadBlockTable(pSelectedOption, pSmiDev, fileName, readOriginalBad);
        }break;

        case OPTION_ERASE_COUNT:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "csv");
            readEraseCountTable(pSelectedOption, pSmiDev, fileName);
        }break;

        case OPTION_L2P_MAPPING:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "csv");
            l2PMapping(pSelectedOption,
                       pSmiDev,
                       argv[2],
                       argv[3],
                       fileName);
        }break;

        case OPTION_READ_LOG:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "");
            readLog(pSelectedOption, pSmiDev, argv[2], argv[3], fileName);
        }break;

        case OPTION_PARSE_LOG:
        {
            parseLog(pSelectedOption, argv[2], argv[3]);
        }break;

        case OPTION_READ_CARD_INFO:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "bin");
            readParameter(pSelectedOption, pSmiDev, fileName);
        }break;

        case OPTION_READ_CID:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "bin");
            readCID(pSelectedOption, pSmiDev, fileName);
        }break;

        case OPTION_READ_RUNNING_SPARE:
        {
            string fileName = makeFileName(pSmiDev, pSelectedOption->FileNamePrefix, "", "txt");
            readRunningSpareCount(pSelectedOption, pSmiDev, fileName);
        }break;

        default:
            printHelp(0, OPTION_TOTAL);
    }

    return true;
}

int VscSm2246::execTool()
{
    cout << endl << endl;
    return 0;
}

void VscSm2246::readLog(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string strStartIndex, string strLogCount, string fileName)
{
    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    Sm2246::eCommandStatus cmdStatus;

    Sm2246::sFlashID flashID;
    cmdStatus = smiVsc.readFlashID(&flashID);
    if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
    {
        cout << endl << pOption->Title << " failed.";
        cout << endl << endl << "ERROR: Read Flash ID error " << Sm2246::GetErrorString(cmdStatus);
        return;
    }

    U32 totalLog = (0 == flashID.TotalLog) ? Sm2246::MAX_INTERNAL_LOG : flashID.TotalLog;

    U32 startIndex;
    U32 logCount;

    string originFileName = fileName;
    stringstream fileIndex;

    if(0 == strStartIndex.compare("last"))
    {
        if(false == StringUtil::IsDecimal(strLogCount))
        {
            cout << endl << endl << pOption->Title << " failed.";
            cout << endl << endl << "ERROR: Parameter log count = " << strLogCount <<" isn't decimal number.";
            return;
        }

        logCount = strtoull(strLogCount.c_str(), 0, 10);
        if(totalLog < logCount)
        {
            cout << endl << endl << "Reduced user input log count " << logCount << " to total drive's logs " << totalLog << ".";
            logCount = totalLog;
        }

        startIndex = totalLog - 1;

        fileIndex << "_last_" << logCount;
    }
    else if(0 == strStartIndex.compare("all"))
    {
        logCount   = totalLog;
        startIndex = totalLog - 1;

        fileIndex << "_all_" << logCount;
    }
    else if(true == StringUtil::IsDecimal(strStartIndex))
    {
        if(false == StringUtil::IsDecimal(strLogCount))
        {
            cout << endl << pOption->Title << " failed.";
            cout << endl << endl << "ERROR: Parameter log count = " << strLogCount <<" isn't decimal number.";
            return;
        }

        logCount   = strtoull(strLogCount.c_str(), 0, 10);
        startIndex = strtoull(strStartIndex.c_str(), 0, 10);

        if(startIndex >= totalLog)
        {
            cout << endl << endl << "Start index has to less than total drive's log " << totalLog;
            return;
        }

        U32 checkLogCount = totalLog - startIndex;
        if(logCount > checkLogCount)
        {
            cout << endl << "Reduced user input total log " << logCount << " from start index " << startIndex << " to " << checkLogCount << ". Total drive's logs is " << totalLog << "." << endl;
            logCount = checkLogCount;
        }

        fileIndex << "_index_" << startIndex << "_" << logCount;

        // adjust start index
        startIndex = (startIndex + logCount) - 1;
    }
    else
    {
        cout << endl << pOption->Title << " failed.";
        cout << endl << endl << "ERROR: Wrong parameter.";
        return;
    }

    cout << endl << pOption->Title << "." << endl;

    if(0 == logCount)
    {
        cout << endl << "Parameter log count is 0";
        return;
    }

    // add time stamp
    //time_t rawtime;
    //struct tm * timeinfo;
    //time ( &rawtime );
    //timeinfo = localtime ( &rawtime );

    //fileIndex << "_";
    //fileIndex << (1900 + timeinfo->tm_year) << (timeinfo->tm_mon + 1) << timeinfo->tm_mday;
    //fileIndex << timeinfo->tm_hour << timeinfo->tm_min << timeinfo->tm_sec;

    stringstream fileCounter;
    U32 existCount = 0;

    while(1)
    {
        fileCounter.str("");
        if(0 != existCount) fileCounter << "_[" << existCount << "]";

        fileName = originFileName + fileIndex.str() + fileCounter.str() + ".bin";

        struct stat statBuffer;
        if(stat(fileName.c_str(), &statBuffer) != 0)
        {
            break;
        }

        ++ existCount;
    }

    U16 logIndex = startIndex;
    U32 nLog;
    U32 totalFail = 0;

    U32 percent = getPercent(logCount);

    for(nLog = 0; nLog < logCount; ++ nLog, -- logIndex)
    {
        Sm2246::sLogEntry logEntry;

        cmdStatus = smiVsc.readLogFromDrive(logIndex, logEntry);

        if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
        {
            Sm2246::eCommandStatus saveStatus;
            // Use truncat for first save to make new file. Next save in append mode to add other log in current read log command
            saveStatus = smiVsc.saveLog(Sm2246::SAVE_LOG_BINARY, fileName, logEntry, (0 != nLog));

            if(Sm2246::CMD_STATUS_ERROR_NONE != saveStatus)
            {
                cout << endl << endl << "Save log index " << logIndex << " to file "  << fileName << " failed.";
                cout << endl << endl << "ERROR: " << Sm2246::GetErrorString(saveStatus);
                return;
            }
        }
        else
        {
            cout << endl << "Read log index" << logIndex << " failed.";
            ++ totalFail;
        }

        U32 totalDone = nLog + 1;
        if(    (0 == (totalDone % percent))
            || (totalDone == logCount)
            || (1 == totalDone)
          )
        {
            cout << endl << "Done " << totalDone << " of " << logCount;
        }
    }

    if(0 != totalFail)
    {
        cout << endl << endl << "User requests read " << logCount << " logs. Read fail " << logCount << " logs.";

        U32 failExistCount = 0;

        string failFile;

        while(1)
        {
            stringstream failFileName;

            failFileName << originFileName;
            failFileName << fileIndex.str();
            failFileName << "_Failed_";
            failFileName << totalFail;

            if(0 != failExistCount) failFileName << "_(" << failExistCount << ")";

            failFileName << ".bin";

            struct stat statBuffer;
            if(stat(failFileName.str().c_str(), &statBuffer) != 0)
            {
                failFile = failFileName.str();
                break;
            }

            ++ failExistCount;
        }

        remove(failFile.c_str()); // delete failed file if it existed
        if( 0 != rename(fileName.c_str(), failFile.c_str()) )
        {
            cout << endl << endl << "Rename file " << fileName << " to " << failFile << " failed.";
        }
        else
        {
            cout << endl << endl << "The logs is saved to " << failFile;
            fileName = failFile;
        }
    }
    else
    {
        cout << endl << endl << "Save to file " << fileName << " successfully.";
    }

    cout << endl << endl << "Please send file " << fileName << " to Virtium FAE for analyzing.";
}

void VscSm2246::parseLog(const VscSm2246::sOption* pOption, string logFileName, string password)
{
    cout << endl << "Translating binary log file to event log... ";;

    // check password
    if(0 != password.compare(virPassword))
    {
        cout << " failed.";
        cout << endl << endl << "ERROR: Wrong password.";
        return;
    }

    cout << endl << "It may take a while..." << endl;

    string fname = logFileName;

    size_t dotPos = fname.find_last_of('.', fname.length()-1);
    fname.erase(dotPos, fname.length());

    string cvsFile   = fname + ".csv";
    string txtFile   = fname + ".txt";
    string eventFile = fname + "_Event.txt";

    Sm2246 smiVsc;
    Sm2246::sLogEntry logEntry;
    Sm2246::eCommandStatus cmdStatus;
    U32 totalLog;

    // do first read to get total log
    cmdStatus = smiVsc.readLogFromFile(logFileName, 0, logEntry, totalLog);

    if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
    {
        cout << endl << "Open file" << logFileName << " failed.";
        cout << endl << "ERROR: " << Sm2246::GetErrorString(cmdStatus);
        return;
    }

    if(0 == totalLog)
    {
        cout << endl << "There is no log in file.";
        return;
    }

    cout << endl << "Total log entries = " << totalLog;

    bool appendMode = false;
    U32 percent = 100;//getPercent(totalLog);

    for(U16 logPosition = 0; logPosition < totalLog; ++ logPosition)
    {
        if(0 == (logPosition % percent))
        {
            U32 nextRange;

            nextRange = logPosition + percent;
            if(nextRange > totalLog) nextRange = totalLog;
            cout << endl << "Translating " << (logPosition + 1)<< " to " << nextRange << " ...";
        }

        cmdStatus = smiVsc.readLogFromFile(logFileName, logPosition, logEntry, totalLog);

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            cout << endl << "Read file" << logFileName << "failed.";
            return;
        }

        if(Sm2246::LOG_STATUS_INVALID == logEntry.Header.Field.Status)
        {
            break;
        }

        cmdStatus = smiVsc.saveLog(Sm2246::SAVE_LOG_STATISTIC, cvsFile, logEntry, appendMode);
        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            cout << endl << endl << "Save log to file " << cvsFile << " failed.";
            cout << endl << endl << "ERROR: " << Sm2246::GetErrorString(cmdStatus);
            return;
        }

        cmdStatus = smiVsc.saveLog(Sm2246::SAVE_LOG_VERBOSE, txtFile, logEntry, appendMode);
        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            cout << endl << endl << "Save log to file " << txtFile << " failed.";
            cout << endl << endl << "ERROR: " << Sm2246::GetErrorString(cmdStatus);
            return;
        }

        cmdStatus = smiVsc.saveLog(Sm2246::SAVE_LOG_EVENT, eventFile, logEntry, appendMode);
        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            cout << endl << endl << "Save log to file " << eventFile << " failed.";
            cout << endl << endl << "ERROR: " << Sm2246::GetErrorString(cmdStatus);
            return;
        }

        appendMode = true;
    }

    cout << endl << "Complete.";
    cout << endl << endl << "Save log to file " << cvsFile << " successfully.";
    cout << endl << endl << "Save log to file " << txtFile << " successfully.";
    cout << endl << endl << "Save log to file " << eventFile << " successfully.";
}

void VscSm2246::readBadBlockTable(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName, bool readOriginalBlock)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    U32 totalRecord;
    Sm2246::sBadBlock* pTable;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readBadBlock(&pTable, &totalRecord, readOriginalBlock);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        if(0 != totalRecord)
        {
            strBuffer << " Total records: " << totalRecord;
        }

        cmdStatus = smiVsc.saveBadBlock(fileName, pTable, totalRecord, false);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::readEraseCountTable(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    U16* pTable;
    U16 totalRecord;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readEraseCountTable(&pTable, &totalRecord);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        if(0 != totalRecord)
        {
            strBuffer << " Total records: " << totalRecord;
        }

        cmdStatus = smiVsc.saveEraseCountTable(fileName, pTable, totalRecord, false);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::readMarkedBadBlock(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    Sm2246::uMarkBadInfo* pBadInfo;
    U16 totalRecord;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readMarkBadBlock(&pBadInfo, &totalRecord);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        if(0 != totalRecord)
        {
            strBuffer << " Total records: " << totalRecord;
        }

        cmdStatus = smiVsc.saveMarkBadBlock(fileName, pBadInfo, totalRecord, false);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::l2PMapping(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string strStartLBA, string strLbaCount, string fileName)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    if(false == StringUtil::IsDecimal(strStartLBA))
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << "ERROR: Parameter start lba = " << strStartLBA <<" isn't decimal number.";
        cout << strBuffer.str();
        return;
    }

    if(false == StringUtil::IsDecimal(strLbaCount))
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << "ERROR: Parameter lba count = " << strLbaCount <<" isn't decimal number.";
        cout << strBuffer.str();
        return;
    }

    U64 startLBA = strtoull(strStartLBA.c_str(), 0, 10);
    U32 lbaCount = strtoul(strLbaCount.c_str(), 0, 10);

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    U32 totalRecord;
    Sm2246::sL2PMapping* pMapping;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readH2FTable(startLBA, lbaCount, &pMapping, &totalRecord);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        if(0 != totalRecord)
        {
            strBuffer << " Total records: " << totalRecord;
        }

        cmdStatus = smiVsc.saveH2FTableToFile(fileName, pMapping, totalRecord, true);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::readParameter(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    Sm2246::sParameterTable parameter;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readParameterTable(&parameter);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        cmdStatus = smiVsc.saveParameterTable(fileName, &parameter);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::readCID(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    std::stringstream strBuffer;

    strBuffer << endl << pOption->Title;

    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    Sm2246::sCIDTable CIDTable;
    Sm2246::eCommandStatus cmdStatus;
    cmdStatus = smiVsc.readCIDTable(&CIDTable);

    if(Sm2246::CMD_STATUS_ERROR_NONE == cmdStatus)
    {
        strBuffer << " successfully.";

        cmdStatus = smiVsc.saveCIDTable(fileName, &CIDTable);

        strBuffer << endl << endl << "Save to file " << fileName;

        if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
        {
            strBuffer << " failed. Could be permission.";
        }
        else
        {
            strBuffer << " successfully.";
        }
    }
    else
    {
        strBuffer << " failed.";
        strBuffer << endl << endl << Sm2246::GetErrorString(cmdStatus);
    }

    cout << strBuffer.str();
}

void VscSm2246::readRunningSpareCount(const sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    Sm2246 smiVsc;
    smiVsc.setDevice(pSmiDev);

    Sm2246::eCommandStatus cmdStatus;

    Sm2246::sFlashID flashID;
    cmdStatus = smiVsc.readFlashID(&flashID);

    if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
    {
        cout << endl;
        cout << endl << "Cannot read Running Spare Count via Vendor specific mode.";
        cout << endl << "Please temporary stop read or write to SSD before executing this command.";
        return;
    }

    cout << endl << "Running Spare Count = " << flashID.TotalSpareCount;

#if 0
    cout << endl;
    cmdStatus = smiVsc.saveRunningSpareCount(fileName, &flashID);
    cout << endl << "Save to file " << fileName;

    if(Sm2246::CMD_STATUS_ERROR_NONE != cmdStatus)
    {
        cout << " failed. Could be permission.";
    }
    else
    {
        cout << " successfully.";
    }
#endif
}

void VscSm2246::scanDevice()
{
    DeviceManager::DeleteDeviceList(m_DevList);
    DeviceManager::InitDeviceList(m_DevList);
}

void VscSm2246::printDevice()
{
    U32 devCount = m_DevList.size();
    cout << endl << "List of SMART device: " << endl;

    for (U32 i = 0; i < devCount; i++)
    {
        BaseDevice* devPtr = m_DevList[i];

        // Print identify info
        if(false == devPtr->readIdentify()) continue;

        U8* idBuffer = devPtr->getBufferPtr();
        cout << endl << "## Drive " << FRMT_U32(2, i) << ":" << endl;

        cout << "Name: " << devPtr->m_Name << endl;

        if (devPtr->m_CommandType != CMDTYPE_SCSI)
        {
            sIdentifyInfo driveInfo;
            DeviceUtil::ParseIdentifyInfo(idBuffer, devPtr->m_Name, driveInfo);
            cout << driveInfo.toString() << endl;
        }
        else
        {
            sInquiryInfo driveInfo;
            DeviceUtil::ParseInquiryInfo(idBuffer, driveInfo);
            cout << driveInfo.toString() << endl;
        }
    }
}

void VscSm2246::printDevice(const VscSm2246::sOption* pOption, BaseDevice* pSmiDev, string fileName)
{
    cout << endl << pOption->Title;

    if(false == pSmiDev->readIdentify())
    {
        cout << " failed.";
        cout << endl << endl << "ERROR: Read Identify.";
        return;
    }

    U8* idBuffer = pSmiDev->getBufferPtr();

    std::stringstream strBuffer;
    strBuffer << "Name: " << pSmiDev->m_Name << endl;
    if (pSmiDev->m_CommandType != CMDTYPE_SCSI)
    {
        sIdentifyInfo driveInfo;
        DeviceUtil::ParseIdentifyInfo(idBuffer, pSmiDev->m_Name, driveInfo);
        strBuffer << driveInfo.toString() << endl;
    }
    else
    {
        sInquiryInfo driveInfo;
        DeviceUtil::ParseInquiryInfo(idBuffer, driveInfo);
        strBuffer << driveInfo.toString() << endl;
    }

    ofstream saveFile;
    ios_base::openmode fileMode = ios_base::out | ios_base::trunc;

    saveFile.open(fileName.data(), fileMode);
    if(false == saveFile.is_open())
    {
        cout << " failed.";
        cout << endl << endl << "ERROR: Generate file " << fileName;
    }
    else
    {
        cout << " successfully.";
        cout << endl << endl << "File name: " << fileName;
        saveFile << strBuffer.str();
        saveFile.close();
    }
}

void VscSm2246::printHelp(U8 startIndex, U8 count)
{
    // check over bound
    if((startIndex + count) > OPTION_TOTAL)
    {
        return;
    }

    cout << endl;

    cout << endl << "Usage: ";
    cout << endl << "       vtFA [option]";
    cout << endl;

    cout << endl << "       1. Make sure this command line administrator priviledge.";
    cout << endl << "          - Windows: \"Run as administrator\" ";
    cout << endl << "          - Linux: sudo -s ";
    cout << endl;
    cout << endl << "       2. Scan SSD devices in system, use command vtFA -s ";
    cout << endl;
    cout << endl << "       3. Use information from device, using command vtFA [option]";

    cout << endl;
    cout << endl << "Option: " << endl;

    // search max string len to auto align
    U8 maxOptStrLen = 0;
    for(U8 nOption = 0; nOption < OPTION_TOTAL; nOption ++)
    {
        U16 strLen = m_BuildInOption[nOption].Name.length();
        if(strLen > maxOptStrLen) maxOptStrLen = strLen;
    }
    maxOptStrLen += 4; // plus blank character betweent 2 column

    for(U8 nOption = startIndex; nOption < (startIndex + count); nOption ++)
    {
        if(0 == m_BuildInOption[nOption].Name.length()) continue;

        cout << endl << left << setw(maxOptStrLen) << m_BuildInOption[nOption].Name << m_BuildInOption[nOption].Title <<".";

        // print example
        size_t endPos;
        string paramStr = m_BuildInOption[nOption].Parameter;
        bool printStrExample = true;

        while(paramStr.length() != 0)
        {
            endPos = paramStr.find('\n', 0);

            string subParamStr;

            if(string::npos != endPos)
            {
                subParamStr = paramStr.substr(0, endPos);
                paramStr.erase(0, endPos + 1);
            }
            else
            {
                subParamStr = paramStr;
                paramStr.clear();
            }

            if(subParamStr.length() != 0)
            {
                cout << endl << left << setw(maxOptStrLen) << " ";
                if(true == printStrExample)
                {
                    if(OPTION_PARSE_LOG == m_BuildInOption[nOption].Id)
                    {
                        cout << "  Usage: ";
                    }
                    else
                    {
                        cout << "  Example: ";
                    }

                    printStrExample = false;
                }
                else
                {
                    if(OPTION_PARSE_LOG == m_BuildInOption[nOption].Id)
                    {
                        cout << "         ";
                    }
                    else
                    {
                        cout << "           ";
                    }
                }

                if(OPTION_PARSE_LOG == m_BuildInOption[nOption].Id)
                {
                    cout << subParamStr;
                }
                else
                {
                    cout << m_BuildInOption[nOption].Name << "  " << subParamStr;
                }
            }
        }

        cout << endl;
    }
}

void VscSm2246::printLicense()
{
    cout << endl << MiscUtil::GetLicenseString() << endl;
}

string VscSm2246::makeFileName(BaseDevice* pSmiDev, string prefix, string posfix, string extention)
{
    string fileName;

    if(false == pSmiDev->readIdentify())
    {
        cout << endl << "FAIL: Make file name read Identify.";
        return fileName;
    }

    fileName = prefix + "_";
    U8* idBuffer = pSmiDev->getBufferPtr();
    if (pSmiDev->m_CommandType != CMDTYPE_SCSI)
    {
        sIdentifyInfo driveInfo;
        DeviceUtil::ParseIdentifyInfo(idBuffer, pSmiDev->m_Name, driveInfo);
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

    return fileName;
}

U32 VscSm2246::getPercent(U32 total)
{
    U32 percent = 50;

    if(total < 100) percent = 50;
    else if(total < 1000)   percent = 100;
    else if(total < 10000)  percent = 1000;
    else if(total < 100000) percent = 10000;
    else percent = 100000;

    return percent;
}

