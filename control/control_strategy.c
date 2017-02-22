#include "syntax.h"

#include "control_gd.h"
#include "control.h"
#include "control_subr.h"
#include "control_screen.h"


/**************************************************************************/
/*-*-*-*-*-*-*--Процедура установки границ для водных контуров--*-*-*-*-*-*/
/**************************************************************************/
int DefRH(void)
{
    if ((!gdp.Hot_Tepl->AllTask.DoRHAir)|| (!gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value)) return 0;
    return(gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value-gdp.Hot_Tepl->AllTask.DoRHAir);

}

void SetNoWorkKontur(void)
{
    SetBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur);
    gdp.TControl_Tepl_Kontur->PumpStatus = 0;
    gdp.TControl_Tepl_Kontur->DoT = 0;
}

void __sMinMaxWater(char fnKontur)
{
//------------------------------------------------------------------------
//Заполняем минимальные и максимальные границы для контуров а также опт температуру
//------------------------------------------------------------------------
    gdp.Hot_Tepl_Kontur->MaxCalc = gdp.Control_Tepl->c_MaxTPipe[fnKontur];
//------------------------------------------------------------------------
//Для контуров начиная с третьего берем минимальные границы и опт. темп. из параметров
//------------------------------------------------------------------------

//	if  ((GD.Hot.MidlSR<GD.TuneClimate.f_MinSun)&&(pGD_Hot_Tepl->AllTask.NextTAir-GD.TControl.OutTemp>GD.TuneClimate.f_DeltaOut))
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].ExtRCS,cbBlockPumpKontur);

    if (fnKontur  ==  cSmKontur4)
    {
        gdp.Hot_Tepl_Kontur->MinTask = gdp.Control_Tepl->c_MinTPipe[1];
        gdp.Hot_Tepl_Kontur->Optimal = gdp.Control_Tepl->c_OptimalTPipe[1];
        gdp.Hot_Tepl_Kontur->MinCalc = gdp.Hot_Tepl_Kontur->MinTask;
    }
//------------------------------------------------------------------------
//Если установлен минимум то насос должен быть всегда включен
//------------------------------------------------------------------------
    if ((gdp.Hot_Tepl_Kontur->MinTask)
        || (YesBit((*(gdp.Hot_Hand_Kontur + cHSmPump)).RCS, cbManMech)))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
//------------------------------------------------------------------------
//Ограничиваем минимум
//------------------------------------------------------------------------
    gdp.Hot_Tepl_Kontur->MinCalc = clamp_min(gdp.Hot_Tepl_Kontur->MinCalc, cMinAllKontur);
//------------------------------------------------------------------------
//В виду особенностей работы 5 контура для него отдельные установки
//------------------------------------------------------------------------
    if (gdp.Hot_Tepl_Kontur->MinCalc > gdp.Hot_Tepl_Kontur->MaxCalc)
    {
        gdp.Hot_Tepl_Kontur->MinCalc = gdp.Hot_Tepl_Kontur->MaxCalc;
    }
    if (fnKontur  ==  cSmKontur5)
    {

        if ((!(YesBit(gdp.Hot_Tepl->InTeplSens[cSmGlassSens].RCS, cbMinMaxVSens)))
            && ((gdp.Hot_Tepl->InTeplSens[cSmGlassSens].Value
                 < GD.TuneClimate.c_DoMinIfGlass)))
        {
            creg.Y = gdp.Hot_Tepl->InTeplSens[cSmGlassSens].Value;
            CorrectionRule(GD.TuneClimate.c_DoMaxIfGlass,
                           GD.TuneClimate.c_DoMinIfGlass,
                           gdp.Hot_Tepl_Kontur->MaxCalc - cMin5Kontur, 0);
            gdp.Hot_Tepl_Kontur->Do = gdp.Hot_Tepl_Kontur->MaxCalc - creg.Z;
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);

        }
        if (YesBit(GD.TControl.bSnow, 0x02))
        {
            creg.Y = GD.TControl.MeteoSensing[cSmOutTSens];
            CorrectionRule(GD.TuneClimate.c_CriticalSnowOut, c_SnowIfOut,
                           gdp.Hot_Tepl_Kontur->MaxCalc - GD.TuneClimate.c_MinIfSnow,
                           0);
            gdp.Hot_Tepl_Kontur->Do = gdp.Hot_Tepl_Kontur->MaxCalc - creg.Z;
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
        }

        if ((gdp.TControl_Tepl->ScrExtraHeat) || (GD.Hot.Util  ==  9))
        {
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
            //pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->MaxCalc*10);
            gdp.Hot_Tepl_Kontur->Do = gdp.Hot_Tepl_Kontur->MaxCalc;
        }
//		else
//		if  (pGD_TControl_Tepl->LastScrExtraHeat)
//			pGD_TControl_Tepl_Kontur->DoT=pGD_TControl_Tepl->TempStart5;

//		pGD_TControl_Tepl->LastScrExtraHeat=pGD_TControl_Tepl->ScrExtraHeat;

//ogrMin(&(pGD_Hot_Tepl_Kontur->MinCalc),cMin5Kontur);
        gdp.Hot_Tepl_Kontur->Do = clamp_min(gdp.Hot_Tepl_Kontur->Do, gdp.Hot_Tepl_Kontur->MinCalc);
        if (!YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur))
        {
            gdp.Hot_Tepl_Kontur->Do = 0;
            gdp.Hot_Tepl_Kontur->MaxCalc = 0;
        }

    }
//	else
//	{
//		if (!pGD_Hot_Tepl_Kontur->Optimal)
//			pGD_Hot_Tepl_Kontur->Optimal=pGD_Hot_Tepl_Kontur->MinCalc+(pGD_Hot_Tepl_Kontur->MaxCalc-pGD_Hot_Tepl_Kontur->MinCalc)/2;
//	}

}

/*************************************************************************/
/*-*-*-*-*-*-*-*--Процедура установки границ для экрана--*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*void __sMinMaxScreen(void)
 {
 if (!pGD_TControl_Tepl->Kontur[cSmScreen].Separate)
 {
 SetBit(pGD_Hot_Tepl->Kontur[cSmScreen].RCS,cbNoWorkKontur);
 return;
 }
 if (!pGD_TControl_Tepl->Screen[0].Mode)
 {
 pGD_TControl_Tepl->Kontur[cSmScreen].DoT=0;
 return;
 }
 if (YesBit(pGD_TControl_Tepl->RCS1,cbSCCorrection))
 pGD_Hot_Tepl->Kontur[cSmScreen].MaxCalc=GD.TuneClimate.sc_TFactor;
 IntY=GD.TControl.OutTemp;
 CorrectionRule(GD.TuneClimate.s_StartCorrPow,GD.TuneClimate.s_EndCorrPow,
 (GD.TuneClimate.s_PowFactor-1000),0);
 pGD_TControl_Tepl->s_Power=GD.TuneClimate.s_PowFactor-IntZ;
 if (GD.TuneClimate.s_PowFactor<1000) pGD_TControl_Tepl->s_Power=1000;

 } */

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*-*-*-*--Процедура установки границ для фрамуг--*-*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sMinMaxWindows(void)
{
    int t_max;
//-------------------------------------------------------
//Проверяем работу по заданию
//------------------------------------------------------

    if ((gdp.Hot_Tepl->AllTask.Win < 2)
        || (YesBit(gdp.TControl_Tepl->Calorifer, 0x02))) //Добавлено для блокировки фрамуг кондиционером
    {
//--------------------------------------------------------------------------------
//Если установлен режим работы по минимому то минимум должен совпасть с максимумом
//--------------------------------------------------------------------------------
        if (gdp.Hot_Tepl->AllTask.Win)
        {
            gdp.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc =
            gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc;
            gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSFollowProg;
            gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSFollowProg;
        }
        else
        {
            memclr(&gdp.Hot_Tepl->Kontur[cSmWindowUnW].Optimal,
                   sizeof(eKontur));
        }
//--------------------------------------------------------------------------------
//Стираем все накопленные данные
//--------------------------------------------------------------------------------
        gdp.TControl_Tepl->Kontur[cSmWindowOnW].DoT = 0;
        gdp.TControl_Tepl->Kontur[cSmWindowUnW].DoT = 0;
        return;
    }
    gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSOn;
    gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSOn;

    creg.Y = DefRH();
    CorrectionRule(GD.TuneClimate.f_min_RHStart, GD.TuneClimate.f_min_RHEnd,
                   ((int) GD.TuneClimate.f_min_Cor), 0);
    gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc = clamp_min(gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc, creg.Z);

    creg.Y = GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(GD.TuneClimate.f_StartCorrPow, GD.TuneClimate.f_EndCorrPow,
                   (GD.TuneClimate.f_PowFactor - 1000), 0);
    gdp.TControl_Tepl->f_Power = GD.TuneClimate.f_PowFactor - creg.Z;
    if (GD.TuneClimate.f_PowFactor < 1000)
        gdp.TControl_Tepl->f_Power = 1000;

    t_max = gdp.Control_Tepl->f_MaxOpenUn;
    if ((t_max > GD.TuneClimate.f_MaxOpenRain) && (GD.TControl.bSnow))
    {
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSWRain;
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSWRain;

        t_max = GD.TuneClimate.f_MaxOpenRain;
    }
    //--------------------------------------------------------------------------------
//Максимум рассчитываем по внешней температуре
//--------------------------------------------------------------------------------
    creg.Y = GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(GD.TuneClimate.f_MinTFreeze,
                   GD.TuneClimate.f_MinTFreeze + f_MaxTFreeze, 200, 0);
    creg.X = creg.Z;
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась подветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
//	if (YesBit(GD.Hot.MeteoSens[cSmOutTSens].RCS,cbMinMaxVSens))
//		IntX=100;
//--------------------------------------------------------------------------------
//И по ветру
//--------------------------------------------------------------------------------
    creg.Y = GD.Hot.MidlWind;
    CorrectionRule(GD.TuneClimate.f_StormWind - f_StartWind,
                   GD.TuneClimate.f_StormWind, 100, 0);
//В IntZ - ограничение по ветру

    if (creg.Z > gdp.TControl_Tepl->OldPozUn)
    {
        gdp.TControl_Tepl->UnWindStorm = GD.TuneClimate.f_WindHold;
        gdp.TControl_Tepl->OldPozUn = creg.Z;
    }
    if (gdp.TControl_Tepl->UnWindStorm > 0)
    {
        gdp.TControl_Tepl->UnWindStorm--;
        creg.Z = gdp.TControl_Tepl->OldPozUn;
    }

//	if (!(GD.TuneClimate.f_MaxWind)) IntZ=100;
    if (creg.X > 100)
    {
        creg.Y = 100 - creg.Z;
        creg.X -= 100;
    }
    else
    {
        creg.Y = creg.X - creg.Z;
        creg.X = 0;
    }
//	IntY=t_max-IntX-IntZ;

    creg.Y = clamp_min(creg.Y, 0);
    creg.Y = clamp_max(creg.Y, t_max);

    gdp.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc = creg.Y; //pGD_TControl_Tepl->PrevMaxWinUnW;
    /*--------------------------------------------------------------------------------
     //Проверяем было ли отличие от предыдущего максимума хотя бы на один шаг
     //--------------------------------------------------------------------------------
     if ((IntY>=pGD_TControl_Tepl->PrevMaxWinUnW+ByteW)
     ||(IntY<=pGD_TControl_Tepl->PrevMaxWinUnW-ByteW))
     {
     pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc=(IntY/ByteW)*ByteW;
     pGD_TControl_Tepl->PrevMaxWinUnW=pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc;
     }
     if ((IntY == 0)||(IntY == 100))
     {
     pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc=IntY;
     pGD_TControl_Tepl->PrevMaxWinUnW=IntY;
     }*/
//--------------------------------------------------------------------------------
//Максимум для наветренной стороны
//--------------------------------------------------------------------------------
    t_max = gdp.Control_Tepl->f_MaxOpenOn;

    creg.Y = -DefRH(); //pGD_Hot_Tepl->AllTask.NextRHAir-pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value;
    CorrectionRule(GD.TuneClimate.f_max_RHStart, GD.TuneClimate.f_max_RHEnd,
                   ((int) GD.TuneClimate.f_max_Cor), 0);
    t_max = clamp_max(t_max, 100 - creg.Z);

    if ((t_max > GD.TuneClimate.f_MaxOpenRain) && (GD.TControl.bSnow))
        t_max = GD.TuneClimate.f_MaxOpenRain;

    creg.Y = GD.Hot.MidlWind;
    CorrectionRule(GD.TuneClimate.f_StormWindOn - f_StartWind,
                   GD.TuneClimate.f_StormWindOn, 100, 0);

    if (creg.Z < gdp.TControl_Tepl->OldPozOn)
    {
        gdp.TControl_Tepl->OnWindStorm = GD.TuneClimate.f_WindHold;
        gdp.TControl_Tepl->OldPozOn = creg.Z;
    }
    if (gdp.TControl_Tepl->OnWindStorm > 0)
    {
        gdp.TControl_Tepl->OnWindStorm--;
        creg.Z = gdp.TControl_Tepl->OldPozOn;
    }
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась наветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
    creg.Y = creg.X - creg.Z;
    creg.Y = clamp_min(creg.Y, 0); //if (IntY<0) IntY=0;
    creg.Y = clamp_max(creg.Y, t_max); //if (IntY<0) IntY=0;

    gdp.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc = creg.Y; //pGD_TControl_Tepl->PrevMaxWinOnW;
    /*    if ((IntY>=pGD_TControl_Tepl->PrevMaxWinOnW+ByteW)
     ||(IntY<=pGD_TControl_Tepl->PrevMaxWinOnW-ByteW))
     {
     pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc=(IntY/ByteW)*ByteW;
     pGD_TControl_Tepl->PrevMaxWinOnW=pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc;
     }
     if ((IntY == 0)||(IntY == 100))
     {
     pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc=IntY;
     pGD_TControl_Tepl->PrevMaxWinOnW=IntY;
     }
     */
//	if (YesBit((*(pGD_Hot_Hand+cHSmWinN+1-GD.Hot.PozFluger)).RCS,(cbNoMech+cbManMech)))
    if (gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status < 20)
    {
        if (gdp.TControl_Tepl->Kontur[cSmWindowOnW].DoT / 10
             ==  gdp.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc)
            gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSReachMax;

        if (gdp.TControl_Tepl->Kontur[cSmWindowOnW].DoT / 10
             ==  gdp.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc)
            gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSReachMin;
    }
    if (gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status < 20)
    {
        if (gdp.TControl_Tepl->Kontur[cSmWindowUnW].DoT / 10
             ==  gdp.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc)
            gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSReachMax;
        if (gdp.TControl_Tepl->Kontur[cSmWindowUnW].DoT / 10
             ==  gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc)
            gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSReachMin;
    }

    if ((!gdp.TControl_Tepl->Kontur[cHSmWinN + GD.Hot.PozFluger].Separate)
        || (YesBit((*(gdp.Hot_Hand + cHSmWinN + GD.Hot.PozFluger)).RCS,
                   cbManMech)))
    {
        gdp.TControl_Tepl->Kontur[cSmWindowOnW].DoT = 0;
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc = 0;
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc = 0;
        SetBit(gdp.Hot_Tepl->Kontur[cSmWindowOnW].RCS, cbNoWorkKontur);
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSWHand;
    }
    if ((!gdp.TControl_Tepl->Kontur[cHSmWinN + 1 - GD.Hot.PozFluger].Separate)
        || (YesBit((*(gdp.Hot_Hand + cHSmWinN + 1 - GD.Hot.PozFluger)).RCS,
                   cbManMech)))
    {
        SetBit(gdp.Hot_Tepl->Kontur[cSmWindowUnW].RCS, cbNoWorkKontur);
        gdp.TControl_Tepl->Kontur[cSmWindowUnW].DoT = 0;
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc = 0;
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc = 0;
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSWHand;
    }

    if (YesBit(gdp.Hot_Tepl->discrete_inputs[0], cSmDVent))
    {
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSAlrExternal;
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSAlrExternal;
    }
    if (YesBit(gdp.TControl_Tepl->MechBusy[cHSmWinN + 1 - GD.Hot.PozFluger].RCS,
               cMSAlarm))
        gdp.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSAlrNoCtrl;
    if (YesBit(gdp.TControl_Tepl->MechBusy[cHSmWinN + GD.Hot.PozFluger].RCS,
               cMSAlarm))
        gdp.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSAlrNoCtrl;

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sInitKontur(char fnKontur)
{
//	char tTepl1;
//------------------------------------------------------------------------
//Определяем работает может ли работать контур
//------------------------------------------------------------------------
    gdp.Hot_Tepl_Kontur->Status = 0;
    if (!gdp.TControl_Tepl_Kontur->Separate)
    {
        SetNoWorkKontur();
        return;
    }
//------------------------------------------------------------------------
//Устанавливаем минимум и максимум
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Диагностика работы контура (выявляем ошибки не позволяющиее работать контуру)
//------------------------------------------------------------------------
    int byte_y = 0;
    gdp.Hot_Tepl_Kontur->SensValue = gdp.TControl_Tepl_Kontur->SensValue;
    if (!gdp.Control_Tepl->c_MaxTPipe[fnKontur])
    {
        gdp.Hot_Tepl_Kontur->Status = cSWNoMax;
        byte_y++;
    }
    if (((YesBit((*(gdp.Hot_Hand_Kontur + cHSmPump)).RCS, cbManMech))
         && (!((*(gdp.Hot_Hand_Kontur + cHSmPump)).Position))))
    {
        gdp.Hot_Tepl_Kontur->Status = cSWHand;
        byte_y++;
    }

    if (byte_y)
    {
        SetNoWorkKontur();
        return;
    }
//		{
//			SetNoWorkKontur();
//			return;
//		}
    if (!gdp.Hot_Tepl_Kontur->SensValue)
    {
        gdp.Hot_Tepl_Kontur->Status = cSAlrNoSens;
        SetBit(gdp.Hot_Tepl_Kontur->RCS, cbNoSensKontur);
        SetNoWorkKontur();
        return;
    }
    __sMinMaxWater(fnKontur);

    for (byte_y = 0; byte_y < 8; byte_y++)
        ((char*) (&(gdp.Hot_Tepl_Kontur->Optimal)))[byte_y] =
        ((char*) (&(GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].Optimal)))[byte_y];
    gdp.TControl_Tepl_Kontur->DoT =
    GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].DoT;
    gdp.TControl_Tepl_Kontur->PumpStatus =
    GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].PumpStatus;
    //GD.TControl.Tepl[cSmTeplB].Kontur[fnKontur].SErr=GD.TControl.Tepl[cSmTeplA].Kontur[fnKontur].SErr;
    if (gdp.TControl_Tepl_Kontur->DoT)
        gdp.Hot_Tepl_Kontur->Status = cSOn;

    if (YesBit(
              GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].ExtRCS,
              cbBlockPumpKontur))
    {
        gdp.Hot_Tepl_Kontur->Status = cSBlPump;
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
    }
    if (gdp.TControl_Tepl_Kontur->DoT / 10  ==  gdp.Hot_Tepl_Kontur->MaxCalc)
        gdp.Hot_Tepl_Kontur->Status = cSReachMax;

    if (gdp.TControl_Tepl_Kontur->DoT / 10  ==  gdp.Hot_Tepl_Kontur->MinCalc)
        gdp.Hot_Tepl_Kontur->Status = cSReachMin;

    if (gdp.Hot_Tepl_Kontur->Do)
        gdp.Hot_Tepl_Kontur->Status = cSFollowProg;

    if (gdp.Hot_Tepl_Kontur->MinCalc > gdp.Hot_Tepl_Kontur->MaxCalc - 100)
        gdp.Hot_Tepl_Kontur->Status = cSWNoRange;
    if ((gdp.Hot_Tepl_Kontur->SensValue < gdp.Hot_Tepl_Kontur->MaxCalc - 50)
        && (gdp.Hot_Hand_Kontur->Position  ==  100))
        gdp.Hot_Tepl_Kontur->Status = cSWNoHeat;
    if (YesBit(gdp.Hot_Hand_Kontur->RCS, cbManMech))
        gdp.Hot_Tepl_Kontur->Status = cSWHand;
    if (YesBit(gdp.Hot_Tepl->discrete_inputs[0], cSmDHeat))
        gdp.Hot_Tepl_Kontur->Status = cSAlrExternal;

//	if (pGD_TControl_Tepl->Critery>0)
//		ClrBit(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].RCS,cbMinusCritery);
//	if (pGD_TControl_Tepl->Critery<0)
//		ClrBit(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].RCS,cbPlusCritery);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-*-*-*-*-*--Процедура определения нерегулируемости для контура-*-*-*-*-*/
/*************************************************************************/
void __sRegulKontur(char fnKontur)
{

//------------------------------------------------------------------------
//Контура с выключенными насосами не участвуют в расчете или если не установлена аварийная граница
//------------------------------------------------------------------------
#ifdef DEMO
    gdp.TControl_Tepl_Kontur->SErr=0;
    return;
#endif
    if ((!gdp.TControl_Tepl_Kontur->PumpStatus))
    {
        gdp.TControl_Tepl_Kontur->SErr = 0;
        return;
    }
//------------------------------------------------------------------------
//Подсчет нерегулирумости для контура
//------------------------------------------------------------------------

    creg.X = ((gdp.TControl_Tepl_Kontur->DoT / 10))
           - gdp.TControl_Tepl_Kontur->SensValue;
    if (((creg.X < 0) && (creg.X > (-cErrKontur)))
        || ((creg.X < cErrKontur) && (creg.X > 0)))
        creg.X = 0;

    if ((!creg.X) || (!SameSign(creg.X, gdp.TControl_Tepl_Kontur->SErr))) //(LngX*((long)(pGD_TControl_Tepl_Kontur->SErr)))<0)))
        gdp.TControl_Tepl_Kontur->SErr = 0;
    gdp.TControl_Tepl_Kontur->SErr += creg.X;

//------------------------------------------------------------------------
//Если общий, то взять нерегулируемость из теплицы А
//------------------------------------------------------------------------
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbAndKontur))
    if ((&(gdp.TControl_Tepl_Kontur->SErr))
        != (&GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].SErr))
        ;
    gdp.TControl_Tepl_Kontur->SErr = 0;
//------------------------------------------------------------------------
//Проверяем на контрольную границу
//------------------------------------------------------------------------
    if ((gdp.TControl_Tepl_Kontur->SErr >= v_ControlMidlWater)
        || (gdp.TControl_Tepl_Kontur->SErr <= -v_ControlMidlWater))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbCtrlErrKontur);

//------------------------------------------------------------------------
//Проверяем на аварийную границу
//------------------------------------------------------------------------

    if (gdp.TControl_Tepl_Kontur->SErr >= v_AlarmMidlWater)
    {
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);
        gdp.TControl_Tepl_Kontur->SErr = v_AlarmMidlWater;
    }
    if (gdp.TControl_Tepl_Kontur->SErr <= -v_AlarmMidlWater)
    {
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);
        gdp.TControl_Tepl_Kontur->SErr = -v_AlarmMidlWater;
    }
//------------------------------------------------------------------------
//Устанавливаем нерегулируемость в процентах со знаком
//------------------------------------------------------------------------

    creg.Y = gdp.TControl_Tepl_Kontur->SErr;
    CorrectionRule(v_ControlMidlWater, v_AlarmMidlWater, 100, 0);
    gdp.Hot_Tepl_Kontur->SError = (char) creg.Z;
    if ((creg.Z > cv_ResetMidlWater)
        && (YesBit(gdp.TControl_Tepl->MechBusy[fnKontur].RCS, cMSBlockRegs)))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur);
    creg.Y = -creg.Y;
    CorrectionRule(v_ControlMidlWater, v_AlarmMidlWater, 100, 0);
    gdp.Hot_Tepl_Kontur->SError += (char) creg.Z;
    if ((creg.Z > cv_ResetMidlWater)
        && (YesBit(gdp.TControl_Tepl->MechBusy[fnKontur].RCS, cMSBlockRegs)))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/****************************************************************************/
/*Процедура опредления возможно ли работать экраном в данных условиях*/
/****************************************************************************/
/*void __sWorkableScreen(void)
 {
 //------------------------------------------------------------------------
 //Если надо охлаждать
 //------------------------------------------------------------------------
 //------------------------------------------------------------------------
 //Установить возможности регулирования
 //------------------------------------------------------------------------
 if (pGD_Hot_Tepl_Kontur->Do<pGD_Hot_Tepl_Kontur->MaxCalc)
 SetBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegDownKontur);
 else
 SetBit(pGD_Hot_Tepl_Kontur->RCS,cbYesMaxKontur);
 //------------------------------------------------------------------------
 //Если надо нагревать
 //------------------------------------------------------------------------
 //------------------------------------------------------------------------
 //Установить возможности регулирования
 //------------------------------------------------------------------------
 if (pGD_Hot_Tepl_Kontur->Do>pGD_Hot_Tepl_Kontur->MinCalc)
 SetBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegUpKontur);
 else
 SetBit(pGD_Hot_Tepl_Kontur->RCS,cbYesMinKontur);



 }  */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/****************************************************************************/
/*Процедура опредления возможно ли работать фрамугами в данных условиях*/
/****************************************************************************/
/*void __sWorkableWindow(void)
 {
 //------------------------------------------------------------------------
 //Если надо охлаждать
 //------------------------------------------------------------------------
 //------------------------------------------------------------------------
 //Установить возможности регулирования
 //------------------------------------------------------------------------
 if ((pGD_Hot_Tepl->Kontur[cSmWindowUnW].Do<pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc)
 ||(pGD_Hot_Tepl->Kontur[cSmWindowOnW].Do<pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc))
 SetBit(pGD_Hot_Tepl->Kontur[cSmWindowUnW].ExtRCS,cbReadyRegDownKontur);
 else
 SetBit(pGD_Hot_Tepl->Kontur[cSmWindowUnW].RCS,cbYesMaxKontur);

 //------------------------------------------------------------------------
 //Если надо нагревать
 //------------------------------------------------------------------------
 //------------------------------------------------------------------------
 //Установить возможности регулирования
 //------------------------------------------------------------------------
 if ((pGD_Hot_Tepl->Kontur[cSmWindowUnW].Do>pGD_Hot_Tepl->Kontur[cSmWindowUnW].MinCalc)
 ||(pGD_Hot_Tepl->Kontur[cSmWindowOnW].Do>pGD_Hot_Tepl->Kontur[cSmWindowOnW].MinCalc))
 SetBit(pGD_Hot_Tepl->Kontur[cSmWindowUnW].ExtRCS,cbReadyRegUpKontur);
 else
 SetBit(pGD_Hot_Tepl->Kontur[cSmWindowUnW].RCS,cbYesMinKontur);


 }
 */
/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Потенциальный приоритет для контура--*-*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sPotentialPosibilityKontur(char fInv)
{
    int16_t *pRealPower;
    pRealPower = &(gdp.TControl_Tepl_Kontur->RealPower[fInv]);
//------------------------------------------------------------------------
//Приоритет по температуре воздуха в теплице
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Приоритет по влажности воздуха в теплице
//------------------------------------------------------------------------
    creg.Y = (int) gdp.Strategy_Kontur->RHPower;
    creg.X = -DefRH();

    creg.Z = (int) (((long) creg.Y) * creg.X / 1000);
//------------------------------------------------------------------------
//Приоритет по оптимальный температуре
//------------------------------------------------------------------------
    creg.X = 0;
    if (gdp.Hot_Tepl_Kontur->Optimal)
    {
        creg.X = (gdp.Hot_Tepl_Kontur->Optimal - gdp.Hot_Tepl_Kontur->Do);
//------------------------------------------------------------------------
//Если контур выключен то оптимальня температура сравнивается с минимумом контура
//------------------------------------------------------------------------
        creg.Y = (int) gdp.Strategy_Kontur->OptimalPower;
        creg.X = (int) (((long) creg.Y * creg.X / 100));
    }

//------------------------------------------------------------------------
//Приоритет с экономичностью
//------------------------------------------------------------------------

    (*pRealPower) = gdp.Strategy_Kontur->TempPower + creg.X + creg.Z;
    if ((*pRealPower) < 1)
        (*pRealPower) = 1;
    if (!fInv)
    {
        (*pRealPower) = 100 - gdp.Strategy_Kontur->TempPower - creg.X - creg.Z;
        if ((*pRealPower) < 1)
            (*pRealPower) = 1;
    }

}
void __WorkableKontur(char fnKontur, char fnTepl)
{

//------------------------------------------------------------------------
//Внимание - нерегулируемость из-за ручного управления
//------------------------------------------------------------------------
    if (YesBit((*(gdp.Hot_Hand_Kontur + cHSmMixVal)).RCS,/*(cbNoMech+*/
               cbManMech))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);

    creg.Y = GD.Hot.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].HandCtrl[cHSmMixVal
                                                                    + fnKontur].Position;
    //------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
    if ((gdp.Hot_Tepl_Kontur->Do < gdp.Hot_Tepl_Kontur->MaxCalc)
#ifndef DEMO
        && (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur)))
#endif
        && (gdp.TControl_Tepl_Kontur->PumpStatus) && ((creg.Y < 100)))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegUpKontur);
//------------------------------------------------------------------------
//Установить бит возможности работы насосом
//------------------------------------------------------------------------
#warning CHECK THIS
// NEW
    if ((!gdp.TControl_Tepl_Kontur->PumpStatus)
        && (gdp.TControl_Tepl->Critery > 0) && (fnKontur < cSmKontur5))
    {
        if ((GD.Hot.MidlSR < GD.TuneClimate.f_MinSun)
            && (gdp.Hot_Tepl->AllTask.NextTAir
                - GD.TControl.MeteoSensing[cSmOutTSens]
                > GD.TuneClimate.f_DeltaOut)
            || ((getTempHeat(fnTepl) - gdp.Hot_Tepl->AllTask.DoTHeat) < 0)
            && (((gdp.Control_Tepl->c_PFactor % 100) < 90)
                || (gdp.TControl_Tepl->StopVentI > 0)))
        {
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur);
            if ((gdp.TControl_Tepl_Kontur->NAndKontur  ==  1)
                && (!gdp.TControl_Tepl->qMaxOwnKonturs))
                gdp.TControl_Tepl_Kontur->RealPower[1] += 100;
        }
    }
//------------------------------------------------------------------------
//Если надо охлаждать и включен насос
//------------------------------------------------------------------------
    if (gdp.TControl_Tepl_Kontur->PumpStatus)
    {
//------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
        if ((gdp.Hot_Tepl_Kontur->Do > gdp.Hot_Tepl_Kontur->MinCalc)
#ifndef DEMO
            && (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur)))
#endif
            && (creg.Y > 0))
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur);
//------------------------------------------------------------------------
//Установить возможности работы насосом
//------------------------------------------------------------------------
        if ((!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur)))
            && (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur)))
            && (!gdp.TControl_Tepl_Kontur->PumpPause)
            && (gdp.TControl_Tepl->Critery < 0) && (!creg.Y)
            && (GD.TControl.MeteoSensing[cSmOutTSens] > 500))
            SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur);
    }

    if ((YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur))
        && (gdp.TControl_Tepl->Critery < 0))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur);
    if ((YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegUpKontur))
        && (gdp.TControl_Tepl->Critery > 0))
        SetBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-Процедура определения приоритетов с возможностями и подсчета их суммы-*/
/*************************************************************************/
void __sRealPosibilityKonturs(char fnKontur, long* fMinMax)
{

//------------------------------------------------------------------------
//Если контур не участвует в управлении то пропускаем
//------------------------------------------------------------------------

    if (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS,
                 (cbReadyRegUpKontur + cbReadyPumpKontur))))
        gdp.TControl_Tepl_Kontur->RealPower[1] = 0;
    if (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS,
                 (cbReadyRegDownKontur + cbReadyPumpKontur))))
        gdp.TControl_Tepl_Kontur->RealPower[0] = 0;

    if (!(YesBit(gdp.Hot_Tepl_Kontur->ExtRCS,
                 (cbReadyPumpKontur + cbReadyRegsKontur))))
        return; //ByteZ;

//------------------------------------------------------------------------
//Если надо охлаждать то меняем знак экономического приоритета для того
//чтобы он в положительной оси отображал желание рабоать, а в отрицательной - нежелание
//------------------------------------------------------------------------
//	if (pGD_TControl_Tepl->Critery<0)

//------------------------------------------------------------------------
//Меняем приоритеты регулирующих контуров в зависимости от нерегулируемости
//------------------------------------------------------------------------

    if (YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur))
    {
        if (gdp.TControl_Tepl_Kontur->NAndKontur  ==  1)
        {
            gdp.TControl_Tepl->NOwnKonturs++;
        }
//------------------------------------------------------------------------
//Ищем максимальный приоритет контуров и запоминаем его номер
//------------------------------------------------------------------------
    }

    if (fMinMax[0]
        < gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower])
    {
        fMinMax[0] =
        gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower];
        gdp.TControl_Tepl->nMaxKontur = fnKontur;
    }
    if ((fMinMax[1]
         < gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower])
        && (!YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[1] =
        gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower];
    }
    if ((fMinMax[2]
         < gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower])
        && (gdp.TControl_Tepl_Kontur->NAndKontur  ==  1)
        && (!YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[2] =
        gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower];

    }

//------------------------------------------------------------------------
//Если есть отрицательные приоритеты то их надо установить в 0
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Если работаем насосами то надо сбросить приоритет
//------------------------------------------------------------------------

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*--Процедура распределения критерия на данный контур--*-*-*-*-*-*/
/*************************************************************************/
long __sRaspKontur(void)
{
    if ((!gdp.TControl_Tepl->qMaxKonturs)
        || (!YesBit(gdp.Hot_Tepl_Kontur->RCS, cbGoMax)))
        return 0;

    gdp.TControl_Tepl->StopI = 0;

    long long_y = gdp.TControl_Tepl->Critery;
    long_y = long_y / gdp.TControl_Tepl->qMaxKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / ((long) gdp.Strategy_Kontur->Powers);

    return long_y;
}
long __sRaspOwnKontur(void)
{
    if ((!gdp.TControl_Tepl->qMaxOwnKonturs)
        || (!YesBit(gdp.Hot_Tepl_Kontur->RCS, cbGoMaxOwn)))
        return 0;
    long long_y = gdp.TControl_Tepl->Critery;
    long_y = long_y / gdp.TControl_Tepl->qMaxOwnKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / ((long) gdp.Strategy_Kontur->Powers);

    return long_y;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
long __TempToVent(void)
{
    long long_y = 100 * ((long) gdp.TControl_Tepl->TVentCritery);
    long_y = long_y / 50;
    long_y = long_y * 30;
    long_y /= ((long) gdp.TControl_Tepl->f_Power);

    return long_y;
}
long __VentToTemp(long sVent)
{
    long long_y = sVent * ((long) gdp.TControl_Tepl->f_Power);
    long_y /= 30;
    long_y *= 50; //((long)pGD_ConstMechanic->ConstMixVal[cHSmWinN].Power);
    return long_y / 100;
}

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки фрамуг--*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sLastCheckWindow(char fnTepl)
{
    int DoUn;
    int DoOn;
    int tDo;
//	long xdata	tCalc;

    int MinUn;
    int MinOn;
    int MaxUn;
    int MaxOn;
    int tMaximum;
    int tSLevel;

    MinOn = gdp.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc;
    MinUn = gdp.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc;
    MaxOn = gdp.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc;
    MaxUn = gdp.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc;

    creg.Y = gdp.TControl_Tepl->Kontur[cSmWindowUnW].CalcT;
    if (creg.Y < 0)
        creg.Y = 0;
    if (creg.Y > GD.TuneClimate.f_S1Level)
        tDo = GD.TuneClimate.f_S1Level;
    else
        tDo = creg.Y;
    creg.Y -= GD.TuneClimate.f_S1Level;
    creg.Y *= GD.TuneClimate.f_S1MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    if (creg.Y > GD.TuneClimate.f_S2Level - GD.TuneClimate.f_S1Level)
        tDo = GD.TuneClimate.f_S2Level;
    else
        tDo += creg.Y;
    creg.Y -= GD.TuneClimate.f_S2Level - GD.TuneClimate.f_S1Level;
    creg.Y = (creg.Y * GD.TuneClimate.f_S2MinDelta) / GD.TuneClimate.f_S1MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    if (creg.Y > GD.TuneClimate.f_S3Level - GD.TuneClimate.f_S2Level)
        tDo = GD.TuneClimate.f_S3Level;
    else
        tDo += creg.Y;
    creg.Y -= GD.TuneClimate.f_S3Level - GD.TuneClimate.f_S2Level;
    creg.Y = (creg.Y * GD.TuneClimate.f_S3MinDelta) / GD.TuneClimate.f_S2MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    tDo += creg.Y;

    creg.Y = GD.TuneClimate.f_DefOnUn;
//	if (MaxUn<IntY) IntY=MaxUn;

    if (tDo < creg.Y)
    {
        DoUn = tDo;
        DoOn = 0;
    }
    else
    {
        tDo -= creg.Y;
        DoUn = creg.Y + tDo / 2;
        DoOn = tDo / 2;
    }
    if (DoUn > MaxUn)
        DoOn += DoUn - MaxUn;
    if (DoOn > MaxOn)
        DoUn += DoOn - MaxOn;

    tMaximum = MaxOn + MaxUn;
    tDo = (tMaximum - GD.TuneClimate.f_S3Level) / GD.TuneClimate.f_S3MinDelta;
    tSLevel = GD.TuneClimate.f_S3Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += (tSLevel - GD.TuneClimate.f_S2Level) / GD.TuneClimate.f_S2MinDelta;
    tSLevel = GD.TuneClimate.f_S2Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += (tSLevel - GD.TuneClimate.f_S1Level) / GD.TuneClimate.f_S1MinDelta;
    tSLevel = GD.TuneClimate.f_S1Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += tSLevel;

    gdp.TControl_Tepl->AbsMaxVent = __VentToTemp(tDo);

    if (DoOn < MinOn)
        DoOn = MinOn;
    if (DoUn < MinUn)
        DoUn = MinUn;

    if (DoUn > MaxUn)
        DoUn = MaxUn;
    if (DoOn > MaxOn)
        DoOn = MaxOn;

    //IntY=CURRENT_TEMP_VALUE-pGD_Hot_Tepl->AllTask.DoTVent;   // было
#warning CHECK THIS
// NEW


    creg.Y = 0;
    if (! getTempVent(fnTepl))
        creg.Y = 0;
    else
        creg.Y = getTempVent(fnTepl) - gdp.Hot_Tepl->AllTask.DoTVent;

    if (((DoUn  ==  MaxUn) && (DoOn  ==  MaxOn) && (creg.Y > 0))
        || ((DoUn  ==  MinUn) && (DoOn  ==  MinOn) && (creg.Y < 0)))
        gdp.TControl_Tepl->StopVentI++;
    else
        gdp.TControl_Tepl->StopVentI = 0;

    if (gdp.TControl_Tepl->StopVentI > cMaxStopI)
        gdp.TControl_Tepl->StopVentI = cMaxStopI;

    gdp.TControl_Tepl->Kontur[cSmWindowUnW].DoT = DoUn;
    gdp.TControl_Tepl->Kontur[cSmWindowOnW].DoT = DoOn;

    gdp.Hot_Tepl->Kontur[cSmWindowUnW].Do = (DoUn);
    gdp.Hot_Tepl->Kontur[cSmWindowOnW].Do = (DoOn);
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки экрана--*-*-*-*-*-*-*-*/
/*************************************************************************/
/*void __sLastCheckScreen(void)
 {
 pGD_TControl_Tepl->Kontur[cSmScreen].DoT-=pGD_TControl_Tepl->Kontur[cSmScreen].CalcT/100;
 pGD_Hot_Tepl_Kontur->Do=(int)(pGD_TControl_Tepl_Kontur->DoT/10);
 if (pGD_Hot_Tepl_Kontur->Do<pGD_Hot_Tepl_Kontur->MinCalc)
 {
 pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MinCalc;
 pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->Do)*10;
 }
 if (pGD_Hot_Tepl_Kontur->Do>pGD_Hot_Tepl_Kontur->MaxCalc)
 {
 pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MaxCalc;
 pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->Do)*10;
 }

 } */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*****************************************************************************/
/*Процедура преобразования значений температуры в температуру первого контура*/
/*****************************************************************************/
long __sThisToFirst(int in)
{
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbScreenKontur)) return 0;
    return(((((long) in) * ((long) gdp.Strategy_Kontur->Powers)) / 50));
    /********************************************************************
     -----------Работа автонастройки временно приостановлена--------------
     *********************************************************************
     *1000/pGD_TControl_Tepl->sac_MomentMull)*1000/pGD_TControl_Tepl->sac_LongMull[0]);
     *********************************************************************/
}

/**************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки контура--*-*-*-*-*-*-*-*/
/**************************************************************************/
void __sLastCheckKontur(char fnKontur)
{
    int OldDoT;
    long TempDo;

    if (YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur))
        gdp.TControl_Tepl_Kontur->PumpStatus = 1;

    if (!gdp.TControl_Tepl_Kontur->PumpStatus)
    {
        /*		if(pGD_TControl_Tepl_Kontur->DoT)
         (*fnCorCritery)+=((int)(pGD_TControl_Tepl_Kontur->DoT/100)-GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10);*/
        gdp.TControl_Tepl_Kontur->DoT = 0;
        gdp.Hot_Tepl_Kontur->Do = 0;
        return;

    }
    //SetBit(pGD_Hot_Tepl_Kontur->RCS,cbOnPumpKontur);

    if (gdp.TControl_Tepl_Kontur->Manual)
        return;

    if ((!SameSign(gdp.TControl_Tepl->Critery, gdp.TControl_Tepl_Kontur->SErr))
        && (YesBit(gdp.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur)))
    {
        OldDoT = gdp.TControl_Tepl_Kontur->DoT;
        gdp.Hot_Tepl_Kontur->Do = ((gdp.TControl_Tepl_Kontur->SensValue));
        gdp.Hot_Tepl_Kontur->Do = clamp_min(gdp.Hot_Tepl_Kontur->Do, gdp.Hot_Tepl_Kontur->MinCalc);
        gdp.Hot_Tepl_Kontur->Do = clamp_max(gdp.Hot_Tepl_Kontur->Do, gdp.Hot_Tepl_Kontur->MaxCalc);
        TempDo = gdp.Hot_Tepl_Kontur->Do * 10;
        gdp.TControl_Tepl_Kontur->SErr = 0;
        gdp.TControl_Tepl->Integral -= __sThisToFirst((int) ((OldDoT - TempDo)))
                                       * 100;
        //	pGD_TControl_Tepl->SaveIntegral-=__sThisToFirst((int)((OldDoT-TempDo)))*100;
        gdp.TControl_Tepl_Kontur->DoT = TempDo;
        return;
    }
    if ((!gdp.TControl_Tepl_Kontur->DoT)
        && (gdp.TControl_Tepl_Kontur->LastDoT < 5000))
        gdp.TControl_Tepl_Kontur->DoT = gdp.TControl_Tepl_Kontur->LastDoT;
    gdp.TControl_Tepl_Kontur->DoT = gdp.TControl_Tepl_Kontur->DoT
                                    + (int) (gdp.TControl_Tepl_Kontur->CalcT);
    gdp.Hot_Tepl_Kontur->Do = (gdp.TControl_Tepl_Kontur->DoT / 10);

    if (gdp.Hot_Tepl_Kontur->Do <= gdp.Hot_Tepl_Kontur->MinCalc)
    {
        gdp.TControl_Tepl_Kontur->DoT = (((gdp.Hot_Tepl_Kontur->MinCalc)))
                                        * 10;
    }
    if (gdp.Hot_Tepl_Kontur->Do >= gdp.Hot_Tepl_Kontur->MaxCalc)
    {
        gdp.TControl_Tepl_Kontur->DoT = (((gdp.Hot_Tepl_Kontur->MaxCalc)))
                                        * 10;
    }
//	if (pGD_Hot_Tepl_Kontur->Do<pGD_Hot_Tepl->AllTask.DoTHeat/10)
//		pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MinCalc;//GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10;

//	(*fnCorCritery)+=(pGD_Hot_Tepl_Kontur->Do-(pGD_TControl_Tepl_Kontur->DoT/10));
    gdp.Hot_Tepl_Kontur->Do = (gdp.TControl_Tepl_Kontur->DoT / 10);

    gdp.TControl_Tepl_Kontur->LastDoT = gdp.TControl_Tepl_Kontur->DoT;

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int __sCalcTempKonturs(void)
{
    int SumTemp = 0;
    for (int i = 0; i < cSWaterKontur - 2; i++)
    {
        SetPointersOnKontur(i);
        //if 	(ByteX=) continue;
        if (gdp.TControl_Tepl_Kontur->DoT)
            SumTemp += __sThisToFirst(gdp.TControl_Tepl_Kontur->DoT)
                       - gdp.Hot_Tepl->AllTask.DoTHeat;
        else if ((gdp.TControl_Tepl_Kontur->LastDoT < 5000)
                 && (gdp.TControl_Tepl_Kontur->LastDoT
                     > gdp.Hot_Tepl->AllTask.DoTHeat))
            SumTemp += __sThisToFirst(gdp.TControl_Tepl_Kontur->LastDoT)
                       - gdp.Hot_Tepl->AllTask.DoTHeat;
    }
    return SumTemp;
}

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sCalcKonturs(void)
{
    long MinMaxPowerReg[3];
    long temp;
    int OldCrit;
    char isFram;
    char fnTepl, tTepl;

    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);

//		if(!pGD_Hot_Tepl->AllTask.NextTAir) return;

//Определение направления нагрев или охлаждени
        gdp.TControl_Tepl->CurrPower = 0;
        if (gdp.TControl_Tepl->Critery > 0)
            gdp.TControl_Tepl->CurrPower = 1;
// Отключить, если нет задания
        if (!gdp.Hot_Tepl->AllTask.DoTHeat)
            continue;
// Расчет минимумов и максимумов контуров
        for (int i = 0; i < cSWaterKontur; i++)
        {
            SetPointersOnKontur(i);
            __sInitKontur(i);
        }
//		__sMinMaxScreen();
// Расчет минимумов и максимумов фрамуг
        __sMinMaxWindows();

    }
// Заново считаем
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);

        MinMaxPowerReg[2] = 0;
        MinMaxPowerReg[1] = 0;
        MinMaxPowerReg[0] = 0;
        gdp.TControl_Tepl->nMaxKontur = -1;
        gdp.TControl_Tepl->NOwnKonturs = 0;
        if (!GD.Hot.Tepl[fnTepl].AllTask.DoTHeat)
            continue;
        for (int i = 0; i < cSWaterKontur; i++)
        {
            SetPointersOnKontur(i);
            __sRegulKontur(i);
            gdp.TControl_Tepl_Kontur->Manual = 0;
            if (YesBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (gdp.Hot_Tepl_Kontur->Do)
            {
                gdp.TControl_Tepl_Kontur->PumpStatus = 1;
                gdp.TControl_Tepl_Kontur->Manual = 1;
                gdp.TControl_Tepl_Kontur->DoT = (gdp.Hot_Tepl_Kontur->Do * 10);
                continue;
            }
            gdp.Hot_Tepl_Kontur->Do = (gdp.TControl_Tepl_Kontur->DoT / 10);
            __sPotentialPosibilityKontur(0); //Приоритет в случае охлаждения
            __sPotentialPosibilityKontur(1); //Приоритет в случае нагрева

            __WorkableKontur(i, fnTepl);
            __sRealPosibilityKonturs(i, MinMaxPowerReg);
            gdp.Hot_Tepl_Kontur->Priority =
            (int) (gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower]);
            gdp.TControl_Tepl->PowMaxKonturs = MinMaxPowerReg[1];
            gdp.TControl_Tepl->PowOwnMaxKonturs = MinMaxPowerReg[2];
        }

//		if ((GD.TControl.PrevPozFluger!=GD.Hot.PozFluger)&&(GD.TuneClimate.f_changeWindows))
//		{
//			temp=pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT;
//			pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT=pGD_TControl_Tepl->Kontur[cSmWindowUnW].DoT;
//			pGD_TControl_Tepl->Kontur[cSmWindowUnW].DoT=temp;
//
//		}
//		pGD_Hot_Tepl->Kontur[cSmWindowOnW].Do=(pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT/10);

//		__sWorkableWindow();

//		pGD_TControl_Tepl->Kontur[cSmWindowUnW].CalcT=0;

        gdp.Hot_Tepl->Kontur[cSmWindowUnW].Optimal =
        gdp.TControl_Tepl->f_NMinDelta;

        gdp.TControl_Tepl->Kontur[cSmWindowUnW].CalcT = __TempToVent();

        __sLastCheckWindow(fnTepl);

        gdp.TControl_Tepl->qMaxKonturs = 0;
        gdp.TControl_Tepl->qMaxOwnKonturs = 0;

        for (int i = 0; i < cSWaterKontur; i++)
        {
            SetPointersOnKontur(i);
            if (YesBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if ((gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower]
                 >= gdp.TControl_Tepl->PowMaxKonturs)
                && (gdp.TControl_Tepl->PowMaxKonturs))
            {
                SetBit(gdp.Hot_Tepl_Kontur->RCS, cbGoMax);
                gdp.TControl_Tepl->qMaxKonturs++;
            }
            if ((gdp.TControl_Tepl_Kontur->RealPower[gdp.TControl_Tepl->CurrPower]
                 >= gdp.TControl_Tepl->PowOwnMaxKonturs)
                && (gdp.TControl_Tepl->PowOwnMaxKonturs)
                && (gdp.TControl_Tepl_Kontur->NAndKontur  ==  1))
            {
                SetBit(gdp.Hot_Tepl_Kontur->RCS, cbGoMaxOwn);
                gdp.TControl_Tepl->qMaxOwnKonturs++;
            }
        }
    }
    GD.TControl.PrevPozFluger = GD.Hot.PozFluger;

    //Очень длинный расчет, поэтому проверим измерения
//	CheckReadyMeasure();
    //------------------------------------------------
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);

//		if (!pGD_TControl_Tepl->qMaxOwnKonturs)
//			pGD_TControl_Tepl->StopI=2;
        if (!gdp.Hot_Tepl->AllTask.DoTHeat)
            continue;

        if ((gdp.TControl_Tepl->nMaxKontur >= 0)
            && (YesBit(
                      gdp.Hot_Tepl->Kontur[gdp.TControl_Tepl->nMaxKontur].ExtRCS,
                      cbReadyPumpKontur))
            && (!YesBit(
                       gdp.Hot_Tepl->Kontur[gdp.TControl_Tepl->nMaxKontur].RCS,
                       cbPumpChange)))
        {
            int byte_y = 0;
            gdp.TControl_Tepl->qMaxKonturs = 0;
            gdp.TControl_Tepl->qMaxOwnKonturs = 0;
            if (gdp.TControl_Tepl->Critery < 0)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(gdp.TControl_Tepl->Kontur[gdp.TControl_Tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;

                    if ((!(YesBit(
                                 GD.Hot.Tepl[tTepl].Kontur[gdp.TControl_Tepl->nMaxKontur].ExtRCS,
                                 cbReadyPumpKontur)))
                        && ((!GD.TControl.Tepl[tTepl].NOwnKonturs)
                            || (gdp.TControl_Tepl->NOwnKonturs)))
                        byte_y = 1;
                }
            }
            if (!byte_y)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(gdp.TControl_Tepl->Kontur[gdp.TControl_Tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;
                    GD.TControl.Tepl[tTepl].Kontur[gdp.TControl_Tepl->nMaxKontur].PumpStatus =
                    1
                    - gdp.TControl_Tepl->Kontur[gdp.TControl_Tepl->nMaxKontur].PumpStatus;
                    GD.TControl.Tepl[tTepl].Kontur[gdp.TControl_Tepl->nMaxKontur].PumpPause =
                    cPausePump;
                    SetBit(
                          GD.Hot.Tepl[tTepl].Kontur[gdp.TControl_Tepl->nMaxKontur].RCS,
                          cbPumpChange);
                }
            }
        }

    }
//	CheckReadyMeasure();
    for (int i = 0; i < cSWaterKontur; i++)
    {

        for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
        {
            SetPointersOnTepl(fnTepl);
            SetPointersOnKontur(i);
            gdp.TControl_Tepl->RealPower = 0;
            if (YesBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            gdp.TControl_Tepl->NAndKontur =
            gdp.TControl_Tepl_Kontur->NAndKontur;
            GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].RealPower +=
            __sRaspKontur();
//			if ((!pGD_TControl_Tepl->NOwnKonturs)&&(pGD_TControl_Tepl_Kontur->RealPower))
//			{
//				pGD_TControl_Tepl->NAndKontur=1;
//				GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].RealPower=__sRaspKontur();
//				break;
//			}
        }
        for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
        {
            SetPointersOnTepl(fnTepl);
            SetPointersOnKontur(i);
            if (YesBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (gdp.TControl_Tepl_Kontur->NAndKontur  ==  1)
                continue;
            long long_y = GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].RealPower;
            int8_t byte_w = GD.TControl.Tepl[gdp.TControl_Tepl_Kontur->MainTepl].NAndKontur;
            long_y /= byte_w;

            OldCrit = gdp.TControl_Tepl->Critery;
            if (gdp.TControl_Tepl->NOwnKonturs)
                gdp.TControl_Tepl->Critery = gdp.TControl_Tepl->Critery
                                             - __sThisToFirst((int) ((long_y)));
            if (!SameSign(OldCrit, gdp.TControl_Tepl->Critery))
                gdp.TControl_Tepl->Critery = 0;
            gdp.TControl_Tepl_Kontur->CalcT = long_y;
            __sLastCheckKontur(i);
            creg.Y = 0;

            if ((gdp.Hot_Tepl->MaxReqWater < gdp.Hot_Tepl_Kontur->Do)
                && (i < cSWaterKontur))
                gdp.Hot_Tepl->MaxReqWater = gdp.Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }

    }
//	CheckReadyMeasure();
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        for (int i = 0; i < cSWaterKontur; i++)
        {
            SetPointersOnKontur(i);
            if (YesBit(gdp.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (gdp.TControl_Tepl_Kontur->NAndKontur != 1)
                continue;
            gdp.TControl_Tepl_Kontur->CalcT = __sRaspOwnKontur();
            __sLastCheckKontur(i);
            creg.Y = 0;
//			if ((((long)IntY)*pGD_TControl_Tepl->Critery)<0) IntY=0;
            if ((gdp.Hot_Tepl->MaxReqWater < gdp.Hot_Tepl_Kontur->Do)
                && (i < cSWaterKontur))
                gdp.Hot_Tepl->MaxReqWater = gdp.Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }
        if ((!gdp.TControl_Tepl->NOwnKonturs))
            gdp.TControl_Tepl->StopI++;
        else
            gdp.TControl_Tepl->StopI = 0;
        if (gdp.TControl_Tepl->StopI > cMaxStopI)
            gdp.TControl_Tepl->StopI = cMaxStopI;
    }

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sMechScreen(void)
{
    char fnTepl;
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        if (gdp.TControl_Tepl->Screen[0].Value)
        {
            gdp.Hot_Tepl->OtherCalc.CorrScreen =
            gdp.Hot_Tepl->Kontur[cSmScreen].Do; //IntZ
        }
        SetPosScreen(cTermHorzScr);
        SetPosScreen(cSunHorzScr);
        SetPosScreen(cTermVertScr1);
        SetPosScreen(cTermVertScr2);
        SetPosScreen(cTermVertScr3);
        SetPosScreen(cTermVertScr4);
    }
}

void __sMechWindows(void)
{
    char WindWin[2];
    char fnTepl;
//Оптимизация
    WindWin[0] = cSWaterKontur + 1 - GD.Hot.PozFluger;
    WindWin[1] = cSWaterKontur + GD.Hot.PozFluger;

    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        for (int i = cSWaterKontur; i < cSWaterKontur + 2; i++)
        {
            int byte_z = i - cSWaterKontur;
            gdp.Hot_Tepl_Kontur = &(gdp.Hot_Tepl->Kontur[i]);
            gdp.TControl_Tepl_Kontur =
            &(gdp.TControl_Tepl->Kontur[WindWin[byte_z]]);
            gdp.Hot_Hand_Kontur = &GD.Hot.Tepl[fnTepl].HandCtrl[cHSmMixVal
                                                                + WindWin[byte_z]];

            if (gdp.TControl_Tepl_Kontur->TPause)
            {
                gdp.TControl_Tepl_Kontur->TPause--;
                gdp.TControl_Tepl_Kontur->TPause = clamp_min(gdp.TControl_Tepl_Kontur->TPause, 0);
                continue;
            }

            creg.X = gdp.Hot_Tepl_Kontur->Do;
//			IntY=abs(IntZ-((int)(pGD_Hot_Hand_Kontur->Position)));

            if (YesBit(gdp.Hot_Hand_Kontur->RCS, cbManMech))
                continue;

            /*		    if ((IntY<pGD_TControl_Tepl->f_NMinDelta)&&(IntX)&&(IntX!=100))continue;

             if ((IntX-pGD_Hot_Tepl_Kontur->MinCalc)<pGD_TControl_Tepl->f_NMinDelta)
             {
             IntX=pGD_Hot_Tepl_Kontur->MinCalc;
             }
             if ((pGD_Hot_Tepl_Kontur->MaxCalc-IntX)<pGD_TControl_Tepl->f_NMinDelta)
             {
             IntX=pGD_Hot_Tepl_Kontur->MaxCalc;
             }
             */

            gdp.Hot_Hand_Kontur->Position = (char) creg.X;

//			pGD_TControl_Tepl_Kontur->TPause=GD.TuneClimate.f_MinTime;
        }
        //pGD_Level_Tepl[cSmWinNSens][cSmUpAlarmLev]=pGD_Hot_Hand[cHSmWinN].Position+GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinNSens][cSmDownAlarmLev]=pGD_Hot_Hand[cHSmWinN].Position-GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinSSens][cSmUpAlarmLev]=pGD_Hot_Hand[cHSmWinS].Position+GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinSSens][cSmDownAlarmLev]=pGD_Hot_Hand[cHSmWinS].Position-GD.TuneClimate.f_MaxAngle;
    }
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void DecPumpPause(void)
{
    for (int i = 0; i < cSWaterKontur; i++)
    {
        SetPointersOnKontur(i);
        if (((*(gdp.Hot_Hand_Kontur + cHSmMixVal)).Position < 100)
            && ((*(gdp.Hot_Hand_Kontur + cHSmMixVal)).Position > 0))
        {
            gdp.TControl_Tepl_Kontur->PumpPause = cPausePump;
            if (gdp.TControl_Tepl_Kontur->DoT > 4000)
                gdp.TControl_Tepl_Kontur->PumpPause += cPausePump;
        }
        if (gdp.TControl_Tepl_Kontur->PumpPause > 0)
        {
            gdp.TControl_Tepl_Kontur->PumpPause--;
//			SetBit(pGD_Hot_Tepl->ExtRCS,cbPausePumpTepl);
        }
        else
            gdp.TControl_Tepl_Kontur->PumpPause = 0;
    }

}

