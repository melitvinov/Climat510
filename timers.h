#ifndef _TIMERS_H_
#define _TIMERS_H_
typedef struct timer_t timer_t;

struct timer_t
{
    timer_t *next;
    u32 start_time;
    u32 timeout;
    void (*callback)(timer_t *me);
    u32 flags;
};

void timer_start(timer_t *tmr, uint timeout, uint is_periodic, void (*callback)(timer_t *));
void timer_restart(timer_t *tmr);
void timer_stop(timer_t *tmr);
uint timer_is_started(const timer_t *tmr);
void timers_process(void);

#endif
