#define _FRIEND_OF_CONTROL_

#include "syntax.h"

#include "control_gd.h"
#include "control.h"
#include "control_subr.h"
#include "control_screen.h"
#include "control_strategy.h"


/**************************************************************************/
/*-*-*-*-*-*-*--Процедура установки границ для водных контуров--*-*-*-*-*-*/
/**************************************************************************/
int DefRH(const gh_t *gh)
{
    if ((! gh->hot->AllTask.DoRHAir)|| (! gh->hot->InTeplSens[cSmRHSens].Value))
        return 0;
    return gh->hot->InTeplSens[cSmRHSens].Value - gh->hot->AllTask.DoRHAir;

}

void SetNoWorkKontur(const contour_t *ctr)
{
    SetBit(ctr->hot->RCS, cbNoWorkKontur);
    ctr->tcontrol->PumpStatus = 0;
    ctr->tcontrol->DoT = 0;
}

void __sMinMaxWater(const contour_t *ctr)
{
//------------------------------------------------------------------------
//Заполняем минимальные и максимальные границы для контуров а также опт температуру
//------------------------------------------------------------------------
    ctr->hot->MaxCalc = ctr->link.gh_ctrl->c_MaxTPipe[ctr->cidx];
//------------------------------------------------------------------------
//Для контуров начиная с третьего берем минимальные границы и опт. темп. из параметров
//------------------------------------------------------------------------

//	if  ((GD.Hot.MidlSR<GD.TuneClimate.f_MinSun)&&(pGD_Hot_Tepl->AllTask.NextTAir-GD.TControl.OutTemp>GD.TuneClimate.f_DeltaOut))
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].ExtRCS,cbBlockPumpKontur);

    if (ctr->cidx  ==  cSmKontur4)
    {
        ctr->hot->MinTask = ctr->link.gh_ctrl->c_MinTPipe[1];
        ctr->hot->Optimal = ctr->link.gh_ctrl->c_OptimalTPipe[1];
        ctr->hot->MinCalc = ctr->hot->MinTask;
    }
//------------------------------------------------------------------------
//Если установлен минимум то насос должен быть всегда включен
//------------------------------------------------------------------------
    if ((ctr->hot->MinTask)
        || (YesBit(ctr->hand[cHSmPump].RCS, cbManMech)))
        SetBit(ctr->hot->ExtRCS, cbBlockPumpKontur);
//------------------------------------------------------------------------
//Ограничиваем минимум
//------------------------------------------------------------------------
    ///минимум всех контуров выше задания на 5 градусов
    int min_all_contour = ctr->link.hot->AllTask.DoTHeat/10 + 20;
    ctr->hot->MinCalc = MAX(ctr->hot->MinCalc, min_all_contour);
//------------------------------------------------------------------------
//В виду особенностей работы 5 контура для него отдельные установки
//------------------------------------------------------------------------
    if (ctr->hot->MinCalc > ctr->hot->MaxCalc)
    {
        ctr->hot->MinCalc = ctr->hot->MaxCalc;
    }
    if (ctr->cidx  ==  cSmKontur5)
    {

        if ((!(YesBit(ctr->link.hot->InTeplSens[cSmGlassSens].RCS, cbMinMaxVSens)))
            && ((ctr->link.hot->InTeplSens[cSmGlassSens].Value
                 < _GD.TuneClimate.c_DoMinIfGlass)))
        {
            int creg_y = ctr->link.hot->InTeplSens[cSmGlassSens].Value;
            int creg_z;
            CorrectionRule(_GD.TuneClimate.c_DoMaxIfGlass,
                           _GD.TuneClimate.c_DoMinIfGlass,
                           ctr->hot->MaxCalc - cMin5Kontur,
                           0,
                           creg_y,
                           &creg_z);
            ctr->hot->Do = ctr->hot->MaxCalc - creg_z;
            SetBit(ctr->hot->ExtRCS, cbBlockPumpKontur);

        }
        if (YesBit(_GD.TControl.bSnow, 0x02))
        {
            int creg_y = _GD.TControl.MeteoSensing[cSmOutTSens];
            int creg_z;
            CorrectionRule(_GD.TuneClimate.c_CriticalSnowOut,
                           c_SnowIfOut,
                           ctr->hot->MaxCalc - _GD.TuneClimate.c_MinIfSnow,
                           0,
                           creg_y,
                           &creg_z);
            ctr->hot->Do = ctr->hot->MaxCalc - creg_z;
            SetBit(ctr->hot->ExtRCS, cbBlockPumpKontur);
        }

        if ((ctr->link.tcontrol_tepl->ScrExtraHeat) || (_GD.Hot.Util  ==  9))
        {
            SetBit(ctr->hot->ExtRCS, cbBlockPumpKontur);
            //pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->MaxCalc*10);
            ctr->hot->Do = ctr->hot->MaxCalc;
        }
//		else
//		if  (pGD_TControl_Tepl->LastScrExtraHeat)
//			pGD_TControl_Tepl_Kontur->DoT=pGD_TControl_Tepl->TempStart5;

//		pGD_TControl_Tepl->LastScrExtraHeat=pGD_TControl_Tepl->ScrExtraHeat;

//ogrMin(&(pGD_Hot_Tepl_Kontur->MinCalc),cMin5Kontur);
        ctr->hot->Do = MAX(ctr->hot->Do, ctr->hot->MinCalc);
        if (!YesBit(ctr->hot->ExtRCS, cbBlockPumpKontur))
        {
            ctr->hot->Do = 0;
            ctr->hot->MaxCalc = 0;
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
void __sMinMaxWindows(const gh_t *gh)
{
    int t_max;
//-------------------------------------------------------
//Проверяем работу по заданию
//------------------------------------------------------

    if ((gh->hot->AllTask.Win < 2)
        || (YesBit(gh->tcontrol_tepl->Calorifer, 0x02))) //Добавлено для блокировки фрамуг кондиционером
    {
//--------------------------------------------------------------------------------
//Если установлен режим работы по минимому то минимум должен совпасть с максимумом
//--------------------------------------------------------------------------------
        if (gh->hot->AllTask.Win)
        {
            gh->hot->Kontur[cSmWindowUnW].MaxCalc =
            gh->hot->Kontur[cSmWindowUnW].MinCalc;
            gh->hot->Kontur[cSmWindowUnW].Status = cSFollowProg;
            gh->hot->Kontur[cSmWindowOnW].Status = cSFollowProg;
        }
        else
        {
            memclr(&gh->hot->Kontur[cSmWindowUnW],
                   sizeof(eKontur));
        }
//--------------------------------------------------------------------------------
//Стираем все накопленные данные
//--------------------------------------------------------------------------------
        gh->tcontrol_tepl->Kontur[cSmWindowOnW].DoT = 0;
        gh->tcontrol_tepl->Kontur[cSmWindowUnW].DoT = 0;
        return;
    }

    gh->hot->Kontur[cSmWindowOnW].Status = cSOn;
    gh->hot->Kontur[cSmWindowUnW].Status = cSOn;

    int creg_y = DefRH(gh);
    int creg_z;
    CorrectionRule(_GD.TuneClimate.f_min_RHStart,
                   _GD.TuneClimate.f_min_RHEnd,
                   (int) _GD.TuneClimate.f_min_Cor,
                   0,
                   creg_y,
                   &creg_z);
    gh->hot->Kontur[cSmWindowUnW].MinCalc = MAX(gh->hot->Kontur[cSmWindowUnW].MinCalc, creg_z);

    creg_y = _GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(_GD.TuneClimate.f_StartCorrPow,
                   _GD.TuneClimate.f_EndCorrPow,
                   (_GD.TuneClimate.f_PowFactor - 1000),
                   0,
                   creg_y,
                   &creg_z);
    gh->tcontrol_tepl->f_Power = _GD.TuneClimate.f_PowFactor - creg_z;
    if (_GD.TuneClimate.f_PowFactor < 1000)
        gh->tcontrol_tepl->f_Power = 1000;

    t_max = gh->gh_ctrl->f_MaxOpenUn;
    if ((t_max > _GD.TuneClimate.f_MaxOpenRain) && (_GD.TControl.bSnow))
    {
        gh->hot->Kontur[cSmWindowOnW].Status = cSWRain;
        gh->hot->Kontur[cSmWindowUnW].Status = cSWRain;

        t_max = _GD.TuneClimate.f_MaxOpenRain;
    }
    //--------------------------------------------------------------------------------
//Максимум рассчитываем по внешней температуре
//--------------------------------------------------------------------------------
    creg_y = _GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(_GD.TuneClimate.f_MinTFreeze,
                   _GD.TuneClimate.f_MinTFreeze + f_MaxTFreeze,
                   200,
                   0,
                   creg_y,
                   &creg_z);
    int creg_x = creg_z;
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась подветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
//	if (YesBit(GD.Hot.MeteoSens[cSmOutTSens].RCS,cbMinMaxVSens))
//		IntX=100;
//--------------------------------------------------------------------------------
//И по ветру
//--------------------------------------------------------------------------------
    creg_y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.f_StormWind - f_StartWind,
                   _GD.TuneClimate.f_StormWind,
                   100,
                   0,
                   creg_y,
                   &creg_z);
//В IntZ - ограничение по ветру

    if (creg_z > gh->tcontrol_tepl->OldPozUn)
    {
        gh->tcontrol_tepl->UnWindStorm = _GD.TuneClimate.f_WindHold;
        gh->tcontrol_tepl->OldPozUn = creg_z;
    }
    if (gh->tcontrol_tepl->UnWindStorm > 0)
    {
        gh->tcontrol_tepl->UnWindStorm--;
        creg_z = gh->tcontrol_tepl->OldPozUn;
    }

//	if (!(GD.TuneClimate.f_MaxWind)) IntZ=100;
    if (creg_x > 100)
    {
        creg_y = 100 - creg_z;
        creg_x -= 100;
    }
    else
    {
        creg_y = creg_x - creg_z;
        creg_x = 0;
    }
//	IntY=t_max-IntX-IntZ;

    creg_y = CLAMP(0, creg_y, t_max);

    gh->hot->Kontur[cSmWindowUnW].MaxCalc = creg_y; //pGD_TControl_Tepl->PrevMaxWinUnW;
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
    t_max = gh->gh_ctrl->f_MaxOpenOn;

    creg_y = -DefRH(gh); //pGD_Hot_Tepl->AllTask.NextRHAir-pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.f_max_RHStart,
                   _GD.TuneClimate.f_max_RHEnd,
                   _GD.TuneClimate.f_max_Cor,
                   0,
                   creg_y,
                   &creg_z);
    t_max = MIN(t_max, 100 - creg_z);

    if ((t_max > _GD.TuneClimate.f_MaxOpenRain) && (_GD.TControl.bSnow))
        t_max = _GD.TuneClimate.f_MaxOpenRain;

    creg_y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.f_StormWindOn - f_StartWind,
                   _GD.TuneClimate.f_StormWindOn,
                   100,
                   0,
                   creg_y,
                   &creg_z);

    if (creg_z < gh->tcontrol_tepl->OldPozOn)
    {
        gh->tcontrol_tepl->OnWindStorm = _GD.TuneClimate.f_WindHold;
        gh->tcontrol_tepl->OldPozOn = creg_z;
    }
    if (gh->tcontrol_tepl->OnWindStorm > 0)
    {
        gh->tcontrol_tepl->OnWindStorm--;
        creg_z = gh->tcontrol_tepl->OldPozOn;
    }
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась наветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
    creg_y = creg_x - creg_z;
    creg_y = CLAMP(0,  creg_y, t_max);

    gh->hot->Kontur[cSmWindowOnW].MaxCalc = creg_y; //pGD_TControl_Tepl->PrevMaxWinOnW;
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
    if (gh->hot->Kontur[cSmWindowOnW].Status < 20)
    {
        if (gh->tcontrol_tepl->Kontur[cSmWindowOnW].DoT / 10 == gh->hot->Kontur[cSmWindowOnW].MaxCalc)
            gh->hot->Kontur[cSmWindowOnW].Status = cSReachMax;

        if (gh->tcontrol_tepl->Kontur[cSmWindowOnW].DoT / 10 == gh->hot->Kontur[cSmWindowOnW].MinCalc)
            gh->hot->Kontur[cSmWindowOnW].Status = cSReachMin;
    }
    if (gh->hot->Kontur[cSmWindowUnW].Status < 20)
    {
        if (gh->tcontrol_tepl->Kontur[cSmWindowUnW].DoT / 10 ==  gh->hot->Kontur[cSmWindowUnW].MaxCalc)
            gh->hot->Kontur[cSmWindowUnW].Status = cSReachMax;
        if (gh->tcontrol_tepl->Kontur[cSmWindowUnW].DoT / 10 ==  gh->hot->Kontur[cSmWindowUnW].MinCalc)
            gh->hot->Kontur[cSmWindowUnW].Status = cSReachMin;
    }

    if ((!gh->tcontrol_tepl->Kontur[cHSmWinN + _GD.Hot.PozFluger].Separate)
        || (YesBit(gh->hand[cHSmWinN + _GD.Hot.PozFluger].RCS, cbManMech)))
    {
        gh->tcontrol_tepl->Kontur[cSmWindowOnW].DoT = 0;
        gh->hot->Kontur[cSmWindowOnW].MaxCalc = 0;
        gh->hot->Kontur[cSmWindowOnW].MinCalc = 0;
        SetBit(gh->hot->Kontur[cSmWindowOnW].RCS, cbNoWorkKontur);
        gh->hot->Kontur[cSmWindowOnW].Status = cSWHand;
    }
    if ((  ! gh->tcontrol_tepl->Kontur[cHSmWinN + 1 - _GD.Hot.PozFluger].Separate)
        || (YesBit(gh->hand[cHSmWinN + 1 - _GD.Hot.PozFluger].RCS, cbManMech)))
    {
        SetBit(gh->hot->Kontur[cSmWindowUnW].RCS, cbNoWorkKontur);
        gh->tcontrol_tepl->Kontur[cSmWindowUnW].DoT = 0;
        gh->hot->Kontur[cSmWindowUnW].MaxCalc = 0;
        gh->hot->Kontur[cSmWindowUnW].MinCalc = 0;
        gh->hot->Kontur[cSmWindowUnW].Status = cSWHand;
    }

    if (YesBit(gh->hot->discrete_inputs[0], cSmDVent))
    {
        gh->hot->Kontur[cSmWindowUnW].Status = cSAlrExternal;
        gh->hot->Kontur[cSmWindowOnW].Status = cSAlrExternal;
    }
    if (YesBit(gh->tcontrol_tepl->MechBusy[cHSmWinN + 1 - _GD.Hot.PozFluger].RCS,
               cMSAlarm))
        gh->hot->Kontur[cSmWindowUnW].Status = cSAlrNoCtrl;
    if (YesBit(gh->tcontrol_tepl->MechBusy[cHSmWinN + _GD.Hot.PozFluger].RCS,
               cMSAlarm))
        gh->hot->Kontur[cSmWindowOnW].Status = cSAlrNoCtrl;

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sInitKontur(const contour_t *ctr)
{
//	char tTepl1;
//------------------------------------------------------------------------
//Определяем работает может ли работать контур
//------------------------------------------------------------------------
    ctr->hot->Status = 0;
    if (! ctr->tcontrol->Separate)
    {
        SetNoWorkKontur(ctr);
        return;
    }
//------------------------------------------------------------------------
//Устанавливаем минимум и максимум
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Диагностика работы контура (выявляем ошибки не позволяющиее работать контуру)
//------------------------------------------------------------------------
    int byte_y = 0;
    ctr->hot->SensValue = ctr->tcontrol->SensValue;
    if (!ctr->link.gh_ctrl->c_MaxTPipe[ctr->cidx])
    {
        ctr->hot->Status = cSWNoMax;
        byte_y++;
    }
    if (YesBit(ctr->hand[cHSmPump].RCS, cbManMech) && (! ctr->hand[cHSmPump].Position))
    {
        ctr->hot->Status = cSWHand;
        byte_y++;
    }

    if (byte_y)
    {
        SetNoWorkKontur(ctr);
        return;
    }
//		{
//			SetNoWorkKontur();
//			return;
//		}
    if (!ctr->hot->SensValue)
    {
        ctr->hot->Status = cSAlrNoSens;
        SetBit(ctr->hot->RCS, cbNoSensKontur);
        SetNoWorkKontur(ctr);
        return;
    }
    __sMinMaxWater(ctr);

    gh_t main_gh = make_gh_ctx(ctr->tcontrol->MainTepl);
    contour_t main_contour = make_contour_ctx(&main_gh, ctr->cidx);

    ctr->hot->Optimal = main_contour.hot->Optimal;
    ctr->hot->MaxCalc = main_contour.hot->MaxCalc;
    ctr->hot->MinTask = main_contour.hot->MinTask;
    ctr->hot->MinCalc = main_contour.hot->MinCalc;

    ctr->tcontrol->DoT = main_contour.tcontrol->DoT;
    ctr->tcontrol->PumpStatus = main_contour.tcontrol->PumpStatus;

    //GD.TControl.Tepl[cSmTeplB].Kontur[fnKontur].SErr=GD.TControl.Tepl[cSmTeplA].Kontur[fnKontur].SErr;
    if (ctr->tcontrol->DoT)
        ctr->hot->Status = cSOn;

    if (YesBit(main_gh.hot->Kontur[ctr->cidx].ExtRCS, cbBlockPumpKontur))
    {
        ctr->hot->Status = cSBlPump;
        SetBit(ctr->hot->ExtRCS, cbBlockPumpKontur);
    }
    if (ctr->tcontrol->DoT / 10  ==  ctr->hot->MaxCalc)
        ctr->hot->Status = cSReachMax;

    if (ctr->tcontrol->DoT / 10  ==  ctr->hot->MinCalc)
        ctr->hot->Status = cSReachMin;

    if (ctr->hot->Do)
        ctr->hot->Status = cSFollowProg;

    if (ctr->hot->MinCalc > ctr->hot->MaxCalc - 100)
        ctr->hot->Status = cSWNoRange;
    if ((ctr->hot->SensValue < ctr->hot->MaxCalc - 50) && (ctr->hand->Position ==  100))
        ctr->hot->Status = cSWNoHeat;
    if (YesBit(ctr->hand->RCS, cbManMech))
        ctr->hot->Status = cSWHand;
    if (YesBit(ctr->link.hot->discrete_inputs[0], cSmDHeat))
        ctr->hot->Status = cSAlrExternal;

//	if (pGD_TControl_Tepl->Critery>0)
//		ClrBit(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].RCS,cbMinusCritery);
//	if (pGD_TControl_Tepl->Critery<0)
//		ClrBit(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].RCS,cbPlusCritery);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-*-*-*-*-*--Процедура определения нерегулируемости для контура-*-*-*-*-*/
/*************************************************************************/
void __sRegulKontur(const contour_t *ctr)
{
    const gh_t *gh = &ctr->link;

//------------------------------------------------------------------------
//Контура с выключенными насосами не участвуют в расчете или если не установлена аварийная граница
//------------------------------------------------------------------------
#ifdef DEMO
    ctr->tcontrol->SErr=0;
    return;
#endif
    if ((!ctr->tcontrol->PumpStatus))
    {
        ctr->tcontrol->SErr = 0;
        return;
    }
//------------------------------------------------------------------------
//Подсчет нерегулирумости для контура
//------------------------------------------------------------------------

    int creg_x = (ctr->tcontrol->DoT / 10) - ctr->tcontrol->SensValue;

    if (((creg_x < 0) && (creg_x > (-cErrKontur)))
        || ((creg_x < cErrKontur) && (creg_x > 0)))
        creg_x = 0;

    if ((! creg_x) || (! SameSign(creg_x, ctr->tcontrol->SErr))) //(LngX*((long)(pGD_TControl_Tepl_Kontur->SErr)))<0)))
        ctr->tcontrol->SErr = 0;
    ctr->tcontrol->SErr += creg_x;

//------------------------------------------------------------------------
//Если общий, то взять нерегулируемость из теплицы А
//------------------------------------------------------------------------
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbAndKontur))
    #warning "next line likely was wrong - condition is has no effect"
//  if ((&(ctr->tcontrol->SErr)) != (&_GD.TControl.Tepl[ctr->tcontrol->MainTepl].Kontur[ctr->cidx].SErr))
//      ;

    ctr->tcontrol->SErr = 0;
//------------------------------------------------------------------------
//Проверяем на контрольную границу
//------------------------------------------------------------------------
    if ((ctr->tcontrol->SErr >= v_ControlMidlWater)
        || (ctr->tcontrol->SErr <= -v_ControlMidlWater))
        SetBit(ctr->hot->ExtRCS, cbCtrlErrKontur);

//------------------------------------------------------------------------
//Проверяем на аварийную границу
//------------------------------------------------------------------------

    if (ctr->tcontrol->SErr >= v_AlarmMidlWater)
    {
        SetBit(ctr->hot->ExtRCS, cbAlarmErrKontur);
        ctr->tcontrol->SErr = v_AlarmMidlWater;
    }
    if (ctr->tcontrol->SErr <= -v_AlarmMidlWater)
    {
        SetBit(ctr->hot->ExtRCS, cbAlarmErrKontur);
        ctr->tcontrol->SErr = -v_AlarmMidlWater;
    }
//------------------------------------------------------------------------
//Устанавливаем нерегулируемость в процентах со знаком
//------------------------------------------------------------------------

    int creg_y = ctr->tcontrol->SErr;
    int creg_z;
    CorrectionRule(v_ControlMidlWater,
                   v_AlarmMidlWater,
                   100,
                   0,
                   creg_y,
                   &creg_z);

    ctr->hot->SError = (char) creg_z;
    if ((creg_z > cv_ResetMidlWater) && (YesBit(gh->tcontrol_tepl->MechBusy[ctr->cidx].RCS, cMSBlockRegs)))
        SetBit(ctr->hot->ExtRCS, cbResetErrKontur);
    creg_y = -creg_y;
    CorrectionRule(v_ControlMidlWater,
                   v_AlarmMidlWater,
                   100,
                   0,
                   creg_y,
                   &creg_z);
    ctr->hot->SError += (char) creg_z;
    if ((creg_z > cv_ResetMidlWater) && (YesBit(gh->tcontrol_tepl->MechBusy[ctr->cidx].RCS, cMSBlockRegs)))
        SetBit(ctr->hot->ExtRCS, cbResetErrKontur);

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
void __sPotentialPosibilityKontur(const contour_t *ctr, char fInv)
{
    int16_t *pRealPower = &(ctr->tcontrol->RealPower[fInv]);
//------------------------------------------------------------------------
//Приоритет по температуре воздуха в теплице
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Приоритет по влажности воздуха в теплице
//------------------------------------------------------------------------
    int creg_y = ctr->strategy->RHPower;
    int creg_x = -DefRH(&ctr->link);

    int creg_z = (int) (((long) creg_y) * creg_x / 1000);
//------------------------------------------------------------------------
//Приоритет по оптимальный температуре
//------------------------------------------------------------------------
    creg_x = 0;
    if (ctr->hot->Optimal)
    {
        creg_x = ctr->hot->Optimal - ctr->hot->Do;
//------------------------------------------------------------------------
//Если контур выключен то оптимальня температура сравнивается с минимумом контура
//------------------------------------------------------------------------
        creg_y = ctr->strategy->OptimalPower;
        creg_x = (int) (((long) creg_y * creg_x / 100));
    }

//------------------------------------------------------------------------
//Приоритет с экономичностью
//------------------------------------------------------------------------

    *pRealPower = ctr->strategy->TempPower + creg_x + creg_z;
    if ((*pRealPower) < 1)
        (*pRealPower) = 1;
    if (!fInv)
    {
        (*pRealPower) = 100 - ctr->strategy->TempPower - creg_x - creg_z;
        if ((*pRealPower) < 1)
            (*pRealPower) = 1;
    }
}

void __WorkableKontur(const contour_t *ctr)
{

    const gh_t *gh = &ctr->link;

//------------------------------------------------------------------------
//Внимание - нерегулируемость из-за ручного управления
//------------------------------------------------------------------------
    if (YesBit(ctr->hand[cHSmMixVal].RCS, cbManMech))
        SetBit(ctr->hot->ExtRCS, cbAlarmErrKontur);

    gh_t main_gh = make_gh_ctx(ctr->tcontrol->MainTepl);

    int creg_y = main_gh.hand[cHSmMixVal + ctr->cidx].Position;
    //------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
    if ((ctr->hot->Do < ctr->hot->MaxCalc)
#ifndef DEMO
        && (!(YesBit(ctr->hot->ExtRCS, cbAlarmErrKontur)))
#endif
        && (ctr->tcontrol->PumpStatus) && ((creg_y < 100)))
        SetBit(ctr->hot->ExtRCS, cbReadyRegUpKontur);
//------------------------------------------------------------------------
//Установить бит возможности работы насосом
//------------------------------------------------------------------------
#warning CHECK THIS
// NEW
    if ((!ctr->tcontrol->PumpStatus) && (gh->tcontrol_tepl->Critery > 0) && (ctr->cidx < cSmKontur5))
    {
        if ((_GD.Hot.MidlSR < _GD.TuneClimate.f_MinSun)
            && (gh->hot->AllTask.NextTAir
                - _GD.TControl.MeteoSensing[cSmOutTSens]
                > _GD.TuneClimate.f_DeltaOut)
            || ((getTempHeat(gh, gh->idx) - gh->hot->AllTask.DoTHeat) < 0)
            && (((gh->gh_ctrl->c_PFactor % 100) < 90)
                || (gh->tcontrol_tepl->StopVentI > 0)))
        {
            SetBit(ctr->hot->ExtRCS, cbReadyPumpKontur);
            if ((ctr->tcontrol->NAndKontur  ==  1)
                && (!gh->tcontrol_tepl->qMaxOwnKonturs))
                ctr->tcontrol->RealPower[1] += 100;
        }
    }
//------------------------------------------------------------------------
//Если надо охлаждать и включен насос
//------------------------------------------------------------------------
    if (ctr->tcontrol->PumpStatus)
    {
//------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
        if ((ctr->hot->Do > ctr->hot->MinCalc)
#ifndef DEMO
            && (!(YesBit(ctr->hot->ExtRCS, cbAlarmErrKontur)))
#endif
            && (creg_y > 0))
            SetBit(ctr->hot->ExtRCS, cbReadyRegDownKontur);
//------------------------------------------------------------------------
//Установить возможности работы насосом
//------------------------------------------------------------------------
        if ((!(YesBit(ctr->hot->ExtRCS, cbReadyRegDownKontur)))
            && (!(YesBit(ctr->hot->ExtRCS, cbBlockPumpKontur)))
            && (!ctr->tcontrol->PumpPause)
            && (gh->tcontrol_tepl->Critery < 0) && (! creg_y)
            && (_GD.TControl.MeteoSensing[cSmOutTSens] > 500))
            SetBit(ctr->hot->ExtRCS, cbReadyPumpKontur);
    }

    if ((YesBit(ctr->hot->ExtRCS, cbReadyRegDownKontur))
        && (gh->tcontrol_tepl->Critery < 0))
        SetBit(ctr->hot->ExtRCS, cbReadyRegsKontur);
    if ((YesBit(ctr->hot->ExtRCS, cbReadyRegUpKontur))
        && (gh->tcontrol_tepl->Critery > 0))
        SetBit(ctr->hot->ExtRCS, cbReadyRegsKontur);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-Процедура определения приоритетов с возможностями и подсчета их суммы-*/
/*************************************************************************/
void __sRealPosibilityKonturs(const contour_t *ctr, long* fMinMax)
{

    const gh_t *gh = &ctr->link;
//------------------------------------------------------------------------
//Если контур не участвует в управлении то пропускаем
//------------------------------------------------------------------------

    if (! YesBit(ctr->hot->ExtRCS, (cbReadyRegUpKontur + cbReadyPumpKontur)))
        ctr->tcontrol->RealPower[1] = 0;
    if (! YesBit(ctr->hot->ExtRCS, (cbReadyRegDownKontur + cbReadyPumpKontur)))
        ctr->tcontrol->RealPower[0] = 0;

    if (! YesBit(ctr->hot->ExtRCS, (cbReadyPumpKontur + cbReadyRegsKontur)))
        return; //ByteZ;

//------------------------------------------------------------------------
//Если надо охлаждать то меняем знак экономического приоритета для того
//чтобы он в положительной оси отображал желание рабоать, а в отрицательной - нежелание
//------------------------------------------------------------------------
//	if (pGD_TControl_Tepl->Critery<0)

//------------------------------------------------------------------------
//Меняем приоритеты регулирующих контуров в зависимости от нерегулируемости
//------------------------------------------------------------------------

    if (YesBit(ctr->hot->ExtRCS, cbReadyRegsKontur))
    {
        if (ctr->tcontrol->NAndKontur  ==  1)
        {
            gh->tcontrol_tepl->NOwnKonturs++;
        }
//------------------------------------------------------------------------
//Ищем максимальный приоритет контуров и запоминаем его номер
//------------------------------------------------------------------------
    }

    if (fMinMax[0] < ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower])
    {
        fMinMax[0] = ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower];
        gh->tcontrol_tepl->nMaxKontur = ctr->cidx;
    }
    if ((fMinMax[1] < ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower])
        && (!YesBit(ctr->hot->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[1] = ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower];
    }
    if ((fMinMax[2] < ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower])
        && (ctr->tcontrol->NAndKontur  ==  1)
        && (!YesBit(ctr->hot->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[2] = ctr->tcontrol->RealPower[gh->tcontrol_tepl->CurrPower];
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
long __sRaspKontur(const contour_t *ctr)
{
    const gh_t *gh = &ctr->link;

    if ((! gh->tcontrol_tepl->qMaxKonturs)
        || (!YesBit(ctr->hot->RCS, cbGoMax)))
        return 0;

    gh->tcontrol_tepl->StopI = 0;

    long long_y = gh->tcontrol_tepl->Critery;
    long_y = long_y / gh->tcontrol_tepl->qMaxKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / ctr->strategy->Powers;

    return long_y;
}

long __sRaspOwnKontur(const contour_t *ctr)
{
    const gh_t *gh = &ctr->link;

    if ((!gh->tcontrol_tepl->qMaxOwnKonturs) || (!YesBit(ctr->hot->RCS, cbGoMaxOwn)))
        return 0;
    long long_y = gh->tcontrol_tepl->Critery;
    long_y = long_y / gh->tcontrol_tepl->qMaxOwnKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / ctr->strategy->Powers;

    return long_y;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
long __TempToVent(const gh_t *gh)
{
    long long_y = 100 * ((long) gh->tcontrol_tepl->TVentCritery);
    long_y = long_y / 50;
    long_y = long_y * 30;
    long_y /= ((long) gh->tcontrol_tepl->f_Power);

    return long_y;
}
long __VentToTemp(const gh_t *gh, long sVent)
{
    long long_y = sVent * ((long) gh->tcontrol_tepl->f_Power);
    long_y /= 30;
    long_y *= 50; //((long)pGD_ConstMechanic->ConstMixVal[cHSmWinN].Power);
    return long_y / 100;
}

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки фрамуг--*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sLastCheckWindow(const gh_t *gh)
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

    MinOn = gh->hot->Kontur[cSmWindowOnW].MinCalc;
    MinUn = gh->hot->Kontur[cSmWindowUnW].MinCalc;
    MaxOn = gh->hot->Kontur[cSmWindowOnW].MaxCalc;
    MaxUn = gh->hot->Kontur[cSmWindowUnW].MaxCalc;

    int creg_y = gh->tcontrol_tepl->Kontur[cSmWindowUnW].CalcT;
    if (creg_y < 0)
        creg_y = 0;
    if (creg_y > _GD.TuneClimate.f_S1Level)
        tDo = _GD.TuneClimate.f_S1Level;
    else
        tDo = creg_y;
    creg_y -= _GD.TuneClimate.f_S1Level;
    creg_y *= _GD.TuneClimate.f_S1MinDelta;
    if (creg_y < 0)
        creg_y = 0;

    if (creg_y > _GD.TuneClimate.f_S2Level - _GD.TuneClimate.f_S1Level)
        tDo = _GD.TuneClimate.f_S2Level;
    else
        tDo += creg_y;
    creg_y -= _GD.TuneClimate.f_S2Level - _GD.TuneClimate.f_S1Level;
    creg_y = (creg_y * _GD.TuneClimate.f_S2MinDelta) / _GD.TuneClimate.f_S1MinDelta;
    if (creg_y < 0)
        creg_y = 0;

    if (creg_y > _GD.TuneClimate.f_S3Level - _GD.TuneClimate.f_S2Level)
        tDo = _GD.TuneClimate.f_S3Level;
    else
        tDo += creg_y;
    creg_y -= _GD.TuneClimate.f_S3Level - _GD.TuneClimate.f_S2Level;
    creg_y = (creg_y * _GD.TuneClimate.f_S3MinDelta) / _GD.TuneClimate.f_S2MinDelta;
    if (creg_y < 0)
        creg_y = 0;

    tDo += creg_y;

    creg_y = _GD.TuneClimate.f_DefOnUn;
//	if (MaxUn<IntY) IntY=MaxUn;

    if (tDo < creg_y)
    {
        DoUn = tDo;
        DoOn = 0;
    }
    else
    {
        tDo -= creg_y;
        DoUn = creg_y + tDo / 2;
        DoOn = tDo / 2;
    }
    if (DoUn > MaxUn)
        DoOn += DoUn - MaxUn;
    if (DoOn > MaxOn)
        DoUn += DoOn - MaxOn;

    tMaximum = MaxOn + MaxUn;
    tDo = (tMaximum - _GD.TuneClimate.f_S3Level) / _GD.TuneClimate.f_S3MinDelta;
    tSLevel = _GD.TuneClimate.f_S3Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += (tSLevel - _GD.TuneClimate.f_S2Level) / _GD.TuneClimate.f_S2MinDelta;
    tSLevel = _GD.TuneClimate.f_S2Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += (tSLevel - _GD.TuneClimate.f_S1Level) / _GD.TuneClimate.f_S1MinDelta;
    tSLevel = _GD.TuneClimate.f_S1Level;
    if (tDo < 0)
    {
        tDo = 0;
        tSLevel = tMaximum;
    }
    tDo += tSLevel;

    gh->tcontrol_tepl->AbsMaxVent = __VentToTemp(gh, tDo);

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


    creg_y = 0;
    if (! getTempVent(gh, gh->idx))
        creg_y = 0;
    else
        creg_y = getTempVent(gh, gh->idx) - gh->hot->AllTask.DoTVent;

    if (((DoUn  ==  MaxUn) && (DoOn  ==  MaxOn) && (creg_y > 0))
        || ((DoUn  ==  MinUn) && (DoOn  ==  MinOn) && (creg_y < 0)))
        gh->tcontrol_tepl->StopVentI++;
    else
        gh->tcontrol_tepl->StopVentI = 0;

    if (gh->tcontrol_tepl->StopVentI > cMaxStopI)
        gh->tcontrol_tepl->StopVentI = cMaxStopI;

    gh->tcontrol_tepl->Kontur[cSmWindowUnW].DoT = DoUn;
    gh->tcontrol_tepl->Kontur[cSmWindowOnW].DoT = DoOn;

    gh->hot->Kontur[cSmWindowUnW].Do = (DoUn);
    gh->hot->Kontur[cSmWindowOnW].Do = (DoOn);
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
long __sThisToFirst(const contour_t *ctr, int in)
{
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbScreenKontur)) return 0;
    return in * ctr->strategy->Powers / 50;
    /********************************************************************
     -----------Работа автонастройки временно приостановлена--------------
     *********************************************************************
     *1000/pGD_TControl_Tepl->sac_MomentMull)*1000/pGD_TControl_Tepl->sac_LongMull[0]);
     *********************************************************************/
}

/**************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки контура--*-*-*-*-*-*-*-*/
/**************************************************************************/
void __sLastCheckKontur(const contour_t *ctr)
{
    const gh_t *gh = &ctr->link;

    int OldDoT;
    long TempDo;

    if (YesBit(ctr->hot->ExtRCS, cbBlockPumpKontur))
        ctr->tcontrol->PumpStatus = 1;

    if (!ctr->tcontrol->PumpStatus)
    {
        /*		if(pGD_TControl_Tepl_Kontur->DoT)
         (*fnCorCritery)+=((int)(pGD_TControl_Tepl_Kontur->DoT/100)-GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10);*/
        ctr->tcontrol->DoT = 0;
        ctr->hot->Do = 0;
        return;

    }
    //SetBit(pGD_Hot_Tepl_Kontur->RCS,cbOnPumpKontur);

    if (ctr->tcontrol->Manual)
        return;

    if ((!SameSign(gh->tcontrol_tepl->Critery, ctr->tcontrol->SErr))
        && (YesBit(ctr->hot->ExtRCS, cbResetErrKontur)))
    {
        OldDoT = ctr->tcontrol->DoT;
        ctr->hot->Do = ((ctr->tcontrol->SensValue));
        ctr->hot->Do = CLAMP(ctr->hot->MinCalc, ctr->hot->Do, ctr->hot->MaxCalc);
        TempDo = ctr->hot->Do * 10;
        ctr->tcontrol->SErr = 0;
        gh->tcontrol_tepl->Integral -= __sThisToFirst(ctr, OldDoT - TempDo)
                                       * 100;
        //	pGD_TControl_Tepl->SaveIntegral-=__sThisToFirst((int)((OldDoT-TempDo)))*100;
        ctr->tcontrol->DoT = TempDo;
        return;
    }
    if ((!ctr->tcontrol->DoT) && (ctr->tcontrol->LastDoT < 5000))
        ctr->tcontrol->DoT = ctr->tcontrol->LastDoT;
    ctr->tcontrol->DoT += ctr->tcontrol->CalcT;
    ctr->hot->Do = ctr->tcontrol->DoT / 10;

    if (ctr->hot->Do <= ctr->hot->MinCalc)
    {
        ctr->tcontrol->DoT = ctr->hot->MinCalc * 10;
    }
    if (ctr->hot->Do >= ctr->hot->MaxCalc)
    {
        ctr->tcontrol->DoT = ctr->hot->MaxCalc * 10;
    }
//	if (pGD_Hot_Tepl_Kontur->Do<pGD_Hot_Tepl->AllTask.DoTHeat/10)
//		pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MinCalc;//GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10;

//	(*fnCorCritery)+=(pGD_Hot_Tepl_Kontur->Do-(pGD_TControl_Tepl_Kontur->DoT/10));
    ctr->hot->Do = (ctr->tcontrol->DoT / 10);

    ctr->tcontrol->LastDoT = ctr->tcontrol->DoT;

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int __sCalcTempKonturs(const gh_t *gh)
{
    int SumTemp = 0;
    for (int contour_idx = 0; contour_idx < cSWaterKontur - 2; contour_idx++)
    {
        const contour_t ctr = make_contour_ctx(gh, contour_idx);

        if (ctr.tcontrol->DoT)
        {
            SumTemp += __sThisToFirst(&ctr, ctr.tcontrol->DoT) - gh->hot->AllTask.DoTHeat;
        }
        else if ((ctr.tcontrol->LastDoT < 5000)
                 && (ctr.tcontrol->LastDoT > gh->hot->AllTask.DoTHeat))
        {
            SumTemp += __sThisToFirst(&ctr, ctr.tcontrol->LastDoT) - gh->hot->AllTask.DoTHeat;
        }
    }
    return SumTemp;
}

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sCalcKonturs(void)
{
    long temp;
    int OldCrit;
    char isFram;
    char gh_idx, tTepl;

    for (gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

//		if(!pGD_Hot_Tepl->AllTask.NextTAir) return;

//Определение направления нагрев или охлаждени
        gh.tcontrol_tepl->CurrPower = 0;
        if (gh.tcontrol_tepl->Critery > 0)
            gh.tcontrol_tepl->CurrPower = 1;
// Отключить, если нет задания
        if (! gh.hot->AllTask.DoTHeat)
            continue;
// Расчет минимумов и максимумов контуров
        for (int i = 0; i < cSWaterKontur; i++)
        {
            contour_t ctr = make_contour_ctx(&gh, i);
            __sInitKontur(&ctr);
        }
//		__sMinMaxScreen();
// Расчет минимумов и максимумов фрамуг
        __sMinMaxWindows(&gh);

    }

// Заново считаем
    for (int gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

        gh.tcontrol_tepl->nMaxKontur = -1;
        gh.tcontrol_tepl->NOwnKonturs = 0;
        if (!gh.hot->AllTask.DoTHeat)
            continue;

        long MinMaxPowerReg[3] = {0, 0, 0};

        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            contour_t ctr = make_contour_ctx(&gh, contour_idx);

            __sRegulKontur(&ctr);

            ctr.tcontrol->Manual = 0;
            if (YesBit(ctr.hot->RCS, cbNoWorkKontur))
                continue;

            if (ctr.hot->Do)
            {
                ctr.tcontrol->PumpStatus = 1;
                ctr.tcontrol->Manual = 1;
                ctr.tcontrol->DoT = ctr.hot->Do * 10;
                continue;
            }

            ctr.hot->Do = (ctr.tcontrol->DoT / 10);
            __sPotentialPosibilityKontur(&ctr, 0); //Приоритет в случае охлаждения
            __sPotentialPosibilityKontur(&ctr, 1); //Приоритет в случае нагрева

            __WorkableKontur(&ctr);

            __sRealPosibilityKonturs(&ctr, MinMaxPowerReg);
            ctr.hot->Priority = ctr.tcontrol->RealPower[gh.tcontrol_tepl->CurrPower];

            gh.tcontrol_tepl->PowMaxKonturs = MinMaxPowerReg[1];
            gh.tcontrol_tepl->PowOwnMaxKonturs = MinMaxPowerReg[2];
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

        gh.hot->Kontur[cSmWindowUnW].Optimal = gh.tcontrol_tepl->f_NMinDelta;

        gh.tcontrol_tepl->Kontur[cSmWindowUnW].CalcT = __TempToVent(&gh);

        __sLastCheckWindow(&gh);

        gh.tcontrol_tepl->qMaxKonturs = 0;
        gh.tcontrol_tepl->qMaxOwnKonturs = 0;

        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            contour_t ctr = make_contour_ctx(&gh, contour_idx);

            if (YesBit(ctr.hot->RCS, cbNoWorkKontur))
                continue;
            if ((ctr.tcontrol->RealPower[gh.tcontrol_tepl->CurrPower] >= gh.tcontrol_tepl->PowMaxKonturs)
                && (gh.tcontrol_tepl->PowMaxKonturs))
            {
                SetBit(ctr.hot->RCS, cbGoMax);
                gh.tcontrol_tepl->qMaxKonturs++;
            }
            if ((ctr.tcontrol->RealPower[gh.tcontrol_tepl->CurrPower]
                 >= gh.tcontrol_tepl->PowOwnMaxKonturs)
                && (gh.tcontrol_tepl->PowOwnMaxKonturs)
                && (ctr.tcontrol->NAndKontur  ==  1))
            {
                SetBit(ctr.hot->RCS, cbGoMaxOwn);
                gh.tcontrol_tepl->qMaxOwnKonturs++;
            }
        }
    }
    _GD.TControl.PrevPozFluger = _GD.Hot.PozFluger;

    //Очень длинный расчет, поэтому проверим измерения
//	CheckReadyMeasure();
    //------------------------------------------------
    for (gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

//		if (!pGD_TControl_Tepl->qMaxOwnKonturs)
//			pGD_TControl_Tepl->StopI=2;
        if (!gh.hot->AllTask.DoTHeat)
            continue;

        if ((gh.tcontrol_tepl->nMaxKontur >= 0)
            && (YesBit(
                      gh.hot->Kontur[gh.tcontrol_tepl->nMaxKontur].ExtRCS,
                      cbReadyPumpKontur))
            && (!YesBit(
                       gh.hot->Kontur[gh.tcontrol_tepl->nMaxKontur].RCS,
                       cbPumpChange)))
        {
            int byte_y = 0;
            gh.tcontrol_tepl->qMaxKonturs = 0;
            gh.tcontrol_tepl->qMaxOwnKonturs = 0;
            if (gh.tcontrol_tepl->Critery < 0)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(gh.tcontrol_tepl->Kontur[gh.tcontrol_tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;

                    if ((!(YesBit(
                                 _GD.Hot.Tepl[tTepl].Kontur[gh.tcontrol_tepl->nMaxKontur].ExtRCS,
                                 cbReadyPumpKontur)))
                        && ((!_GD.TControl.Tepl[tTepl].NOwnKonturs)
                            || (gh.tcontrol_tepl->NOwnKonturs)))
                        byte_y = 1;
                }
            }
            if (!byte_y)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(gh.tcontrol_tepl->Kontur[gh.tcontrol_tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;
                    _GD.TControl.Tepl[tTepl].Kontur[gh.tcontrol_tepl->nMaxKontur].PumpStatus =
                    1
                    - gh.tcontrol_tepl->Kontur[gh.tcontrol_tepl->nMaxKontur].PumpStatus;
                    _GD.TControl.Tepl[tTepl].Kontur[gh.tcontrol_tepl->nMaxKontur].PumpPause =
                    cPausePump;
                    SetBit(
                          _GD.Hot.Tepl[tTepl].Kontur[gh.tcontrol_tepl->nMaxKontur].RCS,
                          cbPumpChange);
                }
            }
        }

    }
//	CheckReadyMeasure();
    for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
    {
        for (gh_idx = 0; gh_idx < cSTepl; gh_idx++)
        {
            gh_t gh = make_gh_ctx(gh_idx);
            contour_t ctr = make_contour_ctx(&gh, contour_idx);

            gh.tcontrol_tepl->RealPower = 0;
            if (YesBit(ctr.hot->RCS, cbNoWorkKontur))
                continue;
            gh.tcontrol_tepl->NAndKontur =
            ctr.tcontrol->NAndKontur;

            gh_t main_gh = make_gh_ctx(ctr.tcontrol->MainTepl);
            main_gh.tcontrol_tepl->RealPower += __sRaspKontur(&ctr);

//			if ((!pGD_TControl_Tepl->NOwnKonturs)&&(pGD_TControl_Tepl_Kontur->RealPower))
//			{
//				pGD_TControl_Tepl->NAndKontur=1;
//				GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].RealPower=__sRaspKontur();
//				break;
//			}
        }
        for (gh_idx = 0; gh_idx < cSTepl; gh_idx++)
        {
            gh_t gh = make_gh_ctx(gh_idx);
            contour_t ctr = make_contour_ctx(&gh, contour_idx);

            if (YesBit(ctr.hot->RCS, cbNoWorkKontur))
                continue;
            if (ctr.tcontrol->NAndKontur  ==  1)
                continue;

            gh_t main_gh = make_gh_ctx(ctr.tcontrol->MainTepl);

            int long_y = main_gh.tcontrol_tepl->RealPower;
            int byte_w = main_gh.tcontrol_tepl->NAndKontur;
            long_y /= byte_w;

            OldCrit = gh.tcontrol_tepl->Critery;
            if (gh.tcontrol_tepl->NOwnKonturs)
                gh.tcontrol_tepl->Critery = gh.tcontrol_tepl->Critery
                                            - __sThisToFirst(&ctr, long_y);
            if (!SameSign(OldCrit, gh.tcontrol_tepl->Critery))
                gh.tcontrol_tepl->Critery = 0;
            ctr.tcontrol->CalcT = long_y;
            __sLastCheckKontur(&ctr);

            if ((gh.hot->MaxReqWater < ctr.hot->Do)
                && (contour_idx < cSWaterKontur))
                gh.hot->MaxReqWater = ctr.hot->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }

    }
//	CheckReadyMeasure();
    for (gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            gh_t gh = make_gh_ctx(gh_idx);
            contour_t ctr = make_contour_ctx(&gh, contour_idx);

            if (YesBit(ctr.hot->RCS, cbNoWorkKontur))
                continue;
            if (ctr.tcontrol->NAndKontur != 1)
                continue;
            ctr.tcontrol->CalcT = __sRaspOwnKontur(&ctr);
            __sLastCheckKontur(&ctr);
//			if ((((long)IntY)*pGD_TControl_Tepl->Critery)<0) IntY=0;
            if ((gh.hot->MaxReqWater < ctr.hot->Do)
                && (contour_idx < cSWaterKontur))
                gh.hot->MaxReqWater = ctr.hot->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }
        if ((!gh.tcontrol_tepl->NOwnKonturs))
            gh.tcontrol_tepl->StopI++;
        else
            gh.tcontrol_tepl->StopI = 0;
        if (gh.tcontrol_tepl->StopI > cMaxStopI)
            gh.tcontrol_tepl->StopI = cMaxStopI;
    }

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sMechScreen(void)
{
    for (int gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

        if (gh.tcontrol_tepl->Screen[0].Value)
        {
            gh.hot->OtherCalc.CorrScreen = gh.hot->Kontur[cSmScreen].Do; //IntZ
        }

        SetPosScreen(&gh, cTermHorzScr);
        SetPosScreen(&gh, cSunHorzScr);
        SetPosScreen(&gh, cTermVertScr1);
        SetPosScreen(&gh, cTermVertScr2);
        SetPosScreen(&gh, cTermVertScr3);
        SetPosScreen(&gh, cTermVertScr4);
    }
}

void __sMechWindows(void)
{
    int WindWin[2] =
    {
        cSWaterKontur + 1 - _GD.Hot.PozFluger,
        cSWaterKontur + _GD.Hot.PozFluger
    };

    for (int gh_idx = 0; gh_idx < cSTepl; gh_idx++)
    {
        gh_t gh = make_gh_ctx(gh_idx);

        for (int i = cSWaterKontur; i < cSWaterKontur + 2; i++)
        {
            #warning "there may be out of bounds access, no ?"
            int byte_z = i - cSWaterKontur;

            eKontur *Hot_Tepl_Kontur = &gh.hot->Kontur[i];
            eTControlKontur *TControl_Tepl_Kontur = &gh.tcontrol_tepl->Kontur[WindWin[byte_z]];
            eMechanic *Hot_Hand_Kontur = &gh.hand[cHSmMixVal + WindWin[byte_z]];

            if (TControl_Tepl_Kontur->TPause)
            {
                TControl_Tepl_Kontur->TPause--;
                TControl_Tepl_Kontur->TPause = MAX(TControl_Tepl_Kontur->TPause, 0);
                continue;
            }

            int creg_x = Hot_Tepl_Kontur->Do;
//			IntY=abs(IntZ-((int)(pGD_Hot_Hand_Kontur->Position)));

            if (YesBit(Hot_Hand_Kontur->RCS, cbManMech))
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

            Hot_Hand_Kontur->Position = (char) creg_x;

//			pGD_TControl_Tepl_Kontur->TPause=GD.TuneClimate.f_MinTime;
        }
        //pGD_Level_Tepl[cSmWinNSens][cSmUpAlarmLev]=pGD_Hot_Hand[cHSmWinN].Position+GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinNSens][cSmDownAlarmLev]=pGD_Hot_Hand[cHSmWinN].Position-GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinSSens][cSmUpAlarmLev]=pGD_Hot_Hand[cHSmWinS].Position+GD.TuneClimate.f_MaxAngle;
        //pGD_Level_Tepl[cSmWinSSens][cSmDownAlarmLev]=pGD_Hot_Hand[cHSmWinS].Position-GD.TuneClimate.f_MaxAngle;
    }
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void DecPumpPause(const gh_t *gh)
{
    for (int i = 0; i < cSWaterKontur; i++)
    {
        if ((gh->hand[cHSmMixVal + i].Position < 100)
            && (gh->hand[cHSmMixVal + i].Position > 0))
        {
            gh->tcontrol_tepl->Kontur[i].PumpPause = cPausePump;
            if (gh->tcontrol_tepl->Kontur[i].DoT > 4000)
                gh->tcontrol_tepl->Kontur[i].PumpPause += cPausePump;
        }
        if (gh->tcontrol_tepl->Kontur[i].PumpPause > 0)
        {
            gh->tcontrol_tepl->Kontur[i].PumpPause--;
        }
        else
        {
            gh->tcontrol_tepl->Kontur[i].PumpPause = 0;
        }
    }

}

