//=======================================================
//---- Проект 405---------
//=======================================================

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
#warning CHECK THIS   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define cSmTCSens		1
#define cSmWinNSens		17
#define cSmWinSSens		18
#define cSmScreenSens	19
#define cSmWaterSens	20



#define cConfSSystem		10

#define cConfSOutput		46

#define cConfSInputs		7

#define cConfSSens			26

#define cConfSMetSens		11

#define cSHandCtrl			cConfSOutput

#define cSRegCtrl			24

#define cSDiskrCtrl			22



#define SUM_NAME_INPUTS			(cConfSOutput)

#define SUM_NAME_INSENS			(SUM_NAME_INPUTS+cConfSInputs)

#define SUM_NAME_OUTSENS		(SUM_NAME_INSENS+cConfSSens)

#define SUM_NAME_CONF			(SUM_NAME_OUTSENS+cConfSMetSens)


#define MAX_SUM_RELAY	80



eNameASens code  NameSensConfig[cConfSSens+cConfSMetSens]={
		/*
		  П - Номер порта
		  В - Номер входа
		  Мин - минимальное измеряемое значение
		  Макс - максимальное измеряемое значение
		  НЭ1 - напряжение эталон1
		  НЭ2 - напряжение эталон2
		  ЗЭ1 - значение эталон1
		  ЗЭ2 - значение эталон2
		  НМин - минимальное напряжение
		  НМкс - максимальное напряжение
		  У - тип усреднения датчика*/
/*Название датчика													Формат	Ед.изм	Тип датч	Т  Вых Мин	  Макс 		НЭ1		НЭ2	 	ЗЭ1  	ЗЭ2  	НМин	НМкс 	Усреднение 		Цифровой фильтр*/
//0
{"Tem\310 air1#Te\274\276 \263o\267\343\311xa 1",					SSpS0,  cuT,	cTypeAnal,	3,	0,	0,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	50}, /* Температура воздуха 1*/
{"Tem\310 air2#Te\274\276 \263o\267\343\311xa 2",					SSpS0,	cuT,	cTypeAnal,	3,	0,	0,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	50}, /* Температура контроль 2*/
{"Tem\310 air3#Te\274\276 \263o\267\343\311xa 3",					SSpS0,	cuT,	cTypeAnal,	3,	0,	0,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	50}, /* Температура контроль 3*/
{"Tem\310 air4#Te\274\276 \263o\267\343\311xa 4",					SSpS0,	cuT,	cTypeAnal,	3,	0,	0,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	50}, /* Температура контроль 4*/
{"Humidit\311 1#B\273a\266\275oc\277\304 1",						SSpS0,  cuPr,	cTypeRH,	0,	0,	200,  9800,		878,	3097,	0,		7530,	800, 	5000,	cExpMidlSens,	1000}, /* Влажность воздуха 1*/
{"Humidit\311 2#B\273a\266\275oc\277\304 2",						SSpS0,  cuPr,	cTypeRH,	0,	0,	200,  9800,		878, 	3097,	0,		7530,	800, 	5000,	cExpMidlSens,	1000}, /* Влажность воздуха 2*/
{"Inside light#B\275\311\277 c\263e\277",							SSSS, 	cuBt,	cTypeAnal,	11,	0,	0,	  1500,		15,  	2250,	0,   	1000, 	0,   	5000,	c3MidlSens,	100}, /* Внутренний свет*/
{"CO2#CO2 \311\310o\263e\275\304",									SSSS,   cuPpm,	cTypeRH,	0,	0,	100,  2000,		0,  	5000,	0,   	2000,	50,  	5000,	c3MidlSens,	80}, /*Концентрация СО2*/
//8
{"Tem\310 evap 1#Te\274\276 \270c\276a\310e\275 1",					SSpS0,  cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /* Температура вход воздуха 1*/
{"Tem\310 evap 2#Te\274\276 \270c\276a\310e\275 2",					SSpS0,  cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /* Температура вход воздуха 2*/
//10
{"Tem\310 cool #Te\274\276 ox\273a\266\343e\275",					SSpS0,  cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /* Температура охлаждающей воды*/
{"Over\310ressure#\245\267\262\303\277 \343a\263\273e\275",			SSpS0,  cuPa,	cTypeAnal,	0,	0,	1,	  9900,		0,		3000,	0,		3000,	0,		5000,	c3MidlSens,	80}, /* Избыточное давление*/
//12
{"Tem\310 roof#Te\274\276 \272\310o\263\273\270",					SSpS0,  cuT,	cTypeAnal,	3,	0,	0,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /* Температура кровли*/
{"Tem\310 soil#Te\274\276 \276o\300\263\303",						SSpS0,  cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /* Температура почвы*/
{"Tem\310 glass#Te\274\276 c\277e\272\273a",						SSpS0,  cuT,	cTypeAnal,	3,	0,	-2000,9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /*Температура стекла*/
{"Tem\310 in AHU#Te\274\276 \263xo\343a AHU",						SSpS0,	cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /*Т входа в AHU*/
{"Tem\310 out AHU#Te\274\276 \263\303xo\343a AHU",					SSpS0,	cuT,	cTypeAnal,	3,	0,	1,	  9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /*Т выхода из AHU*/
//17
{"N vent \310os#\250o\273o\266 \344\310a\274\311\264\270 C",		SSSpS,	cuPr,	cTypeFram,	1,	0,	0,	  1000, 	0,		5000,	0,	  	1000, 	0,		5000,	c3MidlSens,	400}, /*Сокодвижение*/
{"S vent \310os#\250o\273o\266 \344\310a\274\311\264\270 \260",		SSSpS,	cuPr,	cTypeFram,	1,	0,	0,	  1000, 	0,		5000,	0,	  	1000, 	0,		5000,	c3MidlSens,	400}, /*Размер плода*/
{"Screen \310os#\250o\273o\266 \305\272\310a\275",					SSSpS,	cuPr,	cTypeScreen,1,	0,	0,	  1000, 	0,		5000,	0,	  	1000, 	0,		5000,	c3MidlSens,	400}, /*Сокодвижение*/
//20
{"Tem\310 \310i\310e1#Te\274\276 \272o\275\277\311\310a 1",			SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т прям контура1*/
{"Tem\310 \310i\310e2#Te\274\276 \272o\275\277\311\310a 2",			SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т прям контура2*/
{"Tem\310 \310i\310e3#Te\274\276 \272o\275\277\311\310a 3",			SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т прям контура3*/
{"Tem\310 \310i\310e4#Te\274\276 \272o\275\277\311\310a 4",			SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т прям контура4*/
{"Tem\310 \310i\310e5#Te\274\276 \272o\275\277\311\310a 5",			SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т прям контура5*/
{"Tem\310 \310i\310e AHU#Te\274\276 \272o\275\277 AHU",				SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	2000,	5000,	c3MidlSens,	100}, /*Т контура AHU*/
//26
{"Tem\310 out#Te\274\276 \275a\310\311\266\275\307\307",			SSpS0,	cuT,	cTypeAnal,	3,	0,	-6000,9900,		2930,	3230,	2000,	5000,	2000,	5000,	c3MidlSens,	80}, /*Температура наружняя*/
{"FAR#\245\275\277e\275c\270\263\275oc\277\304 co\273\275\345a",	SSSS,	cuBt,	cTypeSun,	11,	0,	0,	  1500,		15,  	2250,	0,   	5000, 	0,   	5000,	c3MidlSens,	200}, /*Солнечная радиация*/
{"Wind s\310eed#C\272o\310oc\277\304 \263e\277\310a",				SSpS0,	cuMSec,	cTypeMeteo,	2,	0,	0,	  9900,		60,  	0,   	1,   	0,  	0,   	0,   	cNoMidlSens,100},			   /*Скорость ветра*/
{"Wind direct#Ha\276\310a\263\273 \263e\277\310a",					SSSi,	cuGr,	cTypeNULL,	2,	0,	0,	  360, 		0,   	4520,	0,   	360, 	0,   	0,   	cNoMidlSens,40},			   /*Направление ветра*/
{"Rain#Oca\343\272\270",											SSSi,	cu,		cTypeRain,	0,	0,	0,    10,  		250, 	650, 	-10, 	10,  	0,   	5000,  	cNoMidlSens,20},			   /*Осадки*/
{"Humidit\311 out#B\273a\266\275oc\277\304",						SSpS0,	cuPr,	cTypeAnal,	0,	0,	200,  9900,		800, 	5000,	0,   	0,   	0,   	5000,  	c3MidlSens,	100},					   /*Влажность наружняя*/
{"T from boil#Te\274\276 \276\310\307\274o\246",					SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	0,		0,		c3MidlSens,	100}, /*Температура воды прямой*/
{"T to boil#Te\274\276 o\262\310a\277\272\270",						SSSpS,	cuT,	cTypeAnal,	3,	0,	10,	  1500,		2930,	3230,	200,	500,	0,		0,		c3MidlSens,	100}, /*Температура воды обратной*/
{"P from boil#\340a\263\273e\275\270e \276\310\307\274o\246",		SSpS0,	cuPa,	cTypeAnal,	0,	0,	0,	  1500,		500, 	4500,	0,   	1464, 	0,   	5000,  	c3MidlSens,	100},			   /*Давление воды прямой*/
{"P to boil#\340a\263\273e\275\270e o\262\310a\277\272\270",		SSpS0,	cuPa,	cTypeAnal,	0,	0,	0,    1500,		500, 	4500,	0,   	1464, 	0,   	5000,  	c3MidlSens,	100},			   /*Давление воды обратной*/
{"Quantit\311#Pacxo\343",											SSSpS,	cuPa,	cTypeAnal,	0,	0,	0,    9000,		500, 	4500,	0,   	105, 	0,   	5000,  	c3MidlSens,	100},			   /*Резерв*/
        };

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



typedef struct  eeNameConfig
        {
        char Name[30];
        char Tip;
        char Min;
        char Max;
        } eNameConfig;

eNameConfig NameOutputConfig[cConfSOutput]={
/*0 Клапан 1*/ 			{"Mixing valve 1#C\274ec \272\273a\276a\275 1",tpRELAY,0,MAX_SUM_RELAY},
/*1 Клапан 2*/			{"Mixing valve 2#C\274ec \272\273a\276a\275 2",tpRELAY,0,MAX_SUM_RELAY},
/*2 Клапан 3*/			{"Mixing valve 3#C\274ec \272\273a\276a\275 3",tpRELAY,0,MAX_SUM_RELAY},
/*3 Клапан 4*/ 			{"Mixing valve 4#C\274ec \272\273a\276a\275 4",tpRELAY,0,MAX_SUM_RELAY},
/*4 Клапан 5*/			{"Mixing valve 5#C\274ec \272\273a\276a\275 5",tpRELAY,0,MAX_SUM_RELAY},
/*5 Клапан AHU*/		{"Mix valve AHU#C\274 \272\273a\276a\275 AHU",tpRELAY,0,MAX_SUM_RELAY},
/*6 Фрамуга СЕВЕР*/		{"Window NORTH#\252\310a\274\311\264a CEBEP",tpRELAY,0,MAX_SUM_RELAY},
/*7 Фрамуга ЮГ*/		{"Window SOUTH#\252\310a\274\311\264a \260\241",tpRELAY,0,MAX_SUM_RELAY},
/*8 Клапан UC*/			{"UC valve#UC \272\273a\276a\275",tpRELAY,0,MAX_SUM_RELAY},
/*9 Скорость AHU*/		{"AHU speed 1#C\272o\310oc\277\304 AHU 1",tpRELAY,0,MAX_SUM_RELAY},
/*10 Экран*/			{"Screen thermal#\257\272\310a\275 \277e\310\274\270\300",tpRELAY,0,MAX_SUM_RELAY},
/*11  Экран затеняющ*/	{"Screen shade#\257\272\310a\275 \267e\277e\275",tpRELAY,0,MAX_SUM_RELAY},
/*12  Верт. экран 1*/	{"Screen vert 1#\257\272\310a\275 \263e\310\277 1",tpRELAY,0,MAX_SUM_RELAY},
/*13 Верт. экран 2 */	{"Screen vert 2#\257\272\310a\275 \263e\310\277 2",tpRELAY,0,MAX_SUM_RELAY},
/*14 Верт. экран 3 */	{"Screen vert 3#\257\272\310a\275 \263e\310\277 3",tpRELAY,0,MAX_SUM_RELAY},
/*15 Верт. экран 4*/	{"Screen vert 4#\257\272\310a\275 \263e\310\277 4",tpRELAY,0,MAX_SUM_RELAY},
/*16 СО2 дозатор*/		{"CO2 valve#CO2 \272\273a\276a\275",tpRELAY,0,MAX_SUM_RELAY},
/*17 Регулятор давл*/	{"Pressure reg#Pe\264\311\273\307\277o\310 \343a\263\273",tpRELAY,0,MAX_SUM_RELAY},
/*18 Досветка */		{"Lighting#\340oc\263e\277\272a",tpRELAY,0,MAX_SUM_RELAY},
/*19 Скорость AHU*/		{"AHU speed 2#C\272o\310oc\277\304 AHU 2",tpRELAY,0,MAX_SUM_RELAY},
/*20 Резерв*/			{"Rezerv#He \270c\276o\273\304\267",tpRELAY,0,MAX_SUM_RELAY},
/*21 Резерв*/			{"Rezerv#He \270c\276o\273\304\267",tpRELAY,0,MAX_SUM_RELAY},
/*22 Резерв*/			{"Rezerv#He \270c\276o\273\304\267",tpRELAY,0,MAX_SUM_RELAY},
/*23 Резерв*/			{"Rezerv#He \270c\276o\273\304\267",tpRELAY,0,MAX_SUM_RELAY},

/*24 Насос 1*/			{"Heat \310um\310 1#Hacoc 1",tpRELAY,0,MAX_SUM_RELAY},
/*25 Насос 2*/			{"Heat \310um\310 2#Hacoc 2",tpRELAY,0,MAX_SUM_RELAY},
/*26 Насос 3*/			{"Heat \310um\310 3#Hacoc 3",tpRELAY,0,MAX_SUM_RELAY},
/*27 Насос 4*/			{"Heat \310um\310 4#Hacoc 4",tpRELAY,0,MAX_SUM_RELAY},
/*28 Насос 5*/			{"Heat \310um\310 5#Hacoc 5",tpRELAY,0,MAX_SUM_RELAY},
/*29 Насос 5*/			{"Pum\310 AHU#Hacoc AHU",tpRELAY,0,MAX_SUM_RELAY},
/*30 Вентиляторы*/		{"Fans#Be\275\277\270\273\307\277o\310\303",tpRELAY,0,MAX_SUM_RELAY},
/*31 Нагреватель*/		{"Heater#Ha\264\310e\263a\277e\273\304",tpRELAY,0,MAX_SUM_RELAY},
/*32 Охладитель*/		{"Cooler#Ox\273a\343\270\277e\273\304",tpRELAY,0,MAX_SUM_RELAY},
/*33 Насос СИО*/		{"Misting \310um\310#Hacoc C\245O",tpRELAY,0,MAX_SUM_RELAY},
/*34 Клапан СИО*/		{"Mist valve start#K\273a\276a\275\303 C\245O",tpRELAY,0,MAX_SUM_RELAY},
/*35 Клапан AHU*/		{"Mist AHU valves#K\273a\276a\275\303 AHU",tpRELAY,0,MAX_SUM_RELAY},

/*36 Авария*/			{"Alarm#A\263a\310\270\307",tpRELAY,0,MAX_SUM_RELAY},

/*37 Регулятор 1*/		{"Regulator 1#Pe\264\311\273\307\277o\310 1",tpRELAY,0,MAX_SUM_RELAY},
/*38 Регулятор 2*/		{"Regulator 2#Pe\264\311\273\307\277o\310 2",tpRELAY,0,MAX_SUM_RELAY},
/*39 Регулятор 3*/		{"Regulator 3#Pe\264\311\273\307\277o\310 3",tpRELAY,0,MAX_SUM_RELAY},
/*40 Регулятор 4*/		{"Regulator 4#Pe\264\311\273\307\277o\310 4",tpRELAY,0,MAX_SUM_RELAY},
/*41 Регулятор 5 */		{"Regulator 5#Pe\264\311\273\307\277o\310 5",tpRELAY,0,MAX_SUM_RELAY},

/*42 Регулятор 1*/		{"Rez 1#Pez 1",tpRELAY,0,MAX_SUM_RELAY},
/*43 Регулятор 1*/		{"Rez 1#Pez 1",tpRELAY,0,MAX_SUM_RELAY},
/*44 Регулятор 1*/		{"Rez 1#Pez 1",tpRELAY,0,MAX_SUM_RELAY},
/*45 Регулятор 1*/		{"Rez 1#Pez 1",tpRELAY,0,MAX_SUM_RELAY},

};


#define cSmDHeat		0
#define cSmDVent		1
#define cSmDLight50		2
#define cSmDLight100	3
#define cSmRainDiskr	4
#define cSmDCO2			5
#define cSmDLight		6


eNameConfig NameInputConfig[cConfSInputs]={
/*0 Авария отопления*/ 			{"Heating alarm#A\263a\310\270\307 o\277o\272\273e\275",tpLEVEL,0,MAX_SUM_RELAY},
/*1 Авария вентиляции*/			{"Vent alarm#A\263a\310\270\307 \263e\275\277\270\273",tpLEVEL,0,MAX_SUM_RELAY},
/*2 Досветка 50%*/				{"Light 50%#50% \343oc\263e\277",tpLEVEL,0,MAX_SUM_RELAY},
/*3 Досветка 100%*/ 			{"Light 100%#100% \343oc\263e\277",tpLEVEL,0,MAX_SUM_RELAY},
/*4 Осадки*/					{"Rain#Oca\343\272\270",tpLEVEL,0,MAX_SUM_RELAY},
/*5 Авария СО2*/				{"CO2 alarm#A\263a\310\270\307 CO2",tpLEVEL,0,MAX_SUM_RELAY},
/*6 Авария света*/				{"Light alarm#A\263a\310\270\307 \343oc\263e\277",tpLEVEL,0,MAX_SUM_RELAY}
};

#define SUM_NAME_PARS 24
code struct  eNameParUpr
        {
        char Name[30];
		char Ed;
        }
NameParUpr[SUM_NAME_PARS]={
/*0 Клапан 1*/ 			{"Max \310i\310e 1#Ma\272c \272o\275\277\311\310 1",SSSpS},
/*1 Клапан 2*/			{"Max \310i\310e 2#Ma\272c \272o\275\277\311\310 2",SSSpS},
/*2 Клапан 3*/			{"Max \310i\310e 3#Ma\272c \272o\275\277\311\310 3",SSSpS},
/*3 Клапан 4*/ 			{"Max \310i\310e 4#Ma\272c \272o\275\277\311\310 4",SSSpS},
/*4 Клапан 5*/			{"Max \310i\310e 5#Ma\272c \272o\275\277\311\310 5",SSSpS},
/*5 Клапан AHU*/		{"Max \310i\310e AHU#Ma\272c \272o\275\277 AHU",SSSpS},
/*6 Фрамуга СЕВЕР*/		{"Max vent UnW#Ma\272c \344\310a\274 \276o\343\263",SSSS},
/*7 Фрамуга ЮГ*/		{"Max vent OnW#Ma\272c \344\310a\274 \275a\263",SSSS},
/*8 Минимум контура 3*/	{"Min tem\310 \310i\310e 3#M\270\275 \272o\275\277\311\310 3",SSSpS},
/*9 Минимум контура 4*/	{"Min tem\310 \310i\310e 4#M\270\275 \272o\275\277\311\310 4",SSSpS},
/*10 Держать разн. давл*/{"Diff \310ressure#Pa\267\275\270\345a \343a\263\273",SSSpS},
/*11 Оптимум контура 3*/{"O\310timal \310i\310e3#O\276\277\270\274\311\274 \272o\275\277 3",SSSpS},
/*12 Оптимум контура 4*/{"O\310timal \310i\310e4#O\276\277\270\274\311\274 \272o\275\277 4",SSSpS},
/*13 И- поправка вент*/	{"Vent I-band#\252\310a\274\311\264\270 \245-\272o\305\344",SSpS0},
/*14 П- поправка отопл*/{"Heat P-band#Te\276\273o \250-\272o\305\344",SSpS0},
/*15 И- поправка отопл*/{"Heat I-band#Te\276\273o \245-\272o\305\344",SSpS0},
/*16 П- поправка вент*/	{"Vent P-band#\252\310a\274\311\264\270 \250-\272o\305\344",SSpS0},
/*17 Нет*/				{"Unused#He \270c\276o\273\304\267\311e\274",SSSS},
/*18 Экран*/			{"Max screen T#Ma\272c \305\272\310a\275 T",SSSS},
/*19  Экран затеняющ*/	{"Max screen S#Ma\272c \305\272\310a\275 \244",SSSS},
/*20  CO2*/				{"CO2 type#T\270\275 CO2",SSSS},
/*21  SIO*/				{"SIO#\SIO",SSSS},
/*22  Досветка*/		{"Light#\Light",SSSS},
/*23  T heat\vent*/		{"T heat vent #T heat vent",SSSS}
};


eNameConfig NameSystemConfig[cConfSSystem]={
/*0 Система отопления*/ 			{"Heating system",tpSUM,0,MAX_SUM_RELAY},
/*1 Система вентиляции*/			{"Ventilation system",tpSUM,0,MAX_SUM_RELAY},
/*2 Система зашторивания*/			{"Screening system",tpSUM,0,MAX_SUM_RELAY},
/*3 Система ультраклима*/ 			{"UltraClima system",tpSUM,0,MAX_SUM_RELAY},
/*4 Система СО2*/					{"CO2 system",tpSUM,0,MAX_SUM_RELAY},
/*5 Система досвечивания*/			{"Lighting system",tpSUM,0,MAX_SUM_RELAY},
/*6 Система СИОД*/					{"Misting system",tpSUM,0,MAX_SUM_RELAY},
/*7 Система рециркуляции*/			{"Fans&Heaters",tpSUM,0,MAX_SUM_RELAY},
/*8 Резерв1*/						{"Rezerved",tpSUM,0,MAX_SUM_RELAY},
/*9 Резерв2*/						{"Rezerved",tpSUM,0,MAX_SUM_RELAY}
};


#define SUM_NAME_TIMER 24
code struct  eNameTimer {
        char Name[30];
		char Frm;
        char Index;
        }

NameTimer[SUM_NAME_TIMER]={
/*Формат	Индекс	*/
{"Zone number#Ho\274e\310 \267o\275\303", 		oS,  		0}, 
{"Time start#B\310e\274\307 c\277a\310\277a",			SSdSS,  	1}, 
{"Type start#T\270\276 c\277a\310\277a",			SS,  	3},
{"Heat tem\310#Te\274\276 o\277o\276\273e\275\270\307",	SSpS0,  	5}, 
{"Vent tem\310#Te\274\276 \263e\275\277\270\273\307\345\270\270",	SSpS0,  	7}, 
{"Humidit\311#B\273a\266\275oc\277\304",			SS,		29},
{"CO2 level#\251\310o\263e\275\304 CO2",			SSSS,  	9},
{"Light \310ower#\250\310o\345e\275\277 \343oc\263e\277\272\270",		SSS,	26},
{"Light mode#Pe\266\270\274 \343oc\263e\277\272\270",			oS,		27},
 
{"Min tem\310 \310i\310e1#M\270\275 \272o\275\277\311\310 1",		SS,		16},
{"Min tem\310 \310i\310e2#M\270\275 \272o\275\277\311\310 2",		SS,		17},
{"Min tem\310 \310i\310e3#M\270\275 \272o\275\277\311\310 3",		SS,		28},
{"Min tem\310 \310i\310e5#M\270\275 \272o\275\277\311\310 5",		SS,		15},

{"O\310timal \310i\310e1#O\276\277\270\274\311\274 \272o\275\277\311\3101",		SS,		11}, 
{"O\310timal \310i\310e2#O\276\277\270\274\311\274 \272o\275\277\311\3102",		SS,		12},
 
{"Tem\310 \310i\310e 3#Te\274\276 \272o\275\277\311\310a 3",			SS,		13}, 
{"Tem\310 \310i\310e 4#Te\274\276 \272o\275\277\311\310a 4",			SS,		14}, 

{"Vent mode#Pe\266\270\274 \344\310a\274\311\264",			oS,		19},
{"Min vent \310os#M\270\275 \344\310a\274\311\264",		SS,		18}, 

{"Mist \310ulse#Pac\276\303\273\307\277\304 C\245O",			SSSS,  	20},

{"T screen mode#Pe\266\270\274 \305\272\310a\275 T",	oS,		22}, 
{"S screen mode#Pe\266\270\274 \305\272\310a\275 \244",	oS,		23}, 
{"V screen mode#Pe\266\270\274 \305\272\310a\275 B",	oS,		24}, 
{"Fans mode#Pe\266\270\274 \263e\275\277\270\273\307\277o\310",			oS,		25}, 


};




//----------------------------------------


