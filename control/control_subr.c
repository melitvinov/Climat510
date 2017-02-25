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


static int16_t getTempSensor(const gh_t *gh, int fnTepl, int sensor)
{
    if (gh->hot->InTeplSens[sensor].RCS  ==  0)
    {
        teplTmes[fnTepl][sensor] = gh->hot->InTeplSens[sensor].Value;
        return gh->hot->InTeplSens[sensor].Value;
    }

    if (gh->hot->InTeplSens[sensor].Value  ==  0)
        return 0;
    return teplTmes[fnTepl][sensor];
}

/*!
\brief Температура воздуха для вентиляци в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t getTempVent(const gh_t *gh, int fnTepl)
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
    calcType = _GD.Control.Tepl[fnTepl].sensT_vent >> 6;
    mask = _GD.Control.Tepl[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        // XXX: shift has a less priority !
        if ((mask >> i & 1) && (getTempSensor(gh, fnTepl, i)))
        {
            temp = getTempSensor(gh, fnTepl, i);
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
            maskN = (maskN >> 1);
        }
    }
    average = average / averageCount;
    if (error)
    {
        _GD.Hot.Tepl[fnTepl].tempParamVent=maskN+(calcType<<6); //GD.Control.Tepl[fnTepl].sensT_vent;
        _GD.Hot.Tepl[fnTepl].tempVent = average;
        if (calcType & 1)
            _GD.Hot.Tepl[fnTepl].tempVent = min;
        if (calcType >> 1 & 1)
            _GD.Hot.Tepl[fnTepl].tempVent = max;
        return _GD.Hot.Tepl[fnTepl].tempVent;
    }
}

/*!
\brief Температура воздуха для обогрева в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/

int16_t getTempHeat(const gh_t *gh, int fnTepl)
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
    calcType = _GD.Control.Tepl[fnTepl].sensT_heat >> 6;
    mask = _GD.Control.Tepl[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(gh, fnTepl, i)))
        {
            temp = getTempSensor(gh, fnTepl, i);
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
        _GD.Hot.Tepl[fnTepl].tempParamHeat=maskN+(calcType<<6);
        _GD.Hot.Tepl[fnTepl].tempHeat = average;
        if (calcType & 1)
            _GD.Hot.Tepl[fnTepl].tempHeat = min;
        if (calcType >> 1 & 1)
            _GD.Hot.Tepl[fnTepl].tempHeat = max;
        return _GD.Hot.Tepl[fnTepl].tempHeat;
    }
}

/*!
\brief Авария датчика температуры воздуха вентиляции в зависимости от выбранного значение в Параметрах управления
*/
int8_t getTempVentAlarm(const gh_t *gh, int fnTepl)
{
    int16_t temp = 0;
    int16_t i;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    int8_t averageCount = 0;
    mask = _GD.Control.Tepl[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(gh, fnTepl, i)))
        {
            temp = getTempSensor(gh, fnTepl, i);
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

int8_t getTempHeatAlarm(const gh_t *gh, int fnTepl)
{
    int16_t temp = 0;
    int16_t i;
    int8_t mask = 0;
    int8_t maskN = 0;
    int16_t max = 0;
    int16_t min = 5000;
    int16_t average = 0;
    int8_t averageCount = 0;
    mask = _GD.Control.Tepl[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(gh, fnTepl, i)))
        {
            temp = getTempSensor(gh, fnTepl, i);
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
    if (GetMetSensConfig(cSmFARSens))
    {
        _GD.Hot.SumSun=(int)((_GD.TControl.SumSun*6)/1000);
    }
    _GD.Hot.MidlWind=(int)((((long int)_GD.Hot.MidlWind)*(1000-o_MidlWindFactor)+((long int)_GD.TControl.MeteoSensing[cSmVWindSens])*o_MidlWindFactor)/1000);

}

void CheckMidlSr(void)
{
    if (GetMetSensConfig(cSmFARSens))
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
    for (t2 = 0;t2<cSTepl;t2++)
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
    char tTepl;
    tTepl=0;
    while (tTepl < ctr->link.idx)
    {
        if ( (ctr->tcontrol->Separate >> tTepl) & 1)
            return tTepl;
        tTepl++;
    }
    return ctr->link.idx;
}

void InitGD(void)
{
    eCalSensor *eCS;

    #warning "this one is fucked. we're cleaning too much"
    memclr(&_GD.Control,sizeof(eControl)
           +sizeof(eFullCal)
           +sizeof(eLevel)
           +sizeof(eTimer)*cSTimer);

    #warning "this one is fucked. we're cleaning too much"
    memclr(&_GD.ConstMechanic[0],sizeof(eTuneClimate)+sizeof(eTControl)+sizeof(eStrategy)*cSStrategy*cSTepl+sizeof(eConstMech)*cSTepl+sizeof(eMechConfig)*cSTepl);


    #warning "meteo is not cleaned"
    memclr(&sensdata.uInTeplSens,sizeof(sensdata.uInTeplSens));

    // default datetime
    _GD.Hot.Year=01;
    _GD.Hot.Date=257;
    _GD.Hot.Time=8*60;

    #warning "sizeof is fucked"
    // XXX: this sizeof is fucked, TStart[i] is fucked too
    for (uint gh_idx  =0; gh_idx< sizeof(NameConst)/3; gh_idx++)
        _GD.TuneClimate.s_TStart[gh_idx] = NameConst[gh_idx].StartZn;

    for (int i=0; i<cConfSMetSens;i++)
    {
        eCS=&caldata.Cal.MeteoSens[i];
        eCS->V0=NameSensConfig[i+cConfSSens].vCal[0];
        eCS->V1=NameSensConfig[i+cConfSSens].vCal[1];
        eCS->U0=NameSensConfig[i+cConfSSens].uCal[0];
        eCS->U1=NameSensConfig[i+cConfSSens].uCal[1];
        eCS->Type=NameSensConfig[i+cConfSSens].TypeInBoard;
        eCS->Output=NameSensConfig[i+cConfSSens].Output;
        //eCS->Input=OutPortsAndInputs[ByteX][0];
        //eCS->nInput=OutPortsAndInputs[ByteX][1];
    }

    for (int gh_idx=0;gh_idx<cSTepl;gh_idx++)
    {
        #warning "looks like this is fucked too"
        eStrategy * Strategy_Tepl= _GD.Strategy[gh_idx];

        for (int int_x=0;int_x<cSStrategy;int_x++)
        {
            for (uint byte_y=0;byte_y<sizeof(eStrategy);byte_y++)
                (*((&(Strategy_Tepl[int_x].TempPower))+byte_y))=(*((&DefStrategy[int_x].TempPower)+byte_y));
        }

        bWaterReset[gh_idx]=1;

        eMechConfig *mech_cfg = &_GD.MechConfig[gh_idx];

        for (int int_x=0;int_x<SUM_NAME_CONF;int_x++)
            mech_cfg->RNum[int_x]=MechC[gh_idx][int_x];

        for (int int_x=0;int_x<cConfSSystem;int_x++)
            mech_cfg->Systems[int_x]=InitSystems[gh_idx][int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eTControlTepl *dst = &_GD.TControl.Tepl[gh_idx];

            //pGD_TControl_Tepl->MechBusy[IntX].BlockRegs=1;
            dst->MechBusy[int_x].PauseMech=300;
            dst->MechBusy[int_x].Sens=0;
        }

        eTeplControl *ctrl = &_GD.Control.Tepl[gh_idx];

        #warning "sizeof is fucked"
        //PANIC_IF(countof(ctrl->c_MaxTPipe) != countof(DefControl));

        for (uint int_x=0;int_x<(sizeof(DefControl)/2);int_x++)
            ctrl->c_MaxTPipe[int_x]=DefControl[int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eConstMech *dst = &_GD.ConstMechanic[gh_idx];

            dst->ConstMixVal[int_x].v_TimeMixVal=DefMechanic[0];
            dst->ConstMixVal[int_x].v_MinTim=(char)DefMechanic[3];
            dst->ConstMixVal[int_x].v_PFactor=DefMechanic[1];
            dst->ConstMixVal[int_x].v_IFactor=DefMechanic[2];
            //pGD_ConstMechanic->ConstMixVal[IntX].Power=(char)DefMechanic[3];
        }

/* Первоначальна настройка калибровок */
        for (int byte_y=0;byte_y<cConfSSens;byte_y++)
        {
            eCS=&caldata.Cal.InTeplSens[gh_idx][byte_y];
            eCS->V0=NameSensConfig[byte_y].vCal[0];
            eCS->V1=NameSensConfig[byte_y].vCal[1];
            eCS->U0=NameSensConfig[byte_y].uCal[0];
            eCS->U1=NameSensConfig[byte_y].uCal[1];
            eCS->Output=NameSensConfig[byte_y].Output;
            eCS->Type=NameSensConfig[byte_y].TypeInBoard;
            //eCS->nInput=InPortsAndInputs[ByteX][ByteY][1];
        }
    }
}


int CorrectionRule(int fStartCorr,int fEndCorr, int fCorrectOnEnd, int fbSet)
{
    if ((creg.Y<=fStartCorr)||(fStartCorr == fEndCorr))
    {
        creg.Z=0;
        return 0;
    }
    if (creg.Y>fEndCorr)
        creg.Z=fCorrectOnEnd;
    else
        creg.Z=(int)((((long)(creg.Y-fStartCorr))*fCorrectOnEnd)/(fEndCorr-fStartCorr));
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
    creg.Z=_GD.TControl.MeteoSensing[cSmDWindSens]+_GD.TuneClimate.o_TeplPosition;
    creg.Z%=360;
    _GD.TControl.Tepl[0].CurrPozFluger=_GD.Hot.PozFluger;
    if ((!_GD.Hot.PozFluger)&&(creg.Z
                              >(90+f_DeadWindDirect))
        &&(creg.Z<(270-f_DeadWindDirect)))
        _GD.TControl.Tepl[0].CurrPozFluger=1;
    if ((_GD.Hot.PozFluger)&&((creg.Z
                              <(90-f_DeadWindDirect))
                             ||(creg.Z>(270+f_DeadWindDirect))))
        _GD.TControl.Tepl[0].CurrPozFluger=0;
    if (_GD.Hot.PozFluger!=_GD.TControl.Tepl[0].CurrPozFluger)
    {
        _GD.TControl.Tepl[0].CurrPozFlugerTime++;
        if (_GD.TControl.Tepl[0].CurrPozFlugerTime<5)
            return;
        _GD.Hot.PozFluger=_GD.TControl.Tepl[0].CurrPozFluger;
    }
    _GD.TControl.Tepl[0].CurrPozFlugerTime=0;
}
