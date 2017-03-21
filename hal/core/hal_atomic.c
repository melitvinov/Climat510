#include "syntax.h"
#include "stm32f10x.h"
#include "hal_atomic.h"

u32 __set_basepri(u32 new_prior)
{
    u32 old_prior;
    new_prior <<= 8 - __NVIC_PRIO_BITS;
    __asm__ volatile ("mrs  %0, basepri_max \n\t"
                      "msr  basepri_max, %1     \n\r"
                      : "=&r" (old_prior) : "r" (new_prior));
    return old_prior;
}

void __restore_basepri(u32 *old_prior)
{
    u32 new_prior = *old_prior;
    __asm__ volatile ("msr basepri, %0" : : "r" (new_prior));
}

