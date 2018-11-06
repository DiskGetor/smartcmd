#include "utility/misc/StdMacro.h"
#include "utility/misc/MiscUtil.h"
#include "utility/cmd/CSMIPrinter.h"
#include "utility/cmd/CSMIPrinterMacro.h"

string CSMIPrinter::ToString(const IOCTL_HEADER& info, U32 idx) {
    stringstream sstr;

    PRINT_VALUE_INT(info, idx, IOControllerNumber);
    PRINT_VALUE_INT(info, idx, Length);
    PRINT_VALUE_INT(info, idx, ReturnCode);
    PRINT_VALUE_INT(info, idx, Timeout);
    PRINT_VALUE_INT(info, idx, Direction);

    return sstr.str();
}
