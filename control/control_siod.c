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

#warning "change vertical params layout to horizontal"
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

void SetUpSiod(const gh_t *gh)
{
    //uint16_t numConf;
    //uint16_t confGroup[8][9];
    char equalConf;
    char nMas, nCon;

    if (! gh->gh_ctrl->sio_SVal) return;                        // нет клапанов

    gh->hot->OtherCalc.TimeSiod= gh->tcontrol_tepl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas == fnTepl) continue;
//	  if (GD.TControl.Tepl[nMas].FazaSiod) return;
//	}

    for (nCon=0; nCon<_GD.Control.ConfSTepl;nCon++)
    {
        if (nCon != gh->idx)
        {
            if (   (gh->mech_cfg->RNum[cHSmSIOPump] == _GD.MechConfig[nCon].RNum[cHSmSIOPump])
                && (_GD.TControl.Tepl[nCon].FazaSiod))
                return;
        }
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

    if (! gh->hot->AllTask.SIO) return;   // нет задания
    //if (pGD_TControl_Tepl->PauseSIO<1440)
    //	pGD_TControl_Tepl->PauseSIO++;
    if (gh->tcontrol_tepl->PauseSIO < 1440*60)
        gh->tcontrol_tepl->PauseSIO++;


    if ((   ((gh->hot->InTeplSens[cSmRHSens].Value - gh->hot->AllTask.DoRHAir)>_GD.TuneClimate.sio_RHStop)
            ||(gh->hot->InTeplSens[cSmRHSens].Value > 9600))
        &&  gh->hot->AllTask.DoRHAir)
        return;              // если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
    if ((gh->hot->AllTask.DoTHeat-getTempHeat(gh, gh->idx))>_GD.TuneClimate.sio_TStop)
        return;  // если держать больше чем измерено
    if (((getTempHeat(gh, gh->idx) - gh->hot->AllTask.DoTHeat)<_GD.TuneClimate.sio_TStart)
        &&(((gh->hot->AllTask.DoRHAir-gh->hot->InTeplSens[cSmRHSens].Value)<_GD.TuneClimate.sio_RHStart)
           ||(!gh->hot->InTeplSens[cSmRHSens].Value))) return;    // условия для начала работы не выполнены

    int creg_y = getTempHeat(gh, gh->idx)-gh->hot->AllTask.DoTHeat;
    int creg_z;

    CorrectionRule(_GD.TuneClimate.sio_TStart,
                   _GD.TuneClimate.sio_TEnd,
                   _GD.TuneClimate.sio_TStartFactor-_GD.TuneClimate.sio_TEndFactor,
                   0,
                   creg_y,
                   &creg_z);
    int creg_x = (int)(_GD.TuneClimate.sio_TStartFactor-creg_z);

    creg_y = gh->hot->AllTask.DoRHAir-gh->hot->InTeplSens[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.sio_RHStart,
                   _GD.TuneClimate.sio_RHEnd,
                   _GD.TuneClimate.sio_RHStartFactor-_GD.TuneClimate.sio_RHEndFactor,
                   0,
                   creg_y,
                   &creg_z);
    creg_z=_GD.TuneClimate.sio_RHStartFactor - creg_z;

    if ((gh->hot->InTeplSens[cSmRHSens].Value)&&(gh->hot->AllTask.DoRHAir))
    {
        if (creg_x > creg_z)
            creg_x = creg_z;
    }

    ctx.fnSIOpause[gh->idx] = gh->tcontrol_tepl->PauseSIO;     // out

    if (gh->tcontrol_tepl->PauseSIO < creg_x*60)
        return;        // проверка паузы между вкл

    gh->tcontrol_tepl->FazaSiod=cSIOFazaPump;
    gh->tcontrol_tepl->TimeSIO+=gh->hot->AllTask.SIO;
    gh->tcontrol_tepl->PauseSIO=0;
    gh->tcontrol_tepl->CurVal=0;
}

void DoSiod(const gh_t *gh)
{
    char NSIO;

    //if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
    //else return;

    if (! YesBit(gh->hand[cHSmSIOVals].RCS,cbManMech))
        gh->hand[cHSmSIOVals].Position=0;
    else
        return;

    NSIO = gh->gh_ctrl->sio_SVal;
    if (NSIO > cNumValSiodMax)
        NSIO = cNumValSiodMax;


    ctx.fnSIOfaza[gh->idx] = gh->tcontrol_tepl->FazaSiod;  // out

    switch (gh->tcontrol_tepl->FazaSiod)
    {
    case cSIOFazaVal:
        {
            int creg_x = 1 << (gh->tcontrol_tepl->CurVal%4);
            //fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out
            SetBit(gh->hand[cHSmSIOVals].Position, creg_x);
        }
        if (!gh->tcontrol_tepl->TPauseSIO)  gh->tcontrol_tepl->TPauseSIO = gh->hot->AllTask.SIO;
        #warning "falling thru. is it ok ?"
    case cSIOFazaPause:
        if (!gh->tcontrol_tepl->TPauseSIO)
        {
            gh->tcontrol_tepl->TPauseSIO=sio_ValPause;
            gh->tcontrol_tepl->CurVal++;

            ctx.fnSIOvelvOut[gh->idx] = gh->tcontrol_tepl->CurVal;           // out
        }
    case cSIOFazaPump:
        gh->hand[cHSmSIOPump].Position = 1;
        if (!gh->tcontrol_tepl->TPauseSIO)  gh->tcontrol_tepl->TPauseSIO=sio_ValPause;

        ctx.fnSIOpumpOut[gh->idx] = gh->tcontrol_tepl->TPauseSIO;            // out

        break;

    case cSIOFazaEnd:
        gh->tcontrol_tepl->FazaSiod=0;
        gh->hand[cHSmSIOPump].Position = 0;       // new
        return;

    default:
        return;
    }
    if (--gh->tcontrol_tepl->TPauseSIO)  return;

    if ((gh->tcontrol_tepl->FazaSiod == cSIOFazaPause)&&(gh->tcontrol_tepl->CurVal<NSIO))
        gh->tcontrol_tepl->FazaSiod=cSIOFazaVal;
    else
        gh->tcontrol_tepl->FazaSiod++;

}
