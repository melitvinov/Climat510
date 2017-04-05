# root module makefile

# C source files

# these are good
SRC += app/sound.c
SRC += app/timers.c
SRC += app/fieldbus.c

# these are semi-good
SRC += app/module.c
SRC += app/fbd.c

# these are ugly
SRC += app/rtc.c
SRC += app/keyboard.c
SRC += app/measure.c
SRC += app/lcd.c

# these are 'coarse fixed'
SRC += app/405_Memory.c

SRC += app/stm_my/ip_arp_udp_tcp.c
SRC += app/stm_my/simple_server.c
SRC += app/stm_my/stm32f10x_Rootines.c

SRC += app/405_Display.c
SRC += app/405_Menu.c

# temporary hacks
SRC += app/wtf.c
SRC += app/405_cfg.c
SRC += app/main_consts.c
SRC += app/unsorted.c

SRC += app/main.c

CINCS += app app/stm_my
