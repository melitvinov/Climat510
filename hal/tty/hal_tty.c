#define _HAL_TTY_C_

#include "syntax.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#include "hal_tty.h"

// bit-bang uart 9600-n-1 on pb7 (i2c1_sda_master)

typedef struct
{
    u16 shiftreg;
    u8 buf[HAL_TTY_TX_BUF_SIZE];
    volatile uint r;
    volatile uint w;
} tty_rt_t;

static GPIO_TypeDef * const port = GPIOB;
static const uint pin_idx = 9;
static TIM_TypeDef * const timer = TIM4;

static tty_rt_t rt;

void HAL_tty_init(void)
{
    RCC->APB2ENR |= RCC_APB2Periph_GPIOB;
//  if (pin_idx < 8)
//      port->CRL |= 1 << (pin_idx * 4);    // output, 10 MHz
//  else
//      port->CRH |= 1 << ((pin_idx - 8) * 4);    // output, 10 MHz
    port->CRH = (port->CRH & 0xF0) | 0x10;
    port->BSRR = 1 << pin_idx;

    RCC->APB1ENR |= RCC_APB1Periph_TIM4;

    // APB1 freq is 36 MHz

    timer->PSC = 0; // 36 MHz
    timer->ARR = 36E6/9600./4.5;
    timer->SR = 0;
    timer->DIER = TIM_DIER_UIE;
    timer->EGR = TIM_EGR_UG;

    #warning "refactor the priorities !"
    NVIC_SetPriority(TIM4_IRQn, 1);
    NVIC_ClearPendingIRQ(TIM4_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);

    rt.shiftreg = 0;
}

void HAL_tty_putc(u8 chr)
{
    #warning "detect if interrupts are disabled and proceed in polling mode"

    uint w = rt.w;
    uint new_w = (w + 1) % countof(rt.buf);
    while (new_w == rt.r);      // wait for free place in buffer

    rt.buf[w] = chr;            // put byte on hold in buffer
    rt.w = new_w;

    timer->CR1 = TIM_CR1_CEN;
}

void HAL_tty_puts(const char *str)
{
    while (*str)
        HAL_tty_putc(*str++);
}

static volatile bool is_inside = 0;


void TIM4_IRQHandler(void)
{
    if (rt.shiftreg != 0)
    {
        if (rt.shiftreg & 0x01)
            port->BSRR = 1 << pin_idx;
        else
            port->BRR = 1 << pin_idx;
        rt.shiftreg >>= 1;
    }
    else
    {
        if (rt.w == rt.r)   // buf is empty, stop
        {
            timer->CR1 = 0;
        }
        else
        {
            uint r = (rt.r + 1) % countof(rt.buf);
            rt.r = r;
            uint chr = rt.buf[r];
            rt.shiftreg = 0x0200 | (chr << 1);  // embed start bit, stop bit
        }
    }

    // NOTE: lame stm is seems to be slow clearing nvic
    // so the nops are for avoiding the dupe interrupt
    #warning "what if timers are running extra fast ? some error with the pll setup ?"
    timer->SR = 0;
    asm volatile ("dmb":::);
    asm volatile ("dmb":::);
//  asm volatile ("nop":::);
}
