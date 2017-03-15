#include "syntax.h"
#include "stm32f10x.h"

#include "hal_sys.h"
#include "hal_beep.h"
#include "hal_sound.h"

#include "debug.h"

// simple sound sequencer for driving beeper

#define SOUND_SEQ_PRIORITY 6

typedef struct
{
    const hal_sound_note_t * volatile note;
    uint remaining_quarters;
} sound_rt_t;

static TIM_TypeDef * const timer = TIM7;

static sound_rt_t rt;

void HAL_sound_init(void)
{
    hal_beep_init();

    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    PANIC_IF(HAL_SYS_F_CPU / 1000 - 1 > 0xFFFF);

    timer->CR1 = 0;
    timer->CNT = 0;
    timer->PSC = HAL_SYS_F_CPU / 1000 - 1;          // prescale to 1 kHz to fit the timer resolution
    timer->ARR = 1000 / (HAL_SOUND_BPM * 4/60.);    // 120 bpm, semiquaver freq is 8 Hz
    timer->SR = 0;
    timer->DIER = TIM_DIER_UIE;

    #warning "refactor the priorities !"
    NVIC_SetPriority(TIM7_IRQn, SOUND_SEQ_PRIORITY);
    NVIC_ClearPendingIRQ(TIM7_IRQn);
    NVIC_EnableIRQ(TIM7_IRQn);
}

void timer7_isr(void)
{
    // NOTE: clearing early to give nvic a chance to sync (at least 2 cycles)
    timer->SR = 0;

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
        rt.note = NULL;
        hal_beep_off();
        timer->CR1 = 0;
    }
}

void HAL_sound_play(const hal_sound_note_t *seq)
{
    // attach new sequence and fire right now
    timer->CR1 = 0;
    rt.note = seq;
    timer->CNT = 0xFFFF;
    timer->CR1 = TIM_CR1_CEN;
}


void HAL_sound_stop(void)
{
    timer->CR1 = 0;
    rt.note = NULL;
    hal_beep_off();
}
