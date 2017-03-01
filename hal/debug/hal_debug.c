#include "syntax.h"
#include "stm32f10x.h"
#include "fw_wstream.h"
#include "debug.h"
#include "hal_tty.h"
#include "hal_debug.h"


static void uart_putc_wrapper(wstream_t *stream, u8 byte) { HAL_tty_putc(byte);}
static void uart_puts_wrapper(wstream_t *stream, const char *str) { HAL_tty_puts(str);}
wstream_t dbg_stream = {.putc = uart_putc_wrapper, .puts = uart_puts_wrapper};

__noreturn void HAL_assert(const char *str)
{
    __disable_irq();
    HAL_tty_init();
    while (1)
    {
        HAL_tty_puts("\n"ANSI_RED"Assertion:");
        HAL_tty_puts(str);
        for (volatile uint i = 10000000; i; i--);
    }
}

__noreturn void hal_exception(u32 pc, const u32 *sp)
{
    __disable_irq();
    HAL_tty_init();
    while (1)
    {
        printf("\n"ANSI_RED"Exception. PC 0x%08x, sp 0x%08x", (uint) pc, (uint)sp);
//      for (volatile uint i = 10000000; i; i--);
    }
}

