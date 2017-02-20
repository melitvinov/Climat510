
#include "405_memory.h"
#include "65_gd.h"
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
    ClimDefStuff.AdrGD[0/*cblHot*/].Adr=&GD.Hot;
    ClimDefStuff.AdrGD[0].MaxSize=sizeof(eHot);
    ClimDefStuff.AdrGD[1/*cblControl*/].Adr=&GD.Control;
    ClimDefStuff.AdrGD[1].MaxSize=sizeof(eControl);
    ClimDefStuff.AdrGD[2/*cblTimer*/].Adr=&GD.Timer[0];
    ClimDefStuff.AdrGD[2].MaxSize=sizeof(eTimer)*cSTimer;
    ClimDefStuff.AdrGD[3/*cblTuneClimate*/].Adr=&GD.TuneClimate;
    ClimDefStuff.AdrGD[3].MaxSize=sizeof(eTuneClimate);
    ClimDefStuff.AdrGD[4/*cblStrategy*/].Adr=&GD.Strategy[0];
    ClimDefStuff.AdrGD[4].MaxSize=sizeof(eStrategy)*cSTepl*cSStrategy;
    ClimDefStuff.AdrGD[5/*cblMechConfig*/].Adr=&GD.MechConfig[0];
    ClimDefStuff.AdrGD[5].MaxSize=sizeof(eMechConfig)*cSTepl;
    ClimDefStuff.AdrGD[6/*cblCal*/].Adr=&GD.Cal;
    ClimDefStuff.AdrGD[6/*cblCal*/].MaxSize=sizeof(eFullCal);
    ClimDefStuff.AdrGD[7/*cblCal*/].Adr=&GD.Cal.MeteoSens;
    ClimDefStuff.AdrGD[7/*cblCal*/].MaxSize=sizeof(eFullCal);
    ClimDefStuff.AdrGD[8/*cblMechanic*/].Adr=&GD.ConstMechanic[0];
    ClimDefStuff.AdrGD[8/*cblCal*/].MaxSize=sizeof(eConstMech)*cSTepl;
    ClimDefStuff.AdrGD[9/*cblLevel*/].Adr=&GD.Level;
    ClimDefStuff.AdrGD[9/*cblCal*/].MaxSize=sizeof(eLevel);
    ClimDefStuff.AdrGD[10/*cblHot*/].Adr=&GD.Hot;
    ClimDefStuff.AdrGD[10/*cblCal*/].MaxSize=sizeof(eHot);
    ClimDefStuff.AdrGD[11/*cblHot*/].Adr=&GD.TControl;
    ClimDefStuff.AdrGD[11/*cblCal*/].MaxSize=sizeof(eTControl);
/* параметры контроллера */

    GD.Control.rModification=cModification;
    GD.Control.rSInTeplSens=cConfSSens;
    GD.Control.rSMechanic=cSRegCtrl;

    GD.Control.rSTepl=GD.Control.ConfSTepl;//cNowSTepl;
    GD.Control.rVersion=cVersion;
}
