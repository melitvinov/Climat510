#include "syntax.h"
#include "stm32f10x.h"

#include "hal_sys.h"
#include "hal_priorities.h"
#include "hal_fastloop.h"

#include "debug.h"

typedef struct
{
    hal_fasttask_t *head;
} fastloop_rt_t;


static TIM_TypeDef * const timer = TIM7;

static fastloop_rt_t rt;

void timer7_isr(void)
{
    timer->SR = 0;
    for (hal_fasttask_t *t = rt.head; t; t = t->next)
        t->handler();
}

static void fastloop_init(void)
{
    static bool is_inited = 0;
    if (is_inited)
        return;

    is_inited = 1;

    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    timer->CR1 = 0;
    timer->CNT = 0;
    timer->PSC = 0;
    timer->ARR = HAL_SYS_F_CPU / HAL_FASTLOOP_FREQ;  // 1 KHz
    timer->SR = 0;

    NVIC_SetPriority(TIM7_IRQn, HAL_FASTLOOP_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(TIM7_IRQn);
    NVIC_EnableIRQ(TIM7_IRQn);
}


static uint task_in_list(const hal_fasttask_t *task)
{
    for (hal_fasttask_t *t = rt.head; t; t = t->next)
        if (t == task) return 1;
    return 0;
}

void hal_fastloop_add_task(hal_fasttask_t *task, void (*handler)(void))
{
    fastloop_init();

    timer->DIER = 0;

    const hal_fasttask_t *orig_head = rt.head;
    task->handler = handler;
    if (! task_in_list(task))
    {
        task->next = rt.head;
        rt.head = task;
    }

    if (! orig_head)
        timer->CR1 = TIM_CR1_CEN;

    timer->DIER = TIM_DIER_UIE;
}

// NOTE: task may remove itself from list, but may not remove other tasks
void hal_fastloop_remove_task(hal_fasttask_t *task)
{
    timer->DIER = 0;

    const hal_fasttask_t *orig_head = rt.head;
    if (rt.head == task)
    {
        rt.head = task->next;
    }
    else
    {
        for (hal_fasttask_t *t = rt.head; t; t = t->next)
        {
            if (t->next == task)    // Found previous connection, relink
            {
                t->next = task->next;
                break;
            }
        }
    }
    if (orig_head && (! rt.head))
        timer->CR1 = 0;

    timer->DIER = TIM_DIER_UIE;
}
