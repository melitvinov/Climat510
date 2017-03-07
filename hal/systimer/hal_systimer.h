#ifndef _HAL_TIMER_H_
#define _HAL_TIMER_H_

void HAL_systimer_init(void);
u32 HAL_systimer_get(void);
void HAL_systimer_sleep(u32 ms);
void hal_systimer_sleep_us(u32 us);

#endif
