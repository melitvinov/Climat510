#include "syntax.h"

#include "hal.h"
#include "keyboard.h"
#include "timers.h"
#include "debug.h"

static u8 BITKL;
static KEY_PRESSED SIM;
static timer_t timer;

static const u8 remap_lut[] =
{
    [HAL_KEY_0] = KEY_0,
    [HAL_KEY_1] = KEY_1,
    [HAL_KEY_2] = KEY_2_UP,
    [HAL_KEY_3] = KEY_3,
    [HAL_KEY_4] = KEY_4_LEFT,
    [HAL_KEY_5] = KEY_5,
    [HAL_KEY_6] = KEY_6_RIGHT,
    [HAL_KEY_7] = KEY_7,
    [HAL_KEY_8] = KEY_8_DOWN,
    [HAL_KEY_9] = KEY_9,
    [HAL_KEY_LEFT] = KEY_LEFT,
    [HAL_KEY_RIGHT] = KEY_RIGHT,
    [HAL_KEY_UP] = KEY_UP,
    [HAL_KEY_DOWN] = KEY_DOWN,
    [HAL_KEY_EDIT] = KEY_EDIT,
};

void keyboardSetBITKL(uchar press)
{
    BITKL = press;
}

uchar keyboardGetBITKL(void)
{
    return BITKL;
}

void keyboardSetSIM(KEY_PRESSED key)
{
    SIM = key;
}

KEY_PRESSED keyboardGetSIM(void)
{
    return SIM;
}

static void on_timer(timer_t *timer)
{
    KeyboardProcess();
}

// непонятно зачем вообще что то возвращать, если это не используется
void KeyboardProcess(void)
{
    u32 keys = HAL_keyb_read();

    if (! keys)
        return;

    int idx = ctz(keys);
    SIM = remap_lut[idx];
    BITKL = 1;

    LOG("sim is %d", SIM);
}

void Keyboard_Init(void)
{
    HAL_keyb_init();
    timer_start(&timer, 100, 1, on_timer);
}
