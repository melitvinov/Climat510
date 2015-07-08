/*!
\brief Температура воздуха для вентиляци в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
/*int16_t strategyGetTempVent()
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
}*/

/*!
\brief Температура воздуха для обогрева в зависимости от выбранного значение в Параметрах управления
@return int16_t Температура
*/
/*int16_t strategyGetTempHeat()
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
}*/

/**************************************************************************/
/*-*-*-*-*-*-*--Процедура установки границ для водных контуров--*-*-*-*-*-*/
/**************************************************************************/
int DefRH(void)
{
	if ((!(*pGD_Hot_Tepl).AllTask.DoRHAir)||(!(*pGD_Hot_Tepl).InTeplSens[cSmRHSens].Value)) return 0;
	return ((*pGD_Hot_Tepl).InTeplSens[cSmRHSens].Value-(*pGD_Hot_Tepl).AllTask.DoRHAir);

}

void SetNoWorkKontur(void)
{
	SetBit((*pGD_Hot_Tepl_Kontur).RCS,cbNoWorkKontur);
	(*pGD_TControl_Tepl_Kontur).PumpStatus=0;
	(*pGD_TControl_Tepl_Kontur).DoT=0;
}

void __sMinMaxWater(char fnKontur)
{	
//------------------------------------------------------------------------			
//Заполняем минимальные и максимальные границы для контуров а также опт температуру
//------------------------------------------------------------------------
	pGD_Hot_Tepl_Kontur->MaxCalc=pGD_Control_Tepl->c_MaxTPipe[fnKontur];	
//------------------------------------------------------------------------
//Для контуров начиная с третьего берем минимальные границы и опт. темп. из параметров
//------------------------------------------------------------------------

	//	if  ((GD.Hot.MidlSR<GD.TuneClimate.f_MinSun)&&(pGD_Hot_Tepl->AllTask.NextTAir-GD.TControl.OutTemp>GD.TuneClimate.f_DeltaOut))
//		SetBit(pGD_Hot_Tepl->Kontur[cSmKontur1].ExtRCS,cbBlockPumpKontur);

	if (fnKontur==cSmKontur4)
	{
		pGD_Hot_Tepl_Kontur->MinTask=pGD_Control_Tepl->c_MinTPipe[1];
		pGD_Hot_Tepl_Kontur->Optimal=pGD_Control_Tepl->c_OptimalTPipe[1];
		pGD_Hot_Tepl_Kontur->MinCalc=pGD_Hot_Tepl_Kontur->MinTask;
	}
//------------------------------------------------------------------------
//Если установлен минимум то насос должен быть всегда включен
//------------------------------------------------------------------------
	if(((*pGD_Hot_Tepl_Kontur).MinTask)||(YesBit((*(pGD_Hot_Hand_Kontur+cHSmPump)).RCS,cbManMech)))
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur);					
//------------------------------------------------------------------------
//Ограничиваем минимум 
//------------------------------------------------------------------------
	ogrMin(&((*pGD_Hot_Tepl_Kontur).MinCalc),cMinAllKontur);		
//------------------------------------------------------------------------
//В виду особенностей работы 5 контура для него отдельные установки
//------------------------------------------------------------------------
	if ((*pGD_Hot_Tepl_Kontur).MinCalc>(*pGD_Hot_Tepl_Kontur).MaxCalc)
	{
		(*pGD_Hot_Tepl_Kontur).MinCalc=(*pGD_Hot_Tepl_Kontur).MaxCalc;
	}
	if(fnKontur==cSmKontur5)
	{

		if ((!(YesBit(pGD_Hot_Tepl->InTeplSens[cSmGlassSens].RCS,cbMinMaxVSens)))
			&&((pGD_Hot_Tepl->InTeplSens[cSmGlassSens].Value<GD.TuneClimate.c_DoMinIfGlass)))
		{
			IntY=pGD_Hot_Tepl->InTeplSens[cSmGlassSens].Value;
			CorrectionRule(GD.TuneClimate.c_DoMaxIfGlass,GD.TuneClimate.c_DoMinIfGlass,
				pGD_Hot_Tepl_Kontur->MaxCalc-cMin5Kontur,0);
			pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MaxCalc-IntZ;
			SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur);
							
		}
		if (YesBit(GD.TControl.bSnow,0x02))
		{
			IntY=GD.TControl.MeteoSensing[cSmOutTSens];
			CorrectionRule(GD.TuneClimate.c_CriticalSnowOut,c_SnowIfOut,
				pGD_Hot_Tepl_Kontur->MaxCalc-GD.TuneClimate.c_MinIfSnow,0);
			pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MaxCalc-IntZ;
			SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur);
		}	

		if ((pGD_TControl_Tepl->ScrExtraHeat)||(GD.Hot.Util==9))
		{
			SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur);
			//pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->MaxCalc*10);
			pGD_Hot_Tepl_Kontur->Do=pGD_Hot_Tepl_Kontur->MaxCalc;
		}
//		else
//		if  (pGD_TControl_Tepl->LastScrExtraHeat)
//			pGD_TControl_Tepl_Kontur->DoT=pGD_TControl_Tepl->TempStart5;
						
//		pGD_TControl_Tepl->LastScrExtraHeat=pGD_TControl_Tepl->ScrExtraHeat;

		//ogrMin(&((*pGD_Hot_Tepl_Kontur).MinCalc),cMin5Kontur);
		ogrMin(&(pGD_Hot_Tepl_Kontur->Do),(*pGD_Hot_Tepl_Kontur).MinCalc);
		if (!YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur))
		{
			pGD_Hot_Tepl_Kontur->Do=0;
			pGD_Hot_Tepl_Kontur->MaxCalc=0;
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
		(*pGD_TControl_Tepl).Kontur[cSmScreen].DoT=0;
		return;
	}
	if (YesBit(pGD_TControl_Tepl->RCS1,cbSCCorrection))
		pGD_Hot_Tepl->Kontur[cSmScreen].MaxCalc=GD.TuneClimate.sc_TFactor;	
	IntY=GD.TControl.OutTemp;
	CorrectionRule(GD.TuneClimate.s_StartCorrPow,GD.TuneClimate.s_EndCorrPow,
		(GD.TuneClimate.s_PowFactor-1000),0);
	(*pGD_TControl_Tepl).s_Power=GD.TuneClimate.s_PowFactor-IntZ;		
	if (GD.TuneClimate.s_PowFactor<1000) (*pGD_TControl_Tepl).s_Power=1000;

} */

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*-*-*-*--Процедура установки границ для фрамуг--*-*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sMinMaxWindows(void)
{	
	int	t_max;
//-------------------------------------------------------	
//Проверяем работу по заданию
//------------------------------------------------------
    


	if(((*pGD_Hot_Tepl).AllTask.Win<2)||(YesBit(pGD_TControl_Tepl->Calorifer,0x02))) //Добавлено для блокировки фрамуг кондиционером
	{
//--------------------------------------------------------------------------------
//Если установлен режим работы по минимому то минимум должен совпасть с максимумом
//--------------------------------------------------------------------------------
		if((*pGD_Hot_Tepl).AllTask.Win)
		{
			(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc=(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MinCalc;
			pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSFollowProg;
			pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSFollowProg;
		}
		else
		{
			MemClr(&(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].Optimal,sizeof(eKontur));
		}
//--------------------------------------------------------------------------------
//Стираем все накопленные данные
//--------------------------------------------------------------------------------
		(*pGD_TControl_Tepl).Kontur[cSmWindowOnW].DoT=0;
		(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].DoT=0;
		return;
	}
	pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSOn;
	pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSOn;



	IntY=DefRH();
	CorrectionRule(GD.TuneClimate.f_min_RHStart,GD.TuneClimate.f_min_RHEnd,
		((int)GD.TuneClimate.f_min_Cor),0);
	ogrMin(&(pGD_Hot_Tepl->Kontur[cSmWindowUnW].MinCalc),IntZ);

	IntY=GD.TControl.MeteoSensing[cSmOutTSens];
	CorrectionRule(GD.TuneClimate.f_StartCorrPow,GD.TuneClimate.f_EndCorrPow,
		(GD.TuneClimate.f_PowFactor-1000),0);
	(*pGD_TControl_Tepl).f_Power=GD.TuneClimate.f_PowFactor-IntZ;		
	if (GD.TuneClimate.f_PowFactor<1000) (*pGD_TControl_Tepl).f_Power=1000;


	t_max=pGD_Control_Tepl->f_MaxOpenUn;
	if ((t_max>GD.TuneClimate.f_MaxOpenRain)&&(GD.TControl.bSnow))
	{
		pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSWRain;
		pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSWRain;

		t_max=GD.TuneClimate.f_MaxOpenRain;
	}
		//--------------------------------------------------------------------------------
//Максимум рассчитываем по внешней температуре
//--------------------------------------------------------------------------------
	IntY=GD.TControl.MeteoSensing[cSmOutTSens];
	CorrectionRule(GD.TuneClimate.f_MinTFreeze,GD.TuneClimate.f_MinTFreeze+f_MaxTFreeze,200,0);
	IntX=IntZ;
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась подветренная сторона устанавливаем максимальный ветер в 0 
//--------------------------------------------------------------------------------
//	if (YesBit(GD.Hot.MeteoSens[cSmOutTSens].RCS,cbMinMaxVSens))
//		IntX=100;
//--------------------------------------------------------------------------------
//И по ветру
//--------------------------------------------------------------------------------
	IntY=GD.Hot.MidlWind;
	CorrectionRule(GD.TuneClimate.f_StormWind-f_StartWind,GD.TuneClimate.f_StormWind,100,0);
//В IntZ - ограничение по ветру

	if (IntZ>pGD_TControl_Tepl->OldPozUn)
	{	
		pGD_TControl_Tepl->UnWindStorm=GD.TuneClimate.f_WindHold;
		pGD_TControl_Tepl->OldPozUn=IntZ;
	}
	if (pGD_TControl_Tepl->UnWindStorm>0)
	{
		pGD_TControl_Tepl->UnWindStorm--;
		IntZ=pGD_TControl_Tepl->OldPozUn;
	}

//	if (!(GD.TuneClimate.f_MaxWind)) IntZ=100;
	if (IntX>100)
	{
		IntY=100-IntZ;
		IntX-=100;
	}
	else
	{
		IntY=IntX-IntZ;
		IntX=0;
	}
//	IntY=t_max-IntX-IntZ;

	ogrMin(&IntY,0);
	ogrMax(&IntY,t_max);

    (*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc=IntY;//(*pGD_TControl_Tepl).PrevMaxWinUnW;  
/*--------------------------------------------------------------------------------
//Проверяем было ли отличие от предыдущего максимума хотя бы на один шаг
//--------------------------------------------------------------------------------
    if ((IntY>=(*pGD_TControl_Tepl).PrevMaxWinUnW+ByteW)
		||(IntY<=(*pGD_TControl_Tepl).PrevMaxWinUnW-ByteW))
	{
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc=(IntY/ByteW)*ByteW;
		(*pGD_TControl_Tepl).PrevMaxWinUnW=(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc;
	}
	if ((IntY==0)||(IntY==100))
	{
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc=IntY;
		(*pGD_TControl_Tepl).PrevMaxWinUnW=IntY;
	}*/
//--------------------------------------------------------------------------------
//Максимум для наветренной стороны 
//--------------------------------------------------------------------------------
	t_max=pGD_Control_Tepl->f_MaxOpenOn;

	IntY=-DefRH();//(*pGD_Hot_Tepl).AllTask.NextRHAir-(*pGD_Hot_Tepl).InTeplSens[cSmRHSens].Value;
	CorrectionRule(GD.TuneClimate.f_max_RHStart,GD.TuneClimate.f_max_RHEnd,
		((int)GD.TuneClimate.f_max_Cor),0);
	ogrMax(&t_max,(100-IntZ));

	if ((t_max>GD.TuneClimate.f_MaxOpenRain)&&(GD.TControl.bSnow))
		t_max=GD.TuneClimate.f_MaxOpenRain;

	IntY=GD.Hot.MidlWind;
	CorrectionRule(GD.TuneClimate.f_StormWindOn-f_StartWind,GD.TuneClimate.f_StormWindOn,100,0);


	if (IntZ<pGD_TControl_Tepl->OldPozOn) 
	{	
		pGD_TControl_Tepl->OnWindStorm=GD.TuneClimate.f_WindHold;
		pGD_TControl_Tepl->OldPozOn=IntZ;
	}
	if (pGD_TControl_Tepl->OnWindStorm>0)
	{
		pGD_TControl_Tepl->OnWindStorm--;
		IntZ=pGD_TControl_Tepl->OldPozOn;
	}
//--------------------------------------------------------------------------------
//Если не хотим чтобы открывалась наветренная сторона устанавливаем максимальный ветер в 0 
//--------------------------------------------------------------------------------
	IntY=IntX-IntZ;
	ogrMin(&IntY,0);//if (IntY<0) IntY=0;
	ogrMax(&IntY,t_max);//if (IntY<0) IntY=0;

    (*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MaxCalc=IntY;//(*pGD_TControl_Tepl).PrevMaxWinOnW;  
/*    if ((IntY>=(*pGD_TControl_Tepl).PrevMaxWinOnW+ByteW)
		||(IntY<=(*pGD_TControl_Tepl).PrevMaxWinOnW-ByteW))
	{
		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MaxCalc=(IntY/ByteW)*ByteW;
		(*pGD_TControl_Tepl).PrevMaxWinOnW=(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MaxCalc;
	}
	if ((IntY==0)||(IntY==100))
	{
		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MaxCalc=IntY;
		(*pGD_TControl_Tepl).PrevMaxWinOnW=IntY;
	}
*/
//	if (YesBit((*(pGD_Hot_Hand+cHSmWinN+1-GD.Hot.PozFluger)).RCS,(cbNoMech+cbManMech)))
    if (pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status<20)
    {
    	if (pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT/10==pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc)
    		pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSReachMax;

    	if (pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT/10==pGD_Hot_Tepl->Kontur[cSmWindowOnW].MinCalc)
    		pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSReachMin;
    }
    if (pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status<20)
    {
    	if (pGD_TControl_Tepl->Kontur[cSmWindowUnW].DoT/10==pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc)
    		pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSReachMax;
    	if (pGD_TControl_Tepl->Kontur[cSmWindowUnW].DoT/10==pGD_Hot_Tepl->Kontur[cSmWindowUnW].MinCalc)
    		pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSReachMin;
    }

	if ((!pGD_TControl_Tepl->Kontur[cHSmWinN+GD.Hot.PozFluger].Separate)||
		(YesBit((*(pGD_Hot_Hand+cHSmWinN+GD.Hot.PozFluger)).RCS,cbManMech)))
	{
		(*pGD_TControl_Tepl).Kontur[cSmWindowOnW].DoT=0;
		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MaxCalc=0;
		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].MinCalc=0;
		SetBit(pGD_Hot_Tepl->Kontur[cSmWindowOnW].RCS,cbNoWorkKontur);
		pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSWHand;
	} 
	if ((!pGD_TControl_Tepl->Kontur[cHSmWinN+1-GD.Hot.PozFluger].Separate)||
		(YesBit((*(pGD_Hot_Hand+cHSmWinN+1-GD.Hot.PozFluger)).RCS,cbManMech)))
	{
		SetBit(pGD_Hot_Tepl->Kontur[cSmWindowUnW].RCS,cbNoWorkKontur);
		(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].DoT=0;
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MaxCalc=0;
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].MinCalc=0;
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].Status=cSWHand;
	}

	if (YesBit(pGD_Hot_Tepl->DiskrSens[0],cSmDVent))
	{
		(*pGD_Hot_Tepl).Kontur[cSmWindowUnW].Status=cSAlrExternal;
		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].Status=cSAlrExternal;
	}
	if (YesBit(pGD_TControl_Tepl->MechBusy[cHSmWinN+1-GD.Hot.PozFluger].RCS,cMSAlarm)) pGD_Hot_Tepl->Kontur[cSmWindowUnW].Status=cSAlrNoCtrl;
	if (YesBit(pGD_TControl_Tepl->MechBusy[cHSmWinN+GD.Hot.PozFluger].RCS,cMSAlarm)) pGD_Hot_Tepl->Kontur[cSmWindowOnW].Status=cSAlrNoCtrl;

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
	pGD_Hot_Tepl_Kontur->Status=0;
	if (!(*pGD_TControl_Tepl_Kontur).Separate)
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
	ByteY=0;
		pGD_Hot_Tepl_Kontur->SensValue=pGD_TControl_Tepl_Kontur->SensValue;
		if(!pGD_Control_Tepl->c_MaxTPipe[fnKontur])
		{
			pGD_Hot_Tepl_Kontur->Status=cSWNoMax;
			ByteY++;
		}
		if (((YesBit((*(pGD_Hot_Hand_Kontur+cHSmPump)).RCS,cbManMech))
			&&(!((*(pGD_Hot_Hand_Kontur+cHSmPump)).Position))))
		{
			pGD_Hot_Tepl_Kontur->Status=cSWHand;
			ByteY++;
		}

		if (ByteY)
		{
			SetNoWorkKontur();
			return;
		}	
//		{
//			SetNoWorkKontur();
//			return;
//		}	
		if (!(*pGD_Hot_Tepl_Kontur).SensValue)
		{
			pGD_Hot_Tepl_Kontur->Status=cSAlrNoSens;
			SetBit((*pGD_Hot_Tepl_Kontur).RCS,cbNoSensKontur);			
			SetNoWorkKontur();
			return;	
		}
		__sMinMaxWater(fnKontur);

	for(ByteY=0;ByteY<8;ByteY++)
		((char*)(&(pGD_Hot_Tepl_Kontur->Optimal)))[ByteY]=((char*)(&(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].Optimal)))[ByteY];
	(*pGD_TControl_Tepl_Kontur).DoT=GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].DoT;
	(*pGD_TControl_Tepl_Kontur).PumpStatus=GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].PumpStatus;
		//GD.TControl.Tepl[cSmTeplB].Kontur[fnKontur].SErr=GD.TControl.Tepl[cSmTeplA].Kontur[fnKontur].SErr;
	if (pGD_TControl_Tepl_Kontur->DoT)
		pGD_Hot_Tepl_Kontur->Status=cSOn;

	if (YesBit(GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].ExtRCS,cbBlockPumpKontur))
	{
		pGD_Hot_Tepl_Kontur->Status=cSBlPump;
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur);					
	}
	if (pGD_TControl_Tepl_Kontur->DoT/10==pGD_Hot_Tepl_Kontur->MaxCalc)
		pGD_Hot_Tepl_Kontur->Status=cSReachMax;

	if (pGD_TControl_Tepl_Kontur->DoT/10==pGD_Hot_Tepl_Kontur->MinCalc)
		pGD_Hot_Tepl_Kontur->Status=cSReachMin;

	if (pGD_Hot_Tepl_Kontur->Do)
		pGD_Hot_Tepl_Kontur->Status=cSFollowProg;

	if (pGD_Hot_Tepl_Kontur->MinCalc>pGD_Hot_Tepl_Kontur->MaxCalc-100)
		pGD_Hot_Tepl_Kontur->Status=cSWNoRange;
	if ((pGD_Hot_Tepl_Kontur->SensValue<pGD_Hot_Tepl_Kontur->MaxCalc-50)&&(pGD_Hot_Hand_Kontur->Position==100))
		pGD_Hot_Tepl_Kontur->Status=cSWNoHeat;
	if (YesBit(pGD_Hot_Hand_Kontur->RCS,cbManMech))
		pGD_Hot_Tepl_Kontur->Status=cSWHand;
	if (YesBit(pGD_Hot_Tepl->DiskrSens[0],cSmDHeat))
		pGD_Hot_Tepl_Kontur->Status=cSAlrExternal;


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
	(*pGD_TControl_Tepl_Kontur).SErr=0;
	return;
#endif
	if ((!(*pGD_TControl_Tepl_Kontur).PumpStatus))
	{
		(*pGD_TControl_Tepl_Kontur).SErr=0;
		return;
	}
//------------------------------------------------------------------------
//Подсчет нерегулирумости для контура	
//------------------------------------------------------------------------

	IntX=(((*pGD_TControl_Tepl_Kontur).DoT/10))-pGD_TControl_Tepl_Kontur->SensValue;
	if (((IntX<0)&&(IntX>(-cErrKontur)))||((IntX<cErrKontur)&&(IntX>0)))
		IntX=0;
	LngX=IntX;

	if ((!IntX)||(!SameSign(IntX,pGD_TControl_Tepl_Kontur->SErr)))//(LngX*((long)((*pGD_TControl_Tepl_Kontur).SErr)))<0)))
		(*pGD_TControl_Tepl_Kontur).SErr=0;
	(*pGD_TControl_Tepl_Kontur).SErr+=IntX;

//------------------------------------------------------------------------
//Если общий, то взять нерегулируемость из теплицы А	
//------------------------------------------------------------------------
//	if (YesBit((*pGD_Hot_Tepl_Kontur).RCS,cbAndKontur))
	if	((&(pGD_TControl_Tepl_Kontur->SErr))!=(&GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].Kontur[fnKontur].SErr));
		pGD_TControl_Tepl_Kontur->SErr=0;
//------------------------------------------------------------------------
//Проверяем на контрольную границу	
//------------------------------------------------------------------------
	if (((*pGD_TControl_Tepl_Kontur).SErr>=v_ControlMidlWater)||
		((*pGD_TControl_Tepl_Kontur).SErr<=-v_ControlMidlWater))
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbCtrlErrKontur);

//------------------------------------------------------------------------
//Проверяем на аварийную границу	
//------------------------------------------------------------------------

	if ((*pGD_TControl_Tepl_Kontur).SErr>=v_AlarmMidlWater)
	{
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbAlarmErrKontur);
		(*pGD_TControl_Tepl_Kontur).SErr=v_AlarmMidlWater;
	}
	if ((*pGD_TControl_Tepl_Kontur).SErr<=-v_AlarmMidlWater)
	{
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbAlarmErrKontur);
		(*pGD_TControl_Tepl_Kontur).SErr=-v_AlarmMidlWater;
	}
//------------------------------------------------------------------------
//Устанавливаем нерегулируемость в процентах со знаком
//------------------------------------------------------------------------
	
	IntY=(*pGD_TControl_Tepl_Kontur).SErr;
	CorrectionRule(v_ControlMidlWater,v_AlarmMidlWater,100,0);
	(*pGD_Hot_Tepl_Kontur).SError=(char)IntZ;
	if ((IntZ>cv_ResetMidlWater)&&(YesBit(pGD_TControl_Tepl->MechBusy[fnKontur].RCS,cMSBlockRegs)))
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbResetErrKontur);
	IntY=-IntY;
	CorrectionRule(v_ControlMidlWater,v_AlarmMidlWater,100,0);
	(*pGD_Hot_Tepl_Kontur).SError+=(char)IntZ;
	if ((IntZ>cv_ResetMidlWater)&&(YesBit(pGD_TControl_Tepl->MechBusy[fnKontur].RCS,cMSBlockRegs)))
		SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbResetErrKontur);


	
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
	int* pRealPower;
	pRealPower=&(pGD_TControl_Tepl_Kontur->RealPower[fInv]);
//------------------------------------------------------------------------
//Приоритет по температуре воздуха в теплице
//------------------------------------------------------------------------
	
//------------------------------------------------------------------------
//Приоритет по влажности воздуха в теплице
//------------------------------------------------------------------------
	IntY=(int)pGD_Strategy_Kontur->RHPower;
	IntX=-DefRH();

	IntZ=(int)(((long)IntY)*IntX/1000);
//------------------------------------------------------------------------
//Приоритет по оптимальный температуре
//------------------------------------------------------------------------
	IntX=0;
	if ((*pGD_Hot_Tepl_Kontur).Optimal)
	{
		IntX=((*pGD_Hot_Tepl_Kontur).Optimal-(*pGD_Hot_Tepl_Kontur).Do);
//------------------------------------------------------------------------
//Если контур выключен то оптимальня температура сравнивается с минимумом контура
//------------------------------------------------------------------------
		IntY=(int)pGD_Strategy_Kontur->OptimalPower;
		IntX=(int)(((long)IntY*IntX/100));
	}
	
//------------------------------------------------------------------------
//Приоритет с экономичностью
//------------------------------------------------------------------------
	
	(*pRealPower)=pGD_Strategy_Kontur->TempPower+IntX+IntZ;
	if ((*pRealPower)<1) (*pRealPower)=1; 
	if (!fInv)
	{
		(*pRealPower)=100-pGD_Strategy_Kontur->TempPower-IntX-IntZ;
		if ((*pRealPower)<1) (*pRealPower)=1; 
	}	



}	
void __WorkableKontur(char fnKontur, char fnTepl)
{

//------------------------------------------------------------------------
//Внимание - нерегулируемость из-за ручного управления
//------------------------------------------------------------------------
		if (YesBit((*(pGD_Hot_Hand_Kontur+cHSmMixVal)).RCS,/*(cbNoMech+*/cbManMech))
			SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbAlarmErrKontur);

		IntY=GD.Hot.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].HandCtrl[cHSmMixVal+fnKontur].Position;
	//------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
		if (((*pGD_Hot_Tepl_Kontur).Do<(*pGD_Hot_Tepl_Kontur).MaxCalc)
#ifndef DEMO
		  &&(!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbAlarmErrKontur)))
#endif
		  &&((*pGD_TControl_Tepl_Kontur).PumpStatus)
		  &&((IntY<100)))
			SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyRegUpKontur);
//------------------------------------------------------------------------
//Установить бит возможности работы насосом
//------------------------------------------------------------------------
#warning CHECK THIS
// NEW
		if ((!(*pGD_TControl_Tepl_Kontur).PumpStatus)&&(pGD_TControl_Tepl->Critery>0)&&(fnKontur<cSmKontur5))
		{
			if  ((GD.Hot.MidlSR<GD.TuneClimate.f_MinSun)&&(pGD_Hot_Tepl->AllTask.NextTAir-GD.TControl.MeteoSensing[cSmOutTSens]>GD.TuneClimate.f_DeltaOut)||
			((getTempHeat(fnTepl)-(*pGD_Hot_Tepl).AllTask.DoTHeat)<0)&&(((pGD_Control_Tepl->c_PFactor%100)<90)||(pGD_TControl_Tepl->StopVentI>0)))
			{
				SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyPumpKontur);
		    	if ((pGD_TControl_Tepl_Kontur->NAndKontur==1)&&(!pGD_TControl_Tepl->qMaxOwnKonturs)) pGD_TControl_Tepl_Kontur->RealPower[1]+=100;
			}
		}
//------------------------------------------------------------------------
//Если надо охлаждать и включен насос
//------------------------------------------------------------------------
		if (pGD_TControl_Tepl_Kontur->PumpStatus)
		{
//------------------------------------------------------------------------
//Установить возможности регулирования
//------------------------------------------------------------------------
			if (((*pGD_Hot_Tepl_Kontur).Do>(*pGD_Hot_Tepl_Kontur).MinCalc)
#ifndef DEMO
		  		&&(!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbAlarmErrKontur)))
#endif
				&&(IntY>0))
				SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyRegDownKontur);
//------------------------------------------------------------------------
//Установить возможности работы насосом
//------------------------------------------------------------------------
			if ((!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyRegDownKontur)))
		  	&&(!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur)))
		  	&&(!pGD_TControl_Tepl_Kontur->PumpPause)&&(pGD_TControl_Tepl->Critery<0)&&(!IntY)&&(GD.TControl.MeteoSensing[cSmOutTSens]>500))
				SetBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyPumpKontur);
		}
  
	if ((YesBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegDownKontur))&&(pGD_TControl_Tepl->Critery<0))
			SetBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegsKontur);
	if ((YesBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegUpKontur))&&(pGD_TControl_Tepl->Critery>0))
			SetBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegsKontur);

		
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*************************************************************************/
/*-Процедура определения приоритетов с возможностями и подсчета их суммы-*/
/*************************************************************************/
void __sRealPosibilityKonturs(char fnKontur,long* fMinMax)
{
	ByteZ=0;

//------------------------------------------------------------------------
//Если контур не участвует в управлении то пропускаем 
//------------------------------------------------------------------------


	if (!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,(cbReadyRegUpKontur+cbReadyPumpKontur))))
		pGD_TControl_Tepl_Kontur->RealPower[1]=0;
	if (!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,(cbReadyRegDownKontur+cbReadyPumpKontur))))
		pGD_TControl_Tepl_Kontur->RealPower[0]=0;

	if	(!(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,(cbReadyPumpKontur+cbReadyRegsKontur)))) 
		return;//ByteZ;

	


//------------------------------------------------------------------------
//Если надо охлаждать то меняем знак экономического приоритета для того 
//чтобы он в положительной оси отображал желание рабоать, а в отрицательной - нежелание
//------------------------------------------------------------------------
//	if (pGD_TControl_Tepl->Critery<0)	

	
//------------------------------------------------------------------------
//Меняем приоритеты регулирующих контуров в зависимости от нерегулируемости 
//------------------------------------------------------------------------

	if	(YesBit(pGD_Hot_Tepl_Kontur->ExtRCS,cbReadyRegsKontur))
	{
		if (pGD_TControl_Tepl_Kontur->NAndKontur==1)
		{
			pGD_TControl_Tepl->NOwnKonturs++;
		}
//------------------------------------------------------------------------
//Ищем максимальный приоритет контуров и запоминаем его номер
//------------------------------------------------------------------------
	}

			
	if (fMinMax[0]<(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower])
	{
		fMinMax[0]=(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower];	
		pGD_TControl_Tepl->nMaxKontur=fnKontur;
	}		
	if ((fMinMax[1]<(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower])
		&&(!YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyPumpKontur)))
	{
		fMinMax[1]=(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower];	
	}
	if ((fMinMax[2]<(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower])
		&&(pGD_TControl_Tepl_Kontur->NAndKontur==1)&&(!YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbReadyPumpKontur)))
	{
		fMinMax[2]=(*pGD_TControl_Tepl_Kontur).RealPower[pGD_TControl_Tepl->CurrPower];	

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
	if ((!pGD_TControl_Tepl->qMaxKonturs)||(!YesBit(pGD_Hot_Tepl_Kontur->RCS,cbGoMax))) return 0;

	pGD_TControl_Tepl->StopI=0;

	LngY=pGD_TControl_Tepl->Critery;
	LngY=LngY/pGD_TControl_Tepl->qMaxKonturs;

	LngY=LngY*50;//((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
	LngY=LngY/((long)pGD_Strategy_Kontur->Powers);

	return LngY;
}
long __sRaspOwnKontur(void)
{	
	if ((!pGD_TControl_Tepl->qMaxOwnKonturs)||(!YesBit(pGD_Hot_Tepl_Kontur->RCS,cbGoMaxOwn))) return 0;
	LngY=pGD_TControl_Tepl->Critery;
	LngY=LngY/pGD_TControl_Tepl->qMaxOwnKonturs;

	LngY=LngY*50;//((long)pGD_ConstMechanic->ConstMixVal[cSmKontur1].Power);
	LngY=LngY/((long)pGD_Strategy_Kontur->Powers);


	return LngY;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
long __TempToVent(void)
{	
	LngY=100*((long)pGD_TControl_Tepl->TVentCritery);
	LngY=LngY/50;
	LngY=LngY*30;
	LngY/=((long)pGD_TControl_Tepl->f_Power);

	return LngY;
}
long __VentToTemp(long sVent)
{	
	LngY=sVent*((long)pGD_TControl_Tepl->f_Power);
	LngY/=30;
	LngY*=50;//((long)pGD_ConstMechanic->ConstMixVal[cHSmWinN].Power);
	return LngY/100;
}


/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки фрамуг--*-*-*-*-*-*-*-*/
/*************************************************************************/
void __sLastCheckWindow(char fnTepl)
{
	int	xdata	DoUn;
	int xdata	DoOn;	
	int xdata	tDo;
//	long xdata	tCalc;

	int xdata	MinUn;
	int xdata	MinOn;
	int xdata	MaxUn;
	int xdata	MaxOn;
	int	xdata 	tMaximum;
	int	xdata 	tSLevel;
	

	MinOn=pGD_Hot_Tepl->Kontur[cSmWindowOnW].MinCalc;
	MinUn=pGD_Hot_Tepl->Kontur[cSmWindowUnW].MinCalc;
	MaxOn=pGD_Hot_Tepl->Kontur[cSmWindowOnW].MaxCalc;
	MaxUn=pGD_Hot_Tepl->Kontur[cSmWindowUnW].MaxCalc;

	IntY=(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].CalcT;
	if (IntY<0) IntY=0;
	if (IntY>GD.TuneClimate.f_S1Level)
		tDo=GD.TuneClimate.f_S1Level;
	else
		tDo=IntY;
	IntY-=GD.TuneClimate.f_S1Level;
	IntY*=GD.TuneClimate.f_S1MinDelta;
	if (IntY<0) IntY=0;

	if (IntY>GD.TuneClimate.f_S2Level-GD.TuneClimate.f_S1Level)
		tDo=GD.TuneClimate.f_S2Level;
	else
	    tDo+=IntY;
	IntY-=GD.TuneClimate.f_S2Level-GD.TuneClimate.f_S1Level;
	IntY=(IntY*GD.TuneClimate.f_S2MinDelta)/GD.TuneClimate.f_S1MinDelta;
	if (IntY<0) IntY=0;

	if (IntY>GD.TuneClimate.f_S3Level-GD.TuneClimate.f_S2Level)
		tDo=GD.TuneClimate.f_S3Level;
	else
	   tDo+=IntY;
	IntY-=GD.TuneClimate.f_S3Level-GD.TuneClimate.f_S2Level;
	IntY=(IntY*GD.TuneClimate.f_S3MinDelta)/GD.TuneClimate.f_S2MinDelta;
	if (IntY<0) IntY=0;

	tDo+=IntY;

	IntY=GD.TuneClimate.f_DefOnUn;
//	if (MaxUn<IntY) IntY=MaxUn;

	if (tDo<IntY)
	{
		DoUn=tDo;
		DoOn=0;
	}
	else
	{
	    tDo-=IntY;
		DoUn=IntY+tDo/2;
		DoOn=tDo/2;
	}
	if (DoUn>MaxUn)
		DoOn+=DoUn-MaxUn;
	if (DoOn>MaxOn)
		DoUn+=DoOn-MaxOn;

	tMaximum=MaxOn+MaxUn;
	tDo=(tMaximum-GD.TuneClimate.f_S3Level)/GD.TuneClimate.f_S3MinDelta;
	tSLevel=GD.TuneClimate.f_S3Level;
	if (tDo<0)
	{
		tDo=0;
		tSLevel=tMaximum;
	}
	tDo+=(tSLevel-GD.TuneClimate.f_S2Level)/GD.TuneClimate.f_S2MinDelta;
	tSLevel=GD.TuneClimate.f_S2Level;
	if (tDo<0)
	{
		tDo=0;
		tSLevel=tMaximum;
	}
	tDo+=(tSLevel-GD.TuneClimate.f_S1Level)/GD.TuneClimate.f_S1MinDelta;
	tSLevel=GD.TuneClimate.f_S1Level;
	if (tDo<0)
	{
		tDo=0;
		tSLevel=tMaximum;
	}
	tDo+=tSLevel;
	
	pGD_TControl_Tepl->AbsMaxVent=__VentToTemp(tDo);

	if (DoOn<MinOn) 
		DoOn=MinOn;
	if (DoUn<MinUn)  
		DoUn=MinUn;

	if (DoUn>MaxUn) 
		DoUn=MaxUn;
	if (DoOn>MaxOn)
		DoOn=MaxOn;

   	//IntY=CURRENT_TEMP_VALUE-(*pGD_Hot_Tepl).AllTask.DoTVent;   // было
#warning CHECK THIS
// NEW
	IntY=getTempVent(fnTepl)-(*pGD_Hot_Tepl).AllTask.DoTVent;

   	if (((DoUn==MaxUn)&&(DoOn==MaxOn)&&(IntY>0))
	|| ((DoUn==MinUn)&&(DoOn==MinOn)&&(IntY<0)))
		pGD_TControl_Tepl->StopVentI++;
	else
		pGD_TControl_Tepl->StopVentI=0;
		
	if (pGD_TControl_Tepl->StopVentI>cMaxStopI) pGD_TControl_Tepl->StopVentI=cMaxStopI;

	pGD_TControl_Tepl->Kontur[cSmWindowUnW].DoT=DoUn;
	pGD_TControl_Tepl->Kontur[cSmWindowOnW].DoT=DoOn;

	pGD_Hot_Tepl->Kontur[cSmWindowUnW].Do=(DoUn);
	pGD_Hot_Tepl->Kontur[cSmWindowOnW].Do=(DoOn);

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/



/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки экрана--*-*-*-*-*-*-*-*/
/*************************************************************************/
/*void __sLastCheckScreen(void)
{
	pGD_TControl_Tepl->Kontur[cSmScreen].DoT-=pGD_TControl_Tepl->Kontur[cSmScreen].CalcT/100;	
	pGD_Hot_Tepl_Kontur->Do=(int)(pGD_TControl_Tepl_Kontur->DoT/10);		
	if ((*pGD_Hot_Tepl_Kontur).Do<(*pGD_Hot_Tepl_Kontur).MinCalc)
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
	return(((((long)in)*((long)pGD_Strategy_Kontur->Powers))/50));
/********************************************************************
-----------Работа автонастройки временно приостановлена--------------
*********************************************************************
*1000/pGD_TControl_Tepl->sac_MomentMull)*1000/pGD_TControl_Tepl->sac_LongMull[0]);	
*********************************************************************/
}

/**************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура завершающей проверки контура--*-*-*-*-*-*-*-*/
/**************************************************************************/
void __sLastCheckKontur(char fnKontur,int* fnCorCritery)
{
	int OldDoT;
	long TempDo;	
	*fnCorCritery=0;



	if(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbBlockPumpKontur)) 
		(*pGD_TControl_Tepl_Kontur).PumpStatus=1;
	
	if	(!(*pGD_TControl_Tepl_Kontur).PumpStatus)
	{
/*		if((*pGD_TControl_Tepl_Kontur).DoT)
			(*fnCorCritery)+=((int)((*pGD_TControl_Tepl_Kontur).DoT/100)-GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10);*/
		(*pGD_TControl_Tepl_Kontur).DoT=0;
		(*pGD_Hot_Tepl_Kontur).Do=0;
		return;
				
	}
	//SetBit((*pGD_Hot_Tepl_Kontur).RCS,cbOnPumpKontur);
	
	if ((*pGD_TControl_Tepl_Kontur).Manual) return;

	
	if ((!SameSign(pGD_TControl_Tepl->Critery,pGD_TControl_Tepl_Kontur->SErr))
		&&(YesBit((*pGD_Hot_Tepl_Kontur).ExtRCS,cbResetErrKontur)))
	{
		OldDoT=pGD_TControl_Tepl_Kontur->DoT;
		pGD_Hot_Tepl_Kontur->Do=((pGD_TControl_Tepl_Kontur->SensValue));
		ogrMin(&(pGD_Hot_Tepl_Kontur->Do),(*pGD_Hot_Tepl_Kontur).MinCalc);
		ogrMax(&(pGD_Hot_Tepl_Kontur->Do),(*pGD_Hot_Tepl_Kontur).MaxCalc);
		TempDo=pGD_Hot_Tepl_Kontur->Do*10;
		pGD_TControl_Tepl_Kontur->SErr=0;
		pGD_TControl_Tepl->Integral-=__sThisToFirst((int)((OldDoT-TempDo)))*100;
	//	(*pGD_TControl_Tepl).SaveIntegral-=__sThisToFirst((int)((OldDoT-TempDo)))*100;
		pGD_TControl_Tepl_Kontur->DoT=TempDo;
		return;
	}
	if ((!(*pGD_TControl_Tepl_Kontur).DoT)&&((*pGD_TControl_Tepl_Kontur).LastDoT<5000))
		 (*pGD_TControl_Tepl_Kontur).DoT=(*pGD_TControl_Tepl_Kontur).LastDoT;
	(*pGD_TControl_Tepl_Kontur).DoT=(*pGD_TControl_Tepl_Kontur).DoT+(int)((*pGD_TControl_Tepl_Kontur).CalcT);
	(*pGD_Hot_Tepl_Kontur).Do=((*pGD_TControl_Tepl_Kontur).DoT/10);

	if ((*pGD_Hot_Tepl_Kontur).Do<=(*pGD_Hot_Tepl_Kontur).MinCalc)
	{
		(*pGD_TControl_Tepl_Kontur).DoT=((((*pGD_Hot_Tepl_Kontur).MinCalc)))*10;
	}
	if ((*pGD_Hot_Tepl_Kontur).Do>=(*pGD_Hot_Tepl_Kontur).MaxCalc)
	{
		(*pGD_TControl_Tepl_Kontur).DoT=((((*pGD_Hot_Tepl_Kontur).MaxCalc)))*10;
	}
//	if ((*pGD_Hot_Tepl_Kontur).Do<pGD_Hot_Tepl->AllTask.DoTHeat/10)
//		(*pGD_Hot_Tepl_Kontur).Do=(*pGD_Hot_Tepl_Kontur).MinCalc;//GD.Hot.Tepl[fnTepl].AllTask.DoTHeat/10;

//	(*fnCorCritery)+=((*pGD_Hot_Tepl_Kontur).Do-((*pGD_TControl_Tepl_Kontur).DoT/10));
	(*pGD_Hot_Tepl_Kontur).Do=((*pGD_TControl_Tepl_Kontur).DoT/10);

	(*pGD_TControl_Tepl_Kontur).LastDoT=(*pGD_TControl_Tepl_Kontur).DoT;

}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int __sCalcTempKonturs(void)
{
	int SumTemp=0;
		for(ByteX=0;ByteX<cSWaterKontur-2;ByteX++)
		{
		    SetPointersOnKontur(ByteX);
			//if 	(ByteX=) continue;
			if (pGD_TControl_Tepl_Kontur->DoT)
				SumTemp+=__sThisToFirst(pGD_TControl_Tepl_Kontur->DoT)-pGD_Hot_Tepl->AllTask.DoTHeat;
			else
				if ((pGD_TControl_Tepl_Kontur->LastDoT<5000)&&(pGD_TControl_Tepl_Kontur->LastDoT>pGD_Hot_Tepl->AllTask.DoTHeat))
					 SumTemp+=__sThisToFirst(pGD_TControl_Tepl_Kontur->LastDoT)-pGD_Hot_Tepl->AllTask.DoTHeat;
		}
	return SumTemp;
	}

/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sCalcKonturs(void)
{
	long MinMaxPowerReg[3];
	long xdata temp;
	int	 OldCrit;
	char isFram;
	char fnTepl,tTepl;
	
	for(fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);

//		if(!(*pGD_Hot_Tepl).AllTask.NextTAir) return;

//Определение направления нагрев или охлаждени
		pGD_TControl_Tepl->CurrPower=0;
		if (pGD_TControl_Tepl->Critery>0)
			pGD_TControl_Tepl->CurrPower=1;
// Отключить, если нет задания
		if(!pGD_Hot_Tepl->AllTask.DoTHeat) continue;
// Расчет минимумов и максимумов контуров
		for(ByteX=0;ByteX<cSWaterKontur;ByteX++)
		{
			SetPointersOnKontur(ByteX);
			ClrDog;
			__sInitKontur(ByteX);
		}
//		__sMinMaxScreen();
// Расчет минимумов и максимумов фрамуг 
		__sMinMaxWindows();

	}
// Заново считаем 
	for(fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);

		MinMaxPowerReg[2]=0;
		MinMaxPowerReg[1]=0;
		MinMaxPowerReg[0]=0;
		(*pGD_TControl_Tepl).nMaxKontur=-1;
		ClrDog;
		(*pGD_TControl_Tepl).NOwnKonturs=0;
		if(!GD.Hot.Tepl[fnTepl].AllTask.DoTHeat) continue;
		for(ByteX=0;ByteX<cSWaterKontur;ByteX++)
		{
			SetPointersOnKontur(ByteX);
			__sRegulKontur(ByteX);
			pGD_TControl_Tepl_Kontur->Manual=0;
			ClrDog;
			if 	(YesBit(pGD_Hot_Tepl_Kontur->RCS,cbNoWorkKontur)) continue;
			if 	(pGD_Hot_Tepl_Kontur->Do)
			{
				pGD_TControl_Tepl_Kontur->PumpStatus=1;
				pGD_TControl_Tepl_Kontur->Manual=1;
				pGD_TControl_Tepl_Kontur->DoT=(pGD_Hot_Tepl_Kontur->Do*10);
				continue;
			}
			pGD_Hot_Tepl_Kontur->Do=(pGD_TControl_Tepl_Kontur->DoT/10);
			ClrDog;
			__sPotentialPosibilityKontur(0);//Приоритет в случае охлаждения
			__sPotentialPosibilityKontur(1);//Приоритет в случае нагрева
			ClrDog;


			__WorkableKontur(ByteX,fnTepl);
			__sRealPosibilityKonturs(ByteX,MinMaxPowerReg);
			(*pGD_Hot_Tepl_Kontur).Priority=(int)(pGD_TControl_Tepl_Kontur->RealPower[pGD_TControl_Tepl->CurrPower]);
			pGD_TControl_Tepl->PowMaxKonturs=MinMaxPowerReg[1];
			pGD_TControl_Tepl->PowOwnMaxKonturs=MinMaxPowerReg[2];
		}

//		if ((GD.TControl.PrevPozFluger!=GD.Hot.PozFluger)&&(GD.TuneClimate.f_changeWindows))
//		{
//			temp=(*pGD_TControl_Tepl).Kontur[cSmWindowOnW].DoT;				
//			(*pGD_TControl_Tepl).Kontur[cSmWindowOnW].DoT=(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].DoT;
//			(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].DoT=temp;
//			
//		}
//		(*pGD_Hot_Tepl).Kontur[cSmWindowOnW].Do=((*pGD_TControl_Tepl).Kontur[cSmWindowOnW].DoT/10);			

//		__sWorkableWindow();
				
//		pGD_TControl_Tepl->Kontur[cSmWindowUnW].CalcT=0;

		pGD_Hot_Tepl->Kontur[cSmWindowUnW].Optimal=pGD_TControl_Tepl->f_NMinDelta;	


		(*pGD_TControl_Tepl).Kontur[cSmWindowUnW].CalcT=__TempToVent();

		  
		__sLastCheckWindow(fnTepl);

		pGD_TControl_Tepl->qMaxKonturs=0;
		pGD_TControl_Tepl->qMaxOwnKonturs=0;

		for(ByteX=0;ByteX<cSWaterKontur;ByteX++)
		{
			SetPointersOnKontur(ByteX);
			if 	(YesBit(pGD_Hot_Tepl_Kontur->RCS,cbNoWorkKontur)) continue;
			if	((pGD_TControl_Tepl_Kontur->RealPower[pGD_TControl_Tepl->CurrPower]>=pGD_TControl_Tepl->PowMaxKonturs)&&(pGD_TControl_Tepl->PowMaxKonturs))
			{
				SetBit(pGD_Hot_Tepl_Kontur->RCS,cbGoMax);
				pGD_TControl_Tepl->qMaxKonturs++;			
			}
			if	((pGD_TControl_Tepl_Kontur->RealPower[pGD_TControl_Tepl->CurrPower]>=pGD_TControl_Tepl->PowOwnMaxKonturs)
				&&(pGD_TControl_Tepl->PowOwnMaxKonturs)&&(pGD_TControl_Tepl_Kontur->NAndKontur==1))
			{
				SetBit(pGD_Hot_Tepl_Kontur->RCS,cbGoMaxOwn);
				pGD_TControl_Tepl->qMaxOwnKonturs++;			
			}
		}	
	}
	GD.TControl.PrevPozFluger=GD.Hot.PozFluger;

	//Очень длинный расчет, поэтому проверим измерения
//	CheckReadyMeasure();	
	//------------------------------------------------
	for(fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);

//		if (!pGD_TControl_Tepl->qMaxOwnKonturs)
//			pGD_TControl_Tepl->StopI=2;
		if(!pGD_Hot_Tepl->AllTask.DoTHeat) continue;

		if ((pGD_TControl_Tepl->nMaxKontur>=0)&&(YesBit((*pGD_Hot_Tepl).Kontur[pGD_TControl_Tepl->nMaxKontur].ExtRCS,cbReadyPumpKontur))
		&&(!YesBit(pGD_Hot_Tepl->Kontur[pGD_TControl_Tepl->nMaxKontur].RCS,cbPumpChange)))
		{
			ByteY=0;
			pGD_TControl_Tepl->qMaxKonturs=0;
			pGD_TControl_Tepl->qMaxOwnKonturs=0;
			if (pGD_TControl_Tepl->Critery<0)
			{
				for (tTepl=0;tTepl<cSTepl;tTepl++)
				{
					ClrDog;
					if (!((*pGD_TControl_Tepl).Kontur[pGD_TControl_Tepl->nMaxKontur].Separate&(1<<tTepl))) continue;

					if ((!(YesBit(GD.Hot.Tepl[tTepl].Kontur[pGD_TControl_Tepl->nMaxKontur].ExtRCS,cbReadyPumpKontur)))
					&&((!GD.TControl.Tepl[tTepl].NOwnKonturs)||(pGD_TControl_Tepl->NOwnKonturs)))
						ByteY=1;
				}
			}
			if (!ByteY)
			{
				for (tTepl=0;tTepl<cSTepl;tTepl++)
				{
					if (!((*pGD_TControl_Tepl).Kontur[pGD_TControl_Tepl->nMaxKontur].Separate&(1<<tTepl))) continue;
					GD.TControl.Tepl[tTepl].Kontur[pGD_TControl_Tepl->nMaxKontur].PumpStatus=1-pGD_TControl_Tepl->Kontur[pGD_TControl_Tepl->nMaxKontur].PumpStatus;	
					GD.TControl.Tepl[tTepl].Kontur[pGD_TControl_Tepl->nMaxKontur].PumpPause=cPausePump;
					SetBit(GD.Hot.Tepl[tTepl].Kontur[pGD_TControl_Tepl->nMaxKontur].RCS,cbPumpChange);
				}		
			}
		}
		
	}		
	ClrDog;	
//	CheckReadyMeasure();
	for(ByteX=0;ByteX<cSWaterKontur;ByteX++)
	{

		for (fnTepl=0;fnTepl<cSTepl;fnTepl++)
		{
			ClrDog;
			SetPointersOnTepl(fnTepl);
			SetPointersOnKontur(ByteX);
			pGD_TControl_Tepl->RealPower=0;
			if 	(YesBit(pGD_Hot_Tepl_Kontur->RCS,cbNoWorkKontur)) continue;
			pGD_TControl_Tepl->NAndKontur=pGD_TControl_Tepl_Kontur->NAndKontur;
			GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].RealPower+=__sRaspKontur();
//			if ((!pGD_TControl_Tepl->NOwnKonturs)&&(pGD_TControl_Tepl_Kontur->RealPower))
//			{
//				pGD_TControl_Tepl->NAndKontur=1;
//				GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].RealPower=__sRaspKontur();
//				break;
//			}
		}
		for (fnTepl=0;fnTepl<cSTepl;fnTepl++)
		{
			SetPointersOnTepl(fnTepl);
			SetPointersOnKontur(ByteX);
			if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbNoWorkKontur)) continue;
			if (pGD_TControl_Tepl_Kontur->NAndKontur==1) continue;
			LngY=(GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].RealPower);
			ByteW=(GD.TControl.Tepl[pGD_TControl_Tepl_Kontur->MainTepl].NAndKontur);
			LngY/=ByteW;
			ClrDog;

			OldCrit=pGD_TControl_Tepl->Critery;
			if (pGD_TControl_Tepl->NOwnKonturs)
				pGD_TControl_Tepl->Critery=pGD_TControl_Tepl->Critery-__sThisToFirst((int)((LngY)));
			if (!SameSign(OldCrit,pGD_TControl_Tepl->Critery))
				pGD_TControl_Tepl->Critery=0;
			pGD_TControl_Tepl_Kontur->CalcT=LngY;
			__sLastCheckKontur(ByteX,&IntY);
			ClrDog;	

			if ((pGD_Hot_Tepl->MaxReqWater<pGD_Hot_Tepl_Kontur->Do)&&(ByteX<cSWaterKontur))
				pGD_Hot_Tepl->MaxReqWater=pGD_Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
		}

	}
ClrDog;
//	CheckReadyMeasure();	
	for (fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);
		for(ByteX=0;ByteX<cSWaterKontur;ByteX++)
		{
			SetPointersOnKontur(ByteX);
			ClrDog;
			if (YesBit(pGD_Hot_Tepl_Kontur->RCS,cbNoWorkKontur)) continue;
			if (pGD_TControl_Tepl_Kontur->NAndKontur!=1) continue;
			pGD_TControl_Tepl_Kontur->CalcT=__sRaspOwnKontur();
			__sLastCheckKontur(ByteX,&IntY);	
//			if ((((long)IntY)*pGD_TControl_Tepl->Critery)<0) IntY=0;
			if ((pGD_Hot_Tepl->MaxReqWater<pGD_Hot_Tepl_Kontur->Do)&&(ByteX<cSWaterKontur))
				pGD_Hot_Tepl->MaxReqWater=pGD_Hot_Tepl_Kontur->Do;
//			pGD_TControl_Tepl->KonturIntegral+=__sThisToFirst(IntY);
		}
		if ((!pGD_TControl_Tepl->NOwnKonturs)) pGD_TControl_Tepl->StopI++;
		else
	   		pGD_TControl_Tepl->StopI=0;
		if (pGD_TControl_Tepl->StopI>cMaxStopI) pGD_TControl_Tepl->StopI=cMaxStopI;
	}
	
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*************************************************************************/
/*-*-*-*-*-*-*-*-*--Процедура начальных установок для контура--*-*-*-*-*-*/
/*************************************************************************/
void __sMechScreen(void)
{
	char xdata fnTepl;
	for(fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);
		if(pGD_TControl_Tepl->Screen[0].Value)
		{
			pGD_Hot_Tepl->OtherCalc.CorrScreen=pGD_Hot_Tepl->Kontur[cSmScreen].Do;//IntZ
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
	char xdata WindWin[2];
	char xdata fnTepl;
//Оптимизация
	WindWin[0]=cSWaterKontur+1-GD.Hot.PozFluger;
	WindWin[1]=cSWaterKontur+GD.Hot.PozFluger;
	
	for(fnTepl=0;fnTepl<cSTepl;fnTepl++)
	{
		SetPointersOnTepl(fnTepl);
		for (ByteX=cSWaterKontur;ByteX<cSWaterKontur+2;ByteX++)
		{
			ByteZ=ByteX-cSWaterKontur;
			pGD_Hot_Tepl_Kontur=&(pGD_Hot_Tepl->Kontur[ByteX]);
			pGD_TControl_Tepl_Kontur=&(pGD_TControl_Tepl->Kontur[WindWin[ByteZ]]);
			pGD_Hot_Hand_Kontur=&GD.Hot.Tepl[fnTepl].HandCtrl[cHSmMixVal+WindWin[ByteZ]];

			if (pGD_TControl_Tepl_Kontur->TPause) 
			{
				pGD_TControl_Tepl_Kontur->TPause--;
				ogrMin(&(pGD_TControl_Tepl_Kontur->TPause),0);
				continue;
			}
	
			IntX=pGD_Hot_Tepl_Kontur->Do;
//			IntY=abs(IntZ-((int)(pGD_Hot_Hand_Kontur->Position)));

			if (YesBit(pGD_Hot_Hand_Kontur->RCS,cbManMech)) continue;

/*		    if ((IntY<(*pGD_TControl_Tepl).f_NMinDelta)&&(IntX)&&(IntX!=100))continue;

			if ((IntX-pGD_Hot_Tepl_Kontur->MinCalc)<(*pGD_TControl_Tepl).f_NMinDelta)
			{
				IntX=pGD_Hot_Tepl_Kontur->MinCalc;	
			}
			if ((pGD_Hot_Tepl_Kontur->MaxCalc-IntX)<(*pGD_TControl_Tepl).f_NMinDelta)
			{
				IntX=pGD_Hot_Tepl_Kontur->MaxCalc;	
			}
*/			

			pGD_Hot_Hand_Kontur->Position=(char)IntX;

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
	for (ByteX=0;ByteX<cSWaterKontur;ByteX++)
	{
		SetPointersOnKontur(ByteX);
		if (((*(pGD_Hot_Hand_Kontur+cHSmMixVal)).Position<100)&&((*(pGD_Hot_Hand_Kontur+cHSmMixVal)).Position>0))
		{
			pGD_TControl_Tepl_Kontur->PumpPause=cPausePump;
			if (pGD_TControl_Tepl_Kontur->DoT>4000) 
				pGD_TControl_Tepl_Kontur->PumpPause+=cPausePump;
		}
		if (pGD_TControl_Tepl_Kontur->PumpPause>0) 
		{
			pGD_TControl_Tepl_Kontur->PumpPause--;
//			SetBit(pGD_Hot_Tepl->ExtRCS,cbPausePumpTepl);
		}
		else	pGD_TControl_Tepl_Kontur->PumpPause=0;	
	}
	

}


