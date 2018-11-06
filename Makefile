CC  = gcc
CXX = g++

#remove @ for no make command prints
DEBUG = @

ifeq ($(ENV),32)
    APP_NAME = vtViewCmd_32b
else
    APP_NAME = vtViewCmd_64b
endif

APP_DIR = .
OUTPUT_DIR = $(APP_DIR)/output

# -------------------------------------------------------
# application
# -------------------------------------------------------
MAIN   = $(APP_DIR)
NVME   = $(APP_DIR)/nvme
LINUX  = $(NVME)/linux
PORT   = $(APP_DIR)/port/linuxtest
UTIL   = $(APP_DIR)/utility
CORE   = $(APP_DIR)/core
MISC   = $(UTIL)/misc
CMD    = $(UTIL)/cmd
DEV    = $(UTIL)/device
CRYPT  = $(UTIL)/crypt
VSC    = $(UTIL)/vsc

APP_INCLUDE_DIRS += -I $(MAIN)/
APP_INCLUDE_DIRS += -I $(NVME)/
APP_INCLUDE_DIRS += -I $(LINUX)/
APP_INCLUDE_DIRS += -I $(PORT)/
APP_INCLUDE_DIRS += -I $(UTIL)/
APP_INCLUDE_DIRS += -I $(CORE)/
APP_INCLUDE_DIRS += -I $(MISC)/
APP_INCLUDE_DIRS += -I $(CMD)/
APP_INCLUDE_DIRS += -I $(CRYPT)/
APP_INCLUDE_DIRS += -I $(VSC)/

APP_SRC_FILES += $(MAIN)/AppMain.cpp \
                 $(MAIN)/AppData.cpp \
                 $(MAIN)/SmartCmd.cpp \
                 $(MAIN)/AppConfigFile.cpp 

APP_SRC_FILES += $(CORE)/CoreData.cpp \
                 $(CORE)/CoreUtil.cpp \
                 $(CORE)/SmartUtil.cpp \
                 $(CORE)/CommonStruct.cpp \
                 $(CORE)/NvmeData.cpp 

APP_SRC_FILES += $(PORT)/SystemUtil.cpp\
                 $(PORT)/PortUtil.cpp\
                 $(PORT)/ATACommand.cpp\
                 $(PORT)/SATCommand.cpp\
                 $(PORT)/SCSICommand.cpp\
                 $(PORT)/NVMECommand.cpp\
                 $(PORT)/CSMICommand.cpp\
                 $(PORT)/STPCommand.cpp\
                 $(PORT)/HYPCommand.cpp\
                 $(PORT)/CSMIPrinter.cpp\
                 $(PORT)/Device.cpp \
                 $(PORT)/DeviceUtil.cpp \
                 $(PORT)/BaseDevice.cpp \
                 $(PORT)/ATADevice.cpp \
                 $(PORT)/SATDevice.cpp \
                 $(PORT)/STPDevice.cpp \
                 $(PORT)/HYPDevice.cpp \
                 $(PORT)/SCSIDevice.cpp \
                 $(PORT)/DeviceManager.cpp \
                 $(PORT)/PortPthread.cpp 

APP_SRC_FILES += $(MISC)/MiscUtil.cpp \
                 $(MISC)/AppLogger.cpp \
                 $(MISC)/ArrayCounter.cpp \
                 $(MISC)/ArgParser.cpp \
                 $(MISC)/CryptUtil.cpp \
                 $(MISC)/HexFormatter.cpp \
                 $(MISC)/ConfigFile.cpp \
                 $(MISC)/FileUtil.cpp \
                 $(MISC)/StringUtil.cpp \
                 $(MISC)/SystemUtilCommon.cpp 

APP_SRC_FILES += $(CMD)/BaseCommand.cpp \
                 $(CMD)/SATCommandCommon.cpp \
                 $(CMD)/ATACommandCommon.cpp \
                 $(CMD)/SCSICommandCommon.cpp \
                 $(CMD)/NVMECommandCommon.cpp \
                 $(CMD)/STPCommandCommon.cpp \
                 $(CMD)/HYPCommandCommon.cpp \
                 $(CMD)/CSMICommandCommon.cpp \
                 $(CMD)/CSMIPrinterCommon.cpp 

APP_SRC_FILES += $(DEV)/DeviceCommon.cpp \
                 $(DEV)/DeviceUtilCommon.cpp \
                 $(DEV)/BaseDeviceCommon.cpp \
                 $(DEV)/ATADeviceCommon.cpp \
                 $(DEV)/SATDeviceCommon.cpp \
                 $(DEV)/STPDeviceCommon.cpp \
                 $(DEV)/HYPDeviceCommon.cpp \
                 $(DEV)/SCSIDeviceCommon.cpp \
                 $(DEV)/NVMEDeviceCommon.cpp \
                 $(DEV)/DeviceManagerCommon.cpp 

APP_SRC_FILES += $(CRYPT)/MD5.cpp \
                 $(CRYPT)/MD5Util.cpp 

APP_SRC_FILES += $(VSC)/smi/SM2250Util.cpp \
                 $(VSC)/virtium/VscVirtiumDefine.cpp \
                 $(VSC)/virtium/VscVirtiumExecutor.cpp \
                 $(VSC)/virtium/VscVirtiumProtocol.cpp \
                 $(VSC)/virtium/VscVirtiumFunction.cpp 

ifeq ($(ENV),32)
	COMPILER_FLAGS += -m32
endif

COMPILER_FLAGS += -pipe -std=c++0x -O2 -std=gnu++0x -static -Wall -W -D_REENTRANT -fPIC -DNVME_VERSION='"1.5"'

DEFINES       = -D__KERNEL__ -D__LINUX__ 

LIBS          = -lm -ldl -pthread -lrt -lpthread

OBJS          = $(NVME)/NvmeCmd.o \
                $(NVME)/argconfig.o \
                $(NVME)/suffix.o \
                $(NVME)/parser.o \
                $(NVME)/nvme-print.o \
                $(NVME)/nvme-ioctl.o \
                $(NVME)/nvme-lightnvm.o \
                $(NVME)/fabrics.o \
                $(NVME)/json.o \
                $(NVME)/plugin.o \
                $(NVME)/intel-nvme.o \
                $(NVME)/lnvm-nvme.o \
                $(NVME)/memblaze-nvme.o \
                $(NVME)/wdc-nvme.o \
                $(NVME)/wdc-utils.o \
                $(NVME)/nvme-models.o \
                $(NVME)/huawei-nvme.o \
                $(NVME)/netapp-nvme.o \
                $(NVME)/toshiba-nvme.o

# -------------------------------------------------------
# Precompiler
# -------------------------------------------------------
PRE_COMP_FLAG   += -D_GNU_SOURCE -D__CHECK_ENDIAN__ -O2 -g -Wall -Werror -std=gnu99 -X -DNVME_VERSION='"1.5"'

PRE_COMP_SRC    += $(NVME)/NvmeCmd.c 

PRE_COMP_INC    += -I $(NVME)/
PRE_COMP_INC    += -I $(LINUX)/
# -------------------------------------------------------

MAKE_CMD = $(CXX) $(COMPILER_FLAGS) $(DEFINES) $(APP_SRC_FILES) $(SUB_INCLUDE) $(APP_INCLUDE_DIRS) $(LIBS) -o $(OUTPUT_DIR)/$(APP_NAME) $(OBJS)

all:
	$(MAKE_CMD)
	$(POST_MAKE_CMD)

clean:
	rm -f $(OUTPUT_DIR)/$(APP_NAME)
