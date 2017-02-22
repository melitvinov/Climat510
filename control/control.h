#ifndef _65_CONTROL_H_
#define _65_CONTROL_H_

#include "control_gd.h"

#ifdef _FRIEND_OF_CONTROL_
typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} control_regs_t;

extern control_regs_t creg;
#endif


void control_init(void);
void control_pre(void);
void control_post(int second, bool is_transfer_in_progress);

#endif
