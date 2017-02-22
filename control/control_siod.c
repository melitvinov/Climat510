#include "syntax.h"
#include "control_gd.h"
#include "control_subr.h"
#include "control.h"
#include "control_siod.h"

#define cSIOFazaPump	1
#define cSIOFazaVal		2
#define cSIOFazaPause	3
#define cSIOFazaEnd		4

#define cNumValSiodMax	4

static siod_ctx_t ctx;

const siod_ctx_t *siod_view(void)
{
    return &ctx;
}

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

    if (!gdp.Control_Tepl->sio_SVal) return;                        // нет клапанов
    gdp.Hot_Tepl->OtherCalc.TimeSiod=gdp.TControl_Tepl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas == fnTepl) continue;
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
        if (GD.MechConfig[nCon].RNum[cHSmSIOPump] == 0) continue;
        equalConf = 0;
        for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)
        {
            if (confGroup[nMas][0]  ==  GD.MechConfig[nCon].RNum[cHSmSIOPump])
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
            if (confGroup[nMas][0]  ==  GD.MechConfig[nCon].RNum[cHSmSIOPump])
            {
                for (nCount=0; nCount<9;nCount++)
                {
                    if (confGroup[nMas][nCount+1]  ==  9)
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
        if (numConf  ==  GD.MechConfig[nMas].RNum[cHSmSIOPump])
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

    if (!gdp.Hot_Tepl->AllTask.SIO) return;   // нет задания
    //if (pGD_TControl_Tepl->PauseSIO<1440)
    //	pGD_TControl_Tepl->PauseSIO++;
    if (gdp.TControl_Tepl->PauseSIO<1440*60)
        gdp.TControl_Tepl->PauseSIO++;


    creg.X=0;
    if ((((gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value-gdp.Hot_Tepl->AllTask.DoRHAir)>GD.TuneClimate.sio_RHStop)
         ||(gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value>9600))
        &&(gdp.Hot_Tepl->AllTask.DoRHAir)) return;              // если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
    if ((gdp.Hot_Tepl->AllTask.DoTHeat-getTempHeat(fnTepl))>GD.TuneClimate.sio_TStop) return;  // если держать больше чем измерено
    if (((getTempHeat(fnTepl)-gdp.Hot_Tepl->AllTask.DoTHeat)<GD.TuneClimate.sio_TStart)
        &&(((gdp.Hot_Tepl->AllTask.DoRHAir-gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value)<GD.TuneClimate.sio_RHStart)
           ||(!gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value))) return;    // условия для начала работы не выполнены

    creg.Y=getTempHeat(fnTepl)-gdp.Hot_Tepl->AllTask.DoTHeat;
    CorrectionRule(GD.TuneClimate.sio_TStart,GD.TuneClimate.sio_TEnd,GD.TuneClimate.sio_TStartFactor-GD.TuneClimate.sio_TEndFactor,0);
    creg.X=(int)(GD.TuneClimate.sio_TStartFactor-creg.Z);

    creg.Y=gdp.Hot_Tepl->AllTask.DoRHAir-gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value;
    CorrectionRule(GD.TuneClimate.sio_RHStart,GD.TuneClimate.sio_RHEnd,GD.TuneClimate.sio_RHStartFactor-GD.TuneClimate.sio_RHEndFactor,0);
    creg.Z=GD.TuneClimate.sio_RHStartFactor-creg.Z;
    if ((gdp.Hot_Tepl->InTeplSens[cSmRHSens].Value)&&(gdp.Hot_Tepl->AllTask.DoRHAir))
        if (creg.X>creg.Z)
            creg.X=creg.Z;

    ctx.fnSIOpause[fnTepl] = gdp.TControl_Tepl->PauseSIO;     // out

    if (gdp.TControl_Tepl->PauseSIO<creg.X*60) return;        // проверка паузы между вкл

    gdp.TControl_Tepl->FazaSiod=cSIOFazaPump;
    gdp.TControl_Tepl->TimeSIO+=gdp.Hot_Tepl->AllTask.SIO;
    gdp.TControl_Tepl->PauseSIO=0;
    gdp.TControl_Tepl->CurVal=0;
}

void DoSiod(char fnTepl)
{
    char NSIO;

    //if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
    //else return;

    if (!(YesBit((*(gdp.Hot_Hand+cHSmSIOVals)).RCS,cbManMech))) (*(gdp.Hot_Hand+cHSmSIOVals)).Position=0;
    else return;

    NSIO=gdp.Control_Tepl->sio_SVal;
    if (cNumValSiodMax<gdp.Control_Tepl->sio_SVal)
        NSIO=cNumValSiodMax;


    ctx.fnSIOfaza[fnTepl] = gdp.TControl_Tepl->FazaSiod;  // out

    switch (gdp.TControl_Tepl->FazaSiod)
    {
    case cSIOFazaVal:
        creg.X=1;
        creg.X<<=(gdp.TControl_Tepl->CurVal%4);

        //fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out

        SetBit((*(gdp.Hot_Hand+cHSmSIOVals)).Position,creg.X);
        if (!gdp.TControl_Tepl->TPauseSIO)  gdp.TControl_Tepl->TPauseSIO=gdp.Hot_Tepl->AllTask.SIO;
    case cSIOFazaPause:
        if (!gdp.TControl_Tepl->TPauseSIO)
        {
            gdp.TControl_Tepl->TPauseSIO=sio_ValPause;
            gdp.TControl_Tepl->CurVal++;

            ctx.fnSIOvelvOut[fnTepl] = gdp.TControl_Tepl->CurVal;           // out
        }
    case cSIOFazaPump:
        (*(gdp.Hot_Hand+cHSmSIOPump)).Position=1;
        if (!gdp.TControl_Tepl->TPauseSIO)  gdp.TControl_Tepl->TPauseSIO=sio_ValPause;

        ctx.fnSIOpumpOut[fnTepl] = gdp.TControl_Tepl->TPauseSIO;            // out

        break;

    case cSIOFazaEnd:
        gdp.TControl_Tepl->FazaSiod=0;
        (*(gdp.Hot_Hand+cHSmSIOPump)).Position=0;       // new
        return;

    default:
        return;
    }
    if (--gdp.TControl_Tepl->TPauseSIO)  return;
    if ((gdp.TControl_Tepl->FazaSiod == cSIOFazaPause)&&(gdp.TControl_Tepl->CurVal<NSIO)) gdp.TControl_Tepl->FazaSiod=cSIOFazaVal;
    else gdp.TControl_Tepl->FazaSiod++;

}
