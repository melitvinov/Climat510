#ifndef _HAL_BEEP_H_
#define _HAL_BEEP_H_

void hal_beep_init(void);

void hal_beep_on(u32 hz);
void hal_beep_off(void);

#endif
