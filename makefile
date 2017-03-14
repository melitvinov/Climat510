# Makefile for climate controller project

# C sources for target
SRC :=
# Asm sources for target
ASRC :=
# C include dirs
CINCS :=
# Autogenerated files (must be generated before main build)
AUTOGENS :=
# Additional files to cleanup
EXTRA_CLEAN :=
# All directories to exist
ALL_DIRS :=

# User configuration file
include configuration.mk

# Include defined modules. Each module will append own files to lists (SRC, ASRC, etc.)
include $(addsuffix /module.mk,$(MODULES))

# Output format. (can be srec, ihex, binary)
FORMAT := binary

# Combined variable - used to create needed dirs later

ALL_SRCS := $(SRC) $(ASRC)
ALL_DIRS += $(addprefix $(BUILD_DIR)/, $(sort $(dir $(ALL_SRCS))))
ALL_DIRS += $(TARG_DIR) $(BUILD_DIR) $(sort $(dir $(AUTOGENS)))
ALL_DIRS +=
# Remove trailing slashes
ALL_DIRS := $(subst / , , $(ALL_DIRS))

#---------------- Common compiler options ----------------
# General
COMMON_CFLAGS = -std=gnu99 -pipe -O$(OPT) -g
#-gdwarf-2
# Listings
COMMON_CFLAGS += -Wa,-adlmns=$(@:.o=.lst)
# Warnings

# these are disabled, since amount of legacy warnings is INSANE
COMMON_CFLAGS += -Wall -Wcast-align -Wimplicit -Wuninitialized
#COMMON_CFLAGS += -Wstrict-prototypes
COMMON_CFLAGS += -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wunused

COMMON_CFLAGS += -Wno-char-subscripts

#COMMON_CFLAGS += -Wshadow -Winline

COMMON_CFLAGS += -Wstrict-aliasing
COMMON_CFLAGS += -Wbad-function-cast -Wsign-compare
COMMON_CFLAGS += -Wunsafe-loop-optimizations
COMMON_CFLAGS += -Wwrite-strings

#COMMON_CFLAGS += -w

# Various
COMMON_CFLAGS += -ffunction-sections -fdata-sections -fverbose-asm -fstrict-aliasing
# Auto-generation of dependencies
COMMON_CFLAGS += -MMD -MP

#---------------- ARM compiler options (default) ----------------
ARM_CFLAGS = -mcpu=$(MCU_ARCH) -mthumb -mfix-cortex-m3-ldrd -mno-thumb-interwork -mapcs-frame
ARM_CFLAGS += -ffreestanding
#-fomit-frame-pointer -mlong-calls
ifeq ($(USE_LTO), true)
  ARM_CFLAGS += -flto
endif
# CFLAGS may be used for overriding some options
ALL_CFLAGS = $(COMMON_CFLAGS) $(ARM_CFLAGS) $(CDEFS) $(addprefix -I,$(CINCS)) $(CFLAGS)

#---------------- Common assembler options ----------------
# Common
COMMON_ASFLAGS = -std=gnu99 -x assembler-with-cpp -pipe
#-gdwarf-2
# Listings
COMMON_ASFLAGS += -Wa,-adlmns=$(@:.o=.lst)
# Auto-generation of dependencies
COMMON_ASFLAGS += -MMD -MP

#---------------- ARM assembler options (default) ----------------
# ARM-specific
ARM_ASFLAGS = -mcpu=$(MCU_ARCH) -mthumb -mno-thumb-interwork -mapcs-frame
ifeq ($(USE_LTO), true)
  ARM_ASFLAGS += -flto
endif
# ASFLAGS may be used for overriding some options
ALL_ASFLAGS += $(COMMON_ASFLAGS) $(ARM_ASFLAGS) $(CDEFS) $(addprefix -I,$(CINCS)) $(ASFLAGS)

#---------------- ARM Linker options ----------------
LDFLAGS := -lc -lgcc -nostartfiles
#-gdwarf-2
ifeq ($(USE_LTO), true)
  LDFLAGS += -fno-use-linker-plugin -fwhole-program
endif
LDFLAGS += -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,-Map=$(TARG_DIR)/$(TARGET).map,--cref,--gc-sections
#---------------- Commands ----------------

ifeq ($(QUIET), true)
  CMDP = @
endif

TOOLCHAIN := arm-none-eabi

SHELL := sh
WINSHELL := cmd

CC := $(TOOLCHAIN)-gcc
OBJCOPY := $(TOOLCHAIN)-objcopy
OBJDUMP := $(TOOLCHAIN)-objdump
SIZE := $(TOOLCHAIN)-size
NM := $(TOOLCHAIN)-nm
STRIP := $(TOOLCHAIN)-strip
STLINK := st-link_cli

GREP := grep
TR := tr
PY := py
REMOVE := rm -f -r
COPY := cp
MD := mkdir -p

#---------------- Objects ----------------
# Define all object files.
OBJS := $(addprefix $(BUILD_DIR)/, $(SRC:.c=.o) $(ASRC:.asm=.o))
# Define all preprocessed files.
PREPS := $(addprefix $(BUILD_DIR)/, $(SRC:.c=.i))
# Dependencies
DEPS = $(OBJS:.o=.d)


#---------------- Chain rules ----------------
# Default target
all: build elfsize

# Clean project
clean: clean_list

# generate prerequisites for build
prebuild: $(ALL_DIRS) $(AUTOGENS)

preprocess: $(ALL_DIRS) $(AUTOGENS) $(PREPS)

# compile sources (may be run in parallel with -j=n)
compile: $(OBJS)

# Build target
build: prebuild elf bin hex sym

# Generate extended listing after build if required
ifeq ($(GENERATE_LSS), true)
  build: lss
endif

burn: build upload

elf: $(TARG_DIR)/$(TARGET).elf
bin: $(TARG_DIR)/$(TARGET).bin
hex: $(TARG_DIR)/$(TARGET).hex
lss: $(TARG_DIR)/$(TARGET).lss
sym: $(TARG_DIR)/$(TARGET).sym

#---------------- Additional depends ----------------
# rebuild sources at change of configuration
$(OBJS) : configuration.mk

#---------------- Recipes ----------------
$(ALL_DIRS):
	@echo 'Creating dir' $@
	$(CMDP) $(MD) $@

begin:
	@echo
	@echo '-------- begin --------'
	@echo 'Target directory is' $(TARG_DIR)

end:
	@echo '--------  end  --------'
	@echo

gccversion :
	$(CMDP) $(CC) --version

elfsize: $(TARG_DIR)/$(TARGET).elf
	@echo
	$(CMDP) $(SIZE) -A $<

binsize: $(TARG_DIR)/$(TARGET.bin)
	@echo
	$(CMDP) $(SIZE) -A --target=$(FORMAT) $<

# Create output file from ELF output file.
$(TARG_DIR)/$(TARGET).bin: $(TARG_DIR)/$(TARGET).elf
	@echo 'Creating target binary' $@
	$(CMDP) $(OBJCOPY) -O binary $< $@

# Create output file from ELF output file.
$(TARG_DIR)/$(TARGET).hex: $(TARG_DIR)/$(TARGET).elf
	@echo 'Creating target hex' $@
	$(CMDP) $(OBJCOPY) -O ihex $< $@

# Create extended listing file from ELF output file.
$(TARG_DIR)/$(TARGET).lss: $(TARG_DIR)/$(TARGET).elf
	@echo 'Creating Extended Listing' $@
	$(CMDP) $(OBJDUMP) -h -w -S -D $< > $@

# Create a symbol table from ELF output file.
$(TARG_DIR)/$(TARGET).sym: $(TARG_DIR)/$(TARGET).elf
	@echo 'Creating Symbol Table' $@
	$(CMDP) $(NM) -n $< > $@

# Link: create ELF output file from object files.
$(TARG_DIR)/$(TARGET).elf: $(OBJS) $(LINKER_SCRIPT)
	@echo 'Linking' $@
	$(CMDP) $(CC) $(ALL_CFLAGS) $(LDFLAGS) --output $@ $(OBJS) $(LIBS)
#	$(CMDP) $(STRIP) $@ --strip-debug

# Compile: create object files from C source files.
$(BUILD_DIR)/%.o : %.c
	@echo 'Compiling' $<
	$(CMDP) $(CC) -c $(ALL_CFLAGS) $< -o $@

# Preprocess: create preprocessed i.files from C source files.
$(BUILD_DIR)/%.i : %.c
	@echo 'Preprocessing' $<
	$(CMDP) $(CC) -E $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(BUILD_DIR)/%.o : %.asm
	@echo 'Assembling' $<
	$(CMDP) $(CC) -c $(ALL_ASFLAGS) $< -o $@

clean_list:
	@echo 'Cleaning project'
	$(CMDP) $(REMOVE) $(BUILD_DIR) $(TARG_DIR)
	$(CMDP) $(REMOVE) $(OBJS)
	$(CMDP) $(REMOVE) $(OBJS:.o=.lst)
	$(CMDP) $(REMOVE) $(DEPS)
	$(CMDP) $(REMOVE) $(AUTOGENS)
	$(CMDP) $(REMOVE) $(EXTRA_CLEAN)

upload: $(TARG_DIR)/$(TARGET).hex
	@echo 'Loading firmware'
	$(CMDP) @$(STLINK_DIR)/$(STLINK) -c SWD -P $< -V -Rst


# autogenerated dependencies
-include $(DEPS)

# Listing of phony targets.
.PHONY : all begin end elfsize binsize gccversion build elf bin lss sym preprocess compile prebuild clean clean_list gdb-config burn upload
