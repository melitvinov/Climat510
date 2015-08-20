/*uint16_t airHeatGetHeatPause(char fnTepl)
{
	return airHeatPause[fnTepl];
}

void airHeatSetHeatPause(char timeInc, char fnTepl)
{
	if (timeInc)
		airHeatPause[fnTepl] = airHeatPause[fnTepl] + timeInc;
	else
		airHeatPause[fnTepl] = 0;
}

uint16_t airHeatGetTimeWork(char fnTepl)
{
	return airHeatTimeWork[fnTepl];
}

void airHeatSetTimeWork(char timeInc, char fnTepl)
{
	if (timeInc)
		airHeatTimeWork[fnTepl] = airHeatTimeWork[fnTepl] + timeInc;
	else
		airHeatTimeWork[fnTepl] = 0;
}

void airHeatOn(char fnTepl)
{
	GD.Hot.Tepl[fnTepl].HandCtrl[cHSmHeat].Position = 1;
	//(*(pGD_Hot_Hand+cHSmHeat)).Position=1;//pGD_TControl_Tepl->Calorifer;
	airHeatOnOff[fnTepl]=1;
	airHeatSetHeatPause(1, fnTepl);
}

void airHeatOff(char fnTepl)
{
	GD.Hot.Tepl[fnTepl].HandCtrl[cHSmHeat].Position = 0;
	airHeatSetTimeWork(0, fnTepl);
	airHeatSetHeatPause(0, fnTepl);
	airHeatSetHeatPause(1, fnTepl);  // запускаем счетчик паузы
	airHeatOnOff[fnTepl]=0;
}

void airHeatInit(void)
{
	char tCTepl;
	for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
	{
		airHeatPause[tCTepl] = 0;
		airHeatTimeWork[tCTepl] = 0;
		airHeatOnOff[tCTepl] = 0;
		//airHeatSetTimeWork(GD.TuneClimate.airHeatMinWork,tCTepl);  // что бы сработывало условие выключения обогревателя при вервом старте
		airHeatOff(tCTepl);
	}
}

void airHeatTimers(void)
{
	char tCTepl;
	for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
	{
//		if ( tCTepl != 0) return;
		if (airHeatOnOff[tCTepl] == 1)
		//if (pGD_Hot_Tepl->airHeatOnOff == 1)
		{
			if (airHeatGetTimeWork(tCTepl) >= (GD.TuneClimate.airHeatMaxWork / 100)) // если обогреватель включен проверяем макс время работы
				airHeatOff(tCTepl);
			else
				airHeatSetTimeWork(1,tCTepl);	// если обогреватель включен увеличиваем его время на 1 мин
		}
		if (airHeatOnOff[tCTepl] == 0)
		//if (pGD_Hot_Tepl->airHeatOnOff == 0)   // если обогреватель выключен увеличиваем его время паузы на 1 мин
		{
			if (airHeatGetHeatPause(tCTepl) >= (GD.TuneClimate.airHeatPauseWork / 100))
				airHeatSetHeatPause(0, tCTepl);	// 0 означает что обогреватель может быть включен
			else if (airHeatGetHeatPause(tCTepl) > 0)
				airHeatSetHeatPause(1, tCTepl);	    // считаем паузу пока она не обнулится
		}
	}
}

void airHeat(char fnTepl)
{
	int16_t tempT;
//	if ( fnTepl != 0) return;
	tempT = getTempHeat(fnTepl);
	if (tempT > 0)
	{
		if ((GD.TuneClimate.airHeatTemperOn >= tempT) && (GD.TuneClimate.airHeatTemperOff > tempT) && (airHeatGetHeatPause(fnTepl) == 0))  // обогреватель можно вкл и пауза между вкл прошла
			airHeatOn(fnTepl);
		if ((GD.TuneClimate.airHeatTemperOff <= tempT) && (airHeatGetTimeWork(fnTepl) >= (GD.TuneClimate.airHeatMinWork / 100)))  // обогреватель можно выклюсить если мин время работы прошло и максимальная температура достигнута
			airHeatOff(fnTepl);
	}
	GD.Hot.Tepl[fnTepl].airHeatTimeWork = airHeatTimeWork[fnTepl];
	GD.Hot.Tepl[fnTepl].airHeatOnOff = airHeatOnOff[fnTepl];
}*/
