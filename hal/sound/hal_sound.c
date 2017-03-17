#define _HAL_

#include "syntax.h"

#include "hal_sys.h"
#include "hal_atomic.h"
#include "hal_systimer.h"

#include "hal_beep.h"
#include "hal_sound.h"

#include "debug.h"

// simple sound sequencer for driving beeper

typedef struct
{
    hal_systimer_task_t task;

    const hal_sound_note_t * volatile note;
    uint remaining_quarters;
    uint remaining_ms;
} sound_rt_t;

static sound_rt_t rt;

void HAL_sound_init(void)
{
    hal_beep_init();
}

static void sequencer(void)
{
    if (rt.remaining_ms && --rt.remaining_ms)
        return;

    rt.remaining_ms = HAL_SYSTIMER_FREQ / (HAL_SOUND_BPM * 4/60.);    // 120 bpm, semiquaver freq is 8 Hz

    if (rt.remaining_quarters && --rt.remaining_quarters)
        return;

    const hal_sound_note_t *note = rt.note;

    if (note->dur)
    {
        rt.remaining_quarters = note->dur;
        hal_beep_on(note->freq);
        rt.note = note + 1;
    }
    else
    {
        hal_beep_off();
        hal_systimer_remove_task(&rt.task);
        rt.note = NULL;
    }
}

void HAL_sound_play(const hal_sound_note_t *seq)
{
    HAL_BLOCK_INTERRUPTS_LEQ_TO(HAL_SYSTIMER_IRQ_PRIORITY)
    {
        rt.note = seq;
        rt.remaining_ms = 0;
        hal_systimer_add_task(&rt.task, sequencer);
    }
}


void HAL_sound_stop(void)
{
    hal_systimer_remove_task(&rt.task);
    rt.note = NULL;
    hal_beep_off();
}

