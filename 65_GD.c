
#include "405_memory.h"
#include "65_gd.h"
#include "defs.h"
#include "climdefstuff.h"

/* ======== Глобальный блок данных=============*/
eGData GD;

eTimer *pGD_Timer;
eKontur *pGD_Hot_Tepl_Kontur;
eTControlKontur *pGD_TControl_Tepl_Kontur;
eMechanic *pGD_Hot_Hand_Kontur;
eTeplControl*pGD_Control_Tepl;
eTepl *pGD_Hot_Tepl;
eTControlTepl *pGD_TControl_Tepl;
eConstMech *pGD_ConstMechanic;
eMechanic *pGD_Hot_Hand;
eStrategy *pGD_Strategy_Tepl;
eStrategy *pGD_Strategy_Kontur;
eMechConfig *pGD_MechConfig;
uint16_t *pGD_MechConfig_Kontur;
eConstMixVal *pGD_ConstMechanic_Mech;
eSensLevel *pGD_Level_Tepl;


void InitBlockEEP(void)
{
/*---!!!!ВНИМАНИЕ!!!! По глупости
номер в массиве BlockEEP должен соответствовать
на единицу меньше номеру в массиве AdrGD
т.е порядки структур должны быть строго одинаковы
и все несохраняеьые в EEP должны быть в конце AdrGD*/
    BlockEEP[0].AdrCopyRAM=&GD.Control.Tepl[0];
    BlockEEP[0].Size=sizeof(eTeplControl)*cSTepl+15;

    BlockEEP[1].AdrCopyRAM=&GD.Timer[0];
    BlockEEP[1].Size=(sizeof(eTimer)*cSTimer);

    BlockEEP[2].AdrCopyRAM=&GD.TuneClimate;
    BlockEEP[2].Size=(sizeof(eTuneClimate));

    BlockEEP[3].AdrCopyRAM=&GD.Strategy[0][0];
    BlockEEP[3].Size=(sizeof(eStrategy)*cSStrategy*cSTepl);

    BlockEEP[4].AdrCopyRAM=&GD.MechConfig[0];
    BlockEEP[4].Size=(sizeof(eMechConfig)*cSTepl);

    BlockEEP[5].AdrCopyRAM=&GD.Cal;
    BlockEEP[5].Size=sizeof(eCalSensor)*cSTepl*cConfSSens;

    BlockEEP[6].AdrCopyRAM=&GD.Cal.MeteoSens;
    BlockEEP[6].Size=sizeof(eCalSensor)*cConfSMetSens;

    BlockEEP[7].AdrCopyRAM=&GD.ConstMechanic[0];
    BlockEEP[7].Size=(sizeof(eConstMech)*cSTepl);
}


void ButtonReset(void)
{
/* адреса передачи данных */
    WTF0.AdrGD[0/*cblHot*/].Adr=&GD.Hot;
    WTF0.AdrGD[0].MaxSize=sizeof(eHot);
    WTF0.AdrGD[1/*cblControl*/].Adr=&GD.Control;
    WTF0.AdrGD[1].MaxSize=sizeof(eControl);
    WTF0.AdrGD[2/*cblTimer*/].Adr=&GD.Timer[0];
    WTF0.AdrGD[2].MaxSize=sizeof(eTimer)*cSTimer;
    WTF0.AdrGD[3/*cblTuneClimate*/].Adr=&GD.TuneClimate;
    WTF0.AdrGD[3].MaxSize=sizeof(eTuneClimate);
    WTF0.AdrGD[4/*cblStrategy*/].Adr=&GD.Strategy[0];
    WTF0.AdrGD[4].MaxSize=sizeof(eStrategy)*cSTepl*cSStrategy;
    WTF0.AdrGD[5/*cblMechConfig*/].Adr=&GD.MechConfig[0];
    WTF0.AdrGD[5].MaxSize=sizeof(eMechConfig)*cSTepl;
    WTF0.AdrGD[6/*cblCal*/].Adr=&GD.Cal;
    WTF0.AdrGD[6/*cblCal*/].MaxSize=sizeof(eFullCal);
    WTF0.AdrGD[7/*cblCal*/].Adr=&GD.Cal.MeteoSens;
    WTF0.AdrGD[7/*cblCal*/].MaxSize=sizeof(eFullCal);
    WTF0.AdrGD[8/*cblMechanic*/].Adr=&GD.ConstMechanic[0];
    WTF0.AdrGD[8/*cblCal*/].MaxSize=sizeof(eConstMech)*cSTepl;
    WTF0.AdrGD[9/*cblLevel*/].Adr=&GD.Level;
    WTF0.AdrGD[9/*cblCal*/].MaxSize=sizeof(eLevel);
    WTF0.AdrGD[10/*cblHot*/].Adr=&GD.Hot;
    WTF0.AdrGD[10/*cblCal*/].MaxSize=sizeof(eHot);
    WTF0.AdrGD[11/*cblHot*/].Adr=&GD.TControl;
    WTF0.AdrGD[11/*cblCal*/].MaxSize=sizeof(eTControl);
/* параметры контроллера */

    GD.Control.rModification=cModification;
    GD.Control.rSInTeplSens=cConfSSens;
    GD.Control.rSMechanic=cSRegCtrl;

    GD.Control.rSTepl=GD.Control.ConfSTepl;//cNowSTepl;
    GD.Control.rVersion=cVersion;
}
