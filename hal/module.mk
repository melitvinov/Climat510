SRC += hal/beep/hal_beep.c
SRC += hal/debug/hal_debug.c
SRC += hal/i2c/hal_i2c_bitbang.c
SRC += hal/keyb/hal_keyb.c
SRC += hal/lcd/hal_lcd.c
SRC += hal/nvmem/hal_nvmem.c
SRC += hal/pincfg/hal_pincfg.c
SRC += hal/rtc/hal_rtc.c
SRC += hal/sys/hal_sys.c
SRC += hal/systimer/hal_systimer.c
#SRC += hal/rs485/hal_rs485.c
SRC += hal/fieldbus/hal_fieldbus.c

#SRC += hal/tty/hal_tty_bitbang.c
SRC += hal/tty/hal_tty_hw.c

HAL_CINCS :=
HAL_CINCS += hal/debug
HAL_CINCS += hal/keyb
HAL_CINCS += hal/lcd
HAL_CINCS += hal/nvmem
HAL_CINCS += hal/rtc
HAL_CINCS += hal/systimer
HAL_CINCS += hal/tty
HAL_CINCS += hal/beep
HAL_CINCS += hal/rs485
HAL_CINCS += hal/fieldbus

# just for now
HAL_CINCS += hal/core

HAL_PRIVATE_CINCS := hal
HAL_PRIVATE_CINCS += env
HAL_PRIVATE_CINCS += hal/sys
HAL_PRIVATE_CINCS += hal/core
HAL_PRIVATE_CINCS += hal/debug
HAL_PRIVATE_CINCS += hal/pincfg
HAL_PRIVATE_CINCS += hal/i2c
HAL_PRIVATE_CINCS += hal/beep

HAL_PRIVATE_CINCS += fwlib

HAL_PRIVATE_CINCS += stm_lib/inc
HAL_PRIVATE_CINCS += cmsis_boot
HAL_PRIVATE_CINCS += cmsis

CINCS += $(HAL_CINCS)

$(BUILD_DIR)/hal/%.o : CINCS := $(HAL_CINCS) $(HAL_PRIVATE_CINCS)
#$(BUILD_DIR)/hal/%.o : CDEFS += -DSTM32F10X_CL

AUTOGENS += hal/lcd/lcd_font.h

hal/lcd/lcd_font.h: $(TOOLS_DIR)/genfont/font.png
	@echo 'Generating lcd font' $@
	$(CMDP) $(PY) $(TOOLS_DIR)/genfont/genfont.py $< $@
