#include "syntax.h"

#include "65_gd.h"
#include "65_subr.h"

#include "65_screen.h"

extern uchar       bNight;

extern int16_t IntX;
extern int16_t IntY;
extern int16_t IntZ;

void CheckModeScreen(char typScr,char chType, char fnTepl)
{
//Оптимицация на typScr
    char tvs_DegSt,tvs_DegEnd;
    char ttyp,bZad;
    eScreen *pScr;
    int SunZClose;
    pScr=&gdp.TControl_Tepl->Screen[typScr];
    ttyp=typScr;
    if (ttyp>cHSmScrV_S1) ttyp=cHSmScrV_S1;
    bZad=0;
    if (pScr->PauseMode) bZad=1;

    if ((gdp.Hot_Tepl->AllTask.Screen[ttyp]<2)&&(!bZad))
    {
        pScr->Mode=gdp.Hot_Tepl->AllTask.Screen[ttyp];
        bZad=1;
    }

#warning CHECK THIS
    // NEW
    IntY=getTempHeat(fnTepl)-gdp.Hot_Tepl->AllTask.DoTHeat;
    CorrectionRule(GD.TuneClimate.sc_dTStart,GD.TuneClimate.sc_dTEnd,GD.TuneClimate.sc_dTSunFactor,0);
    SunZClose=GD.TuneClimate.sc_ZSRClose-IntZ;
    IntZ=GD.TControl.MeteoSensing[cSmOutTSens];
    switch (chType)
    {
    case 0: // термический горизонтальный экран

        if (!bZad)
        {
            if (IntZ<GD.TuneClimate.sc_TOutClose)
                pScr->Mode=1;
            if ((IntZ>GD.TuneClimate.sc_TOutClose+200)||(!GD.TuneClimate.sc_TOutClose))
                pScr->Mode=0;
            if (GD.TuneClimate.sc_TSROpen)
            {
                if (GD.Hot.MidlSR>GD.TuneClimate.sc_TSROpen)
                    pScr->Mode=0;
            }
            else
                if (!bNight) pScr->Mode=0;

#warning CHECK THIS
            if (YesBit(gdp.Hot_Tepl->InTeplSens[cSmTSens1].RCS,cbDownAlarmSens))
                pScr->Mode=1;



            if ((GD.TuneClimate.sc_ZSRClose)&&(GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }
        if (pScr->Mode!=pScr->OldMode)
        {
            ClrBit(gdp.TControl_Tepl->RCS1,cbSCCorrection);
            pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
            if (!pScr->Mode)
            {
                //pGD_TControl_Tepl->TempStart5=pGD_TControl_Tepl->Kontur[cSmKontur5].DoT;
                gdp.TControl_Tepl->ScrExtraHeat=GD.TuneClimate.c_5ExtrHeat;
            }
        }
        pScr->OldMode=pScr->Mode;
        gdp.TControl_Tepl->ScrExtraHeat--;
        if (gdp.TControl_Tepl->ScrExtraHeat>0) return;
        gdp.TControl_Tepl->ScrExtraHeat=0;
        IntY=gdp.Hot_Tepl->InTeplSens[cSmGlassSens].Value;
        CorrectionRule(GD.TuneClimate.sc_GlassStart,GD.TuneClimate.sc_GlassEnd,GD.TuneClimate.sc_GlassMax,0);
        if ((YesBit(gdp.Hot_Tepl->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens))) IntZ=GD.TuneClimate.sc_GlassMax;
        {
            pScr->Value=pScr->Mode*(gdp.Control_Tepl->sc_TMaxOpen-(GD.TuneClimate.sc_GlassMax-IntZ));     // итоговое значение открытия экрана
            //if (pScr->Mode==0) // если экран закрывается
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
            if (IntZ<GD.TuneClimate.sc_ZOutClose)
                pScr->Mode=1;
            if ((!gdp.TControl_Tepl->Screen[0].Mode)||(IntZ>GD.TuneClimate.sc_ZOutClose+200)||(!GD.TuneClimate.sc_ZOutClose))
                pScr->Mode=0;
            if ((GD.TuneClimate.sc_ZSRClose)&&(GD.Hot.MidlSR>SunZClose))
                pScr->Mode=1;
        }
        if (pScr->Mode!=pScr->OldMode)
            pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
        pScr->OldMode=pScr->Mode;
        pScr->Value=pScr->Mode*gdp.Control_Tepl->sc_ZMaxOpen;
        break;
    default:
        if (!bZad)
        {
            if (IntZ<GD.TuneClimate.sc_TVOutClose)
                pScr->Mode=1;
            if ((IntZ>GD.TuneClimate.sc_TVOutClose+200)||(!GD.TuneClimate.sc_TVOutClose))
                pScr->Mode=0;
            if ((GD.Hot.MidlSR>GD.TuneClimate.sc_TVSRMaxOpen)&&(GD.TuneClimate.sc_TVSRMaxOpen))
                pScr->Mode=0;
        }
        if (pScr->Mode!=pScr->OldMode)
            pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
        pScr->OldMode=pScr->Mode;
        pScr->Value=pScr->Mode*100;
        break;
    }
}

void InitScreen(char typScr, char fnTepl)
{
    eScreen *pScr;
    pScr=&gdp.TControl_Tepl->Screen[typScr];
    if (!(gdp.MechConfig->RNum[cHSmScrTH+typScr])) return;
    pScr->PauseMode--;
    if ((pScr->PauseMode<0)||
        (pScr->PauseMode>GD.TuneClimate.sc_PauseMode))
        pScr->PauseMode=0;
    CheckModeScreen(typScr,typScr,fnTepl);
}

void SetPosScreen(char typScr)
{
    char *pMech;
    eScreen *pScr;
    char step;

    pScr=&gdp.TControl_Tepl->Screen[typScr];  // итоговая позиция экрана
    pMech=&((*(gdp.Hot_Hand+cHSmScrTH+typScr)).Position);

    if (YesBit((*(gdp.Hot_Hand+cHSmScrTH+typScr)).RCS,/*(cbNoMech+*/cbManMech)) return;

    if (pScr->Pause<0) pScr->Pause=0;
    if (pScr->Pause)
    {
        pScr->Pause--;return;
    }

    int byte_x=(*pMech);    //  текущее положение экрана
    IntZ=pScr->Value;

    if (!typScr) // Только если термический, то произвести коррекцию
    {
        IntZ-=gdp.Hot_Tepl->Kontur[cSmScreen].Do;//pGD_Hot_Tepl->OtherCalc.CorrScreen;

        if (YesBit(gdp.TControl_Tepl->RCS1,cbSCCorrection))
        {
            IntX=((int)(byte_x))-IntZ;
            if ((!IntZ)||(IntZ==100)||(ABS(IntX)>GD.TuneClimate.sc_MinDelta))
                (*pMech)=IntZ;
            return;
        }
    }
    step=0;
    if ((byte_x>=GD.TuneClimate.sc_StartP2Zone)&&(byte_x<GD.TuneClimate.sc_StartP1Zone))
    {
        step=GD.TuneClimate.sc_StepS2Zone;
        pScr->Pause=GD.TuneClimate.sc_StepP2Zone;
        if ((pScr->Mode == 1) && (!typScr))
            step = step * GD.TuneClimate.ScreenCloseSpeed;
    }
    if (byte_x>=GD.TuneClimate.sc_StartP1Zone)
    {
        step=GD.TuneClimate.sc_StepS1Zone;
        pScr->Pause=GD.TuneClimate.sc_StepP1Zone;
        if ((pScr->Mode == 1) && (!typScr))
            step = step * GD.TuneClimate.ScreenCloseSpeed;
    }
    IntX=((int)(byte_x))-IntZ;
    if (IntX>0)
    {
        (*pMech)-=step;
        if (!step)
            (*pMech)=0;
        return;
    }
    if (IntX<0)
    {
        (*pMech)+=step;
        if (!step)
            (*pMech)=(char)GD.TuneClimate.sc_StartP2Zone;
        if (((*pMech)>=(char)IntZ)&&(pScr->Mode))
        {
            (*pMech)=(char)IntZ;
            SetBit(gdp.TControl_Tepl->RCS1,cbSCCorrection);
        }
        return;
    }
    pScr->Pause=0;
}


void LaunchVent(char fnTepl)
{
    if ((((*(gdp.Hot_Hand+cHSmWinN)).Position+(*(gdp.Hot_Hand+cHSmWinS)).Position)>0)&&((GD.TuneClimate.cool_PFactor)))
        gdp.TControl_Tepl->OutFan=1;
    else
        gdp.TControl_Tepl->OutFan=0;

    if ((!gdp.Hot_Tepl->AllTask.Vent)||((*(gdp.Hot_Hand+cHSmWinN)).Position+(*(gdp.Hot_Hand+cHSmWinS)).Position>GD.TuneClimate.f_BlockFan))
    {
        gdp.TControl_Tepl->Vent=0;
        return;
    }
    // Вентилляторы перемешивания
    IntY=0;
    if (gdp.Hot_Tepl->InTeplSens[cSmTSens2].Value)
    {
        //IntY= getTempVent(fnTepl)-pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
        IntY = gdp.Hot_Tepl->InTeplSens[cSmTSens1].Value - gdp.Hot_Tepl->InTeplSens[cSmTSens2].Value;
        if (IntY<0)
            IntY=-IntY;
    }

    gdp.Hot_Tepl->OtherCalc.TaskCritery=GD.TuneClimate.vt_PFactor;
    gdp.Hot_Tepl->OtherCalc.CriteryNow=IntY;

    if (gdp.TControl_Tepl->PauseVent)
    {
        gdp.TControl_Tepl->PauseVent--;return;
    }

    if (gdp.TControl_Tepl->Vent)
    {
        gdp.TControl_Tepl->PauseVent=GD.TuneClimate.vt_StopTime;
        gdp.TControl_Tepl->Vent=0;
        return;
    }
    if ((IntY<gdp.Hot_Tepl->OtherCalc.TaskCritery)&&(gdp.Hot_Tepl->AllTask.Vent!=1)) return;
    gdp.TControl_Tepl->Vent=1;
    gdp.TControl_Tepl->PauseVent=GD.TuneClimate.vt_WorkTime;

}

void LaunchCalorifer(char fnTepl)
{
//		if (!(pGD_MechConfig->RNum[cHSmHeat])) {pGD_TControl_Tepl->Calorifer=0;return;}

#warning CHECK THIS
    // NEW
    if (getTempHeat(fnTepl)<(gdp.Hot_Tepl->AllTask.DoTHeat
                             -GD.TuneClimate.vt_StartCalorifer))
        SetBit(gdp.TControl_Tepl->Calorifer,0x01);
    if ((getTempHeat(fnTepl)>(gdp.Hot_Tepl->AllTask.DoTHeat
                              +GD.TuneClimate.vt_EndCalorifer))||(!GD.TuneClimate.vt_StartCalorifer))
        ClrBit(gdp.TControl_Tepl->Calorifer,0x01);
//		pGD_TControl_Tepl->Calorifer=0;
    if ((GD.TControl.MeteoSensing[cSmOutTSens]>gdp.Hot_Tepl->AllTask.DoTVent+GD.TuneClimate.cool_PFactor)&&(GD.TuneClimate.cool_PFactor))
        SetBit(gdp.TControl_Tepl->Calorifer,0x02);
    if (((GD.TControl.MeteoSensing[cSmOutTSens]<gdp.Hot_Tepl->AllTask.DoTVent+GD.TuneClimate.cool_PFactor-100))||(!GD.TuneClimate.cool_PFactor))
        ClrBit(gdp.TControl_Tepl->Calorifer,0x02);
}

void    SetReg(char fHSmReg,int DoValue,int MeasValue)
{
    eRegsSettings* fReg;
    fReg=&gdp.TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];

    if (!(gdp.MechConfig->RNum[fHSmReg])) return;
    if (YesBit((*(gdp.Hot_Hand+fHSmReg)).RCS,cbManMech))
    {
        fReg->IntVal=0;
        return;
    }
    if ((!DoValue)||(!MeasValue))
    {
        if (fHSmReg==cHSmCO2)
        {
            gdp.TControl_Tepl->COPosition=0;
            gdp.TControl_Tepl->COPause=GD.TuneClimate.COPause;
        }
        fReg->IntVal=0;
        (*(gdp.Hot_Hand+fHSmReg)).Position=0;
        return;
    }

    if (fHSmReg==cHSmCO2)
    {
        //if (GD.Hot.blockCO2) return;
        if (DoValue > MeasValue)
            gdp.Hot_Tepl->NextTCalc.DiffCO2=DoValue-MeasValue;
        else gdp.Hot_Tepl->NextTCalc.DiffCO2 = 0;
        if (!gdp.Hot_Tepl->NextTCalc.DiffCO2)
            gdp.Hot_Tepl->NextTCalc.DiffCO2=-1;
//		if ((pGD_Hot_Tepl->NextTCalc.DiffCO2<=0))
//		   pGD_Hot_Tepl->NextTCalc.DiffCO2=10;

        if (DoValue-MeasValue+((int)GD.TuneClimate.co_Dif)<0)
        {
            gdp.TControl_Tepl->COPosition=0;
            (*(gdp.Hot_Hand+fHSmReg)).Position=0;
        }
        if (gdp.TControl_Tepl->COPause<0) gdp.TControl_Tepl->COPause=0;
        if (gdp.TControl_Tepl->COPause)
        {
            gdp.TControl_Tepl->COPause--;
            return;
        }
    }

    IntX=(((long)DoValue-MeasValue)*gdp.ConstMechanic->ConstMixVal[fHSmReg].v_PFactor/*GD.TuneClimate.reg_PFactor[fHSmReg-cHSmCO2]*/)/1000;
    IntY=(fReg->IntVal/100);
    IntZ=100;
    if (gdp.Control_Tepl->co_model>=2) IntZ=200;
    if (IntY>IntZ-IntX)
    {
        IntY=IntZ-IntX;
//	   fReg->IntVal=IntY*100;

    }
    else if (IntY+IntX<0)
    {
        IntY=-IntX;
//	   fReg->IntVal=IntY*100;
    }
    else
        fReg->IntVal=fReg->IntVal+(int)((((long)DoValue-MeasValue)*gdp.ConstMechanic->ConstMixVal[fHSmReg].v_IFactor)/1000);
    gdp.TControl_Tepl->COPosition=IntX+IntY;
    if (gdp.TControl_Tepl->COPosition>100)
        (*(gdp.Hot_Hand+fHSmReg)).Position=100;
    else
        (*(gdp.Hot_Hand+fHSmReg)).Position=gdp.TControl_Tepl->COPosition;

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
    fReg=&gdp.TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];
    tMech=gdp.TControl_Tepl->COPosition;
    if ((!gdp.Control_Tepl->co_model)||(fHSmReg!=cHSmCO2)) return;
    //if (pGD_Control_Tepl->co_model==2)
    if (gdp.Control_Tepl->co_model>=2)
        tMech-=100;
    if (tMech<0) tMech=0;
    IntY=0;


    if (gdp.Control_Tepl->co_model==3)
    {
        COset = gdp.Hot_Tepl->AllTask.DoCO2;

        sum =  (*(gdp.Hot_Hand+cHSmWinN)).Position + (*(gdp.Hot_Hand+cHSmWinS)).Position;
        if ((sum >= GD.TuneClimate.co2Fram1) && (sum <= GD.TuneClimate.co2Fram2))
        {
            if (COset > GD.TuneClimate.co2Off)
            {
                if (GD.TuneClimate.co2Fram2 > GD.TuneClimate.co2Fram1)
                    val = GD.TuneClimate.co2Fram2 - GD.TuneClimate.co2Fram1;
                val = ((sum - GD.TuneClimate.co2Fram1) * GD.TuneClimate.co2Off) / val;
                GD.TuneClimate.co_MaxTime-(((int)tMech)*(GD.TuneClimate.co_MaxTime-GD.TuneClimate.co_MinTime))/100;
                COset = COset - val;
            }
        }
        if (sum > GD.TuneClimate.co2Fram2)
        {
            COset = COset - GD.TuneClimate.co2Off;
        }
        gdp.Hot_Tepl->CO2valveTask = 0;
        if (COset > gdp.Hot_Tepl->InTeplSens[cSmCOSens].Value)
        {
            delta = COset - gdp.Hot_Tepl->InTeplSens[cSmCOSens].Value;
            gdp.Hot_Tepl->CO2valveTask = delta;
            if (delta < GD.TuneClimate.co2On)
                tMech = 0;
        }
        else
            tMech = 0;
    }

    fReg->Work=GD.TuneClimate.co_Impuls;
    if (!tMech) fReg->Work=0;
    fReg->Stop=GD.TuneClimate.co_MaxTime-(((int)tMech)*(GD.TuneClimate.co_MaxTime-GD.TuneClimate.co_MinTime))/100;
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

