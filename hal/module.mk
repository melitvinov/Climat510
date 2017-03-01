SRC += hal/tty/hal_tty.c
SRC += hal/sys/hal_sys.c

HAL_CINCS :=
HAL_CINCS += hal/tty

# just for now
HAL_CINCS += hal/core

HAL_PRIVATE_CINCS := hal
HAL_PRIVATE_CINCS += env
HAL_PRIVATE_CINCS += hal/sys
HAL_PRIVATE_CINCS += hal/core

HAL_PRIVATE_CINCS += stm_lib/inc
HAL_PRIVATE_CINCS += cmsis_boot
HAL_PRIVATE_CINCS += cmsis

CINCS += $(HAL_CINCS)

$(BUILD_DIR)/hal/%.o : CINCS := $(HAL_CINCS) $(HAL_PRIVATE_CINCS)
