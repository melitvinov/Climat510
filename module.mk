# root module makefile

# C source files
SRC += stm_lib/stm32f10x_rcc.c
SRC += stm_lib/stm32f10x_gpio.c
SRC += stm_lib/stm32f10x_exti.c
SRC += stm_lib/stm32f10x_bkp.c
SRC += stm_lib/stm32f10x_pwr.c
SRC += stm_lib/stm32f10x_spi.c
SRC += stm_lib/stm32f10x_usart.c
SRC += stm_lib/misc.c
SRC += stm_lib/stm32f10x_iwdg.c

SRC += stm_my/stm32f10x_RS485Master.c
SRC += stm_my/stm32f10x_RS485.c
SRC += stm_my/stm32f10x__clock.c
SRC += keyboard.c

# these are 'coarse fixed'
SRC += 405_Memory.c


SRC += stm_my/spi.c
SRC += stm_my/enc28j60.c
SRC += stm_my/ip_arp_udp_tcp.c
SRC += stm_my/simple_server.c
SRC += stm_my/stm32f10x_Rootines.c
SRC += stm_my/stm32f10x_LCD240x64.c

SRC += 405_Display.c
SRC += 405_Menu.c

# temporary hacks
SRC += wtf.c
SRC += 405_cfg.c
SRC += main_consts.c
SRC += unsorted.c

SRC += main.c

CINCS += . stm_my stm_lib

# from coocox
CDEFS += -DSTM32F10X_CL
