#include "syntax.h"
#include "control_gd.h"

eGData _GD;

const eGData *gd(void)
{
    return &_GD;
}

eGData *gd_rw(void)
{
    return &_GD;
}
