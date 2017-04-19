
// a lot of unsorted crap
#include "syntax.h"

#include "defs.h"

#include "keyboard.h"
#include "control_gd.h"
#include "405_memory.h"
#include "measure.h"

#include "wtf.h"
#include "unsorted.h"

#include "defs.h"

#include "debug.h"

extern uchar nReset;

// so we're persisting this data:
// 0) Tepl of GD.Control
// 1) GD.Timers
// 2) GD.TuneClimate
// 3) GD.Strategy
// 4) GD.MechConfig
// 5) caldata.Cal.InTeplSens
// 6) caldata.Cal.MeteoSens
// 7) GD.ConstMechanic

void InitBlockEEP(void)
{
    #warning "WTF: +15 ?"
    BlockEEP[0].AdrCopyRAM=&gd()->Control.Zones;
    BlockEEP[0].Size = sizeof(gd()->Control.Zones)+15;

    BlockEEP[1].AdrCopyRAM=&gd_rw()->Timers;
    BlockEEP[1].Size = sizeof(gd()->Timers);

    BlockEEP[2].AdrCopyRAM=&gd_rw()->TuneClimate;
    BlockEEP[2].Size = sizeof(gd()->TuneClimate);

    BlockEEP[3].AdrCopyRAM=&gd_rw()->Strategy;
    BlockEEP[3].Size = sizeof(gd()->Strategy);

    BlockEEP[4].AdrCopyRAM=&gd_rw()->MechConfig;
    BlockEEP[4].Size = sizeof(gd()->MechConfig);

    BlockEEP[5].AdrCopyRAM=&caldata.IndoorSensors;
    BlockEEP[5].Size = sizeof(caldata.IndoorSensors);

    BlockEEP[6].AdrCopyRAM=&caldata.MeteoSensors;
    BlockEEP[6].Size = sizeof(caldata.MeteoSensors);

    BlockEEP[7].AdrCopyRAM=&gd_rw()->ConstMechanic;
    BlockEEP[7].Size = sizeof(gd()->ConstMechanic);
}

// so far so good.
// remote is expecting blob of:
// 0) GD.HOT (state to be monitored)
// 1) GD.Control (parameters of control)
// 2) GD.Timers (control tasks)
// 3) GD.TuneClimate (fine-tune of the control)
// 4) GD.Strategy (strategy of the control)
// 5) GD.MechConfig (fieldbus nodes settings and something else)
// 6) caldata.Cal (full ! settings of the sensors)
// 7) caldata.Cal.MeteoSens (settings of the meteo sensors. part of caldata.Cal). BUT ! sized for a full caldata :-)
// 8) GD.ConstMechanic (config of actuators)
// 9) GD.Level (? some mismatch here)
// 10) GD.HOT again
// 11) GD.TControl

void setup_scatter(void)
{
    // scatter list of some kind
    wtf0.AdrGD[0].Adr=&gd_rw()->Hot;
    wtf0.AdrGD[0].MaxSize=sizeof(gd()->Hot);

    wtf0.AdrGD[1].Adr=&gd_rw()->Control;
    wtf0.AdrGD[1].MaxSize=sizeof(gd_rw()->Control);

    wtf0.AdrGD[2].Adr=&gd_rw()->Timers;
    wtf0.AdrGD[2].MaxSize=sizeof(gd()->Timers);

    wtf0.AdrGD[3].Adr=&gd_rw()->TuneClimate;
    wtf0.AdrGD[3].MaxSize=sizeof(gd()->TuneClimate);

    wtf0.AdrGD[4].Adr=&gd_rw()->Strategy;
    wtf0.AdrGD[4].MaxSize=sizeof(gd()->Strategy);

    wtf0.AdrGD[5].Adr=&gd_rw()->MechConfig;
    wtf0.AdrGD[5].MaxSize=sizeof(gd()->MechConfig);

    wtf0.AdrGD[6].Adr=&caldata;
    wtf0.AdrGD[6].MaxSize=sizeof(caldata);

    #warning "WTF: size is fullcal again ?"
    wtf0.AdrGD[7].Adr=&caldata.MeteoSensors;
    wtf0.AdrGD[7].MaxSize=sizeof(calibration_t);

    wtf0.AdrGD[8].Adr=&gd_rw()->ConstMechanic;
    wtf0.AdrGD[8].MaxSize=sizeof(gd()->ConstMechanic);

    wtf0.AdrGD[9].Adr=&gd_rw()->Level;
    wtf0.AdrGD[9].MaxSize=sizeof(gd()->Level);

    #warning "WTF: Hot again ?"
//  wtf0.AdrGD[10].Adr=&gd_rw()->Hot;
//  wtf0.AdrGD[10].MaxSize=sizeof(gd()->Hot);
//
//  wtf0.AdrGD[11].Adr=&gd_rw()->TControl;
//  wtf0.AdrGD[11].MaxSize=sizeof(gd()->TControl);


    LOG("scatter addresses:");
    uint total = 0;
    for (uint i = 0; i < countof(wtf0.AdrGD); i++)
    {
        if (! wtf0.AdrGD[i].Adr)
            break;

        LOG("address: 0x%08x, size %d", (uint)wtf0.AdrGD[i].Adr, wtf0.AdrGD[i].MaxSize);

        total += wtf0.AdrGD[i].MaxSize;
    }

    LOG("scatter total size %d", total);

}

void ButtonReset(void)
{
    setup_scatter();

    gd_rw()->Control.rModification=cModification;
    gd_rw()->Control.rSInTeplSens=cConfSSens;
    gd_rw()->Control.rSMechanic=cSRegCtrl;
    gd_rw()->Control.Language=1;

    gd_rw()->Control.rSTepl=gd_rw()->Control.ConfSTepl;//cNowSTepl;
    gd_rw()->Control.rVersion=cVersion;
}

void SetRelay(uint16_t nRelay)
{
    WARN("set relay not implemented");

//  SetOutIPCDigit(nRelay / 100, nRelay % 100 - 1, 1);
}
//----------------------------------------
void ClrRelay(uint16_t nRelay)
{
    WARN("clr relay not implemented");

//  SetOutIPCDigit(nRelay / 100, nRelay % 100 - 1, 0);
}

char TestRelay(uint16_t nRelay)
{
    WARN("test relay not implemented");

//  return GetOutIPCDigit(nRelay / 100, nRelay % 100);
}

void InitAllThisThings(char fTipReset)
{
    if (fTipReset>2)
        memclr(&gd_rw()->Hot, sizeof(gd()->Hot));

    InitControl();
    reset_calibration();

    wtf0.SostRS=OUT_UNIT;

    gd_rw()->Control.NFCtr=NumCtr;
    gd_rw()->Control.IPAddr[0]=192;
    gd_rw()->Control.IPAddr[1]=168;
    gd_rw()->Control.IPAddr[2]=1;
    gd_rw()->Control.IPAddr[3]=100+NumCtr;

    gd_rw()->Control.Read1W=9;
    gd_rw()->Control.Write1W=4;
    //gd_rw()->Control.ConfSTepl=cConfSTepl;
    gd_rw()->Control.ConfSTepl=2;
    gd_rw()->Control.Language=cDefLanguage;
    gd_rw()->Control.Cod=111;
    gd_rw()->Control.Screener=40;

    gd_rw()->Hot.News |= bReset;

    reset_menu();
    keyboardSetSIM(100);
    nReset=25;
}
