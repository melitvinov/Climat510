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

void SetUpSiod(const gh_t *me)
{
    //uint16_t numConf;
    //uint16_t confGroup[8][9];
    char equalConf;
    char nMas, nCon;

    if (! me->gh_ctrl->sio_SVal) return;                        // нет клапанов

    me->gh_base->OtherCalc.TimeSiod= me->gh_tctrl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas == fnTepl) continue;
//	  if (GD.TControl.Tepl[nMas].FazaSiod) return;
//	}

    for (nCon=0; nCon<_GD.Control.ConfSTepl;nCon++)
    {
        if (nCon != me->idx)
        {
            if ((_GD.MechConfig[me->idx].RNum[cHSmSIOPump] == _GD.MechConfig[nCon].RNum[cHSmSIOPump]) && (_GD.TControl.Tepl[nCon].FazaSiod))
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

    if (! me->gh_base->AllTask.SIO) return;   // нет задания
    //if (pGD_TControl_Tepl->PauseSIO<1440)
    //	pGD_TControl_Tepl->PauseSIO++;
    if ( me->gh_tctrl->PauseSIO < 1440*60)
        me->gh_tctrl->PauseSIO++;


    creg.X=0;
    if (
         (
              ((me->gh_base->InTeplSens[cSmRHSens].Value - me->gh_base->AllTask.DoRHAir)>_GD.TuneClimate.sio_RHStop)
            ||(me->gh_base->InTeplSens[cSmRHSens].Value > 9600)
         )
         &&  me->gh_base->AllTask.DoRHAir)
        return;              // если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
    if ((me->gh_base->AllTask.DoTHeat-getTempHeat(me->idx))>_GD.TuneClimate.sio_TStop) return;  // если держать больше чем измерено
    if (((getTempHeat(me->idx)-me->gh_base->AllTask.DoTHeat)<_GD.TuneClimate.sio_TStart)
        &&(((me->gh_base->AllTask.DoRHAir-me->gh_base->InTeplSens[cSmRHSens].Value)<_GD.TuneClimate.sio_RHStart)
           ||(!me->gh_base->InTeplSens[cSmRHSens].Value))) return;    // условия для начала работы не выполнены

    creg.Y=getTempHeat(me->idx)-me->gh_base->AllTask.DoTHeat;
    CorrectionRule(_GD.TuneClimate.sio_TStart,_GD.TuneClimate.sio_TEnd,_GD.TuneClimate.sio_TStartFactor-_GD.TuneClimate.sio_TEndFactor,0);
    creg.X=(int)(_GD.TuneClimate.sio_TStartFactor-creg.Z);

    creg.Y=me->gh_base->AllTask.DoRHAir-me->gh_base->InTeplSens[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.sio_RHStart,_GD.TuneClimate.sio_RHEnd,_GD.TuneClimate.sio_RHStartFactor-_GD.TuneClimate.sio_RHEndFactor,0);
    creg.Z=_GD.TuneClimate.sio_RHStartFactor-creg.Z;
    if ((me->gh_base->InTeplSens[cSmRHSens].Value)&&(me->gh_base->AllTask.DoRHAir))
        if (creg.X>creg.Z)
            creg.X=creg.Z;

    ctx.fnSIOpause[me->idx] = me->gh_tctrl->PauseSIO;     // out

    if (me->gh_tctrl->PauseSIO<creg.X*60) return;        // проверка паузы между вкл

    me->gh_tctrl->FazaSiod=cSIOFazaPump;
    me->gh_tctrl->TimeSIO+=me->gh_base->AllTask.SIO;
    me->gh_tctrl->PauseSIO=0;
    me->gh_tctrl->CurVal=0;
}

void DoSiod(const gh_t *me)
{
    char NSIO;

    //if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
    //else return;

    if (! YesBit(me->hand[cHSmSIOVals].RCS,cbManMech))
        me->hand[cHSmSIOVals].Position=0;
    else
        return;

    NSIO = me->gh_ctrl->sio_SVal;
    if (NSIO > cNumValSiodMax)
        NSIO = cNumValSiodMax;


    ctx.fnSIOfaza[me->idx] = me->gh_tctrl->FazaSiod;  // out

    switch (me->gh_tctrl->FazaSiod)
    {
    case cSIOFazaVal:
        creg.X=1;
        creg.X<<=(me->gh_tctrl->CurVal%4);

        //fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out

        SetBit(me->hand[cHSmSIOVals].Position, creg.X);
        if (!me->gh_tctrl->TPauseSIO)  me->gh_tctrl->TPauseSIO = me->gh_base->AllTask.SIO;
        #warning "falling thru. is it ok ?"
    case cSIOFazaPause:
        if (!me->gh_tctrl->TPauseSIO)
        {
            me->gh_tctrl->TPauseSIO=sio_ValPause;
            me->gh_tctrl->CurVal++;

            ctx.fnSIOvelvOut[me->idx] = me->gh_tctrl->CurVal;           // out
        }
    case cSIOFazaPump:
        me->hand[cHSmSIOPump].Position = 1;
        if (!me->gh_tctrl->TPauseSIO)  me->gh_tctrl->TPauseSIO=sio_ValPause;

        ctx.fnSIOpumpOut[me->idx] = me->gh_tctrl->TPauseSIO;            // out

        break;

    case cSIOFazaEnd:
        me->gh_tctrl->FazaSiod=0;
        me->hand[cHSmSIOPump].Position = 0;       // new
        return;

    default:
        return;
    }
    if (--me->gh_tctrl->TPauseSIO)  return;

    if ((me->gh_tctrl->FazaSiod == cSIOFazaPause)&&(me->gh_tctrl->CurVal<NSIO))
        me->gh_tctrl->FazaSiod=cSIOFazaVal;
    else
        me->gh_tctrl->FazaSiod++;

}
