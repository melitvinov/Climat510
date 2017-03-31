# Configuration for cnc_mole project

# Target file name (without extension).
TARGET := climate_510

# MCU architecture
MCU_ARCH := cortex-m3

# Compiler optimization
OPT := 2

# Firmware version ("manual" part)
FW_VER := "trunk"

# Build type
CFG := release

# Boot image name
BOOT_IMAGE := firmware.bin

# Should extended listing be generated
GENERATE_LSS := false
#GENERATE_LSS := true

# Print commands executed during build
#QUIET := false
QUIET := true

# Use link-time optimization (a slightly longer build)

# XXX: not working for coocox-provided startup. tbd.

#USE_LTO := true
USE_LTO := false

# Modules to build project from
MODULES := hal env fwlib app app/control

# Build directory - place for intermediate objects, listings etc.
BUILD_DIR := $(CFG)/build
# Target directory. Final binary will be placed there.
TARG_DIR := $(CFG)/out
# Tools directory. Python scripts are hosted there
TOOLS_DIR := tools
# Path to stlink
STLINK := st-link_cli

# Defines to pass to compiler
CDEFS := -DAGAPOVSKIY_DOUBLE_VALVE -DDEBUG -DNOTESTMEM

# Additional libraries
LIBS := -lm

# Global disable of logging
#CDEFS += -DNOLOG
#CDEFS += -DNOWARN -DNOERR
#CDEFS += -DNOASSERT -DNOPOSTMORTEM

# Global disable of profiling
#CDEFS += -DNOPROFILE

# Linker script
LINKER_SCRIPT := link.ld
