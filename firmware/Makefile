# Project Name
TARGET = SproutExample

# Sampling Rate
SAMPLING_RATE = 96000

# INTERNAL FLASH
# APP_TYPE = BOOT_NONE
# SRAM
APP_TYPE = BOOT_SRAM
# QSPI
#APP_TYPE = BOOT_QSPI

ifdef ARM_TOOLKIT_ROOT
GCC_PATH = $(ARM_TOOLKIT_ROOT)/bin
endif

# Sources
SRC_DIR=./src/
CPP_SOURCES = $(wildcard $(SRC_DIR)*.cpp) $(wildcard $(SRC_DIR)periph/*.cpp) $(wildcard $(SRC_DIR)util/*.cpp) $(wildcard $(SRC_DIR)fx_model/*.cpp)
CPP_SOURCES += igb_sdk/igb_stm32/periph/systick.cpp
C_SOURCES = $(wildcard $(SRC_DIR)*.c)

C_INCLUDES = -Isrc -Iigb_sdk -DUSE_DAISY -DUSE_FULL_LL_DRIVER -DUSE_FULL_LL_DRIVER -DIGB_FIXED_SAMPLING_RATE=$(SAMPLING_RATE)

CPP_STANDARD = -std=c++2b -fconcepts

# Library Locations
LIBDAISY_DIR = libdaisy

# FatFs
FATFS_DIR = $(LIBDAISY_DIR)/Middlewares/Third_Party/FatFs/src
C_SOURCES += \
	$(FATFS_DIR)/diskio.c \
	$(FATFS_DIR)/ff.c \
	$(FATFS_DIR)/ff_gen_drv.c \
	$(FATFS_DIR)/option/ccsbcs.c \

# FATFS dir contains fatfs itself, libdaisy sys folder contains ffconf 
C_INCLUDES += -I$(FATFS_DIR) -I$(LIBDAISY_DIR)/src/sys


# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

