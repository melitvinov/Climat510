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

// XXX: these 3 'registers' are used in control wide and wild
control_regs_t creg;

int8_t  bWaterReset[16];


uchar       bNight;


const uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};

static control_ctx_t ctx;

const gh_ctx_t make_gh_ctx(int gh_idx);

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
int JumpNext(int Now,int Next,char Check, char Mull)
{
    long int var;
    if ((Check)&&((!Next)||(!Now)))
        return Now*Mull;
    creg.Z = Next-Now;
    var = creg.Z;
    var*=creg.X;
    if (!creg.Y)
        return Now*Mull;
    var/=creg.Y;
    return(Now+var)*Mull;
}

//int16_t TempOld, TempNew = 0;

void TaskTimer(char fsmTime, char fnTeplTimer, char fnTeplLoad)
{
    int8_t nTimer,sTimerNext,sTimerPrev,sTimerMin,sTimerMax;
    int MaxTimeStart,MinTimeStart,NextTimeStart,PrevTimeStart,tVal;
    eTimer *pGD_CurrTimer;
    eTimer *pGD_NextTimer;
    int16_t typeStartCorrection;
    int16_t nextTimer = 0;
    int16_t prevTimer = 0;
    _GDP.Hot_Tepl=&_GD.Hot.Tepl[fnTeplLoad];
    _GDP.Hot_Tepl->AllTask.TAir = 0;

    creg.Z= _GD.Hot.Time + fsmTime;
    creg.Z%=1440;
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
        if ((typeStartCorrection>=creg.Z)&&(NextTimeStart>typeStartCorrection))
        {
            NextTimeStart = typeStartCorrection;
            sTimerNext = nTimer;
        }
        if ((typeStartCorrection<creg.Z)&&(PrevTimeStart<typeStartCorrection))
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

    pGD_CurrTimer=&_GD.Timers[sTimerPrev];
    pGD_NextTimer=&_GD.Timers[sTimerNext];
    eTimer *Timer = &_GD.Timers[sTimerPrev];
    prevTimer = controlTypeStartCorrection(Timer->TypeStart, Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
    Timer = &_GD.Timers[sTimerNext];
    nextTimer = controlTypeStartCorrection(Timer->TypeStart, Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
    creg.X= _GD.Hot.Time - prevTimer;
    creg.Y= nextTimer - prevTimer;
//        if (!GD.Timer[nTimer].TimeStart)
//            continue;
//        if (GD.Timer[nTimer].Zone[0]!=fnTeplTimer+1)
//            continue;
//
//        if (GD.Timer[nTimer].TimeStart<MinTimeStart)
//        {
//            MinTimeStart = GD.Timer[nTimer].TimeStart;
//            sTimerMin = nTimer;
//        }
//        if (GD.Timer[nTimer].TimeStart>MaxTimeStart)
//        {
//            MaxTimeStart = GD.Timer[nTimer].TimeStart;
//            sTimerMax = nTimer;
//        }
//        if ((GD.Timer[nTimer].TimeStart>=IntZ)&&(NextTimeStart>GD.Timer[nTimer].TimeStart))
//        {
//            NextTimeStart = GD.Timer[nTimer].TimeStart;
//            sTimerNext = nTimer;
//        }
//        if ((GD.Timer[nTimer].TimeStart<IntZ)&&(PrevTimeStart<GD.Timer[nTimer].TimeStart))
//        {
//            PrevTimeStart = GD.Timer[nTimer].TimeStart;
//            sTimerPrev = nTimer;
//        }
//    }
//    if (MinTimeStart == 1440) return;
//
//    if (sTimerNext<0)
//        sTimerNext = sTimerMin;
//
//    if (sTimerPrev<0)
//        sTimerPrev = sTimerMax;
//
//    pGD_CurrTimer=&GD.Timer[sTimerPrev];
//    pGD_NextTimer=&GD.Timer[sTimerNext];
//
//    IntX = CtrTime-GD.Timer[sTimerPrev].TimeStart;
//    IntY = GD.Timer[sTimerNext].TimeStart-GD.Timer[sTimerPrev].TimeStart;
    if (creg.Y<0)
    {
        creg.Y+=1440;
    }

    if (creg.X<0)
    {
        creg.X+=1440;
    }

    if (fsmTime)
    {
        _GDP.Hot_Tepl->AllTask.NextTAir = JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1);
//Блокировка нулевой темепратуры вентиляции
        tVal = pGD_CurrTimer->TVentAir;
        if (!tVal) tVal = pGD_CurrTimer->TAir+100;
        _GDP.Hot_Tepl->AllTask.NextTVent = JumpNext(tVal,pGD_NextTimer->TVentAir,1,1);
        _GDP.Hot_Tepl->AllTask.Light = pGD_CurrTimer->Light;
        _GDP.Hot_Tepl->AllTask.ModeLight = pGD_CurrTimer->ModeLight;
//		if (pGD_Hot_Tepl->InTeplSens[cSmRHSens])
//		pGD_Hot_Tepl->AllTask.NextRHAir = JumpNext(pGD_CurrTimer->RHAir,pGD_NextTimer->RHAir,1);
        return;
    }
#warning temp !!!!!!!!!!!!!!!!!!!!!!!!!!

    // T отопления, в зависимости что стоит в параметрах упраления, то и ставим в температуру
    _GDP.Hot_Tepl->AllTask.TAir = JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1);


    //if (pGD_Hot_Tepl->AllTask.TAir - TempOld > 50)
    //	NOP;
    //TempOld = pGD_Hot_Tepl->AllTask.TAir;

    //Блокировка нулевой темепратуры вентиляции
    tVal = pGD_CurrTimer->TVentAir;
    if (!tVal) tVal = pGD_CurrTimer->TAir+100;

    // T вентиляции
    _GDP.Hot_Tepl->AllTask.DoTVent = JumpNext(tVal,pGD_NextTimer->TVentAir,1,1);

    _GDP.Hot_Tepl->AllTask.SIO = pGD_CurrTimer->SIO;
    _GDP.Hot_Tepl->AllTask.RHAir = JumpNext(pGD_CurrTimer->RHAir_c,pGD_NextTimer->RHAir_c,1,100);
    _GDP.Hot_Tepl->AllTask.CO2 = JumpNext(pGD_CurrTimer->CO2,pGD_NextTimer->CO2,1,1);
    _GDP.Hot_Tepl->Kontur[cSmKontur1].MinTask = JumpNext(pGD_CurrTimer->MinTPipe1,pGD_NextTimer->MinTPipe1,1,10);

    _GDP.Hot_Tepl->Kontur[cSmKontur2].MinTask = JumpNext(pGD_CurrTimer->MinTPipe2,pGD_NextTimer->MinTPipe2,1,10);

    _GDP.Hot_Tepl->Kontur[cSmKontur3].MinTask = JumpNext(pGD_CurrTimer->MinTPipe3,pGD_NextTimer->MinTPipe3,1,10);
    _GDP.Hot_Tepl->Kontur[cSmKontur5].MinTask = JumpNext(pGD_CurrTimer->MinTPipe5,pGD_NextTimer->MinTPipe5,1,10);

    _GDP.Hot_Tepl->Kontur[cSmKontur1].Optimal = JumpNext(pGD_CurrTimer->TOptimal1,pGD_NextTimer->TOptimal1,1,10);

    _GDP.Hot_Tepl->Kontur[cSmKontur2].Optimal = JumpNext(pGD_CurrTimer->TOptimal2,pGD_NextTimer->TOptimal2,1,10);

    _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinTask = JumpNext(((uchar)pGD_CurrTimer->MinOpenWin),((uchar)pGD_NextTimer->MinOpenWin),0,1);
    _GDP.Hot_Tepl->AllTask.Win = pGD_CurrTimer->Win;
    _GDP.Hot_Tepl->AllTask.Screen[0]=pGD_CurrTimer->Screen[0];
    _GDP.Hot_Tepl->AllTask.Screen[1]=pGD_CurrTimer->Screen[1];
    _GDP.Hot_Tepl->AllTask.Screen[2]=pGD_CurrTimer->Screen[2];
    _GDP.Hot_Tepl->AllTask.Vent = pGD_CurrTimer->Vent;
//	pGD_Hot_Tepl->AllTask.Poise = pGD_CurrTimer->Poise;
    _GDP.Hot_Tepl->Kontur[cSmKontur3].Do = JumpNext(pGD_CurrTimer->TPipe3,pGD_NextTimer->TPipe3,1,10);
    _GDP.Hot_Tepl->Kontur[cSmKontur4].Do = JumpNext(pGD_CurrTimer->TPipe4,pGD_NextTimer->TPipe4,1,10);

}


void AllTaskAndCorrection(const gh_ctx_t *me)
{
    int sum;
    int val = 0;

    creg.Y = _GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    /*Установка и коррекция по солнцу температуры обогрева*/
    me->gh->AllTask.DoTHeat = me->gh->AllTask.TAir;
    creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                          _GD.TuneClimate.s_TConst,cbCorrTOnSun);
    SetBit(me->gh->RCS,creg.X);
    me->gh->AllTask.DoTHeat+=creg.Z;
    /*Коррекция прогноза*/
    me->gh->AllTask.NextTAir+=creg.Z;
    if (me->gh->AllTask.DoTVent)
    {
        CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                       _GD.TuneClimate.s_TVentConst,0);
        me->gh->AllTask.DoTVent+=creg.Z;
        me->gh->AllTask.NextTVent+=creg.Z;
    }
    else
    {
        me->gh->AllTask.DoTVent = me->gh->AllTask.DoTHeat+100;
        me->gh->AllTask.NextTVent = me->gh->AllTask.NextTAir+100;
    }
//	if ((*(pGD_Hot_Hand+cHSmScrTH)).Position)
//		pGD_Hot_Tepl->AllTask.NextRezTAir = pGD_Hot_Tepl->AllTask.NextTAir-GD.TuneClimate.sc_DoTemp;
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу заданной влажности*/
    if (me->gh->AllTask.RHAir)
    {
        me->gh->AllTask.DoRHAir = me->gh->AllTask.RHAir;
        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_RHConst,cbCorrRHOnSun);
        SetBit(me->gh->RCS,creg.X);
        me->gh->AllTask.DoRHAir-=creg.Z;
        /*Коррекция прогноза*/
//		pGD_Hot_Tepl->AllTask.NextRHAir-=IntZ;
    }
    /*---------------------------------------------------*/

    /*Установка и коррекция по солнцу заданной концентрации СО2*/
    if (me->gh->AllTask.CO2)
    {
//ОПТИМИЗАЦИЯ
// Коррекция СО2 держать по фрамугам
        me->gh->AllTask.DoCO2 = me->gh->AllTask.CO2;    // так было, строчка лишняя но все же решил оставить

        if (_GDP.Control_Tepl->co_model!=3)
        {
            sum = me->hand[cHSmWinN].Position + me->hand[cHSmWinS].Position;
            if ((sum >= _GD.TuneClimate.co2Fram1) && (sum <= _GD.TuneClimate.co2Fram2))
            {
                if (me->gh->AllTask.CO2 > _GD.TuneClimate.co2Off)
                {
                    if (_GD.TuneClimate.co2Fram2 > _GD.TuneClimate.co2Fram1)
                        val = _GD.TuneClimate.co2Fram2 - _GD.TuneClimate.co2Fram1;
                    val = ((sum - _GD.TuneClimate.co2Fram1) * _GD.TuneClimate.co2Off) / val;
                    me->gh->AllTask.DoCO2 = me->gh->AllTask.DoCO2 - val;
                }
            }
            if (sum > _GD.TuneClimate.co2Fram2)
                me->gh->AllTask.DoCO2 = me->gh->AllTask.DoCO2 - _GD.TuneClimate.co2Off;
        }

        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_CO2Const,cbCorrCO2OnSun);
        SetBit(me->gh->RCS,creg.X);
        me->gh->AllTask.DoCO2+=creg.Z;
    }
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу минимальной температуры в контурах 1 и 2*/

    if (me->gh->Kontur[cSmKontur1].MinTask)
    {
        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_MinTPipeConst,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        me->gh->Kontur[cSmKontur1].MinCalc = me->gh->Kontur[cSmKontur1].MinTask-creg.Z;
        creg.Y = DefRH();//MeteoSens[cSmFARSens].Value;

        creg.X = CorrectionRule(_GD.TuneClimate.c_RHStart,_GD.TuneClimate.c_RHEnd,
                              _GD.TuneClimate.c_RHOnMin1,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        me->gh->Kontur[cSmKontur1].MinCalc+=creg.Z;

    }
    me->gh->Kontur[cSmKontur2].MinCalc = me->gh->Kontur[cSmKontur2].MinTask;
    if (me->gh->Kontur[cSmKontur2].MinTask)
    {
        creg.Y = DefRH();//MeteoSens[cSmFARSens].Value;

        creg.X = CorrectionRule(_GD.TuneClimate.c_RHStart,_GD.TuneClimate.c_RHEnd,
                              _GD.TuneClimate.c_RHOnMin2,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        me->gh->Kontur[cSmKontur2].MinCalc+=creg.Z;
    }

    creg.Y = _GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    if (me->gh->Kontur[cSmKontur3].MinTask)
    {
        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_MinTPipe3,0);
        me->gh->Kontur[cSmKontur3].MinCalc = me->gh->Kontur[cSmKontur3].MinTask-creg.Z;
    }

//	pGD_Hot_Tepl->Kontur[cSmKontur3].MinCalc = pGD_Hot_Tepl->Kontur[cSmKontur3].MinTask;
    if (me->gh->Kontur[cSmKontur5].MinTask)
    {
        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_MinTPipe5,0/*cbCorrMinTaskOnSun*/);
        me->gh->Kontur[cSmKontur5].MinCalc = me->gh->Kontur[cSmKontur5].MinTask+creg.Z;
    }

    me->gh->AllTask.DoPressure = _GDP.Control_Tepl->c_DoPres;
    /*-------------------------------------------------------------*/

    /*Установка и коррекция по солнцу минимального положения подветренных фрамуг*/
    if (me->gh->Kontur[cSmWindowUnW].MinTask)
    {
        me->gh->Kontur[cSmWindowUnW].MinCalc = me->gh->Kontur[cSmWindowUnW].MinTask;
        creg.X = CorrectionRule(_GD.TuneClimate.s_TStart[0],_GD.TuneClimate.s_TEnd,
                              _GD.TuneClimate.s_MinOpenWinConst,0/*cbCorrMinTaskOnSun*/);
        SetBit(me->gh->Kontur[cSmWindowUnW].RCS,creg.X);
        me->gh->Kontur[cSmWindowUnW].MinCalc+=creg.Z;
    }
    /*----------------------------------------------------------------*/
    creg.Y = DefRH();
    CorrectionRule(_GD.TuneClimate.f_min_RHStart,_GD.TuneClimate.f_min_RHEnd,
                   _GD.TuneClimate.f_CorrTVent,0);
    me->gh->AllTask.NextTVent-=creg.Z;
    me->gh->AllTask.DoTVent-=creg.Z;
    creg.Y=-creg.Y;

    CorrectionRule(_GD.TuneClimate.f_max_RHStart,_GD.TuneClimate.f_max_RHEnd,
                   _GD.TuneClimate.f_CorrTVentUp,0);
    me->gh->AllTask.NextTVent+=creg.Z;
    me->gh->AllTask.DoTVent+=creg.Z;
    /*--------------------------------------------------------------*/

}
void SetIfReset(void)
{

    for (int i = 0; i < cSWaterKontur; i++)
    {
        SetPointersOnKontur(i);
        _GDCP.TControl_Tepl_Kontur->DoT = _GDCP.TControl_Tepl_Kontur->SensValue*10;//((long int)pGD_Hot_Tepl->InTeplSens[ByteX+cSmWaterSens].Value)*100;
        _GDCP.TControl_Tepl_Kontur->PumpPause = cPausePump;
        _GDCP.TControl_Tepl_Kontur->PumpStatus = 1;
    }
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinS)).Position))*10;
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+1-GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinN)).Position))*10;

}

/**********************************************************************/
/*-*-*-*-*--Нахождение прогнозируемого изменения температуры--*-*-*-*-*/
/**********************************************************************/
//#warning Прогнозы температуры по внешним факторам !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void __cNextTCalc(const gh_ctx_t *me)
{
    int CalcAllKontur;

    if (!me->gh->AllTask.NextTAir) return;


    if (bWaterReset[me->idx])
    {
        SetIfReset();
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
    int cSmTSens = 0;
    for (cSmTSens = 0; cSmTSens<4; cSmTSens++)  // 4 датчика температуры
    {
        _GDP.Level_Tepl[cSmTSens][cSmUpAlarmLev]=0;
        _GDP.Level_Tepl[cSmTSens][cSmDownAlarmLev]=0;
        if (_GD.TuneClimate.c_MaxDifTUp)
            _GDP.Level_Tepl[cSmTSens][cSmUpAlarmLev]=me->gh->AllTask.DoTHeat+_GD.TuneClimate.c_MaxDifTUp;
        if (_GD.TuneClimate.c_MaxDifTDown)
            _GDP.Level_Tepl[cSmTSens][cSmDownAlarmLev]=me->gh->AllTask.DoTHeat-_GD.TuneClimate.c_MaxDifTDown;
    }

    me->gh->NextTCalc.DifTAirTDo = me->gh->AllTask.NextTAir-getTempHeat(me->idx);
    /**********************************************/
    /*СУПЕР АЛГОРИТМ ДЛЯ РАСЧЕТА*/
    me->gh->AllTask.Rez[0]=getTempHeat(me->idx);
    creg.X=(me->gh->AllTask.DoTHeat-getTempHeat(me->idx));

/**********************************************/
/*Вычиляем увеличение от солнечной радиации*/
    creg.Y = _GD.Hot.MidlSR;
    if ((!YesBit(me->gh->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        creg.Y = me->gh->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(_GD.TuneClimate.c_SRStart,_GD.TuneClimate.c_SREnd,
                   _GD.TuneClimate.c_SRFactor,0);
    me->gh->NextTCalc.UpSR = creg.Z;
/*Вычиляем увеличение от разницы температуры задания и стекла*/
    creg.Y = me->gh->AllTask.NextTAir-me->gh->InTeplSens[cSmGlassSens].Value;

    CorrectionRule(_GD.TuneClimate.c_GlassStart,_GD.TuneClimate.c_GlassEnd,
                   _GD.TuneClimate.c_GlassFactor,0);
    me->gh->NextTCalc.LowGlass = creg.Z;

    if (me->gh_tctrl->Screen[0].Mode < 2)
        //if (pGD_TControl_Tepl->Screen[0].Mode < 2)
        me->gh->NextTCalc.CorrectionScreen = _GD.TuneClimate.CorrectionScreen * me->gh_tctrl->Screen[0].Mode;

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
    creg.Y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.c_WindStart,_GD.TuneClimate.c_WindEnd,
                   _GD.TuneClimate.c_WindFactor,0);
    creg.Y = me->gh->AllTask.NextTAir-_GD.TControl.MeteoSensing[cSmOutTSens]-creg.Z;
    CorrectionRule(_GD.TuneClimate.c_OutStart,_GD.TuneClimate.c_OutEnd,
                   _GD.TuneClimate.c_OutFactor,0);
    me->gh->NextTCalc.LowOutWinWind+=creg.Z;

//	if (YesBit(pGD_Hot_Tepl->DiskrSens[0],cSmLightDiskr))
    me->gh->NextTCalc.UpLight=(((long)_GD.TuneClimate.c_LightFactor)*((*(_GDP.Hot_Hand+cHSmLight)).Position))/100;
//******************** NOT NEEDED
//	IntY = 1;
//	IntY<<=fnTepl;
//	if (YesBit(GD.Hot.isLight,IntY))
//		pGD_Hot_Tepl->NextTCalc.UpLight = GD.TuneClimate.c_LightFactor;
//********************************************************

/*Считаем сумму поправок*/
    me->gh->NextTCalc.dSumCalc=
    +me->gh->NextTCalc.UpSR
    -me->gh->NextTCalc.LowGlass
    -me->gh->NextTCalc.LowOutWinWind
    +me->gh->NextTCalc.UpLight
    -me->gh->NextTCalc.CorrectionScreen;

//		-pGD_Hot_Tepl->NextTCalc.LowRain;
    if (_GD.TControl.bSnow)
        me->gh->NextTCalc.dSumCalc-=_GD.TuneClimate.c_CloudFactor;
/*********************************************************************
******* СЧИТАЕМ СУММУ ВЛИЯНИЙ ДЛЯ ФРАМУГ *******************************
***********************************************************************/
    me->gh->NextTCalc.dSumCalcF = 0;
/*Вычиляем увеличение от солнечной радиации*/
    creg.Y = _GD.Hot.MidlSR;
/*if work on internal light sensor, then change IntY*/

    if ((!YesBit(me->gh->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        creg.Y = me->gh->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(_GD.TuneClimate.c_SRStart,_GD.TuneClimate.c_SREnd,
                   _GD.TuneClimate.f_SRFactor,0);
    me->gh->NextTCalc.dSumCalcF+=creg.Z;

/*Вычисляем корректировки ветра фрамуг и разницы между температурой задания
и внешней температуры соответственно*/
/*Ветер и фрамуги увеличивают эту разницу*/
    creg.Y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.c_WindStart,_GD.TuneClimate.c_WindEnd,
                   _GD.TuneClimate.f_WindFactor,0);
    creg.Y = me->gh->AllTask.NextTAir-_GD.TControl.MeteoSensing[cSmOutTSens]-creg.Z;
    CorrectionRule(_GD.TuneClimate.c_OutStart,_GD.TuneClimate.c_OutEnd,
                   _GD.TuneClimate.f_OutFactor,0);
    me->gh->NextTCalc.dSumCalcF+=creg.Z;
/*********************************************************************
***********************************************************************
***********************************************************************/



    creg.Y = me->gh->NextTCalc.DifTAirTDo;
    if ((_GD.TuneClimate.c_MullDown>10)&&(creg.Y<0)&&(_GD.TuneClimate.c_MullDown<30))
        creg.Y=(((long)creg.Y)*_GD.TuneClimate.c_MullDown)/10;
    me->gh->NextTCalc.PCorrection=((int)((((long)(creg.Y))*((long)_GDP.Control_Tepl->c_PFactor))/100));
    if (me->gh_tctrl->StopI<2)
        me->gh_tctrl->Integral+=((((long)(me->gh->NextTCalc.DifTAirTDo))*((long)_GDP.Control_Tepl->c_IFactor))/10);
    if (me->gh_tctrl->Integral>2000000)
        me->gh_tctrl->Integral = 2000000;
    if (me->gh_tctrl->Integral<-2000000)
        me->gh_tctrl->Integral=-2000000;
    if (!_GDP.Control_Tepl->c_IFactor)
        me->gh_tctrl->Integral = 0;
    me->gh->NextTCalc.ICorrection=(int)(me->gh_tctrl->Integral/100);
    me->gh_tctrl->Critery = me->gh->NextTCalc.PCorrection+me->gh->NextTCalc.ICorrection-me->gh->NextTCalc.dSumCalc;
    CalcAllKontur = __sCalcTempKonturs();
    me->gh_tctrl->Critery-=CalcAllKontur;
//	pGD_Hot_Tepl->NextTCalc.dNextTCalc = CalcAllKontur;
    if (me->gh_tctrl->StopI>4)
    {
        me->gh_tctrl->Integral = me->gh_tctrl->SaveIntegral;
    }
//	IntY = pGD_Hot_Tepl->NextTCalc.DifTAirTDo;
    me->gh_tctrl->SaveIntegral = me->gh_tctrl->Integral;
    if ((me->gh_tctrl->StopI>3)&&(ABS(creg.Y)<cResetDifTDo))
    {

//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (me->gh_tctrl->Critery>cResetCritery)
        {
            me->gh_tctrl->SaveIntegral
            =cResetCritery+CalcAllKontur
             -me->gh->NextTCalc.PCorrection+me->gh->NextTCalc.dSumCalc;
            me->gh_tctrl->SaveIntegral*=100;
        }
//		IntY=-IntY;
//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (me->gh_tctrl->Critery<-cResetCritery)
        {
            me->gh_tctrl->SaveIntegral
            =-cResetCritery+CalcAllKontur
             -me->gh->NextTCalc.PCorrection+me->gh->NextTCalc.dSumCalc;
            me->gh_tctrl->SaveIntegral*=100;
        }
    }
    if ((me->gh_tctrl->StopI>3)&&(!SameSign(creg.Y,me->gh_tctrl->Critery)))
    {
        me->gh_tctrl->SaveIntegral
        =creg.Y+CalcAllKontur
         -me->gh->NextTCalc.PCorrection+me->gh->NextTCalc.dSumCalc;
        me->gh_tctrl->SaveIntegral*=100;
    }
    if (!me->gh_tctrl->Critery)
    {
        me->gh_tctrl->Critery = 1;
        if (me->gh->NextTCalc.DifTAirTDo<0)
            me->gh_tctrl->Critery=-1;

    }
    me->gh->NextTCalc.Critery = me->gh_tctrl->Critery;

/******************************************************************
        Далее расчет критерия для фрамуг
*******************************************************************/
    if (getTempVent(me->idx))
        creg.Y = getTempVent(me->idx)-me->gh->AllTask.DoTVent;
    else creg.Y = 0;

    me->gh->NextTCalc.PCorrectionVent=((int)((((long)(creg.Y))*((long)_GDP.Control_Tepl->f_PFactor))/100));
    if (me->gh_tctrl->StopVentI<2)
        me->gh_tctrl->IntegralVent+=((((long)(creg.Y))*((long)_GDP.Control_Tepl->f_IFactor))/10);
    if (me->gh_tctrl->IntegralVent<0) me->gh_tctrl->IntegralVent = 0;
    me->gh->NextTCalc.ICorrectionVent=(int)(me->gh_tctrl->IntegralVent/100);
    if (!_GDP.Control_Tepl->f_IFactor)
        me->gh_tctrl->IntegralVent = 0;

    creg.X = me->gh->NextTCalc.PCorrectionVent+me->gh->NextTCalc.ICorrectionVent+me->gh->NextTCalc.dSumCalcF;
//Блокировка фрамуг при отоплении
    if ((me->gh_tctrl->TVentCritery<creg.X)&&(!me->gh_tctrl->StopI)&&(creg.X>0)&&((_GDP.Control_Tepl->f_PFactor%100)>89))
    {
        me->gh_tctrl->IntegralVent = me->gh_tctrl->TVentCritery-me->gh->NextTCalc.PCorrectionVent-me->gh->NextTCalc.dSumCalcF;
        me->gh_tctrl->IntegralVent*=100;
        creg.X = me->gh_tctrl->TVentCritery;
    }
    me->gh_tctrl->TVentCritery = creg.X;
//	if (!SameSign(pGD_TControl_Tepl->TVentCritery,pGD_TControl_Tepl->LastTVentCritery))
//	  	pGD_TControl_Tepl->StopVentI = 0;
    if (me->gh_tctrl->StopVentI>4)
    {
        me->gh_tctrl->IntegralVent = me->gh_tctrl->SaveIntegralVent;
    }
    if (me->gh_tctrl->StopVentI>3)
    {
        me->gh_tctrl->SaveIntegralVent = me->gh_tctrl->IntegralVent;
        CorrectionRule(0,100,500,0);
        if (me->gh_tctrl->AbsMaxVent>0)
            creg.Z+=me->gh_tctrl->AbsMaxVent;
        if (me->gh_tctrl->TVentCritery>creg.Z)
        {
            me->gh_tctrl->SaveIntegralVent
            =creg.Z-me->gh->NextTCalc.PCorrectionVent-me->gh->NextTCalc.dSumCalcF;
            me->gh_tctrl->SaveIntegralVent*=100;
        }
        creg.Y=-creg.Y;
        CorrectionRule(0,100,500,0);
        creg.Z++;
        if (me->gh_tctrl->TVentCritery<-creg.Z)
        {
            me->gh_tctrl->SaveIntegralVent
            =-creg.Z-me->gh->NextTCalc.PCorrectionVent-me->gh->NextTCalc.dSumCalcF;
            me->gh_tctrl->SaveIntegralVent*=100;
        }
    }
    me->gh->NextTCalc.TVentCritery = me->gh_tctrl->TVentCritery;

}

/*------------------------------------------------------*/
void    SetMixValvePosition(void)
{
    int16_t *IntVal;
    for (int i = 0;i<cSWaterKontur;i++)
    {
        SetPointersOnKontur(i);
        if (YesBit((*(_GDCP.Hot_Hand_Kontur+cHSmMixVal)).RCS,(/*cbNoMech+*/cbManMech))) continue;
        IntVal=&(_GDP.TControl_Tepl->IntVal[i]);
        if (!_GDCP.TControl_Tepl_Kontur->PumpStatus)
        {
            (*(_GDCP.Hot_Hand_Kontur+cHSmMixVal)).Position = 0;
            continue;
        }
        _GDCP.TControl_Tepl_Kontur->TPause = MAX(_GDCP.TControl_Tepl_Kontur->TPause, 0);// pGD_TControl_Tepl->Kontur[ByteX].TPause = 0;
        if (YesBit(_GDP.TControl_Tepl->MechBusy[i].RCS,cMSBusyMech)) continue;
        if (_GDCP.TControl_Tepl_Kontur->TPause)
        {
            _GDCP.TControl_Tepl_Kontur->TPause--;
            continue;
        }
        _GDCP.TControl_Tepl_Kontur->TPause = cMinPauseMixValve;

        creg.X = _GDCP.Hot_Tepl_Kontur->Do-_GDCP.TControl_Tepl_Kontur->SensValue;
        //(*IntVal)=(*IntVal)+IntX;
        long long_y = _GDP.ConstMechanic->ConstMixVal[i].v_PFactor;
        long_y = long_y*creg.X;//(*IntVal);
        creg.Y=(int16_t)(long_y/10000);
        //if (!IntY) continue;
        creg.Z=(*IntVal)/100;
        //IntZ=(*(pGD_Hot_Hand_Kontur+cHSmMixVal)).Position;
        creg.Z+=creg.Y;
        if (creg.Z>100)
        {
            (*IntVal)=(100-creg.Y)*100;
            creg.Z = 100;
        }
        else
            if (creg.Z<0)
        {
            (*IntVal)=(-creg.Y)*100;
            creg.Z = 0;
        }
        else
            (*IntVal)+=(int16_t)((((long)creg.X)*_GDP.ConstMechanic->ConstMixVal[i].v_IFactor)/100);

        //ogrMax(&IntZ,100);//if (IntZ>100) IntZ = 100;
        //ogrMin(&IntZ,0);//if (IntZ<0)	IntZ = 0;
        (*(_GDCP.Hot_Hand_Kontur+cHSmMixVal)).Position=(char)(creg.Z);
    }
}

void    DoPumps(void)
{
    for (int i = 0; i < cSWaterKontur; i++)
    {
        if (!(YesBit((*(_GDP.Hot_Hand+cHSmPump+i)).RCS,(/*cbNoMech+*/cbManMech))))
            (*(_GDP.Hot_Hand+cHSmPump+i)).Position = _GDP.TControl_Tepl->Kontur[i].PumpStatus;
    }

}

//#warning вкл воздушного обогревателя !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void    DoVentCalorifer(void)
{

//	if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,cbManMech)) return;
//		(*(pGD_Hot_Hand+cHSmHeat)).Position = pGD_TControl_Tepl->Calorifer;
//
//
    if (!(YesBit((*(_GDP.Hot_Hand+cHSmVent)).RCS,(/*cbNoMech+*/cbManMech))))   // было так
    {
        (*(_GDP.Hot_Hand+cHSmVent)).Position = _GDP.TControl_Tepl->Vent;
        (*(_GDP.Hot_Hand+cHSmVent)).Position+=_GDP.TControl_Tepl->OutFan<<1;
    }
//	if (!(YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,(/*cbNoMech+*/cbManMech))))
//	{
//
//		(*(pGD_Hot_Hand+cHSmHeat)).Position = pGD_TControl_Tepl->Calorifer;
//	}
}


//#warning вкл подсветки !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void    DoLights(void)
{
    if (YesBit((*(_GDP.Hot_Hand+cHSmLight)).RCS,(/*cbNoMech+*/cbManMech))) return;
//	pGD_Hot_Hand[cHSmLight].Position = 0;
    _GDP.Hot_Hand[cHSmLight].Position = _GDP.TControl_Tepl->LightValue;
}

/*void	DoPoisen(void)
{
    if (YesBit((*(pGD_Hot_Hand+cHSmPoise)).RCS,(cbManMech))) return;
    pGD_Hot_Hand[cHSmPoise].Position = 0;
        if (pGD_Hot_Tepl->AllTask.Poise)
            pGD_Hot_Hand[cHSmPoise].Position = 1;
} */

void    SetSensOnMech(void)
{
    for (int i = 0;i<cSRegCtrl;i++)
        _GDP.TControl_Tepl->MechBusy[i].Sens = 0;
    _GDP.TControl_Tepl->MechBusy[cHSmWinN].Sens=&_GDP.Hot_Tepl->InTeplSens[cSmWinNSens];
    _GDP.TControl_Tepl->MechBusy[cHSmWinS].Sens=&_GDP.Hot_Tepl->InTeplSens[cSmWinSSens];
    _GDP.TControl_Tepl->MechBusy[cHSmScrTH].Sens=&_GDP.Hot_Tepl->InTeplSens[cSmScreenSens];
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
    char fnAlr;
    char fnTepl;
    for (fnTepl = 0;fnTepl<cSTepl;fnTepl++)
    {
        for (fnAlr = 0;fnAlr<cSRegCtrl;fnAlr++)
        {
            ClrBit(_GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].RCS,cMSAlarm);
            _GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].TryMove = 0;
            _GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].TryMes = 0;
        }
    }
    #warning "statement with no effect"
    for (fnAlr = 0;fnAlr<MAX_ALARMS;fnAlr++)
        _GD.TControl.Tepl[fnTepl].Alarms[fnAlr];
}

void SetAlarm(void)
{
    char fnTepl;
    for (fnTepl = 0;fnTepl<_GD.Control.ConfSTepl;fnTepl++)
        write_output_bit(fnTepl,cHSmAlarm,1,0);
    for (fnTepl = 0;fnTepl<_GD.Control.ConfSTepl;fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        _GDP.TControl_Tepl->bAlarm = 0;
        if ((YesBit(_GDP.Hot_Tepl->RCS,(cbNoTaskForTepl+cbNoSensingTemp+cbNoSensingOutT)))
            //	||(YesBit(pGD_Hot_Tepl->InTeplSens[cSmTSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            ||(YesBit(_GDP.Hot_Tepl->InTeplSens[cSmWaterSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens))))
        {
            write_output_bit(fnTepl,cHSmAlarm,0,0);
            _GDP.TControl_Tepl->bAlarm = 100;
        }

        if (getTempHeatAlarm(fnTepl)  ==  0)
        {
            write_output_bit(fnTepl,cHSmAlarm,0,0);
            _GDP.TControl_Tepl->bAlarm = 100;
        }

        if (getTempVentAlarm(fnTepl)  ==  0)
        {
            write_output_bit(fnTepl,cHSmAlarm,0,0);
            _GDP.TControl_Tepl->bAlarm = 100;
        }

        for (int i = 0;i<cConfSSens;i++)
        {
            if (YesBit(_GDP.Hot_Tepl->InTeplSens[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            {
                write_output_bit(fnTepl,cHSmAlarm,0,0);
                _GDP.TControl_Tepl->bAlarm = 100;
            }
        }
    }
    for (int i = 0;i<cConfSMetSens;i++)
        if (YesBit(_GD.Hot.MeteoSensing[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
        {
            write_output_bit(cSmZone1,cHSmAlarm,0,0);
            _GD.TControl.Tepl[cSmZone1].bAlarm = 100;
        }

}

void SetDiskr(char fnTepl)
{
    int nLight;
    char tMaxLight;

    if (!(YesBit((*(_GDP.Hot_Hand+cHSmAHUSpeed1)).RCS,cbManMech)))
        (*(_GDP.Hot_Hand+cHSmAHUSpeed1)).Position = _GDP.Hot_Tepl->Kontur[cSmKontur4].Do/10;
    if (!(YesBit((*(_GDP.Hot_Hand+cHSmAHUSpeed2)).RCS,cbManMech)))
        (*(_GDP.Hot_Hand+cHSmAHUSpeed2)).Position = _GDP.Hot_Tepl->Kontur[cSmKontur4].Do/10;

    for (int i = cHSmPump;i<cHSmRegs;i++)
    {
        //if ((ByteX == cHSmSIOVals)||(ByteX == cHSmLight)) continue;
        if ((i == cHSmSIOPump)||(i == cHSmSIOVals)||(i == cHSmLight)) continue;

        write_output_bit(fnTepl,i,1,0);

        if (YesBit((*(_GDP.Hot_Hand+i)).Position,0x01))
            write_output_bit(fnTepl,i,0,0);
        if (((i == cHSmHeat)||(i == cHSmVent))&&(YesBit((*(_GDP.Hot_Hand+i)).Position,0x02)))
            write_output_bit(fnTepl,i,0,1);
    }
    nLight = 0;
    if (((uchar)((*(_GDP.Hot_Hand+cHSmLight)).Position))>100) (*(_GDP.Hot_Hand+cHSmLight)).Position = 100;
    if ((_GDP.Hot_Tepl->AllTask.DoTHeat)||(YesBit((*(_GDP.Hot_Hand+cHSmLight)).RCS,cbManMech)))
    {
        nLight=((*(_GDP.Hot_Hand+cHSmLight)).Position-50)/10+2;
        if (nLight<1) nLight = 1;
    }
    bool is_light_on = 0;
    if (nLight>1)
    {
        write_output_bit(fnTepl,cHSmLight,0,0);
        is_light_on = 1;
    }
    tMaxLight = 8;

    switch (_GDP.Control_Tepl->sLight)
    {
    case 2:
        if (nLight == 7) nLight = 0x04;
        else
            if (nLight>1) nLight = 0x02;
        else
            if (nLight == 1) nLight = 0x01;
        break;
    case 1:
        if (nLight == 7) nLight = 1;
        else nLight = 0;
        break;
    case 8:
        if (nLight!=1) nLight = 0;
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
        tMaxLight = _GDP.Control_Tepl->sLight-10;
        if (ctx.fLightPause>CONTROL_LIGHT_DELAY*8) ctx.fLightPause = CONTROL_LIGHT_DELAY*8;
        if (ctx.fLightPause<0) ctx.fLightPause = 0;
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
            write_output_bit(fnTepl,cHSmLight,0,i+1);

    }

/*	if (YesBit((*(pGD_Hot_Hand+cHSmVent)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmVent,0,0);
    if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmHeat,0,0);*/
    int i = 1;
    if (_GDP.Control_Tepl->co_model>=2) i = 2;

    if ((_GDP.TControl_Tepl->SetupRegs[0].On)
        &&(_GDP.Control_Tepl->co_model))
        write_output_bit(fnTepl,cHSmCO2,0,i);

    // насос
    //__SetBitOutReg(fnTepl,cHSmSIOPump,1,0);
    if (YesBit((*(_GDP.Hot_Hand+cHSmSIOPump)).Position,0x01))
        write_output_bit(fnTepl,cHSmSIOPump,0,0);

    for (int i = 0;i<4;i++)
    {
        creg.X = 1;
        creg.X<<=i;
        if (YesBit((*(_GDP.Hot_Hand+cHSmSIOVals)).Position,creg.X))
            write_output_bit(fnTepl,cHSmSIOVals,0,i);
    }

#ifdef AGAPOVSKIY_DOUBLE_VALVE
    if (YesBit((*(_GDP.Hot_Hand+cHSmSIOVals)).Position,0x02))
        write_output_bit(fnTepl,cHSmAHUVals,0,0);
#endif
    for (int i = 0;i<5;i++)
    {
        if (_GD.Hot.Regs[i])
            write_output_bit(fnTepl,i+cHSmRegs,0,0);
    }
}

void DoMechanics(char fnTepl)
{
    char fErr;
    for (int i = cHSmMixVal;i<cHSmPump;i++)
    {
        SetPointersOnKontur(i);

        const eConstMixVal *ConstMechanic_Mech = &_GDP.ConstMechanic->ConstMixVal[i];

//		pGD_Hot_Hand_Kontur = pGD_Hot_Hand+ByteX;
        eMechBusy *mechbusy=&(_GDP.TControl_Tepl->MechBusy[i]);

        if (_GDCP.Hot_Hand_Kontur->Position>100)
            _GDCP.Hot_Hand_Kontur->Position = 100;
        if (_GDCP.Hot_Hand_Kontur->Position<0)
            _GDCP.Hot_Hand_Kontur->Position = 0;

        if ((i == cHSmAHUSpeed1))
        {
//			Sound;
            write_output_register(_GDCP.Hot_Hand_Kontur->Position,mtRS485,_GD.MechConfig[fnTepl].RNum[i],&fErr,&_GD.FanBlock[fnTepl][0].FanData[0]);
            continue;
        }
/*		GD.FanBlock[fnTepl][0].FanData[0].ActualSpeed = fnTepl*5;
        GD.FanBlock[fnTepl][0].FanData[1].ActualSpeed = fnTepl*5+1;
        GD.FanBlock[fnTepl][1].FanData[1].ActualSpeed = fnTepl*5+2;
        GD.FanBlock[fnTepl][1].FanData[2].ActualSpeed = fnTepl*5+3;
*/
        if ((i == cHSmAHUSpeed2))
        {
//			Sound;
            write_output_register(_GDCP.Hot_Hand_Kontur->Position,mtRS485,_GD.MechConfig[fnTepl].RNum[i],&fErr,&_GD.FanBlock[fnTepl][1].FanData[0]);
            continue;
        }


        if ((i == cHSmCO2)&&(_GDP.Control_Tepl->co_model == 1)) continue;

        write_output_bit(fnTepl,i,1,0);
        write_output_bit(fnTepl,i,1,1);

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
                    continue;
                }
                if (mechbusy->PrevTask == _GDCP.Hot_Hand_Kontur->Position*10)
                {
                    creg.Y = _GD.TuneClimate.f_MaxAngle*10;
                    creg.Y = CLAMP(10, creg.Y, 50);
                    if (abs(mechbusy->Sens->Value - mechbusy->PrevTask)>creg.Y)
                    {
                        mechbusy->TryMes++;
                        if (mechbusy->TryMes>4)
                        {
                            mechbusy->TryMes = 0;
                            mechbusy->TryMove+=(abs(mechbusy->Sens->Value-mechbusy->PrevTask)/creg.Y);
                        }
                        else continue;
                    }


                    long long_x = mechbusy->Sens->Value;
                    long_x *= ConstMechanic_Mech->v_TimeMixVal;//MBusy->CalcTime;
                    long_x /= 1000;

                    if (abs(mechbusy->Sens->Value-mechbusy->PrevTask)<=creg.Y)
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
                mechbusy->PrevTask = _GDCP.Hot_Hand_Kontur->Position*10;
            }
            //else return;
        }
//Выход из паузы при блокировке на крайних положениях
        if (YesBit(mechbusy->RCS,cMSBlockRegs)
            &&((_GDCP.Hot_Hand_Kontur->Position>0)||(mechbusy->TimeSetMech>0))
            &&((_GDCP.Hot_Hand_Kontur->Position<100)||(mechbusy->TimeSetMech < ConstMechanic_Mech->v_TimeMixVal)))
        {
            ClrBit(mechbusy->RCS,cMSBlockRegs);
            mechbusy->TimeRealMech = mechbusy->TimeSetMech;
            byte_y++;
        }
//Расчет

        if ((!mechbusy->PauseMech)||(YesBit(_GDCP.Hot_Hand_Kontur->RCS,cbManMech)))
        {
            long long_x =_GDCP.Hot_Hand_Kontur->Position;
            long_x *= ConstMechanic_Mech->v_TimeMixVal;
            long_x /= 100;
            mechbusy->TimeSetMech=(int)(long_x);
/*			if (YesBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech))
            {
                MBusy->TimeRealMech = MBusy->TimeSetMech;
                ClrBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech);
                ByteY++;
            }*/
            if (!YesBit(_GDCP.Hot_Hand_Kontur->RCS,cbManMech))
            {
                if (!_GDCP.Hot_Hand_Kontur->Position)
                {
                    SetBit(mechbusy->RCS,cMSBlockRegs);
                    mechbusy->TimeRealMech+= ConstMechanic_Mech->v_TimeMixVal/4;
                }
                if (_GDCP.Hot_Hand_Kontur->Position == 100)
                {
                    SetBit(mechbusy->RCS,cMSBlockRegs);
                    mechbusy->TimeRealMech -= ConstMechanic_Mech->v_TimeMixVal/4;
                }
            }
        }
        if (mechbusy->TimeSetMech>mechbusy->TimeRealMech)
        {
            mechbusy->TimeRealMech++;
            write_output_bit(fnTepl,i,0,1);
            SetBit(mechbusy->RCS,cMSBusyMech);
            //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
            byte_y++;
        }
        if (mechbusy->TimeSetMech<mechbusy->TimeRealMech)
        {
            mechbusy->TimeRealMech--;
            write_output_bit(fnTepl,i,0,0);
            SetBit(mechbusy->RCS,cMSBusyMech);
            //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
            byte_y++;
        }
        if (byte_y)
        {
            creg.Y = ConstMechanic_Mech->v_MinTim;
/*			if ((ByteX == cHSmWinN)||(ByteX == cHSmWinS))
            {
                ogrMin(&IntY,90);
                pGD_TControl_Tepl->FramUpdate[ByteX-cHSmWinN]=0;
            }*/
            creg.Y = MAX(creg.Y, 5);
            mechbusy->PauseMech = creg.Y;
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
        else if (_GD.TControl.TimeMeteoSensing[i]<30) _GD.TControl.TimeMeteoSensing[i]++;
    }
    if (_GD.TControl.Tepl[0].SnowTime>=_GD.TuneClimate.MinRainTime)
        _GD.TControl.bSnow=!_GD.TControl.bSnow;
    if (((_GD.TControl.MeteoSensing[cSmRainSens]<cMinRain)&&(_GD.TControl.bSnow))
        ||((_GD.TControl.MeteoSensing[cSmRainSens]>cMinRain)&&(!_GD.TControl.bSnow)))
    {
        _GD.TControl.Tepl[0].SnowTime++;
//		GD.TControl.Tepl[0].SnowTime = 10;
    }
    else
        _GD.TControl.Tepl[0].SnowTime = 0;

    if ((_GD.TControl.MeteoSensing[cSmOutTSens]<c_SnowIfOut)&&(_GD.TControl.bSnow))
        SetBit(_GD.TControl.bSnow,0x02);
    _GD.TControl.Tepl[0].SumSens+=_GD.TControl.MeteoSensing[cSmFARSens];//GD.Hot.MeteoSens[cSmFARSens].Value;
    _GD.TControl.Tepl[0].TimeSumSens++;
    if (_GD.TControl.Tepl[0].TimeSumSens>=15)
    {
        _GD.TControl.Tepl[0].SensHourAgo = _GD.TControl.Tepl[0].SensHalfHourAgo;
        _GD.TControl.Tepl[0].SensHalfHourAgo = _GD.TControl.Tepl[0].SumSens/_GD.TControl.Tepl[0].TimeSumSens;
        _GD.TControl.Tepl[0].SumSens = 0;
        _GD.TControl.Tepl[0].TimeSumSens = 0;
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
void SetLighting(const gh_ctx_t *me)
{
    char bZad;

    if (!(_GDP.MechConfig->RNum[cHSmLight])) return;  // if hand mode exit
    creg.Z = 0;

//	if(SameSign(IntY,IntZ)) pGD_TControl_Tepl->LightExtraPause = 0;

    me->gh_tctrl->LightPauseMode--;

    if (    (me->gh_tctrl->LightPauseMode < 0)
         || (me->gh_tctrl->LightPauseMode > _GD.TuneClimate.l_PauseMode))
        me->gh_tctrl->LightPauseMode = 0;

    bZad = 0;     // if bZab = 0 calc sun sensor

    if (me->gh_tctrl->LightPauseMode)
        bZad = 1;  // if bZad = 1 don't calc sun senasor

// old
//	if ((pGD_Hot_Tepl->AllTask.ModeLight<2))//&&(!bZad))	// если режим досветки не авто
//	{
//		pGD_TControl_Tepl->LightMode = pGD_Hot_Tepl->AllTask.ModeLight*pGD_Hot_Tepl->AllTask.Light;
//		bZad = 1;
//	}

    if (me->gh->AllTask.ModeLight < 2)
    {
        me->gh_tctrl->LightMode = me->gh->AllTask.ModeLight * me->gh->AllTask.Light;
        bZad = 1;
    }

    if (!bZad)
    {
        if (_GD.Hot.Zax-60>_GD.Hot.Time)
            me->gh_tctrl->LightMode = 0;

        // XXX: using greenhouse 0 ?
        if (_GD.TControl.Tepl[0].SensHalfHourAgo > _GD.TuneClimate.l_SunOn50)  // sun > 50% then off light
            me->gh_tctrl->LightMode = 0;

        if (_GD.TControl.Tepl[0].SensHalfHourAgo < _GD.TuneClimate.l_SunOn50)
        {
//			pGD_TControl_Tepl->LightMode = 50;
            creg.Y = _GD.Hot.MidlSR;
            CorrectionRule(_GD.TuneClimate.l_SunOn100,_GD.TuneClimate.l_SunOn50,50,0);
            me->gh_tctrl->LightMode = 100-creg.Z;
        }

//		if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn100)
//			pGD_TControl_Tepl->LightMode = 100;
    }
    if (me->gh_tctrl->LightMode != me->gh_tctrl->OldLightMode)
    {
        if (! (((int)me->gh_tctrl->LightMode)*((int)me->gh_tctrl->OldLightMode)))
        {
            me->gh_tctrl->DifLightMode = me->gh_tctrl->LightMode - me->gh_tctrl->OldLightMode;
            me->gh_tctrl->LightPauseMode = _GD.TuneClimate.l_PauseMode;
//			pGD_TControl_Tepl->LightExtraPause = o_DeltaTime;
        }
        else
        {
            me->gh_tctrl->LightPauseMode = _GD.TuneClimate.l_SoftPauseMode;
        }
    }
    me->gh_tctrl->OldLightMode = me->gh_tctrl->LightMode;

//	pGD_TControl_Tepl->LightExtraPause--;
//	if (pGD_TControl_Tepl->LightExtraPause>0) return;
//	pGD_TControl_Tepl->LightExtraPause = 0;

    // new
    if (me->gh->AllTask.ModeLight  ==  2)           // авто досветка
    {
        if (me->gh->AllTask.Light < me->gh_tctrl->LightMode)
            me->gh_tctrl->LightValue = me->gh->AllTask.Light;
        else
            me->gh_tctrl->LightValue = me->gh_tctrl->LightMode;
    }
    else
        me->gh_tctrl->LightValue = me->gh_tctrl->LightMode;
    // new

    if (me->gh_tctrl->LightValue > 100)
        me->gh_tctrl->LightValue = 100;

    //old
    //pGD_TControl_Tepl->LightValue = pGD_TControl_Tepl->LightMode;		// значение досветки

}

void SetTepl(const gh_ctx_t *me)
{
/***********************************************************************
--------------Вычисление изменения показаний датчика температуры-------
************************************************************************/

/***********************************************************************/
    if (! me->gh->AllTask.NextTAir)
        me->gh->RCS |= cbNoTaskForTepl;

//	if(!pGD_Hot_Tepl->InTeplSens[cSmTSens].Value)
//		SetBit(pGD_Hot_Tepl->RCS,cbNoSensingTemp);
// NEW
    if (! me->gh->InTeplSens[cSmTSens1].Value)
        me->gh->RCS |= cbNoSensingTemp;
    if (! me->gh->InTeplSens[cSmTSens2].Value)
        me->gh->RCS |= cbNoSensingTemp;
    if (! me->gh->InTeplSens[cSmTSens3].Value)
        me->gh->RCS |= cbNoSensingTemp;
    if (! me->gh->InTeplSens[cSmTSens4].Value)
        me->gh->RCS |= cbNoSensingTemp;

//	if(!pGD_Hot_Tepl->RCS)
    {
        AllTaskAndCorrection(me);
        LaunchCalorifer(me);

        __cNextTCalc(me);

        DecPumpPause();

        //SetUpSiod(fnTepl);


        InitScreen(me, cTermHorzScr);
        InitScreen(me, cSunHorzScr);
        InitScreen(me, cTermVertScr1);
        InitScreen(me, cTermVertScr2);
        InitScreen(me, cTermVertScr3);
        InitScreen(me, cTermVertScr4);

        SetReg(me, cHSmCO2, me->gh->AllTask.DoCO2, me->gh->InTeplSens[cSmCOSens].Value);

        me->gh->OtherCalc.MeasDifPress = _GD.TControl.MeteoSensing[cSmPresureSens]-_GD.TControl.MeteoSensing[cSmPresureSens+1];

        if (! me->gh->OtherCalc.MeasDifPress)
            me->gh->OtherCalc.MeasDifPress = 1;
        if ((! me->tctrl->MeteoSensing[cSmPresureSens]) || (! me->tctrl->MeteoSensing[cSmPresureSens+1]))
            me->gh->OtherCalc.MeasDifPress = 0;

        SetReg(me, cHSmPressReg, me->gh->AllTask.DoPressure, me->gh->OtherCalc.MeasDifPress);
        LaunchVent(me);
        SetLighting(me);
        SetCO2();               // CO2
    }
}

//Есть место для оптимизации!!!!!!!!!!!!!!!!!!!!
void SubConfig(char fnTepl)
{
    SetPointersOnTepl(fnTepl);
    for (int i = 0;i<cHSmPump;i++)
    {
        SetPointersOnKontur(i);
        if (i<cSKontur)
        {
            _GDCP.TControl_Tepl_Kontur->Separate = CheckSeparate(i);
            _GDCP.TControl_Tepl_Kontur->MainTepl = CheckMain(fnTepl);

            _GDCP.Hot_Hand_Kontur->RCS=
            _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].HandCtrl[i].RCS;
            _GDCP.Hot_Hand_Kontur->Position=
            _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].HandCtrl[i].Position;
            if (i<cSWaterKontur)
            {
                int byte_y = i+cHSmPump;
                _GDCP.TControl_Tepl_Kontur->SensValue = _GDP.Hot_Tepl->InTeplSens[i+cSmWaterSens].Value;
                _GDP.Hot_Hand[byte_y].RCS=
                _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].HandCtrl[byte_y].RCS;
                _GDP.Hot_Hand[byte_y].Position=
                _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].HandCtrl[byte_y].Position;
                _GDCP.TControl_Tepl_Kontur->SensValue=
                _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[i].SensValue;
            }
        }

    }

}
void Configuration(void)
{
    for (int gh_idx = 0; gh_idx<cSTepl; gh_idx++)
        SubConfig(gh_idx);
}
#ifdef Vitebsk
void    TransferWaterToBoil(void)
{
    creg.X = _GD.Hot.MaxReqWater/100; //Делаем запас на 5 градусов
//	IntX = IntX/100;
    creg.X++;
//	IntY = 0;
    switch (creg.X)
    {
    case 0:
    case 1:
    case 2:
    case 3: creg.Y = 1; break;
    case 4:
    case 5:
    case 6: creg.Y = creg.X-2; break;
    case 7: creg.Y = 0; break;
    case 8:
    case 9:
    case 10: creg.Y = creg.X-3; break;
    default: creg.Y = 7;
    }

    if (YesBit(creg.Y,0x01))
        write_output_bit(0,cHSmRegs+4,0,0);
    if (YesBit(creg.Y,0x02))
        write_output_bit(0,cHSmRegs+4,0,1);
    if (YesBit(creg.Y,0x04))
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
    dst[0] = _GD.Hot.Tepl[tCTepl].tempParamHeat;
    dst[1] = _GD.Hot.Tepl[tCTepl].tempParamVent;
    dst[2] = _GD.Hot.Tepl[tCTepl].tempHeat;
    dst[3] = _GD.Hot.Tepl[tCTepl].tempVent;
    dst[4] = _GD.Hot.Tepl[tCTepl].newsZone;
}

void loadSettings(char tCTepl, const int *src)
{
    _GD.Hot.Tepl[tCTepl].tempParamHeat = src[0];
    _GD.Hot.Tepl[tCTepl].tempParamVent = src[1];
    _GD.Hot.Tepl[tCTepl].tempHeat = src[2];
    _GD.Hot.Tepl[tCTepl].tempVent = src[3];
    _GD.Hot.Tepl[tCTepl].newsZone = src[4];
}

void control_pre(void)
{
    Configuration();
    SetAlarm();

    for (int gh_idx = 0;gh_idx<cSTepl;gh_idx++)
    {
        SetPointersOnTepl(gh_idx);
        SetSensOnMech();
        DoMechanics(gh_idx);
        SetDiskr(gh_idx);

        SetUpSiod(gh_idx);  // !!!


        DoSiod(gh_idx);
        DoPumps();
//			CheckReadyMeasure();
        DoVentCalorifer();
        DoLights();
//			DoPoisen();
        RegWorkDiskr(cHSmCO2);
        RegWorkDiskr(cHSmPressReg);
#ifdef Vitebsk
        TransferWaterToBoil();
#endif
    }
}

void control_post(int second, bool is_transfer_in_progress)
{
    for (int tCTepl = 0;tCTepl<cSTepl;tCTepl++)
    {
        SetPointersOnTepl(tCTepl);
        SetMixValvePosition();
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
            for (int tCTepl = 0;tCTepl<cSTepl;tCTepl++)
            {
                int shadow[8];
                saveSettings(tCTepl, shadow);
                memclr(&_GD.Hot.Tepl[tCTepl].ExtRCS,(
                                                   sizeof(char)*2+sizeof(eClimTask)+sizeof(eOtherCalc)+
                                                   sizeof(eNextTCalc)+sizeof(eKontur)*cSKontur+20));
                creg.Z=((_GD.Hot.Time+o_DeltaTime)%(24*60));
                TaskTimer(1,tCTepl,tCTepl);
                int ttTepl = tCTepl;
                while ((!_GD.Hot.Tepl[tCTepl].AllTask.NextTAir)&&(ttTepl))
                {
                    TaskTimer(1,--ttTepl,tCTepl);
                }
                creg.Z = _GD.Hot.Time;
                loadSettings(tCTepl, shadow);
                TaskTimer(0,ttTepl,tCTepl);
                SetPointersOnTepl(tCTepl);

                gh_ctx_t ctx = make_gh_ctx(tCTepl);

                SetTepl(&ctx);

                airHeat(&ctx);
            }
            __sCalcKonturs();
            __sMechWindows();
            __sMechScreen();
            for (int tCTepl = 0;tCTepl<cSTepl;tCTepl++)
            {
                if (_GD.Hot.MaxReqWater<_GD.Hot.Tepl[tCTepl].MaxReqWater)
                    _GD.Hot.MaxReqWater = _GD.Hot.Tepl[tCTepl].MaxReqWater;
                bWaterReset[tCTepl]=0;
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
        creg.X = ctx.PastPerRas;
        if (ctx.TecPerRas>creg.X) creg.X = ctx.TecPerRas;
        _GD.TControl.NowRasx=(long)_GD.TuneClimate.ScaleRasx*(long)23040/(long)creg.X/100;
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
        for (int tCTepl = 0;tCTepl<cSTepl;tCTepl++)
        {
//			GD.TControl.Tepl[tCTepl].Functional = 1;
            _GD.TControl.Tepl[tCTepl].TimeSIO = 0;
        }
        _GD.TControl.SumSun = 0;
        _GD.TControl.Date = _GD.Hot.Date;
        _GD.TControl.FullVol = 0;
    }
}

const gh_ctx_t make_gh_ctx(int gh_idx)
{
    gh_ctx_t ctx =
    {
        .idx = gh_idx,
        .gh = &_GD.Hot.Tepl[gh_idx],
        .hand = _GD.Hot.Tepl[gh_idx].HandCtrl,
        .gh_tctrl = &_GD.TControl.Tepl[gh_idx],
        .tctrl = &_GD.TControl,
    };

    return ctx;
}

void control_init(void)
{
    ClearAllAlarms();
    siodInit();
    airHeatInit();   // airHeat
}
