#ifndef _HAL_SOUND_H_
#define _HAL_SOUND_H_

//NOTE: each note is a semiquaver

#define HAL_SOUND_BPM 120

typedef struct
{
    u16 freq;
    u16 dur;
} hal_sound_note_t;

void HAL_sound_init(void);
void HAL_sound_play(const hal_sound_note_t *seq);
void HAL_sound_stop(void);

#endif
