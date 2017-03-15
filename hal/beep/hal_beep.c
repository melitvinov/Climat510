#include "syntax.h"
#include "stm32f10x.h"
#include "hal_priorities.h"
#include "hal_pincfg.h"
#include "hal_sys.h"
#include "hal_beep.h"

// piezo beeper on pa4, 4 kHz resonant freq

static GPIO_TypeDef * const port = GPIOA;
static const uint pin_idx = 4;
static TIM_TypeDef * const timer = TIM6;

void hal_beep_init(void)
{
    hal_pincfg_out(port, pin_idx);

    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    timer->CR1 = 0;
    timer->CNT = 0;
    timer->PSC = 0; // 1:1 prescaler
    timer->ARR = HAL_SYS_F_CPU / 1000000. - 1;   // 1 MHz
    timer->SR = 0;
    timer->DIER = TIM_DIER_UIE;

    NVIC_SetPriority(TIM6_IRQn, HAL_IRQ_PRIORITY_NORMAL - 1);   // a little above normal
    NVIC_ClearPendingIRQ(TIM6_IRQn);
    NVIC_EnableIRQ(TIM6_IRQn);
}

void timer6_isr(void)
{
    // NOTE: clearing early to give nvic a chance to sync (at least 2 cycles)
    timer->SR = 0;

    static int is_high = 0;

    if (is_high)
        port->BSRR = 1 << (pin_idx + 16);
    else
        port->BSRR = 1 << pin_idx;

    is_high = ~is_high;
}

void hal_beep_on(u32 hz)
{
    if (hz == 0)
    {
        timer->CR1 = 0;
    }
    else
    {
        timer->PSC = ((1000000 / 2) + hz / 2) / hz;     // 1MHz / Hz rounded to nearest, 2x clock
        timer->CR1 = TIM_CR1_CEN;
    }
}

void hal_beep_off(void)
{
    hal_beep_on(0);
}
