#ifndef _SOUND_H_
#define _SOUND_H_

//NOTE: each note is a semiquaver

#define HAL_SOUND_BPM 120

typedef struct
{
    u16 freq;
    u16 dur;
} hal_sound_note_t;

void sound_init(void);
void sound_play(const hal_sound_note_t *seq);
void sound_stop(void);

#endif
