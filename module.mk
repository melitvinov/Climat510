# root module makefile

# C source files

# these are good
SRC += sound.c
SRC += timers.c

# these are semi-good
SRC += module.c
SRC += modules_master.c

# these are ugly
SRC += rtc.c
SRC += keyboard.c
SRC += measure.c
SRC += lcd.c

# these are 'coarse fixed'
SRC += 405_Memory.c

SRC += stm_my/ip_arp_udp_tcp.c
SRC += stm_my/simple_server.c
SRC += stm_my/stm32f10x_Rootines.c

SRC += 405_Display.c
SRC += 405_Menu.c

# temporary hacks
SRC += wtf.c
SRC += 405_cfg.c
SRC += main_consts.c
SRC += unsorted.c

SRC += main.c

CINCS += . stm_my
