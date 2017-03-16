#define _HAL_TTY_C_

#include "syntax.h"
#include "stm32f10x.h"

#include "hal_priorities.h"
#include "hal_sys.h"
#include "hal_pincfg.h"
#include "hal_tty.h"

// hardware uart on pa9 tx (usart1_tx_slave), pa10 rx (usart1_rx_slave)

typedef struct
{
    struct
    {
        volatile uint r;
        volatile uint w;
        volatile u8 buf[HAL_TTY_TX_BUF_SIZE];
    } tx;

    struct
    {
        volatile uint r;
        volatile uint w;
        volatile u8 buf[HAL_TTY_RX_BUF_SIZE];
    } rx;
} tty_rt_t;

static USART_TypeDef *const uart = USART1;
static const u32 active_cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

static tty_rt_t rt;


static bool is_isr_blocked(void)
{
    u32 primask;
    u32 ipsr;
    __asm__ volatile ("mrs  %0, ipsr" : "=r" (ipsr));
    __asm__ volatile ("mrs %0, primask" : "=r" (primask));

    return (primask & 0x01) || (ipsr & 0x1FF);
}


void usart1_isr(void)
{
    u32 mask = USART_SR_ORE | USART_SR_RXNE | USART_SR_TXE;
    u32 sr = uart->SR & mask;

    // process overrun
    if (sr & USART_SR_ORE)
    {
        uart->DR;
    }
    else if (sr & USART_SR_RXNE)
    {
        uint chr = uart->DR;

        uint w = rt.rx.w;
        uint new_w = (w + 1) % countof(rt.rx.buf);
        if (new_w != rt.rx.r)
        {
            rt.rx.buf[w] = chr;
            rt.rx.w = new_w;
        }
    }

    // NOTE: this will trigger on reception if transmit buffer is empty, since
    // TXE flag would be pending. Nothing to worry, receptions should carry low traffic
    if (sr & USART_SR_TXE)
    {
        // transmit buffer empty, may send something
        uint r = rt.tx.r;
        if (r == rt.tx.w)
        {
            uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // disable tx interrupt
        }
        else
        {
            uart->DR = rt.tx.buf[r];
            rt.tx.r = (r + 1) % countof(rt.tx.buf);
        }
    }
}

static void flush_buf(void)
{
    uint w = rt.tx.w;
    uint r = rt.tx.r;

    while (r != w)
    {
        while (! (uart->SR & USART_SR_TXE));
        uart->DR = rt.tx.buf[r];
        r = (r + 1) % countof(rt.tx.buf);
    }

    rt.tx.r = r;
}


void HAL_tty_init(uint baud)
{
    NVIC_DisableIRQ(USART1_IRQn);

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // in case we're reiniting, flush buffer and wait for shift register to become empty
    if (uart->CR1 & USART_CR1_UE)
    {
        flush_buf();
        while (! (uart->SR & USART_SR_TC));
    }

    hal_pincfg_out_alt(GPIOA, 9);

    GPIOA->BSRR = 1 << 10;      // rx with pull-up
    hal_pincfg_in(GPIOA, 10);

    NVIC_SetPriority(USART1_IRQn, HAL_IRQ_PRIORITY_LOWEST);
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);

    uart->CR1 = 0;
    uart->CR2 = 0;
    uart->CR3 = 0;
    uart->BRR = (HAL_SYS_F_CPU + baud/2 - 1) / baud;
    uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_RXNEIE;
}


void HAL_tty_putc(u8 chr)
{
    if (is_isr_blocked())
    {
        // proceed in polling mode
        flush_buf();
        while (! (uart->SR & USART_SR_TC));
        uart->DR = chr;
    }
    else
    {
        uint w = rt.tx.w;
        uint new_w = (w + 1) % countof(rt.tx.buf);

        while (new_w == rt.tx.r)
        {
            //HAL_systimer_sleep(1000);
            ; // wait free space in the buffer. we shouldn't be locked forever.
        }

        // NOTE: smart compiler may rearrange writes to the buffer and w index.
        // so buffer is made volatile too
        rt.tx.buf[w] = chr;
        rt.tx.w = new_w;

        uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_RXNEIE; // enable all interrupts
    }
}

void HAL_tty_puts(const char *str)
{
    while (*str)
        HAL_tty_putc(*str++);
}

int HAL_tty_getc(void)
{
    uint r = rt.rx.r;
    if (r == rt.rx.w)
        return -1;
    rt.rx.r = (r + 1) % countof(rt.rx.buf);
    return rt.rx.buf[r];
}
