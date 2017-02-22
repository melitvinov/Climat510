#include "syntax.h"


#include "defs.h"
#include "main_consts.h"

#include "control_const.h"
// XXX: dirty, but ok for now
#include "control_const.c"

#include "control_gd.h"
#include "control.h"
#include "control_subr.h"

static int16_t  airHeatPause[8];
static int16_t  airHeatTimeWork[8];
static int16_t  airHeatOnOff[8];

extern int8_t  bWaterReset[16];

static int16_t teplTmes[8][6];


static int16_t getTempSensor(char fnTepl, char sensor)
{
    if (gdp.Hot_Tepl->InTeplSens[sensor].RCS  ==  0)
    {
        teplTmes[fnTepl][sensor] = gdp.Hot_Tepl->InTeplSens[sensor].Value;
        return gdp.Hot_Tepl->InTeplSens[sensor].Value;
    }
    if (gdp.Hot_Tepl->InTeplSens[sensor].RCS != 0)
    {
        if (gdp.Hot_Tepl->InTeplSens[sensor].Value  ==  0)
            return 0;
        return teplTmes[fnTepl][sensor];
    }
}

/*!
\brief Температура воздуха для вентиляци в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t getTempVent(char fnTepl)
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
    int16_t singleSensor = 0;
    calcType = GD.Control.Tepl[fnTepl].sensT_vent >> 6;
    mask = GD.Control.Tepl[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        // XXX: shift has a less priority !
        if ((mask >> i & 1) && (getTempSensor(fnTepl, i)))
        {
            temp = getTempSensor(fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            singleSensor = temp;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else maskN = (maskN >> 1);
    }
    average = average / averageCount;
    if (error)
    {
        GD.Hot.Tepl[fnTepl].tempParamVent=maskN+(calcType<<6); //GD.Control.Tepl[fnTepl].sensT_vent;
        GD.Hot.Tepl[fnTepl].tempVent = average;
        if (calcType & 1)
            GD.Hot.Tepl[fnTepl].tempVent = min;
        if (calcType >> 1 & 1)
            GD.Hot.Tepl[fnTepl].tempVent = max;
        return GD.Hot.Tepl[fnTepl].tempVent;
    }
}

/*!
\brief Температура воздуха для обогрева в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t getTempHeat(char fnTepl)
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
    int16_t singleSensor = 0;
    calcType = GD.Control.Tepl[fnTepl].sensT_heat >> 6;
    mask = GD.Control.Tepl[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(fnTepl, i)))
        {
            temp = getTempSensor(fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            singleSensor = temp;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else maskN = (maskN >> 1);
    }
    average = average / averageCount;
    if (error)
    {
        GD.Hot.Tepl[fnTepl].tempParamHeat=maskN+(calcType<<6);
        GD.Hot.Tepl[fnTepl].tempHeat = average;
        if (calcType & 1)
            GD.Hot.Tepl[fnTepl].tempHeat = min;
        if (calcType >> 1 & 1)
            GD.Hot.Tepl[fnTepl].tempHeat = max;
        return GD.Hot.Tepl[fnTepl].tempHeat;
    }
}

/*!
\brief Авария датчика температуры воздуха вентиляции в зависимости от выбранного значение в Параметрах управления
*/
int8_t getTempVentAlarm(char fnTepl)
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
    int16_t singleSensor = 0;
    calcType = GD.Control.Tepl[fnTepl].sensT_vent >> 6;
    mask = GD.Control.Tepl[fnTepl].sensT_vent << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(fnTepl, i)))
        {
            temp = getTempSensor(fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            singleSensor = temp;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else maskN = (maskN >> 1);
    }
    return maskN;
}

/*!
\brief Авария датчика температуры воздуха обогрева в зависимости от выбранного значение в Параметрах управления
*/
int8_t getTempHeatAlarm(char fnTepl)
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
    int16_t singleSensor = 0;
    calcType = GD.Control.Tepl[fnTepl].sensT_heat >> 6;
    mask = GD.Control.Tepl[fnTepl].sensT_heat << 2;
    mask = mask >> 2;
    error = 0;
    for (i=0;i<6;i++)
    {
        if ((mask >> i & 1) && (getTempSensor(fnTepl, i)))
        {
            temp = getTempSensor(fnTepl, i);
            if (min > temp)
                min = temp;
            if (max < temp)
                max = temp;
            average += temp;
            averageCount++;
            singleSensor = temp;
            maskN = (maskN >> 1) + 32;
            error = 1;
        }
        else maskN = (maskN >> 1);
    }
    return maskN;
}

//*****************************************************************************************************************
static uint16_t airHeatGetHeatPause(char fnTepl)
{
    return airHeatPause[fnTepl];
}

static void airHeatSetHeatPause(char timeInc, char fnTepl)
{
    if (timeInc)
        airHeatPause[fnTepl] = airHeatPause[fnTepl] + timeInc;
    else
        airHeatPause[fnTepl] = 0;
}

static uint16_t airHeatGetTimeWork(char fnTepl)
{
    return airHeatTimeWork[fnTepl];
}

static void airHeatSetTimeWork(char timeInc, char fnTepl)
{
    if (timeInc)
        airHeatTimeWork[fnTepl] = airHeatTimeWork[fnTepl] + timeInc;
    else
        airHeatTimeWork[fnTepl] = 0;
}

static void airHeatOn(char fnTepl)
{
    GD.Hot.Tepl[fnTepl].HandCtrl[cHSmHeat].Position = 1;
    //(*(pGD_Hot_Hand+cHSmHeat)).Position=1;//pGD_TControl_Tepl->Calorifer;
    airHeatOnOff[fnTepl]=1;
    airHeatSetHeatPause(1, fnTepl);
}

static void airHeatOff(char fnTepl)
{
    GD.Hot.Tepl[fnTepl].HandCtrl[cHSmHeat].Position = 0;
    airHeatSetTimeWork(0, fnTepl);
    airHeatSetHeatPause(0, fnTepl);
    airHeatSetHeatPause(1, fnTepl);  // запускаем счетчик паузы
    airHeatOnOff[fnTepl]=0;
}

void airHeatInit(void)
{
    char tCTepl;
    for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
    {
        airHeatPause[tCTepl] = 0;
        airHeatTimeWork[tCTepl] = 0;
        airHeatOnOff[tCTepl] = 0;
        //airHeatSetTimeWork(GD.TuneClimate.airHeatMinWork,tCTepl);  // что бы сработывало условие выключения обогревателя при вервом старте
        airHeatOff(tCTepl);
    }
}

void airHeatTimers(void)
{
    char tCTepl;
    for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
    {
//		if ( tCTepl != 0) return;
        if (airHeatOnOff[tCTepl]  ==  1)
        //if (pGD_Hot_Tepl->airHeatOnOff  ==  1)
        {
            if (airHeatGetTimeWork(tCTepl) >= (GD.TuneClimate.airHeatMaxWork / 100)) // если обогреватель включен проверяем макс время работы
                airHeatOff(tCTepl);
            else
                airHeatSetTimeWork(1,tCTepl);   // если обогреватель включен увеличиваем его время на 1 мин
        }
        if (airHeatOnOff[tCTepl]  ==  0)
        //if (pGD_Hot_Tepl->airHeatOnOff  ==  0)   // если обогреватель выключен увеличиваем его время паузы на 1 мин
        {
            if (airHeatGetHeatPause(tCTepl) >= (GD.TuneClimate.airHeatPauseWork / 100))
                airHeatSetHeatPause(0, tCTepl); // 0 означает что обогреватель может быть включен
            else if (airHeatGetHeatPause(tCTepl) > 0)
                airHeatSetHeatPause(1, tCTepl);     // считаем паузу пока она не обнулится
        }
    }
}

void airHeat(char fnTepl)
{
    if ((YesBit((gdp.Hot_Hand+cHSmHeat)->RCS,(cbManMech)))) return;
    int16_t tempT, tempTon, tempToff = 0;
//	if ( fnTepl != 0) return;
    tempT = getTempHeat(fnTepl);
    if (tempT < GD.Hot.Tepl[fnTepl].AllTask.TAir)
        tempTon = GD.Hot.Tepl[fnTepl].AllTask.TAir - tempT;
    else tempToff = tempT - GD.Hot.Tepl[fnTepl].AllTask.TAir;
    if (tempT > 0)
    {
        //if ((GD.TuneClimate.airHeatTemperOn >= tempTon) && (GD.TuneClimate.airHeatTemperOff > tempToff) && (airHeatGetHeatPause(fnTepl)  ==  0))  // обогреватель можно вкл и пауза между вкл прошла
        if ((GD.TuneClimate.airHeatTemperOn <= tempTon) && (airHeatGetHeatPause(fnTepl)  ==  0))  // обогреватель можно вкл и пауза между вкл прошла
            airHeatOn(fnTepl);
        if ((GD.TuneClimate.airHeatTemperOff <= tempToff) && (airHeatGetTimeWork(fnTepl) >= (GD.TuneClimate.airHeatMinWork / 100)))  // обогреватель можно выклюсить если мин время работы прошло и максимальная температура достигнута
            airHeatOff(fnTepl);
    }
    GD.Hot.Tepl[fnTepl].airHeatTimeWork = airHeatTimeWork[fnTepl]*100;
    GD.Hot.Tepl[fnTepl].airHeatOnOff = airHeatOnOff[fnTepl];
}

//***********************************************************************************************************


bool SameSign(int Val1,int Val2)
{
    if (((Val1>0)&&(Val2>0))
        || ((Val1<0)&&(Val2<0))) return 1;
    else
        return 0;
}

void SetPointersOnTepl(char fnTepl)
{
    gdp.Hot_Tepl=&GD.Hot.Tepl[fnTepl];
    gdp.TControl_Tepl=&GD.TControl.Tepl[fnTepl];
    gdp.Control_Tepl=&GD.Control.Tepl[fnTepl];
    gdp.Hot_Hand=&GD.Hot.Tepl[fnTepl].HandCtrl[0];
    gdp.ConstMechanic=&GD.ConstMechanic[fnTepl];
    gdp.MechConfig=&GD.MechConfig[fnTepl];
    #warning "this was fucked. not sure if [0] is ok"
    gdp.Level_Tepl=&GD.Level.InTeplSens[fnTepl][0];
    #warning "this was fucked too. not sure if [0] is ok"
    gdp.Strategy_Tepl=&GD.Strategy[fnTepl][0];
}



void SetPointersOnKontur(char fnKontur)
{
    gdp.Hot_Tepl_Kontur=&(gdp.Hot_Tepl->Kontur[fnKontur]);
    gdp.TControl_Tepl_Kontur=&(gdp.TControl_Tepl->Kontur[fnKontur]);
    gdp.Hot_Hand_Kontur=&(gdp.Hot_Hand[fnKontur]);
    gdp.Strategy_Kontur=&gdp.Strategy_Tepl[fnKontur];
    gdp.MechConfig_Kontur=&gdp.MechConfig->RNum[fnKontur];
    gdp.ConstMechanic_Mech=&gdp.ConstMechanic->ConstMixVal[fnKontur];
}

void MidlWindAndSr(void)
{
    GD.TControl.SumSun+=((long int)GD.TControl.MeteoSensing[cSmFARSens]);
    GD.TControl.MidlSR=((((long int)GD.TControl.MidlSR)*(1000-o_MidlSRFactor))/1000
                        +((long int)GD.TControl.MeteoSensing[cSmFARSens])*o_MidlSRFactor);
    GD.Hot.MidlSR=(int)(GD.TControl.MidlSR/1000);
    if (GetMetSensConfig(cSmFARSens))
    {
        GD.Hot.SumSun=(int)((GD.TControl.SumSun*6)/1000);
    }
    GD.Hot.MidlWind=(int)((((long int)GD.Hot.MidlWind)*(1000-o_MidlWindFactor)+((long int)GD.TControl.MeteoSensing[cSmVWindSens])*o_MidlWindFactor)/1000);

}

void CheckMidlSr(void)
{
    if (GetMetSensConfig(cSmFARSens))
    {
        GD.Hot.SumSun=(int)((GD.TControl.SumSun*6)/1000);
    }
    GD.Hot.MidlSR=(int)(GD.TControl.MidlSR/1000);
}

int clamp_min(int f_in, int f_gr)
{
    return f_in < f_gr ? f_gr : f_in;
}

int clamp_max(int f_in, int f_gr)
{
    return f_in > f_gr ? f_gr : f_in;
}

char CheckSeparate (char fnKontur)
{
    char t2;
    char t1;
    gdp.TControl_Tepl_Kontur->NAndKontur=0;
    if (!(*gdp.MechConfig_Kontur))
        return 0;
    t1 = 0;
    for (t2 = 0;t2<cSTepl;t2++)
        if (GD.MechConfig[t2].RNum[fnKontur] == (*gdp.MechConfig_Kontur))
        {
            t1 |= (1<<t2);
            gdp.TControl_Tepl_Kontur->NAndKontur++;
        }
    return t1;
}

char CheckMain(char fnTepl)
{
    char tTepl;
    tTepl=0;
    while (tTepl<cSTepl)
    {
        if ((gdp.TControl_Tepl_Kontur->Separate>>tTepl)&1)
            return tTepl;
        tTepl++;
    }
    return fnTepl;
}

void InitGD(void)
{
    eCalSensor *eCS;

    #warning "this one is fucked. we're cleaning too much"
    memclr(&GD.Control,sizeof(eControl)
           +sizeof(eFullCal)
           +sizeof(eLevel)
           +sizeof(eTimer)*cSTimer);

    #warning "this one is fucked. we're cleaning too much"
    memclr(&GD.ConstMechanic[0],sizeof(eTuneClimate)+sizeof(eTControl)+sizeof(eStrategy)*cSStrategy*cSTepl+sizeof(eConstMech)*cSTepl+sizeof(eMechConfig)*cSTepl);


    #warning "meteo is not cleaned"
    memclr(&sensdata.uInTeplSens,sizeof(sensdata.uInTeplSens));

    // default datetime
    GD.Hot.Year=01;
    GD.Hot.Date=257;
    GD.Hot.Time=8*60;

    #warning "sizeof is fucked"
    // XXX: this sizeof is fucked, TStart[i] is fucked too
    for (uint gh_idx  =0; gh_idx< sizeof(NameConst)/3; gh_idx++)
        GD.TuneClimate.s_TStart[gh_idx] = NameConst[gh_idx].StartZn;

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
        SetPointersOnTepl(gh_idx);
        for (int int_x=0;int_x<cSStrategy;int_x++)
        {
            #warning "looks like this is fucked too"
            for (uint byte_y=0;byte_y<sizeof(eStrategy);byte_y++)
                (*((&(gdp.Strategy_Tepl[int_x].TempPower))+byte_y))=(*((&DefStrategy[int_x].TempPower)+byte_y));
        }

        bWaterReset[gh_idx]=1;

        eMechConfig *mech_cfg = &GD.MechConfig[gh_idx];

        for (int int_x=0;int_x<SUM_NAME_CONF;int_x++)
            mech_cfg->RNum[int_x]=MechC[gh_idx][int_x];

        for (int int_x=0;int_x<cConfSSystem;int_x++)
            mech_cfg->Systems[int_x]=InitSystems[gh_idx][int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eTControlTepl *dst = &GD.TControl.Tepl[gh_idx];

            //pGD_TControl_Tepl->MechBusy[IntX].BlockRegs=1;
            dst->MechBusy[int_x].PauseMech=300;
            dst->MechBusy[int_x].Sens=0;
        }

        eTeplControl *ctrl = &GD.Control.Tepl[gh_idx];

        #warning "sizeof is fucked"
        //PANIC_IF(countof(ctrl->c_MaxTPipe) != countof(DefControl));

        for (uint int_x=0;int_x<(sizeof(DefControl)/2);int_x++)
            ctrl->c_MaxTPipe[int_x]=DefControl[int_x];

        for (int int_x=0;int_x<cSRegCtrl;int_x++)
        {
            eConstMech *dst = &GD.ConstMechanic[gh_idx];

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
    GD.Hot.PozFluger &= 1;
    if (GD.TuneClimate.o_TeplPosition == 180)
    {
        GD.Hot.PozFluger=0;
        return;
    }
    if (GD.Hot.MidlWind<GD.TuneClimate.f_WindStart) return;
    creg.Z=GD.TControl.MeteoSensing[cSmDWindSens]+GD.TuneClimate.o_TeplPosition;
    creg.Z%=360;
    GD.TControl.Tepl[0].CurrPozFluger=GD.Hot.PozFluger;
    if ((!GD.Hot.PozFluger)&&(creg.Z
                              >(90+f_DeadWindDirect))
        &&(creg.Z<(270-f_DeadWindDirect)))
        GD.TControl.Tepl[0].CurrPozFluger=1;
    if ((GD.Hot.PozFluger)&&((creg.Z
                              <(90-f_DeadWindDirect))
                             ||(creg.Z>(270+f_DeadWindDirect))))
        GD.TControl.Tepl[0].CurrPozFluger=0;
    if (GD.Hot.PozFluger!=GD.TControl.Tepl[0].CurrPozFluger)
    {
        GD.TControl.Tepl[0].CurrPozFlugerTime++;
        if (GD.TControl.Tepl[0].CurrPozFlugerTime<5)
            return;
        GD.Hot.PozFluger=GD.TControl.Tepl[0].CurrPozFluger;
    }
    GD.TControl.Tepl[0].CurrPozFlugerTime=0;
}



/*void _SetBitOutReg(char fnTepl,char fnMech,char fnBit,char fnclr,char fnSm)
{	char nBit,nByte,Mask;
    nBit=BitMech[fnTepl][fnMech][fnBit]&0x0F;
    if(!nBit) return;
    Mask=1;
    Mask<<=(nBit-1+fnSm);
    nByte=BitMech[fnTepl][fnMech][fnBit]/16;
    if (fnclr)
        OutR[nByte] &= (~Mask);
    else
        OutR[nByte] |= Mask;

}*/
