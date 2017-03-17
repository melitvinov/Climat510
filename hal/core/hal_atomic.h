#ifndef __HAL_ATOMIC_H__
#define __HAL_ATOMIC_H__

#include "stm32f10x.h"
#include "hal_priorities.h"

static inline u32 __set_basepri(u32 new_prior)
{
    u32 old_prior;
    new_prior <<= 8 - __NVIC_PRIO_BITS;
    __asm__ volatile ("mrs  %0, basepri_max \n\t"
                      "msr  basepri_max, %1     \n\r"
                      : "=&r" (old_prior) : "r" (new_prior));
    return old_prior;
}

static inline void __restore_basepri(u32 *old_prior)
{
    u32 new_prior = *old_prior;
    __asm__ volatile ("msr basepri, %0" : : "r" (new_prior));
}

// rise priority of code block to specified level. All exceptions with lower and same priority will temporaly disabled
#define HAL_BLOCK_INTERRUPTS_LEQ_TO(prior)    for (u32 __updstate __cleanup(__restore_basepri) = __set_basepri((prior)), __todo = 1; __todo; __todo = 0)

#endif
