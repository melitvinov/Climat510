#define cSIOFazaPump	1
#define cSIOFazaVal		2
#define cSIOFazaPause	3
#define cSIOFazaEnd		4

//uint16_t fnSIOfaza[8];
//uint16_t fnSIOpumpOut[8];
//uint16_t fnSIOvelvOut[8];
//uint16_t fnSIOpause[8];

void siodInit()
{
	char tepl;
	for (tepl=0;tepl<GD.Control.ConfSTepl;tepl++)
		GD.TControl.Tepl[tepl].PauseSIO = 0;
}

void SetUpSiod(char fnTepl)
{
	//uint16_t numConf;
	//uint16_t confGroup[8][9];
	char equalConf;
	char nMas, nCon;

	if (!pGD_Control_Tepl->sio_SVal) return;  						// нет клапанов
	pGD_Hot_Tepl->OtherCalc.TimeSiod=pGD_TControl_Tepl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas==fnTepl) continue;
//	  if (GD.TControl.Tepl[nMas].FazaSiod) return;
//	}

	for (nCon=0; nCon<GD.Control.ConfSTepl;nCon++)
	{
		if (nCon != fnTepl)
			if ((GD.MechConfig[fnTepl].RNum[cHSmSIOPump] == GD.MechConfig[nCon].RNum[cHSmSIOPump]) && (GD.TControl.Tepl[nCon].FazaSiod))
				return;
	}

// new
/*	numConf = 0;
	numZone = 1;
	for (nCon=0; nCon<GD.Control.ConfSTepl;nCon++)
	{
		if (GD.MechConfig[nCon].RNum[cHSmSIOPump]==0) continue;
		equalConf = 0;
		for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)
		{
			if (confGroup[nMas][0] == GD.MechConfig[nCon].RNum[cHSmSIOPump])
				equalConf = 1;
		}
		if (!equalConf)
		{
			confGroup[numConf][0] = GD.MechConfig[nCon].RNum[cHSmSIOPump];
			numConf++;
		}
	}

	for (nCon=0; nCon<GD.Control.ConfSTepl;nCon++)
	{
		for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)
		{
			if (confGroup[nMas][0] == GD.MechConfig[nCon].RNum[cHSmSIOPump])
			{
				for (nCount=0; nCount<9;nCount++)
				{
					if (confGroup[nMas][nCount+1] == 9)
					{
						confGroup[nMas][nCount+1] = nCon;
						break;
					}
				}
			}
		}
	}
*/

// старое
/*	equalConf = 1;
	numConf = GD.MechConfig[0].RNum[cHSmSIOPump];
	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)
	{
		if (numConf == GD.MechConfig[nMas].RNum[cHSmSIOPump])
		{
			equalConf = 1;
		}
		else
			{
				equalConf = 0;
				break;
			}
	}
	//fnSIOvelvOut[fnTepl] = equalConf;
	if (equalConf)
	{
		for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)
			if (GD.TControl.Tepl[nMas].FazaSiod)
				return;
	}
*/

	//if (GD.TControl.Tepl[fnTepl].FazaSiod) return;

	if (!pGD_Hot_Tepl->AllTask.SIO) return;   // нет задания
	//if (pGD_TControl_Tepl->PauseSIO<1440)
	//	pGD_TControl_Tepl->PauseSIO++;
	if (pGD_TControl_Tepl->PauseSIO<1440*60)
		pGD_TControl_Tepl->PauseSIO++;


	IntX=0;
	if ((((pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value-pGD_Hot_Tepl->AllTask.DoRHAir)>GD.TuneClimate.sio_RHStop)
		||(pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value>9600))
		&&(pGD_Hot_Tepl->AllTask.DoRHAir)) return;				// если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
	if ((pGD_Hot_Tepl->AllTask.DoTHeat-getTempHeat(fnTepl))>GD.TuneClimate.sio_TStop) return;  // если держать больше чем измерено
	if (((getTempHeat(fnTepl)-pGD_Hot_Tepl->AllTask.DoTHeat)<GD.TuneClimate.sio_TStart)
		&&(((pGD_Hot_Tepl->AllTask.DoRHAir-pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value)<GD.TuneClimate.sio_RHStart)
		||(!pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value))) return;	// условия для начала работы не выполнены

	IntY=getTempHeat(fnTepl)-pGD_Hot_Tepl->AllTask.DoTHeat;
	CorrectionRule(GD.TuneClimate.sio_TStart,GD.TuneClimate.sio_TEnd,GD.TuneClimate.sio_TStartFactor-GD.TuneClimate.sio_TEndFactor,0);
	IntX=(int)(GD.TuneClimate.sio_TStartFactor-IntZ);

	IntY=pGD_Hot_Tepl->AllTask.DoRHAir-pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value;
	CorrectionRule(GD.TuneClimate.sio_RHStart,GD.TuneClimate.sio_RHEnd,GD.TuneClimate.sio_RHStartFactor-GD.TuneClimate.sio_RHEndFactor,0);
	IntZ=GD.TuneClimate.sio_RHStartFactor-IntZ;
	if ((pGD_Hot_Tepl->InTeplSens[cSmRHSens].Value)&&(pGD_Hot_Tepl->AllTask.DoRHAir))
		if (IntX>IntZ) 
			IntX=IntZ;

//	fnSIOpause[fnTepl] = pGD_TControl_Tepl->PauseSIO;     // out

	if (pGD_TControl_Tepl->PauseSIO<IntX*60) return;		// проверка паузы между вкл
	if (YesBit(RegLEV,cSmSIONo)) return;

	pGD_TControl_Tepl->FazaSiod=cSIOFazaPump;
	pGD_TControl_Tepl->TimeSIO+=pGD_Hot_Tepl->AllTask.SIO;
	pGD_TControl_Tepl->PauseSIO=0;
	pGD_TControl_Tepl->CurVal=0;
}

void DoSiod(char fnTepl)
{
	char NSIO;

	//if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
	//else return;

	if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOVals)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOVals)).Position=0;
	else return;

	NSIO=pGD_Control_Tepl->sio_SVal;
	if (cNumValSiodMax<pGD_Control_Tepl->sio_SVal)
		NSIO=cNumValSiodMax;


//	fnSIOfaza[fnTepl] = pGD_TControl_Tepl->FazaSiod;  // out

	switch(pGD_TControl_Tepl->FazaSiod)
	{
	case cSIOFazaVal:
		IntX=1;
		IntX<<=(pGD_TControl_Tepl->CurVal%4);

		//fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out

		SetBit((*(pGD_Hot_Hand+cHSmSIOVals)).Position,IntX);
		if (!pGD_TControl_Tepl->TPauseSIO)	pGD_TControl_Tepl->TPauseSIO=pGD_Hot_Tepl->AllTask.SIO;	
	case cSIOFazaPause:
		if (!pGD_TControl_Tepl->TPauseSIO)	
		{
			pGD_TControl_Tepl->TPauseSIO=sio_ValPause;
			pGD_TControl_Tepl->CurVal++;

//			fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out
		}	
	case cSIOFazaPump:
		(*(pGD_Hot_Hand+cHSmSIOPump)).Position=1;
		if (!pGD_TControl_Tepl->TPauseSIO)	pGD_TControl_Tepl->TPauseSIO=sio_ValPause;

//		fnSIOpumpOut[fnTepl] = pGD_TControl_Tepl->TPauseSIO;			// out

		break;

	case cSIOFazaEnd:
		pGD_TControl_Tepl->FazaSiod=0;
		(*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;		// new
		return;

	default:
		return;
	}
	if(--pGD_TControl_Tepl->TPauseSIO)	return;	
	if ((pGD_TControl_Tepl->FazaSiod==cSIOFazaPause)&&(pGD_TControl_Tepl->CurVal<NSIO)) pGD_TControl_Tepl->FazaSiod=cSIOFazaVal;
	else pGD_TControl_Tepl->FazaSiod++;

}
