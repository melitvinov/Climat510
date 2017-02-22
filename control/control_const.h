#ifndef _65_CONST_H_
#define _65_CONST_H_

#include "lcdconsts.h"

/*-----------------------------
*******************************
    ��������� ������� C324
********************************
------------------------------*/
#define FWVersion		"ver.09/15.1.00.21_2"	// 21_2  CRC ������ ethrnet
// 21_1  ������ ��� ������ checkConfig
// 21 - ��� ������ ��2 = 3 ����� ���������� ��2 ������ ������� �� 0
// 20 - �� ������� ��� ���
// 19 - �� ������� �������� �� ��� �� 50 � 100 %
// 18 - ����� ������ � TestFRAM, ������ ������� �� ������������! ��������
// 17 - ��������� � ���� �� 0 � �������� ���� � ���� �� ��������� ������
// 16 - ������� �� ��2 ����� ������� � ����������� �� ������
// 15 - ��2 ����� 3 ���������� �������� ��2 �� �������� � � ����� ������� �� ��2 ����� ������� � ����������� �� ������
// 14 - ������� ���� ������ ����� ������ ��������� ������ � ���������� �������
// 13 - ������� ���� ������ �������� ������ � ���������� ����
// 12 - ������������ ������������� ����������
// 11 - ��������� ����, ������ �������� ������ �� ���������� ���� � �� ������, �� ��� ����� ��� ������ ��������������� ���� �� ������, ���� ���� ������ ����
// 10 - ��������� ����, �� �� ��������� ���� ������ ������� �� ����� ���� - �� ��������
// 09 - �������� ��������
// 08 - �������� ������� ���������� ���������� ������������
// 07 - ������ DoVentCalorifer ��� ����� �����������
// 04 - ��� ����������� ��������, // 05 - ����  t vent � t heat ��������� ���� CO2 � ���������� ����������
// 06 - ������������ ��������� ���������

#define NameProg        "\310\252\245TO-K\247\245MAT\311"
#define Proect          "FC325-K-II-"

#define	cMaxStopI		20

#define cResetCritery	150
#define cResetDifTDo	100


#define cMinRain		5
#define cMinRainTime	1
/*-----------����� ���������----------------*/

#define bRasxod         0x10

#define Rasxod          (RegRasxod & bRasxod)

#define cVersion		72 // 69
/*������ �� ��� ��*/

#define cSTimer			40
/*���������� �������� ��� �������*/

#define cSTimerSave		20
/*���������� �������� ��� �������*/

#define cSArx			3
/*���������� �������� ���� � �������*/


/*���������� �������� 1-5 - ����,6 - ������������ �������, 7 - ����������� �������*/
#define cSStrategy		8
/*���������� �������� 1-5 - �������,�����,���*/
//#define cSMechanic		10
/*1-5 - ������������ �������, 6 - ��� �������, 7 - ��� �������, 8 - ��2,
9 - �����, 10 - ����������� � ������*/
#define cv_ResetMidlWater	80


#define c_PAirToWater		35
#ifdef DEMO
#define c_UpPosOn			40
#define c_DownPosOn			40
#else
#define c_UpPosOn			5
#define c_DownPosOn			5
#endif

#define v_AlarmMidlWater	1000
#define v_ControlMidlWater	300
#define f_DeadWindDirect	5
#define f_AbsMinWind		50
#ifdef SIO_PAUSE
#define sio_ValPause		SIO_PAUSE
#else
#define sio_ValPause		6
#endif

#define o_MidlSRFactor		200
#define o_MidlWindFactor	200
#define o_DeltaTime			21
#define c_SnowIfOut			400
#define f_MaxTFreeze		500
#define f_StartWind			500
#define f_StormPause		60

#define c_MaxWaterOff		5000

/*��������� �������� ������*/
#define cSmZone1		0
#define cSmZone2		1
#define cSmZone3		2
#define cSmZone4		3

/*��������� ��� ������������� ����������� ����*/
#define cErrKontur			10	//1 ������
#define cMin5Kontur			250 //45 ��������
#define cMinPumpOff			350
#define cMinAllKontur		(gdp.Hot_Tepl->AllTask.DoTHeat/10+20)	//������� ���� �������� ���� ������� �� 5 ��������
#define cMinPowerKontur		10
#define cIFactorEnd			10
#define cPFactorEnd			300

//#define cMinFreeze			4

#define cMinPauseMixValve	5
#define cPausePump			20


/*��������� ���������� ��������*/
#define cSmLightDiskr		0x01
#define cSmPolivDiskr		0x02

#define cSmRain				0x01
#define cSmSIONo			0x02
/*------------------------------------*/
/*----------------------------------*/
/*��������� ��� RCS ���������*/
//#define cbMinMech			0x02
//#define cbMaxMech			0x03



//----------------------------------------------------------------------
//--------------------��������� ��������� ���������---------------------
//----------------------------------------------------------------------

#define	MAX_ALARMS			30

//GREEN LIGHT
#define cSOK				0 //��������
#define cSOn				1 //�������
#define cSBlPump			2 //����� �� ���������
//#define cSMinReach			3 //������������� �������
#define cSCloseCond			4 //������ �� ��������
#define cSCloseProg			5 //������ �� ���������
#define cSFollowProg		6 //����������� �� ���������
#define cSOnProg			7 //�������� �� ���������
#define cSOnCond			8 //�������� �� ��������
#define cSReachMax			9 //�������� �� ����������
#define cSReachMin			10 //�������� �� ����������



//YELLOW LIGHT
#define cSWNoHeat			20 //��� �����
#define cSWHand				21 //������ ����������
#define cSWFrost			22 //������ �� ������
#define cSWScreenFrost		23 //�������� ����������
#define cSWNoRange			24 //����� �������� ������
#define cSWNoMax			25 //�������� �� ����������
#define cSWRain				26 //�����



//RED LIGHT
#define cSAlrExternal		30 //������� ������
#define cSAlrNoCtrl			31 //��� ����������
#define cSAlrNoSens			32 //��� ���������
#define cSAlrStorm			33 //������ �� ������



#define aNoTaskA	0x00

#define aTempA	0x00
#define aRHA	0x00
#define aLeafA	0x00
#define aGrndA	0x00
#define aScreen	0x00
#define aGlassA	0x00
#define aCOA	0x00
#define aTempCA	0x00
#define aWinNA	0x00
#define aWinSA	0x00
#define aTT1A	0x00
#define aTT2A	0x00
#define aTT3A	0x00
#define aTT4A	0x00
#define aTT5A	0x00

#define aNoTaskB	0x00

#define aTempB	0x00
#define aRHB	0x00
#define aLeafB	0x00
#define aGrndB	0x00
#define aRezB	0x00
#define aGlassB	0x00
#define aCOB	0x00
#define aTempCB	0x00
#define aWinNB	0x00
#define aWinSB	0x00
#define aTT1B	0x00
#define aTT2B	0x00
#define aTT3B	0x00
#define aTT4B	0x00
#define aTT5B	0x00

#define aTempO	0x00
#define aSunO	0x00
#define aSpeedO	0x00
#define aDirO	0x00
#define aRainO	0x00
#define aRHO	0x00
#define aTTInO	0x00
#define aTTOutO	0x00
#define aPInO	0x00
#define aPOutO	0x00
#define aQO		0x00

/*-----------------------------*/
/*������� ���������� � �������*/
/*
#define cSmKontur1Mech			0
#define cSmKontur2Mech			1
#define cSmKontur3Mech			2
#define cSmKontur4Mech			3
#define cSmKontur5Mech			4
#define cSmNorthWinMech			5
#define cSmSouthWinMech			6
#define cSmScreenMech			7
#define cSmCOMech				8
#define cSmVentMech				9
#define cSmAirHeatMech			10
#define cSmSiodMech				11
#define cSmLightMech			12
*/
/*-----------------------------*/
/*----------------------------------------*/
/*�������� ����� �������� � �������� ����������*/
#define cSmPumpBit		0
#define cSmCloseBit		1
#define cSmOpenBit		2
/*---------------------------------------*/

#define mtRS485			1



#define cbManMech		0x01
#define cbResetMech		0x04

#define cMSAlarm		0x08
#define cMSBusyMech		0x04
#define cMSFreshSens	0x02
#define cMSBlockRegs	0x01

//#define cbNoMech		0x02
/*----------------------------------*/

#define cTermHorzScr	0
#define cSunHorzScr		1
#define cTermVertScr1	2
#define cTermVertScr2	3
#define cTermVertScr3	4
#define cTermVertScr4	5

/*-------��������� ����--------------
#define cmTimeDate		0
#define cmTimer			1
#define cmArxiv			2
#define cmControl		3
#define cmHand			4
#define cmCalibr		5
#define cmLevel			6
#define cEndMenu		7*/
/*------------------------------------*/

/*��������� ���������� �������� */
#define cOutSensing		0
#define cWaterSensing	1
#define cInSensing		2
#define cMissSensing	3

#define cTypeNULL		0 //�� ���������� ������
#define cTypeSun		1 //������
#define cTypeMeteo		5 //�������� � ����������� �����
#define cTypeRain		2 //������

#define cTypeAnal		10
#define cTypeRH			11
#define cTypeFram		12
#define cTypeScreen		13
/*-------------------------------*/

/*��������� ��� ����������*/
#define cNoMidlSens		0
#define c2MidlSens		1
#define c3MidlSens		2
#define cExpMidlSens	3
/*����������� ����������������� ����������*/
#define cKExpMidl		50
/*------------------------------*/

/*������� ������� � �������*/
#define cSmDownAlarmLev	0
#define cSmDownCtrlLev	1
#define cSmUpCtrlLev	2
#define cSmUpAlarmLev	3
/*----------------------------*/


/*��������� RCS ��������*/
#define cbNoWorkSens	0x01  //���� 0 - ������ �� �����������
#define	cbNotGoodSens	0x02  //������ ��������� - ������ ��� ����������� �������������
#define cbUpAlarmSens	0x04  //��������� ������� ��������� ������� �� "��������"
#define	cbDownAlarmSens	0x08  //��������� ������ ��������� ������� �� "��������"
#define	cbUpCtrlSens	0x10  //��������� ������� ����������� ������� �� "��������"
#define cbMinMaxVSens	0x20  //��� ����������� ������� ���������
#define cbMinMaxUSens	0x40  //��� ����������� ������� ����������
#define	cbDownCtrlSens	0x80  //��������� ������ ����������� ������� �� "��������"
/*-----------------------------*/

/*��������� RCS �������*/
#define cbNoTaskForTepl		0x01
#define cbNoSensingTemp		0x02
#define cbCorrTOnSun		0x04
#define cbCorrRHOnSun		0x08
#define cbCorrCO2OnSun		0x10
#define cbCorrTAirUpOnRH	0x20
#define cbNoSensingOutT		0x40
#define cbCorrTAirDownOnRH	0x80

/*----------------------------*/

/*��������� ExtRCS �������*/
#define cbPausePumpTepl		0x01
#define cbMaxFreezeTepl		0x02
/*----------------------------*/



/*��������� ��� RCS �������*/
#define cbNoWorkKontur		0x01
#define cbNoSensKontur		0x02
//#define cbCorrMinTaskOnSun	0x04
//#define cbMinMaxAlarm		0x08
//#define cbScreenKontur		0x04
#define cbPumpChange		0x08
#define cbYesMinKontur		0x10
#define cbYesMaxKontur		0x20
#define cbGoMax				0x40
#define cbGoMaxOwn			0x80
//#define cbOnPumpKontur		0x80

/*��������� ��� RCS1 TControl*/
#define cbSCCorrection			0x01

/*��������� ��� ExtRCS �������*/
#define cbCtrlErrKontur			0x01
#define cbAlarmErrKontur		0x02
#define cbBlockPumpKontur		0x04
#define cbReadyPumpKontur		0x08
#define cbResetErrKontur		0x10
#define cbReadyRegUpKontur		0x20
#define cbReadyRegDownKontur	0x40
#define cbReadyRegsKontur		0x80
/*-----------------------------*/
/*��������� ��� ��������� ����������*/
/*������� � ���������� ��������*/
//#define cNKontur	0
//#define cAndKontur	1
//#define cSepKontur	2

/*���� ��������*/
#define	cfOnLight		1
#define	cfPauseLight	0

/*������ ��������*/
#define	cmOnLight		1
#define	cmOffLight		0

/*��������� ��������*/

#define	cHelpKontur5	5

/*-----------------------------*/

/*-------��������� ����--------------*/
#define cmTimeDate		0
#define cmTimer			1
//#define cmArxiv			cmTimer+cSTepl
#define cmControl		cmTimer+1//cmArxiv+1
#define cmConstMech		cmControl+1
#define cmStrategy		cmConstMech+1
#define cmMechConfig	cmStrategy+1
#define cmHand			cmMechConfig+1
#define cmCalibr		cmHand+1
//#define cmLevel			cmCalibr+1
#define cEndMenu		cmCalibr+1
/*-------------------------------*/
/*������� ���������*/
#define cu				0
#define cuB				1
#define cuT				2
#define cuPr			3
#define cuYesNo			4
#define cuOnOff			5
#define cuBt			6
#define cuDj			7
#define cuPa			8
#define cuMM			9
#define cuMSec			10
#define cuPPM			11
#define cuGr			12
#define cumV			13
#define cuPpm			14


/*-----------------------------------*/


u8 NowDayOfWeek;


/*-----------------------------*/
typedef struct
{
    char TempPower;
    char RHPower;
    char OptimalPower;
    char EcoPower;
    char Power;
    char Separate;
    char KonturHelp;
} eDefStrategy;


/*typedef struct eeNameLev {
        char Name[12];
        } eNameLev;
*/
typedef struct  eeNameSens
{
    char Name[30];
    char Frm;
    char Ed;
    char TypeSens;
    char TypeInBoard;
    char Output;
    int16_t  Min;
    int16_t  Max;
    int16_t  uCal[2];
    int16_t  vCal[2];
    int16_t  uMin;
    int16_t  uMax;
    char TypeMidl;
    char DigitMidl;
    char AlarmA;
    char AlarmB;
} eNameASens;

typedef struct  eeNameConst
{
    uint16_t  StartZn;
    char Frm;
} eNameConst;


#endif
