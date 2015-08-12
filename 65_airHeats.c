void airHeatInit(void)
{
	char tCTepl;
	for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
		airHeatPause[tCTepl] = 0;
}

void airHeatOn(void)
{
	(*(pGD_Hot_Hand+cHSmHeat)).Position=pGD_TControl_Tepl->Calorifer;
	pGD_Hot_Tepl->airHeatOnOff=1;
}

void airHeatOff(char fnTepl)
{
	(*(pGD_Hot_Hand+cHSmHeat)).Position=0;
	airHeatSetTimeWork(0);
	airHeatPause[fnTepl] = 1;  // запускаем счетчик паузы
	pGD_Hot_Tepl->airHeatOnOff=0;
}

uint16_t airHeatGetTimeWork(void)
{
	return pGD_Hot_Tepl->airHeatTimeWork;
}

void airHeatSetTimeWork(char timeInc)
{
	if (timeInc)
		pGD_Hot_Tepl->airHeatTimeWork++;
	else
		pGD_Hot_Tepl->airHeatTimeWork = 0;
}

void airHeatTimers(void)
{
	char tCTepl;
	for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
	{
		if (pGD_Hot_Tepl->airHeatOnOff == 1)
		{
			if (airHeatGetTimeWork() >= GD.TuneClimate.airHeatMaxWork) // если обогреватель включен проверяем макс время работы
				airHeatOff(tCTepl);
			else
				airHeatSetTimeWork(1);		// если обогреватель включен увеличиваем его время на 1 мин
		}
		if (pGD_Hot_Tepl->airHeatOnOff == 0)   // если обогреватель выключен увеличиваем его время паузы на 1 мин
		{
			if (airHeatPause[tCTepl] >= GD.TuneClimate.airHeatPauseWork)
				airHeatPause[tCTepl] = 0;	// 0 означает что обогреватель может быть включен
			else if (airHeatPause[tCTepl] > 0)
				airHeatPause[tCTepl]++;	    // считаем паузу пока она не обнулится
		}
	}
}

void airHeat(char fnTepl)
{
	int16_t tempT;
	tempT = getTempHeat(fnTepl);
	if (tempT > 0)
	{
		if ((GD.TuneClimate.airHeatTemperOn >= tempT) && (airHeatPause[fnTepl] == 0))  // обогреватель можно вкл и пауза между вкл прошла
			airHeatOn();
		if ((GD.TuneClimate.airHeatTemperOff >= tempT) && (airHeatGetTimeWork() >= GD.TuneClimate.airHeatMinWork))  // обогреватель можно выклюсить если мин время работы прошло и максимальная температура достигнута
			airHeatOff(fnTepl);
	}
}
