/*!
\brief Температура воздуха для вентиляци в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t screenGetTempVent()
{
	int16_t	tempVent;
	int16_t temp = 0;
	int16_t i;
		switch (pGD_Control_Tepl->sensT_vent)
		{
			case 0: // sensor temp 1
				tempVent = CURRENT_TEMP1_VALUE;
			break;
			case 1: // sensor temp 2
				tempVent = CURRENT_TEMP2_VALUE;
			break;
			case 2: // sensor temp 3
				tempVent = CURRENT_TEMP3_VALUE;
			break;
			case 3: // sensor temp 4
				tempVent = CURRENT_TEMP4_VALUE;
			break;
			case 4: // min
			{
				temp = pGD_Hot_Tepl->InTeplSens[cSmTSens1].Value;
				for (i=1;i<4;i++)
				{
					if (temp > pGD_Hot_Tepl->InTeplSens[i].Value)
						temp = pGD_Hot_Tepl->InTeplSens[i].Value;
				}
				tempVent = temp;
			}
			break;
			case 5: // max
			{
				temp = pGD_Hot_Tepl->InTeplSens[cSmTSens1].Value;
				for (i=1;i<4;i++)
				{
					if (temp < pGD_Hot_Tepl->InTeplSens[i].Value)
						temp = pGD_Hot_Tepl->InTeplSens[i].Value;
				}
				tempVent = temp;
			}
			break;
			case 6: // average
			{
				for (i=0;i<4;i++)
					temp = temp + pGD_Hot_Tepl->InTeplSens[i].Value;
				temp = temp / 4;
				tempVent = temp;
			}
			break;
		}
		return tempVent;
}

/*!
\brief Температура воздуха для обогрева в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
int16_t screenGetTempHeat()
{
	int16_t	tempHeat;
	int16_t temp = 0;
	int16_t i;
		switch (pGD_Control_Tepl->sensT_heat)
		{
			case 0: // sensor temp 1
				tempHeat = CURRENT_TEMP1_VALUE;
			break;
			case 1: // sensor temp 2
				tempHeat = CURRENT_TEMP2_VALUE;
			break;
			case 2: // sensor temp 3
				tempHeat = CURRENT_TEMP3_VALUE;
			break;
			case 3: // sensor temp 4
				tempHeat = CURRENT_TEMP4_VALUE;
			break;
			case 4: // min
			{
				temp = pGD_Hot_Tepl->InTeplSens[cSmTSens1].Value;
				for (i=1;i<4;i++)
				{
					if (temp > pGD_Hot_Tepl->InTeplSens[i].Value)
						temp = pGD_Hot_Tepl->InTeplSens[i].Value;
				}
				tempHeat = temp;
			}
			break;
			case 5: // max
			{
				temp = pGD_Hot_Tepl->InTeplSens[cSmTSens1].Value;
				for (i=1;i<4;i++)
				{
					if (temp < pGD_Hot_Tepl->InTeplSens[i].Value)
						temp = pGD_Hot_Tepl->InTeplSens[i].Value;
				}
				tempHeat = temp;
			}
			break;
			case 6: // average
			{
				for (i=0;i<4;i++)
					temp = temp + pGD_Hot_Tepl->InTeplSens[i].Value;
				temp = temp / 4;
				tempHeat = temp;
			}
			break;
		}
		return tempHeat;
}

void CheckModeScreen(char typScr,char chType)
{
//Оптимицация на typScr
	char tvs_DegSt,tvs_DegEnd;
	char ttyp,bZad;
    eScreen *pScr;
	int SunZClose;
	pScr=&pGD_TControl_Tepl->Screen[typScr];
	ttyp=typScr;
	if (ttyp>cHSmScrV_S1) ttyp=cHSmScrV_S1;	
	bZad=0;
	if (pScr->PauseMode) bZad=1;

	if ((pGD_Hot_Tepl->AllTask.Screen[ttyp]<2)&&(!bZad))
	{
		pScr->Mode=pGD_Hot_Tepl->AllTask.Screen[ttyp];
		bZad=1;
	}

#warning CHECK THIS
	// NEW
	IntY=screenGetTempHeat()-pGD_Hot_Tepl->AllTask.DoTHeat;
	CorrectionRule(GD.TuneClimate.sc_dTStart,GD.TuneClimate.sc_dTEnd,GD.TuneClimate.sc_dTSunFactor,0);
	SunZClose=GD.TuneClimate.sc_ZSRClose-IntZ;
	IntZ=GD.TControl.MeteoSensing[cSmOutTSens];
	switch(chType) 
	{
	case 0:	// термический горизонтальный экран
		
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
			if (YesBit(pGD_Hot_Tepl->InTeplSens[cSmTSens1].RCS,cbDownAlarmSens))
				pScr->Mode=1;



			if((GD.TuneClimate.sc_ZSRClose)&&(GD.Hot.MidlSR>SunZClose))
				pScr->Mode=1;
		}
		if 	(pScr->Mode!=pScr->OldMode)
		{
			ClrBit(pGD_TControl_Tepl->RCS1,cbSCCorrection);
			pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
			if (!pScr->Mode) 
			{
				//pGD_TControl_Tepl->TempStart5=pGD_TControl_Tepl->Kontur[cSmKontur5].DoT;
				pGD_TControl_Tepl->ScrExtraHeat=GD.TuneClimate.c_5ExtrHeat;
			}
		}
		pScr->OldMode=pScr->Mode;
		pGD_TControl_Tepl->ScrExtraHeat--;
		if (pGD_TControl_Tepl->ScrExtraHeat>0) return;
		pGD_TControl_Tepl->ScrExtraHeat=0;	 
		IntY=pGD_Hot_Tepl->InTeplSens[cSmGlassSens].Value;
		CorrectionRule(GD.TuneClimate.sc_GlassStart,GD.TuneClimate.sc_GlassEnd,GD.TuneClimate.sc_GlassMax,0);
		if ((YesBit(pGD_Hot_Tepl->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens))) IntZ=GD.TuneClimate.sc_GlassMax;
		{
			pScr->Value=pScr->Mode*(pGD_Control_Tepl->sc_TMaxOpen-(GD.TuneClimate.sc_GlassMax-IntZ));	  // итоговое значение открытия экрана
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
			if ((!pGD_TControl_Tepl->Screen[0].Mode)||(IntZ>GD.TuneClimate.sc_ZOutClose+200)||(!GD.TuneClimate.sc_ZOutClose))
				pScr->Mode=0;
			if((GD.TuneClimate.sc_ZSRClose)&&(GD.Hot.MidlSR>SunZClose))
				pScr->Mode=1;
		}
		if 	(pScr->Mode!=pScr->OldMode)
			pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
		pScr->OldMode=pScr->Mode;
		pScr->Value=pScr->Mode*pGD_Control_Tepl->sc_ZMaxOpen;		
	break;
	default:
		if (!bZad)
		{
			ByteX=0;
			ByteZ=0;
			if (IntZ<GD.TuneClimate.sc_TVOutClose)
				pScr->Mode=1;
			if ((IntZ>GD.TuneClimate.sc_TVOutClose+200)||(!GD.TuneClimate.sc_TVOutClose))
				pScr->Mode=0;
			if ((GD.Hot.MidlSR>GD.TuneClimate.sc_TVSRMaxOpen)&&(GD.TuneClimate.sc_TVSRMaxOpen))
				pScr->Mode=0;
		}
		if 	(pScr->Mode!=pScr->OldMode)
			pScr->PauseMode=GD.TuneClimate.sc_PauseMode;
		pScr->OldMode=pScr->Mode;
		pScr->Value=pScr->Mode*100;		
	break;
	}
}

void InitScreen(char typScr)
{
    eScreen *pScr;
	pScr=&pGD_TControl_Tepl->Screen[typScr];
	if (!(pGD_MechConfig->RNum[cHSmScrTH+typScr])) return;
	pScr->PauseMode--;
	if ((pScr->PauseMode<0)||
		(pScr->PauseMode>GD.TuneClimate.sc_PauseMode))
		pScr->PauseMode=0;
	CheckModeScreen(typScr,typScr);
}

void SetPosScreen(char typScr)
{
	char xdata *pMech;
    eScreen *pScr;
	char step;

	pScr=&pGD_TControl_Tepl->Screen[typScr];  // итоговая позиция экрана
    pMech=&((*(pGD_Hot_Hand+cHSmScrTH+typScr)).Position);

	if (YesBit((*(pGD_Hot_Hand+cHSmScrTH+typScr)).RCS,/*(cbNoMech+*/cbManMech)) return;

	if(pScr->Pause<0) pScr->Pause=0;
	if(pScr->Pause) {pScr->Pause--;return;}
	
	ByteX=(*pMech);    //  текущее положение экрана
	IntZ=pScr->Value;	

	if (!typScr) // Только если термический, то произвести коррекцию
	{
		IntZ-=pGD_Hot_Tepl->Kontur[cSmScreen].Do;//pGD_Hot_Tepl->OtherCalc.CorrScreen;

		if YesBit(pGD_TControl_Tepl->RCS1,cbSCCorrection)
		{
			IntX=((int)(ByteX))-IntZ;
			if ((!IntZ)||(IntZ==100)||(abs(IntX)>GD.TuneClimate.sc_MinDelta))
				(*pMech)=IntZ;
			return;
		}
	} 
	step=0;
	if ((ByteX>=GD.TuneClimate.sc_StartP2Zone)&&(ByteX<GD.TuneClimate.sc_StartP1Zone))
	{
		step=GD.TuneClimate.sc_StepS2Zone;
		pScr->Pause=GD.TuneClimate.sc_StepP2Zone;
	    if ((pScr->Mode == 1) && (!typScr))
	    	step = step * GD.TuneClimate.ScreenCloseSpeed;
	}
	if (ByteX>=GD.TuneClimate.sc_StartP1Zone)	 
	{
		step=GD.TuneClimate.sc_StepS1Zone;
		pScr->Pause=GD.TuneClimate.sc_StepP1Zone;
		if ((pScr->Mode == 1) && (!typScr))
	    	step = step * GD.TuneClimate.ScreenCloseSpeed;
	}
	IntX=((int)(ByteX))-IntZ;
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
			SetBit(pGD_TControl_Tepl->RCS1,cbSCCorrection);
		}
		return;
	}
	pScr->Pause=0;
}


void LaunchVent(void)
{
	if ((((*(pGD_Hot_Hand+cHSmWinN)).Position+(*(pGD_Hot_Hand+cHSmWinS)).Position)>0)&&((GD.TuneClimate.cool_PFactor)))
		pGD_TControl_Tepl->OutFan=1;
	else
		pGD_TControl_Tepl->OutFan=0;	

	ByteZ=0;
	if ((!pGD_Hot_Tepl->AllTask.Vent)||((*(pGD_Hot_Hand+cHSmWinN)).Position+(*(pGD_Hot_Hand+cHSmWinS)).Position>GD.TuneClimate.f_BlockFan)) 
	{	
		pGD_TControl_Tepl->Vent=0;
		return;
	}
#warning CHECK THIS
	// NEW
	IntY=0;
	if (pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value)
	{
		IntY= screenGetTempVent()-pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
		if (IntY<0)
			IntY=-IntY;
	}

	pGD_Hot_Tepl->OtherCalc.TaskCritery=GD.TuneClimate.vt_PFactor;
	pGD_Hot_Tepl->OtherCalc.CriteryNow=IntY;

	if(pGD_TControl_Tepl->PauseVent) {pGD_TControl_Tepl->PauseVent--;return;}

	if 	(pGD_TControl_Tepl->Vent) 
	{
		pGD_TControl_Tepl->PauseVent=GD.TuneClimate.vt_StopTime;
		pGD_TControl_Tepl->Vent=0;
		return;
	}
	if((IntY<pGD_Hot_Tepl->OtherCalc.TaskCritery)&&(pGD_Hot_Tepl->AllTask.Vent!=1)) return;
	pGD_TControl_Tepl->Vent=1;
	pGD_TControl_Tepl->PauseVent=GD.TuneClimate.vt_WorkTime;
		
}

void LaunchCalorifer(void)
{
	if (!(pGD_MechConfig->RNum[cHSmHeat])) {pGD_TControl_Tepl->Calorifer=0;return;}

#warning CHECK THIS
	// NEW
	if(screenGetTempHeat()<(pGD_Hot_Tepl->AllTask.DoTHeat
		-GD.TuneClimate.vt_StartCalorifer))
		SetBit(pGD_TControl_Tepl->Calorifer,0x01);
	if ((screenGetTempHeat()>(pGD_Hot_Tepl->AllTask.DoTHeat
		+GD.TuneClimate.vt_EndCalorifer))||(!GD.TuneClimate.vt_StartCalorifer)) 
		ClrBit(pGD_TControl_Tepl->Calorifer,0x01);
//		pGD_TControl_Tepl->Calorifer=0;
	if ((GD.TControl.MeteoSensing[cSmOutTSens]>pGD_Hot_Tepl->AllTask.DoTVent+GD.TuneClimate.cool_PFactor)&&(GD.TuneClimate.cool_PFactor))
		SetBit(pGD_TControl_Tepl->Calorifer,0x02);
	if (((GD.TControl.MeteoSensing[cSmOutTSens]<pGD_Hot_Tepl->AllTask.DoTVent+GD.TuneClimate.cool_PFactor-100))||(!GD.TuneClimate.cool_PFactor))
		ClrBit(pGD_TControl_Tepl->Calorifer,0x02);
}

void    SetReg(char fHSmReg,int DoValue,int MeasValue)
	{
	eRegsSettings* fReg;
	fReg=&pGD_TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];

	if (!(pGD_MechConfig->RNum[fHSmReg])) return;
	if (YesBit((*(pGD_Hot_Hand+fHSmReg)).RCS,cbManMech)) 
	{	
		fReg->IntVal=0;
		return;
	}
	if ((!DoValue)||(!MeasValue)) 
	{	
		if (fHSmReg==cHSmCO2)
		{
			pGD_TControl_Tepl->COPosition=0;
			pGD_TControl_Tepl->COPause=GD.TuneClimate.COPause;
		}
		fReg->IntVal=0;
		(*(pGD_Hot_Hand+fHSmReg)).Position=0;
		return;
	}

	if (fHSmReg==cHSmCO2)
	{
		//if (GD.Hot.blockCO2) return; 
			
		pGD_Hot_Tepl->NextTCalc.DiffCO2=DoValue-MeasValue;
		if (!pGD_Hot_Tepl->NextTCalc.DiffCO2) 
			pGD_Hot_Tepl->NextTCalc.DiffCO2=-1;
//		if ((pGD_Hot_Tepl->NextTCalc.DiffCO2<=0))
//		   pGD_Hot_Tepl->NextTCalc.DiffCO2=10;
		
		if(DoValue-MeasValue+((int)GD.TuneClimate.co_Dif)<0)
		{
			pGD_TControl_Tepl->COPosition=0;
			(*(pGD_Hot_Hand+fHSmReg)).Position=0;
		}
		if (pGD_TControl_Tepl->COPause<0) pGD_TControl_Tepl->COPause=0;
		if (pGD_TControl_Tepl->COPause)
		{
			pGD_TControl_Tepl->COPause--;
			return;
		}


	}

	
	
	
	IntX=(((long)DoValue-MeasValue)*pGD_ConstMechanic->ConstMixVal[fHSmReg].v_PFactor/*GD.TuneClimate.reg_PFactor[fHSmReg-cHSmCO2]*/)/1000;
	IntY=(fReg->IntVal/100);
	IntZ=100;
	if (pGD_Control_Tepl->co_model==2) IntZ=200;
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
		fReg->IntVal=fReg->IntVal+(int)((((long)DoValue-MeasValue)*pGD_ConstMechanic->ConstMixVal[fHSmReg].v_IFactor)/1000);
	pGD_TControl_Tepl->COPosition=IntX+IntY;
	if (pGD_TControl_Tepl->COPosition>100)
		(*(pGD_Hot_Hand+fHSmReg)).Position=100;
	else
		(*(pGD_Hot_Hand+fHSmReg)).Position=pGD_TControl_Tepl->COPosition;

	}

void RegWorkDiskr(char fHSmReg)
	{
	eRegsSettings* fReg;
	int tMech;
		fReg=&pGD_TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];
		tMech=pGD_TControl_Tepl->COPosition;
		if ((!pGD_Control_Tepl->co_model)||(fHSmReg!=cHSmCO2)) return;
		if (pGD_Control_Tepl->co_model==2)
			tMech-=100;
		if (tMech<0) tMech=0;

		IntY=0;	
		ByteZ=0;
		fReg->Work=GD.TuneClimate.co_Impuls;
		if (!tMech) fReg->Work=0; 
		fReg->Stop=GD.TuneClimate.co_MaxTime-(((int)tMech)*(GD.TuneClimate.co_MaxTime-GD.TuneClimate.co_MinTime))/100;
		if(fReg->Pause) {fReg->Pause--;return;}
		if ((fReg->On)&&(fReg->Stop) )
		{
			fReg->On=0;
			fReg->Pause=fReg->Stop-1;
			return;
		}
		if(!fReg->Work) return;
		fReg->On=1;
		fReg->Pause=fReg->Work-1;
	}

