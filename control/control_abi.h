#ifndef _CONTROL_ABI_
#define _CONTROL_ABI_

// XXX: would it work in GCC ?
#pragma pack(1)

typedef struct  //20
{
    uint8_t   RCS;
    int8_t  Position;
} eMechanic;


typedef struct
{

    int16_t     Optimal;
    int16_t     MaxCalc;
    int16_t     MinTask;
    int16_t     MinCalc;

    int8_t      SError;
    int8_t      rez;
    int16_t     Priority;

//				int16_t		TempWeight;

    int16_t     SensValue;
    int16_t     Do;

    int8_t      RCS;
    int8_t      ExtRCS;

    int8_t      Status;
    int8_t      Rez1;
    int16_t     Rez[2];

//12*2
} eKontur;


typedef struct
{
    int16_t     DifTAirTDo;//������� ��������������� � ��������
    int16_t     UpSR;//��������� ����������� ������� �������	2
    int16_t     LowGlass;//�������� ������	4
    int16_t     LowOutWinWind;//�������� ����� � ������� �����������	6
    int16_t     UpLight;//�������� ��������� ��������	8
    int16_t     ICorrection;
    int16_t     dSumCalcF;//�������� ������	12

    int16_t     DiffCO2;//�������������� ����������� � ����������� �� ������������� ���������	14
    int16_t     TVentCritery;//�������� �����	16
    int16_t     PCorrection;//��������� ����������� ������� �� 1 ��������	18

    int16_t     Critery;//����-�������� ������������� ��	20
    int16_t     ICorrectionVent;//������� ����������� (��� �����������)	22

    int16_t     dSumCalc;//���������� ������� �������	24

    int16_t     PCorrectionVent;//������� �� ���������� ��������	26

    int16_t     CorrectionScreen; // ����� �������� ��

    int16_t     Rez[9];

} eNextTCalc;


typedef struct
{
    int16_t         CorrScreen;
    int16_t         TaskCritery;
    int16_t         CriteryNow;
    int16_t         TimeSiod;
    int16_t         MeasDifPress;
    int16_t         Rez[10];
} eOtherCalc;


typedef struct
{
    int16_t     TAir;
    int16_t     DoTHeat;
    int16_t     DoTVent;

    int16_t     NextTAir;
    int16_t     NextTVent;

    int16_t     RHAir;
    int16_t     DoRHAir;

    int16_t     CO2;
    int16_t     DoCO2;

    int8_t      Win;
    int16_t     SIO;

    int16_t     DoPressure;

    int8_t      Vent;

    int8_t      Screen[3];

    int8_t      Light;
    int8_t      ModeLight;

    int8_t      Status;

    int16_t     Rez[10];
} eClimTask;


typedef struct
{
    int16_t     Value;
    uint8_t   RCS;
/*
    0x01 - 1 - ��������
    0x02 - 1 - ����� �� ������� ����������� ���������
    0x04 - 1 - ����� �� ������� ��������� �������������� ����������
    0x08 - 1 - ��������� ������� ��������� ������� ���������
    0x10 - 1 - ��������� ������� ����������� ������� ���������
    0x20 - 1 - ��������� ������ ����������� ������� ���������
    0x40 - 1 - ��������� ������ ��������� ������� ���������
*/
} eSensing;

typedef struct
{

    uint8_t       Cfg[2];

    eSensing    InTeplSens[cConfSSens];
    int16_t     discrete_inputs[2];

    int8_t      ExtRCS;
    int8_t      RCS;
/*
    0�01 - 1 - ��� ������� �����������
    0�02 - 1 - ��� ��������� �����������
    0�04 - 1 - ����������� ��������������� �� ������
    0�08 - 1 - �����������
*/
    eClimTask   AllTask;
    eOtherCalc  OtherCalc;

    eNextTCalc  NextTCalc;
    eKontur     Kontur[cSWaterKontur+2];

    int8_t      SystemStatus;
    int8_t      ConnectionStatus;

    int8_t      SensingStatus;
    int8_t      RezStatus;

    int8_t      ScreenStatus;
    int8_t      SIOStatus;

    int8_t      LightStatus;
    int8_t      COStatus;

    uint8_t       tempParamHeat;  // new
    uint8_t       tempParamVent;  // new
    uint16_t    tempHeat;       // new
    uint16_t    tempVent;       // new

    uint16_t    airHeatOnOff;   // new
    uint16_t    airHeatTimeWork;// new
    uint16_t    CO2valveTask;   // new



    int16_t     Rez1[27];       // 27
    //int16_t		Rez1[32];

    int16_t     MaxReqWater;  // ���� �������������
    uint16_t    newsZone;       // new
    int16_t     Rez[8];    // 9
    eMechanic   HandCtrl[cSHandCtrl];
} eZone;


typedef struct
{
//0
    uint8_t       News;
    /*���� ����� �� ��*/
    eSensing    MeteoSensing[cConfSMetSens];
    int16_t     MaxReqWater;
    int16_t     HeatPower;
    int16_t     FullHeat;
//40
    int16_t     NextRHAirSens;
    int16_t     Vosx;
    int16_t     Zax;
/***************************************/
    int16_t     SumSun;
    uint8_t       Regs[5];
    uint8_t       Util;
//50
    uint8_t       isLight;
    uint8_t       blockCO2;

    uint8_t       Rez[4];
    int16_t     MidlSR;
    int16_t     MidlWind;
    int16_t     PozFluger;

    uint16_t    Time;
    uint16_t    Date;
    uint8_t       Year;
    uint8_t       Demo;
/***************************************/
    eZone       Zones[NZONES];

    int16_t     Rez2[10];
/***************************************/
} eHot;

//----

// blobs in remote reporting
typedef struct
{
    int16_t     c_MaxTPipe[cSWaterKontur];   /*����������� ���������� ����������� ������1-5*/
    int16_t     f_MaxOpenUn;   /*�������_����������� ���������� ��������*/
    int16_t     f_MaxOpenOn;   /*�������_�������� ��� ��������� ��2*/
    int16_t     c_MinTPipe[2];
    int16_t     c_DoPres;
    int16_t     c_OptimalTPipe[2];
    int16_t     f_IFactor;
    int16_t     c_PFactor;  /*������ 1 - ��������(����-����)������ ��*/
    int16_t     c_IFactor; /*������������ �������� ��������*/
    int16_t     f_PFactor; /*������ 1 - ��������(����-����)�������� ��*/
    int8_t      vs_DegSt; //������� ��� ���������� ������� ������
    int8_t      vs_DegEnd; //������� ��� ���������� ������� ������
    int16_t     sc_TMaxOpen; // ����� �����������
    int16_t     sc_ZMaxOpen; // ����� ����������
    uint16_t    co_model;/*��2 - �����������(0-���������,1-������)*/
    uint16_t    sio_SVal;
    uint16_t    sLight;
    int8_t      sensT_heat;
    int8_t      sensT_vent;

    int16_t     Rez[19];
//+42 �����
} eZoneControl;

typedef struct
{
    uint8_t           rModification;
    uint8_t           rSInTeplSens;
    uint8_t           rSMechanic;
    uint8_t           rSTepl;
    uint8_t           rVersion;
    eZoneControl    Zones[NZONES];
    uint8_t           NFCtr;
    uint8_t         IPAddr[4];
    uint8_t           ConfSTepl;
    int8_t          Language;
    int8_t          Read1W;
    int8_t          Write1W;
    uint8_t         Screener;
    uint8_t         Cod;
    //	uint8_t			Saverez;
    int16_t             Rez[5];

} eControl;

// ---

typedef struct  eeTimer
{
    uint8_t       Zone[1];
    uint16_t    TimeStart;
    uint8_t     TypeStart;
    uint8_t     Rez1;
    uint16_t    TAir;
    uint16_t    TVentAir;
    uint16_t    CO2;

    uint8_t       TOptimal1;
    uint8_t       TOptimal2;

    uint8_t       TPipe3;
    uint8_t       TPipe4;

    uint8_t       MinTPipe5;

    uint8_t       MinTPipe1;
    uint8_t       MinTPipe2;

    uint8_t       MinOpenWin;
    uint8_t       Win;

    uint16_t    SIO;

    uint8_t       Screen[3];
    uint8_t       Vent;

    uint8_t       Light;
    uint8_t       ModeLight;

    uint8_t       MinTPipe3;
    uint8_t       RHAir_c;

    uint16_t    Rez[10];
} eTimer;

// ---

typedef struct
{
    int16_t     v_TimeMixVal;
    int16_t     v_PFactor;  /*������������ ������ - �-���� ������� � ����*/
    int16_t     v_IFactor;  /*������������ ������ - �-���� ������� � ����*/
    int8_t      v_MinTim;
    int8_t      v_Type;
} eConstMixVal;

typedef struct
{
    eConstMixVal     ConstMixVal[cSRegCtrl];
} eConstMech;

// ---

#ifdef AHU1
typedef struct
{
    int8_t   TUpRHUp;
    int8_t   TUpRHDown;
    int8_t   TDownRHUp;
    int8_t   TDownRHDown;
    int8_t   Optimal;
    uint16_t Power1;
    int8_t   Point1;
    int16_t  Power2;
    int8_t   Point2;
} eStrategy;
#else

typedef struct
{
    int8_t TempPower;
    int8_t RHPower;
    int8_t OptimalPower;
    int8_t Economic;
    int8_t Powers;
    int8_t Separate;
    int8_t KonturHelp;
} eStrategy;
#endif

// ---

typedef struct
{
    uint16_t    Systems[cConfSSystem];
    uint16_t    RNum[SUM_NAME_CONF];
} eMechConfig;

// ---

typedef struct
{

/*-- ������ ����������� ������, ������ 130 ����*/
/*-----------------------------------
                ���������-������
------------------------------------*/
    int16_t     s_TStart[1];   /*����������� ������ - ������ �������� ������ ���*/
    int16_t     s_TEnd;   /*����������� ������ - ������ ������ ��*/
    int16_t     s_TConst;  /*����������� ������ - ������ ����������� ��*/
    int16_t     s_TVentConst;   /*��������������� - ������ �������� ������ ���*/

    int16_t     cool_PFactor;   /*��������������� - ������ ������ ��*/
    int16_t     s_RHConst;  /*��������������� - ������ ����������� ��*/
    int16_t     COPause;   /*������������ ��2 - ������ �������� ������ ���*/
    int16_t     f_SRFactor;   /*������������ ��2 - ������ ������ ��*/

    int16_t     s_CO2Const;  /*������������ ��2 - ������ ����������� ��*/
    int16_t     f_OutFactor;   /*������ 1_������� � �����- ������ �������� ������ ���*/
    int16_t     f_CorrTVentUp;   /*������ 1_������� � ��������� - ������ ������ ��*/
    int16_t     s_MinTPipeConst;/*������ 1_������� � ��������� - ������ ��������� ��*/

    int16_t     s_MinTPipe5;   /*�������_������� � �����- ������ �������� ������ ���*/
    int16_t     s_MinTPipe3;   /*�������_������� � ��������� - ������ ������ ��*/
    int16_t     s_MinOpenWinConst; /*�������_������� � ��������� - ������ ����������� ��*/
//30 ����
/*-----------------------------------
                ���������-��� ����������� ����������
------------------------------------*/
    int16_t     f_min_RHStart;
    int16_t     f_min_RHEnd;
    int16_t     f_max_RHStart;
    int16_t     f_max_RHEnd;
    int8_t      f_min_Cor;
    int8_t      f_max_Cor;
//42 �����
/*-----------------------------------
                ���������-�������
------------------------------------*/
//64 �����
    int16_t     c_DoMinIfGlass;  /*������ 5 - ��� ���� ������ � ����� ������*/
    int16_t     f_CorrTVent;  /*������ 5 - ��� ������� �������*/
    int16_t     c_MinIfSnow;  /*������ 5 - ������� ������� ��� ������� ����������� ������*/

    int16_t     o_DeltaLight;
    int8_t      f_changeWindows;
    int8_t      f_only;
//74 �����
/*-----------------------------------
                ���������-������� �� �����
------------------------------------*/

    int8_t      f_MaxAngle;  /*������ 1 - (����-����)�������� ������ ���*/
    int8_t      c_MullDown;  /*������ 1 - (����-����)�������� ������ ���*/
    int8_t      f_DefOnUn;  /*������ 1 - (����-����)������ ��*/

    int8_t      c_5ExtrHeat;  /*������ 1 - (����-����)������ ��*/
    int16_t     c_CriticalSnowOut;  /*������ 1 - (����-����)����������� ��*/
    int16_t     c_DoMaxIfGlass;  /*������ 1 - ��������(����-����)�������� ������ ���*/


    int16_t     c_GlassStart;  /*������ 1 - (����-�������)�������� ������ ���*/
    int16_t     c_GlassEnd;  /*������ 1 - (����-�������)������ ��*/
    int16_t     c_GlassFactor;  /*������ 1 - (����-�������)����������� ��*/
/*������������ ���������*/
    int16_t     c_MaxDifTDown;  /*������ 1 - (����-������)�������� ������ ���*/
    int16_t     f_MinSun;  /*������ 1 - (����-������)������ ��*/
    int16_t     f_DeltaOut;  /*������ 1 - (����-������)����������� ��*/
/*______________________*/
    int16_t     c_SRStart;  /*������ 1 - ������ �������� ������ ���*/
    int16_t     c_SREnd;  /*������ 1 - ������ ������ ��*/
    int16_t     c_SRFactor;  /*������ 1 - ������ ��������� ��*/

    int16_t     c_OutStart;  /*������ 1 - (����-�����)�������� ������ ���*/
    int16_t     c_OutEnd;  /*������ 1 - (����-�����)������ ��*/
    int16_t     c_OutFactor;  /*������ 1 - (����-�����)����������� ��*/

    int16_t     c_WindStart;  /*������ 1 - ����� �������� ������ ���*/
//114 �����
    int16_t     c_WindEnd;  /*������ 1 - ����� ������ ��*/
    int16_t     c_WindFactor;  /*������ 1 - ����� ����������� ��*/

    int16_t     sc_GlassStart;  /*������ 1 - ������� �������� ������ ���*/
    int16_t     sc_GlassEnd;  /*������ 1 - ������� ������ ��*/
    int16_t     sc_GlassMax;  /*������ 1 - ������� ����������� ��*/

    int16_t     c_LightFactor;  /*������ 1 - �������� ��� ��������� ��������*/
    int16_t     c_ScreenFactor;  /*������ 1 - �������� ��� �������� ������*/
    int16_t     c_CloudFactor;  /*������ 1 - �������� ��� �������*/
//  100�������


/*-----------------------------------
                ���������-����������
------------------------------------*/
    uint8_t       f_S1MinDelta; /*������� - ������ �� ������ ������*/
    uint8_t       f_S1Level;
    uint8_t       f_S2MinDelta;
    uint8_t       f_S2Level;
    uint8_t       f_S3MinDelta;
    uint8_t       f_S3Level;
//+6
    int16_t     f_MinDelta;  /*������� - ��������� ���� ��� ������*/
    int16_t     c_MaxDifTUp;  /*������� - ������������ ����� ����� �����������*/
    int16_t     f_MinTime;  /*������� - ����������� ����� ����� �����������*/
    int16_t     f_StormWindOn; /*������� - ����� ��������� ����������� ������� ���*/
    int16_t     f_StormWind; /*������� - ����� ��������� ������������ ������� ���*/
    int16_t     f_WindStart;  /*������� - ����� �������� ������ ���*/
//154 �����
//+18
    int16_t     f_WindHold;  /*������� - ����� ���� ��������� �������� �������� ���*/
    int16_t     f_MinTFreeze;   /*������� - ����� ���� ��������� ���*/
//+22
    int16_t     f_MaxOpenRain;  /*������� - ��� ����� ��������� ��������� ��*/
    int8_t      co_MaxTime; /*������� - ������ ������ ����������� ������� ���*/
    int8_t      co_MinTime;
/*-----------------------------------
                ���������-������� �� �������
------------------------------------*/

    int8_t      co_Impuls;       /*������� - (����-�����������)������� �� �������� ���*/
    int8_t      co_Dif;
    int16_t     f_StartCorrPow;  /*������� - ������ �������� ������ ���*/
    int16_t     f_EndCorrPow;  /*������� - ������ ������ ��*/
    int16_t     f_PowFactor;        /*������� - ������ ����������� ��*/
/*-----------------------------------
                ���������-CO2
-----------------------------------*/
    int16_t     reg_PFactor[2];/*��2 - ���� ����������������*/
/*-----------------------------------
                ���������-�����
------------------------------------*/
    int16_t     sc_ZSRClose; /*����� ���������� - ������ ��������� ���*/
    int16_t     sc_TSROpen;  /*����� ����������� - ������ ��������� ���*/
    int16_t     sc_TOutClose;/*����� ����������� - ����� ���� ��������� ���*/
    int16_t     sc_TVOutClose;/*����� ����������� - ����� ����� ��������� ���*/

    int16_t     sc_ZOutClose;/*����� ����������� - ����� �������� ������ ���*/
    int16_t     sc_TWindStart;/*����� ����������� - ����� ������ ��*/
    int16_t     sc_TVSRMaxOpen; /*����� ����������� - ����� ����������� � ���� ��*/
    int16_t     sc_TVSROpen;   /*����� ����������� - ������ ��������� ������������ ���� ���*/

    int16_t     f_WindFactor;/*����� - (����-����) ������ ��*/
    int16_t     sc_TFactor;/*����� - (����-����) ������������ ��*/
    int16_t     s_StartCorrPow;  /*������� - ������ �������� ������ ���*/
    int16_t     s_EndCorrPow;  /*������� - ������ ������ ��*/

    int16_t     s_PowFactor;        /*������� - ������ ����������� ��*/
    int16_t     sc_StartP2Zone;/*����� ��������� ������ ��*/
    int16_t     sc_StepS2Zone;/*����� ���������-���������� �����*/
    int16_t     sc_StepP2Zone;/*����� ���������-����� ����� �����������*/

    int16_t     sc_StartP1Zone;/*����� ��������� ������ ��*/
    int16_t     sc_StepS1Zone;/*����� ���������-���������� �����*/
    int16_t     sc_StepP1Zone;/*����� ���������-����� ����� �����������*/
    int16_t     sc_MinDelta;/*����� ��������� ���� ��� ������*/
//214
/*-----------------------------------
                ���������- ����
------------------------------------*/
    int16_t     sio_TStart;/*���� - ����������,���� ����������� ������ ������� ��*/
    int16_t     sio_TStartFactor;/*���� - ��� ����������� ���������� ����������� �����*/
    int16_t     sio_TEnd;/*���� - ����������� ������ ������� ������ ��*/
    int16_t     sio_TEndFactor;/*���� - ��� ������� ��������������� ����������� �����*/
    int16_t     sio_TStop;/*���� - ��������,���� ����������� ������ ������� ��*/
    int16_t     sio_RHStart;/*���� - ����������,���� ��������� ������ ������� ��*/
    int16_t     sio_RHStartFactor;/*���� - ��� ����������� ���������� ����������� �����*/
    int16_t     sio_RHEnd;/*���� - ��������� ������ ������� ������ ��*/
    int16_t     sio_RHEndFactor;/*���� - ��� ������� ��������������� ����������� �����*/
    int16_t     sio_RHStop;/*���� - ��������,���� ��������� ������ ������� ��*/

    int16_t     ScaleRasx;/*���� - ����� ����� ����������� ��������*/

/*-----------------------------------
                ���������-�����������
-----------------------------------*/

    int16_t     vt_PFactor;/*����������� - ��������, ���� ��������������� ������*/
    int16_t     vt_WorkTime;/*����������� - ����� ������*/
    int16_t     vt_StopTime;/*����������� - ����� �����*/

/*-----------------------------------
                ���������-��������� �������
------------------------------------*/
    int16_t     vt_StartCalorifer;/*��������� ������� - ��������,���� ����������� ������ ��*/
    int16_t     vt_EndCalorifer;/*��������� ������� - ���������,���� ����������� ������ ��*/
/*-----------------------------------------
                ��������� - �����
------------------------------------------*/
    int16_t     sc_dTStart;     /*����������� ���������� �������� ��������*/
    int16_t     sc_dTEnd;       /*����������� ���������� �����*/
    int16_t     sc_dTSunFactor; /*����������� ����������*/
/*-----------------------------------------
                ��������� - ��������� �� ��������� ���������� �������
------------------------------------------*/

    int16_t     co_IFactor;
    int16_t     MinRainTime;
    int16_t     f_BlockFan;
    int16_t     sc_RHStart;
    int16_t     sc_RHEnd;
    int16_t     sc_RHMax;


    int16_t     v_MinFreeze;
    int16_t     l_SunOn50;
    int16_t     l_SunOn100;
    int16_t     l_PauseMode;
    int16_t     sc_PauseMode;

    int16_t     l_SoftPauseMode;
    int16_t     o_TeplPosition;

    int16_t     c_RHStart;      // ������ 1,2 RH��� - RH��� �������� ������ �� ������� ���
    int16_t     c_RHEnd;        // ������ 1,2 RH��� - RH��� ������ �� ������� ��
    int16_t     c_RHOnMin1;     // ������ 1
    int16_t     c_RHOnMin2;     // ������ 2

    int16_t     CorrectionScreen; // ����� �������� ��
    int16_t     ScreenCloseSpeed; // �������� �������� ������

    int16_t     airHeatTemperOn;
    int16_t     airHeatTemperOff;
    int16_t     airHeatMinWork;
    int16_t     airHeatMaxWork;
    int16_t     airHeatPauseWork;

    int16_t     co2On;
    int16_t     co2Fram1;
    int16_t     co2Fram2;
    int16_t     co2Off;

    int16_t     Rez[5];  //9
//282
}
eTuneClimate;

// ---

typedef struct
{
    int16_t             LastDoT;
    int16_t             RealPower[2];
    int16_t             SErr;
    int16_t             DoT;
    int32_t             CalcT;
    int8_t              PumpStatus;
    int8_t              MainZone;
    int16_t             TPause;
    int8_t              Manual;
    int8_t              Separate;
    int16_t             SensValue;
    int8_t              NAndKontur;
    int8_t              PumpPause;
} eTControlKontur;

typedef struct
{
    int32_t             TimeSetMech;
    int32_t             TimeRealMech;
    int32_t             PauseMech;
    uint8_t              RCS;
    eSensing*       Sens;
    int32_t             PrevDelta;
    int32_t             PrevTask;
    int16_t         CalcTim;
    int8_t          TryMes;
    int8_t          TryMove;
} eMechBusy;

typedef struct
{
    int16_t             IntVal;
    int16_t             Stop;
    int16_t             Work;
    int8_t              On;
    int16_t             Pause;

} eRegsSettings;

typedef struct
{
    int16_t             Value;
    int8_t              Mode;
    int8_t              OldMode;
    int16_t             Pause;
    int16_t             PauseMode;
//		int16_t				TimeChangeMode;
//		int16_t				TempStart;
} eScreen;

typedef struct
{
    eTControlKontur Kontur[cSKontur];
    int16_t         LastLastInTeplSensing[cConfSSens];
    int16_t         LastInTeplSensing[cConfSSens];
    uint8_t         TimeInTepl[cConfSSens];
    int32_t         SaveIntegralVent;
    int32_t         Integral;
    int16_t         TVentCritery;//Critery;
    int16_t         Critery;
    int32_t         IntegralVent;
    int32_t         SaveIntegral;
//24
    int16_t         qMaxKonturs;
    int16_t         qMaxOwnKonturs;
    int16_t         AbsMaxVent;
    int16_t         LastTVentCritery;
    int16_t         LastCritery;
    int16_t         IntVal[cSWaterKontur];
//36
//		int16_t				PrevSig[cSWaterKontur];
//46
    eMechBusy       MechBusy[cSRegCtrl];
//126
    int8_t          WindWin[2];

//		int8_t			PausePump;//[cSWaterKontur];
    int8_t          ModeLight;
    int8_t          nMaxKontur;
//131
//		int8_t			NumLight;
    int16_t         PowMaxKonturs;
    int16_t         PowOwnMaxKonturs;
    int16_t         TimeSIO;
    int8_t          Vent;
    int8_t          PrevNLight;//CorrScreen;
//137
    int8_t          OldPozOn;
    int8_t          OldPozUn;
    int16_t         TForControl;

    int16_t         nReset;
    int16_t         COPosition;

    int32_t         Rez3[2];
//141

//143
    int16_t         PauseSIO;
    int8_t          CurVal;
    int8_t          FazaSiod;
    eScreen         Screen[6];
    int8_t          OutFan;

    eRegsSettings   SetupRegs[2];
//161
    int16_t         PauseVent;
    int8_t          Calorifer;
//164
    int8_t          StopI;
    int8_t          StopVentI;


    int16_t         SensHalfHourAgo;
    int16_t         SensHourAgo;
    int16_t         TimeSumSens;
    int32_t         SumSens;
    int16_t         LightPauseMode;
    int8_t          LightMode;
    int8_t          OldLightMode;
    int32_t         SumFAR;
    int8_t          DifLightMode;
    int8_t          RCS1;
    int16_t         LightExtraPause;
    int16_t         TempStart5;//sac_Integral2;
    int8_t          LastScrExtraHeat;
    int8_t          WithoutPC;
    int8_t          LightValue;
    int8_t          CurrPozFluger;
    int8_t          CurrPozFlugerTime;
    int8_t          UnWindStorm;
    int8_t          OnWindStorm;
    int8_t          ScrExtraHeat;//sac_fram;

//		int32_t			Functional;
    int32_t         RealPower;//MidlSens[2];
    int16_t         MaxDifT;//MidlTimeSens[2];

    int16_t         TPauseSIO;
//		eSensorD		SensorD;
    int8_t          FramUpdate[2];
    int8_t          PauseChangeLight;
    int8_t          NewLight;
    int16_t         COPause;
//		int16_t			Rez1[7];

    int8_t          NOwnKonturs;
    int8_t          CurrPower;
    int8_t          SnowTime;
    int16_t         s_Power;
    int16_t         ii_PFactor;
    int8_t          VentBlock;
    int16_t         f_Power;
    int16_t         f_NMinDelta;
    int8_t          bAlarm;
    int8_t          NAndKontur;
    int8_t          Alarms[MAX_ALARMS];
} eTControlZone;

typedef struct
{
    eTControlZone   Zones[NZONES];
//		int16_t 		LastLastMeteoSensing[cConfSMetSens];
    int16_t         MeteoSensing[cConfSMetSens];
    uint8_t         TimeMeteoSensing[cConfSMetSens];
    int32_t         SumSun;
    int8_t          PrevPozFluger;
    int8_t          Delay;
    int32_t         MidlSR;
    int16_t         Date;
//		int16_t			OutTemp;
//		int8_t			OutPause;
//		int16_t			OutSR;
//		int8_t			OutSRPause;
    int16_t         NowRasx;
    int32_t         FullVol;
    int8_t          bSnow;
    uint8_t         NowCod;
    uint8_t           tCodTime;
} eTControl;

// ---
typedef int16_t eSensLevel[4];

typedef struct
{
    eSensLevel InTeplSens[NZONES][cConfSSens];
    eSensLevel MeteoSens[cConfSMetSens];
} eLevel;

#endif
