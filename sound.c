#include "syntax.h"
#include "hal_systimer.h"
#include "hal_beep.h"
#include "timers.h"
#include "sound.h"

#include "debug.h"

// simple sound sequencer for driving beeper

typedef struct
{
    timer_t timer;
    const hal_sound_note_t *note;
} sound_rt_t;

static sound_rt_t rt;

void sound_init(void)
{
    hal_beep_init();
}

static void sequencer(timer_t *timer)
{
    const hal_sound_note_t *note = rt.note;

    if (note->dur)
    {
        hal_beep_on(note->freq);

        timer_start(&rt.timer, note->dur * (uint)(1E3 / (HAL_SOUND_BPM * 4/60.)), 0, sequencer);    // 120 bpm, semiquaver freq is 8 Hz
        rt.note = note + 1;
    }
    else
    {
        sound_stop();
    }
}

void sound_play(const hal_sound_note_t *seq)
{
    rt.note = seq;
    timer_start(&rt.timer, 0, 0, sequencer);    // start on a next cycle
}


void sound_stop(void)
{
    hal_beep_off();
    timer_stop(&rt.timer);
    rt.note = NULL;
}

