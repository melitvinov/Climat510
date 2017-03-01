#define _HAL_TTY_C_

#include "syntax.h"
#include "stm32f10x.h"

#include "hal_tty.h"
#include "hal_sys.h"

// bit-bang uart 9600-n-1 on pb7 (i2c1_sda_master)

typedef struct
{
    volatile u16 shiftreg;
    u8 buf[HAL_TTY_TX_BUF_SIZE];
    volatile uint r;
    volatile uint w;
} tty_rt_t;

static GPIO_TypeDef * const port = GPIOB;
static const uint pin_idx = 9;
static TIM_TypeDef * const timer = TIM4;

static tty_rt_t rt;

#define TTY_PRIORITY    1

void HAL_tty_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
//  if (pin_idx < 8)
//      port->CRL |= 1 << (pin_idx * 4);    // output, 10 MHz
//  else
//      port->CRH |= 1 << ((pin_idx - 8) * 4);    // output, 10 MHz
    port->CRH = (port->CRH & 0xF0) | 0x10;
    port->BSRR = 1 << pin_idx;

    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    timer->PSC = 0; // 1:1 prescaler
    timer->ARR = HAL_SYS_F_CPU / 9600.;
    timer->SR = 0;
    timer->DIER = TIM_DIER_UIE;
    timer->EGR = TIM_EGR_UG;

    #warning "refactor the priorities !"
    NVIC_SetPriority(TIM4_IRQn, TTY_PRIORITY);
    NVIC_ClearPendingIRQ(TIM4_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);

    rt.shiftreg = 0;
    rt.r = 0;
    rt.w = 0;
}

void timer4_handler(void)
{
    uint shiftreg = rt.shiftreg;
    if (shiftreg != 0)
    {
        if (shiftreg & 0x01)
            port->BSRR = 1 << pin_idx;
        else
            port->BRR = 1 << pin_idx;
        rt.shiftreg = shiftreg >> 1;
    }
    else
    {
        if (rt.w == rt.r)   // buf is empty, stop
        {
            timer->CR1 = 0;
        }
        else
        {
            uint r = rt.r;
            uint chr = rt.buf[r];
            rt.r = (r + 1) % countof(rt.buf);
            rt.shiftreg = 0x0200 | (chr << 1);  // embed start bit, stop bit
        }
    }

    // NOTE: lame stm is seems to be slow clearing nvic
    // so the nops are for avoiding the dupe interrupt
    timer->SR = 0;
    asm volatile ("dmb":::);
    asm volatile ("dmb":::);
}

void HAL_tty_putc(u8 chr)
{
    u32 primask;
    u32 basepri;
    __asm__ volatile("mrs %0, primask" : "=r" (primask));
    __asm__ volatile ("MRS  %0, basepri_max" : "=r" (basepri));
    basepri >>= 8 - __NVIC_PRIO_BITS;

    if (unlikely((primask & 0x01) || (basepri && basepri <= TTY_PRIORITY))) // we're running with disabled interrupts. proceed in polling mode
    {
        // flush
        while (rt.shiftreg)
        {
            timer->CR1 = TIM_CR1_CEN;
            while (! (timer->SR & TIM_SR_UIF));
            timer4_handler();
        }

        uint w = rt.w;
        uint new_w = (w + 1) % countof(rt.buf);
        rt.buf[w] = chr;            // put byte on hold in buffer
        rt.w = new_w;

        do
        {
            timer->CR1 = TIM_CR1_CEN;
            while (! (timer->SR & TIM_SR_UIF));
            timer4_handler();
        }
        while (rt.shiftreg);
    }
    else
    {
        uint w = rt.w;
        uint new_w = (w + 1) % countof(rt.buf);
        while (new_w == rt.r);      // wait for free place in buffer

        rt.buf[w] = chr;            // put byte on hold in buffer
        rt.w = new_w;

        timer->CR1 = TIM_CR1_CEN;
    }
}

void HAL_tty_puts(const char *str)
{
    while (*str)
        HAL_tty_putc(*str++);
}
