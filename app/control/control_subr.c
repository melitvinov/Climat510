#define _FRIEND_OF_CONTROL_

#include "syntax.h"


#include "defs.h"
#include "main_consts.h"

#include "control_const.h"
// XXX: dirty, but ok for now
#include "control_const.c"

#include "control_gd.h"
#include "control.h"
#include "control_subr.h"

extern int8_t  bWaterReset[16];

static int16_t teplTmes[8][6];


static int16_t getTempSensor(const zone_t *zone, int fnTepl, int sensor)
{
    if (zone->hot->IndoorSensors[sensor].RCS  ==  0)
    {
        teplTmes[fnTepl][sensor] = zone->hot->IndoorSensors[sensor].Value;
        return zone->hot->IndoorSensors[sensor].Value;
    }

    if (zone->hot->IndoorSensors[sensor].Value  ==  0)
        return 0;
    return teplTmes[fnTepl][sensor];
}

/*!
\brief Температура воздуха для вентиляци в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t getTempVent(const zone_t *zone, int fnTepl)
{
    int16_t error = 0;
    int16_t temp = 0;
    int16_t i;
    int8_t calcType = 0;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    char averageCount = 0;
    calcType = _GD.Control.Zones[fnTepl].sensT_vent >> 6;
    mask = _GD.Control.Zones[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if (((mask >> i) & 1) && (getTempSensor(zone, fnTepl, i)))
        {
            temp = getTempSensor(zone, fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else
        {
            maskN = maskN >> 1;
        }
    }
    average = average / averageCount;
    if (error)
    {
        _GD.Hot.Zones[fnTepl].tempParamVent=maskN+(calcType<<6); //GD.Control.Tepl[fnTepl].sensT_vent;
        _GD.Hot.Zones[fnTepl].tempVent = average;
        if (calcType & 1)
            _GD.Hot.Zones[fnTepl].tempVent = min;
        if ((calcType >> 1) & 1)
            _GD.Hot.Zones[fnTepl].tempVent = max;
        return _GD.Hot.Zones[fnTepl].tempVent;
    }
}

/*!
\brief Температура воздуха для обогрева в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/

int16_t getTempHeat(const zone_t *zone, int fnTepl)
{
    int16_t error = 0;
    int16_t temp = 0;
    int16_t i;
    int8_t calcType = 0;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    int8_t averageCount = 0;
    calcType = _GD.Control.Zones[fnTepl].sensT_heat >> 6;
    mask = _GD.Control.Zones[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if (((mask >> i) & 1) && (getTempSensor(zone, fnTepl, i)))
        {
            temp = getTempSensor(zone, fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else maskN = (maskN >> 1);
    }
    average = average / averageCount;
    if (error)
    {
        _GD.Hot.Zones[fnTepl].tempParamHeat=maskN+(calcType<<6);
        _GD.Hot.Zones[fnTepl].tempHeat = average;
        if (calcType & 1)
            _GD.Hot.Zones[fnTepl].tempHeat = min;
        if ((calcType >> 1) & 1)
            _GD.Hot.Zones[fnTepl].tempHeat = max;
        return _GD.Hot.Zones[fnTepl].tempHeat;
    }
}

/*!
\brief Авария датчика температуры воздуха вентиляции в зависимости от выбранного значение в Параметрах управления
*/
int8_t getTempVentAlarm(const zone_t *zone, int fnTepl)
{
    int16_t temp = 0;
    int16_t i;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    int8_t averageCount = 0;
    mask = _GD.Control.Zones[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    for (i=0;i<6;i++)
    {
        if (((mask >> i) & 1) && (getTempSensor(zone, fnTepl, i)))
        {
            temp = getTempSensor(zone, fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            maskN = (maskN >> 1) + 32;
        }
        else
        {
            maskN = (maskN >> 1);
        }
    }
    return maskN;
}

/*!
\brief Авария датчика температуры воздуха обогрева в зависимости от выбранного значение в Параметрах управления
*/

int8_t getTempHeatAlarm(const zone_t *zone, int fnTepl)
{
    int16_t temp = 0;
    int16_t i;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    int8_t averageCount = 0;
    mask = _GD.Control.Zones[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    for (i=0;i<6;i++)
    {
        if (((mask >> i) & 1) && (getTempSensor(zone, fnTepl, i)))
        {
            temp = getTempSensor(zone, fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            maskN = (maskN >> 1) + 32;
        }
        else
        {
            maskN = (maskN >> 1);
        }
    }
    return maskN;
}


bool SameSign(int Val1,int Val2)
{
    if (((Val1>0)&&(Val2>0))
        || ((Val1<0)&&(Val2<0))) return 1;
    else
        return 0;
}

void MidlWindAndSr(void)
{
    _GD.TControl.SumSun+=((long int)_GD.TControl.MeteoSensing[cSmFARSens]);
    _GD.TControl.MidlSR=((((long int)_GD.TControl.MidlSR)*(1000-o_MidlSRFactor))/1000
                        +((long int)_GD.TControl.MeteoSensing[cSmFARSens])*o_MidlSRFactor);
    _GD.Hot.MidlSR=(int)(_GD.TControl.MidlSR/1000);
    if (GetMeteoSensorConfig(cSmFARSens))
    {
        _GD.Hot.SumSun=(int)((_GD.TControl.SumSun*6)/1000);
    }
    _GD.Hot.MidlWind=(int)((((long int)_GD.Hot.MidlWind)*(1000-o_MidlWindFactor)+((long int)_GD.TControl.MeteoSensing[cSmVWindSens])*o_MidlWindFactor)/1000);

}

void CheckMidlSr(void)
{
    if (GetMeteoSensorConfig(cSmFARSens))
    {
        _GD.Hot.SumSun=(int)((_GD.TControl.SumSun*6)/1000);
    }
    _GD.Hot.MidlSR=(int)(_GD.TControl.MidlSR/1000);
}

char CheckSeparate (const contour_t *ctx)
{
    char t2;
    char t1;
    ctx->tcontrol->NAndKontur=0;
    if (! ctx->link.mech_cfg->RNum[ctx->cidx])
        return 0;
    t1 = 0;
    for (t2 = 0;t2<NZONES;t2++)
    {
        if (_GD.MechConfig[t2].RNum[ctx->cidx] == ctx->link.mech_cfg->RNum[ctx->cidx])
        {
            t1 |= (1<<t2);
            ctx->tcontrol->NAndKontur++;
        }
    }
    return t1;
}

char CheckMain(const contour_t *ctr)
{
    int zone_idx = 0;
    while (zone_idx < ctr->link.idx)
    {
        if ( (ctr->tcontrol->Separate >> zone_idx) & 1)
            return zone_idx;
        zone_idx++;
    }
    return ctr->link.idx;
}

void InitControl(void)
{
    #warning "this one is fucked. we're cleaning too much"
    memclr(&_GD.Control,sizeof(eControl)
           +sizeof(eLevel)
           +sizeof(eTimer)*cSTimer);

    #warning "this one is fucked. we're cleaning too much"
    memclr(&_GD.ConstMechanic[0],sizeof(eTuneClimate)+sizeof(eTControl)+sizeof(eStrategy)*cSStrategy*NZONES+sizeof(eConstMech)*NZONES+sizeof(eMechConfig)*NZONES);

    // default datetime
    _GD.Hot.Year=01;
    _GD.Hot.Date=257;
    _GD.Hot.Time=8*60;

    #warning "sizeof is fucked"
    // XXX: this sizeof is fucked, TStart[i] is fucked too
    for (uint zone_idx  =0; zone_idx< sizeof(NameConst)/3; zone_idx++)
        _GD.TuneClimate.s_TStart[zone_idx] = NameConst[zone_idx].StartZn;


    for (int zone_idx=0;zone_idx<NZONES;zone_idx++)
    {
        #warning "looks like this is fucked too"
        eStrategy * Strategy_Tepl= _GD.Strategy[zone_idx];

        for (int int_x=0;int_x<cSStrategy;int_x++)
        {
            for (uint byte_y=0;byte_y<sizeof(eStrategy);byte_y++)
                (*((&(Strategy_Tepl[int_x].TempPower))+byte_y))=(*((&DefStrategy[int_x].TempPower)+byte_y));
        }

        bWaterReset[zone_idx]=1;

        eMechConfig *mech_cfg = &_GD.MechConfig[zone_idx];

        for (int int_x=0;int_x<SUM_NAME_CONF;int_x++)
            mech_cfg->RNum[int_x]=MechC[zone_idx][int_x];

        for (int int_x=0;int_x<cConfSSystem;int_x++)
            mech_cfg->Systems[int_x]=InitSystems[zone_idx][int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eTControlZone *dst = &_GD.TControl.Zones[zone_idx];

            //pGD_TControl_Tepl->MechBusy[IntX].BlockRegs=1;
            dst->MechBusy[int_x].PauseMech=300;
            dst->MechBusy[int_x].Sens=0;
        }

        eZoneControl *ctrl = &_GD.Control.Zones[zone_idx];

        #warning "sizeof is fucked"
        //PANIC_IF(countof(ctrl->c_MaxTPipe) != countof(DefControl));

        for (uint int_x=0;int_x<(sizeof(DefControl)/2);int_x++)
            ctrl->c_MaxTPipe[int_x]=DefControl[int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eConstMech *dst = &_GD.ConstMechanic[zone_idx];

            dst->ConstMixVal[int_x].v_TimeMixVal=DefMechanic[0];
            dst->ConstMixVal[int_x].v_MinTim=(char)DefMechanic[3];
            dst->ConstMixVal[int_x].v_PFactor=DefMechanic[1];
            dst->ConstMixVal[int_x].v_IFactor=DefMechanic[2];
            //pGD_ConstMechanic->ConstMixVal[IntX].Power=(char)DefMechanic[3];
        }
    }
}


int CorrectionRule(int fStartCorr,int fEndCorr, int fCorrectOnEnd, int fbSet, int creg_y, int *reg_z)
{
    if (creg_y <= fStartCorr ||fStartCorr == fEndCorr)
    {
        *reg_z = 0;
        return 0;
    }
    if (creg_y > fEndCorr)
        *reg_z = fCorrectOnEnd;
    else
        *reg_z = (int)((((long)(creg_y - fStartCorr))*fCorrectOnEnd)/(fEndCorr-fStartCorr));
    return fbSet;
}

void WindDirect(void)
{
    _GD.Hot.PozFluger &= 1;
    if (_GD.TuneClimate.o_TeplPosition == 180)
    {
        _GD.Hot.PozFluger=0;
        return;
    }
    if (_GD.Hot.MidlWind<_GD.TuneClimate.f_WindStart) return;
    int creg_z = _GD.TControl.MeteoSensing[cSmDWindSens]+_GD.TuneClimate.o_TeplPosition;
    creg_z %= 360;

    const zone_t first_gh = make_zone_ctx(0);

    first_gh.tcontrol_tepl->CurrPozFluger = _GD.Hot.PozFluger;
    if (   (!_GD.Hot.PozFluger)
        && (creg_z >(90+f_DeadWindDirect))
        && (creg_z<(270-f_DeadWindDirect)))
    {
        first_gh.tcontrol_tepl->CurrPozFluger=1;
    }

    if (   (_GD.Hot.PozFluger)
        && ((creg_z <(90-f_DeadWindDirect)) ||(creg_z>(270+f_DeadWindDirect))))
        first_gh.tcontrol_tepl->CurrPozFluger=0;

    if (_GD.Hot.PozFluger != first_gh.tcontrol_tepl->CurrPozFluger)
    {
        first_gh.tcontrol_tepl->CurrPozFlugerTime++;
        if (first_gh.tcontrol_tepl->CurrPozFlugerTime < 5)
            return;
        _GD.Hot.PozFluger = first_gh.tcontrol_tepl->CurrPozFluger;
    }
    first_gh.tcontrol_tepl->CurrPozFlugerTime = 0;
}
