#ifndef _HAL_FASTLOOP_H_
#define _HAL_FASTLOOP_H_

// 1 kHz, 1ms period
#define HAL_FASTLOOP_FREQ   1000
#define HAL_FASTLOOP_IRQ_PRIORITY   (HAL_IRQ_PRIORITY_NORMAL + 1)   // just below normal priority

typedef struct hal_fasttask_t hal_fasttask_t;

struct hal_fasttask_t
{
    hal_fasttask_t *next;
    void (*handler)(void);
};

void hal_fastloop_add_task(hal_fasttask_t *task, void (*handler)(void));
void hal_fastloop_remove_task(hal_fasttask_t *task);


#endif
