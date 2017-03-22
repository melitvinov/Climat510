SRC += hal/beep/hal_beep.c
SRC += hal/core/hal_atomic.c
SRC += hal/debug/hal_debug.c
SRC += hal/i2c/hal_i2c_bitbang.c
SRC += hal/keyb/hal_keyb.c
SRC += hal/lcd/hal_lcd.c
SRC += hal/net/hal_mac.c
SRC += hal/nvmem/hal_nvmem.c
SRC += hal/pincfg/hal_pincfg.c
SRC += hal/rtc/hal_rtc.c
SRC += hal/sys/hal_sys.c
SRC += hal/systimer/hal_systimer.c
SRC += hal/fieldbus/hal_fieldbus.c

# this one is very low-level and replaced by the fieldbus
#SRC += hal/rs485/hal_rs485.c

#SRC += hal/tty/hal_tty_bitbang.c
SRC += hal/tty/hal_tty_hw.c

CINCS += hal

HAL_CINCS :=
HAL_CINCS += env
HAL_CINCS += fwlib
HAL_CINCS += hal/beep
HAL_CINCS += hal/core
HAL_CINCS += hal/debug
HAL_CINCS += hal/fieldbus
HAL_CINCS += hal/i2c
HAL_CINCS += hal/keyb
HAL_CINCS += hal/lcd
HAL_CINCS += hal/net
HAL_CINCS += hal/nvmem
HAL_CINCS += hal/pincfg
HAL_CINCS += hal/rs485
HAL_CINCS += hal/rtc
HAL_CINCS += hal/sys
HAL_CINCS += hal/systimer
HAL_CINCS += hal/tty


HAL_API_OUTPUT := hal/hal.h

HAL_API_HEADERS :=
HAL_API_HEADERS += hal/beep/hal_beep.h
HAL_API_HEADERS += hal/tty/hal_tty.h
HAL_API_HEADERS += hal/net/hw_flags.h
HAL_API_HEADERS += hal/net/hal_mac.h
HAL_API_HEADERS += hal/debug/hal_debug.h
HAL_API_HEADERS += hal/lcd/hal_lcd.h
HAL_API_HEADERS += hal/nvmem/hal_nvmem.h
HAL_API_HEADERS += hal/systimer/hal_systimer.h
HAL_API_HEADERS += hal/fieldbus/hal_fieldbus.h
HAL_API_HEADERS += hal/rtc/hal_rtc.h
HAL_API_HEADERS += hal/keyb/hal_keyb.h

$(BUILD_DIR)/hal/%.o : CINCS := $(HAL_CINCS)
$(BUILD_DIR)/hal/%.o : CDEFS += -DSTM32F10X_CL

AUTOGENS += hal/lcd/lcd_font.h $(HAL_API_OUTPUT)

hal/lcd/lcd_font.h: $(TOOLS_DIR)/genfont/font.png
	@echo 'Generating lcd font' $@
	$(CMDP) $(PY) $(TOOLS_DIR)/genfont/genfont.py $< $@

$(HAL_API_OUTPUT): $(HAL_API_HEADERS)
	@echo 'Generating HAL API header' $@
	$(CMDP) $(PY) $(TOOLS_DIR)/apigen/apigen.py --guard _HAL_H_ -o $@ $^
