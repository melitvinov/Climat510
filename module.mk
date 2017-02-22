# root module makefile

# C source files
SRC += cmsis_boot/startup/startup_stm32f10x_cl.c
SRC += cmsis_boot/system_stm32f10x.c

SRC += stm_lib/src/stm32f10x_rcc.c
SRC += stm_lib/src/stm32f10x_gpio.c
SRC += stm_lib/src/stm32f10x_exti.c
SRC += stm_lib/src/stm32f10x_bkp.c
SRC += stm_lib/src/stm32f10x_pwr.c
SRC += stm_lib/src/stm32f10x_rtc.c
SRC += stm_lib/src/stm32f10x_spi.c
SRC += stm_lib/src/stm32f10x_usart.c
SRC += stm_lib/src/misc.c
SRC += stm_lib/src/stm32f10x_iwdg.c

SRC += stm_my/src/stm32f10x_RS485Master.c
SRC += stm_my/src/stm32f10x_RS485.c
SRC += stm_my/src/i2csoft.c
SRC += stm_my/src/stm32f10x__clock.c
SRC += keyboard/keyboard.c

# these are 'coarse fixed'
SRC += 405_Memory.c


SRC += stm_my/src/spi.c
SRC += stm_my/src/enc28j60.c
SRC += stm_my/src/ip_arp_udp_tcp.c
SRC += stm_my/src/simple_server.c
SRC += stm_my/src/stm32f10x_Rootines.c
SRC += stm_my/src/stm32f10x_LCD240x64.c

SRC += 405_Display.c
SRC += 405_Menu.c

# temporary hacks
SRC += wtf.c
SRC += 405_cfg.c
SRC += main_consts.c
SRC += unsorted.c

SRC += main.c

CINCS += . stm_my/inc stm_lib/inc cmsis_boot cmsis keyboard

# C sources which are #included (ugly !)
CINCS += stm_my/src

# from coocox
CDEFS += -DSTM32F105RC -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER -D__ASSEMBLY__
