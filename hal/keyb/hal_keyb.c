#include "syntax.h"
#include "stm32f10x.h"
#include "hal_pincfg.h"
#include "hal_systimer.h"
#include "hal_keyb.h"

// matrix keyboard scanning module
//
// row pins are for driving (open drain outputs)
// PC0, PC1, PC2, PC3
//
// col pins are for sensing (inputs with the pullups)
// PA0, PA1, PA2, PA3

typedef struct
{
    u32 last_valid_read;
} keyb_rt_t;

static GPIO_TypeDef * const sense_port = GPIOA;
static GPIO_TypeDef * const drive_port = GPIOC;
static const u32 pins_mask = 0x0F;
static const u32 all_keys_mask = 0xFFFF & ~(1 << _HAL_KEY_NC);

static keyb_rt_t rt;

void HAL_keyb_init(void)
{
    // activate pullups
    sense_port->BSRR |= pins_mask;
    drive_port->BSRR |= pins_mask;

    hal_pincfg_in(sense_port, 0);
    hal_pincfg_in(sense_port, 1);
    hal_pincfg_in(sense_port, 2);
    hal_pincfg_in(sense_port, 3);

    hal_pincfg_od(drive_port, 0);
    hal_pincfg_od(drive_port, 1);
    hal_pincfg_od(drive_port, 2);
    hal_pincfg_od(drive_port, 3);

    rt.last_valid_read = 0;
}


u32 HAL_keyb_read(void)
{
    u32 result = 0;

    for (int row_idx = 3; row_idx >= 0; row_idx--)
    {
        u32 row_mask = 1 << row_idx;

        drive_port->BSRR = (row_mask << 16) | (pins_mask & ~row_mask);   // drive row line low, clear prev row line
        hal_systimer_sleep_us(1);                                        // wait 1us to let the pins settle
        result <<= 4;
        result |= sense_port->IDR & pins_mask;
    }

    result = ~result & all_keys_mask;    // leave connected keys, invert to make pressed bits set

    // accept sample only if there are no multiple keypresses.
    // this hack will prevent ghosts

    if ((result & (result - 1)) == 0)
        rt.last_valid_read = result;

    return rt.last_valid_read;
}
