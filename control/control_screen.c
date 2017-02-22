#define _FRIEND_OF_CONTROL_

#include "syntax.h"

#include "control_gd.h"
#include "control_subr.h"
#include "control.h"
#include "control_screen.h"

extern uchar       bNight;

void CheckModeScreen(char typScr,char chType, char fnTepl)
{
//Оптимицация на typScr
    char tvs_DegSt,tvs_DegEnd;
    char ttyp,bZad;
    eScreen *pScr;
    int SunZClose;
    pScr=&_GDP.TControl_Tepl->Screen[typScr];
    ttyp=typScr;
    if (ttyp>cHSmScrV_S1) ttyp=cHSmScrV_S1;
    bZad=0;
    if (pScr->PauseMode) bZad=1;

    if ((_GDP.Hot_Tepl->AllTask.Screen[ttyp]<2)&&(!bZad))
    {
        pScr->Mode=_GDP.Hot_Tepl->AllTask.Screen[ttyp];
        bZad=1;
    }

#warning CHECK THIS
    // NEW
    creg.Y=getTempHeat(fnTepl)-_GDP.Hot_Tepl->AllTask.DoTHeat;
    CorrectionRule(_GD.TuneClimate.sc_dTStart,_GD.TuneClimate.sc_dTEnd,_GD.TuneClimate.sc_dTSunFactor,0);
    SunZClose=_GD.TuneClimate.sc_ZSRClose-creg.Z;
    creg.Z=_GD.TControl.MeteoSensing[cSmOutTSens];
    switch (chType)
    {
    case 0: // термический горизонтальный экран

        if (!bZad)
        {
            if (creg.Z<_GD.TuneClimate.sc_TOutClose)
                pScr->Mode=1;
            if ((creg.Z>_GD.TuneClimate.sc_TOutClose+200)||(!_GD.TuneClimate.sc_TOutClose))
                pScr->Mode=0;
            if (_GD.TuneClimate.sc_TSROpen)
            {
                if (_GD.Hot.MidlSR>_GD.TuneClimate.sc_TSROpen)
                    pScr->Mode=0;
            }
            else
                if (!bNight) pScr->Mode=0;

#warning CHECK THIS
            if (YesBit(_GDP.Hot_Tepl->InTeplSens[cSmTSens1].RCS,cbDownAlarmSens))
                pScr->Mode=1;



            if ((_GD.TuneClimate.sc_ZSRClose)&&(_GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }
        if (pScr->Mode!=pScr->OldMode)
        {
            ClrBit(_GDP.TControl_Tepl->RCS1,cbSCCorrection);
            pScr->PauseMode=_GD.TuneClimate.sc_PauseMode;
            if (!pScr->Mode)
            {
                //pGD_TControl_Tepl->TempStart5=pGD_TControl_Tepl->Kontur[cSmKontur5].DoT;
                _GDP.TControl_Tepl->ScrExtraHeat=_GD.TuneClimate.c_5ExtrHeat;
            }
        }
        pScr->OldMode=pScr->Mode;
        _GDP.TControl_Tepl->ScrExtraHeat--;
        if (_GDP.TControl_Tepl->ScrExtraHeat>0) return;
        _GDP.TControl_Tepl->ScrExtraHeat=0;
        creg.Y=_GDP.Hot_Tepl->InTeplSens[cSmGlassSens].Value;
        CorrectionRule(_GD.TuneClimate.sc_GlassStart,_GD.TuneClimate.sc_GlassEnd,_GD.TuneClimate.sc_GlassMax,0);
        if ((YesBit(_GDP.Hot_Tepl->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens))) creg.Z=_GD.TuneClimate.sc_GlassMax;
        {
            pScr->Value=pScr->Mode*(_GDP.Control_Tepl->sc_TMaxOpen-(_GD.TuneClimate.sc_GlassMax-creg.Z));     // итоговое значение открытия экрана
            //if (pScr->Mode == 0) // если экран закрывается
            //	pScr->Value = pScr->Value * GD.TuneClimate.ScreenCloseSpeed;
        }

/*Влияние разницы влажности на открытие экрана
        IntY=pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value-pGD_Hot_Tepl->AllTask.DoRHAir;
        CorrectionRule(GD.TuneClimate.sc_RHStart,GD.TuneClimate.sc_RHEnd,GD.TuneClimate.sc_RHMax,0);
        if (!pGD_Hot_Tepl->AllTask.DoRHAir) IntZ=0;
        pScr->Value-=IntZ;
*/
        break;
    case 1:
        if (!bZad)
        {
            if (creg.Z<_GD.TuneClimate.sc_ZOutClose)
                pScr->Mode=1;
            if ((!_GDP.TControl_Tepl->Screen[0].Mode)||(creg.Z>_GD.TuneClimate.sc_ZOutClose+200)||(!_GD.TuneClimate.sc_ZOutClose))
                pScr->Mode=0;
            if ((_GD.TuneClimate.sc_ZSRClose)&&(_GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }
        if (pScr->Mode!=pScr->OldMode)
            pScr->PauseMode=_GD.TuneClimate.sc_PauseMode;
        pScr->OldMode=pScr->Mode;
        pScr->Value=pScr->Mode*_GDP.Control_Tepl->sc_ZMaxOpen;
        break;
    default:
        if (!bZad)
        {
            if (creg.Z<_GD.TuneClimate.sc_TVOutClose)
                pScr->Mode=1;
            if ((creg.Z>_GD.TuneClimate.sc_TVOutClose+200)||(!_GD.TuneClimate.sc_TVOutClose))
                pScr->Mode=0;
            if ((_GD.Hot.MidlSR>_GD.TuneClimate.sc_TVSRMaxOpen)&&(_GD.TuneClimate.sc_TVSRMaxOpen))
                pScr->Mode=0;
        }
        if (pScr->Mode!=pScr->OldMode)
            pScr->PauseMode=_GD.TuneClimate.sc_PauseMode;
        pScr->OldMode=pScr->Mode;
        pScr->Value=pScr->Mode*100;
        break;
    }
}

void InitScreen(char typScr, char fnTepl)
{
    eScreen *pScr;
    pScr=&_GDP.TControl_Tepl->Screen[typScr];
    if (!(_GDP.MechConfig->RNum[cHSmScrTH+typScr])) return;
    pScr->PauseMode--;
    if ((pScr->PauseMode<0)||
        (pScr->PauseMode>_GD.TuneClimate.sc_PauseMode))
        pScr->PauseMode=0;
    CheckModeScreen(typScr,typScr,fnTepl);
}

void SetPosScreen(char typScr)
{
    char *pMech;
    eScreen *pScr;
    char step;

    pScr=&_GDP.TControl_Tepl->Screen[typScr];  // итоговая позиция экрана
    pMech=&((*(_GDP.Hot_Hand+cHSmScrTH+typScr)).Position);

    if (YesBit((*(_GDP.Hot_Hand+cHSmScrTH+typScr)).RCS,/*(cbNoMech+*/cbManMech)) return;

    if (pScr->Pause<0) pScr->Pause=0;
    if (pScr->Pause)
    {
        pScr->Pause--;return;
    }

    int byte_x=(*pMech);    //  текущее положение экрана
    creg.Z=pScr->Value;

    if (!typScr) // Только если термический, то произвести коррекцию
    {
        creg.Z-=_GDP.Hot_Tepl->Kontur[cSmScreen].Do;//pGD_Hot_Tepl->OtherCalc.CorrScreen;

        if (YesBit(_GDP.TControl_Tepl->RCS1,cbSCCorrection))
        {
            creg.X=((int)(byte_x))-creg.Z;
            if ((!creg.Z)||(creg.Z == 100)||(ABS(creg.X)>_GD.TuneClimate.sc_MinDelta))
                (*pMech)=creg.Z;
            return;
        }
    }
    step=0;
    if ((byte_x>=_GD.TuneClimate.sc_StartP2Zone)&&(byte_x<_GD.TuneClimate.sc_StartP1Zone))
    {
        step=_GD.TuneClimate.sc_StepS2Zone;
        pScr->Pause=_GD.TuneClimate.sc_StepP2Zone;
        if ((pScr->Mode  ==  1) && (!typScr))
            step = step * _GD.TuneClimate.ScreenCloseSpeed;
    }
    if (byte_x>=_GD.TuneClimate.sc_StartP1Zone)
    {
        step=_GD.TuneClimate.sc_StepS1Zone;
        pScr->Pause=_GD.TuneClimate.sc_StepP1Zone;
        if ((pScr->Mode  ==  1) && (!typScr))
            step = step * _GD.TuneClimate.ScreenCloseSpeed;
    }
    creg.X=((int)(byte_x))-creg.Z;
    if (creg.X>0)
    {
        (*pMech)-=step;
        if (!step)
            (*pMech)=0;
        return;
    }
    if (creg.X<0)
    {
        (*pMech)+=step;
        if (!step)
            (*pMech)=(char)_GD.TuneClimate.sc_StartP2Zone;
        if (((*pMech)>=(char)creg.Z)&&(pScr->Mode))
        {
            (*pMech)=(char)creg.Z;
            SetBit(_GDP.TControl_Tepl->RCS1,cbSCCorrection);
        }
        return;
    }
    pScr->Pause=0;
}


void LaunchVent(char fnTepl)
{
    if ((((*(_GDP.Hot_Hand+cHSmWinN)).Position+(*(_GDP.Hot_Hand+cHSmWinS)).Position)>0)&&((_GD.TuneClimate.cool_PFactor)))
        _GDP.TControl_Tepl->OutFan=1;
    else
        _GDP.TControl_Tepl->OutFan=0;

    if ((!_GDP.Hot_Tepl->AllTask.Vent)||((*(_GDP.Hot_Hand+cHSmWinN)).Position+(*(_GDP.Hot_Hand+cHSmWinS)).Position>_GD.TuneClimate.f_BlockFan))
    {
        _GDP.TControl_Tepl->Vent=0;
        return;
    }
    // Вентилляторы перемешивания
    creg.Y=0;
    if (_GDP.Hot_Tepl->InTeplSens[cSmTSens2].Value)
    {
        //IntY= getTempVent(fnTepl)-pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
        creg.Y = _GDP.Hot_Tepl->InTeplSens[cSmTSens1].Value - _GDP.Hot_Tepl->InTeplSens[cSmTSens2].Value;
        if (creg.Y<0)
            creg.Y=-creg.Y;
    }

    _GDP.Hot_Tepl->OtherCalc.TaskCritery=_GD.TuneClimate.vt_PFactor;
    _GDP.Hot_Tepl->OtherCalc.CriteryNow=creg.Y;

    if (_GDP.TControl_Tepl->PauseVent)
    {
        _GDP.TControl_Tepl->PauseVent--;return;
    }

    if (_GDP.TControl_Tepl->Vent)
    {
        _GDP.TControl_Tepl->PauseVent=_GD.TuneClimate.vt_StopTime;
        _GDP.TControl_Tepl->Vent=0;
        return;
    }
    if ((creg.Y<_GDP.Hot_Tepl->OtherCalc.TaskCritery)&&(_GDP.Hot_Tepl->AllTask.Vent!=1)) return;
    _GDP.TControl_Tepl->Vent=1;
    _GDP.TControl_Tepl->PauseVent=_GD.TuneClimate.vt_WorkTime;

}

void LaunchCalorifer(char fnTepl)
{
//		if (!(pGD_MechConfig->RNum[cHSmHeat])) {pGD_TControl_Tepl->Calorifer=0;return;}

#warning CHECK THIS
    // NEW
    if (getTempHeat(fnTepl)<(_GDP.Hot_Tepl->AllTask.DoTHeat
                             -_GD.TuneClimate.vt_StartCalorifer))
        SetBit(_GDP.TControl_Tepl->Calorifer,0x01);
    if ((getTempHeat(fnTepl)>(_GDP.Hot_Tepl->AllTask.DoTHeat
                              +_GD.TuneClimate.vt_EndCalorifer))||(!_GD.TuneClimate.vt_StartCalorifer))
        ClrBit(_GDP.TControl_Tepl->Calorifer,0x01);
//		pGD_TControl_Tepl->Calorifer=0;
    if ((_GD.TControl.MeteoSensing[cSmOutTSens]>_GDP.Hot_Tepl->AllTask.DoTVent+_GD.TuneClimate.cool_PFactor)&&(_GD.TuneClimate.cool_PFactor))
        SetBit(_GDP.TControl_Tepl->Calorifer,0x02);
    if (((_GD.TControl.MeteoSensing[cSmOutTSens]<_GDP.Hot_Tepl->AllTask.DoTVent+_GD.TuneClimate.cool_PFactor-100))||(!_GD.TuneClimate.cool_PFactor))
        ClrBit(_GDP.TControl_Tepl->Calorifer,0x02);
}

void    SetReg(char fHSmReg,int DoValue,int MeasValue)
{
    eRegsSettings* fReg;
    fReg=&_GDP.TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];

    if (!(_GDP.MechConfig->RNum[fHSmReg])) return;
    if (YesBit((*(_GDP.Hot_Hand+fHSmReg)).RCS,cbManMech))
    {
        fReg->IntVal=0;
        return;
    }
    if ((!DoValue)||(!MeasValue))
    {
        if (fHSmReg == cHSmCO2)
        {
            _GDP.TControl_Tepl->COPosition=0;
            _GDP.TControl_Tepl->COPause=_GD.TuneClimate.COPause;
        }
        fReg->IntVal=0;
        (*(_GDP.Hot_Hand+fHSmReg)).Position=0;
        return;
    }

    if (fHSmReg == cHSmCO2)
    {
        //if (GD.Hot.blockCO2) return;
        if (DoValue > MeasValue)
            _GDP.Hot_Tepl->NextTCalc.DiffCO2=DoValue-MeasValue;
        else _GDP.Hot_Tepl->NextTCalc.DiffCO2 = 0;
        if (!_GDP.Hot_Tepl->NextTCalc.DiffCO2)
            _GDP.Hot_Tepl->NextTCalc.DiffCO2=-1;
//		if ((pGD_Hot_Tepl->NextTCalc.DiffCO2<=0))
//		   pGD_Hot_Tepl->NextTCalc.DiffCO2 = 10;

        if (DoValue-MeasValue+((int)_GD.TuneClimate.co_Dif)<0)
        {
            _GDP.TControl_Tepl->COPosition=0;
            (*(_GDP.Hot_Hand+fHSmReg)).Position=0;
        }
        if (_GDP.TControl_Tepl->COPause<0) _GDP.TControl_Tepl->COPause=0;
        if (_GDP.TControl_Tepl->COPause)
        {
            _GDP.TControl_Tepl->COPause--;
            return;
        }
    }

    creg.X=(((long)DoValue-MeasValue)*_GDP.ConstMechanic->ConstMixVal[fHSmReg].v_PFactor/*GD.TuneClimate.reg_PFactor[fHSmReg-cHSmCO2]*/)/1000;
    creg.Y=(fReg->IntVal/100);
    creg.Z=100;
    if (_GDP.Control_Tepl->co_model>=2) creg.Z=200;
    if (creg.Y>creg.Z-creg.X)
    {
        creg.Y=creg.Z-creg.X;
//	   fReg->IntVal=IntY*100;

    }
    else if (creg.Y+creg.X<0)
    {
        creg.Y=-creg.X;
//	   fReg->IntVal=IntY*100;
    }
    else
        fReg->IntVal=fReg->IntVal+(int)((((long)DoValue-MeasValue)*_GDP.ConstMechanic->ConstMixVal[fHSmReg].v_IFactor)/1000);
    _GDP.TControl_Tepl->COPosition=creg.X+creg.Y;
    if (_GDP.TControl_Tepl->COPosition>100)
        (*(_GDP.Hot_Hand+fHSmReg)).Position=100;
    else
        (*(_GDP.Hot_Hand+fHSmReg)).Position=_GDP.TControl_Tepl->COPosition;

}


//#warning CO2!!!!
void RegWorkDiskr(char fHSmReg)
{
    eRegsSettings* fReg;
    int tMech;
    int delta;
    int sum;
    int COset;
    int val = 0;
    fReg=&_GDP.TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];
    tMech=_GDP.TControl_Tepl->COPosition;
    if ((!_GDP.Control_Tepl->co_model)||(fHSmReg!=cHSmCO2)) return;
    //if (pGD_Control_Tepl->co_model == 2)
    if (_GDP.Control_Tepl->co_model>=2)
        tMech-=100;
    if (tMech<0) tMech=0;
    creg.Y=0;


    if (_GDP.Control_Tepl->co_model == 3)
    {
        COset = _GDP.Hot_Tepl->AllTask.DoCO2;

        sum =  (*(_GDP.Hot_Hand+cHSmWinN)).Position + (*(_GDP.Hot_Hand+cHSmWinS)).Position;
        if ((sum >= _GD.TuneClimate.co2Fram1) && (sum <= _GD.TuneClimate.co2Fram2))
        {
            if (COset > _GD.TuneClimate.co2Off)
            {
                if (_GD.TuneClimate.co2Fram2 > _GD.TuneClimate.co2Fram1)
                    val = _GD.TuneClimate.co2Fram2 - _GD.TuneClimate.co2Fram1;
                val = ((sum - _GD.TuneClimate.co2Fram1) * _GD.TuneClimate.co2Off) / val;
                _GD.TuneClimate.co_MaxTime-(((int)tMech)*(_GD.TuneClimate.co_MaxTime-_GD.TuneClimate.co_MinTime))/100;
                COset = COset - val;
            }
        }
        if (sum > _GD.TuneClimate.co2Fram2)
        {
            COset = COset - _GD.TuneClimate.co2Off;
        }
        _GDP.Hot_Tepl->CO2valveTask = 0;
        if (COset > _GDP.Hot_Tepl->InTeplSens[cSmCOSens].Value)
        {
            delta = COset - _GDP.Hot_Tepl->InTeplSens[cSmCOSens].Value;
            _GDP.Hot_Tepl->CO2valveTask = delta;
            if (delta < _GD.TuneClimate.co2On)
                tMech = 0;
        }
        else
            tMech = 0;
    }

    fReg->Work=_GD.TuneClimate.co_Impuls;
    if (!tMech) fReg->Work=0;
    fReg->Stop=_GD.TuneClimate.co_MaxTime-(((int)tMech)*(_GD.TuneClimate.co_MaxTime-_GD.TuneClimate.co_MinTime))/100;
    if (fReg->Pause)
    {
        fReg->Pause--;return;
    }
    if ((fReg->On)&&(fReg->Stop))
    {
        fReg->On=0;
        fReg->Pause=fReg->Stop-1;
        return;
    }
    if (!fReg->Work) return;
    fReg->On=1;
    fReg->Pause=fReg->Work-1;
}

