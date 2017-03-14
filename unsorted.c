
// a lot of unsorted crap
#include "syntax.h"

#include "defs.h"

#include "keyboard.h"
#include "control_gd.h"
#include "405_memory.h"

#include "wtf.h"
#include "unsorted.h"

#include "stm32f10x_Define.h"

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

    BlockEEP[5].AdrCopyRAM=&caldata.Cal.InTeplSens;
    BlockEEP[5].Size = sizeof(caldata.Cal.InTeplSens);

    BlockEEP[6].AdrCopyRAM=&caldata.Cal.MeteoSens;
    BlockEEP[6].Size = sizeof(caldata.Cal.MeteoSens);

    BlockEEP[7].AdrCopyRAM=&gd_rw()->ConstMechanic;
    BlockEEP[7].Size = sizeof(gd()->ConstMechanic);
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

    wtf0.AdrGD[6].Adr=&caldata.Cal;
    wtf0.AdrGD[6].MaxSize=sizeof(caldata.Cal);

    #warning "WTF: size is fullcal again ?"
    wtf0.AdrGD[7].Adr=&caldata.Cal.MeteoSens;
    wtf0.AdrGD[7].MaxSize=sizeof(eFullCal);

    wtf0.AdrGD[8].Adr=&gd_rw()->ConstMechanic;
    wtf0.AdrGD[8].MaxSize=sizeof(gd()->ConstMechanic);

    wtf0.AdrGD[9].Adr=&gd_rw()->Level;
    wtf0.AdrGD[9].MaxSize=sizeof(gd()->Level);

    #warning "WTF: Hot again ?"
    wtf0.AdrGD[10].Adr=&gd_rw()->Hot;
    wtf0.AdrGD[10].MaxSize=sizeof(gd()->Hot);

    wtf0.AdrGD[11].Adr=&gd_rw()->TControl;
    wtf0.AdrGD[11].MaxSize=sizeof(gd()->TControl);
}

void ButtonReset(void)
{
    setup_scatter();

    gd_rw()->Control.rModification=cModification;
    gd_rw()->Control.rSInTeplSens=cConfSSens;
    gd_rw()->Control.rSMechanic=cSRegCtrl;
    gd_rw()->Control.Language=0;

    gd_rw()->Control.rSTepl=gd_rw()->Control.ConfSTepl;//cNowSTepl;
    gd_rw()->Control.rVersion=cVersion;
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
        memclr(&gd_rw()->Hot, sizeof(gd()->Hot));

    InitGD();
    wtf0.SostRS=OUT_UNIT;

    gd_rw()->Control.NFCtr=NumCtr;
    gd_rw()->Control.IPAddr[0]=192;
    gd_rw()->Control.IPAddr[1]=168;
    gd_rw()->Control.IPAddr[2]=1;
    gd_rw()->Control.IPAddr[3]=100+NumCtr;

    gd_rw()->Control.Read1W=9;
    gd_rw()->Control.Write1W=4;
    gd_rw()->Control.ConfSTepl=cConfSTepl;
    gd_rw()->Control.Language=cDefLanguage;
    gd_rw()->Control.Cod=111;
    gd_rw()->Control.Screener=40;

    gd_rw()->Hot.News |= bReset;

    Y_menu=0;
    x_menu=0;
    keyboardSetSIM(100);
    nReset=25;
}
