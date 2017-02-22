#define _FRIEND_OF_CONTROL_

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
    if ((!_GDP.Hot_Tepl->AllTask.DoRHAir)|| (!_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value)) return 0;
    return(_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value-_GDP.Hot_Tepl->AllTask.DoRHAir);

}

void SetNoWorkKontur(void)
{
    SetBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur);
    _GDCP.TControl_Tepl_Kontur->PumpStatus = 0;
    _GDCP.TControl_Tepl_Kontur->DoT = 0;
}

void __sMinMaxWater(char fnKontur)
{
//------------------------------------------------------------------------
//Заполняем минимальные и максимальные границы для контуров а также опт температуру
//------------------------------------------------------------------------
    _GDCP.Hot_Tepl_Kontur->MaxCalc = _GDP.Control_Tepl->c_MaxTPipe[fnKontur];
//------------------------------------------------------------------------
//Для контуров начиная с третьего берем минимальные границы и опт. темп. из параметров
//------------------------------------------------------------------------

//	if  ((GD.Hot.MidlSR<GD.TuneClimate.f_MinSun)&&(pGD_Hot_Tepl->AllTask.NextTAir-GD.TControl.OutTemp>GD.TuneClimate.f_DeltaOut))
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].ExtRCS,cbBlockPumpKontur);

    if (fnKontur  ==  cSmKontur4)
    {
        _GDCP.Hot_Tepl_Kontur->MinTask = _GDP.Control_Tepl->c_MinTPipe[1];
        _GDCP.Hot_Tepl_Kontur->Optimal = _GDP.Control_Tepl->c_OptimalTPipe[1];
        _GDCP.Hot_Tepl_Kontur->MinCalc = _GDCP.Hot_Tepl_Kontur->MinTask;
    }
//------------------------------------------------------------------------
//Если установлен минимум то насос должен быть всегда включен
//------------------------------------------------------------------------
    if ((_GDCP.Hot_Tepl_Kontur->MinTask)
        || (YesBit((*(_GDCP.Hot_Hand_Kontur + cHSmPump)).RCS, cbManMech)))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
//------------------------------------------------------------------------
//Ограничиваем минимум
//------------------------------------------------------------------------
    _GDCP.Hot_Tepl_Kontur->MinCalc = MAX(_GDCP.Hot_Tepl_Kontur->MinCalc, cMinAllKontur);
//------------------------------------------------------------------------
//В виду особенностей работы 5 контура для него отдельные установки
//------------------------------------------------------------------------
    if (_GDCP.Hot_Tepl_Kontur->MinCalc > _GDCP.Hot_Tepl_Kontur->MaxCalc)
    {
        _GDCP.Hot_Tepl_Kontur->MinCalc = _GDCP.Hot_Tepl_Kontur->MaxCalc;
    }
    if (fnKontur  ==  cSmKontur5)
    {

        if ((!(YesBit(_GDP.Hot_Tepl->InTeplSens[cSmGlassSens].RCS, cbMinMaxVSens)))
            && ((_GDP.Hot_Tepl->InTeplSens[cSmGlassSens].Value
                 < _GD.TuneClimate.c_DoMinIfGlass)))
        {
            creg.Y = _GDP.Hot_Tepl->InTeplSens[cSmGlassSens].Value;
            CorrectionRule(_GD.TuneClimate.c_DoMaxIfGlass,
                           _GD.TuneClimate.c_DoMinIfGlass,
                           _GDCP.Hot_Tepl_Kontur->MaxCalc - cMin5Kontur, 0);
            _GDCP.Hot_Tepl_Kontur->Do = _GDCP.Hot_Tepl_Kontur->MaxCalc - creg.Z;
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);

        }
        if (YesBit(_GD.TControl.bSnow, 0x02))
        {
            creg.Y = _GD.TControl.MeteoSensing[cSmOutTSens];
            CorrectionRule(_GD.TuneClimate.c_CriticalSnowOut, c_SnowIfOut,
                           _GDCP.Hot_Tepl_Kontur->MaxCalc - _GD.TuneClimate.c_MinIfSnow,
                           0);
            _GDCP.Hot_Tepl_Kontur->Do = _GDCP.Hot_Tepl_Kontur->MaxCalc - creg.Z;
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
        }

        if ((_GDP.TControl_Tepl->ScrExtraHeat) || (_GD.Hot.Util  ==  9))
        {
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
            //pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->MaxCalc*10);
            _GDCP.Hot_Tepl_Kontur->Do = _GDCP.Hot_Tepl_Kontur->MaxCalc;
        }
//		else
//		if  (pGD_TControl_Tepl->LastScrExtraHeat)
//			pGD_TControl_Tepl_Kontur->DoT=pGD_TControl_Tepl->TempStart5;

//		pGD_TControl_Tepl->LastScrExtraHeat=pGD_TControl_Tepl->ScrExtraHeat;

//ogrMin(&(pGD_Hot_Tepl_Kontur->MinCalc),cMin5Kontur);
        _GDCP.Hot_Tepl_Kontur->Do = MAX(_GDCP.Hot_Tepl_Kontur->Do, _GDCP.Hot_Tepl_Kontur->MinCalc);
        if (!YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur))
        {
            _GDCP.Hot_Tepl_Kontur->Do = 0;
            _GDCP.Hot_Tepl_Kontur->MaxCalc = 0;
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

    if ((_GDP.Hot_Tepl->AllTask.Win < 2)
        || (YesBit(_GDP.TControl_Tepl->Calorifer, 0x02))) //Добавлено для блокировки фрамуг кондиционером
    {
//--------------------------------------------------------------------------------
//Если установлен режим работы по минимому то минимум должен совпасть с максимумом
//--------------------------------------------------------------------------------
        if (_GDP.Hot_Tepl->AllTask.Win)
        {
            _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc =
            _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc;
            _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSFollowProg;
            _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSFollowProg;
        }
        else
        {
            memclr(&_GDP.Hot_Tepl->Kontur[cSmWindowUnW].Optimal,
                   sizeof(eKontur));
        }
//--------------------------------------------------------------------------------
//Стираем все накопленные данные
//--------------------------------------------------------------------------------
        _GDP.TControl_Tepl->Kontur[cSmWindowOnW].DoT = 0;
        _GDP.TControl_Tepl->Kontur[cSmWindowUnW].DoT = 0;
        return;
    }
    _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSOn;
    _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSOn;

    creg.Y = DefRH();
    CorrectionRule(_GD.TuneClimate.f_min_RHStart, _GD.TuneClimate.f_min_RHEnd,
                   ((int) _GD.TuneClimate.f_min_Cor), 0);
    _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc = MAX(_GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc, creg.Z);

    creg.Y = _GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(_GD.TuneClimate.f_StartCorrPow, _GD.TuneClimate.f_EndCorrPow,
                   (_GD.TuneClimate.f_PowFactor - 1000), 0);
    _GDP.TControl_Tepl->f_Power = _GD.TuneClimate.f_PowFactor - creg.Z;
    if (_GD.TuneClimate.f_PowFactor < 1000)
        _GDP.TControl_Tepl->f_Power = 1000;

    t_max = _GDP.Control_Tepl->f_MaxOpenUn;
    if ((t_max > _GD.TuneClimate.f_MaxOpenRain) && (_GD.TControl.bSnow))
    {
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSWRain;
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSWRain;

        t_max = _GD.TuneClimate.f_MaxOpenRain;
    }
    //--------------------------------------------------------------------------------
//Максимум рассчитываем по внешней температуре
//--------------------------------------------------------------------------------
    creg.Y = _GD.TControl.MeteoSensing[cSmOutTSens];
    CorrectionRule(_GD.TuneClimate.f_MinTFreeze,
                   _GD.TuneClimate.f_MinTFreeze + f_MaxTFreeze, 200, 0);
    creg.X = creg.Z;
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась подветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
//	if (YesBit(GD.Hot.MeteoSens[cSmOutTSens].RCS,cbMinMaxVSens))
//		IntX=100;
//--------------------------------------------------------------------------------
//И по ветру
//--------------------------------------------------------------------------------
    creg.Y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.f_StormWind - f_StartWind,
                   _GD.TuneClimate.f_StormWind, 100, 0);
//В IntZ - ограничение по ветру

    if (creg.Z > _GDP.TControl_Tepl->OldPozUn)
    {
        _GDP.TControl_Tepl->UnWindStorm = _GD.TuneClimate.f_WindHold;
        _GDP.TControl_Tepl->OldPozUn = creg.Z;
    }
    if (_GDP.TControl_Tepl->UnWindStorm > 0)
    {
        _GDP.TControl_Tepl->UnWindStorm--;
        creg.Z = _GDP.TControl_Tepl->OldPozUn;
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

    creg.Y = CLAMP(0, creg.Y, t_max);

    _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc = creg.Y; //pGD_TControl_Tepl->PrevMaxWinUnW;
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
    t_max = _GDP.Control_Tepl->f_MaxOpenOn;

    creg.Y = -DefRH(); //pGD_Hot_Tepl->AllTask.NextRHAir-pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.f_max_RHStart, _GD.TuneClimate.f_max_RHEnd,
                   ((int) _GD.TuneClimate.f_max_Cor), 0);
    t_max = MIN(t_max, 100 - creg.Z);

    if ((t_max > _GD.TuneClimate.f_MaxOpenRain) && (_GD.TControl.bSnow))
        t_max = _GD.TuneClimate.f_MaxOpenRain;

    creg.Y = _GD.Hot.MidlWind;
    CorrectionRule(_GD.TuneClimate.f_StormWindOn - f_StartWind,
                   _GD.TuneClimate.f_StormWindOn, 100, 0);

    if (creg.Z < _GDP.TControl_Tepl->OldPozOn)
    {
        _GDP.TControl_Tepl->OnWindStorm = _GD.TuneClimate.f_WindHold;
        _GDP.TControl_Tepl->OldPozOn = creg.Z;
    }
    if (_GDP.TControl_Tepl->OnWindStorm > 0)
    {
        _GDP.TControl_Tepl->OnWindStorm--;
        creg.Z = _GDP.TControl_Tepl->OldPozOn;
    }
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась наветренная сторона устанавливаем максимальный ветер в 0
//--------------------------------------------------------------------------------
    creg.Y = creg.X - creg.Z;
    creg.Y = CLAMP(0,  creg.Y, t_max);

    _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc = creg.Y; //pGD_TControl_Tepl->PrevMaxWinOnW;
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
    if (_GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status < 20)
    {
        if (_GDP.TControl_Tepl->Kontur[cSmWindowOnW].DoT / 10
             ==  _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc)
            _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSReachMax;

        if (_GDP.TControl_Tepl->Kontur[cSmWindowOnW].DoT / 10
             ==  _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc)
            _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSReachMin;
    }
    if (_GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status < 20)
    {
        if (_GDP.TControl_Tepl->Kontur[cSmWindowUnW].DoT / 10
             ==  _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc)
            _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSReachMax;
        if (_GDP.TControl_Tepl->Kontur[cSmWindowUnW].DoT / 10
             ==  _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc)
            _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSReachMin;
    }

    if ((!_GDP.TControl_Tepl->Kontur[cHSmWinN + _GD.Hot.PozFluger].Separate)
        || (YesBit((*(_GDP.Hot_Hand + cHSmWinN + _GD.Hot.PozFluger)).RCS,
                   cbManMech)))
    {
        _GDP.TControl_Tepl->Kontur[cSmWindowOnW].DoT = 0;
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc = 0;
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc = 0;
        SetBit(_GDP.Hot_Tepl->Kontur[cSmWindowOnW].RCS, cbNoWorkKontur);
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSWHand;
    }
    if ((!_GDP.TControl_Tepl->Kontur[cHSmWinN + 1 - _GD.Hot.PozFluger].Separate)
        || (YesBit((*(_GDP.Hot_Hand + cHSmWinN + 1 - _GD.Hot.PozFluger)).RCS,
                   cbManMech)))
    {
        SetBit(_GDP.Hot_Tepl->Kontur[cSmWindowUnW].RCS, cbNoWorkKontur);
        _GDP.TControl_Tepl->Kontur[cSmWindowUnW].DoT = 0;
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc = 0;
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc = 0;
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSWHand;
    }

    if (YesBit(_GDP.Hot_Tepl->discrete_inputs[0], cSmDVent))
    {
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSAlrExternal;
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSAlrExternal;
    }
    if (YesBit(_GDP.TControl_Tepl->MechBusy[cHSmWinN + 1 - _GD.Hot.PozFluger].RCS,
               cMSAlarm))
        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Status = cSAlrNoCtrl;
    if (YesBit(_GDP.TControl_Tepl->MechBusy[cHSmWinN + _GD.Hot.PozFluger].RCS,
               cMSAlarm))
        _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Status = cSAlrNoCtrl;

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
    _GDCP.Hot_Tepl_Kontur->Status = 0;
    if (!_GDCP.TControl_Tepl_Kontur->Separate)
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
    _GDCP.Hot_Tepl_Kontur->SensValue = _GDCP.TControl_Tepl_Kontur->SensValue;
    if (!_GDP.Control_Tepl->c_MaxTPipe[fnKontur])
    {
        _GDCP.Hot_Tepl_Kontur->Status = cSWNoMax;
        byte_y++;
    }
    if (((YesBit((*(_GDCP.Hot_Hand_Kontur + cHSmPump)).RCS, cbManMech))
         && (!((*(_GDCP.Hot_Hand_Kontur + cHSmPump)).Position))))
    {
        _GDCP.Hot_Tepl_Kontur->Status = cSWHand;
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
    if (!_GDCP.Hot_Tepl_Kontur->SensValue)
    {
        _GDCP.Hot_Tepl_Kontur->Status = cSAlrNoSens;
        SetBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoSensKontur);
        SetNoWorkKontur();
        return;
    }
    __sMinMaxWater(fnKontur);

    for (byte_y = 0; byte_y < 8; byte_y++)
        ((char*) (&(_GDCP.Hot_Tepl_Kontur->Optimal)))[byte_y] =
        ((char*) (&(_GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].Optimal)))[byte_y];
    _GDCP.TControl_Tepl_Kontur->DoT =
    _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].DoT;
    _GDCP.TControl_Tepl_Kontur->PumpStatus =
    _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].PumpStatus;
    //GD.TControl.Tepl[cSmTeplB].Kontur[fnKontur].SErr=GD.TControl.Tepl[cSmTeplA].Kontur[fnKontur].SErr;
    if (_GDCP.TControl_Tepl_Kontur->DoT)
        _GDCP.Hot_Tepl_Kontur->Status = cSOn;

    if (YesBit(
              _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].ExtRCS,
              cbBlockPumpKontur))
    {
        _GDCP.Hot_Tepl_Kontur->Status = cSBlPump;
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur);
    }
    if (_GDCP.TControl_Tepl_Kontur->DoT / 10  ==  _GDCP.Hot_Tepl_Kontur->MaxCalc)
        _GDCP.Hot_Tepl_Kontur->Status = cSReachMax;

    if (_GDCP.TControl_Tepl_Kontur->DoT / 10  ==  _GDCP.Hot_Tepl_Kontur->MinCalc)
        _GDCP.Hot_Tepl_Kontur->Status = cSReachMin;

    if (_GDCP.Hot_Tepl_Kontur->Do)
        _GDCP.Hot_Tepl_Kontur->Status = cSFollowProg;

    if (_GDCP.Hot_Tepl_Kontur->MinCalc > _GDCP.Hot_Tepl_Kontur->MaxCalc - 100)
        _GDCP.Hot_Tepl_Kontur->Status = cSWNoRange;
    if ((_GDCP.Hot_Tepl_Kontur->SensValue < _GDCP.Hot_Tepl_Kontur->MaxCalc - 50)
        && (_GDCP.Hot_Hand_Kontur->Position  ==  100))
        _GDCP.Hot_Tepl_Kontur->Status = cSWNoHeat;
    if (YesBit(_GDCP.Hot_Hand_Kontur->RCS, cbManMech))
        _GDCP.Hot_Tepl_Kontur->Status = cSWHand;
    if (YesBit(_GDP.Hot_Tepl->discrete_inputs[0], cSmDHeat))
        _GDCP.Hot_Tepl_Kontur->Status = cSAlrExternal;

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
    _GDCP.TControl_Tepl_Kontur->SErr=0;
    return;
#endif
    if ((!_GDCP.TControl_Tepl_Kontur->PumpStatus))
    {
        _GDCP.TControl_Tepl_Kontur->SErr = 0;
        return;
    }
//------------------------------------------------------------------------
//Подсчет нерегулирумости для контура
//------------------------------------------------------------------------

    creg.X = ((_GDCP.TControl_Tepl_Kontur->DoT / 10))
           - _GDCP.TControl_Tepl_Kontur->SensValue;
    if (((creg.X < 0) && (creg.X > (-cErrKontur)))
        || ((creg.X < cErrKontur) && (creg.X > 0)))
        creg.X = 0;

    if ((!creg.X) || (!SameSign(creg.X, _GDCP.TControl_Tepl_Kontur->SErr))) //(LngX*((long)(pGD_TControl_Tepl_Kontur->SErr)))<0)))
        _GDCP.TControl_Tepl_Kontur->SErr = 0;
    _GDCP.TControl_Tepl_Kontur->SErr += creg.X;

//------------------------------------------------------------------------
//Если общий, то взять нерегулируемость из теплицы А
//------------------------------------------------------------------------
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbAndKontur))
    if ((&(_GDCP.TControl_Tepl_Kontur->SErr))
        != (&_GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].SErr))
        ;
    _GDCP.TControl_Tepl_Kontur->SErr = 0;
//------------------------------------------------------------------------
//Проверяем на контрольную границу
//------------------------------------------------------------------------
    if ((_GDCP.TControl_Tepl_Kontur->SErr >= v_ControlMidlWater)
        || (_GDCP.TControl_Tepl_Kontur->SErr <= -v_ControlMidlWater))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbCtrlErrKontur);

//------------------------------------------------------------------------
//Проверяем на аварийную границу
//------------------------------------------------------------------------

    if (_GDCP.TControl_Tepl_Kontur->SErr >= v_AlarmMidlWater)
    {
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);
        _GDCP.TControl_Tepl_Kontur->SErr = v_AlarmMidlWater;
    }
    if (_GDCP.TControl_Tepl_Kontur->SErr <= -v_AlarmMidlWater)
    {
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);
        _GDCP.TControl_Tepl_Kontur->SErr = -v_AlarmMidlWater;
    }
//------------------------------------------------------------------------
//Устанавливаем нерегулируемость в процентах со знаком
//------------------------------------------------------------------------

    creg.Y = _GDCP.TControl_Tepl_Kontur->SErr;
    CorrectionRule(v_ControlMidlWater, v_AlarmMidlWater, 100, 0);
    _GDCP.Hot_Tepl_Kontur->SError = (char) creg.Z;
    if ((creg.Z > cv_ResetMidlWater)
        && (YesBit(_GDP.TControl_Tepl->MechBusy[fnKontur].RCS, cMSBlockRegs)))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur);
    creg.Y = -creg.Y;
    CorrectionRule(v_ControlMidlWater, v_AlarmMidlWater, 100, 0);
    _GDCP.Hot_Tepl_Kontur->SError += (char) creg.Z;
    if ((creg.Z > cv_ResetMidlWater)
        && (YesBit(_GDP.TControl_Tepl->MechBusy[fnKontur].RCS, cMSBlockRegs)))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur);

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
void __sPotentialPosibilityKontur(char fInv, const eStrategy *strategy)
{
    int16_t *pRealPower;
    pRealPower = &(_GDCP.TControl_Tepl_Kontur->RealPower[fInv]);
//------------------------------------------------------------------------
//Приоритет по температуре воздуха в теплице
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Приоритет по влажности воздуха в теплице
//------------------------------------------------------------------------
    creg.Y = strategy->RHPower;
    creg.X = -DefRH();

    creg.Z = (int) (((long) creg.Y) * creg.X / 1000);
//------------------------------------------------------------------------
//Приоритет по оптимальный температуре
//------------------------------------------------------------------------
    creg.X = 0;
    if (_GDCP.Hot_Tepl_Kontur->Optimal)
    {
        creg.X = (_GDCP.Hot_Tepl_Kontur->Optimal - _GDCP.Hot_Tepl_Kontur->Do);
//------------------------------------------------------------------------
//Если контур выключен то оптимальня температура сравнивается с минимумом контура
//------------------------------------------------------------------------
        creg.Y = strategy->OptimalPower;
        creg.X = (int) (((long) creg.Y * creg.X / 100));
    }

//------------------------------------------------------------------------
//Приоритет с экономичностью
//------------------------------------------------------------------------

    *pRealPower = strategy->TempPower + creg.X + creg.Z;
    if ((*pRealPower) < 1)
        (*pRealPower) = 1;
    if (!fInv)
    {
        (*pRealPower) = 100 - strategy->TempPower - creg.X - creg.Z;
        if ((*pRealPower) < 1)
            (*pRealPower) = 1;
    }
}

void __WorkableKontur(char fnKontur, char fnTepl)
{

//------------------------------------------------------------------------
//Внимание - нерегулируемость из-за ручного управления
//------------------------------------------------------------------------
    if (YesBit((*(_GDCP.Hot_Hand_Kontur + cHSmMixVal)).RCS,/*(cbNoMech+*/
               cbManMech))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur);

    creg.Y = _GD.Hot.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].HandCtrl[cHSmMixVal
                                                                    + fnKontur].Position;
    //------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
    if ((_GDCP.Hot_Tepl_Kontur->Do < _GDCP.Hot_Tepl_Kontur->MaxCalc)
#ifndef DEMO
        && (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur)))
#endif
        && (_GDCP.TControl_Tepl_Kontur->PumpStatus) && ((creg.Y < 100)))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegUpKontur);
//------------------------------------------------------------------------
//Установить бит возможности работы насосом
//------------------------------------------------------------------------
#warning CHECK THIS
// NEW
    if ((!_GDCP.TControl_Tepl_Kontur->PumpStatus)
        && (_GDP.TControl_Tepl->Critery > 0) && (fnKontur < cSmKontur5))
    {
        if ((_GD.Hot.MidlSR < _GD.TuneClimate.f_MinSun)
            && (_GDP.Hot_Tepl->AllTask.NextTAir
                - _GD.TControl.MeteoSensing[cSmOutTSens]
                > _GD.TuneClimate.f_DeltaOut)
            || ((getTempHeat(fnTepl) - _GDP.Hot_Tepl->AllTask.DoTHeat) < 0)
            && (((_GDP.Control_Tepl->c_PFactor % 100) < 90)
                || (_GDP.TControl_Tepl->StopVentI > 0)))
        {
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur);
            if ((_GDCP.TControl_Tepl_Kontur->NAndKontur  ==  1)
                && (!_GDP.TControl_Tepl->qMaxOwnKonturs))
                _GDCP.TControl_Tepl_Kontur->RealPower[1] += 100;
        }
    }
//------------------------------------------------------------------------
//Если надо охлаждать и включен насос
//------------------------------------------------------------------------
    if (_GDCP.TControl_Tepl_Kontur->PumpStatus)
    {
//------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
        if ((_GDCP.Hot_Tepl_Kontur->Do > _GDCP.Hot_Tepl_Kontur->MinCalc)
#ifndef DEMO
            && (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbAlarmErrKontur)))
#endif
            && (creg.Y > 0))
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur);
//------------------------------------------------------------------------
//Установить возможности работы насосом
//------------------------------------------------------------------------
        if ((!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur)))
            && (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur)))
            && (!_GDCP.TControl_Tepl_Kontur->PumpPause)
            && (_GDP.TControl_Tepl->Critery < 0) && (!creg.Y)
            && (_GD.TControl.MeteoSensing[cSmOutTSens] > 500))
            SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur);
    }

    if ((YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegDownKontur))
        && (_GDP.TControl_Tepl->Critery < 0))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur);
    if ((YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegUpKontur))
        && (_GDP.TControl_Tepl->Critery > 0))
        SetBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur);

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

    if (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS,
                 (cbReadyRegUpKontur + cbReadyPumpKontur))))
        _GDCP.TControl_Tepl_Kontur->RealPower[1] = 0;
    if (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS,
                 (cbReadyRegDownKontur + cbReadyPumpKontur))))
        _GDCP.TControl_Tepl_Kontur->RealPower[0] = 0;

    if (!(YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS,
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

    if (YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyRegsKontur))
    {
        if (_GDCP.TControl_Tepl_Kontur->NAndKontur  ==  1)
        {
            _GDP.TControl_Tepl->NOwnKonturs++;
        }
//------------------------------------------------------------------------
//Ищем максимальный приоритет контуров и запоминаем его номер
//------------------------------------------------------------------------
    }

    if (fMinMax[0]
        < _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower])
    {
        fMinMax[0] =
        _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower];
        _GDP.TControl_Tepl->nMaxKontur = fnKontur;
    }
    if ((fMinMax[1]
         < _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower])
        && (!YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[1] =
        _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower];
    }
    if ((fMinMax[2]
         < _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower])
        && (_GDCP.TControl_Tepl_Kontur->NAndKontur  ==  1)
        && (!YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbReadyPumpKontur)))
    {
        fMinMax[2] =
        _GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower];

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
long __sRaspKontur(const eStrategy *strategy)
{
    if ((!_GDP.TControl_Tepl->qMaxKonturs)
        || (!YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbGoMax)))
        return 0;

    _GDP.TControl_Tepl->StopI = 0;

    long long_y = _GDP.TControl_Tepl->Critery;
    long_y = long_y / _GDP.TControl_Tepl->qMaxKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / strategy->Powers;

    return long_y;
}

long __sRaspOwnKontur(const eStrategy *strategy)
{
    if ((!_GDP.TControl_Tepl->qMaxOwnKonturs)
        || (!YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbGoMaxOwn)))
        return 0;
    long long_y = _GDP.TControl_Tepl->Critery;
    long_y = long_y / _GDP.TControl_Tepl->qMaxOwnKonturs;

    long_y = long_y * 50; //((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
    long_y = long_y / strategy->Powers;

    return long_y;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
long __TempToVent(void)
{
    long long_y = 100 * ((long) _GDP.TControl_Tepl->TVentCritery);
    long_y = long_y / 50;
    long_y = long_y * 30;
    long_y /= ((long) _GDP.TControl_Tepl->f_Power);

    return long_y;
}
long __VentToTemp(long sVent)
{
    long long_y = sVent * ((long) _GDP.TControl_Tepl->f_Power);
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

    MinOn = _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MinCalc;
    MinUn = _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MinCalc;
    MaxOn = _GDP.Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc;
    MaxUn = _GDP.Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc;

    creg.Y = _GDP.TControl_Tepl->Kontur[cSmWindowUnW].CalcT;
    if (creg.Y < 0)
        creg.Y = 0;
    if (creg.Y > _GD.TuneClimate.f_S1Level)
        tDo = _GD.TuneClimate.f_S1Level;
    else
        tDo = creg.Y;
    creg.Y -= _GD.TuneClimate.f_S1Level;
    creg.Y *= _GD.TuneClimate.f_S1MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    if (creg.Y > _GD.TuneClimate.f_S2Level - _GD.TuneClimate.f_S1Level)
        tDo = _GD.TuneClimate.f_S2Level;
    else
        tDo += creg.Y;
    creg.Y -= _GD.TuneClimate.f_S2Level - _GD.TuneClimate.f_S1Level;
    creg.Y = (creg.Y * _GD.TuneClimate.f_S2MinDelta) / _GD.TuneClimate.f_S1MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    if (creg.Y > _GD.TuneClimate.f_S3Level - _GD.TuneClimate.f_S2Level)
        tDo = _GD.TuneClimate.f_S3Level;
    else
        tDo += creg.Y;
    creg.Y -= _GD.TuneClimate.f_S3Level - _GD.TuneClimate.f_S2Level;
    creg.Y = (creg.Y * _GD.TuneClimate.f_S3MinDelta) / _GD.TuneClimate.f_S2MinDelta;
    if (creg.Y < 0)
        creg.Y = 0;

    tDo += creg.Y;

    creg.Y = _GD.TuneClimate.f_DefOnUn;
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

    _GDP.TControl_Tepl->AbsMaxVent = __VentToTemp(tDo);

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
        creg.Y = getTempVent(fnTepl) - _GDP.Hot_Tepl->AllTask.DoTVent;

    if (((DoUn  ==  MaxUn) && (DoOn  ==  MaxOn) && (creg.Y > 0))
        || ((DoUn  ==  MinUn) && (DoOn  ==  MinOn) && (creg.Y < 0)))
        _GDP.TControl_Tepl->StopVentI++;
    else
        _GDP.TControl_Tepl->StopVentI = 0;

    if (_GDP.TControl_Tepl->StopVentI > cMaxStopI)
        _GDP.TControl_Tepl->StopVentI = cMaxStopI;

    _GDP.TControl_Tepl->Kontur[cSmWindowUnW].DoT = DoUn;
    _GDP.TControl_Tepl->Kontur[cSmWindowOnW].DoT = DoOn;

    _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Do = (DoUn);
    _GDP.Hot_Tepl->Kontur[cSmWindowOnW].Do = (DoOn);
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
long __sThisToFirst(int in, const eStrategy *strategy)
{
//	if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbScreenKontur)) return 0;
    return in * strategy->Powers / 50;
    /********************************************************************
     -----------Работа автонастройки временно приостановлена--------------
     *********************************************************************
     *1000/pGD_TControl_Tepl->sac_MomentMull)*1000/pGD_TControl_Tepl->sac_LongMull[0]);
     *********************************************************************/
}

/**************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки контура--*-*-*-*-*-*-*-*/
/**************************************************************************/
void __sLastCheckKontur(char fnKontur, const eStrategy *strategy)
{
    int OldDoT;
    long TempDo;

    if (YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbBlockPumpKontur))
        _GDCP.TControl_Tepl_Kontur->PumpStatus = 1;

    if (!_GDCP.TControl_Tepl_Kontur->PumpStatus)
    {
        /*		if(pGD_TControl_Tepl_Kontur->DoT)
         (*fnCorCritery)+=((int)(pGD_TControl_Tepl_Kontur->DoT/100)-GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10);*/
        _GDCP.TControl_Tepl_Kontur->DoT = 0;
        _GDCP.Hot_Tepl_Kontur->Do = 0;
        return;

    }
    //SetBit(pGD_Hot_Tepl_Kontur->RCS,cbOnPumpKontur);

    if (_GDCP.TControl_Tepl_Kontur->Manual)
        return;

    if ((!SameSign(_GDP.TControl_Tepl->Critery, _GDCP.TControl_Tepl_Kontur->SErr))
        && (YesBit(_GDCP.Hot_Tepl_Kontur->ExtRCS, cbResetErrKontur)))
    {
        OldDoT = _GDCP.TControl_Tepl_Kontur->DoT;
        _GDCP.Hot_Tepl_Kontur->Do = ((_GDCP.TControl_Tepl_Kontur->SensValue));
        _GDCP.Hot_Tepl_Kontur->Do = CLAMP(_GDCP.Hot_Tepl_Kontur->MinCalc, _GDCP.Hot_Tepl_Kontur->Do, _GDCP.Hot_Tepl_Kontur->MaxCalc);
        TempDo = _GDCP.Hot_Tepl_Kontur->Do * 10;
        _GDCP.TControl_Tepl_Kontur->SErr = 0;
        _GDP.TControl_Tepl->Integral -= __sThisToFirst((int) ((OldDoT - TempDo)), strategy)
                                       * 100;
        //	pGD_TControl_Tepl->SaveIntegral-=__sThisToFirst((int)((OldDoT-TempDo)))*100;
        _GDCP.TControl_Tepl_Kontur->DoT = TempDo;
        return;
    }
    if ((!_GDCP.TControl_Tepl_Kontur->DoT)
        && (_GDCP.TControl_Tepl_Kontur->LastDoT < 5000))
        _GDCP.TControl_Tepl_Kontur->DoT = _GDCP.TControl_Tepl_Kontur->LastDoT;
    _GDCP.TControl_Tepl_Kontur->DoT = _GDCP.TControl_Tepl_Kontur->DoT
                                    + (int) (_GDCP.TControl_Tepl_Kontur->CalcT);
    _GDCP.Hot_Tepl_Kontur->Do = (_GDCP.TControl_Tepl_Kontur->DoT / 10);

    if (_GDCP.Hot_Tepl_Kontur->Do <= _GDCP.Hot_Tepl_Kontur->MinCalc)
    {
        _GDCP.TControl_Tepl_Kontur->DoT = (((_GDCP.Hot_Tepl_Kontur->MinCalc)))
                                        * 10;
    }
    if (_GDCP.Hot_Tepl_Kontur->Do >= _GDCP.Hot_Tepl_Kontur->MaxCalc)
    {
        _GDCP.TControl_Tepl_Kontur->DoT = (((_GDCP.Hot_Tepl_Kontur->MaxCalc)))
                                        * 10;
    }
//	if (pGD_Hot_Tepl_Kontur->Do<pGD_Hot_Tepl->AllTask.DoTHeat/10)
//		pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MinCalc;//GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10;

//	(*fnCorCritery)+=(pGD_Hot_Tepl_Kontur->Do-(pGD_TControl_Tepl_Kontur->DoT/10));
    _GDCP.Hot_Tepl_Kontur->Do = (_GDCP.TControl_Tepl_Kontur->DoT / 10);

    _GDCP.TControl_Tepl_Kontur->LastDoT = _GDCP.TControl_Tepl_Kontur->DoT;

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int __sCalcTempKonturs(void)
{
    int SumTemp = 0;
    for (int contour_idx = 0; contour_idx < cSWaterKontur - 2; contour_idx++)
    {
        SetPointersOnKontur(contour_idx);
        const eStrategy *strategy = &_GDP.Strategy_Tepl[contour_idx];

        if (_GDCP.TControl_Tepl_Kontur->DoT)
        {
            SumTemp += __sThisToFirst(_GDCP.TControl_Tepl_Kontur->DoT, strategy) - _GDP.Hot_Tepl->AllTask.DoTHeat;
        }
        else if (   (_GDCP.TControl_Tepl_Kontur->LastDoT < 5000)
                 && (_GDCP.TControl_Tepl_Kontur->LastDoT > _GDP.Hot_Tepl->AllTask.DoTHeat))
        {
            SumTemp += __sThisToFirst(_GDCP.TControl_Tepl_Kontur->LastDoT, strategy) - _GDP.Hot_Tepl->AllTask.DoTHeat;
        }
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
        _GDP.TControl_Tepl->CurrPower = 0;
        if (_GDP.TControl_Tepl->Critery > 0)
            _GDP.TControl_Tepl->CurrPower = 1;
// Отключить, если нет задания
        if (!_GDP.Hot_Tepl->AllTask.DoTHeat)
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
        _GDP.TControl_Tepl->nMaxKontur = -1;
        _GDP.TControl_Tepl->NOwnKonturs = 0;
        if (!_GD.Hot.Tepl[fnTepl].AllTask.DoTHeat)
            continue;
        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            SetPointersOnKontur(contour_idx);
            const eStrategy *strategy = &_GDP.Strategy_Tepl[contour_idx];

            __sRegulKontur(contour_idx);
            _GDCP.TControl_Tepl_Kontur->Manual = 0;
            if (YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (_GDCP.Hot_Tepl_Kontur->Do)
            {
                _GDCP.TControl_Tepl_Kontur->PumpStatus = 1;
                _GDCP.TControl_Tepl_Kontur->Manual = 1;
                _GDCP.TControl_Tepl_Kontur->DoT = (_GDCP.Hot_Tepl_Kontur->Do * 10);
                continue;
            }
            _GDCP.Hot_Tepl_Kontur->Do = (_GDCP.TControl_Tepl_Kontur->DoT / 10);
            __sPotentialPosibilityKontur(0, strategy); //Приоритет в случае охлаждения
            __sPotentialPosibilityKontur(1, strategy); //Приоритет в случае нагрева

            __WorkableKontur(contour_idx, fnTepl);
            __sRealPosibilityKonturs(contour_idx, MinMaxPowerReg);
            _GDCP.Hot_Tepl_Kontur->Priority =
            (int) (_GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower]);
            _GDP.TControl_Tepl->PowMaxKonturs = MinMaxPowerReg[1];
            _GDP.TControl_Tepl->PowOwnMaxKonturs = MinMaxPowerReg[2];
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

        _GDP.Hot_Tepl->Kontur[cSmWindowUnW].Optimal =
        _GDP.TControl_Tepl->f_NMinDelta;

        _GDP.TControl_Tepl->Kontur[cSmWindowUnW].CalcT = __TempToVent();

        __sLastCheckWindow(fnTepl);

        _GDP.TControl_Tepl->qMaxKonturs = 0;
        _GDP.TControl_Tepl->qMaxOwnKonturs = 0;

        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            SetPointersOnKontur(contour_idx);
            if (YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if ((_GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower]
                 >= _GDP.TControl_Tepl->PowMaxKonturs)
                && (_GDP.TControl_Tepl->PowMaxKonturs))
            {
                SetBit(_GDCP.Hot_Tepl_Kontur->RCS, cbGoMax);
                _GDP.TControl_Tepl->qMaxKonturs++;
            }
            if ((_GDCP.TControl_Tepl_Kontur->RealPower[_GDP.TControl_Tepl->CurrPower]
                 >= _GDP.TControl_Tepl->PowOwnMaxKonturs)
                && (_GDP.TControl_Tepl->PowOwnMaxKonturs)
                && (_GDCP.TControl_Tepl_Kontur->NAndKontur  ==  1))
            {
                SetBit(_GDCP.Hot_Tepl_Kontur->RCS, cbGoMaxOwn);
                _GDP.TControl_Tepl->qMaxOwnKonturs++;
            }
        }
    }
    _GD.TControl.PrevPozFluger = _GD.Hot.PozFluger;

    //Очень длинный расчет, поэтому проверим измерения
//	CheckReadyMeasure();
    //------------------------------------------------
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);

//		if (!pGD_TControl_Tepl->qMaxOwnKonturs)
//			pGD_TControl_Tepl->StopI=2;
        if (!_GDP.Hot_Tepl->AllTask.DoTHeat)
            continue;

        if ((_GDP.TControl_Tepl->nMaxKontur >= 0)
            && (YesBit(
                      _GDP.Hot_Tepl->Kontur[_GDP.TControl_Tepl->nMaxKontur].ExtRCS,
                      cbReadyPumpKontur))
            && (!YesBit(
                       _GDP.Hot_Tepl->Kontur[_GDP.TControl_Tepl->nMaxKontur].RCS,
                       cbPumpChange)))
        {
            int byte_y = 0;
            _GDP.TControl_Tepl->qMaxKonturs = 0;
            _GDP.TControl_Tepl->qMaxOwnKonturs = 0;
            if (_GDP.TControl_Tepl->Critery < 0)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(_GDP.TControl_Tepl->Kontur[_GDP.TControl_Tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;

                    if ((!(YesBit(
                                 _GD.Hot.Tepl[tTepl].Kontur[_GDP.TControl_Tepl->nMaxKontur].ExtRCS,
                                 cbReadyPumpKontur)))
                        && ((!_GD.TControl.Tepl[tTepl].NOwnKonturs)
                            || (_GDP.TControl_Tepl->NOwnKonturs)))
                        byte_y = 1;
                }
            }
            if (!byte_y)
            {
                for (tTepl = 0; tTepl < cSTepl; tTepl++)
                {
                    if (!(_GDP.TControl_Tepl->Kontur[_GDP.TControl_Tepl->nMaxKontur].Separate
                          & (1 << tTepl)))
                        continue;
                    _GD.TControl.Tepl[tTepl].Kontur[_GDP.TControl_Tepl->nMaxKontur].PumpStatus =
                    1
                    - _GDP.TControl_Tepl->Kontur[_GDP.TControl_Tepl->nMaxKontur].PumpStatus;
                    _GD.TControl.Tepl[tTepl].Kontur[_GDP.TControl_Tepl->nMaxKontur].PumpPause =
                    cPausePump;
                    SetBit(
                          _GD.Hot.Tepl[tTepl].Kontur[_GDP.TControl_Tepl->nMaxKontur].RCS,
                          cbPumpChange);
                }
            }
        }

    }
//	CheckReadyMeasure();
    for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
    {

        for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
        {
            SetPointersOnTepl(fnTepl);
            SetPointersOnKontur(contour_idx);
            const eStrategy *strategy = &_GDP.Strategy_Tepl[contour_idx];

            _GDP.TControl_Tepl->RealPower = 0;
            if (YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            _GDP.TControl_Tepl->NAndKontur =
            _GDCP.TControl_Tepl_Kontur->NAndKontur;
            _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].RealPower +=
            __sRaspKontur(strategy);
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
            SetPointersOnKontur(contour_idx);
            const eStrategy *strategy = &_GDP.Strategy_Tepl[contour_idx];

            if (YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (_GDCP.TControl_Tepl_Kontur->NAndKontur  ==  1)
                continue;
            long long_y = _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].RealPower;
            int8_t byte_w = _GD.TControl.Tepl[_GDCP.TControl_Tepl_Kontur->MainTepl].NAndKontur;
            long_y /= byte_w;

            OldCrit = _GDP.TControl_Tepl->Critery;
            if (_GDP.TControl_Tepl->NOwnKonturs)
                _GDP.TControl_Tepl->Critery = _GDP.TControl_Tepl->Critery
                                             - __sThisToFirst((int) ((long_y)), strategy);
            if (!SameSign(OldCrit, _GDP.TControl_Tepl->Critery))
                _GDP.TControl_Tepl->Critery = 0;
            _GDCP.TControl_Tepl_Kontur->CalcT = long_y;
            __sLastCheckKontur(contour_idx, strategy);
            creg.Y = 0;

            if ((_GDP.Hot_Tepl->MaxReqWater < _GDCP.Hot_Tepl_Kontur->Do)
                && (contour_idx < cSWaterKontur))
                _GDP.Hot_Tepl->MaxReqWater = _GDCP.Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }

    }
//	CheckReadyMeasure();
    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);

        for (int contour_idx = 0; contour_idx < cSWaterKontur; contour_idx++)
        {
            SetPointersOnKontur(contour_idx);
            const eStrategy *strategy = &_GDP.Strategy_Tepl[contour_idx];

            if (YesBit(_GDCP.Hot_Tepl_Kontur->RCS, cbNoWorkKontur))
                continue;
            if (_GDCP.TControl_Tepl_Kontur->NAndKontur != 1)
                continue;
            _GDCP.TControl_Tepl_Kontur->CalcT = __sRaspOwnKontur(strategy);
            __sLastCheckKontur(contour_idx, strategy);
            creg.Y = 0;
//			if ((((long)IntY)*pGD_TControl_Tepl->Critery)<0) IntY=0;
            if ((_GDP.Hot_Tepl->MaxReqWater < _GDCP.Hot_Tepl_Kontur->Do)
                && (contour_idx < cSWaterKontur))
                _GDP.Hot_Tepl->MaxReqWater = _GDCP.Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
        }
        if ((!_GDP.TControl_Tepl->NOwnKonturs))
            _GDP.TControl_Tepl->StopI++;
        else
            _GDP.TControl_Tepl->StopI = 0;
        if (_GDP.TControl_Tepl->StopI > cMaxStopI)
            _GDP.TControl_Tepl->StopI = cMaxStopI;
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
        if (_GDP.TControl_Tepl->Screen[0].Value)
        {
            _GDP.Hot_Tepl->OtherCalc.CorrScreen =
            _GDP.Hot_Tepl->Kontur[cSmScreen].Do; //IntZ
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
    WindWin[0] = cSWaterKontur + 1 - _GD.Hot.PozFluger;
    WindWin[1] = cSWaterKontur + _GD.Hot.PozFluger;

    for (fnTepl = 0; fnTepl < cSTepl; fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        for (int i = cSWaterKontur; i < cSWaterKontur + 2; i++)
        {
            int byte_z = i - cSWaterKontur;
            _GDCP.Hot_Tepl_Kontur = &(_GDP.Hot_Tepl->Kontur[i]);
            _GDCP.TControl_Tepl_Kontur =
            &(_GDP.TControl_Tepl->Kontur[WindWin[byte_z]]);
            _GDCP.Hot_Hand_Kontur = &_GD.Hot.Tepl[fnTepl].HandCtrl[cHSmMixVal
                                                                + WindWin[byte_z]];

            if (_GDCP.TControl_Tepl_Kontur->TPause)
            {
                _GDCP.TControl_Tepl_Kontur->TPause--;
                _GDCP.TControl_Tepl_Kontur->TPause = MAX(_GDCP.TControl_Tepl_Kontur->TPause, 0);
                continue;
            }

            creg.X = _GDCP.Hot_Tepl_Kontur->Do;
//			IntY=abs(IntZ-((int)(pGD_Hot_Hand_Kontur->Position)));

            if (YesBit(_GDCP.Hot_Hand_Kontur->RCS, cbManMech))
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

            _GDCP.Hot_Hand_Kontur->Position = (char) creg.X;

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
        if (((*(_GDCP.Hot_Hand_Kontur + cHSmMixVal)).Position < 100)
            && ((*(_GDCP.Hot_Hand_Kontur + cHSmMixVal)).Position > 0))
        {
            _GDCP.TControl_Tepl_Kontur->PumpPause = cPausePump;
            if (_GDCP.TControl_Tepl_Kontur->DoT > 4000)
                _GDCP.TControl_Tepl_Kontur->PumpPause += cPausePump;
        }
        if (_GDCP.TControl_Tepl_Kontur->PumpPause > 0)
        {
            _GDCP.TControl_Tepl_Kontur->PumpPause--;
//			SetBit(pGD_Hot_Tepl->ExtRCS,cbPausePumpTepl);
        }
        else
            _GDCP.TControl_Tepl_Kontur->PumpPause = 0;
    }

}

