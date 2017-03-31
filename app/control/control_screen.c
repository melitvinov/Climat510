#define _FRIEND_OF_CONTROL_

#include "syntax.h"

#include "control_gd.h"
#include "control_subr.h"
#include "control.h"
#include "control_screen.h"

extern uchar       bNight;

void CheckModeScreen(const zone_t *zone, char typScr,char chType)
{
//����������� �� typScr
    char ttyp,bZad;
    eScreen *pScr;
    int SunZClose;
    pScr= &zone->tcontrol_tepl->Screen[typScr];
    ttyp=typScr;

    if (ttyp>cHSmScrV_S1)
        ttyp=cHSmScrV_S1;

    bZad=0;

    if (pScr->PauseMode)
        bZad=1;

    if ((zone->hot->AllTask.Screen[ttyp]<2)&&(!bZad))
    {
        pScr->Mode=zone->hot->AllTask.Screen[ttyp];
        bZad=1;
    }

#warning CHECK THIS
    // NEW
    int creg_z;
    int creg_y = getTempHeat(zone, zone->idx)-zone->hot->AllTask.DoTHeat;
    CorrectionRule(_GD.TuneClimate.sc_dTStart,
                   _GD.TuneClimate.sc_dTEnd,
                   _GD.TuneClimate.sc_dTSunFactor,
                   0,
                   creg_y,
                   &creg_z);
    SunZClose=_GD.TuneClimate.sc_ZSRClose - creg_z;
    creg_z = _GD.TControl.MeteoSensing[cSmOutTSens];
    switch (chType)
    {
    case 0: // ����������� �������������� �����

        if (!bZad)
        {
            if (creg_z < _GD.TuneClimate.sc_TOutClose)
                pScr->Mode=1;
            if ((creg_z>_GD.TuneClimate.sc_TOutClose+200)||(!_GD.TuneClimate.sc_TOutClose))
                pScr->Mode=0;
            if (_GD.TuneClimate.sc_TSROpen)
            {
                if (_GD.Hot.MidlSR>_GD.TuneClimate.sc_TSROpen)
                    pScr->Mode=0;
            }
            else
            {
                if (!bNight)
                    pScr->Mode=0;
            }

#warning CHECK THIS
            if (YesBit(zone->hot->InTeplSens[cSmTSens1].RCS, cbDownAlarmSens))
                pScr->Mode=1;


            if ((_GD.TuneClimate.sc_ZSRClose)&&(_GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }

        if (pScr->Mode!=pScr->OldMode)
        {
            ClrBit(zone->tcontrol_tepl->RCS1,cbSCCorrection);
            pScr->PauseMode=_GD.TuneClimate.sc_PauseMode;
            if (!pScr->Mode)
            {
                //pGD_TControl_Tepl->TempStart5=pGD_TControl_Tepl->Kontur[cSmKontur5].DoT;
                zone->tcontrol_tepl->ScrExtraHeat=_GD.TuneClimate.c_5ExtrHeat;
            }
        }
        pScr->OldMode=pScr->Mode;
        zone->tcontrol_tepl->ScrExtraHeat--;

        if (zone->tcontrol_tepl->ScrExtraHeat > 0)
            return;

        zone->tcontrol_tepl->ScrExtraHeat=0;
        creg_y = zone->hot->InTeplSens[cSmGlassSens].Value;
        CorrectionRule(_GD.TuneClimate.sc_GlassStart,
                       _GD.TuneClimate.sc_GlassEnd,
                       _GD.TuneClimate.sc_GlassMax,
                       0,
                       creg_y,
                       &creg_z);

        if ((YesBit(zone->hot->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens)))
            creg_z = _GD.TuneClimate.sc_GlassMax;

        #warning "is it right ? strange C-block was here"
        {
            pScr->Value=pScr->Mode * (zone->gh_ctrl->sc_TMaxOpen-(_GD.TuneClimate.sc_GlassMax - creg_z));     // �������� �������� �������� ������
            //if (pScr->Mode == 0) // ���� ����� �����������
            //	pScr->Value = pScr->Value * GD.TuneClimate.ScreenCloseSpeed;
        }

/*������� ������� ��������� �� �������� ������
        IntY=pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value-pGD_Hot_Tepl->AllTask.DoRHAir;
        CorrectionRule(GD.TuneClimate.sc_RHStart,GD.TuneClimate.sc_RHEnd,GD.TuneClimate.sc_RHMax,0);
        if (!pGD_Hot_Tepl->AllTask.DoRHAir) IntZ=0;
        pScr->Value-=IntZ;
*/
        break;
    case 1:
        if (!bZad)
        {
            if (creg_z < _GD.TuneClimate.sc_ZOutClose)
                pScr->Mode=1;
            if ((!zone->tcontrol_tepl->Screen[0].Mode)||(creg_z > _GD.TuneClimate.sc_ZOutClose+200)||(!_GD.TuneClimate.sc_ZOutClose))
                pScr->Mode=0;
            if ((_GD.TuneClimate.sc_ZSRClose)&&(_GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }
        if (pScr->Mode!=pScr->OldMode)
            pScr->PauseMode=_GD.TuneClimate.sc_PauseMode;
        pScr->OldMode=pScr->Mode;
        pScr->Value=pScr->Mode * zone->gh_ctrl->sc_ZMaxOpen;
        break;
    default:
        if (!bZad)
        {
            if (creg_z < _GD.TuneClimate.sc_TVOutClose)
                pScr->Mode=1;
            if ((creg_z > _GD.TuneClimate.sc_TVOutClose+200)||(!_GD.TuneClimate.sc_TVOutClose))
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

void InitScreen(const zone_t *zone, char screen_type)
{
    eScreen *pScr = &zone->tcontrol_tepl->Screen[screen_type];

    if (! zone->mech_cfg->RNum[cHSmScrTH+screen_type])
        return;
    pScr->PauseMode--;
    if ((pScr->PauseMode<0) || (pScr->PauseMode>_GD.TuneClimate.sc_PauseMode))
        pScr->PauseMode=0;
    CheckModeScreen(zone, screen_type, screen_type);
}

void SetPosScreen(const zone_t *zone, char screen_type)
{
    eScreen *pScr = &zone->tcontrol_tepl->Screen[screen_type];
    int8_t *pMech = &zone->hand[cHSmScrTH+screen_type].Position;

    char step;

    if (YesBit(zone->hand[cHSmScrTH+screen_type].RCS,/*(cbNoMech+*/cbManMech))
        return;

    if (pScr->Pause<0)
        pScr->Pause=0;

    if (pScr->Pause)
    {
        pScr->Pause--;
        return;
    }

    int byte_x=(*pMech);    //  ������� ��������� ������
    int creg_z = pScr->Value;

    if (!screen_type) // ������ ���� �����������, �� ���������� ���������
    {
        creg_z -= zone->hot->Kontur[cSmScreen].Do;//pGD_Hot_Tepl->OtherCalc.CorrScreen;

        if (YesBit(zone->tcontrol_tepl->RCS1, cbSCCorrection))
        {
            int creg_x =((int)(byte_x)) - creg_z;
            if ((! creg_z)||(creg_z == 100)||(ABS(creg_x)>_GD.TuneClimate.sc_MinDelta))
                (*pMech) = creg_z;
            return;
        }
    }
    step=0;
    if ((byte_x>=_GD.TuneClimate.sc_StartP2Zone)&&(byte_x<_GD.TuneClimate.sc_StartP1Zone))
    {
        step=_GD.TuneClimate.sc_StepS2Zone;
        pScr->Pause=_GD.TuneClimate.sc_StepP2Zone;
        if ((pScr->Mode  ==  1) && (!screen_type))
            step = step * _GD.TuneClimate.ScreenCloseSpeed;
    }
    if (byte_x>=_GD.TuneClimate.sc_StartP1Zone)
    {
        step=_GD.TuneClimate.sc_StepS1Zone;
        pScr->Pause=_GD.TuneClimate.sc_StepP1Zone;
        if ((pScr->Mode  ==  1) && (!screen_type))
            step = step * _GD.TuneClimate.ScreenCloseSpeed;
    }
    int creg_x = ((int)(byte_x)) - creg_z;
    if (creg_x > 0)
    {
        (*pMech)-=step;
        if (!step)
            (*pMech)=0;
        return;
    }
    if (creg_x <0)
    {
        (*pMech)+=step;
        if (!step)
            (*pMech)=(char)_GD.TuneClimate.sc_StartP2Zone;
        if (((*pMech)>=(char) creg_z)&&(pScr->Mode))
        {
            (*pMech)=(char) creg_z;
            SetBit(zone->tcontrol_tepl->RCS1, cbSCCorrection);
        }
        return;
    }
    pScr->Pause=0;
}


void LaunchVent(const zone_t *zone)
{
    if ((zone->hand[cHSmWinN].Position + zone->hand[cHSmWinS].Position > 0) && _GD.TuneClimate.cool_PFactor)
        zone->tcontrol_tepl->OutFan = 1;
    else
        zone->tcontrol_tepl->OutFan = 0;

    if (   (! zone->hot->AllTask.Vent)
        || (zone->hand[cHSmWinN].Position + zone->hand[cHSmWinS].Position > _GD.TuneClimate.f_BlockFan))
    {
        zone->tcontrol_tepl->Vent = 0;
        return;
    }
    // ����������� �������������
    int creg_y = 0;
    if (zone->hot->InTeplSens[cSmTSens2].Value)
    {
        //IntY= getTempVent(fnTepl)-pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
        creg_y = zone->hot->InTeplSens[cSmTSens1].Value - zone->hot->InTeplSens[cSmTSens2].Value;
        if (creg_y < 0)
            creg_y =- creg_y;
    }

    zone->hot->OtherCalc.TaskCritery=_GD.TuneClimate.vt_PFactor;
    zone->hot->OtherCalc.CriteryNow = creg_y;

    if (zone->tcontrol_tepl->PauseVent)
    {
        zone->tcontrol_tepl->PauseVent--;
        return;
    }

    if (zone->tcontrol_tepl->Vent)
    {
        zone->tcontrol_tepl->PauseVent=_GD.TuneClimate.vt_StopTime;
        zone->tcontrol_tepl->Vent=0;
        return;
    }

    if ((creg_y < zone->hot->OtherCalc.TaskCritery) && (zone->hot->AllTask.Vent!=1))
        return;

    zone->tcontrol_tepl->Vent =1;
    zone->tcontrol_tepl->PauseVent = _GD.TuneClimate.vt_WorkTime;
}

void LaunchCalorifer(const zone_t *zone)
{
//		if (!(pGD_MechConfig->RNum[cHSmHeat])) {pGD_TControl_Tepl->Calorifer=0;return;}

#warning CHECK THIS
    // NEW
    if (getTempHeat(zone, zone->idx)<(zone->hot->AllTask.DoTHeat - _GD.TuneClimate.vt_StartCalorifer))
        SetBit(zone->tcontrol_tepl->Calorifer, 0x01);

    if ((getTempHeat(zone, zone->idx)>(zone->hot->AllTask.DoTHeat +_GD.TuneClimate.vt_EndCalorifer))
        ||(!_GD.TuneClimate.vt_StartCalorifer))
    {
        ClrBit(zone->tcontrol_tepl->Calorifer,0x01);
    }
//		pGD_TControl_Tepl->Calorifer=0;
    if ((_GD.TControl.MeteoSensing[cSmOutTSens] > zone->hot->AllTask.DoTVent + _GD.TuneClimate.cool_PFactor)&&(_GD.TuneClimate.cool_PFactor))
        SetBit(zone->tcontrol_tepl->Calorifer,0x02);
    if (((_GD.TControl.MeteoSensing[cSmOutTSens] < zone->hot->AllTask.DoTVent+_GD.TuneClimate.cool_PFactor-100))||(!_GD.TuneClimate.cool_PFactor))
        ClrBit(zone->tcontrol_tepl->Calorifer,0x02);
}

void SetReg(const zone_t *zone, char fHSmReg, int DoValue, int MeasValue)
{
    eRegsSettings* fReg = &zone->tcontrol_tepl->SetupRegs[fHSmReg-cHSmCO2];

    if (! zone->mech_cfg->RNum[fHSmReg])
        return;

    if (YesBit(zone->hand[fHSmReg].RCS,cbManMech))
    {
        fReg->IntVal=0;
        return;
    }
    if ((!DoValue)||(!MeasValue))
    {
        if (fHSmReg == cHSmCO2)
        {
            zone->tcontrol_tepl->COPosition=0;
            zone->tcontrol_tepl->COPause=_GD.TuneClimate.COPause;
        }
        fReg->IntVal=0;
        zone->hand[fHSmReg].Position=0;
        return;
    }

    if (fHSmReg == cHSmCO2)
    {
        //if (GD.Hot.blockCO2) return;
        if (DoValue > MeasValue)
            zone->hot->NextTCalc.DiffCO2=DoValue-MeasValue;
        else
            zone->hot->NextTCalc.DiffCO2 = 0;
        if (!zone->hot->NextTCalc.DiffCO2)
            zone->hot->NextTCalc.DiffCO2=-1;
//		if ((pGD_Hot_Tepl->NextTCalc.DiffCO2<=0))
//		   pGD_Hot_Tepl->NextTCalc.DiffCO2 = 10;

        if (DoValue-MeasValue+((int)_GD.TuneClimate.co_Dif)<0)
        {
            zone->tcontrol_tepl->COPosition=0;
            zone->hand[fHSmReg].Position=0;
        }
        if (zone->tcontrol_tepl->COPause<0) zone->tcontrol_tepl->COPause=0;
        if (zone->tcontrol_tepl->COPause)
        {
            zone->tcontrol_tepl->COPause--;
            return;
        }
    }


    int creg_x=(((long)DoValue-MeasValue) * zone->const_mech->ConstMixVal[fHSmReg].v_PFactor/*GD.TuneClimate.reg_PFactor[fHSmReg-cHSmCO2]*/)/1000;
    int creg_y = fReg->IntVal / 100;
    int creg_z = 100;
    if (zone->gh_ctrl->co_model>=2)
        creg_z = 200;
    if (creg_y > creg_z-creg_x)
    {
        creg_y = creg_z-creg_x;
//	   fReg->IntVal=IntY*100;

    }
    else if (creg_y+creg_x < 0)
    {
        creg_y =- creg_x;
//	   fReg->IntVal=IntY*100;
    }
    else
        fReg->IntVal=fReg->IntVal+(int)((((long)DoValue-MeasValue) * zone->const_mech->ConstMixVal[fHSmReg].v_IFactor)/1000);
    zone->tcontrol_tepl->COPosition=creg_x + creg_y;
    if (zone->tcontrol_tepl->COPosition>100)
        zone->hand[fHSmReg].Position=100;
    else
        zone->hand[fHSmReg].Position=zone->tcontrol_tepl->COPosition;
}


//#warning CO2!!!!
void RegWorkDiskr(const zone_t *zone, char fHSmReg)
{
    eRegsSettings* fReg;
    int tMech;
    int delta;
    int sum;
    int COset;
    int val = 0;
    fReg= &zone->tcontrol_tepl->SetupRegs[fHSmReg-cHSmCO2];
    tMech=zone->tcontrol_tepl->COPosition;
    if ((!zone->gh_ctrl->co_model)||(fHSmReg!=cHSmCO2)) return;
    //if (pGD_Control_Tepl->co_model == 2)
    if (zone->gh_ctrl->co_model>=2)
        tMech-=100;
    if (tMech<0)
        tMech=0;


    if (zone->gh_ctrl->co_model == 3)
    {
        COset = zone->hot->AllTask.DoCO2;

        sum =  zone->hand[cHSmWinN].Position + zone->hand[cHSmWinS].Position;
        if ((sum >= _GD.TuneClimate.co2Fram1) && (sum <= _GD.TuneClimate.co2Fram2))
        {
            if (COset > _GD.TuneClimate.co2Off)
            {
                if (_GD.TuneClimate.co2Fram2 > _GD.TuneClimate.co2Fram1)
                    val = _GD.TuneClimate.co2Fram2 - _GD.TuneClimate.co2Fram1;
                val = ((sum - _GD.TuneClimate.co2Fram1) * _GD.TuneClimate.co2Off) / val;
                #warning "WTF: statement with no effect"
                _GD.TuneClimate.co_MaxTime-(((int)tMech)*(_GD.TuneClimate.co_MaxTime-_GD.TuneClimate.co_MinTime))/100;
                COset = COset - val;
            }
        }
        if (sum > _GD.TuneClimate.co2Fram2)
        {
            COset = COset - _GD.TuneClimate.co2Off;
        }
        zone->hot->CO2valveTask = 0;
        if (COset > zone->hot->InTeplSens[cSmCOSens].Value)
        {
            delta = COset - zone->hot->InTeplSens[cSmCOSens].Value;
            zone->hot->CO2valveTask = delta;
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
        fReg->Pause--;
        return;
    }
    if ((fReg->On)&&(fReg->Stop))
    {
        fReg->On=0;
        fReg->Pause=fReg->Stop-1;
        return;
    }
    if (!fReg->Work)
        return;

    fReg->On=1;
    fReg->Pause=fReg->Work-1;
}
