#define _TIMERS_C_

#include "syntax.h"
#include "defs.h"
#include "hal.h"
#include "timers.h"
#include "debug.h"

#define F_IS_PERIODIC   0x01

static timer_t *head = NULL;

static uint is_timer_in_list(const timer_t *tmr)
{
    for (timer_t *t = head; t; t = t->next)
        if (t == tmr) return 1;
    return 0;
}

// it's expected the timer is fully populated
static void insert_timer(timer_t *tmr)
{
    // find the timer which should fire after us
    timer_t *prev = NULL;
    for (timer_t *t = head; t; t = t->next)
    {
        s32 later = tmr->deadline - t->deadline;
        if (later < 0)
            break;
        prev = t;
    }

    // we're the first
    if (! prev)
    {
        tmr->next = head;
        head = tmr;
    }
    else
    {
        tmr->next = prev->next;
        prev->next = tmr;
    }
}

static void remove_timer(timer_t *tmr)
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

void timer_start(timer_t *tmr, uint timeout, uint is_periodic, void (*callback)(timer_t *))
{
    REQUIRE((int)tmr->timeout >= 0);

    remove_timer(tmr);

    tmr->timeout = timeout;
    tmr->flags = is_periodic ? F_IS_PERIODIC : 0;
    tmr->callback = callback;
    tmr->deadline = HAL_systimer_get() + timeout;

    insert_timer(tmr);
}

// timer should be in list - it is not checked
void timer_restart(timer_t *tmr)
{
    remove_timer(tmr);
    tmr->deadline = HAL_systimer_get() + tmr->timeout;
    insert_timer(tmr);
}

uint timer_is_started(const timer_t *tmr)
{
    return is_timer_in_list(tmr);
}

void timer_stop(timer_t *tmr)
{
    remove_timer(tmr);
}

void timers_process(void)
{
    timer_t *t = head;

    if (! t)
        return;

    u32 systime = HAL_systimer_get();
    s32 remain = t->deadline - systime;

    if (remain > 0)
        return;

    remove_timer(t);

    if (t->flags & F_IS_PERIODIC)
    {
        t->deadline = systime + t->timeout;
        insert_timer(t);
    }

    if (t->callback)
        t->callback(t);
}
