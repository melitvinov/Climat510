#ifndef _65_GD_H_
#define _65_GD_H_

#define SetBit(Val,NBit)        (Val |= (NBit))
#define ClrBit(Val,NBit)        (Val &=~(NBit))
#define YesBit(Val,NBit)        (Val & (NBit))

// XXX: isolation
#include "405_ConfigEnRuSR.h"
#include "control_abi.h"

#pragma pack(1)


typedef struct
{
    uint8_t     Type;
    uint8_t     nInput;
    uint8_t     Output;
    uint8_t     Corr;
    uint16_t        U0;
    uint16_t        V0;
    uint16_t        U1;
    uint16_t        V1;

}eCalSensor;

// blobs in remote reporting
typedef struct
{
    eCalSensor  InTeplSens[NZONES][cConfSSens];
    // blobs in remote reporting
    eCalSensor  MeteoSens[cConfSMetSens];
} eFullCal;

/*-----------------------------------
                        Допуски
------------------------------------*/

/*typedef struct
    {
        int16_t				SumD;
        int16_t				ChangeD;
        int16_t				LastChangeD;
        int16_t				LastTAir;
        int16_t				Past3D;
        int16_t				Past2D;
        int16_t				Past1D;
        int16_t				Past3DTDo;
        int16_t				Past2DTDo;
        int16_t				Past1DTDo;

    } eSensorD;	*/



#define MAX_FAN_COUNT	64

typedef struct
{
    uint8_t         Actual;
    uint16_t        Cond;
    uint16_t        ActualSpeed;
    uint16_t        ActualPower;
    uint16_t        ActualPWM;

} eFanData;


typedef struct
{
    uint16_t        Speed;
    uint8_t         NFans;
    eFanData        FanData[MAX_FAN_COUNT];

} eFanBlock;


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
    // NOT blobs in remote reporting
    eFanBlock FanBlock[NZONES][2];
} eGData;

typedef struct
{
    eFullCal        Cal;
} caldata_t;

typedef struct
{
    int16_t         uInTeplSens[NZONES][cConfSSens];
    int16_t         uMeteoSens[cConfSMetSens];
} sens_t;

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
    const eFanBlock *fanblock;
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

// XXX: caldata do not relate to 65 in fact
extern caldata_t caldata;
extern sens_t sensdata;

const eGData *gd(void);
eGData *gd_rw(void);

#endif
