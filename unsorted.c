
// a lot of unsorted crap
#include "syntax.h"

#include "defs.h"

#include "keyboard.h"
#include "control_gd.h"
#include "405_memory.h"

#include "wtf.h"
#include "unsorted.h"

#include "defs.h"

extern uchar nReset;

extern uint16_t Y_menu;
extern uint16_t x_menu;

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
/*---!!!!ВНИМАНИЕ!!!! По глупости
номер в массиве BlockEEP должен соответствовать
на единицу меньше номеру в массиве AdrGD
т.е порядки структур должны быть строго одинаковы
и все несохраняеьые в EEP должны быть в конце AdrGD*/


    #warning "WTF: +15 ?"
    BlockEEP[0].AdrCopyRAM=&GD.Control.Tepl;
    BlockEEP[0].Size = sizeof(GD.Control.Tepl)+15;

    BlockEEP[1].AdrCopyRAM=&GD.Timers;
    BlockEEP[1].Size = sizeof(&GD.Timers);

    BlockEEP[2].AdrCopyRAM=&GD.TuneClimate;
    BlockEEP[2].Size = sizeof(GD.TuneClimate);

    BlockEEP[3].AdrCopyRAM=&GD.Strategy;
    BlockEEP[3].Size = sizeof(GD.Strategy);

    BlockEEP[4].AdrCopyRAM=&GD.MechConfig;
    BlockEEP[4].Size = sizeof(GD.MechConfig);

    BlockEEP[5].AdrCopyRAM=&caldata.Cal.InTeplSens;
    BlockEEP[5].Size = sizeof(caldata.Cal.InTeplSens);

    BlockEEP[6].AdrCopyRAM=&caldata.Cal.MeteoSens;
    BlockEEP[6].Size = sizeof(caldata.Cal.MeteoSens);

    BlockEEP[7].AdrCopyRAM=&GD.ConstMechanic;
    BlockEEP[7].Size = sizeof(GD.ConstMechanic);
}

// so far so good.
// remote is expecting blob of:
// 0) GD.HOT
// 1) GD.Control
// 2) GD.Timers
// 3) GD.TuneClimate
// 4) GD.Strategy
// 5) GD.MechConfig
// 6) caldata.Cal (full !)
// 7) caldata.Cal.MeteoSens (part of caldata.Cal). BUT ! sized for a full caldata :-)
// 8) GD.ConstMechanic
// 9) GD.Level
// 10) GD.HOT again
// 11) GD.TControl

static void setup_scatter(void)
{
    // scatter list of some kind
    wtf0.AdrGD[0].Adr=&GD.Hot;
    wtf0.AdrGD[0].MaxSize=sizeof(GD.Hot);

    wtf0.AdrGD[1].Adr=&GD.Control;
    wtf0.AdrGD[1].MaxSize=sizeof(GD.Control);

    wtf0.AdrGD[2].Adr=&GD.Timers;
    wtf0.AdrGD[2].MaxSize=sizeof(GD.Timers);

    wtf0.AdrGD[3].Adr=&GD.TuneClimate;
    wtf0.AdrGD[3].MaxSize=sizeof(GD.TuneClimate);

    wtf0.AdrGD[4].Adr=&GD.Strategy;
    wtf0.AdrGD[4].MaxSize=sizeof(GD.Strategy);

    wtf0.AdrGD[5].Adr=&GD.MechConfig;
    wtf0.AdrGD[5].MaxSize=sizeof(GD.MechConfig);

    wtf0.AdrGD[6].Adr=&caldata.Cal;
    wtf0.AdrGD[6].MaxSize=sizeof(caldata.Cal);

    #warning "WTF: size is fullcal again ?"
    wtf0.AdrGD[7].Adr=&caldata.Cal.MeteoSens;
    wtf0.AdrGD[7].MaxSize=sizeof(eFullCal);

    wtf0.AdrGD[8].Adr=&GD.ConstMechanic;
    wtf0.AdrGD[8].MaxSize=sizeof(GD.ConstMechanic);

    wtf0.AdrGD[9].Adr=&GD.Level;
    wtf0.AdrGD[9].MaxSize=sizeof(GD.Level);

    #warning "WTF: Hot again ?"
    wtf0.AdrGD[10].Adr=&GD.Hot;
    wtf0.AdrGD[10].MaxSize=sizeof(GD.Hot);

    wtf0.AdrGD[11].Adr=&GD.TControl;
    wtf0.AdrGD[11].MaxSize=sizeof(GD.TControl);
}

void ButtonReset(void)
{
    setup_scatter();

    GD.Control.rModification=cModification;
    GD.Control.rSInTeplSens=cConfSSens;
    GD.Control.rSMechanic=cSRegCtrl;

    GD.Control.rSTepl=GD.Control.ConfSTepl;//cNowSTepl;
    GD.Control.rVersion=cVersion;
}

void SetRelay(uint16_t nRelay)
{
    char bRelay;
    if (GetIPCComMod(nRelay))
    {
        SetOutIPCDigit(1,nRelay,&bRelay);
    }
}
//----------------------------------------
void ClrRelay(uint16_t nRelay)
{
    char bRelay;
    if (GetIPCComMod(nRelay))
    {
        SetOutIPCDigit(0,nRelay,&bRelay);
    }
}

char TestRelay(uint16_t nRelay)
{
    char bRelay;
    if (GetIPCComMod(nRelay))
        return GetOutIPCDigit(nRelay,&bRelay);
    // XXX: is it right to report 0 ?
    return 0;
}

void InitAllThisThings(char fTipReset)
{
    if (fTipReset>2)
        memclr(&GD.Hot, sizeof(GD.Hot));

    InitGD();
    wtf0.SostRS=OUT_UNIT;

    GD.Control.NFCtr=NumCtr;
    GD.Control.IPAddr[0]=192;
    GD.Control.IPAddr[1]=168;
    GD.Control.IPAddr[2]=1;
    GD.Control.IPAddr[3]=100+NumCtr;

    GD.Control.Read1W=9;
    GD.Control.Write1W=4;
    GD.Control.ConfSTepl=cConfSTepl;
    GD.Control.Language=cDefLanguage;
    GD.Control.Cod=111;
    GD.Control.Screener=40;

    GD.Hot.News |= bReset;

    Y_menu=0;
    x_menu=0;
    keyboardSetSIM(100);
    nReset=25;
}
