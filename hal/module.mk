SRC += hal/tty/hal_tty.c

HAL_CINCS :=
HAL_CINCS += hal/tty

HAL_PRIVATE_CINCS := hal
HAL_PRIVATE_CINCS += env
HAL_PRIVATE_CINCS += stm_lib/inc
HAL_PRIVATE_CINCS += cmsis_boot
HAL_PRIVATE_CINCS += cmsis

CINCS += $(HAL_CINCS)

$(BUILD_DIR)/hal/%.o : CINCS := $(HAL_CINCS) $(HAL_PRIVATE_CINCS)

