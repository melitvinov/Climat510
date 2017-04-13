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

void siodInit(void)
{
    for (int zone_idx=0; zone_idx<_GD.Control.ConfSTepl; zone_idx++)
    {
        zone_t zone = make_zone_ctx(zone_idx);
        zone.tcontrol_tepl->PauseSIO = 0;
    }
}

void SetUpSiod(const zone_t *zone)
{
    //uint16_t numConf;
    //uint16_t confGroup[8][9];
    char equalConf;
    char nMas, nCon;

    if (! zone->gh_ctrl->sio_SVal) return;                        // нет клапанов

    zone->hot->OtherCalc.TimeSiod= zone->tcontrol_tepl->TimeSIO;

//	for (nMas=0;nMas<GD.Control.ConfSTepl;nMas++)		// !!!
//	{
//	  //if (nMas == fnTepl) continue;
//	  if (GD.TControl.Tepl[nMas].FazaSiod) return;
//	}

    for (nCon=0; nCon<_GD.Control.ConfSTepl;nCon++)
    {
        if (nCon != zone->idx)
        {
            zone_t other_gh = make_zone_ctx(nCon);

            if ((zone->mech_cfg->RNum[cHSmSIOPump] == other_gh.mech_cfg->RNum[cHSmSIOPump])
                && (other_gh.tcontrol_tepl->FazaSiod))
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

    if (! zone->hot->AllTask.SIO)
        return;   // нет задания
    //if (pGD_TControl_Tepl->PauseSIO<1440)
    //	pGD_TControl_Tepl->PauseSIO++;
    if (zone->tcontrol_tepl->PauseSIO < 1440*60)
        zone->tcontrol_tepl->PauseSIO++;


    if ((   ((zone->hot->IndoorSensors[cSmRHSens].Value - zone->hot->AllTask.DoRHAir)>_GD.TuneClimate.sio_RHStop)
            ||(zone->hot->IndoorSensors[cSmRHSens].Value > 9600))
        &&  zone->hot->AllTask.DoRHAir)
        return;              // если достигнута заданная влажность влажность + коэфицент


#warning CHECK THIS
// NEW
    if ((zone->hot->AllTask.DoTHeat-getTempHeat(zone, zone->idx))>_GD.TuneClimate.sio_TStop)
        return;  // если держать больше чем измерено
    if (((getTempHeat(zone, zone->idx) - zone->hot->AllTask.DoTHeat)<_GD.TuneClimate.sio_TStart)
        &&(((zone->hot->AllTask.DoRHAir-zone->hot->IndoorSensors[cSmRHSens].Value)<_GD.TuneClimate.sio_RHStart)
           ||(!zone->hot->IndoorSensors[cSmRHSens].Value))) return;    // условия для начала работы не выполнены

    int creg_y = getTempHeat(zone, zone->idx)-zone->hot->AllTask.DoTHeat;
    int creg_z;

    CorrectionRule(_GD.TuneClimate.sio_TStart,
                   _GD.TuneClimate.sio_TEnd,
                   _GD.TuneClimate.sio_TStartFactor-_GD.TuneClimate.sio_TEndFactor,
                   0,
                   creg_y,
                   &creg_z);
    int creg_x = (int)(_GD.TuneClimate.sio_TStartFactor-creg_z);

    creg_y = zone->hot->AllTask.DoRHAir-zone->hot->IndoorSensors[cSmRHSens].Value;
    CorrectionRule(_GD.TuneClimate.sio_RHStart,
                   _GD.TuneClimate.sio_RHEnd,
                   _GD.TuneClimate.sio_RHStartFactor-_GD.TuneClimate.sio_RHEndFactor,
                   0,
                   creg_y,
                   &creg_z);
    creg_z=_GD.TuneClimate.sio_RHStartFactor - creg_z;

    if ((zone->hot->IndoorSensors[cSmRHSens].Value)&&(zone->hot->AllTask.DoRHAir))
    {
        if (creg_x > creg_z)
            creg_x = creg_z;
    }

    ctx.fnSIOpause[zone->idx] = zone->tcontrol_tepl->PauseSIO;     // out

    if (zone->tcontrol_tepl->PauseSIO < creg_x*60)
        return;        // проверка паузы между вкл

    zone->tcontrol_tepl->FazaSiod=cSIOFazaPump;
    zone->tcontrol_tepl->TimeSIO+=zone->hot->AllTask.SIO;
    zone->tcontrol_tepl->PauseSIO=0;
    zone->tcontrol_tepl->CurVal=0;
}

void DoSiod(const zone_t *zone)
{
    char NSIO;

    //if (!(YesBit((*(pGD_Hot_Hand+cHSmSIOPump)).RCS,cbManMech))) (*(pGD_Hot_Hand+cHSmSIOPump)).Position=0;
    //else return;

    if (! YesBit(zone->hand[cHSmSIOVals].RCS,cbManMech))
        zone->hand[cHSmSIOVals].Position=0;
    else
        return;

    NSIO = zone->gh_ctrl->sio_SVal;
    if (NSIO > cNumValSiodMax)
        NSIO = cNumValSiodMax;


    ctx.fnSIOfaza[zone->idx] = zone->tcontrol_tepl->FazaSiod;  // out

    switch (zone->tcontrol_tepl->FazaSiod)
    {
    case cSIOFazaVal:
        {
            int creg_x = 1 << (zone->tcontrol_tepl->CurVal%4);
            //fnSIOvelvOut[fnTepl] = pGD_TControl_Tepl->CurVal;			// out
            SetBit(zone->hand[cHSmSIOVals].Position, creg_x);
        }
        if (!zone->tcontrol_tepl->TPauseSIO)
            zone->tcontrol_tepl->TPauseSIO = zone->hot->AllTask.SIO;
        #warning "falling thru. is it ok ?"
    case cSIOFazaPause:
        if (!zone->tcontrol_tepl->TPauseSIO)
        {
            zone->tcontrol_tepl->TPauseSIO=sio_ValPause;
            zone->tcontrol_tepl->CurVal++;

            ctx.fnSIOvelvOut[zone->idx] = zone->tcontrol_tepl->CurVal;           // out
        }
    case cSIOFazaPump:
        zone->hand[cHSmSIOPump].Position = 1;
        if (!zone->tcontrol_tepl->TPauseSIO)
            zone->tcontrol_tepl->TPauseSIO=sio_ValPause;

        ctx.fnSIOpumpOut[zone->idx] = zone->tcontrol_tepl->TPauseSIO;            // out
        break;

    case cSIOFazaEnd:
        zone->tcontrol_tepl->FazaSiod=0;
        zone->hand[cHSmSIOPump].Position = 0;       // new
        return;

    default:
        return;
    }
    if (--zone->tcontrol_tepl->TPauseSIO)
        return;

    if ((zone->tcontrol_tepl->FazaSiod == cSIOFazaPause)&&(zone->tcontrol_tepl->CurVal<NSIO))
        zone->tcontrol_tepl->FazaSiod=cSIOFazaVal;
    else
        zone->tcontrol_tepl->FazaSiod++;

}
