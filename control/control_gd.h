#ifndef _65_GD_H_
#define _65_GD_H_

#define SetBit(Val,NBit)        (Val |= (NBit))
#define ClrBit(Val,NBit)        (Val &=~(NBit))
#define YesBit(Val,NBit)        (Val & (NBit))

// XXX: isolation
#include "405_ConfigEnRuSR.h"

#include "control_screen.h"

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
    eCalSensor  InTeplSens[cSTepl][cConfSSens];
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
    eConstMech      ConstMechanic[cSTepl];
    eStrategy       Strategy[cSTepl][cSStrategy];
    eMechConfig     MechConfig[cSTepl];
    eTuneClimate    TuneClimate;
    eTControl       TControl;
    eLevel          Level;
    // NOT blobs in remote reporting
    eFanBlock FanBlock[cSTepl][2];
} eGData;

typedef struct
{
    eFullCal        Cal;
} caldata_t;

typedef struct
{
    int16_t         uInTeplSens[cSTepl][cConfSSens];
    int16_t         uMeteoSens[cConfSMetSens];
} sens_t;

typedef struct
{
    eTepl *Hot_Tepl;
    eMechanic *Hot_Hand;
    eTControlTepl *TControl_Tepl;
    eTeplControl *Control_Tepl;
    eConstMech *ConstMechanic;
    eMechConfig *MechConfig;
    eSensLevel *Level_Tepl;
    eStrategy *Strategy_Tepl;
    eKontur *Hot_Tepl_Kontur;
    eTControlKontur *TControl_Tepl_Kontur;
    eMechanic *Hot_Hand_Kontur;
    eStrategy *Strategy_Kontur;
    uint16_t *MechConfig_Kontur;
    eConstMixVal *ConstMechanic_Mech;
} gdp_t;

extern eGData GD;
extern gdp_t gdp;
// XXX: caldata do not relate to 65 in fact
extern caldata_t caldata;
extern sens_t sensdata;

#endif
