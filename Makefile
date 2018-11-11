# Flags
CXXFLAGS        = -std=c++11 -O2 -MD -Wall -Wextra -g
LIBS          = -lm -ldl -pthread -lrt -lpthread
DEFINES       = -D__KERNEL__ -D__LINUX__
# Compiler, tools
CC              ?= gcc
CXX             ?= g++
# Executable 
APP_NAME = vtfa
# Define directories
APP             = .
PORT            = port/linuxtest
UTIL            = utility
CORE            = core
MISC            = $(UTIL)/misc
CMD             = $(UTIL)/cmd
DEV             = $(UTIL)/device
CRYPT           = $(UTIL)/crypt
VSC             = $(UTIL)/vsc
OUTPUT_DIR      = output


CFLAGS			= -I$(APP) -I$(PORT) -I$(UTIL) -I$(CORE) -I$(MISC) -I$(CMD) -I$(DEV) -I$(CRYPT)

APP_SRC_FILES += AppMain.cpp \
                 Sm2246.cpp \
                 VscSm2246.cpp \
                 EventLog.cpp

APP_SRC_FILES += $(CORE)/CoreData.cpp \
                 $(CORE)/CoreUtil.cpp \
                 $(CORE)/SmartUtil.cpp \
                 $(CORE)/CommonStruct.cpp 

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


OBJECTS      	= $(patsubst %.cpp, $(OUTPUT_DIR)/%.o, $(APP_SRC_FILES))
DEPS 			= $(OBJECTS:.o=.d)

.PHONY: dirs all clean
all: dirs $(OUTPUT_DIR)/$(APP_NAME)

dirs:
	@echo "Creating directories"
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(dir $(OBJECTS))

	
$(OUTPUT_DIR)/%.o: %.cpp
	@echo "Compiling $< --> $@"
	@$(CXX) -c -o $@ $< $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(LIBS)

$(OUTPUT_DIR)/$(APP_NAME): $(OBJECTS)
	@echo "Linking: $@"
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(LIBS)
	@echo "Executable file: $@"

clean:
	@rm -rf $(OUTPUT_DIR)

# Add dependency files, if they exist
-include $(DEPS)