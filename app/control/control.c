#define _FRIEND_OF_CONTROL_

#include "syntax.h"

//#include "defs.h"
#include "control_def.h"
#include "control_gd.h"
#include "control_siod.h"
#include "control_subr.h"
#include "control_strategy.h"
#include "control_upstream.h"
#include "control_air_heat.h"
#include "control_screen.h"
#include "control.h"

#warning "WTF: control ABI contains pointer. no reason to pass pointers thru ABI"

/*!
\brief Таблица типов старта
*/
typedef enum
{
    TYPE_START_OFF              = 0,
    TYPE_START_BEFORE_SUNSET    = 1,
    TYPE_START_AFTER_SUNSET     = 2,
    TYPE_START_BEFORE_SUNRISE   = 3,
    TYPE_START_AFTER_SUNRISE    = 4,
} TYPE_START;

typedef struct
{
    int16_t settingsVosx;
    int16_t settingsZax;
    int16_t fLightPause;
    uchar MaskRas;
    int PastPerRas;
    int TecPerRas;
} control_ctx_t;


int8_t  bWaterReset[16];


uchar       bNight;


const uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};

static control_ctx_t ctx;

const zone_t make_zone_ctx(int zone_idx);

static int abs(int f_in)
{
    if (f_in<0) return(-f_in);
    return f_in;
}

/*!
\brief Коррекция времени старта по типу старта
@param typeStart тип старта, timeStart - время старта, sunRise - время восхода, sunSet - время захода. Параметры из задания
@return int16_t скорректированное время. 0 - тип старта = 0, -1 - ошибка
*/
int16_t controlTypeStartCorrection(TYPE_START typeStart, int16_t timeStart, int16_t sunRise, int16_t sunSet)
{
    switch (typeStart)
    {
    case TYPE_START_BEFORE_SUNSET:
        if (sunSet >= timeStart)
            return sunSet - timeStart;
        //else return -1;
        break;
    case TYPE_START_AFTER_SUNSET:
        if ((sunSet + timeStart) > 1440)
            return(sunSet + timeStart) % 1440;
        else return sunSet + timeStart;
        //return sunSet + timeStart;
        break;
    case TYPE_START_BEFORE_SUNRISE:
        if (sunRise >= timeStart)
            return sunRise - timeStart;
        #warning "garbage is returned"
        //else return -1;
        break;
    case TYPE_START_AFTER_SUNRISE:
        if ((sunRise + timeStart) > 1440)
            return(sunRise + timeStart) % 1440;
        else return sunRise + timeStart;
        //return sunRise + timeStart;
        break;
    case TYPE_START_OFF:
        return timeStart;
        break;
    }
}

/*------------------------------------------
Вычисление значения задаваемой величины
--------------------------------------------*/
/*----------------------------------------*/

/*----------------------------------------------------
                Находим нужную программу
------------------------------------------------------*/
int JumpNext(int Now,int Next,char Check, char Mull, int creg_x, int creg_y)
{
    if ((Check)&&((!Next)||(!Now)))
        return Now*Mull;
    int var = Next-Now;
    var*=creg_x;
    if (!creg_y)
        return Now*Mull;
    var/=creg_y;
    return(Now+var)*Mull;
}

//int16_t TempOld, TempNew = 0;

void TaskTimer(char fsmTime, char fnTeplTimer, char fnTeplLoad)
{
    int8_t nTimer,sTimerNext,sTimerPrev,sTimerMin,sTimerMax;
    int MaxTimeStart,MinTimeStart,NextTimeStart,PrevTimeStart,tVal;
    int16_t typeStartCorrection;
    int16_t nextTimer = 0;
    int16_t prevTimer = 0;

    eZone *hot = &_GD.Hot.Zones[fnTeplLoad];
    hot->AllTask.TAir = 0;


    int creg_z = _GD.Hot.Time + fsmTime;
    creg_z %= 1440;
    MaxTimeStart = 0;
    PrevTimeStart = 0;
    NextTimeStart = 1440;
    MinTimeStart = 1440;
    sTimerNext=-1;
    sTimerPrev=-1;
    for (nTimer = 0;nTimer<cSTimer;nTimer++) //20
    {
        eTimer *Timer = &_GD.Timers[nTimer];
        typeStartCorrection = controlTypeStartCorrection(Timer->TypeStart, Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
        //if (typeStartCorrection  ==  -1)
        //    typeStartCorrection = GD.Timer[nTimer].TimeStart;

        if (!typeStartCorrection)
            continue;
        if (_GD.Timers[nTimer].Zone[0]!=fnTeplTimer+1)
            continue;

        if (typeStartCorrection<MinTimeStart)
        {
            MinTimeStart = typeStartCorrection;
            sTimerMin = nTimer;
        }
        if (typeStartCorrection>MaxTimeStart)
        {
            MaxTimeStart = typeStartCorrection;
            sTimerMax = nTimer;
        }
        if ((typeStartCorrection >= creg_z )&&(NextTimeStart>typeStartCorrection))
        {
            NextTimeStart = typeStartCorrection;
            sTimerNext = nTimer;
        }
        if ((typeStartCorrection < creg_z)&&(PrevTimeStart<typeStartCorrection))
        {
            PrevTimeStart = typeStartCorrection;
            sTimerPrev = nTimer;
        }
    }
    if (MinTimeStart == 1440) return;

    if (sTimerNext<0)
        sTimerNext = sTimerMin;

    if (sTimerPrev<0)
        sTimerPrev = sTimerMax;

    eTimer *pGD_CurrTimer = &_GD.Timers[sTimerPrev];
    eTimer *pGD_NextTimer = &_GD.Timers[sTimerNext];
    eTimer *Timer = &_GD.Timers[sTimerPrev];

    prevTimer = controlTypeStartCorrection(Timer->TypeStart, Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
    Timer = &_GD.Timers[sTimerNext];
    nextTimer = controlTypeStartCorrection(Timer->TypeStart, Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);

    int creg_x = _GD.Hot.Time - prevTimer;
    int creg_y = nextTimer - prevTimer;

    if (creg_y < 0)
    {
        creg_y += 1440;
    }

    if (creg_x <0)
    {
        creg_x += 1440;
    }

    if (fsmTime)
    {
        hot->AllTask.NextTAir = JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1, creg_x, creg_y);
//Блокировка нулевой темепратуры вентиляции
        tVal = pGD_CurrTimer->TVentAir;
        if (!tVal) tVal = pGD_CurrTimer->TAir+100;
        hot->AllTask.NextTVent = JumpNext(tVal,pGD_NextTimer->TVentAir,1,1, creg_x, creg_y);
        hot->AllTask.Light = pGD_CurrTimer->Light;
        hot->AllTask.ModeLight = pGD_CurrTimer->ModeLight;
//		if (pGD_Hot_Tepl->InTeplSens[cSmRHSens])
//		pGD_Hot_Tepl->AllTask.NextRHAir = JumpNext(pGD_CurrTimer->RHAir,pGD_NextTimer->RHAir,1);
        return;
    }
#warning temp !!!!!!!!!!!!!!!!!!!!!!!!!!

    // T отопления, в зависимости что стоит в параметрах упраления, то и ставим в температуру
    hot->AllTask.TAir = JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1, creg_x, creg_y);


    //if (pGD_Hot_Tepl->AllTask.TAir - TempOld > 50)
    //	NOP;
    //TempOld = pGD_Hot_Tepl->AllTask.TAir;

    //Блокировка нулевой темепратуры вентиляции
    tVal = pGD_CurrTimer->TVentAir;
    if (!tVal) tVal = pGD_CurrTimer->TAir+100;

    // T вентиляции
    hot->AllTask.DoTVent = JumpNext(tVal,pGD_NextTimer->TVentAir,1,1, creg_x, creg_y);

    hot->AllTask.SIO = pGD_CurrTimer->SIO;
    hot->AllTask.RHAir = JumpNext(pGD_CurrTimer->RHAir_c,pGD_NextTimer->RHAir_c,1,100, creg_x, creg_y);
    hot->AllTask.CO2 = JumpNext(pGD_CurrTimer->CO2,pGD_NextTimer->CO2,1,1, creg_x, creg_y);
    hot->Kontur[cSmKontur1].MinTask = JumpNext(pGD_CurrTimer->MinTPipe1,pGD_NextTimer->MinTPipe1,1,10, creg_x, creg_y);

    hot->Kontur[cSmKontur2].MinTask = JumpNext(pGD_CurrTimer->MinTPipe2,pGD_NextTimer->MinTPipe2,1,10, creg_x, creg_y);

    hot->Kontur[cSmKontur3].MinTask = JumpNext(pGD_CurrTimer->MinTPipe3,pGD_NextTimer->MinTPipe3,1,10, creg_x, creg_y);
    hot->Kontur[cSmKontur5].MinTask = JumpNext(pGD_CurrTimer->MinTPipe5,pGD_NextTimer->MinTPipe5,1,10, creg_x, creg_y);

    hot->Kontur[cSmKontur1].Optimal = JumpNext(pGD_CurrTimer->TOptimal1,pGD_NextTimer->TOptimal1,1,10, creg_x, creg_y);

    hot->Kontur[cSmKontur2].Optimal = JumpNext(pGD_CurrTimer->TOptimal2,pGD_NextTimer->TOptimal2,1,10, creg_x, creg_y);

    hot->Kontur[cSmWindowUnW].MinTask = JumpNext(((uchar)pGD_CurrTimer->MinOpenWin),((uchar)pGD_NextTimer->MinOpenWin),0,1, creg_x, creg_y);
    hot->AllTask.Win = pGD_CurrTimer->Win;
    hot->AllTask.Screen[0]=pGD_CurrTimer->Screen[0];
    hot->AllTask.Screen[1]=pGD_CurrTimer->Screen[1];
    hot->AllTask.Screen[2]=pGD_CurrTimer->Screen[2];
    hot->AllTask.Vent = pGD_CurrTimer->Vent;
//	pGD_Hot_Tepl->AllTask.Poise = pGD_CurrTimer->Poise;
    hot->Kontur[cSmKontur3].Do = JumpNext(pGD_CurrTimer->TPipe3,pGD_NextTimer->TPipe3,1,10, creg_x, creg_y);
    hot->Kontur[cSmKontur4].Do = JumpNext(pGD_CurrTimer->TPipe4,pGD_NextTimer->TPipe4,1,10, creg_x, creg_y);

}


void AllTaskAndCorrection(const zone_t *zone)
{
    int sum;
    int val = 0;

    zone->hot->AllTask.DoTHeat = zone->hot->AllTask.TAir;
    int creg_y = _GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    int creg_z;
    /*Установка и коррекция по солнцу температуры обогрева*/
    int creg_x = CorrectionRule(_GD.TuneClimate.s_TStart[0],
                            _GD.TuneClimate.s_TEnd,
                            _GD.TuneClimate.s_TConst,
                            cbCorrTOnSun,
                            creg_y,
                            &creg_z);
    SetBit(zone->hot->RCS, cbCorrTOnSun);

    zone->hot->AllTask.DoTHeat += creg_z;
    /*Коррекция прогноза*/
    zone->hot->AllTask.NextTAir += creg_z;
    if (zone->hot->AllTask.DoTVent)
    {
        CorrectionRule(_GD.TuneClimate.s_TStart[0],
                       _GD.TuneClimate.s_TEnd,
                       _GD.TuneClimate.s_TVentConst,
                       0,
                       creg_y,
                       &creg_z);
        zone->hot->AllTask.DoTVent+=creg_z;
        zone->hot->AllTask.NextTVent+=creg_z;
    }
    else
    {
        zone->hot->AllTask.DoTVent = zone->hot->AllTask.DoTHeat+100;
        zone->hot->AllTask.NextTVent = zone->hot->AllTask.NextTAir+100;
    }
//	if ((*(pGD_Hot_Hand+cHSmScrTH)).Position)
//		pGD_Hot_Tepl->AllTask.NextRezTAir = pGD_Hot_Tepl->AllTask.NextTAir-GD.TuneClimate.sc_DoTemp;
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу заданной влажности*/
    if (zone->hot->AllTask.RHAir)
    {
        zone->hot->AllTask.DoRHAir = zone->hot->AllTask.RHAir;
        int creg_x = CorrectionRule(_GD.TuneClimate.s_TStart[0],
                                _GD.TuneClimate.s_TEnd,
                                _GD.TuneClimate.s_RHConst,
                                cbCorrRHOnSun,
                                creg_y,
                                &creg_z);
        SetBit(zone->hot->RCS, creg_x);
        zone->hot->AllTask.DoRHAir-=creg_z;
        /*Коррекция прогноза*/
//		pGD_Hot_Tepl->AllTask.NextRHAir-=IntZ;
    }
    /*---------------------------------------------------*/

    /*Установка и коррекция по солнцу заданной концентрации СО2*/
    if (zone->hot->AllTask.CO2)
    {
//ОПТИМИЗАЦИЯ
// Коррекция СО2 держать по фрамугам
        zone->hot->AllTask.DoCO2 = zone->hot->AllTask.CO2;    // так было, строчка лишняя но все же решил оставить

        if (zone->gh_ctrl->co_model!=3)
        {
            sum = zone->hand[cHSmWinN].Position + zone->hand[cHSmWinS].Position;
            if ((sum >= _GD.TuneClimate.co2Fram1) && (sum <= _GD.TuneClimate.co2Fram2))
            {
                if (zone->hot->AllTask.CO2 > _GD.TuneClimate.co2Off)
                {
                    if (_GD.TuneClimate.co2Fram2 > _GD.TuneClimate.co2Fram1)
                        val = _GD.TuneClimate.co2Fram2 - _GD.TuneClimate.co2Fram1;
                    val = ((sum - _GD.TuneClimate.co2Fram1) * _GD.TuneClimate.co2Off) / val;
                    zone->hot->AllTask.DoCO2 = zone->hot->AllTask.DoCO2 - val;
                }
            }
            if (sum > _GD.TuneClimate.co2Fram2)
                zone->hot->AllTask.DoCO2 = zone->hot->AllTask.DoCO2 - _GD.TuneClimate.co2Off;
        }

        int creg_x = CorrectionRule(_GD.TuneClimate.s_TStart[0],
                                _GD.TuneClimate.s_TEnd,
                                _GD.TuneClimate.s_CO2Const,
                                cbCorrCO2OnSun,
                                creg_y,
                                &creg_z);
        SetBit(zone->hot->RCS, cbCorrCO2OnSun);
        zone->hot->AllTask.DoCO2+=creg_z;
    }
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу минимальной температуры в контурах 1 и 2*/

    if (zone->hot->Kontur[cSmKontur1].MinTask)
    {
        CorrectionRule(_GD.TuneClimate.s_TStart[0],
                        _GD.TuneClimate.s_TEnd,
                        _GD.TuneClimate.s_MinTPipeConst,
                        0/*cbCorrMinTaskOnSun*/,
                        creg_y,
                        &creg_z);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        zone->hot->Kontur[cSmKontur1].MinCalc = zone->hot->Kontur[cSmKontur1].MinTask-creg_z;
        creg_y = DefRH(zone);//MeteoSens[cSmFARSens].Value;

        CorrectionRule(_GD.TuneClimate.c_RHStart,
                        _GD.TuneClimate.c_RHEnd,
                        _GD.TuneClimate.c_RHOnMin1,
                        0/*cbCorrMinTaskOnSun*/,
                        creg_y,
                        &creg_z);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        zone->hot->Kontur[cSmKontur1].MinCalc += creg_z;

    }
    zone->hot->Kontur[cSmKontur2].MinCalc = zone->hot->Kontur[cSmKontur2].MinTask;
    if (zone->hot->Kontur[cSmKontur2].MinTask)
    {
        creg_y = DefRH(zone);//MeteoSens[cSmFARSens].Value;

         CorrectionRule(_GD.TuneClimate.c_RHStart,
                        _GD.TuneClimate.c_RHEnd,
                        _GD.TuneClimate.c_RHOnMin2,
                        0/*cbCorrMinTaskOnSun*/,
                        creg_y,
                        &creg_z);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        zone->hot->Kontur[cSmKontur2].MinCalc += creg_z;
    }

    creg_y = _GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    if (zone->hot->Kontur[cSmKontur3].MinTask)
    {
        CorrectionRule(_GD.TuneClimate.s_TStart[0],
                        _GD.TuneClimate.s_TEnd,
                        _GD.TuneClimate.s_MinTPipe3,
                        0,
                        creg_y,
                        &creg_z);
        zone->hot->Kontur[cSmKontur3].MinCalc = zone->hot->Kontur[cSmKontur3].MinTask - creg_z;
    }

//	pGD_Hot_Tepl->Kontur[cSmKontur3].MinCalc = pGD_Hot_Tepl->Kontur[cSmKontur3].MinTask;
    if (zone->hot->Kontur[cSmKontur5].MinTask)
    {
        CorrectionRule(_GD.TuneClimate.s_TStart[0],
                        _GD.TuneClimate.s_TEnd,
                        _GD.TuneClimate.s_MinTPipe5,
                        0/*cbCorrMinTaskOnSun*/,
                        creg_y,
                        &creg_z);
        zone->hot->Kontur[cSmKontur5].MinCalc = zone->hot->Kontur[cSmKontur5].MinTask + creg_z;
    }

    zone->hot->AllTask.DoPressure = zone->gh_ctrl->c_DoPres;
    /*-------------------------------------------------------------*/

    /*Установка и коррекция по солнцу минимального положения подветренных фрамуг*/
    if (zone->hot->Kontur[cSmWindowUnW].MinTask)
    {
        zone->hot->Kontur[cSmWindowUnW].MinCalc = zone->hot->Kontur[cSmWindowUnW].MinTask;
        CorrectionRule(_GD.TuneClimate.s_TStart[0],
                       _GD.TuneClimate.s_TEnd,
                       _GD.TuneClimate.s_MinOpenWinConst,
                       0/*cbCorrMinTaskOnSun*/,
                       creg_y,
                       &creg_z);
        SetBit(zone->hot->Kontur[cSmWindowUnW].RCS, 0);
        zone->hot->Kontur[cSmWindowUnW].MinCalc += creg_z;
    }
    /*----------------------------------------------------------------*/
    creg_y = DefRH(zone);
    CorrectionRule(_GD.TuneClimate.f_min_RHStart,
                   _GD.TuneClimate.f_min_RHEnd,
                   _GD.TuneClimate.f_CorrTVent,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->AllTask.NextTVent -= creg_z;
    zone->hot->AllTask.DoTVent -= creg_z;
    creg_y = -creg_y;

    CorrectionRule(_GD.TuneClimate.f_max_RHStart,
                   _GD.TuneClimate.f_max_RHEnd,
                   _GD.TuneClimate.f_CorrTVentUp,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->AllTask.NextTVent += creg_z;
    zone->hot->AllTask.DoTVent += creg_z;
    /*--------------------------------------------------------------*/
}

void SetIfReset(const zone_t *zone)
{

    for (int i = 0; i < cSWaterKontur; i++)
    {
        const contour_t ctr = make_contour_ctx(zone, i);

        ctr.tcontrol->DoT = ctr.tcontrol->SensValue*10;//((long int)pGD_Hot_Tepl->InTeplSens[ByteX+cSmWaterSens].Value)*100;
        ctr.tcontrol->PumpPause = cPausePump;
        ctr.tcontrol->PumpStatus = 1;
    }
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinS)).Position))*10;
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+1-GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinN)).Position))*10;

}

/**********************************************************************/
/*-*-*-*-*--Нахождение прогнозируемого изменения температуры--*-*-*-*-*/
/**********************************************************************/
//#warning Прогнозы температуры по внешним факторам !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void __cNextTCalc(const zone_t *zone)
{
    int CalcAllKontur;

    if (!zone->hot->AllTask.NextTAir) return;


    if (bWaterReset[zone->idx])
    {
        SetIfReset(zone);
    }
/*Расчитываем Тизмерения-Тзадания*/
//	pGD_Level_Tepl[cSmTSens][cSmUpAlarmLev]=0;
//	pGD_Level_Tepl[cSmTSens][cSmDownAlarmLev]=0;

//	if (GD.TuneClimate.c_MaxDifTUp)
//		pGD_Level_Tepl[cSmTSens][cSmUpAlarmLev]=pGD_Hot_Tepl->AllTask.DoTHeat+GD.TuneClimate.c_MaxDifTUp;
//	if (GD.TuneClimate.c_MaxDifTDown)
//		pGD_Level_Tepl[cSmTSens][cSmDownAlarmLev]=pGD_Hot_Tepl->AllTask.DoTHeat-GD.TuneClimate.c_MaxDifTDown;

//	pGD_Hot_Tepl->NextTCalc.DifTAirTDo = pGD_Hot_Tepl->AllTask.NextTAir-CURRENT_TEMP_VALUE;
/**********************************************/
/*СУПЕР АЛГОРИТМ ДЛЯ РАСЧЕТА*/
//	pGD_Hot_Tepl->AllTask.Rez[0]=CURRENT_TEMP_VALUE;
//	IntX=(pGD_Hot_Tepl->AllTask.DoTHeat-CURRENT_TEMP_VALUE);

//#warning NEW CHECK THIS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// смотря по какому датчику работаем того и считаем
// ---------------------
// NEW

    #warning "these indexes are fucked up"
    eSensLevel *Level_Tepl=_GD.Level.InTeplSens[zone->idx];

    int cSmTSens = 0;
    for (cSmTSens = 0; cSmTSens<4; cSmTSens++)  // 4 датчика температуры
    {
        Level_Tepl[cSmTSens][cSmUpAlarmLev]=0;
        Level_Tepl[cSmTSens][cSmDownAlarmLev]=0;
        if (_GD.TuneClimate.c_MaxDifTUp)
            Level_Tepl[cSmTSens][cSmUpAlarmLev]=zone->hot->AllTask.DoTHeat+_GD.TuneClimate.c_MaxDifTUp;
        if (_GD.TuneClimate.c_MaxDifTDown)
            Level_Tepl[cSmTSens][cSmDownAlarmLev]=zone->hot->AllTask.DoTHeat-_GD.TuneClimate.c_MaxDifTDown;
    }

    zone->hot->NextTCalc.DifTAirTDo = zone->hot->AllTask.NextTAir-getTempHeat(zone, zone->idx);
    /**********************************************/
    /*СУПЕР АЛГОРИТМ ДЛЯ РАСЧЕТА*/
    zone->hot->AllTask.Rez[0]=getTempHeat(zone, zone->idx);
    int creg_x = zone->hot->AllTask.DoTHeat - getTempHeat(zone, zone->idx);

/**********************************************/
/*Вычиляем увеличение от солнечной радиации*/
    int creg_y = _GD.Hot.MidlSR;
    int creg_z;

    if ((!YesBit(zone->hot->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        creg_y = zone->hot->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(_GD.TuneClimate.c_SRStart,
                   _GD.TuneClimate.c_SREnd,
                   _GD.TuneClimate.c_SRFactor,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->NextTCalc.UpSR = creg_z;
/*Вычиляем увеличение от разницы температуры задания и стекла*/
    creg_y = zone->hot->AllTask.NextTAir-zone->hot->InTeplSens[cSmGlassSens].Value;

    CorrectionRule(_GD.TuneClimate.c_GlassStart,
                   _GD.TuneClimate.c_GlassEnd,
                   _GD.TuneClimate.c_GlassFactor,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->NextTCalc.LowGlass = creg_z;

    if (zone->tcontrol_tepl->Screen[0].Mode < 2)
        //if (pGD_TControl_Tepl->Screen[0].Mode < 2)
        zone->hot->NextTCalc.CorrectionScreen = _GD.TuneClimate.CorrectionScreen * zone->tcontrol_tepl->Screen[0].Mode;

    //vdv
    //if (pGD_TControl_Tepl->Screen[0].Mode)

    //IntY = pGD_TControl_Tepl->Screen[0].Value;
    //if (pGD_Hot_Tepl->AllTask.Screen[0]  ==  2)
    //{
    //	if ((*(pGD_Hot_Hand+cHSmScrTH+0)).Position)
    //	{
    //		CorrectionRule(100, 0, GD.TuneClimate.CorrectionScreen,0);
    //		pGD_Hot_Tepl->NextTCalc.CorrectionScreen = IntZ;
    //	}
    //}
//		screenOldPosiyion = screenTermoGetPossition();
//
//		if ((screenTermoGetPossition() != 0) && (screenTermoGetCurrentPossition() > 0))
//		{
//			CorrectionRule(100, 0, GD.TuneClimate.CorrectionScreen,0);
//			pGD_Hot_Tepl->NextTCalc.CorrectionScreen = IntZ;
    // расчет. Экран понижает t отопление на
    //GD.TuneClimate.CorrectionScreen
//	}
//		}

/*Вычисляем корректировки ветра фрамуг и разницы между температурой задания
и внешней температуры соответственно*/
/*Ветер и фрамуги увеличивают эту разницу*/
    creg_y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.c_WindStart,
                   _GD.TuneClimate.c_WindEnd,
                   _GD.TuneClimate.c_WindFactor,
                   0,
                   creg_y,
                   &creg_z);
    creg_y = zone->hot->AllTask.NextTAir-_GD.TControl.MeteoSensing[cSmOutTSens]-creg_z;
    CorrectionRule(_GD.TuneClimate.c_OutStart,
                   _GD.TuneClimate.c_OutEnd,
                   _GD.TuneClimate.c_OutFactor,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->NextTCalc.LowOutWinWind += creg_z;

//	if (YesBit(pGD_Hot_Tepl->DiskrSens[0],cSmLightDiskr))
    zone->hot->NextTCalc.UpLight=((long)_GD.TuneClimate.c_LightFactor * zone->hand[cHSmLight].Position) /100;
//******************** NOT NEEDED
//	IntY = 1;
//	IntY<<=fnTepl;
//	if (YesBit(GD.Hot.isLight,IntY))
//		pGD_Hot_Tepl->NextTCalc.UpLight = GD.TuneClimate.c_LightFactor;
//********************************************************

/*Считаем сумму поправок*/
    zone->hot->NextTCalc.dSumCalc=
    +zone->hot->NextTCalc.UpSR
    -zone->hot->NextTCalc.LowGlass
    -zone->hot->NextTCalc.LowOutWinWind
    +zone->hot->NextTCalc.UpLight
    -zone->hot->NextTCalc.CorrectionScreen;

//		-pGD_Hot_Tepl->NextTCalc.LowRain;
    if (_GD.TControl.bSnow)
        zone->hot->NextTCalc.dSumCalc-=_GD.TuneClimate.c_CloudFactor;
/*********************************************************************
******* СЧИТАЕМ СУММУ ВЛИЯНИЙ ДЛЯ ФРАМУГ *******************************
***********************************************************************/
    zone->hot->NextTCalc.dSumCalcF = 0;
/*Вычиляем увеличение от солнечной радиации*/
    creg_y = _GD.Hot.MidlSR;
/*if work on internal light sensor, then change IntY*/

    if ((!YesBit(zone->hot->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        creg_y = zone->hot->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(_GD.TuneClimate.c_SRStart,
                   _GD.TuneClimate.c_SREnd,
                   _GD.TuneClimate.f_SRFactor,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->NextTCalc.dSumCalcF+=creg_z;

/*Вычисляем корректировки ветра фрамуг и разницы между температурой задания
и внешней температуры соответственно*/
/*Ветер и фрамуги увеличивают эту разницу*/
    creg_y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.c_WindStart,
                   _GD.TuneClimate.c_WindEnd,
                   _GD.TuneClimate.f_WindFactor,
                   0,
                   creg_y,
                   &creg_z);
    creg_y = zone->hot->AllTask.NextTAir-_GD.TControl.MeteoSensing[cSmOutTSens]-creg_z;
    CorrectionRule(_GD.TuneClimate.c_OutStart,
                   _GD.TuneClimate.c_OutEnd,
                   _GD.TuneClimate.f_OutFactor,
                   0,
                   creg_y,
                   &creg_z);
    zone->hot->NextTCalc.dSumCalcF+=creg_z;
/*********************************************************************
***********************************************************************
***********************************************************************/



    creg_y = zone->hot->NextTCalc.DifTAirTDo;
    if ((_GD.TuneClimate.c_MullDown>10)&&(creg_y<0)&&(_GD.TuneClimate.c_MullDown<30))
        creg_y=(((long)creg_y)*_GD.TuneClimate.c_MullDown)/10;
    zone->hot->NextTCalc.PCorrection=((int)((((long)(creg_y))*((long)zone->gh_ctrl->c_PFactor))/100));
    if (zone->tcontrol_tepl->StopI<2)
        zone->tcontrol_tepl->Integral+=((((long)(zone->hot->NextTCalc.DifTAirTDo))*((long)zone->gh_ctrl->c_IFactor))/10);
    if (zone->tcontrol_tepl->Integral>2000000)
        zone->tcontrol_tepl->Integral = 2000000;
    if (zone->tcontrol_tepl->Integral<-2000000)
        zone->tcontrol_tepl->Integral=-2000000;
    if (! zone->gh_ctrl->c_IFactor)
        zone->tcontrol_tepl->Integral = 0;
    zone->hot->NextTCalc.ICorrection=(int)(zone->tcontrol_tepl->Integral/100);
    zone->tcontrol_tepl->Critery = zone->hot->NextTCalc.PCorrection+zone->hot->NextTCalc.ICorrection-zone->hot->NextTCalc.dSumCalc;
    CalcAllKontur = __sCalcTempKonturs(zone);
    zone->tcontrol_tepl->Critery-=CalcAllKontur;
//	pGD_Hot_Tepl->NextTCalc.dNextTCalc = CalcAllKontur;
    if (zone->tcontrol_tepl->StopI>4)
    {
        zone->tcontrol_tepl->Integral = zone->tcontrol_tepl->SaveIntegral;
    }
//	IntY = pGD_Hot_Tepl->NextTCalc.DifTAirTDo;
    zone->tcontrol_tepl->SaveIntegral = zone->tcontrol_tepl->Integral;
    if ((zone->tcontrol_tepl->StopI>3)&&(ABS(creg_y)<cResetDifTDo))
    {

//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (zone->tcontrol_tepl->Critery>cResetCritery)
        {
            zone->tcontrol_tepl->SaveIntegral
            =cResetCritery+CalcAllKontur
             -zone->hot->NextTCalc.PCorrection+zone->hot->NextTCalc.dSumCalc;
            zone->tcontrol_tepl->SaveIntegral*=100;
        }
//		IntY=-IntY;
//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (zone->tcontrol_tepl->Critery<-cResetCritery)
        {
            zone->tcontrol_tepl->SaveIntegral
            =-cResetCritery+CalcAllKontur
             -zone->hot->NextTCalc.PCorrection+zone->hot->NextTCalc.dSumCalc;
            zone->tcontrol_tepl->SaveIntegral*=100;
        }
    }
    if ((zone->tcontrol_tepl->StopI>3)&&(!SameSign(creg_y, zone->tcontrol_tepl->Critery)))
    {
        zone->tcontrol_tepl->SaveIntegral =creg_y + CalcAllKontur - zone->hot->NextTCalc.PCorrection + zone->hot->NextTCalc.dSumCalc;
        zone->tcontrol_tepl->SaveIntegral*=100;
    }
    if (!zone->tcontrol_tepl->Critery)
    {
        zone->tcontrol_tepl->Critery = 1;
        if (zone->hot->NextTCalc.DifTAirTDo<0)
            zone->tcontrol_tepl->Critery=-1;

    }
    zone->hot->NextTCalc.Critery = zone->tcontrol_tepl->Critery;

/******************************************************************
        Далее расчет критерия для фрамуг
*******************************************************************/
    if (getTempVent(zone, zone->idx))
        creg_y = getTempVent(zone, zone->idx)-zone->hot->AllTask.DoTVent;
    else
        creg_y = 0;

    zone->hot->NextTCalc.PCorrectionVent=((int)((((long)(creg_y))*((long)zone->gh_ctrl->f_PFactor))/100));
    if (zone->tcontrol_tepl->StopVentI<2)
        zone->tcontrol_tepl->IntegralVent+=((((long)(creg_y))*((long)zone->gh_ctrl->f_IFactor))/10);
    if (zone->tcontrol_tepl->IntegralVent<0) zone->tcontrol_tepl->IntegralVent = 0;
    zone->hot->NextTCalc.ICorrectionVent=(int)(zone->tcontrol_tepl->IntegralVent/100);
    if (! zone->gh_ctrl->f_IFactor)
        zone->tcontrol_tepl->IntegralVent = 0;

    creg_x = zone->hot->NextTCalc.PCorrectionVent+zone->hot->NextTCalc.ICorrectionVent+zone->hot->NextTCalc.dSumCalcF;
//Блокировка фрамуг при отоплении
    if ((zone->tcontrol_tepl->TVentCritery < creg_x)&&(!zone->tcontrol_tepl->StopI)&&(creg_x > 0)&&((zone->gh_ctrl->f_PFactor%100)>89))
    {
        zone->tcontrol_tepl->IntegralVent = zone->tcontrol_tepl->TVentCritery-zone->hot->NextTCalc.PCorrectionVent-zone->hot->NextTCalc.dSumCalcF;
        zone->tcontrol_tepl->IntegralVent*=100;
        creg_x = zone->tcontrol_tepl->TVentCritery;
    }
    zone->tcontrol_tepl->TVentCritery = creg_x;
//	if (!SameSign(pGD_TControl_Tepl->TVentCritery,pGD_TControl_Tepl->LastTVentCritery))
//	  	pGD_TControl_Tepl->StopVentI = 0;
    if (zone->tcontrol_tepl->StopVentI>4)
    {
        zone->tcontrol_tepl->IntegralVent = zone->tcontrol_tepl->SaveIntegralVent;
    }
    if (zone->tcontrol_tepl->StopVentI>3)
    {
        zone->tcontrol_tepl->SaveIntegralVent = zone->tcontrol_tepl->IntegralVent;
        CorrectionRule(0,100,500,0, creg_y, &creg_z);
        if (zone->tcontrol_tepl->AbsMaxVent>0)
            creg_z += zone->tcontrol_tepl->AbsMaxVent;
        if (zone->tcontrol_tepl->TVentCritery > creg_z)
        {
            zone->tcontrol_tepl->SaveIntegralVent = creg_z-zone->hot->NextTCalc.PCorrectionVent - zone->hot->NextTCalc.dSumCalcF;
            zone->tcontrol_tepl->SaveIntegralVent*=100;
        }
        creg_y = -creg_y;
        CorrectionRule(0,100,500,0, creg_y, &creg_z);
        creg_z++;
        if (zone->tcontrol_tepl->TVentCritery< -creg_z)
        {
            zone->tcontrol_tepl->SaveIntegralVent = -creg_z-zone->hot->NextTCalc.PCorrectionVent - zone->hot->NextTCalc.dSumCalcF;
            zone->tcontrol_tepl->SaveIntegralVent*=100;
        }
    }
    zone->hot->NextTCalc.TVentCritery = zone->tcontrol_tepl->TVentCritery;

}

/*------------------------------------------------------*/
void SetMixValvePosition(const zone_t *zone)
{
    for (int contour_idx = 0; contour_idx<cSWaterKontur; contour_idx++)
    {
        const contour_t ctr = make_contour_ctx(zone, contour_idx);

        if (YesBit(ctr.hand[cHSmMixVal].RCS, cbManMech))
            continue;
        int16_t *IntVal = &zone->tcontrol_tepl->IntVal[contour_idx];
        if (! ctr.tcontrol->PumpStatus)
        {
            ctr.hand[cHSmMixVal].Position = 0;
            continue;
        }

        ctr.tcontrol->TPause = MAX(ctr.tcontrol->TPause, 0); // pGD_TControl_Tepl->Kontur[ByteX].TPause = 0;
        if (YesBit(zone->tcontrol_tepl->MechBusy[contour_idx].RCS, cMSBusyMech))
            continue;
        if (ctr.tcontrol->TPause)
        {
            ctr.tcontrol->TPause--;
            continue;
        }
        ctr.tcontrol->TPause = cMinPauseMixValve;

        int creg_x = ctr.hot->Do - ctr.tcontrol->SensValue;
        //(*IntVal)=(*IntVal)+IntX;
        long long_y = zone->const_mech->ConstMixVal[contour_idx].v_PFactor;
        long_y = long_y * creg_x;//(*IntVal);
        int creg_y = (int16_t)(long_y/10000);
        //if (!IntY) continue;
        int creg_z = (*IntVal)/100;
        //IntZ=(*(pGD_Hot_Hand_Kontur+cHSmMixVal)).Position;
        creg_z += creg_y;
        if (creg_z > 100)
        {
            (*IntVal)=(100 - creg_y)*100;
            creg_z = 100;
        }
        else if (creg_z < 0)
        {
            (*IntVal)=(-creg_y) * 100;
            creg_z = 0;
        }
        else
        {
            (*IntVal)+=(int16_t)((((long)creg_x) * zone->const_mech->ConstMixVal[contour_idx].v_IFactor)/100);
        }

        //ogrMax(&IntZ,100);//if (IntZ>100) IntZ = 100;
        //ogrMin(&IntZ,0);//if (IntZ<0)	IntZ = 0;
        ctr.hand[cHSmMixVal].Position=(char)creg_z;
    }
}

void DoPumps(const zone_t *zone)
{
    for (int i = 0; i < cSWaterKontur; i++)
    {
        #warning "WTF: are these are pumps ?"
        if (! YesBit(zone->hand[cHSmPump + i].RCS, cbManMech))
            zone->hand[cHSmPump + i].Position = zone->tcontrol_tepl->Kontur[i].PumpStatus;
    }
}

//#warning вкл воздушного обогревателя !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void DoVentCalorifer(const zone_t *zone)
{

//	if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,cbManMech)) return;
//		(*(pGD_Hot_Hand+cHSmHeat)).Position = pGD_TControl_Tepl->Calorifer;
//
//
    if (! YesBit( zone->hand[cHSmVent].RCS, cbManMech))   // было так
    {
        zone->hand[cHSmVent].Position = zone->tcontrol_tepl->Vent + (zone->tcontrol_tepl->OutFan<<1);
    }
//	if (!(YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,(/*cbNoMech+*/cbManMech))))
//	{
//
//		(*(pGD_Hot_Hand+cHSmHeat)).Position = pGD_TControl_Tepl->Calorifer;
//	}
}


//#warning вкл подсветки !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void DoLights(const zone_t *zone)
{
    if (YesBit( zone->hand[cHSmLight].RCS, cbManMech)) return;
//	pGD_Hot_Hand[cHSmLight].Position = 0;
    zone->hand[cHSmLight].Position = zone->tcontrol_tepl->LightValue;
}

/*void	DoPoisen(void)
{
    if (YesBit((*(pGD_Hot_Hand+cHSmPoise)).RCS,(cbManMech))) return;
    pGD_Hot_Hand[cHSmPoise].Position = 0;
        if (pGD_Hot_Tepl->AllTask.Poise)
            pGD_Hot_Hand[cHSmPoise].Position = 1;
} */

void SetSensOnMech(const zone_t *zone)
{
    for (int i = 0;i<cSRegCtrl;i++)
        zone->tcontrol_tepl->MechBusy[i].Sens = 0;

    zone->tcontrol_tepl->MechBusy[cHSmWinN].Sens=&zone->hot->InTeplSens[cSmWinNSens];
    zone->tcontrol_tepl->MechBusy[cHSmWinS].Sens=&zone->hot->InTeplSens[cSmWinSSens];
    zone->tcontrol_tepl->MechBusy[cHSmScrTH].Sens=&zone->hot->InTeplSens[cSmScreenSens];
/*	if ((YesBit((*(pGD_Hot_Hand+cHSmWinS)).RCS,(cbManMech))))
    {
    if 	((pGD_TControl_Tepl->FramUpdate[1])&&(abs((char)(pGD_Hot_Tepl->InTeplSens[cSmWinSSens].Value)-(*(pGD_Hot_Hand+cHSmWinS)).Position)>GD.TuneClimate.f_MaxAngle))
        {
            (*(pGD_Hot_Hand+cHSmWinS)).Position=(char)pGD_Hot_Tepl->InTeplSens[cSmWinSSens].Value;
            SetBit((*(pGD_Hot_Hand+cHSmWinS)).RCS,cbResetMech);
        }
        pGD_TControl_Tepl->Kontur[cSmWindowUnW+GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinS)).Position))*10;
    }
    if ((YesBit((*(pGD_Hot_Hand+cHSmWinN)).RCS,(cbManMech))))
    {
        if 	((pGD_TControl_Tepl->FramUpdate[0])&&(abs((char)(pGD_Hot_Tepl->InTeplSens[cSmWinNSens].Value)-(*(pGD_Hot_Hand+cHSmWinN)).Position)>GD.TuneClimate.f_MaxAngle))
        {
            (*(pGD_Hot_Hand+cHSmWinN)).Position=(char)pGD_Hot_Tepl->InTeplSens[cSmWinNSens].Value;
            SetBit((*(pGD_Hot_Hand+cHSmWinN)).RCS,cbResetMech);
        }
        pGD_TControl_Tepl->Kontur[cSmWindowUnW+1-GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinN)).Position))*10;
    }
*/
}

//!!!Оптимизация
void ClearAllAlarms(void)
{
    char alarm_idx;
    char zone_idx;
    for (zone_idx = 0;zone_idx<NZONES;zone_idx++)
    {
        const zone_t zone = make_zone_ctx(zone_idx);

        for (alarm_idx = 0; alarm_idx < cSRegCtrl; alarm_idx++)
        {
            ClrBit(zone.tcontrol_tepl->MechBusy[alarm_idx].RCS, cMSAlarm);
            zone.tcontrol_tepl->MechBusy[alarm_idx].TryMove = 0;
            zone.tcontrol_tepl->MechBusy[alarm_idx].TryMes = 0;
        }
    }
    #warning "here was a statement with no effect - disabled"
//  for (alarm_idx = 0;alarm_idx<MAX_ALARMS;alarm_idx++)
//      _GD.TControl.Tepl[zone_idx].Alarms[alarm_idx];
}

void SetAlarm(void)
{
    for (int zone_idx = 0;zone_idx<_GD.Control.ConfSTepl;zone_idx++)
        write_output_bit(zone_idx,cHSmAlarm,1,0);

    for (int zone_idx = 0;zone_idx<_GD.Control.ConfSTepl;zone_idx++)
    {
        zone_t zone = make_zone_ctx(zone_idx);

        zone.tcontrol_tepl->bAlarm = 0;
        if ((YesBit(zone.hot->RCS,(cbNoTaskForTepl+cbNoSensingTemp+cbNoSensingOutT)))
            //	||(YesBit(pGD_Hot_Tepl->InTeplSens[cSmTSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            ||(YesBit(zone.hot->InTeplSens[cSmWaterSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens))))
        {
            write_output_bit(zone_idx,cHSmAlarm,0,0);
            zone.tcontrol_tepl->bAlarm = 100;
        }

        if (getTempHeatAlarm(&zone, zone_idx)  ==  0)
        {
            write_output_bit(zone_idx,cHSmAlarm,0,0);
            zone.tcontrol_tepl->bAlarm = 100;
        }

        if (getTempVentAlarm(&zone, zone_idx)  ==  0)
        {
            write_output_bit(zone_idx,cHSmAlarm,0,0);
            zone.tcontrol_tepl->bAlarm = 100;
        }

        for (int i = 0;i<cConfSSens;i++)
        {
            if (YesBit(zone.hot->InTeplSens[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            {
                write_output_bit(zone_idx,cHSmAlarm,0,0);
                zone.tcontrol_tepl->bAlarm = 100;
            }
        }
    }
    for (int i = 0;i<cConfSMetSens;i++)
    {
        if (YesBit(_GD.Hot.MeteoSensing[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
        {
            write_output_bit(cSmZone1,cHSmAlarm,0,0);
            _GD.TControl.Zones[cSmZone1].bAlarm = 100;
        }
    }

}

void SetDiscreteOutputs(const zone_t *zone)
{
    int nLight;
    char tMaxLight;

    if (! YesBit(zone->hand[cHSmAHUSpeed1].RCS, cbManMech))
        zone->hand[cHSmAHUSpeed1].Position = zone->hot->Kontur[cSmKontur4].Do/10;
    if (! YesBit(zone->hand[cHSmAHUSpeed2].RCS, cbManMech))
        zone->hand[cHSmAHUSpeed2].Position = zone->hot->Kontur[cSmKontur4].Do/10;

    for (int i = cHSmPump;i<cHSmRegs;i++)
    {
        //if ((ByteX == cHSmSIOVals)||(ByteX == cHSmLight)) continue;
        if ((i == cHSmSIOPump)||(i == cHSmSIOVals)||(i == cHSmLight)) continue;

        write_output_bit(zone->idx, i, 1, 0);

        if (YesBit(zone->hand[i].Position,0x01))
            write_output_bit(zone->idx, i, 0,0);
        if (((i == cHSmHeat)||(i == cHSmVent)) && YesBit(zone->hand[i].Position,0x02))
            write_output_bit(zone->idx, i, 0,1);
    }


    nLight = 0;
    if ((uchar) zone->hand[cHSmLight].Position > 100)
        zone->hand[cHSmLight].Position = 100;
    if ((zone->hot->AllTask.DoTHeat)||YesBit(zone->hand[cHSmLight].RCS,cbManMech))
    {
        nLight=(zone->hand[cHSmLight].Position-50)/10+2;
        if (nLight<1)
            nLight = 1;
    }
    bool is_light_on = 0;
    if (nLight>1)
    {
        write_output_bit(zone->idx, cHSmLight, 0, 0);
        is_light_on = 1;
    }
    tMaxLight = 8;

    switch (zone->gh_ctrl->sLight)
    {
    case 2:
        if (nLight == 7)
            nLight = 0x04;
        else if (nLight>1)
            nLight = 0x02;
        else if (nLight == 1)
            nLight = 0x01;
        break;
    case 1:
        if (nLight == 7)
            nLight = 1;
        else
            nLight = 0;
        break;
    case 8:
        if (nLight!=1)
            nLight = 0;
        break;
    case 6:
        break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        tMaxLight = zone->gh_ctrl->sLight-10;
        if (ctx.fLightPause>CONTROL_LIGHT_DELAY*8)
            ctx.fLightPause = CONTROL_LIGHT_DELAY*8;
        if (ctx.fLightPause<0)
            ctx.fLightPause = 0;
        if (is_light_on)
        {
            nLight=~(0xff<<(ctx.fLightPause/CONTROL_LIGHT_DELAY));
            ctx.fLightPause++;
        }
        else
        {
            nLight=~(0xff>>(ctx.fLightPause/CONTROL_LIGHT_DELAY));
            ctx.fLightPause--;
        }
        break;
    default:
        nLight = 0;
        break;

    }

    for (int i = 0;i<tMaxLight;i++)
    {
        if (YesBit(nLight,(0x01<<i)))
            write_output_bit(zone->idx, cHSmLight, 0, i+1);

    }

/*	if (YesBit((*(pGD_Hot_Hand+cHSmVent)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmVent,0,0);
    if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmHeat,0,0);*/
    int i = 1;
    if (zone->gh_ctrl->co_model>=2)
        i = 2;

    if (zone->tcontrol_tepl->SetupRegs[0].On && zone->gh_ctrl->co_model)
        write_output_bit(zone->idx, cHSmCO2, 0, i);

    // насос
    //__SetBitOutReg(fnTepl,cHSmSIOPump,1,0);
    if (YesBit(zone->hand[cHSmSIOPump].Position,0x01))
        write_output_bit(zone->idx, cHSmSIOPump, 0, 0);

    for (int i = 0;i<4;i++)
    {
        if (YesBit(zone->hand[cHSmSIOVals].Position, 1 << i))
            write_output_bit(zone->idx, cHSmSIOVals, 0, i);
    }

#ifdef AGAPOVSKIY_DOUBLE_VALVE
    if (YesBit( zone->hand[cHSmSIOVals].Position,0x02))
        write_output_bit(zone->idx, cHSmAHUVals, 0, 0);
#endif
    for (int i = 0;i<5;i++)
    {
        if (_GD.Hot.Regs[i])
            write_output_bit(zone->idx, i+cHSmRegs, 0, 0);
    }
}

static void do_contour_mechanics(const zone_t *zone, int mech_idx)
{
    eMechanic *mech = &zone->hand[mech_idx];

    const eConstMixVal *ConstMechanic_Mech = &_GD.ConstMechanic[zone->idx].ConstMixVal[mech_idx];

    char fErr;

//		pGD_Hot_Hand_Kontur = pGD_Hot_Hand+ByteX;
    eMechBusy *mechbusy=&(zone->tcontrol_tepl->MechBusy[mech_idx]);

    if (mech->Position>100)
        mech->Position = 100;
    if (mech->Position<0)
        mech->Position = 0;

    if ((mech_idx == cHSmAHUSpeed1))
    {
        write_output_register(mech->Position, mtRS485, zone->mech_cfg->RNum[mech_idx], &fErr, &zone->fanblock[0].FanData);
        return;
    }
/*		GD.FanBlock[fnTepl][0].FanData[0].ActualSpeed = fnTepl*5;
    GD.FanBlock[fnTepl][0].FanData[1].ActualSpeed = fnTepl*5+1;
    GD.FanBlock[fnTepl][1].FanData[1].ActualSpeed = fnTepl*5+2;
    GD.FanBlock[fnTepl][1].FanData[2].ActualSpeed = fnTepl*5+3;
*/
    if ((mech_idx == cHSmAHUSpeed2))
    {
//			Sound;
        write_output_register(mech->Position, mtRS485, zone->mech_cfg->RNum[mech_idx], &fErr, &zone->fanblock[1].FanData);
        return;
    }


    if ((mech_idx == cHSmCO2)&&(zone->gh_ctrl->co_model == 1))
        return;

    write_output_bit(zone->idx, mech_idx,1,0);
    write_output_bit(zone->idx, mech_idx,1,1);

    ClrBit(mechbusy->RCS,cMSBusyMech);
    int byte_y = 0;
    if ((!YesBit(mechbusy->RCS,cMSAlarm))&&(mechbusy->Sens)&&(!YesBit(mechbusy->Sens->RCS,cbNoWorkSens))&&(_GD.TuneClimate.f_MaxAngle))
    {
        mechbusy->PauseMech = 10;
        if (YesBit(mechbusy->RCS,cMSFreshSens))
        {
            mechbusy->PauseMech = 0;
            ClrBit(mechbusy->RCS,cMSFreshSens);
//				if ((MBusy->PrevDelta>10)&&(MBusy->Sens->Value-MBusy->PrevTask>10))
//					MBusy->CalcTime=(((long)MBusy->CalcTime)*MBusy->PrevDelta/(MBusy->Sens->Value-MBusy->PrevTask));
            //(MBusy->PrevPosition-MBusy->CurrPosition)
            if (mechbusy->TryMove>4)
            {
                SetBit(mechbusy->RCS,cMSAlarm);
                return;
            }
            if (mechbusy->PrevTask == mech->Position*10)
            {
                int creg_y = _GD.TuneClimate.f_MaxAngle*10;
                creg_y = CLAMP(10, creg_y, 50);
                if (abs(mechbusy->Sens->Value - mechbusy->PrevTask) > creg_y)
                {
                    mechbusy->TryMes++;
                    if (mechbusy->TryMes>4)
                    {
                        mechbusy->TryMes = 0;
                        mechbusy->TryMove+=(abs(mechbusy->Sens->Value-mechbusy->PrevTask) / creg_y);
                    }
                    else
                    {
                        return;
                    }
                }

                long long_x = mechbusy->Sens->Value;
                long_x *= ConstMechanic_Mech->v_TimeMixVal;//MBusy->CalcTime;
                long_x /= 1000;

                if (abs(mechbusy->Sens->Value-mechbusy->PrevTask) <= creg_y)
                {
                    mechbusy->TryMove = 0;
                    if (mechbusy->PrevDelta>10)
                    {
                        mechbusy->TimeRealMech++;
                    }
                    if (mechbusy->PrevDelta<-10)
                    {
                        mechbusy->TimeRealMech--;
                    }
                }
                else
                {
                    mechbusy->TimeRealMech=(int)long_x;
                }
            }
            mechbusy->PrevTask = mech->Position*10;
        }
        //else return;
    }
//Выход из паузы при блокировке на крайних положениях
    if (YesBit(mechbusy->RCS,cMSBlockRegs)
        &&((mech->Position>0)||(mechbusy->TimeSetMech>0))
        &&((mech->Position<100)||(mechbusy->TimeSetMech < ConstMechanic_Mech->v_TimeMixVal)))
    {
        ClrBit(mechbusy->RCS,cMSBlockRegs);
        mechbusy->TimeRealMech = mechbusy->TimeSetMech;
        byte_y++;
    }
//Расчет

    if ((!mechbusy->PauseMech)||(YesBit(mech->RCS,cbManMech)))
    {
        long long_x = mech->Position;
        long_x *= ConstMechanic_Mech->v_TimeMixVal;
        long_x /= 100;
        mechbusy->TimeSetMech=(int)(long_x);
/*			if (YesBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech))
        {
            MBusy->TimeRealMech = MBusy->TimeSetMech;
            ClrBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech);
            ByteY++;
        }*/
        if (!YesBit(mech->RCS,cbManMech))
        {
            if (! mech->Position)
            {
                SetBit(mechbusy->RCS,cMSBlockRegs);
                mechbusy->TimeRealMech+= ConstMechanic_Mech->v_TimeMixVal/4;
            }
            if (mech->Position == 100)
            {
                SetBit(mechbusy->RCS,cMSBlockRegs);
                mechbusy->TimeRealMech -= ConstMechanic_Mech->v_TimeMixVal/4;
            }
        }
    }
    if (mechbusy->TimeSetMech>mechbusy->TimeRealMech)
    {
        mechbusy->TimeRealMech++;
        write_output_bit(zone->idx, mech_idx, 0, 1);
        SetBit(mechbusy->RCS,cMSBusyMech);
        //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
        byte_y++;
    }
    if (mechbusy->TimeSetMech<mechbusy->TimeRealMech)
    {
        mechbusy->TimeRealMech--;
        write_output_bit(zone->idx, mech_idx, 0, 0);
        SetBit(mechbusy->RCS,cMSBusyMech);
        //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
        byte_y++;
    }
    if (byte_y)
    {
        int creg_y = ConstMechanic_Mech->v_MinTim;
/*			if ((ByteX == cHSmWinN)||(ByteX == cHSmWinS))
        {
            ogrMin(&IntY,90);
            pGD_TControl_Tepl->FramUpdate[ByteX-cHSmWinN]=0;
        }*/
        creg_y = MAX(creg_y, 5);
        mechbusy->PauseMech = creg_y;
        if (YesBit(mechbusy->RCS,cMSBlockRegs))
            mechbusy->PauseMech = 150;
    }
    //	if (GD.Hot.Hand) continue;
    if (mechbusy->PauseMech)
    {
        mechbusy->PauseMech--;
//			if (!(YesBit(MBusy->RCS,cMSBlockRegs)))
//				SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);

    }
    mechbusy->PauseMech = MAX(mechbusy->PauseMech, 0);// MBusy->PauseMech = 0;
}

void DoMechanics(const zone_t *zone)
{
    for (int i = cHSmMixVal;i<cHSmPump;i++)
    {
        do_contour_mechanics(zone, i);
    }
}

void SetMeteo(void)
{
    uint16_t tMes,i;
    for (i = 0;i<cConfSMetSens;i++)
    {
        tMes = _GD.Hot.MeteoSensing[i].Value;
        if (((tMes<=_GD.TControl.MeteoSensing[i]+NameSensConfig[cConfSSens+i].DigitMidl)&&(tMes>=_GD.TControl.MeteoSensing[i]-NameSensConfig[cConfSSens+i].DigitMidl))||(_GD.TControl.TimeMeteoSensing[i]>20))
        {
            _GD.TControl.TimeMeteoSensing[i]=0;
            _GD.TControl.MeteoSensing[i]=tMes;
        }
        else if (_GD.TControl.TimeMeteoSensing[i]<30)
        {
            _GD.TControl.TimeMeteoSensing[i]++;
        }
    }

    const zone_t first_gh = make_zone_ctx(0);

    if (first_gh.tcontrol_tepl->SnowTime >= _GD.TuneClimate.MinRainTime)
        _GD.TControl.bSnow=!_GD.TControl.bSnow;
    if (((_GD.TControl.MeteoSensing[cSmRainSens]<cMinRain)&&(_GD.TControl.bSnow))
        ||((_GD.TControl.MeteoSensing[cSmRainSens]>cMinRain)&&(!_GD.TControl.bSnow)))
    {
        first_gh.tcontrol_tepl->SnowTime++;
//		GD.TControl.Tepl[0].SnowTime = 10;
    }
    else
    {
        first_gh.tcontrol_tepl->SnowTime = 0;
    }

    if ((_GD.TControl.MeteoSensing[cSmOutTSens]<c_SnowIfOut)&&(_GD.TControl.bSnow))
        SetBit(_GD.TControl.bSnow,0x02);
    first_gh.tcontrol_tepl->SumSens+=_GD.TControl.MeteoSensing[cSmFARSens];//GD.Hot.MeteoSens[cSmFARSens].Value;
    first_gh.tcontrol_tepl->TimeSumSens++;
    if (first_gh.tcontrol_tepl->TimeSumSens>=15)
    {
        first_gh.tcontrol_tepl->SensHourAgo = first_gh.tcontrol_tepl->SensHalfHourAgo;
        first_gh.tcontrol_tepl->SensHalfHourAgo = first_gh.tcontrol_tepl->SumSens/first_gh.tcontrol_tepl->TimeSumSens;
        first_gh.tcontrol_tepl->SumSens = 0;
        first_gh.tcontrol_tepl->TimeSumSens = 0;
    }
}

void SetCO2(void)
{
//	if (!(pGD_MechConfig->RNum[cHSmCO2])) return;  // if hand mode exit
//	if (pGD_Control_Tepl->co_model  ==  2)
//		{
//	if (pGD_Hot_Tepl->AllTask.DoCO2 > pGD_Hot_Tepl->InTeplSens[cSmCOSens].Value)
//	{
//		(*(pGD_Hot_Hand+cHSmCO2)).Position = 1;
//		pGD_TControl_Tepl->COPosition = 1;
//	}
//		}

/*	pGD_TControl_Tepl->COPosition = 0;
    (*(pGD_Hot_Hand+fHSmReg)).Position = 0;
    pGD_Hot_Tepl->AllTask.DoCO2
    pGD_Hot_Tepl->InTeplSens[cSmCOSens].Value;
    co2On
    co2Fram1
    co2Fram2
    co2Off


    char bZad;
    if (!(pGD_MechConfig->RNum[cHSmLight])) return;  // if hand mode exit
    IntZ = 0;
    pGD_TControl_Tepl->LightPauseMode--;
    if ((pGD_TControl_Tepl->LightPauseMode<0)||(pGD_TControl_Tepl->LightPauseMode>GD.TuneClimate.l_PauseMode))
        pGD_TControl_Tepl->LightPauseMode = 0;
    ClrDog;
    bZad = 0;		// if bZab = 0 calc sun sensor
    if (pGD_TControl_Tepl->LightPauseMode) bZad = 1;  // if bZad = 1 don't calc sun senasor
    pGD_TControl_Tepl->LightMode = pGD_Hot_Tepl->AllTask.ModeLight * pGD_Hot_Tepl->AllTask.Light;
    if (!bZad)
    {
        if (GD.Hot.Zax-60>GD.Hot.Time)
            pGD_TControl_Tepl->LightMode = 0;
        if (GD.TControl.Tepl[0].SensHalfHourAgo>GD.TuneClimate.l_SunOn50)  // sun > 50% then off light
            pGD_TControl_Tepl->LightMode = 0;

        if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn50)
        {
            IntY = GD.Hot.MidlSR;
            CorrectionRule(GD.TuneClimate.l_SunOn100,GD.TuneClimate.l_SunOn50,50,0);
            pGD_TControl_Tepl->LightMode = 100-IntZ;
        }
    }
    if (pGD_TControl_Tepl->LightMode!=pGD_TControl_Tepl->OldLightMode)
    {
        if (!(((int)pGD_TControl_Tepl->LightMode)*((int)pGD_TControl_Tepl->OldLightMode)))
        {
            pGD_TControl_Tepl->DifLightMode = pGD_TControl_Tepl->LightMode-pGD_TControl_Tepl->OldLightMode;
            pGD_TControl_Tepl->LightPauseMode = GD.TuneClimate.l_PauseMode;
//			pGD_TControl_Tepl->LightExtraPause = o_DeltaTime;
        }
        else
        {
            pGD_TControl_Tepl->LightPauseMode = GD.TuneClimate.l_SoftPauseMode;
        }
    }
    pGD_TControl_Tepl->OldLightMode = pGD_TControl_Tepl->LightMode;
    // new
    if (pGD_Hot_Tepl->AllTask.ModeLight  ==  2)    		// авто досветка
    {
        if (pGD_Hot_Tepl->AllTask.Light < pGD_TControl_Tepl->LightMode)
            pGD_TControl_Tepl->LightValue = pGD_Hot_Tepl->AllTask.Light;
        else
            pGD_TControl_Tepl->LightValue = pGD_TControl_Tepl->LightMode;
    }
    else
        pGD_TControl_Tepl->LightValue = pGD_TControl_Tepl->LightMode;
    // new

    if (pGD_TControl_Tepl->LightValue > 100)
        pGD_TControl_Tepl->LightValue = 100;    */
}


//#warning light Досветка !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void SetLighting(const zone_t *zone)
{
    char bZad;

    if (! zone->mech_cfg->RNum[cHSmLight]) return;  // if hand mode exit
    int creg_z = 0;

//	if(SameSign(IntY,IntZ)) pGD_TControl_Tepl->LightExtraPause = 0;

    zone->tcontrol_tepl->LightPauseMode--;

    if ((zone->tcontrol_tepl->LightPauseMode < 0)
        || (zone->tcontrol_tepl->LightPauseMode > _GD.TuneClimate.l_PauseMode))
        zone->tcontrol_tepl->LightPauseMode = 0;

    bZad = 0;     // if bZab = 0 calc sun sensor

    if (zone->tcontrol_tepl->LightPauseMode)
        bZad = 1;  // if bZad = 1 don't calc sun senasor

// old
//	if ((pGD_Hot_Tepl->AllTask.ModeLight<2))//&&(!bZad))	// если режим досветки не авто
//	{
//		pGD_TControl_Tepl->LightMode = pGD_Hot_Tepl->AllTask.ModeLight*pGD_Hot_Tepl->AllTask.Light;
//		bZad = 1;
//	}

    if (zone->hot->AllTask.ModeLight < 2)
    {
        zone->tcontrol_tepl->LightMode = zone->hot->AllTask.ModeLight * zone->hot->AllTask.Light;
        bZad = 1;
    }

    if (!bZad)
    {
        if (_GD.Hot.Zax-60>_GD.Hot.Time)
            zone->tcontrol_tepl->LightMode = 0;

        // XXX: using greenhouse 0 ?
        if (_GD.TControl.Zones[0].SensHalfHourAgo > _GD.TuneClimate.l_SunOn50)  // sun > 50% then off light
            zone->tcontrol_tepl->LightMode = 0;

        if (_GD.TControl.Zones[0].SensHalfHourAgo < _GD.TuneClimate.l_SunOn50)
        {
//			pGD_TControl_Tepl->LightMode = 50;
            int creg_y = _GD.Hot.MidlSR;
            int creg_z;
            CorrectionRule(_GD.TuneClimate.l_SunOn100,
                           _GD.TuneClimate.l_SunOn50,
                           50,
                           0,
                           creg_y,
                           &creg_z);
            zone->tcontrol_tepl->LightMode = 100 - creg_z;
        }

//		if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn100)
//			pGD_TControl_Tepl->LightMode = 100;
    }
    if (zone->tcontrol_tepl->LightMode != zone->tcontrol_tepl->OldLightMode)
    {
        if (! (((int)zone->tcontrol_tepl->LightMode)*((int)zone->tcontrol_tepl->OldLightMode)))
        {
            zone->tcontrol_tepl->DifLightMode = zone->tcontrol_tepl->LightMode - zone->tcontrol_tepl->OldLightMode;
            zone->tcontrol_tepl->LightPauseMode = _GD.TuneClimate.l_PauseMode;
//			pGD_TControl_Tepl->LightExtraPause = o_DeltaTime;
        }
        else
        {
            zone->tcontrol_tepl->LightPauseMode = _GD.TuneClimate.l_SoftPauseMode;
        }
    }
    zone->tcontrol_tepl->OldLightMode = zone->tcontrol_tepl->LightMode;

//	pGD_TControl_Tepl->LightExtraPause--;
//	if (pGD_TControl_Tepl->LightExtraPause>0) return;
//	pGD_TControl_Tepl->LightExtraPause = 0;

    // new
    if (zone->hot->AllTask.ModeLight  ==  2)           // авто досветка
    {
        if (zone->hot->AllTask.Light < zone->tcontrol_tepl->LightMode)
            zone->tcontrol_tepl->LightValue = zone->hot->AllTask.Light;
        else
            zone->tcontrol_tepl->LightValue = zone->tcontrol_tepl->LightMode;
    }
    else
        zone->tcontrol_tepl->LightValue = zone->tcontrol_tepl->LightMode;
    // new

    if (zone->tcontrol_tepl->LightValue > 100)
        zone->tcontrol_tepl->LightValue = 100;

    //old
    //pGD_TControl_Tepl->LightValue = pGD_TControl_Tepl->LightMode;		// значение досветки

}

void SetTepl(const zone_t *zone)
{
/***********************************************************************
--------------Вычисление изменения показаний датчика температуры-------
************************************************************************/

/***********************************************************************/
    if (! zone->hot->AllTask.NextTAir)
        zone->hot->RCS |= cbNoTaskForTepl;

//	if(!pGD_Hot_Tepl->InTeplSens[cSmTSens].Value)
//		SetBit(pGD_Hot_Tepl->RCS,cbNoSensingTemp);
// NEW
    if (! zone->hot->InTeplSens[cSmTSens1].Value)
        zone->hot->RCS |= cbNoSensingTemp;
    if (! zone->hot->InTeplSens[cSmTSens2].Value)
        zone->hot->RCS |= cbNoSensingTemp;
    if (! zone->hot->InTeplSens[cSmTSens3].Value)
        zone->hot->RCS |= cbNoSensingTemp;
    if (! zone->hot->InTeplSens[cSmTSens4].Value)
        zone->hot->RCS |= cbNoSensingTemp;

//	if(!pGD_Hot_Tepl->RCS)
    {
        AllTaskAndCorrection(zone);
        LaunchCalorifer(zone);

        __cNextTCalc(zone);

        DecPumpPause(zone);

        //SetUpSiod(fnTepl);


        InitScreen(zone, cTermHorzScr);
        InitScreen(zone, cSunHorzScr);
        InitScreen(zone, cTermVertScr1);
        InitScreen(zone, cTermVertScr2);
        InitScreen(zone, cTermVertScr3);
        InitScreen(zone, cTermVertScr4);

        SetReg(zone, cHSmCO2, zone->hot->AllTask.DoCO2, zone->hot->InTeplSens[cSmCOSens].Value);

        zone->hot->OtherCalc.MeasDifPress = _GD.TControl.MeteoSensing[cSmPresureSens]-_GD.TControl.MeteoSensing[cSmPresureSens+1];

        if (! zone->hot->OtherCalc.MeasDifPress)
            zone->hot->OtherCalc.MeasDifPress = 1;
        if ((! _GD.TControl.MeteoSensing[cSmPresureSens]) || (! _GD.TControl.MeteoSensing[cSmPresureSens+1]))
            zone->hot->OtherCalc.MeasDifPress = 0;

        SetReg(zone, cHSmPressReg, zone->hot->AllTask.DoPressure, zone->hot->OtherCalc.MeasDifPress);
        LaunchVent(zone);
        SetLighting(zone);
        SetCO2();               // CO2
    }
}

//Есть место для оптимизации!!!!!!!!!!!!!!!!!!!!
static void SubConfig(const zone_t *zone)
{
    for (int i = 0;i<cHSmPump;i++)
    {
        contour_t ctr = make_contour_ctx(zone, i);

        #warning "WTF: cSKontur < cHSmPump"
        if (i<cSKontur)
        {
            ctr.tcontrol->Separate = CheckSeparate(&ctr);
            ctr.tcontrol->MainZone = CheckMain(&ctr);

            zone_t main_gh = make_zone_ctx(ctr.tcontrol->MainZone);
            contour_t main_contour = make_contour_ctx(&main_gh, i);

            *ctr.hand = *main_contour.hand;

            if (i<cSWaterKontur)
            {
                ctr.tcontrol->SensValue = zone->hot->InTeplSens[i+cSmWaterSens].Value;

                zone->hand[i+cHSmPump] = main_gh.hand[i+cHSmPump];
                ctr.tcontrol->SensValue = main_contour.tcontrol->SensValue;
            }
        }

    }

}

void Configuration(void)
{
    for (int zone_idx = 0; zone_idx<NZONES; zone_idx++)
    {
        zone_t zone = make_zone_ctx(zone_idx);
        SubConfig(&zone);
    }
}
#ifdef Vitebsk
void    TransferWaterToBoil(void)
{
    int creg_x = _GD.Hot.MaxReqWater/100; //Делаем запас на 5 градусов
//	IntX = IntX/100;
    creg_x++;
    int creg_y;
//	IntY = 0;
    switch (creg_x)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        creg_y = 1;
        break;

    case 4:
    case 5:
    case 6:
        creg_y = creg_x - 2;
        break;
    case 7:
        creg_y = 0;
        break;
    case 8:
    case 9:
    case 10:
        creg_y = creg_x - 3;
        break;
    default:
        creg_y = 7;
        break;
    }

    if (YesBit(creg_y,0x01))
        write_output_bit(0,cHSmRegs+4,0,0);
    if (YesBit(creg_y,0x02))
        write_output_bit(0,cHSmRegs+4,0,1);
    if (YesBit(creg_y,0x04))
        write_output_bit(0,cHSmRegs+4,0,2);
//	GD.Hot.Demon = IntY;

}
#endif
/*------------------------------------------------
        Управлюща программа
        Вариант "Ромашка"
        Создана от 14.04.04
--------------------------------------------------*/

void saveSettings(char tCTepl, int *dst)
{
    dst[0] = _GD.Hot.Zones[tCTepl].tempParamHeat;
    dst[1] = _GD.Hot.Zones[tCTepl].tempParamVent;
    dst[2] = _GD.Hot.Zones[tCTepl].tempHeat;
    dst[3] = _GD.Hot.Zones[tCTepl].tempVent;
    dst[4] = _GD.Hot.Zones[tCTepl].newsZone;
}

void loadSettings(char tCTepl, const int *src)
{
    _GD.Hot.Zones[tCTepl].tempParamHeat = src[0];
    _GD.Hot.Zones[tCTepl].tempParamVent = src[1];
    _GD.Hot.Zones[tCTepl].tempHeat = src[2];
    _GD.Hot.Zones[tCTepl].tempVent = src[3];
    _GD.Hot.Zones[tCTepl].newsZone = src[4];
}

void control_pre(void)
{
    Configuration();
    SetAlarm();

    for (int zone_idx = 0;zone_idx<NZONES;zone_idx++)
    {
        zone_t zone = make_zone_ctx(zone_idx);

        SetSensOnMech(&zone);
        DoMechanics(&zone);


        SetDiscreteOutputs(&zone);

        SetUpSiod(&zone);
        DoSiod(&zone);

        DoPumps(&zone);
//			CheckReadyMeasure();
        DoVentCalorifer(&zone);
        DoLights(&zone);
//			DoPoisen();
        RegWorkDiskr(&zone, cHSmCO2);
        RegWorkDiskr(&zone, cHSmPressReg);
#ifdef Vitebsk
        TransferWaterToBoil();
#endif
    }
}

void control_post(int second, bool is_transfer_in_progress)
{
    for (int zone_idx = 0;zone_idx<NZONES;zone_idx++)
    {
        zone_t zone = make_zone_ctx(zone_idx);
        SetMixValvePosition(&zone);
    }
    if (second == 20)
    {
        SetMeteo();
    }
    if (second == 40 || _GD.TControl.Delay)
    {
        if (is_transfer_in_progress)
        {
            _GD.TControl.Delay = 1;
        }
        else
        {
            // this should block reception, but it not - ethernet is not interrupt-driven)
            // WTF0.PORTNUM = 0;
            CheckMidlSr();
            _GD.TControl.Delay = 0;
            for (int zone_idx = 0;zone_idx<NZONES;zone_idx++)
            {
                int shadow[8];
                saveSettings(zone_idx, shadow);
                memclr(&_GD.Hot.Zones[zone_idx].ExtRCS,(
                                                    sizeof(char)*2+sizeof(eClimTask)+sizeof(eOtherCalc)+
                                                    sizeof(eNextTCalc)+sizeof(eKontur)*cSKontur+20));
                TaskTimer(1,zone_idx,zone_idx);
                int ttTepl = zone_idx;
                while ((!_GD.Hot.Zones[zone_idx].AllTask.NextTAir)&&(ttTepl))
                {
                    TaskTimer(1,--ttTepl,zone_idx);
                }
                loadSettings(zone_idx, shadow);
                TaskTimer(0,ttTepl,zone_idx);

                zone_t zone = make_zone_ctx(zone_idx);

                SetTepl(&zone);

                airHeat(&zone);
            }
            __sCalcKonturs();
            __sMechWindows();
            __sMechScreen();
            for (int zone_idx = 0;zone_idx<NZONES;zone_idx++)
            {
                if (_GD.Hot.MaxReqWater<_GD.Hot.Zones[zone_idx].MaxReqWater)
                    _GD.Hot.MaxReqWater = _GD.Hot.Zones[zone_idx].MaxReqWater;
                bWaterReset[zone_idx]=0;
            }
        }
    }
    // this should unblock reception, but it not - ethernet is not interrupt-driven)
    // WTF0.PORTNUM = DEF_PORTNUM;
    ctx.MaskRas = bRasxod;
    if (ctx.TecPerRas > 2305)
    {
        ctx.TecPerRas = 2305;
        _GD.TControl.NowRasx = 0;
    }
    else
    {
        int creg_x = ctx.PastPerRas;
        if (ctx.TecPerRas > creg_x)
            creg_x = ctx.TecPerRas;
        _GD.TControl.NowRasx=(long)_GD.TuneClimate.ScaleRasx*(long)23040/(long)creg_x/100;
    }
    #warning "useless - volume is always zero"
    //GD.TControl.FullVol+=ctx.Volume;
    if ((!_GD.TControl.MeteoSensing[cSmMainTSens])||(!_GD.TControl.MeteoSensing[cSmMainTSens+1]))
    {
        _GD.Hot.HeatPower=(int)(((long)_GD.TControl.NowRasx)*(_GD.TControl.MeteoSensing[cSmMainTSens]-_GD.TControl.MeteoSensing[cSmMainTSens+1])/100);
        _GD.Hot.FullHeat=(int)((_GD.TControl.FullVol*_GD.TuneClimate.ScaleRasx/100)*(_GD.TControl.MeteoSensing[cSmMainTSens]-_GD.TControl.MeteoSensing[cSmMainTSens+1])/1000);
    }
    //ctx.Volume = 0;

    if (second < 60)
        return;

    airHeatTimers();
    MidlWindAndSr();
    WindDirect();


    if (_GD.Hot.Vosx != 0)
        ctx.settingsVosx = _GD.Hot.Vosx;
    if (_GD.Hot.Zax != 0)
        ctx.settingsZax = _GD.Hot.Zax;

    bNight = 1;
    if ((_GD.Hot.Time>=_GD.Hot.Vosx)&&(_GD.Hot.Time<_GD.Hot.Zax))
        bNight = 0;

    if (_GD.TControl.Date!=_GD.Hot.Date)      /*новые сутки*/
    {
        for (int zone_idx = 0;zone_idx<NZONES;zone_idx++)
        {
//			GD.TControl.Tepl[tCTepl].Functional = 1;
            _GD.TControl.Zones[zone_idx].TimeSIO = 0;
        }
        _GD.TControl.SumSun = 0;
        _GD.TControl.Date = _GD.Hot.Date;
        _GD.TControl.FullVol = 0;
    }
}

const zone_t make_zone_ctx(int zone_idx)
{
    zone_t ctx =
    {
        .idx = zone_idx,
        .hot = &_GD.Hot.Zones[zone_idx],
        .hand = _GD.Hot.Zones[zone_idx].HandCtrl,
        .tcontrol_tepl = &_GD.TControl.Zones[zone_idx],
        .gh_ctrl = &_GD.Control.Zones[zone_idx],
        .tctrl = &_GD.TControl,
        .mech_cfg = &_GD.MechConfig[zone_idx],
        .const_mech = &_GD.ConstMechanic[zone_idx],
        .strategies = _GD.Strategy[zone_idx],
        .fanblock = _GD.FanBlock[zone_idx],
    };

    return ctx;
}

const contour_t make_contour_ctx(const zone_t *zone, int contour_idx)
{
    contour_t ctx =
    {
        .link = *zone,
        .hot = &zone->hot->Kontur[contour_idx],
        .cidx = contour_idx,
        .tcontrol = &zone->tcontrol_tepl->Kontur[contour_idx],
        .hand = &zone->hand[contour_idx],
        .strategy = &zone->strategies[contour_idx],
    };

    return ctx;
}

void control_init(void)
{
    ClearAllAlarms();
    siodInit();
    airHeatInit();   // airHeat
}
