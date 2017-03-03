#ifndef _HAL_KEYB_H_
#define _HAL_KEYB_H_

// key numbers aka bit positions
enum hal_keys_e
{
    HAL_KEY_0,
    HAL_KEY_4,
    HAL_KEY_8,
    HAL_KEY_LEFT,
    HAL_KEY_1,
    HAL_KEY_5,
    HAL_KEY_9,
    HAL_KEY_UP,
    HAL_KEY_2,
    HAL_KEY_6,
    HAL_KEY_EDIT,
    HAL_KEY_RIGHT,
    HAL_KEY_3,
    HAL_KEY_7,
    _HAL_KEY_NC,
    HAL_KEY_DOWN,
};

void HAL_keyb_init(void);
u32 HAL_keyb_read(void);

#endif
