#include "syntax.h"
#include "control_gd.h"

eGData _GD;
gdp_t _GDP;

// blobs in remote reporting
caldata_t caldata;

sens_t sensdata;

const eGData *gd(void)
{
    return &_GD;
}

eGData *gd_rw(void)
{
    return &_GD;
}
