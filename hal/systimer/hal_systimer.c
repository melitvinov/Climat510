#include "syntax.h"
#include "stm32f10x.h"
#include "hal_priorities.h"
#include "hal_atomic.h"
#include "hal_sys.h"
#include "hal_systimer.h"

#include "debug.h"

// 1ms system timer. overflow period is about 50 days
// 16-bit timers tim2 (low) and tim3 (high) are chained to form a single 32-bit timer

// us sleep timer for short busy-waits is done with systick

static TIM_TypeDef * const lowtimer = TIM2;
static TIM_TypeDef * const hightimer = TIM3;
static SysTick_Type * const systick = SysTick;


void HAL_systimer_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM7EN;

    lowtimer->PSC = HAL_SYS_F_CPU / 1E3 - 1;      // 1 ms prescaler
    lowtimer->ARR = 0xFFFF;                       // full span (65.536 sec overflow)
    lowtimer->CR2 = (lowtimer->CR2 & ~TIM_CR2_MMS) | TIM_CR2_MMS_1; // generate edge on TRG0 upon overflow
    lowtimer->CNT = 0;

    hightimer->PSC = 0;
    hightimer->ARR = 0xFFFF;
    hightimer->SMCR |= TIM_SMCR_TS_0 | TIM_SMCR_SMS;    // clock on master edge
    hightimer->CNT = 0;

    lowtimer->EGR = TIM_EGR_UG;
    hightimer->EGR = TIM_EGR_UG;

    // go, slave timer starts first
    hightimer->CR1 = TIM_CR1_CEN;
    lowtimer->CR1 = TIM_CR1_CEN;

}

u32 HAL_systimer_get(void)
{
    while (1)
    {
        u32 high = hightimer->CNT;
        u32 low = lowtimer->CNT;
        if (hightimer->CNT == high)
            return (high << 16) | low;
    }
}

void HAL_systimer_sleep(u32 ms)
{
    u32 start = HAL_systimer_get();
    // NOTE: compare is unsigned to get correct result in face of overflow
    while ((HAL_systimer_get() - start) < ms);
}

void hal_systimer_sleep_us(u32 us)
{
    const u32 scale = HAL_SYS_F_CPU / 1E6;

    REQUIRE(us < ((1 << 24) / scale));

    // fire systick timer
    systick->LOAD = scale * us;
    systick->CTRL |= (1 << 2) | (1 << 0);    // enable count (running on a processor clock)

    while (! (systick->CTRL & (1 << 16)));

    systick->CTRL = 0;
}
