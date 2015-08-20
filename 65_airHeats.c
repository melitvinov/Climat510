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
	airHeatSetHeatPause(1, fnTepl);  // ��������� ������� �����
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
		//airHeatSetTimeWork(GD.TuneClimate.airHeatMinWork,tCTepl);  // ��� �� ����������� ������� ���������� ������������ ��� ������ ������
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
			if (airHeatGetTimeWork(tCTepl) >= (GD.TuneClimate.airHeatMaxWork / 100)) // ���� ������������ ������� ��������� ���� ����� ������
				airHeatOff(tCTepl);
			else
				airHeatSetTimeWork(1,tCTepl);	// ���� ������������ ������� ����������� ��� ����� �� 1 ���
		}
		if (airHeatOnOff[tCTepl] == 0)
		//if (pGD_Hot_Tepl->airHeatOnOff == 0)   // ���� ������������ �������� ����������� ��� ����� ����� �� 1 ���
		{
			if (airHeatGetHeatPause(tCTepl) >= (GD.TuneClimate.airHeatPauseWork / 100))
				airHeatSetHeatPause(0, tCTepl);	// 0 �������� ��� ������������ ����� ���� �������
			else if (airHeatGetHeatPause(tCTepl) > 0)
				airHeatSetHeatPause(1, tCTepl);	    // ������� ����� ���� ��� �� ���������
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
		if ((GD.TuneClimate.airHeatTemperOn >= tempT) && (GD.TuneClimate.airHeatTemperOff > tempT) && (airHeatGetHeatPause(fnTepl) == 0))  // ������������ ����� ��� � ����� ����� ��� ������
			airHeatOn(fnTepl);
		if ((GD.TuneClimate.airHeatTemperOff <= tempT) && (airHeatGetTimeWork(fnTepl) >= (GD.TuneClimate.airHeatMinWork / 100)))  // ������������ ����� ��������� ���� ��� ����� ������ ������ � ������������ ����������� ����������
			airHeatOff(fnTepl);
	}
	GD.Hot.Tepl[fnTepl].airHeatTimeWork = airHeatTimeWork[fnTepl];
	GD.Hot.Tepl[fnTepl].airHeatOnOff = airHeatOnOff[fnTepl];
}*/
