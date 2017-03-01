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
    int16_t     DifTAirTDo;//Разница междуизмеренным и заданным
    int16_t     UpSR;//Повышение температуры теплицы солнцем	2
    int16_t     LowGlass;//Понижает стекло	4
    int16_t     LowOutWinWind;//Понижает ветер и внешняя температура	6
    int16_t     UpLight;//Повышает включение досветки	8
    int16_t     ICorrection;
    int16_t     dSumCalcF;//Понижают осадки	12

    int16_t     DiffCO2;//Корректирующий коэффициент в зависимости от колебательных процессов	14
    int16_t     TVentCritery;//Изменяет экран	16
    int16_t     PCorrection;//Коррекция управляющей функции по 1 разности	18

    int16_t     Critery;//Цель-изменить теплоноситель на	20
    int16_t     ICorrectionVent;//Прогноз температуры (для приоритетов)	22

    int16_t     dSumCalc;//Приращение внешних влияний	24

    int16_t     PCorrectionVent;//Остаток от предыдущих расчетов	26

    int16_t     CorrectionScreen; // экран понижает на

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
    0x01 - 1 - выключен
    0x02 - 1 - выход за пределы измеряемого диапазона
    0x04 - 1 - выход за пределы диапазона измерительного напряжения
    0x08 - 1 - превышена верхняя аварийная граница измерения
    0x10 - 1 - превышена верхняя контрольная граница измерения
    0x20 - 1 - превышена нижняя контрольная граница измерения
    0x40 - 1 - превышена нижняя аварийная граница измерения
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
    0х01 - 1 - нет задания температуры
    0х02 - 1 - нет измерения температуры
    0х04 - 1 - температура скорректирована по солнцу
    0х08 - 1 - температура
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

    int16_t     MaxReqWater;  // надо теплоноситель
    uint16_t    newsZone;       // new
    int16_t     Rez[8];    // 9
    eMechanic   HandCtrl[cSHandCtrl];
} eZone;


typedef struct
{
//0
    uint8_t       News;
    /*Блок пиема из ПК*/
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
    int16_t     c_MaxTPipe[cSWaterKontur];   /*Максимально допустимая температура Контур1-5*/
    int16_t     f_MaxOpenUn;   /*Фрамуги_Максимально допустимое открытие*/
    int16_t     f_MaxOpenOn;   /*Фрамуги_Максимум при подкормке СО2*/
    int16_t     c_MinTPipe[2];
    int16_t     c_DoPres;
    int16_t     c_OptimalTPipe[2];
    int16_t     f_IFactor;
    int16_t     c_PFactor;  /*Контур 1 - Динамика(Тзад-Тизм)влияет до*/
    int16_t     c_IFactor; /*Интегральная поправка контуров*/
    int16_t     f_PFactor; /*Контур 1 - Динамика(Тзад-Тизм)понижает на*/
    int8_t      vs_DegSt; //Градусы для вычисления позиции экрана
    int8_t      vs_DegEnd; //Градусы для вычисления позиции экрана
    int16_t     sc_TMaxOpen; // экран термический
    int16_t     sc_ZMaxOpen; // экран затеняющий
    uint16_t    co_model;/*СО2 - исполнитель(0-регулятор,1-клапан)*/
    uint16_t    sio_SVal;
    uint16_t    sLight;
    int8_t      sensT_heat;
    int8_t      sensT_vent;

    int16_t     Rez[19];
//+42 байта
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
    int16_t     v_PFactor;  /*Смесительный клапан - П-коэф разницы Т воды*/
    int16_t     v_IFactor;  /*Смесительный клапан - И-коэф разницы Т воды*/
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

/*-- начало сохраняемых данных, размер 130 байт*/
/*-----------------------------------
                Параметры-солнце
------------------------------------*/
    int16_t     s_TStart[1];   /*Температура задана - солнце начинает влиять при*/
    int16_t     s_TEnd;   /*Температура задана - солнце влияет до*/
    int16_t     s_TConst;  /*Температура задана - солнце увеличивает на*/
    int16_t     s_TVentConst;   /*Влажностьзадана - солнце начинает влиять при*/

    int16_t     cool_PFactor;   /*Влажностьзадана - солнце влияет до*/
    int16_t     s_RHConst;  /*Влажностьзадана - солнце увеличивает на*/
    int16_t     COPause;   /*Концентрация СО2 - солнце начинает влиять при*/
    int16_t     f_SRFactor;   /*Концентрация СО2 - солнце влияет до*/

    int16_t     s_CO2Const;  /*Концентрация СО2 - солнце увеличивает на*/
    int16_t     f_OutFactor;   /*Контур 1_Минимум в прогр- солнце начинает влиять при*/
    int16_t     f_CorrTVentUp;   /*Контур 1_Минимум в программе - солнце влияет до*/
    int16_t     s_MinTPipeConst;/*Контур 1_Минимум в программе - солнце уменьшает на*/

    int16_t     s_MinTPipe5;   /*Фрамуги_Минимум в прогр- солнце начинает влиять при*/
    int16_t     s_MinTPipe3;   /*Фрамуги_Минимум в программе - солнце влияет до*/
    int16_t     s_MinOpenWinConst; /*Фрамуги_Минимум в программе - солнце увеличивает на*/
//30 байт
/*-----------------------------------
                Параметры-для температуры вентиляции
------------------------------------*/
    int16_t     f_min_RHStart;
    int16_t     f_min_RHEnd;
    int16_t     f_max_RHStart;
    int16_t     f_max_RHEnd;
    int8_t      f_min_Cor;
    int8_t      f_max_Cor;
//42 байта
/*-----------------------------------
                Параметры-обогрев
------------------------------------*/
//64 байта
    int16_t     c_DoMinIfGlass;  /*Контур 5 - вкл если осадки и Твнеш меньше*/
    int16_t     f_CorrTVent;  /*Контур 5 - при осадках держать*/
    int16_t     c_MinIfSnow;  /*Контур 5 - держать минимум при внешней температуре меньше*/

    int16_t     o_DeltaLight;
    int8_t      f_changeWindows;
    int8_t      f_only;
//74 байта
/*-----------------------------------
                Параметры-влияния на Тводы
------------------------------------*/

    int8_t      f_MaxAngle;  /*Контур 1 - (Тзад-Тизм)начинает влиять при*/
    int8_t      c_MullDown;  /*Контур 1 - (Тзад-Тизм)начинает влиять при*/
    int8_t      f_DefOnUn;  /*Контур 1 - (Тзад-Тизм)влияет до*/

    int8_t      c_5ExtrHeat;  /*Контур 1 - (Тзад-Тизм)влияет до*/
    int16_t     c_CriticalSnowOut;  /*Контур 1 - (Тзад-Тизм)увеличивает на*/
    int16_t     c_DoMaxIfGlass;  /*Контур 1 - Динамика(Тзад-Тизм)начинает влиять при*/


    int16_t     c_GlassStart;  /*Контур 1 - (Тзад-Тстекла)начинает влиять при*/
    int16_t     c_GlassEnd;  /*Контур 1 - (Тзад-Тстекла)влияет до*/
    int16_t     c_GlassFactor;  /*Контур 1 - (Тзад-Тстекла)увеличивает на*/
/*Искуственный интеллект*/
    int16_t     c_MaxDifTDown;  /*Контур 1 - (Тзад-Тшатра)начинает влиять при*/
    int16_t     f_MinSun;  /*Контур 1 - (Тзад-Тшатра)влияет до*/
    int16_t     f_DeltaOut;  /*Контур 1 - (Тзад-Тшатра)увеличивает на*/
/*______________________*/
    int16_t     c_SRStart;  /*Контур 1 - солнце начинает влиять при*/
    int16_t     c_SREnd;  /*Контур 1 - солнце влияет до*/
    int16_t     c_SRFactor;  /*Контур 1 - солнце уменьшает на*/

    int16_t     c_OutStart;  /*Контур 1 - (Тзад-Твнеш)начинает влиять при*/
    int16_t     c_OutEnd;  /*Контур 1 - (Тзад-Твнеш)влияет до*/
    int16_t     c_OutFactor;  /*Контур 1 - (Тзад-Твнеш)увеличивает на*/

    int16_t     c_WindStart;  /*Контур 1 - ветер начинает влиять при*/
//114 байта
    int16_t     c_WindEnd;  /*Контур 1 - ветер влияет до*/
    int16_t     c_WindFactor;  /*Контур 1 - ветер увеличивает на*/

    int16_t     sc_GlassStart;  /*Контур 1 - фрамуги начинают влиять при*/
    int16_t     sc_GlassEnd;  /*Контур 1 - фрамуги влияют до*/
    int16_t     sc_GlassMax;  /*Контур 1 - фрамуги увеличивают на*/

    int16_t     c_LightFactor;  /*Контур 1 - понизить при включении досветки*/
    int16_t     c_ScreenFactor;  /*Контур 1 - понизить при закрытом экране*/
    int16_t     c_CloudFactor;  /*Контур 1 - повысить при осадках*/
//  100байтНОВ


/*-----------------------------------
                Параметры-вентиляция
------------------------------------*/
    uint8_t       f_S1MinDelta; /*Фрамуги - шагать на первом уровне*/
    uint8_t       f_S1Level;
    uint8_t       f_S2MinDelta;
    uint8_t       f_S2Level;
    uint8_t       f_S3MinDelta;
    uint8_t       f_S3Level;
//+6
    int16_t     f_MinDelta;  /*Фрамуги - исполнять если шаг больше*/
    int16_t     c_MaxDifTUp;  /*Фрамуги - максимальная Пауза между включениями*/
    int16_t     f_MinTime;  /*Фрамуги - минимальная Пауза между включениями*/
    int16_t     f_StormWindOn; /*Фрамуги - Ветер закрывает Наветренную сторону при*/
    int16_t     f_StormWind; /*Фрамуги - Ветер закрывает Подветренную сторону при*/
    int16_t     f_WindStart;  /*Фрамуги - Ветер начинает влиять при*/
//154 байта
//+18
    int16_t     f_WindHold;  /*Фрамуги - Внешр темп допускает максимум открытия при*/
    int16_t     f_MinTFreeze;   /*Фрамуги - Внешн темп закрывает при*/
//+22
    int16_t     f_MaxOpenRain;  /*Фрамуги - при Дожде допустимо открывать на*/
    int8_t      co_MaxTime; /*Фрамуги - Флюгер меняет наветренную сторону при*/
    int8_t      co_MinTime;
/*-----------------------------------
                Параметры-влияние на фрамуги
------------------------------------*/

    int8_t      co_Impuls;       /*Фрамуги - (Тизм-Твентиляции)открыть на максимум при*/
    int8_t      co_Dif;
    int16_t     f_StartCorrPow;  /*Фрамуги - Солнце начинает влиять при*/
    int16_t     f_EndCorrPow;  /*Фрамуги - Солнце влияет до*/
    int16_t     f_PowFactor;        /*Фрамуги - Солнце увеличивает на*/
/*-----------------------------------
                Параметры-CO2
-----------------------------------*/
    int16_t     reg_PFactor[2];/*СО2 - коэф Пропорциональный*/
/*-----------------------------------
                Параметры-Экран
------------------------------------*/
    int16_t     sc_ZSRClose; /*Экран затеняющий - Солнце закрывает при*/
    int16_t     sc_TSROpen;  /*Экран термический - Солнце открывает при*/
    int16_t     sc_TOutClose;/*Экран термический - Твнеш днем закрывает при*/
    int16_t     sc_TVOutClose;/*Экран термический - Твнеш ночью закрывает при*/

    int16_t     sc_ZOutClose;/*Экран термический - ветер начинает влиять при*/
    int16_t     sc_TWindStart;/*Экран термический - ветер влияет до*/
    int16_t     sc_TVSRMaxOpen; /*Экран термический - ветер увеличивает Т внеш на*/
    int16_t     sc_TVSROpen;   /*Экран термический - Солнце открывает вертикальный тоже при*/

    int16_t     f_WindFactor;/*Экран - (Тзад-Тизм) влияет до*/
    int16_t     sc_TFactor;/*Экран - (Тзад-Тизм) приоткрывает на*/
    int16_t     s_StartCorrPow;  /*Фрамуги - Солнце начинает влиять при*/
    int16_t     s_EndCorrPow;  /*Фрамуги - Солнце влияет до*/

    int16_t     s_PowFactor;        /*Фрамуги - Солнце увеличивает на*/
    int16_t     sc_StartP2Zone;/*Экран закрывать шагами по*/
    int16_t     sc_StepS2Zone;/*Экран закрывать-количество шагов*/
    int16_t     sc_StepP2Zone;/*Экран закрывать-пауза между включениями*/

    int16_t     sc_StartP1Zone;/*Экран открывать шагами по*/
    int16_t     sc_StepS1Zone;/*Экран открывать-количество шагов*/
    int16_t     sc_StepP1Zone;/*Экран открывать-пауза между включениями*/
    int16_t     sc_MinDelta;/*Экран исполнять если шаг больше*/
//214
/*-----------------------------------
                Параметры- СИОД
------------------------------------*/
    int16_t     sio_TStart;/*СИОД - включается,если температура больше задания на*/
    int16_t     sio_TStartFactor;/*СИОД - при минимальном превышении повторяется через*/
    int16_t     sio_TEnd;/*СИОД - температура больше задания влияет до*/
    int16_t     sio_TEndFactor;/*СИОД - при большом рассогласовании повторяется через*/
    int16_t     sio_TStop;/*СИОД - запрещен,если температура меньше задания на*/
    int16_t     sio_RHStart;/*СИОД - включается,если влажность меньше задания на*/
    int16_t     sio_RHStartFactor;/*СИОД - при минимальном превышении повторяется через*/
    int16_t     sio_RHEnd;/*СИОД - влажность меньше задания влияет до*/
    int16_t     sio_RHEndFactor;/*СИОД - при большом рассогласовании повторяется через*/
    int16_t     sio_RHStop;/*СИОД - запрещен,если влажность больше задания на*/

    int16_t     ScaleRasx;/*СИОД - пауза между включениями клапанов*/

/*-----------------------------------
                Параметры-Вентиляторы
-----------------------------------*/

    int16_t     vt_PFactor;/*Вентиляторы - включать, если рассогласование больше*/
    int16_t     vt_WorkTime;/*Вентиляторы - время работы*/
    int16_t     vt_StopTime;/*Вентиляторы - время паузы*/

/*-----------------------------------
                Параметры-воздушный обогрев
------------------------------------*/
    int16_t     vt_StartCalorifer;/*Воздушный обогрев - включать,если температура меньше на*/
    int16_t     vt_EndCalorifer;/*Воздушный обогрев - выключать,если температура больше на*/
/*-----------------------------------------
                Параметры - ОБЩИЕ
------------------------------------------*/
    int16_t     sc_dTStart;     /*Коэффициент усреднения солнечой радиации*/
    int16_t     sc_dTEnd;       /*Коэффициент усреднения ветра*/
    int16_t     sc_dTSunFactor; /*Коэффициент упреждения*/
/*-----------------------------------------
                Параметры - коррекция по влажности темпеатуры задания
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

    int16_t     c_RHStart;      // Контур 1,2 RHизм - RHзад начинает влиять на минимум при
    int16_t     c_RHEnd;        // Контур 1,2 RHизм - RHзад влияет на минимум до
    int16_t     c_RHOnMin1;     // Контур 1
    int16_t     c_RHOnMin2;     // Контур 2

    int16_t     CorrectionScreen; // экран понижает на
    int16_t     ScreenCloseSpeed; // скорость закрытия экрана

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
