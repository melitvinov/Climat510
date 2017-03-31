#ifndef _65_GD_H_
#define _65_GD_H_

#define SetBit(Val,NBit)        (Val |= (NBit))
#define ClrBit(Val,NBit)        (Val &=~(NBit))
#define YesBit(Val,NBit)        (Val & (NBit))

// XXX: isolation
#include "405_ConfigEnRuSR.h"
#include "control_abi.h"


typedef struct  eGData
{
    eHot            Hot;
    eControl        Control;
    eTimer          Timers[cSTimer];
    eConstMech      ConstMechanic[NZONES];
    eStrategy       Strategy[NZONES][cSStrategy];
    eMechConfig     MechConfig[NZONES];
    eTuneClimate    TuneClimate;
    eTControl       TControl;
    eLevel          Level;
} eGData;


typedef struct
{
    int idx;
    eZone *hot;
    eMechanic *hand;
    eTControlZone *tcontrol_tepl;
    eZoneControl *gh_ctrl;
    eTControl *tctrl;
    const eMechConfig *mech_cfg;
    const eConstMech *const_mech;
    const eStrategy *strategies;
} zone_t;

typedef struct
{
    int cidx;
    eKontur *hot;
    eTControlKontur *tcontrol;
    eMechanic *hand;
    const eStrategy *strategy;
    zone_t link;
} contour_t;

#ifdef _FRIEND_OF_CONTROL_
extern eGData _GD;
#endif

const eGData *gd(void);
eGData *gd_rw(void);

#endif
