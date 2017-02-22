#define _FRIEND_OF_CONTROL_

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
    for (tepl=0;tepl<_GD.Control.ConfSTepl;tepl++)
        _GD.TControl.Tepl[tepl].PauseSIO = 0;
}

void SetUpSiod(char fnTepl)
{
    //uint16_t numConf;
    //uint16_t confGroup[8][9];
    char equalConf;
    char nMas, nCon;

    if (!_GDP.Control_Tepl->sio_SVal) return;                        // нет клапанов
    _GDP.Hot_Tepl->OtherCalc.TimeSiod=_GDP.TControl_Tepl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas == fnTepl) continue;
//	  if (GD.TControl.Tepl[nMas].FazaSiod) return;
//	}

    for (nCon=0; nCon<_GD.Control.ConfSTepl;nCon++)
    {
        if (nCon != fnTepl)
            if ((_GD.MechConfig[fnTepl].RNum[cHSmSIOPump] == _GD.MechConfig[nCon].RNum[cHSmSIOPump]) && (_GD.TControl.Tepl[nCon].FazaSiod))
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

    if (!_GDP.Hot_Tepl->AllTask.SIO) return;   // нет задания
    //if (pGD_TControl_Tepl->PauseSIO<1440)
    //	pGD_TControl_Tepl->PauseSIO++;
    if (_GDP.TControl_Tepl->PauseSIO<1440*60)
        _GDP.TControl_Tepl->PauseSIO++;


    creg.X=0;
    if ((((_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value-_GDP.Hot_Tepl->AllTask.DoRHAir)>_GD.TuneClimate.sio_RHStop)
         ||(_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value>9600))
        &&(_GDP.Hot_Tepl->AllTask.DoRHAir)) return;              // если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
    if ((_GDP.Hot_Tepl->AllTask.DoTHeat-getTempHeat(fnTepl))>_GD.TuneClimate.sio_TStop) return;  // если держать больше чем измерено
    if (((getTempHeat(fnTepl)-_GDP.Hot_Tepl->AllTask.DoTHeat)<_GD.TuneClimate.sio_TStart)
        &&(((_GDP.Hot_Tepl->AllTask.DoRHAir-_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value)<_GD.TuneClimate.sio_RHStart)
           ||(!_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value))) return;    // условия для начала работы не выполнены

    creg.Y=getTempHeat(fnTepl)-_GDP.Hot_Tepl->AllTask.DoTHeat;
    CorrectionRule(_GD.TuneClimate.sio_TStart,_GD.TuneClimate.sio_TEnd,_GD.TuneClimate.sio_TStartFactor-_GD.TuneClimate.sio_TEndFactor,0);
    creg.X=(int)(_GD.TuneClimate.sio_TStartFactor-creg.Z);

    creg.Y=_GDP.Hot_Tepl->AllTask.DoRHAir-_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.sio_RHStart,_GD.TuneClimate.sio_RHEnd,_GD.TuneClimate.sio_RHStartFactor-_GD.TuneClimate.sio_RHEndFactor,0);
    creg.Z=_GD.TuneClimate.sio_RHStartFactor-creg.Z;
    if ((_GDP.Hot_Tepl->InTeplSens[cSmRHSens].Value)&&(_GDP.Hot_Tepl->AllTask.DoRHAir))
        if (creg.X>creg.Z)
            creg.X=creg.Z;

    ctx.fnSIOpause[fnTepl] = _GDP.TControl_Tepl->PauseSIO;     // out

    if (_GDP.TControl_Tepl->PauseSIO<creg.X*60) return;        // проверка паузы между вкл

    _GDP.TControl_Tepl->FazaSiod=cSIOFazaPump;
    _GDP.TControl_Tepl->TimeSIO+=_GDP.Hot_Tepl->AllTask.SIO;
    _GDP.TControl_Tepl->PauseSIO=0;
    _GDP.TControl_Tepl->CurVal=0;
}

void DoSiod(char fnTepl)
{
    char NSIO;

    //if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
    //else return;

    if (!(YesBit((*(_GDP.Hot_Hand+cHSmSIOVals)).RCS,cbManMech))) (*(_GDP.Hot_Hand+cHSmSIOVals)).Position=0;
    else return;

    NSIO=_GDP.Control_Tepl->sio_SVal;
    if (cNumValSiodMax<_GDP.Control_Tepl->sio_SVal)
        NSIO=cNumValSiodMax;


    ctx.fnSIOfaza[fnTepl] = _GDP.TControl_Tepl->FazaSiod;  // out

    switch (_GDP.TControl_Tepl->FazaSiod)
    {
    case cSIOFazaVal:
        creg.X=1;
        creg.X<<=(_GDP.TControl_Tepl->CurVal%4);

        //fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out

        SetBit((*(_GDP.Hot_Hand+cHSmSIOVals)).Position,creg.X);
        if (!_GDP.TControl_Tepl->TPauseSIO)  _GDP.TControl_Tepl->TPauseSIO=_GDP.Hot_Tepl->AllTask.SIO;
    case cSIOFazaPause:
        if (!_GDP.TControl_Tepl->TPauseSIO)
        {
            _GDP.TControl_Tepl->TPauseSIO=sio_ValPause;
            _GDP.TControl_Tepl->CurVal++;

            ctx.fnSIOvelvOut[fnTepl] = _GDP.TControl_Tepl->CurVal;           // out
        }
    case cSIOFazaPump:
        (*(_GDP.Hot_Hand+cHSmSIOPump)).Position=1;
        if (!_GDP.TControl_Tepl->TPauseSIO)  _GDP.TControl_Tepl->TPauseSIO=sio_ValPause;

        ctx.fnSIOpumpOut[fnTepl] = _GDP.TControl_Tepl->TPauseSIO;            // out

        break;

    case cSIOFazaEnd:
        _GDP.TControl_Tepl->FazaSiod=0;
        (*(_GDP.Hot_Hand+cHSmSIOPump)).Position=0;       // new
        return;

    default:
        return;
    }
    if (--_GDP.TControl_Tepl->TPauseSIO)  return;
    if ((_GDP.TControl_Tepl->FazaSiod == cSIOFazaPause)&&(_GDP.TControl_Tepl->CurVal<NSIO)) _GDP.TControl_Tepl->FazaSiod=cSIOFazaVal;
    else _GDP.TControl_Tepl->FazaSiod++;

}
