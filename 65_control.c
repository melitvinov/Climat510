#include "syntax.h"

#include "wtf.h"
#include "defs.h"
#include "65_gd.h"
#include "65_siod.h"
#include "65_subr.h"
#include "65_control.h"
#include "65_strategy3m.h"

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
    eTimer *Timer;
    uchar Volume;
    int8_t TimeReset;
    int16_t settingsVosx;
    int16_t settingsZax;
    bool fLightOn;
    int16_t fLightPause;
    uchar vNFCtr;
    uchar MaskRas;
    int PastPerRas;
    int TecPerRas;
    uchar DemoMode;
    eMechBusy *MBusy;
} control_ctx_t;


int8_t  bWaterReset[16];


uchar       bNight;


const uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};

// XXX: these 3 'registers' are used in control
int16_t IntX;
int16_t IntY;
int16_t IntZ;

static control_ctx_t ctx;


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
    IntZ=Next-Now;
    var=IntZ;
    var*=IntX;
    if (!IntY)
        return Now*Mull;
    var/=IntY;
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
    gdp.Hot_Tepl=&GD.Hot.Tepl[fnTeplLoad];
    gdp.Hot_Tepl->AllTask.TAir=0;

    IntZ= GD.Hot.Time + fsmTime;
    IntZ%=1440;
    MaxTimeStart=0;
    PrevTimeStart=0;
    NextTimeStart=1440;
    MinTimeStart=1440;
    sTimerNext=-1;
    sTimerPrev=-1;
    for (nTimer=0;nTimer<cSTimer;nTimer++) //20
    {
        ctx.Timer = &GD.Timer[nTimer];
        typeStartCorrection = controlTypeStartCorrection(ctx.Timer->TypeStart, ctx.Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
        //if (typeStartCorrection == -1)
        //    typeStartCorrection = GD.Timer[nTimer].TimeStart;

        if (!typeStartCorrection)
            continue;
        if (GD.Timer[nTimer].Zone[0]!=fnTeplTimer+1)
            continue;

        if (typeStartCorrection<MinTimeStart)
        {
            MinTimeStart=typeStartCorrection;
            sTimerMin=nTimer;
        }
        if (typeStartCorrection>MaxTimeStart)
        {
            MaxTimeStart=typeStartCorrection;
            sTimerMax=nTimer;
        }
        if ((typeStartCorrection>=IntZ)&&(NextTimeStart>typeStartCorrection))
        {
            NextTimeStart=typeStartCorrection;
            sTimerNext=nTimer;
        }
        if ((typeStartCorrection<IntZ)&&(PrevTimeStart<typeStartCorrection))
        {
            PrevTimeStart=typeStartCorrection;
            sTimerPrev=nTimer;
        }
    }
    if (MinTimeStart==1440) return;

    if (sTimerNext<0)
        sTimerNext=sTimerMin;

    if (sTimerPrev<0)
        sTimerPrev=sTimerMax;

    pGD_CurrTimer=&GD.Timer[sTimerPrev];
    pGD_NextTimer=&GD.Timer[sTimerNext];
    ctx.Timer = &GD.Timer[sTimerPrev];
    prevTimer = controlTypeStartCorrection(ctx.Timer->TypeStart, ctx.Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
    ctx.Timer = &GD.Timer[sTimerNext];
    nextTimer = controlTypeStartCorrection(ctx.Timer->TypeStart, ctx.Timer->TimeStart, ctx.settingsVosx, ctx.settingsZax);
    IntX= GD.Hot.Time - prevTimer;
    IntY= nextTimer - prevTimer;
//        if (!GD.Timer[nTimer].TimeStart)
//            continue;
//        if (GD.Timer[nTimer].Zone[0]!=fnTeplTimer+1)
//            continue;
//
//        if (GD.Timer[nTimer].TimeStart<MinTimeStart)
//        {
//            MinTimeStart=GD.Timer[nTimer].TimeStart;
//            sTimerMin=nTimer;
//        }
//        if (GD.Timer[nTimer].TimeStart>MaxTimeStart)
//        {
//            MaxTimeStart=GD.Timer[nTimer].TimeStart;
//            sTimerMax=nTimer;
//        }
//        if ((GD.Timer[nTimer].TimeStart>=IntZ)&&(NextTimeStart>GD.Timer[nTimer].TimeStart))
//        {
//            NextTimeStart=GD.Timer[nTimer].TimeStart;
//            sTimerNext=nTimer;
//        }
//        if ((GD.Timer[nTimer].TimeStart<IntZ)&&(PrevTimeStart<GD.Timer[nTimer].TimeStart))
//        {
//            PrevTimeStart=GD.Timer[nTimer].TimeStart;
//            sTimerPrev=nTimer;
//        }
//    }
//    if (MinTimeStart==1440) return;
//
//    if (sTimerNext<0)
//        sTimerNext=sTimerMin;
//
//    if (sTimerPrev<0)
//        sTimerPrev=sTimerMax;
//
//    pGD_CurrTimer=&GD.Timer[sTimerPrev];
//    pGD_NextTimer=&GD.Timer[sTimerNext];
//
//    IntX=CtrTime-GD.Timer[sTimerPrev].TimeStart;
//    IntY=GD.Timer[sTimerNext].TimeStart-GD.Timer[sTimerPrev].TimeStart;
    if (IntY<0)
    {
        IntY+=1440;
    }

    if (IntX<0)
    {
        IntX+=1440;
    }

    if (fsmTime)
    {
        gdp.Hot_Tepl->AllTask.NextTAir=JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1);
//Блокировка нулевой темепратуры вентиляции
        tVal=pGD_CurrTimer->TVentAir;
        if (!tVal) tVal=pGD_CurrTimer->TAir+100;
        gdp.Hot_Tepl->AllTask.NextTVent=JumpNext(tVal,pGD_NextTimer->TVentAir,1,1);
        gdp.Hot_Tepl->AllTask.Light=pGD_CurrTimer->Light;
        gdp.Hot_Tepl->AllTask.ModeLight=pGD_CurrTimer->ModeLight;
//		if (pGD_Hot_Tepl->InTeplSens[cSmRHSens])
//		pGD_Hot_Tepl->AllTask.NextRHAir=JumpNext(pGD_CurrTimer->RHAir,pGD_NextTimer->RHAir,1);
        return;
    }
#warning temp !!!!!!!!!!!!!!!!!!!!!!!!!!

    // T отопления, в зависимости что стоит в параметрах упраления, то и ставим в температуру
    gdp.Hot_Tepl->AllTask.TAir=JumpNext(pGD_CurrTimer->TAir,pGD_NextTimer->TAir,1,1);


    //if (pGD_Hot_Tepl->AllTask.TAir - TempOld > 50)
    //	NOP;
    //TempOld = pGD_Hot_Tepl->AllTask.TAir;

    //Блокировка нулевой темепратуры вентиляции
    tVal=pGD_CurrTimer->TVentAir;
    if (!tVal) tVal=pGD_CurrTimer->TAir+100;

    // T вентиляции
    gdp.Hot_Tepl->AllTask.DoTVent=JumpNext(tVal,pGD_NextTimer->TVentAir,1,1);

    gdp.Hot_Tepl->AllTask.SIO=pGD_CurrTimer->SIO;
    gdp.Hot_Tepl->AllTask.RHAir=JumpNext(pGD_CurrTimer->RHAir_c,pGD_NextTimer->RHAir_c,1,100);
    gdp.Hot_Tepl->AllTask.CO2=JumpNext(pGD_CurrTimer->CO2,pGD_NextTimer->CO2,1,1);
    gdp.Hot_Tepl->Kontur[cSmKontur1].MinTask=JumpNext(pGD_CurrTimer->MinTPipe1,pGD_NextTimer->MinTPipe1,1,10);

    gdp.Hot_Tepl->Kontur[cSmKontur2].MinTask=JumpNext(pGD_CurrTimer->MinTPipe2,pGD_NextTimer->MinTPipe2,1,10);

    gdp.Hot_Tepl->Kontur[cSmKontur3].MinTask=JumpNext(pGD_CurrTimer->MinTPipe3,pGD_NextTimer->MinTPipe3,1,10);
    gdp.Hot_Tepl->Kontur[cSmKontur5].MinTask=JumpNext(pGD_CurrTimer->MinTPipe5,pGD_NextTimer->MinTPipe5,1,10);

    gdp.Hot_Tepl->Kontur[cSmKontur1].Optimal=JumpNext(pGD_CurrTimer->TOptimal1,pGD_NextTimer->TOptimal1,1,10);

    gdp.Hot_Tepl->Kontur[cSmKontur2].Optimal=JumpNext(pGD_CurrTimer->TOptimal2,pGD_NextTimer->TOptimal2,1,10);

    gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinTask=JumpNext(((uchar)pGD_CurrTimer->MinOpenWin),((uchar)pGD_NextTimer->MinOpenWin),0,1);
    gdp.Hot_Tepl->AllTask.Win=pGD_CurrTimer->Win;
    gdp.Hot_Tepl->AllTask.Screen[0]=pGD_CurrTimer->Screen[0];
    gdp.Hot_Tepl->AllTask.Screen[1]=pGD_CurrTimer->Screen[1];
    gdp.Hot_Tepl->AllTask.Screen[2]=pGD_CurrTimer->Screen[2];
    gdp.Hot_Tepl->AllTask.Vent=pGD_CurrTimer->Vent;
//	pGD_Hot_Tepl->AllTask.Poise=pGD_CurrTimer->Poise;
    gdp.Hot_Tepl->Kontur[cSmKontur3].Do=JumpNext(pGD_CurrTimer->TPipe3,pGD_NextTimer->TPipe3,1,10);
    gdp.Hot_Tepl->Kontur[cSmKontur4].Do=JumpNext(pGD_CurrTimer->TPipe4,pGD_NextTimer->TPipe4,1,10);

}


void AllTaskAndCorrection(void)
{
    int sum;
    int val = 0;

    IntY=GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    /*Установка и коррекция по солнцу температуры обогрева*/
    gdp.Hot_Tepl->AllTask.DoTHeat=gdp.Hot_Tepl->AllTask.TAir;
    IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                        GD.TuneClimate.s_TConst,cbCorrTOnSun);
    SetBit(gdp.Hot_Tepl->RCS,IntX);
    gdp.Hot_Tepl->AllTask.DoTHeat+=IntZ;
    /*Коррекция прогноза*/
    gdp.Hot_Tepl->AllTask.NextTAir+=IntZ;
    if (gdp.Hot_Tepl->AllTask.DoTVent)
    {
        CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                       GD.TuneClimate.s_TVentConst,0);
        gdp.Hot_Tepl->AllTask.DoTVent+=IntZ;
        gdp.Hot_Tepl->AllTask.NextTVent+=IntZ;
    }
    else
    {
        gdp.Hot_Tepl->AllTask.DoTVent=gdp.Hot_Tepl->AllTask.DoTHeat+100;
        gdp.Hot_Tepl->AllTask.NextTVent=gdp.Hot_Tepl->AllTask.NextTAir+100;
    }
//	if ((*(pGD_Hot_Hand+cHSmScrTH)).Position)
//		pGD_Hot_Tepl->AllTask.NextRezTAir=pGD_Hot_Tepl->AllTask.NextTAir-GD.TuneClimate.sc_DoTemp;
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу заданной влажности*/
    if (gdp.Hot_Tepl->AllTask.RHAir)
    {
        gdp.Hot_Tepl->AllTask.DoRHAir=gdp.Hot_Tepl->AllTask.RHAir;
        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_RHConst,cbCorrRHOnSun);
        SetBit(gdp.Hot_Tepl->RCS,IntX);
        gdp.Hot_Tepl->AllTask.DoRHAir-=IntZ;
        /*Коррекция прогноза*/
//		pGD_Hot_Tepl->AllTask.NextRHAir-=IntZ;
    }
    /*---------------------------------------------------*/

    /*Установка и коррекция по солнцу заданной концентрации СО2*/
    if (gdp.Hot_Tepl->AllTask.CO2)
    {
//ОПТИМИЗАЦИЯ
// Коррекция СО2 держать по фрамугам
        gdp.Hot_Tepl->AllTask.DoCO2=gdp.Hot_Tepl->AllTask.CO2;    // так было, строчка лишняя но все же решил оставить

        if (gdp.Control_Tepl->co_model!=3)
        {
            sum =  (*(gdp.Hot_Hand+cHSmWinN)).Position + (*(gdp.Hot_Hand+cHSmWinS)).Position;
            if ((sum >= GD.TuneClimate.co2Fram1) && (sum <= GD.TuneClimate.co2Fram2))
            {
                if (gdp.Hot_Tepl->AllTask.CO2 > GD.TuneClimate.co2Off)
                {
                    if (GD.TuneClimate.co2Fram2 > GD.TuneClimate.co2Fram1)
                        val = GD.TuneClimate.co2Fram2 - GD.TuneClimate.co2Fram1;
                    val = ((sum - GD.TuneClimate.co2Fram1) * GD.TuneClimate.co2Off) / val;
                    gdp.Hot_Tepl->AllTask.DoCO2 = gdp.Hot_Tepl->AllTask.DoCO2 - val;
                }
            }
            if (sum > GD.TuneClimate.co2Fram2)
                gdp.Hot_Tepl->AllTask.DoCO2 = gdp.Hot_Tepl->AllTask.DoCO2 - GD.TuneClimate.co2Off;
        }

        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_CO2Const,cbCorrCO2OnSun);
        SetBit(gdp.Hot_Tepl->RCS,IntX);
        gdp.Hot_Tepl->AllTask.DoCO2+=IntZ;
    }
    /*---------------------------------------------------*/
    /*Установка и коррекция по солнцу минимальной температуры в контурах 1 и 2*/

    if (gdp.Hot_Tepl->Kontur[cSmKontur1].MinTask)
    {
        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_MinTPipeConst,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        gdp.Hot_Tepl->Kontur[cSmKontur1].MinCalc=gdp.Hot_Tepl->Kontur[cSmKontur1].MinTask-IntZ;
        IntY=DefRH();//MeteoSens[cSmFARSens].Value;

        IntX=CorrectionRule(GD.TuneClimate.c_RHStart,GD.TuneClimate.c_RHEnd,
                            GD.TuneClimate.c_RHOnMin1,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        gdp.Hot_Tepl->Kontur[cSmKontur1].MinCalc+=IntZ;

    }
    gdp.Hot_Tepl->Kontur[cSmKontur2].MinCalc=gdp.Hot_Tepl->Kontur[cSmKontur2].MinTask;
    if (gdp.Hot_Tepl->Kontur[cSmKontur2].MinTask)
    {
        IntY=DefRH();//MeteoSens[cSmFARSens].Value;

        IntX=CorrectionRule(GD.TuneClimate.c_RHStart,GD.TuneClimate.c_RHEnd,
                            GD.TuneClimate.c_RHOnMin2,0/*cbCorrMinTaskOnSun*/);
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].RCS,IntX);
        gdp.Hot_Tepl->Kontur[cSmKontur2].MinCalc+=IntZ;
    }

    IntY=GD.Hot.MidlSR;//MeteoSens[cSmFARSens].Value;
    if (gdp.Hot_Tepl->Kontur[cSmKontur3].MinTask)
    {
        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_MinTPipe3,0);
        gdp.Hot_Tepl->Kontur[cSmKontur3].MinCalc=gdp.Hot_Tepl->Kontur[cSmKontur3].MinTask-IntZ;
    }

//	pGD_Hot_Tepl->Kontur[cSmKontur3].MinCalc=pGD_Hot_Tepl->Kontur[cSmKontur3].MinTask;
    if (gdp.Hot_Tepl->Kontur[cSmKontur5].MinTask)
    {
        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_MinTPipe5,0/*cbCorrMinTaskOnSun*/);
        gdp.Hot_Tepl->Kontur[cSmKontur5].MinCalc=gdp.Hot_Tepl->Kontur[cSmKontur5].MinTask+IntZ;
    }

    gdp.Hot_Tepl->AllTask.DoPressure=gdp.Control_Tepl->c_DoPres;
    /*-------------------------------------------------------------*/

    /*Установка и коррекция по солнцу минимального положения подветренных фрамуг*/
    if (gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinTask)
    {
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc=gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinTask;
        IntX=CorrectionRule(GD.TuneClimate.s_TStart[0],GD.TuneClimate.s_TEnd,
                            GD.TuneClimate.s_MinOpenWinConst,0/*cbCorrMinTaskOnSun*/);
        SetBit(gdp.Hot_Tepl->Kontur[cSmWindowUnW].RCS,IntX);
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc+=IntZ;
    }
    /*----------------------------------------------------------------*/
    IntY=DefRH();
    CorrectionRule(GD.TuneClimate.f_min_RHStart,GD.TuneClimate.f_min_RHEnd,
                   GD.TuneClimate.f_CorrTVent,0);
    gdp.Hot_Tepl->AllTask.NextTVent-=IntZ;
    gdp.Hot_Tepl->AllTask.DoTVent-=IntZ;
    IntY=-IntY;

    CorrectionRule(GD.TuneClimate.f_max_RHStart,GD.TuneClimate.f_max_RHEnd,
                   GD.TuneClimate.f_CorrTVentUp,0);
    gdp.Hot_Tepl->AllTask.NextTVent+=IntZ;
    gdp.Hot_Tepl->AllTask.DoTVent+=IntZ;
    /*--------------------------------------------------------------*/

}
void SetIfReset(void)
{

    for (int i = 0; i < cSWaterKontur; i++)
    {
        SetPointersOnKontur(i);
        gdp.TControl_Tepl_Kontur->DoT=gdp.TControl_Tepl_Kontur->SensValue*10;//((long int)pGD_Hot_Tepl->InTeplSens[ByteX+cSmWaterSens].Value)*100;
        gdp.TControl_Tepl_Kontur->PumpPause=cPausePump;
        gdp.TControl_Tepl_Kontur->PumpStatus=1;
    }
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinS)).Position))*10;
//	pGD_TControl_Tepl->Kontur[cSmWindowUnW+1-GD.Hot.PozFluger].DoT=(((*(pGD_Hot_Hand+cHSmWinN)).Position))*10;

}

/**********************************************************************/
/*-*-*-*-*--Нахождение прогнозируемого изменения температуры--*-*-*-*-*/
/**********************************************************************/
#warning Прогнозы температуры по внешним факторам !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void __cNextTCalc(char fnTepl)
{
    int CalcAllKontur;

    if (!gdp.Hot_Tepl->AllTask.NextTAir) return;


    if (bWaterReset[fnTepl])
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

//	pGD_Hot_Tepl->NextTCalc.DifTAirTDo=pGD_Hot_Tepl->AllTask.NextTAir-CURRENT_TEMP_VALUE;
/**********************************************/
/*СУПЕР АЛГОРИТМ ДЛЯ РАСЧЕТА*/
//	pGD_Hot_Tepl->AllTask.Rez[0]=CURRENT_TEMP_VALUE;
//	IntX=(pGD_Hot_Tepl->AllTask.DoTHeat-CURRENT_TEMP_VALUE);

#warning NEW CHECK THIS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// смотря по какому датчику работаем того и считаем
// ---------------------
// NEW
    int cSmTSens=0;
    for (cSmTSens=0; cSmTSens<4; cSmTSens++)  // 4 датчика температуры
    {
        gdp.Level_Tepl[cSmTSens][cSmUpAlarmLev]=0;
        gdp.Level_Tepl[cSmTSens][cSmDownAlarmLev]=0;
        if (GD.TuneClimate.c_MaxDifTUp)
            gdp.Level_Tepl[cSmTSens][cSmUpAlarmLev]=gdp.Hot_Tepl->AllTask.DoTHeat+GD.TuneClimate.c_MaxDifTUp;
        if (GD.TuneClimate.c_MaxDifTDown)
            gdp.Level_Tepl[cSmTSens][cSmDownAlarmLev]=gdp.Hot_Tepl->AllTask.DoTHeat-GD.TuneClimate.c_MaxDifTDown;
    }

    gdp.Hot_Tepl->NextTCalc.DifTAirTDo=gdp.Hot_Tepl->AllTask.NextTAir-getTempHeat(fnTepl);
    /**********************************************/
    /*СУПЕР АЛГОРИТМ ДЛЯ РАСЧЕТА*/
    gdp.Hot_Tepl->AllTask.Rez[0]=getTempHeat(fnTepl);
    IntX=(gdp.Hot_Tepl->AllTask.DoTHeat-getTempHeat(fnTepl));

/**********************************************/
/*Вычиляем увеличение от солнечной радиации*/
    IntY=GD.Hot.MidlSR;
    if ((!YesBit(gdp.Hot_Tepl->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        IntY=gdp.Hot_Tepl->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(GD.TuneClimate.c_SRStart,GD.TuneClimate.c_SREnd,
                   GD.TuneClimate.c_SRFactor,0);
    gdp.Hot_Tepl->NextTCalc.UpSR=IntZ;
/*Вычиляем увеличение от разницы температуры задания и стекла*/
    IntY=gdp.Hot_Tepl->AllTask.NextTAir-gdp.Hot_Tepl->InTeplSens[cSmGlassSens].Value;

    CorrectionRule(GD.TuneClimate.c_GlassStart,GD.TuneClimate.c_GlassEnd,
                   GD.TuneClimate.c_GlassFactor,0);
    gdp.Hot_Tepl->NextTCalc.LowGlass=IntZ;

    if (gdp.TControl_Tepl->Screen[0].Mode < 2)
        //if (pGD_TControl_Tepl->Screen[0].Mode < 2)
        gdp.Hot_Tepl->NextTCalc.CorrectionScreen = GD.TuneClimate.CorrectionScreen * gdp.TControl_Tepl->Screen[0].Mode;

    //vdv
    //if (pGD_TControl_Tepl->Screen[0].Mode)

    //IntY = pGD_TControl_Tepl->Screen[0].Value;
    //if (pGD_Hot_Tepl->AllTask.Screen[0] == 2)
    //{
    //	if ((*(pGD_Hot_Hand+cHSmScrTH+0)).Position)
    //	{
    //		CorrectionRule(100, 0, GD.TuneClimate.CorrectionScreen,0);
    //		pGD_Hot_Tepl->NextTCalc.CorrectionScreen=IntZ;
    //	}
    //}
//		screenOldPosiyion = screenTermoGetPossition();
//
//		if ((screenTermoGetPossition() != 0) && (screenTermoGetCurrentPossition() > 0))
//		{
//			CorrectionRule(100, 0, GD.TuneClimate.CorrectionScreen,0);
//			pGD_Hot_Tepl->NextTCalc.CorrectionScreen=IntZ;
    // расчет. Экран понижает t отопление на
    //GD.TuneClimate.CorrectionScreen
//	}
//		}

/*Вычисляем корректировки ветра фрамуг и разницы между температурой задания
и внешней температуры соответственно*/
/*Ветер и фрамуги увеличивают эту разницу*/
    IntY=GD.Hot.MidlWind;
    CorrectionRule(GD.TuneClimate.c_WindStart,GD.TuneClimate.c_WindEnd,
                   GD.TuneClimate.c_WindFactor,0);
    IntY=gdp.Hot_Tepl->AllTask.NextTAir-GD.TControl.MeteoSensing[cSmOutTSens]-IntZ;
    CorrectionRule(GD.TuneClimate.c_OutStart,GD.TuneClimate.c_OutEnd,
                   GD.TuneClimate.c_OutFactor,0);
    gdp.Hot_Tepl->NextTCalc.LowOutWinWind+=IntZ;

//	if (YesBit(pGD_Hot_Tepl->DiskrSens[0],cSmLightDiskr))
    gdp.Hot_Tepl->NextTCalc.UpLight=(((long)GD.TuneClimate.c_LightFactor)*((*(gdp.Hot_Hand+cHSmLight)).Position))/100;
//******************** NOT NEEDED
//	IntY=1;
//	IntY<<=fnTepl;
//	if (YesBit(GD.Hot.isLight,IntY))
//		pGD_Hot_Tepl->NextTCalc.UpLight=GD.TuneClimate.c_LightFactor;
//********************************************************

/*Считаем сумму поправок*/
    gdp.Hot_Tepl->NextTCalc.dSumCalc=
    +gdp.Hot_Tepl->NextTCalc.UpSR
    -gdp.Hot_Tepl->NextTCalc.LowGlass
    -gdp.Hot_Tepl->NextTCalc.LowOutWinWind
    +gdp.Hot_Tepl->NextTCalc.UpLight
    -gdp.Hot_Tepl->NextTCalc.CorrectionScreen;

//		-pGD_Hot_Tepl->NextTCalc.LowRain;
    if (GD.TControl.bSnow)
        gdp.Hot_Tepl->NextTCalc.dSumCalc-=GD.TuneClimate.c_CloudFactor;
/*********************************************************************
******* СЧИТАЕМ СУММУ ВЛИЯНИЙ ДЛЯ ФРАМУГ *******************************
***********************************************************************/
    gdp.Hot_Tepl->NextTCalc.dSumCalcF=0;
/*Вычиляем увеличение от солнечной радиации*/
    IntY=GD.Hot.MidlSR;
/*if work on internal light sensor, then change IntY*/

    if ((!YesBit(gdp.Hot_Tepl->InTeplSens[cSmInLightSens].RCS,cbNoWorkSens)))
        IntY=gdp.Hot_Tepl->InTeplSens[cSmInLightSens].Value;

    CorrectionRule(GD.TuneClimate.c_SRStart,GD.TuneClimate.c_SREnd,
                   GD.TuneClimate.f_SRFactor,0);
    gdp.Hot_Tepl->NextTCalc.dSumCalcF+=IntZ;

/*Вычисляем корректировки ветра фрамуг и разницы между температурой задания
и внешней температуры соответственно*/
/*Ветер и фрамуги увеличивают эту разницу*/
    IntY=GD.Hot.MidlWind;
    CorrectionRule(GD.TuneClimate.c_WindStart,GD.TuneClimate.c_WindEnd,
                   GD.TuneClimate.f_WindFactor,0);
    IntY=gdp.Hot_Tepl->AllTask.NextTAir-GD.TControl.MeteoSensing[cSmOutTSens]-IntZ;
    CorrectionRule(GD.TuneClimate.c_OutStart,GD.TuneClimate.c_OutEnd,
                   GD.TuneClimate.f_OutFactor,0);
    gdp.Hot_Tepl->NextTCalc.dSumCalcF+=IntZ;
/*********************************************************************
***********************************************************************
***********************************************************************/



    IntY=gdp.Hot_Tepl->NextTCalc.DifTAirTDo;
    if ((GD.TuneClimate.c_MullDown>10)&&(IntY<0)&&(GD.TuneClimate.c_MullDown<30))
        IntY=(((long)IntY)*GD.TuneClimate.c_MullDown)/10;
    gdp.Hot_Tepl->NextTCalc.PCorrection=((int)((((long)(IntY))*((long)gdp.Control_Tepl->c_PFactor))/100));
    if (gdp.TControl_Tepl->StopI<2)
        gdp.TControl_Tepl->Integral+=((((long)(gdp.Hot_Tepl->NextTCalc.DifTAirTDo))*((long)gdp.Control_Tepl->c_IFactor))/10);
    if (gdp.TControl_Tepl->Integral>2000000)
        gdp.TControl_Tepl->Integral=2000000;
    if (gdp.TControl_Tepl->Integral<-2000000)
        gdp.TControl_Tepl->Integral=-2000000;
    if (!gdp.Control_Tepl->c_IFactor)
        gdp.TControl_Tepl->Integral=0;
    gdp.Hot_Tepl->NextTCalc.ICorrection=(int)(gdp.TControl_Tepl->Integral/100);
    gdp.TControl_Tepl->Critery=gdp.Hot_Tepl->NextTCalc.PCorrection+gdp.Hot_Tepl->NextTCalc.ICorrection-gdp.Hot_Tepl->NextTCalc.dSumCalc;
    CalcAllKontur=__sCalcTempKonturs();
    gdp.TControl_Tepl->Critery-=CalcAllKontur;
//	pGD_Hot_Tepl->NextTCalc.dNextTCalc=CalcAllKontur;
    if (gdp.TControl_Tepl->StopI>4)
    {
        gdp.TControl_Tepl->Integral=gdp.TControl_Tepl->SaveIntegral;
    }
//	IntY=pGD_Hot_Tepl->NextTCalc.DifTAirTDo;
    gdp.TControl_Tepl->SaveIntegral=gdp.TControl_Tepl->Integral;
    if ((gdp.TControl_Tepl->StopI>3)&&(ABS(IntY)<cResetDifTDo))
    {

//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (gdp.TControl_Tepl->Critery>cResetCritery)
        {
            gdp.TControl_Tepl->SaveIntegral
            =cResetCritery+CalcAllKontur
             -gdp.Hot_Tepl->NextTCalc.PCorrection+gdp.Hot_Tepl->NextTCalc.dSumCalc;
            gdp.TControl_Tepl->SaveIntegral*=100;
        }
//		IntY=-IntY;
//		CorrectionRule(0,200,1000,0);
//		IntZ--;
        if (gdp.TControl_Tepl->Critery<-cResetCritery)
        {
            gdp.TControl_Tepl->SaveIntegral
            =-cResetCritery+CalcAllKontur
             -gdp.Hot_Tepl->NextTCalc.PCorrection+gdp.Hot_Tepl->NextTCalc.dSumCalc;
            gdp.TControl_Tepl->SaveIntegral*=100;
        }
    }
    if ((gdp.TControl_Tepl->StopI>3)&&(!SameSign(IntY,gdp.TControl_Tepl->Critery)))
    {
        gdp.TControl_Tepl->SaveIntegral
        =IntY+CalcAllKontur
         -gdp.Hot_Tepl->NextTCalc.PCorrection+gdp.Hot_Tepl->NextTCalc.dSumCalc;
        gdp.TControl_Tepl->SaveIntegral*=100;
    }
    if (!gdp.TControl_Tepl->Critery)
    {
        gdp.TControl_Tepl->Critery=1;
        if (gdp.Hot_Tepl->NextTCalc.DifTAirTDo<0)
            gdp.TControl_Tepl->Critery=-1;

    }
    gdp.Hot_Tepl->NextTCalc.Critery=gdp.TControl_Tepl->Critery;

/******************************************************************
        Далее расчет критерия для фрамуг
*******************************************************************/
    if (getTempVent(fnTepl))
        IntY=getTempVent(fnTepl)-gdp.Hot_Tepl->AllTask.DoTVent;
    else IntY=0;

    gdp.Hot_Tepl->NextTCalc.PCorrectionVent=((int)((((long)(IntY))*((long)gdp.Control_Tepl->f_PFactor))/100));
    if (gdp.TControl_Tepl->StopVentI<2)
        gdp.TControl_Tepl->IntegralVent+=((((long)(IntY))*((long)gdp.Control_Tepl->f_IFactor))/10);
    if (gdp.TControl_Tepl->IntegralVent<0) gdp.TControl_Tepl->IntegralVent=0;
    gdp.Hot_Tepl->NextTCalc.ICorrectionVent=(int)(gdp.TControl_Tepl->IntegralVent/100);
    if (!gdp.Control_Tepl->f_IFactor)
        gdp.TControl_Tepl->IntegralVent=0;

    IntX=gdp.Hot_Tepl->NextTCalc.PCorrectionVent+gdp.Hot_Tepl->NextTCalc.ICorrectionVent+gdp.Hot_Tepl->NextTCalc.dSumCalcF;
//Блокировка фрамуг при отоплении
    if ((gdp.TControl_Tepl->TVentCritery<IntX)&&(!gdp.TControl_Tepl->StopI)&&(IntX>0)&&((gdp.Control_Tepl->f_PFactor%100)>89))
    {
        gdp.TControl_Tepl->IntegralVent=gdp.TControl_Tepl->TVentCritery-gdp.Hot_Tepl->NextTCalc.PCorrectionVent-gdp.Hot_Tepl->NextTCalc.dSumCalcF;
        gdp.TControl_Tepl->IntegralVent*=100;
        IntX=gdp.TControl_Tepl->TVentCritery;
    }
    gdp.TControl_Tepl->TVentCritery=IntX;
//	if (!SameSign(pGD_TControl_Tepl->TVentCritery,pGD_TControl_Tepl->LastTVentCritery))
//	  	pGD_TControl_Tepl->StopVentI=0;
    if (gdp.TControl_Tepl->StopVentI>4)
    {
        gdp.TControl_Tepl->IntegralVent=gdp.TControl_Tepl->SaveIntegralVent;
    }
    if (gdp.TControl_Tepl->StopVentI>3)
    {
        gdp.TControl_Tepl->SaveIntegralVent=gdp.TControl_Tepl->IntegralVent;
        CorrectionRule(0,100,500,0);
        if (gdp.TControl_Tepl->AbsMaxVent>0)
            IntZ+=gdp.TControl_Tepl->AbsMaxVent;
        if (gdp.TControl_Tepl->TVentCritery>IntZ)
        {
            gdp.TControl_Tepl->SaveIntegralVent
            =IntZ-gdp.Hot_Tepl->NextTCalc.PCorrectionVent-gdp.Hot_Tepl->NextTCalc.dSumCalcF;
            gdp.TControl_Tepl->SaveIntegralVent*=100;
        }
        IntY=-IntY;
        CorrectionRule(0,100,500,0);
        IntZ++;
        if (gdp.TControl_Tepl->TVentCritery<-IntZ)
        {
            gdp.TControl_Tepl->SaveIntegralVent
            =-IntZ-gdp.Hot_Tepl->NextTCalc.PCorrectionVent-gdp.Hot_Tepl->NextTCalc.dSumCalcF;
            gdp.TControl_Tepl->SaveIntegralVent*=100;
        }
    }
    gdp.Hot_Tepl->NextTCalc.TVentCritery=gdp.TControl_Tepl->TVentCritery;

}

/*------------------------------------------------------*/
void    SetMixValvePosition(void)
{
    int16_t *IntVal;
    for (int i=0;i<cSWaterKontur;i++)
    {
        SetPointersOnKontur(i);
        if (YesBit((*(gdp.Hot_Hand_Kontur+cHSmMixVal)).RCS,(/*cbNoMech+*/cbManMech))) continue;
        IntVal=&(gdp.TControl_Tepl->IntVal[i]);
        if (!gdp.TControl_Tepl_Kontur->PumpStatus)
        {
            (*(gdp.Hot_Hand_Kontur+cHSmMixVal)).Position=0;
            continue;
        }
        gdp.TControl_Tepl_Kontur->TPause = clamp_min(gdp.TControl_Tepl_Kontur->TPause, 0);// pGD_TControl_Tepl->Kontur[ByteX].TPause=0;
        if (YesBit(gdp.TControl_Tepl->MechBusy[i].RCS,cMSBusyMech)) continue;
        if (gdp.TControl_Tepl_Kontur->TPause)
        {
            gdp.TControl_Tepl_Kontur->TPause--;
            continue;
        }
        gdp.TControl_Tepl_Kontur->TPause=cMinPauseMixValve;

        IntX=gdp.Hot_Tepl_Kontur->Do-gdp.TControl_Tepl_Kontur->SensValue;
        //(*IntVal)=(*IntVal)+IntX;
        long long_y=gdp.ConstMechanic->ConstMixVal[i].v_PFactor;
        long_y=long_y*IntX;//(*IntVal);
        IntY=(int16_t)(long_y/10000);
        //if (!IntY) continue;
        IntZ=(*IntVal)/100;
        //IntZ=(*(pGD_Hot_Hand_Kontur+cHSmMixVal)).Position;
        IntZ+=IntY;
        if (IntZ>100)
        {
            (*IntVal)=(100-IntY)*100;
            IntZ=100;
        }
        else
            if (IntZ<0)
        {
            (*IntVal)=(-IntY)*100;
            IntZ=0;
        }
        else
            (*IntVal)+=(int16_t)((((long)IntX)*gdp.ConstMechanic->ConstMixVal[i].v_IFactor)/100);

        //ogrMax(&IntZ,100);//if (IntZ>100) IntZ=100;
        //ogrMin(&IntZ,0);//if (IntZ<0)	IntZ=0;
        (*(gdp.Hot_Hand_Kontur+cHSmMixVal)).Position=(char)(IntZ);
    }
}

void    DoPumps(void)
{
    for (int i=0; i < cSWaterKontur; i++)
    {
        if (!(YesBit((*(gdp.Hot_Hand+cHSmPump+i)).RCS,(/*cbNoMech+*/cbManMech))))
            (*(gdp.Hot_Hand+cHSmPump+i)).Position=gdp.TControl_Tepl->Kontur[i].PumpStatus;
    }

}

#warning вкл воздушного обогревателя !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void    DoVentCalorifer(void)
{

//	if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,cbManMech)) return;
//		(*(pGD_Hot_Hand+cHSmHeat)).Position=pGD_TControl_Tepl->Calorifer;
//
//
    if (!(YesBit((*(gdp.Hot_Hand+cHSmVent)).RCS,(/*cbNoMech+*/cbManMech))))   // было так
    {
        (*(gdp.Hot_Hand+cHSmVent)).Position=gdp.TControl_Tepl->Vent;
        (*(gdp.Hot_Hand+cHSmVent)).Position+=gdp.TControl_Tepl->OutFan<<1;
    }
//	if (!(YesBit((*(pGD_Hot_Hand+cHSmHeat)).RCS,(/*cbNoMech+*/cbManMech))))
//	{
//
//		(*(pGD_Hot_Hand+cHSmHeat)).Position=pGD_TControl_Tepl->Calorifer;
//	}
}


#warning вкл подсветки !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void    DoLights(void)
{
    if (YesBit((*(gdp.Hot_Hand+cHSmLight)).RCS,(/*cbNoMech+*/cbManMech))) return;
//	pGD_Hot_Hand[cHSmLight].Position=0;
    gdp.Hot_Hand[cHSmLight].Position=gdp.TControl_Tepl->LightValue;
}

/*void	DoPoisen(void)
{
    if (YesBit((*(pGD_Hot_Hand+cHSmPoise)).RCS,(cbManMech))) return;
    pGD_Hot_Hand[cHSmPoise].Position=0;
        if (pGD_Hot_Tepl->AllTask.Poise)
            pGD_Hot_Hand[cHSmPoise].Position=1;
} */

void    SetSensOnMech(void)
{
    for (int i=0;i<cSRegCtrl;i++)
        gdp.TControl_Tepl->MechBusy[i].Sens=0;
    gdp.TControl_Tepl->MechBusy[cHSmWinN].Sens=&gdp.Hot_Tepl->InTeplSens[cSmWinNSens];
    gdp.TControl_Tepl->MechBusy[cHSmWinS].Sens=&gdp.Hot_Tepl->InTeplSens[cSmWinSSens];
    gdp.TControl_Tepl->MechBusy[cHSmScrTH].Sens=&gdp.Hot_Tepl->InTeplSens[cSmScreenSens];
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
    for (fnTepl=0;fnTepl<cSTepl;fnTepl++)
        for (fnAlr=0;fnAlr<cSRegCtrl;fnAlr++)
        {
            ClrBit(GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].RCS,cMSAlarm);
            GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].TryMove=0;
            GD.TControl.Tepl[fnTepl].MechBusy[fnAlr].TryMes=0;
        }
    for (fnAlr=0;fnAlr<MAX_ALARMS;fnAlr++)
        GD.TControl.Tepl[fnTepl].Alarms[fnAlr];
}

void SetAlarm(void)
{
    char fnTepl;
    for (fnTepl=0;fnTepl<GD.Control.ConfSTepl;fnTepl++)
        __SetBitOutReg(fnTepl,cHSmAlarm,1,0);
    for (fnTepl=0;fnTepl<GD.Control.ConfSTepl;fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        gdp.TControl_Tepl->bAlarm=0;
        if ((YesBit(gdp.Hot_Tepl->RCS,(cbNoTaskForTepl+cbNoSensingTemp+cbNoSensingOutT)))
            //	||(YesBit(pGD_Hot_Tepl->InTeplSens[cSmTSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            ||(YesBit(gdp.Hot_Tepl->InTeplSens[cSmWaterSens].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens))))
        {
            __SetBitOutReg(fnTepl,cHSmAlarm,0,0);
            gdp.TControl_Tepl->bAlarm=100;
        }

        if (getTempHeatAlarm(fnTepl) == 0)
        {
            __SetBitOutReg(fnTepl,cHSmAlarm,0,0);
            gdp.TControl_Tepl->bAlarm=100;
        }

        if (getTempVentAlarm(fnTepl) == 0)
        {
            __SetBitOutReg(fnTepl,cHSmAlarm,0,0);
            gdp.TControl_Tepl->bAlarm=100;
        }

        for (int i=0;i<cConfSSens;i++)
        {
            if (YesBit(gdp.Hot_Tepl->InTeplSens[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
            {
                __SetBitOutReg(fnTepl,cHSmAlarm,0,0);
                gdp.TControl_Tepl->bAlarm=100;
            }
        }
    }
    for (int i=0;i<cConfSMetSens;i++)
        if (YesBit(GD.Hot.MeteoSensing[i].RCS,(cbUpAlarmSens+cbDownAlarmSens+cbMinMaxVSens)))
        {
            __SetBitOutReg(cSmZone1,cHSmAlarm,0,0);
            GD.TControl.Tepl[cSmZone1].bAlarm=100;
        }

}

void SetDiskr(char fnTepl)
{
    int nLight;
    char tMaxLight;

    if (!(YesBit((*(gdp.Hot_Hand+cHSmAHUSpeed1)).RCS,cbManMech)))
        (*(gdp.Hot_Hand+cHSmAHUSpeed1)).Position=gdp.Hot_Tepl->Kontur[cSmKontur4].Do/10;
    if (!(YesBit((*(gdp.Hot_Hand+cHSmAHUSpeed2)).RCS,cbManMech)))
        (*(gdp.Hot_Hand+cHSmAHUSpeed2)).Position=gdp.Hot_Tepl->Kontur[cSmKontur4].Do/10;

    for (int i=cHSmPump;i<cHSmRegs;i++)
    {
        //if ((ByteX==cHSmSIOVals)||(ByteX==cHSmLight)) continue;
        if ((i==cHSmSIOPump)||(i==cHSmSIOVals)||(i==cHSmLight)) continue;

        __SetBitOutReg(fnTepl,i,1,0);

        if (YesBit((*(gdp.Hot_Hand+i)).Position,0x01))
            __SetBitOutReg(fnTepl,i,0,0);
        if (((i==cHSmHeat)||(i==cHSmVent))&&(YesBit((*(gdp.Hot_Hand+i)).Position,0x02)))
            __SetBitOutReg(fnTepl,i,0,1);
    }
    nLight=0;
    if (((uchar)((*(gdp.Hot_Hand+cHSmLight)).Position))>100) (*(gdp.Hot_Hand+cHSmLight)).Position=100;
    if ((gdp.Hot_Tepl->AllTask.DoTHeat)||(YesBit((*(gdp.Hot_Hand+cHSmLight)).RCS,cbManMech)))
    {
        nLight=((*(gdp.Hot_Hand+cHSmLight)).Position-50)/10+2;
        if (nLight<1) nLight=1;
    }
    ctx.fLightOn=0;
    if (nLight>1)
    {
        __SetBitOutReg(fnTepl,cHSmLight,0,0);
        ctx.fLightOn=1;
    }
    tMaxLight=8;

    switch (gdp.Control_Tepl->sLight)
    {
    case 2:
        if (nLight==7) nLight=0x04;
        else
            if (nLight>1) nLight=0x02;
        else
            if (nLight==1) nLight=0x01;
        break;
    case 1:
        if (nLight==7) nLight=1;
        else nLight=0;
        break;
    case 8:
        if (nLight!=1) nLight=0;
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
        tMaxLight=gdp.Control_Tepl->sLight-10;
        if (ctx.fLightPause>cLightDelay*8) ctx.fLightPause=cLightDelay*8;
        if (ctx.fLightPause<0) ctx.fLightPause=0;
        if (ctx.fLightOn)
        {
            nLight=~(0xff<<(ctx.fLightPause/cLightDelay));
            ctx.fLightPause++;
        }
        else
        {
            nLight=~(0xff>>(ctx.fLightPause/cLightDelay));
            ctx.fLightPause--;
        }

        break;
    default:
        nLight=0;
        break;

    }

    for (int i=0;i<tMaxLight;i++)
    {
        if (YesBit(nLight,(0x01<<i)))
            __SetBitOutReg(fnTepl,cHSmLight,0,i+1);

    }

/*	if (YesBit((*(pGD_Hot_Hand+cHSmVent)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmVent,0,0);
    if (YesBit((*(pGD_Hot_Hand+cHSmHeat)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmHeat,0,0);*/
    int i=1;
    if (gdp.Control_Tepl->co_model>=2) i=2;

    if ((gdp.TControl_Tepl->SetupRegs[0].On)
        &&(gdp.Control_Tepl->co_model))
        __SetBitOutReg(fnTepl,cHSmCO2,0,i);

    // насос
    //__SetBitOutReg(fnTepl,cHSmSIOPump,1,0);
    if (YesBit((*(gdp.Hot_Hand+cHSmSIOPump)).Position,0x01))
        __SetBitOutReg(fnTepl,cHSmSIOPump,0,0);

    for (int i=0;i<4;i++)
    {
        IntX=1;
        IntX<<=i;
        if (YesBit((*(gdp.Hot_Hand+cHSmSIOVals)).Position,IntX))
            __SetBitOutReg(fnTepl,cHSmSIOVals,0,i);
    }

#ifdef AGAPOVSKIY_DOUBLE_VALVE
    if (YesBit((*(gdp.Hot_Hand+cHSmSIOVals)).Position,0x02))
        __SetBitOutReg(fnTepl,cHSmAHUVals,0,0);
#endif
    for (int i=0;i<5;i++)
    {
        if (GD.Hot.Regs[i])
            __SetBitOutReg(fnTepl,i+cHSmRegs,0,0);
    }
}

void DoMechanics(char fnTepl)
{
    char fErr;
    for (int i=cHSmMixVal;i<cHSmPump;i++)
    {
        SetPointersOnKontur(i);
//		pGD_Hot_Hand_Kontur=pGD_Hot_Hand+ByteX;
        ctx.MBusy=&(gdp.TControl_Tepl->MechBusy[i]);

        if (gdp.Hot_Hand_Kontur->Position>100)
            gdp.Hot_Hand_Kontur->Position=100;
        if (gdp.Hot_Hand_Kontur->Position<0)
            gdp.Hot_Hand_Kontur->Position=0;

        if ((i==cHSmAHUSpeed1))
        {
//			Sound;
            SetOutIPCReg(gdp.Hot_Hand_Kontur->Position,mtRS485,GD.MechConfig[fnTepl].RNum[i],&fErr,&GD.FanBlock[fnTepl][0].FanData[0]);
            continue;
        }
/*		GD.FanBlock[fnTepl][0].FanData[0].ActualSpeed=fnTepl*5;
        GD.FanBlock[fnTepl][0].FanData[1].ActualSpeed=fnTepl*5+1;
        GD.FanBlock[fnTepl][1].FanData[1].ActualSpeed=fnTepl*5+2;
        GD.FanBlock[fnTepl][1].FanData[2].ActualSpeed=fnTepl*5+3;
*/
        if ((i==cHSmAHUSpeed2))
        {
//			Sound;
            SetOutIPCReg(gdp.Hot_Hand_Kontur->Position,mtRS485,GD.MechConfig[fnTepl].RNum[i],&fErr,&GD.FanBlock[fnTepl][1].FanData[0]);
            continue;
        }


        if ((i==cHSmCO2)&&(gdp.Control_Tepl->co_model==1)) continue;

        __SetBitOutReg(fnTepl,i,1,0);
        __SetBitOutReg(fnTepl,i,1,1);

        ClrBit(ctx.MBusy->RCS,cMSBusyMech);
        int byte_y = 0;
        if ((!YesBit(ctx.MBusy->RCS,cMSAlarm))&&(ctx.MBusy->Sens)&&(!YesBit(ctx.MBusy->Sens->RCS,cbNoWorkSens))&&(GD.TuneClimate.f_MaxAngle))
        {
            ctx.MBusy->PauseMech=10;
            if (YesBit(ctx.MBusy->RCS,cMSFreshSens))
            {
                ctx.MBusy->PauseMech=0;
                ClrBit(ctx.MBusy->RCS,cMSFreshSens);
//				if ((MBusy->PrevDelta>10)&&(MBusy->Sens->Value-MBusy->PrevTask>10))
//					MBusy->CalcTime=(((long)MBusy->CalcTime)*MBusy->PrevDelta/(MBusy->Sens->Value-MBusy->PrevTask));
                //(MBusy->PrevPosition-MBusy->CurrPosition)
                if (ctx.MBusy->TryMove>4)
                {
                    SetBit(ctx.MBusy->RCS,cMSAlarm);
                    continue;
                }
                if (ctx.MBusy->PrevTask==gdp.Hot_Hand_Kontur->Position*10)
                {
                    IntY=GD.TuneClimate.f_MaxAngle*10;
                    IntY = clamp_max(IntY, 50);
                    IntY = clamp_min(IntY, 10);
                    if (abs(ctx.MBusy->Sens->Value-ctx.MBusy->PrevTask)>IntY)
                    {
                        ctx.MBusy->TryMes++;
                        if (ctx.MBusy->TryMes>4)
                        {
                            ctx.MBusy->TryMes=0;
                            ctx.MBusy->TryMove+=(abs(ctx.MBusy->Sens->Value-ctx.MBusy->PrevTask)/IntY);
                        }
                        else continue;
                    }


                    long long_x = ctx.MBusy->Sens->Value;
                    long_x *= gdp.ConstMechanic_Mech->v_TimeMixVal;//MBusy->CalcTime;
                    long_x /= 1000;

                    if (abs(ctx.MBusy->Sens->Value-ctx.MBusy->PrevTask)<=IntY)
                    {
                        ctx.MBusy->TryMove=0;
                        if (ctx.MBusy->PrevDelta>10)
                        {
                            ctx.MBusy->TimeRealMech++;
                        }
                        if (ctx.MBusy->PrevDelta<-10)
                        {
                            ctx.MBusy->TimeRealMech--;
                        }
                    }
                    else
                    {
                        ctx.MBusy->TimeRealMech=(int)long_x;
                    }
                }
                ctx.MBusy->PrevTask=gdp.Hot_Hand_Kontur->Position*10;
            }
            //else return;
        }
//Выход из паузы при блокировке на крайних положениях
        if (YesBit(ctx.MBusy->RCS,cMSBlockRegs)
            &&((gdp.Hot_Hand_Kontur->Position>0)||(ctx.MBusy->TimeSetMech>0))
            &&((gdp.Hot_Hand_Kontur->Position<100)||(ctx.MBusy->TimeSetMech<gdp.ConstMechanic_Mech->v_TimeMixVal)))
        {
            ClrBit(ctx.MBusy->RCS,cMSBlockRegs);
            ctx.MBusy->TimeRealMech=ctx.MBusy->TimeSetMech;
            byte_y++;
        }
//Расчет

        if ((!ctx.MBusy->PauseMech)||(YesBit(gdp.Hot_Hand_Kontur->RCS,cbManMech)))
        {
            long long_x =gdp.Hot_Hand_Kontur->Position;
            long_x *= gdp.ConstMechanic_Mech->v_TimeMixVal;
            long_x /= 100;
            ctx.MBusy->TimeSetMech=(int)(long_x);
/*			if (YesBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech))
            {
                MBusy->TimeRealMech=MBusy->TimeSetMech;
                ClrBit(pGD_Hot_Hand_Kontur->RCS,cbResetMech);
                ByteY++;
            }*/
            if (!YesBit(gdp.Hot_Hand_Kontur->RCS,cbManMech))
            {
                if (!gdp.Hot_Hand_Kontur->Position)
                {
                    SetBit(ctx.MBusy->RCS,cMSBlockRegs);
                    ctx.MBusy->TimeRealMech+=gdp.ConstMechanic_Mech->v_TimeMixVal/4;
                }
                if (gdp.Hot_Hand_Kontur->Position==100)
                {
                    SetBit(ctx.MBusy->RCS,cMSBlockRegs);
                    ctx.MBusy->TimeRealMech-=gdp.ConstMechanic_Mech->v_TimeMixVal/4;
                }
            }
        }
        if (ctx.MBusy->TimeSetMech>ctx.MBusy->TimeRealMech)
        {
            ctx.MBusy->TimeRealMech++;
            __SetBitOutReg(fnTepl,i,0,1);
            SetBit(ctx.MBusy->RCS,cMSBusyMech);
            //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
            byte_y++;
        }
        if (ctx.MBusy->TimeSetMech<ctx.MBusy->TimeRealMech)
        {
            ctx.MBusy->TimeRealMech--;
            __SetBitOutReg(fnTepl,i,0,0);
            SetBit(ctx.MBusy->RCS,cMSBusyMech);
            //SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);
            byte_y++;
        }
        if (byte_y)
        {
            IntY=(int)gdp.ConstMechanic_Mech->v_MinTim;
/*			if ((ByteX==cHSmWinN)||(ByteX==cHSmWinS))
            {
                ogrMin(&IntY,90);
                pGD_TControl_Tepl->FramUpdate[ByteX-cHSmWinN]=0;
            }*/
            IntY = clamp_min(IntY, 5);
            ctx.MBusy->PauseMech=IntY;
            if (YesBit(ctx.MBusy->RCS,cMSBlockRegs))
                ctx.MBusy->PauseMech=150;
        }
        //	if (GD.Hot.Hand) continue;
        if (ctx.MBusy->PauseMech)
        {
            ctx.MBusy->PauseMech--;
//			if (!(YesBit(MBusy->RCS,cMSBlockRegs)))
//				SetBit(pGD_Hot_Hand_Kontur->RCS,cbBusyMech);

        }
        ctx.MBusy->PauseMech = clamp_min(ctx.MBusy->PauseMech, 0);// MBusy->PauseMech=0;

    }
}

void SetMeteo(void)
{
    uint16_t tMes,i;
    for (i=0;i<cConfSMetSens;i++)
    {
        tMes=GD.Hot.MeteoSensing[i].Value;
        if (((tMes<=GD.TControl.MeteoSensing[i]+NameSensConfig[cConfSSens+i].DigitMidl)&&(tMes>=GD.TControl.MeteoSensing[i]-NameSensConfig[cConfSSens+i].DigitMidl))||(GD.TControl.TimeMeteoSensing[i]>20))
        {
            GD.TControl.TimeMeteoSensing[i]=0;
            GD.TControl.MeteoSensing[i]=tMes;
        }
        else if (GD.TControl.TimeMeteoSensing[i]<30) GD.TControl.TimeMeteoSensing[i]++;
    }
    if (GD.TControl.Tepl[0].SnowTime>=GD.TuneClimate.MinRainTime)
        GD.TControl.bSnow=!GD.TControl.bSnow;
    if (((GD.TControl.MeteoSensing[cSmRainSens]<cMinRain)&&(GD.TControl.bSnow))
        ||((GD.TControl.MeteoSensing[cSmRainSens]>cMinRain)&&(!GD.TControl.bSnow)))
    {
        GD.TControl.Tepl[0].SnowTime++;
//		GD.TControl.Tepl[0].SnowTime=10;
    }
    else
        GD.TControl.Tepl[0].SnowTime=0;

    if ((GD.TControl.MeteoSensing[cSmOutTSens]<c_SnowIfOut)&&(GD.TControl.bSnow))
        SetBit(GD.TControl.bSnow,0x02);
    GD.TControl.Tepl[0].SumSens+=GD.TControl.MeteoSensing[cSmFARSens];//GD.Hot.MeteoSens[cSmFARSens].Value;
    GD.TControl.Tepl[0].TimeSumSens++;
    if (GD.TControl.Tepl[0].TimeSumSens>=15)
    {
        GD.TControl.Tepl[0].SensHourAgo=GD.TControl.Tepl[0].SensHalfHourAgo;
        GD.TControl.Tepl[0].SensHalfHourAgo=GD.TControl.Tepl[0].SumSens/GD.TControl.Tepl[0].TimeSumSens;
        GD.TControl.Tepl[0].SumSens=0;
        GD.TControl.Tepl[0].TimeSumSens=0;
    }
}

void SetCO2(void)
{
//	if (!(pGD_MechConfig->RNum[cHSmCO2])) return;  // if hand mode exit
//	if (pGD_Control_Tepl->co_model == 2)
//		{
//	if (pGD_Hot_Tepl->AllTask.DoCO2 > pGD_Hot_Tepl->InTeplSens[cSmCOSens].Value)
//	{
//		(*(pGD_Hot_Hand+cHSmCO2)).Position=1;
//		pGD_TControl_Tepl->COPosition=1;
//	}
//		}

/*	pGD_TControl_Tepl->COPosition=0;
    (*(pGD_Hot_Hand+fHSmReg)).Position=0;
    pGD_Hot_Tepl->AllTask.DoCO2
    pGD_Hot_Tepl->InTeplSens[cSmCOSens].Value;
    co2On
    co2Fram1
    co2Fram2
    co2Off


    char bZad;
    if (!(pGD_MechConfig->RNum[cHSmLight])) return;  // if hand mode exit
    IntZ=0;
    pGD_TControl_Tepl->LightPauseMode--;
    if ((pGD_TControl_Tepl->LightPauseMode<0)||(pGD_TControl_Tepl->LightPauseMode>GD.TuneClimate.l_PauseMode))
        pGD_TControl_Tepl->LightPauseMode=0;
    ClrDog;
    bZad=0;		// if bZab = 0 calc sun sensor
    if (pGD_TControl_Tepl->LightPauseMode) bZad=1;  // if bZad = 1 don't calc sun senasor
    pGD_TControl_Tepl->LightMode = pGD_Hot_Tepl->AllTask.ModeLight * pGD_Hot_Tepl->AllTask.Light;
    if (!bZad)
    {
        if (GD.Hot.Zax-60>GD.Hot.Time)
            pGD_TControl_Tepl->LightMode=0;
        if (GD.TControl.Tepl[0].SensHalfHourAgo>GD.TuneClimate.l_SunOn50)  // sun > 50% then off light
            pGD_TControl_Tepl->LightMode=0;

        if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn50)
        {
            IntY=GD.Hot.MidlSR;
            CorrectionRule(GD.TuneClimate.l_SunOn100,GD.TuneClimate.l_SunOn50,50,0);
            pGD_TControl_Tepl->LightMode=100-IntZ;
        }
    }
    if (pGD_TControl_Tepl->LightMode!=pGD_TControl_Tepl->OldLightMode)
    {
        if (!(((int)pGD_TControl_Tepl->LightMode)*((int)pGD_TControl_Tepl->OldLightMode)))
        {
            pGD_TControl_Tepl->DifLightMode=pGD_TControl_Tepl->LightMode-pGD_TControl_Tepl->OldLightMode;
            pGD_TControl_Tepl->LightPauseMode=GD.TuneClimate.l_PauseMode;
//			pGD_TControl_Tepl->LightExtraPause=o_DeltaTime;
        }
        else
        {
            pGD_TControl_Tepl->LightPauseMode=GD.TuneClimate.l_SoftPauseMode;
        }
    }
    pGD_TControl_Tepl->OldLightMode=pGD_TControl_Tepl->LightMode;
    // new
    if (pGD_Hot_Tepl->AllTask.ModeLight == 2)    		// авто досветка
    {
        if (pGD_Hot_Tepl->AllTask.Light < pGD_TControl_Tepl->LightMode)
            pGD_TControl_Tepl->LightValue = pGD_Hot_Tepl->AllTask.Light;
        else
            pGD_TControl_Tepl->LightValue=pGD_TControl_Tepl->LightMode;
    }
    else
        pGD_TControl_Tepl->LightValue=pGD_TControl_Tepl->LightMode;
    // new

    if (pGD_TControl_Tepl->LightValue > 100)
        pGD_TControl_Tepl->LightValue = 100;    */
}


#warning light Досветка !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void SetLighting(void)
{
    char bZad;
    if (!(gdp.MechConfig->RNum[cHSmLight])) return;  // if hand mode exit
    IntZ=0;

//	if(SameSign(IntY,IntZ)) pGD_TControl_Tepl->LightExtraPause=0;

    gdp.TControl_Tepl->LightPauseMode--;
    if ((gdp.TControl_Tepl->LightPauseMode<0)||(gdp.TControl_Tepl->LightPauseMode>GD.TuneClimate.l_PauseMode))
        gdp.TControl_Tepl->LightPauseMode=0;
    bZad=0;     // if bZab = 0 calc sun sensor
    if (gdp.TControl_Tepl->LightPauseMode) bZad=1;  // if bZad = 1 don't calc sun senasor

// old
//	if ((pGD_Hot_Tepl->AllTask.ModeLight<2))//&&(!bZad))	// если режим досветки не авто
//	{
//		pGD_TControl_Tepl->LightMode=pGD_Hot_Tepl->AllTask.ModeLight*pGD_Hot_Tepl->AllTask.Light;
//		bZad=1;
//	}

    if (gdp.Hot_Tepl->AllTask.ModeLight<2)
    {
        gdp.TControl_Tepl->LightMode = gdp.Hot_Tepl->AllTask.ModeLight * gdp.Hot_Tepl->AllTask.Light;
        bZad=1;
    }

    if (!bZad)
    {
        if (GD.Hot.Zax-60>GD.Hot.Time)
            gdp.TControl_Tepl->LightMode=0;
        if (GD.TControl.Tepl[0].SensHalfHourAgo>GD.TuneClimate.l_SunOn50)  // sun > 50% then off light
            gdp.TControl_Tepl->LightMode=0;

        if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn50)
        {
//			pGD_TControl_Tepl->LightMode=50;
            IntY=GD.Hot.MidlSR;
            CorrectionRule(GD.TuneClimate.l_SunOn100,GD.TuneClimate.l_SunOn50,50,0);
            gdp.TControl_Tepl->LightMode=100-IntZ;
        }

//		if (GD.TControl.Tepl[0].SensHalfHourAgo<GD.TuneClimate.l_SunOn100)
//			pGD_TControl_Tepl->LightMode=100;
    }
    if (gdp.TControl_Tepl->LightMode!=gdp.TControl_Tepl->OldLightMode)
    {
        if (!(((int)gdp.TControl_Tepl->LightMode)*((int)gdp.TControl_Tepl->OldLightMode)))
        {
            gdp.TControl_Tepl->DifLightMode=gdp.TControl_Tepl->LightMode-gdp.TControl_Tepl->OldLightMode;
            gdp.TControl_Tepl->LightPauseMode=GD.TuneClimate.l_PauseMode;
//			pGD_TControl_Tepl->LightExtraPause=o_DeltaTime;
        }
        else
        {
            gdp.TControl_Tepl->LightPauseMode=GD.TuneClimate.l_SoftPauseMode;
        }
    }
    gdp.TControl_Tepl->OldLightMode=gdp.TControl_Tepl->LightMode;

//	pGD_TControl_Tepl->LightExtraPause--;
//	if (pGD_TControl_Tepl->LightExtraPause>0) return;
//	pGD_TControl_Tepl->LightExtraPause=0;

    // new
    if (gdp.Hot_Tepl->AllTask.ModeLight == 2)           // авто досветка
    {
        if (gdp.Hot_Tepl->AllTask.Light < gdp.TControl_Tepl->LightMode)
            gdp.TControl_Tepl->LightValue = gdp.Hot_Tepl->AllTask.Light;
        else
            gdp.TControl_Tepl->LightValue=gdp.TControl_Tepl->LightMode;
    }
    else
        gdp.TControl_Tepl->LightValue=gdp.TControl_Tepl->LightMode;
    // new

    if (gdp.TControl_Tepl->LightValue > 100)
        gdp.TControl_Tepl->LightValue = 100;

    //old
    //pGD_TControl_Tepl->LightValue=pGD_TControl_Tepl->LightMode;		// значение досветки

}

void SetTepl(char fnTepl)
{
/***********************************************************************
--------------Вычисление изменения показаний датчика температуры-------
************************************************************************/

/***********************************************************************/
    if (!gdp.Hot_Tepl->AllTask.NextTAir)
        SetBit(gdp.Hot_Tepl->RCS,cbNoTaskForTepl);

//	if(!pGD_Hot_Tepl->InTeplSens[cSmTSens].Value)
//		SetBit(pGD_Hot_Tepl->RCS,cbNoSensingTemp);
// NEW
    if (!gdp.Hot_Tepl->InTeplSens[cSmTSens1].Value)
        SetBit(gdp.Hot_Tepl->RCS,cbNoSensingTemp);
    if (!gdp.Hot_Tepl->InTeplSens[cSmTSens2].Value)
        SetBit(gdp.Hot_Tepl->RCS,cbNoSensingTemp);
    if (!gdp.Hot_Tepl->InTeplSens[cSmTSens3].Value)
        SetBit(gdp.Hot_Tepl->RCS,cbNoSensingTemp);
    if (!gdp.Hot_Tepl->InTeplSens[cSmTSens4].Value)
        SetBit(gdp.Hot_Tepl->RCS,cbNoSensingTemp);

//	if(!pGD_Hot_Tepl->RCS)
    {
        AllTaskAndCorrection();
        LaunchCalorifer(fnTepl);

        __cNextTCalc(fnTepl);

        DecPumpPause();

        //SetUpSiod(fnTepl);


        InitScreen(cTermHorzScr,fnTepl);
        InitScreen(cSunHorzScr,fnTepl);
        InitScreen(cTermVertScr1,fnTepl);
        InitScreen(cTermVertScr2,fnTepl);
        InitScreen(cTermVertScr3,fnTepl);
        InitScreen(cTermVertScr4,fnTepl);
        SetReg(cHSmCO2,
               gdp.Hot_Tepl->AllTask.DoCO2,gdp.Hot_Tepl->InTeplSens[cSmCOSens].Value);

        gdp.Hot_Tepl->OtherCalc.MeasDifPress=GD.TControl.MeteoSensing[cSmPresureSens]-GD.TControl.MeteoSensing[cSmPresureSens+1];
        if (!gdp.Hot_Tepl->OtherCalc.MeasDifPress) gdp.Hot_Tepl->OtherCalc.MeasDifPress=1;
        if ((!GD.TControl.MeteoSensing[cSmPresureSens])||(!GD.TControl.MeteoSensing[cSmPresureSens+1]))
            gdp.Hot_Tepl->OtherCalc.MeasDifPress=0;
        SetReg(cHSmPressReg,
               gdp.Hot_Tepl->AllTask.DoPressure,gdp.Hot_Tepl->OtherCalc.MeasDifPress);
        LaunchVent(fnTepl);
        SetLighting();
        SetCO2();               // CO2
    }
}
//Есть место для оптимизации!!!!!!!!!!!!!!!!!!!!
void SubConfig(char fnTepl)
{
    SetPointersOnTepl(fnTepl);
    for (int i=0;i<cHSmPump;i++)
    {
        SetPointersOnKontur(i);
        if (i<cSKontur)
        {
            gdp.TControl_Tepl_Kontur->Separate=CheckSeparate(i);
            gdp.TControl_Tepl_Kontur->MainTepl=CheckMain(fnTepl);

            gdp.Hot_Hand_Kontur->RCS=
            GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].HandCtrl[i].RCS;
            gdp.Hot_Hand_Kontur->Position=
            GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].HandCtrl[i].Position;
            if (i<cSWaterKontur)
            {
                int byte_y=i+cHSmPump;
                gdp.TControl_Tepl_Kontur->SensValue=gdp.Hot_Tepl->InTeplSens[i+cSmWaterSens].Value;
                gdp.Hot_Hand[byte_y].RCS=
                GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].HandCtrl[byte_y].RCS;
                gdp.Hot_Hand[byte_y].Position=
                GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].HandCtrl[byte_y].Position;
                gdp.TControl_Tepl_Kontur->SensValue=
                GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[i].SensValue;
            }
        }

    }

}
void Configuration(void)
{
    char tTepl;
    for (tTepl=0;tTepl<cSTepl;tTepl++)
        SubConfig(tTepl);
}
#ifdef Vitebsk
void    TransferWaterToBoil(void)
{
    IntX=GD.Hot.MaxReqWater/100; //Делаем запас на 5 градусов
//	IntX=IntX/100;
    IntX++;
//	IntY=0;
    switch (IntX)
    {
    case 0:
    case 1:
    case 2:
    case 3: IntY=1; break;
    case 4:
    case 5:
    case 6: IntY=IntX-2; break;
    case 7: IntY=0; break;
    case 8:
    case 9:
    case 10: IntY=IntX-3; break;
    default: IntY=7;
    }

    if (YesBit(IntY,0x01))
        __SetBitOutReg(0,cHSmRegs+4,0,0);
    if (YesBit(IntY,0x02))
        __SetBitOutReg(0,cHSmRegs+4,0,1);
    if (YesBit(IntY,0x04))
        __SetBitOutReg(0,cHSmRegs+4,0,2);
//	GD.Hot.Demon=IntY;

}
#endif
/*------------------------------------------------
        Управлюща программа
        Вариант "Ромашка"
        Создана от 14.04.04
--------------------------------------------------*/

int volatile settingsArray[8];

void saveSettings(char tCTepl)
{
    settingsArray[0] = GD.Hot.Tepl[tCTepl].tempParamHeat;
    settingsArray[1] = GD.Hot.Tepl[tCTepl].tempParamVent;
    settingsArray[2] = GD.Hot.Tepl[tCTepl].tempHeat;
    settingsArray[3] = GD.Hot.Tepl[tCTepl].tempVent;
    settingsArray[4] = GD.Hot.Tepl[tCTepl].newsZone;
}

void loadSettings(char tCTepl)
{
    GD.Hot.Tepl[tCTepl].tempParamHeat = settingsArray[0];
    GD.Hot.Tepl[tCTepl].tempParamVent = settingsArray[1];
    GD.Hot.Tepl[tCTepl].tempHeat = settingsArray[2];
    GD.Hot.Tepl[tCTepl].tempVent = settingsArray[3];
    GD.Hot.Tepl[tCTepl].newsZone = settingsArray[4];
}

void Control(void)
{
    char tCTepl,ttTepl;
    Configuration();
    SetDiskrSens();

    if (ctx.DemoMode!=9)
        ctx.DemoMode=0;
    if (!ctx.DemoMode)
    {
        ClrAllOutIPCDigit();
        SetAlarm();

        for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
        {
            SetPointersOnTepl(tCTepl);
            SetSensOnMech();
            DoMechanics(tCTepl);
            SetDiskr(tCTepl);

            SetUpSiod(tCTepl);  // !!!


            DoSiod(tCTepl);
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
        ResumeOutIPCDigit();

    }
/*--------------------------------------------------------
если минута */

    for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
    {
        SetPointersOnTepl(tCTepl);
        SetMixValvePosition();
    }
    if (WTF0.Second==20)
    {
        InitLCD();
        SetMeteo();
    }
    if ((WTF0.Second==40)||(GD.TControl.Delay))
    {
        if (GD.SostRS==WORK_UNIT)
        {
            GD.TControl.Delay=1;
        }
        else
        {
            WTF0.PORTNUM=0;
            ctx.vNFCtr=0;
            CheckMidlSr();
            GD.TControl.Delay=0;
            for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
            {
                saveSettings(tCTepl);
                memclr(&GD.Hot.Tepl[tCTepl].ExtRCS,(
                                                   sizeof(char)*2+sizeof(eClimTask)+sizeof(eOtherCalc)+
                                                   sizeof(eNextTCalc)+sizeof(eKontur)*cSKontur+20));
                IntZ=((GD.Hot.Time+o_DeltaTime)%(24*60));
                TaskTimer(1,tCTepl,tCTepl);
                ttTepl=tCTepl;
                while ((!GD.Hot.Tepl[tCTepl].AllTask.NextTAir)&&(ttTepl))
                {
                    TaskTimer(1,--ttTepl,tCTepl);
                }
                IntZ=GD.Hot.Time;
                loadSettings(tCTepl);
                TaskTimer(0,ttTepl,tCTepl);
                SetPointersOnTepl(tCTepl);
                SetTepl(tCTepl);

                airHeat(tCTepl);    // airHeat
            }
            __sCalcKonturs();
            __sMechWindows();
            __sMechScreen();
            for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
            {
                if (GD.Hot.MaxReqWater<GD.Hot.Tepl[tCTepl].MaxReqWater)
                    GD.Hot.MaxReqWater=GD.Hot.Tepl[tCTepl].MaxReqWater;
                bWaterReset[tCTepl]=0;
            }
        }
    }
    ctx.vNFCtr=GD.Control.NFCtr;
    WTF0.PORTNUM=DEF_PORTNUM;
    ctx.MaskRas=bRasxod;
    if (ctx.TecPerRas > 2305)
    {
        ctx.TecPerRas=2305;
        GD.TControl.NowRasx=0;
    }
    else
    {
        IntX=ctx.PastPerRas;
        if (ctx.TecPerRas>IntX) IntX=ctx.TecPerRas;
        GD.TControl.NowRasx=(long)GD.TuneClimate.ScaleRasx*(long)23040/(long)IntX/100;
    }
    GD.TControl.FullVol+=ctx.Volume;
    if ((!GD.TControl.MeteoSensing[cSmMainTSens])||(!GD.TControl.MeteoSensing[cSmMainTSens+1]))
    {
        GD.Hot.HeatPower=(int)(((long)GD.TControl.NowRasx)*(GD.TControl.MeteoSensing[cSmMainTSens]-GD.TControl.MeteoSensing[cSmMainTSens+1])/100);
        GD.Hot.FullHeat=(int)((GD.TControl.FullVol*GD.TuneClimate.ScaleRasx/100)*(GD.TControl.MeteoSensing[cSmMainTSens]-GD.TControl.MeteoSensing[cSmMainTSens+1])/1000);
    }
    ctx.Volume=0;
    if (WTF0.Second < 60) return;

    airHeatTimers();    // airHeat

    WriteToFRAM();
    MidlWindAndSr();
    WindDirect();

#ifndef NOTESTMEM
    if ((!WTF0.Menu)&&(GD.SostRS==OUT_UNIT))
        TestMem(1);
#endif

    WTF0.Second=0;
    if (ctx.TimeReset)
        ctx.TimeReset--;
    if (ctx.TimeReset<0)
        ctx.TimeReset=1;
    GD.Hot.Time++;
    GetRTC();
    #warning "disabled this beepy"
//  not=220;ton=10;

    if (GD.Hot.Vosx != 0)
        ctx.settingsVosx = GD.Hot.Vosx;
    if (GD.Hot.Zax != 0)
        ctx.settingsZax = GD.Hot.Zax;

    bNight=1;
    if ((GD.Hot.Time>=GD.Hot.Vosx)&&(GD.Hot.Time<GD.Hot.Zax))
        bNight=0;

    if (GD.Hot.Time>=24*60)      /*новые сутки*/
    {
        GD.Hot.Time=0;
        GD.Hot.Data++;
        int byte_x=GD.Hot.Data/256;  /* это месц */
        if (byte_x<=0)
        {
            byte_x=1;GD.Hot.Data=1;
        }
        if ((GD.Hot.Data%256)>Mon[byte_x-1])
        {
            if ((++byte_x)>12)
            {
                GD.Hot.Year++;
                byte_x=1;
            }
            GD.Hot.Data=(int)byte_x*256+1;
        }
    }
    if (GD.TControl.Data!=GD.Hot.Data)      /*новые сутки*/
    {
        for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
        {
//			GD.TControl.Tepl[tCTepl].Functional=1;
            GD.TControl.Tepl[tCTepl].TimeSIO=0;
        }
        GD.TControl.SumSun=0;
        GD.TControl.Data=GD.Hot.Data;
        GD.TControl.FullVol=0;
    }
}
