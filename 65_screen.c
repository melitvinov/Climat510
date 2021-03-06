void CheckModeScreen(char typScr,char chType, char fnTepl)
{
//����������� �� typScr
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

	// �� �������������
	int Tz;
	int Ti;
	Tz = pGD_Hot_Tepl->AllTask.DoTHeat;
	Ti = getTempHeat(fnTepl);
	volatile int MidlSun = GD.Hot.MidlSR;

	IntY=getTempHeat(fnTepl)-pGD_Hot_Tepl->AllTask.DoTHeat;
	CorrectionRule(GD.TuneClimate.sc_dTStart,GD.TuneClimate.sc_dTEnd,GD.TuneClimate.sc_dTSunFactor,0);   // �������� ������ �� ������ ���, �������� ������ �� ������ ��, ��������� ������ ��
	SunZClose=GD.TuneClimate.sc_ZSRClose-IntZ;
	volatile int sc_Tout = GD.TControl.MeteoSensing[cSmOutTSens];

	switch(chType)
	{
		case 0:
		if (!bZad)
		{
			if (GD.TuneClimate.sc_TSROpen)
				if (MidlSun>GD.TuneClimate.sc_TSROpen) pScr->Mode=0;
			else
				if (!bNight) pScr->Mode=0;
			if((GD.TuneClimate.sc_ZSRClose)&&(MidlSun>SunZClose)) pScr->Mode=1;
		}
		volatile int CorrRes;
		volatile int deltaTz;
		volatile int deltaTi;

		int sc_deltaTstart = GD.TuneClimate.sc_deltaTstart;
		int sc_deltaTend = GD.TuneClimate.sc_deltaTend;
		int sc_MaxOpenCorrect = GD.TuneClimate.sc_MaxOpenCorrect;
		int sc_TCorrMin = GD.TuneClimate.sc_TCorrMin*100;
		int sc_TCorrMax = GD.TuneClimate.sc_TCorrMax*100;
		int sc_TMinOpenCorrect = GD.TuneClimate.sc_TMinOpenCorrect;
		int sc_StartP1Zone = GD.TuneClimate.sc_StartP2Zone;
		int sc_TOutClose = GD.TuneClimate.sc_TOutClose;
		volatile int sc_LineSunVol = GD.TuneClimate.sc_LineSunVol*10;
		int sc_RHinTepl = pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value;
		int sc_RHStart = GD.TuneClimate.sc_RHStart;
		int sc_RHEnd = GD.TuneClimate.sc_RHEnd;
		int sc_RHMax = GD.TuneClimate.sc_RHMax;
		int sc_TSROpen = GD.TuneClimate.sc_TSROpen;
		int sc_ZSRClose = GD.TuneClimate.sc_ZSRClose;

		if (Tz >= Ti)
			deltaTz = (pGD_Hot_Tepl->AllTask.DoTHeat - getTempHeat(fnTepl));
		else
			deltaTz = 0;
		if (Ti > Tz)
			deltaTi = (getTempHeat(fnTepl) - pGD_Hot_Tepl->AllTask.DoTHeat);
		else
			deltaTi = 0;

			if (MidlSun < sc_TSROpen)
			{
				if ( (bNight)&&(sc_TOutClose < sc_Tout) ) // ��������� �� ������� ����������� � ��������� ����, ����� �� ���� �����������
				{
					pScr->Mode=1;
					pScr->Value = 0;
				}
				else
				{
					pScr->Mode=0;
					pScr->Value = pGD_Control_Tepl->sc_TMaxOpen;  // �� 100 � ���� �� �����
				}
			}
			if ( (MidlSun > sc_TSROpen)&(MidlSun < sc_ZSRClose) )
			{
				pScr->Mode=1;
				pScr->Value = 0;
			}
			if (MidlSun > sc_ZSRClose)
			{
				pScr->Mode=0;										// ������������� ����� �� ��������
				pScr->Value = pGD_Control_Tepl->sc_TMaxOpen;

				if ((sc_LineSunVol)&&(sc_LineSunVol>sc_ZSRClose))	// ���� ����� �������� �������, �� �� sc_ZSRClose �� sc_LineSunVol ������� �� ������ ������������� �����
				{
					if ((MidlSun > sc_ZSRClose)&&(MidlSun < sc_LineSunVol))
					{
						CorrRes = ((MidlSun - sc_ZSRClose) * pGD_Control_Tepl->sc_TMaxOpen) / (sc_LineSunVol - sc_ZSRClose);
						pScr->Value = CorrRes;
					}
					else
					{
						pScr->Mode=0;										// ������������� ����� �� ��������
						pScr->Value = pGD_Control_Tepl->sc_TMaxOpen;
					}

				}
			}

			// ������ ������������ �������� ������
			if ( (sc_deltaTend)&&(sc_MaxOpenCorrect)&&(pScr->Value == pGD_Control_Tepl->sc_TMaxOpen) )   // ������ �������� ��� ����� ��������������� ���������
			{
				if (deltaTi > 0)
				{
					if ((deltaTi > sc_deltaTstart) && (deltaTi < sc_deltaTend))
					{
						if (sc_deltaTend > sc_deltaTstart)
							CorrRes = ((deltaTi - sc_deltaTstart) * sc_MaxOpenCorrect) / (sc_deltaTend - sc_deltaTstart);
						else
							CorrRes = (deltaTi * sc_MaxOpenCorrect) / sc_deltaTend;
						if (pGD_Control_Tepl->sc_TMaxOpen > CorrRes)
							pScr->Value = pGD_Control_Tepl->sc_TMaxOpen - CorrRes;
					}
					if (deltaTi > sc_deltaTend)
					{
						if (pGD_Control_Tepl->sc_TMaxOpen > sc_MaxOpenCorrect)
							pScr->Value = pGD_Control_Tepl->sc_TMaxOpen - sc_MaxOpenCorrect;
					}
				}
			}

		if ( (sc_TMinOpenCorrect>0) && (sc_TCorrMax>0) )
		{
		// ������ ������������ �������� ������
			if (deltaTz > 0)
			{
				if (deltaTz < sc_TCorrMin)
				{
					pScr->Mode=1;
					pScr->Value = sc_StartP1Zone;
				}
				if ( (deltaTz > sc_TCorrMin)&&(deltaTz < sc_TCorrMax) )
				{
					if (sc_TCorrMax > sc_TCorrMin)
						CorrRes = (deltaTz * sc_TMinOpenCorrect) / ((sc_TCorrMax - sc_TCorrMin));
					else
						CorrRes = (deltaTz * sc_TMinOpenCorrect) / ((sc_TCorrMax));
					pScr->Mode=0;
					pScr->Value = CorrRes;
					if (sc_TMinOpenCorrect + sc_StartP1Zone > pGD_Control_Tepl->sc_TMaxOpen)
						pScr->Value = sc_StartP1Zone;
				}
				if (deltaTz >= sc_TCorrMax)
				{
					pScr->Mode=0;
					pScr->Value = sc_TMinOpenCorrect;
					if (sc_TMinOpenCorrect + sc_StartP1Zone > pGD_Control_Tepl->sc_TMaxOpen)
						pScr->Value = sc_StartP1Zone;
				}
			}
		}

		// ������� ������� ��������� �� �������� ������
		if ( (sc_RHinTepl) && (sc_RHStart) && (sc_RHEnd) && (sc_RHMax) )
		{
			IntY=pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value-pGD_Hot_Tepl->AllTask.DoRHAir;
			CorrectionRule(GD.TuneClimate.sc_RHStart,GD.TuneClimate.sc_RHEnd,GD.TuneClimate.sc_RHMax,0);
			if (!pGD_Hot_Tepl->AllTask.DoRHAir) IntZ=0;
				pScr->Value-=IntZ;
		}



/* ������� ������
		IntY=pGD_Hot_Tepl->InTeplSens[cSmGlassSens].Value;
		CorrectionRule(GD.TuneClimate.sc_GlassStart,GD.TuneClimate.sc_GlassEnd,GD.TuneClimate.sc_GlassMax,0);
		if ((YesBit(pGD_Hot_Tepl->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens))) IntZ=GD.TuneClimate.sc_GlassMax;
		pScr->Value=pScr->Mode*(pGD_Control_Tepl->sc_TMaxOpen-(GD.TuneClimate.sc_GlassMax-IntZ));  */
/*������� ������� ��������� �� �������� ������
		IntY=pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value-pGD_Hot_Tepl->AllTask.DoRHAir;
		CorrectionRule(GD.TuneClimate.sc_RHStart,GD.TuneClimate.sc_RHEnd,GD.TuneClimate.sc_RHMax,0);
		if (!pGD_Hot_Tepl->AllTask.DoRHAir) IntZ=0;
		pScr->Value-=IntZ;*/
// �� �������������


	 /*  ����
#warning CHECK THIS
	// NEW
	IntY=getTempHeat(fnTepl)-pGD_Hot_Tepl->AllTask.DoTHeat;
	CorrectionRule(GD.TuneClimate.sc_dTStart,GD.TuneClimate.sc_dTEnd,GD.TuneClimate.sc_dTSunFactor,0);
	SunZClose=GD.TuneClimate.sc_ZSRClose-IntZ;
	IntZ=GD.TControl.MeteoSensing[cSmOutTSens];
	switch(chType) 
	{
	case 0:	// ����������� �������������� �����
		
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

			// 22 ��������
			//if (YesBit(pGD_Hot_Tepl->InTeplSens[cSmTSens1].RCS,cbDownAlarmSens))
			//	pScr->Mode=1;



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
			pScr->Value=pScr->Mode*(pGD_Control_Tepl->sc_TMaxOpen-(GD.TuneClimate.sc_GlassMax-IntZ));	  // �������� �������� �������� ������
			//if (pScr->Mode==0) // ���� ����� �����������
			//	pScr->Value = pScr->Value * GD.TuneClimate.ScreenCloseSpeed;
		}
		*/  // ����

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

void InitScreen(char typScr, char fnTepl)
{
    eScreen *pScr;
	pScr=&pGD_TControl_Tepl->Screen[typScr];
	if (!(pGD_MechConfig->RNum[cHSmScrTH+typScr])) return;
	pScr->PauseMode--;
	if ((pScr->PauseMode<0)||
		(pScr->PauseMode>GD.TuneClimate.sc_PauseMode))
		pScr->PauseMode=0;
	CheckModeScreen(typScr,typScr,fnTepl);
}

void SetPosScreen(char typScr)
{
	char xdata *pMech;
    eScreen *pScr;
	char step;
	char stepClose;

	pScr=&pGD_TControl_Tepl->Screen[typScr];  // �������� ������� ������
    pMech=&((*(pGD_Hot_Hand+cHSmScrTH+typScr)).Position);

	if (YesBit((*(pGD_Hot_Hand+cHSmScrTH+typScr)).RCS,/*(cbNoMech+*/cbManMech)) return;

	if(pScr->Pause<0) pScr->Pause=0;
	if(pScr->Pause) {pScr->Pause--;return;}
	
	ByteX=(*pMech);    //  ������� ��������� ������
	IntZ=pScr->Value;	

	if (!typScr) // ������ ���� �����������, �� ���������� ���������
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
/*   // ���� ����� � ����� �� ��������, �� ������������� !
	if ((pScr->Mode != 1) && (typScr != 0))
	{
		step=0;
		if ((ByteX>=GD.TuneClimate.sc_StartP2Zone)&&(ByteX<GD.TuneClimate.sc_StartP1Zone))
		{
			step=GD.TuneClimate.sc_StepS2Zone;
			pScr->Pause=GD.TuneClimate.sc_StepP2Zone;
		}
		if (ByteX>=GD.TuneClimate.sc_StartP1Zone)
		{
			step=GD.TuneClimate.sc_StepS1Zone;
			pScr->Pause=GD.TuneClimate.sc_StepP1Zone;
		}
	}
	else
	{
		if (ByteX >= GD.TuneClimate.sc_StartClosePoint)
		{
			step=0;
			if ((ByteX>=GD.TuneClimate.sc_StartP2Zone)&&(ByteX<GD.TuneClimate.sc_StartP1Zone))
			{
				step=GD.TuneClimate.sc_StepS2Zone;
				pScr->Pause=GD.TuneClimate.sc_StepP2Zone;
			}
			if (ByteX>=GD.TuneClimate.sc_StartP1Zone)
			{
				step=GD.TuneClimate.sc_StepS1Zone;
				pScr->Pause=GD.TuneClimate.sc_StepP1Zone;
			}
		}
		else
		{
			step=GD.TuneClimate.sc_CloseStep;
			pScr->Pause=GD.TuneClimate.sc_PauseStep;
		}
	}
*/

	//  ��� ����
	/*
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
	} */   //����

	step=0;
//	if (pScr->Mode == 1)&&(GD.TuneClimate.ScreenCloseSpeed>0))
//	{
//  	stepClose = GD.TuneClimate.ScreenCloseSpeed;
//	}
	//	    else
	//		stepClose = 1;
		if ((ByteX>=GD.TuneClimate.sc_StartP2Zone)&&(ByteX<GD.TuneClimate.sc_StartP1Zone))
		{
			step=GD.TuneClimate.sc_StepS2Zone;
			if (step + ByteX > pScr->Value)
				step = pGD_Control_Tepl->sc_TMaxOpen - ByteX;
			pScr->Pause=GD.TuneClimate.sc_StepP2Zone;
		}
		if (ByteX>=GD.TuneClimate.sc_StartP1Zone)
		{
			if (ByteX < pScr->Value)
			{
				if (ByteX + GD.TuneClimate.sc_StepS1Zone <= pScr->Value)//pGD_Control_Tepl->sc_TMaxOpen)
					step=GD.TuneClimate.sc_StepS1Zone;
				else
					step = pScr->Value - ByteX;
			}
			else
				step = ByteX - pScr->Value;
			pScr->Pause=GD.TuneClimate.sc_StepP1Zone;
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


void LaunchVent(char fnTepl)
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
	// ������������ �������������
	IntY=0;
	if (pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value)
	{
		//IntY= getTempVent(fnTepl)-pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
		IntY = pGD_Hot_Tepl->InTeplSens[cSmTSens1].Value - pGD_Hot_Tepl->InTeplSens[cSmTSens2].Value;
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

#warning Calorifer !!!!!!!!!!!!!!!!
void LaunchCalorifer(char fnTepl)
{
//		if (!(pGD_MechConfig->RNum[cHSmHeat])) {pGD_TControl_Tepl->Calorifer=0;return;}

#warning CHECK THIS
	// NEW
	if(getTempHeat(fnTepl)<(pGD_Hot_Tepl->AllTask.DoTHeat
		-GD.TuneClimate.vt_StartCalorifer))
		SetBit(pGD_TControl_Tepl->Calorifer,0x01);
	if ((getTempHeat(fnTepl)>(pGD_Hot_Tepl->AllTask.DoTHeat
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
		if (DoValue > MeasValue)
			pGD_Hot_Tepl->NextTCalc.DiffCO2=DoValue-MeasValue;
		else pGD_Hot_Tepl->NextTCalc.DiffCO2 = 0;
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
	if (pGD_Control_Tepl->co_model>=2) IntZ=200;
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


#warning CO2!!!!
void RegWorkDiskr(char fHSmReg)
	{
	eRegsSettings* fReg;
	int tMech;
	int delta;
	int sum;
	int COset;
	int val = 0;
		fReg=&pGD_TControl_Tepl->SetupRegs[fHSmReg-cHSmCO2];
		tMech=pGD_TControl_Tepl->COPosition;
		if ((!pGD_Control_Tepl->co_model)||(fHSmReg!=cHSmCO2)) return;
		//if (pGD_Control_Tepl->co_model==2)
		if (pGD_Control_Tepl->co_model>=2)
			tMech-=100;
		if (tMech<0) tMech=0;
		IntY=0;	
		ByteZ=0;


		if (pGD_Control_Tepl->co_model==3)
		{
			COset = (*pGD_Hot_Tepl).AllTask.DoCO2;

			sum =  (*(pGD_Hot_Hand+cHSmWinN)).Position + (*(pGD_Hot_Hand+cHSmWinS)).Position;
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
			(*pGD_Hot_Tepl).CO2valveTask = 0;
		    if (COset > (*pGD_Hot_Tepl).InTeplSens[cSmCOSens].Value)
		    {
			  delta = COset - (*pGD_Hot_Tepl).InTeplSens[cSmCOSens].Value;
			  (*pGD_Hot_Tepl).CO2valveTask = delta;
			  if (delta < GD.TuneClimate.co2On)
				tMech = 0;
		    }
		    else
		    	tMech = 0;
		}

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

