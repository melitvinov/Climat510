#ifndef _HAL_ATOMIC_H_
#define _HAL_ATOMIC_H_

// rise priority of code block to specified level. All exceptions with lower and same priority will temporaly disabled
#define HAL_BLOCK_INTERRUPTS_LEQ_TO(prior)    for (u32 __updstate __cleanup(__restore_basepri) = __set_basepri((prior)), __todo = 1; __todo; __todo = 0)

u32 __set_basepri(u32 new_prior);
void __restore_basepri(u32 *old_prior);

#endif
