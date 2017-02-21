#ifndef _405_CONFIG_EN_RUS_SR_H_
#define _405_CONFIG_EN_RUS_SR_H_


//=======================================================
//---- Проект 405---------
//=======================================================
//

// XXX: isolation
#include "lcdconsts.h"
#include "65_const.h"

#define GetSensConfig(nTepl,nSens)	GD.MechConfig[nTepl].RNum[nSens+SUM_NAME_INSENS]
#define GetInputConfig(nTepl,nSens)	GD.MechConfig[nTepl].RNum[nSens+SUM_NAME_INPUTS]
#define GetMetSensConfig(nSens)	GD.MechConfig[0].RNum[nSens+SUM_NAME_OUTSENS]

#define tpRELAY		1
#define tpLEVEL		2
#define tpSUM		3


/*Смещения датчиков в общем массиве*/
#define cSmOutTSens		0
#define cSmFARSens		1
#define cSmVWindSens	2
#define cSmDWindSens	3
#define cSmRainSens		4
#define cSmMainTSens	6
#define cSmPresureSens	8

//#define cSmTSens		0
// NEW
#define cSmTSens1		0	// 4 датчика температуры
#define cSmTSens2		1
#define cSmTSens3		2
#define cSmTSens4		3

#define cSmRHSens		4
#define cSmInLightSens	6
#define cSmCOSens		7
#define cSmRoofSens		12
#define cSmGlassSens	14
// XXX: warning was too messy. disabled for now
// #warning CHECK THIS   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define cSmTCSens		1
#define cSmWinNSens		17
#define cSmWinSSens		18
#define cSmScreenSens	19
#define cSmWaterSens	20

// number of greenhouses
#define cSTepl			8

#define cConfSSystem		10

#define cConfSOutput		46

#define cConfSInputs		7

#define cConfSSens			26

#define cConfSMetSens		11

#define cSHandCtrl			cConfSOutput

#define cSRegCtrl			24

#define cSDiskrCtrl			22


#define DEF_PORTNUM         2012


#define SUM_NAME_INPUTS			(cConfSOutput)

#define SUM_NAME_INSENS			(SUM_NAME_INPUTS+cConfSInputs)

#define SUM_NAME_OUTSENS		(SUM_NAME_INSENS+cConfSSens)

#define SUM_NAME_CONF			(SUM_NAME_OUTSENS+cConfSMetSens)


#define MAX_SUM_RELAY	80



#define cSKontur		11
#define cSWaterKontur	6


/*Константы смещения контуров*/
#define cSmKontur1		0
#define cSmKontur2		1
#define cSmKontur3		2
#define cSmKontur4		3
#define cSmKontur5		4
#define cSmKonturAHU	5
#define cSmWindowUnW	6
#define cSmWindowOnW	7
#define cSmUCValve		8
#define cSmAHUSpeed		9
#define cSmScreen		10

/*----------------------------------*/

/*Константы смещения ручного управления и биты его*/
#define cHSmMixVal		0
#define cHSmWinN		6
#define cHSmWinS		7
#define cHSmUCValve		8
#define cHSmAHUSpeed1	9
#define cHSmScrTH		10
#define cHSmScrSH		11
#define cHSmScrV_S1		12
#define cHSmScrV_S2		13
#define cHSmScrV_S3		14
#define cHSmScrV_S4		15
#define cHSmCO2			16
#define cHSmPressReg	17
#define cHSmLight		18
#define cHSmAHUSpeed2	19
#define cHSmRez1		20
#define cHSmRez2		21
#define cHSmRez3		22
#define cHSmRez4		23

#define cHSmPump		24
#define cHSmVent		30
#define cHSmHeat		31
#define cHSmCool		32
#define cHSmSIOPump		33
#define cHSmSIOVals		34
#define cHSmAHUVals		35
#define cHSmAlarm		36
#define cHSmRegs		37



typedef struct
{
    const char Name[30];
    char Tip;
    char Min;
    char Max;
} eNameConfig;


#define cSmDHeat		0
#define cSmDVent		1
#define cSmDLight50		2
#define cSmDLight100	3
#define cSmRainDiskr	4
#define cSmDCO2			5
#define cSmDLight		6


#define SUM_NAME_PARS 24
typedef struct
{
    const char Name[30];
    char Ed;
} eNameParUpr;

#define SUM_NAME_TIMER 24
typedef struct
{
    const char Name[30];
    char Frm;
    char Index;
} eNameTimer;


//----------------------------------------


extern const eNameASens NameSensConfig[cConfSSens+cConfSMetSens];
extern const eNameTimer NameTimer[SUM_NAME_TIMER];
extern const eNameConfig NameSystemConfig[cConfSSystem];
extern const eNameParUpr NameParUpr[SUM_NAME_PARS];
extern const eNameConfig NameInputConfig[cConfSInputs];
extern const eNameConfig NameOutputConfig[cConfSOutput];

#endif
