#ifndef _HAL_TIMER_H_
#define _HAL_TIMER_H_

void HAL_systimer_init(void);

u32 HAL_systimer_get(void);
void HAL_systimer_sleep(u32 ms);
void hal_systimer_sleep_us(u32 us);

#ifdef _HAL_
// hal-private

#define HAL_SYSTIMER_FREQ   1000
#define HAL_SYSTIMER_IRQ_PRIORITY   HAL_IRQ_PRIORITY_NORMAL

typedef struct hal_systimer_task_t hal_systimer_task_t;

struct hal_systimer_task_t
{
    hal_systimer_task_t *next;
    void (*handler)(void);
};

void hal_systimer_add_task(hal_systimer_task_t *task, void (*handler)(void));
void hal_systimer_remove_task(hal_systimer_task_t *task);

#endif
#endif
