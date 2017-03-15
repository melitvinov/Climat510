#ifndef _HAL_TTY_H_
#define _HAL_TTY_H_

void HAL_tty_init(void);
void HAL_tty_putc(u8 chr);
void HAL_tty_puts(const char *str);
int HAL_tty_getc(void);

#ifdef _HAL_TTY_C_
// private

#define HAL_TTY_TX_BUF_SIZE    512
#define HAL_TTY_RX_BUF_SIZE    128

#endif
#endif
