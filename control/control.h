#ifndef _65_CONTROL_H_
#define _65_CONTROL_H_

#include "control_gd.h"

typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} control_regs_t;

extern control_regs_t creg;

void Control_pre(void);
void Control_post(int second, bool is_transfer_in_progress);

#endif
