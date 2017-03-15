#include "syntax.h"
#include "stm32f10x.h"
#include "fw_wstream.h"
#include "debug.h"
#include "hal_tty.h"
#include "hal_lcd.h"

#include "hal_systimer.h"
#include "hal_pincfg.h"
#include "hal_debug.h"

static void uart_putc_wrapper(wstream_t *stream, u8 byte) { HAL_tty_putc(byte);}
static void uart_puts_wrapper(wstream_t *stream, const char *str) { HAL_tty_puts(str);}
wstream_t dbg_stream = {.putc = uart_putc_wrapper, .puts = uart_puts_wrapper};


static void blink(void)
{
    static int clock = -1;
    if (clock < 0)
        hal_pincfg_out(GPIOB, 8);

    if (clock)
    {
        GPIOB->BSRR = 1 << 8;
        clock = 0;
    }
    else
    {
        GPIOB->BSRR = 1 << (8 + 16);
        clock = 1;
    }
}


static void print_u32(char *str, u32 val)
{
    for (uint i = 8; i; i--, val <<= 4)
    {
        u32 nib = val >> 28;
        *str++ = nib < 0x0A ? (nib + '0') : (nib - 0x0A + 'A');
    }
    *str = '\0';
}


static void __noinline show_lcd_assert(const char *prefix, const char *str)
{
    hal_lcd_text_buf_t buf;
    memset(&buf, 0,  sizeof(buf));
    strcpy((char *)buf.lines[0], prefix);
    strcpy((char *)buf.lines[2], str);

    HAL_lcd_render_text(&buf);
}


__noreturn void HAL_assert(const char *str)
{
    __disable_irq();
    HAL_tty_init();

    int cnt = 0;
    while (1)
    {
        HAL_tty_puts("\n"ANSI_RED"Assertion:");
        HAL_tty_puts(str);
        HAL_systimer_sleep(500);
        blink();

        // render lcd assert a little later in case it fail (if our stack is corrupted etc)
        // give tty a chance to deliver message
        if (cnt++ == 10)
        {
            show_lcd_assert("Assertion: ", str);
        }
    }
}


__noreturn void hal_exception(u32 pc, const u32 *sp)
{
    __disable_irq();
    HAL_tty_init();


    uint cnt = 0;

    char pc_str[9];
    char sp_str[9];

    print_u32(pc_str, pc);
    print_u32(sp_str, (u32)sp);

    while (1)
    {
        HAL_tty_puts("\n"ANSI_RED"Exception. PC 0x");
        HAL_tty_puts(pc_str);
        HAL_tty_puts(", sp 0x");
        HAL_tty_puts(sp_str);
        HAL_systimer_sleep(500);
        blink();

        if (cnt++ == 10)
        {
            show_lcd_assert("System Exception", pc_str);
        }
    }
}

