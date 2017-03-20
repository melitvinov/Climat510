#define _TIMERS_C_

#include "syntax.h"
#include "defs.h"
#include "hal_systimer.h"
#include "timers.h"
#include "debug.h"

#define F_IS_PERIODIC   0x01

static timer_t *head = NULL;

static uint timer_in_list(const timer_t *tmr)
{
    for (timer_t *t = head; t; t = t->next)
        if (t == tmr) return 1;
    return 0;
}

void timer_start(timer_t *tmr, uint timeout, uint is_periodic, void (*callback)(timer_t *))
{
    if (! timer_in_list(tmr))
    {
        tmr->next = head;
        head = tmr;
    }
    tmr->start_time = HAL_systimer_get();
    tmr->timeout = timeout;
    tmr->flags = is_periodic ? F_IS_PERIODIC : 0;
    tmr->callback = callback;
}

// timer should be in list - it is not checked
void timer_restart(timer_t *tmr)
{
    tmr->start_time = HAL_systimer_get();
}

uint timer_is_started(const timer_t *tmr)
{
    return timer_in_list(tmr);
}

void timer_stop(timer_t *tmr)
{
    if (tmr == head)
    {
        head = tmr->next;
    }
    else
    {
        for (timer_t *t = head; t; t = t->next)
        {
            if (t->next == tmr)    // Found previous connection, relink
            {
                t->next = tmr->next;
                break;
            }
        }
    }
}

static void process_single_timer(timer_t *tmr, u32 systime)
{
    if (systime - tmr->start_time > tmr->timeout)
    {
        if (tmr->flags & F_IS_PERIODIC)
        {
            tmr->start_time += tmr->timeout;
        }
        else
        {
            timer_stop(tmr);
        }
        if (tmr->callback)
            tmr->callback(tmr);
    }
}

void timers_process(void)
{
    u32 systime = HAL_systimer_get();
    for (timer_t *t = head; t; t = t->next)
        process_single_timer(t, systime);
}
