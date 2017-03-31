

#include "syntax.h"

//#include "control_const.h"
#include "405_ConfigEnRuSR.h"

const eNameASens NameSensConfig[cConfSSens+cConfSMetSens]={
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
/*Название датчика               Формат	Ед.изм	Тип датч  Т  Вых Мин Макс 		НЭ1		НЭ2	 	ЗЭ1  	ЗЭ2  	НМин	НМкс 	Усреднение 		Цифровой фильтр*/
//0
	{"Temp air1#Темп воздуха 1", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* Температура воздуха 1*/
	{"Temp air2#Темп воздуха 2", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* Температура контроль 2*/
	{"Temp air3#Темп воздуха 3", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* Температура контроль 3*/
	{"Temp air4#Темп воздуха 4", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* Температура контроль 4*/
	{"Humidity 1#Влажность 1",   SSpS0, cuPr,  cTypeRH,   0,  0, 200, 9800, {878,  3097}, {0,    7530}, 800,  5000, cExpMidlSens, 1000}, /* Влажность воздуха 1*/
	{"Humidity 2#Влажность 2",   SSpS0, cuPr,  cTypeRH,   0,  0, 200, 9800, {878,  3097}, {0,    7530}, 800,  5000, cExpMidlSens, 1000}, /* Влажность воздуха 2*/
	{"Inside light#Внут свет",   SSSS,  cuBt,  cTypeAnal, 11, 0, 0,   1500, {15,   2250}, {0,    1000}, 0,    5000, c3MidlSens,   100},  /* Внутренний свет*/
	{"CO2#CO2 уровень",          SSSS,  cuPpm, cTypeRH,   0,  0, 100, 2000, {0,    5000}, {0,    2000}, 50,   5000, c3MidlSens,   80},   /*Концентрация СО2*/
	 //8
//8
	{"Temp evap 1#Темп испарен 1", SSpS0, cuT, cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* Температура вход воздуха 1*/
	{"Temp evap 2#Темп испарен 2", SSpS0, cuT, cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* Температура вход воздуха 2*/

//10
	{"Temp cool #Темп охлажден",  SSpS0, cuT,  cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* Температура охлаждающей воды*/
	{"Overpressure#Избыт давлен", SSpS0, cuPa, cTypeAnal, 0, 0, 1, 9900, {0,    3000}, {0,    3000}, 0,    5000, c3MidlSens, 80}, /* Избыточное давление*/

//12
	{"Temp roof#Темп кровли",        SSpS0, cuT, cTypeAnal, 3, 0, 0,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* Температура кровли*/
	{"Temp soil#Темп почвы",         SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* Температура почвы*/
	{"Temp glass#Темп стекла",       SSpS0, cuT, cTypeAnal, 3, 0, -2000, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*Температура стекла*/
	{"Temp in AHU#Темп входа AHU",   SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*Т входа в AHU*/
	{"Temp out AHU#Темп выхода AHU", SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*Т выхода из AHU*/

//17
	{"N vent pos#Полож Фрамуги С", SSSpS, cuPr, cTypeFram,   1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*Сокодвижение*/
	{"S vent pos#Полож Фрамуги Ю", SSSpS, cuPr, cTypeFram,   1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*Размер плода*/
	{"Screen pos#Полож экран",     SSSpS, cuPr, cTypeScreen, 1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*Сокодвижение*/

//20
	{"Temp pipe1#Темп контура 1",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т прям контура1*/
	{"Temp pipe2#Темп контура 2",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т прям контура2*/
	{"Temp pipe3#Темп контура 3",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т прям контура3*/
	{"Temp pipe4#Темп контура 4",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т прям контура4*/
	{"Temp pipe5#Темп контура 5",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т прям контура5*/
	{"Temp pipe AHU#Темп конт AHU", SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*Т контура AHU*/

//26
	{"Temp out#Темп наружняя",      SSpS0, cuT,    cTypeAnal,  3,  0, -6000, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,  80},  /*Температура наружняя*/
	{"FAR#Интенсивность солнца",    SSSS,  cuBt,   cTypeSun,   11, 0, 0,     1500, {15,   2250}, {0,    5000}, 0,    5000, c3MidlSens,  200}, /*Солнечная радиация*/
	{"Wind speed#Скорость ветра",   SSpS0, cuMSec, cTypeMeteo, 2,  0, 0,     9900, {60,   0},    {1,    0},    0,    0,    cNoMidlSens, 100}, /*Скорость ветра*/
	{"Wind direct#Направл ветра",   SSSi,  cuGr,   cTypeNULL,  2,  0, 0,     360,  {0,    4520}, {0,    360},  0,    0,    cNoMidlSens, 40},  /*Направление ветра*/
	{"Rain#Осадки",                 SSSi,  cu,     cTypeRain,  0,  0, 0,     10,   {250,  650},  {-10,  10},   0,    5000, cNoMidlSens, 20},  /*Осадки*/
	{"Humidity out#Влажность",      SSpS0, cuPr,   cTypeAnal,  0,  0, 200,   9900, {800,  5000}, {0,    0},    0,    5000, c3MidlSens,  100}, /*Влажность наружняя*/
	{"T from boil#Темп прямой",     SSSpS, cuT,    cTypeAnal,  3,  0, 10,    1500, {2930, 3230}, {200,  500},  0,    0,    c3MidlSens,  100}, /*Температура воды прямой*/
	{"T to boil#Темп обратки",      SSSpS, cuT,    cTypeAnal,  3,  0, 10,    1500, {2930, 3230}, {200,  500},  0,    0,    c3MidlSens,  100}, /*Температура воды обратной*/
	{"P from boil#Давление прямой", SSpS0, cuPa,   cTypeAnal,  0,  0, 0,     1500, {500,  4500}, {0,    1464}, 0,    5000, c3MidlSens,  100}, /*Давление воды прямой*/
	{"P to boil#Давление обратки",  SSpS0, cuPa,   cTypeAnal,  0,  0, 0,     1500, {500,  4500}, {0,    1464}, 0,    5000, c3MidlSens,  100}, /*Давление воды обратной*/
	{"Quantity#Расход",             SSSpS, cuPa,   cTypeAnal,  0,  0, 0,     9000, {500,  4500}, {0,    105},  0,    5000, c3MidlSens,  100}, /*Резерв*/
};


const eNameTimer NameTimer[SUM_NAME_TIMER]={
/*Формат	Индекс	*/
	{"Zone number#Номер зоны",        oS,    0},
	{"Time start#Время старта",       SSdSS, 1},
	{"Type start#Тип старта",         SS,    3},
	{"Heat temp#Темп отопления",      SSpS0, 5},
	{"Vent temp#Темп вентиляции",     SSpS0, 7},
	{"Humidity#Влажность",            SS,    29},
	{"CO2 level#Уровень CO2",         SSSS,  9},
	{"Light power#Процент досветки",  SSS,   26},
	{"Light mode#Режим досветки",     oS,    27},

	{"Min temp pipe1#Мин контур 1",   SS,    16},
	{"Min temp pipe2#Мин контур 2",   SS,    17},
	{"Min temp pipe3#Мин контур 3",   SS,    28},
	{"Min temp pipe5#Мин контур 5",   SS,    15},

	{"Optimal pipe1#Оптимум контур1", SS,    11},
	{"Optimal pipe2#Оптимум контур2", SS,    12},

	{"Temp pipe 3#Темп контура 3",    SS,    13},
	{"Temp pipe 4#Темп контура 4",    SS,    14},

	{"Vent mode#Режим Фрамуг",        oS,    19},
	{"Min vent pos#Мин Фрамуг",       SS,    18},

	{"Mist pulse#Распылять СИО",      SSSS,  20},

	{"T screen mode#Режим экран T",   oS,    22},
	{"S screen mode#Режим экран З",   oS,    23},
	{"V screen mode#Режим экран В",   oS,    24},
	{"Fans mode#Режим вентилятор",    oS,    25},
};

// XXX: should be const ?
const eNameConfig NameSystemConfig[cConfSSystem]={
	/*0 Система отопления*/             {"Heating system",     tpSUM, 0, MAX_SUM_RELAY},
	/*1 Система вентиляции*/            {"Ventilation system", tpSUM, 0, MAX_SUM_RELAY},
	/*2 Система зашторивания*/          {"Screening system",   tpSUM, 0, MAX_SUM_RELAY},
	/*3 Система ультраклима*/           {"UltraClima system",  tpSUM, 0, MAX_SUM_RELAY},
	/*4 Система СО2*/                   {"CO2 system",         tpSUM, 0, MAX_SUM_RELAY},
	/*5 Система досвечивания*/          {"Lighting system",    tpSUM, 0, MAX_SUM_RELAY},
	/*6 Система СИОД*/                  {"Misting system",     tpSUM, 0, MAX_SUM_RELAY},
	/*7 Система рециркуляции*/          {"Fans&Heaters",       tpSUM, 0, MAX_SUM_RELAY},
	/*8 Резерв1*/                       {"Rezerved",           tpSUM, 0, MAX_SUM_RELAY},
	/*9 Резерв2*/                       {"Rezerved",           tpSUM, 0, MAX_SUM_RELAY},

};

const eNameParUpr NameParUpr[SUM_NAME_PARS]={
	/*0 Клапан 1*/          {"Max pipe 1#Макс контур 1",     SSSpS},
	/*1 Клапан 2*/          {"Max pipe 2#Макс контур 2",     SSSpS},
	/*2 Клапан 3*/          {"Max pipe 3#Макс контур 3",     SSSpS},
	/*3 Клапан 4*/          {"Max pipe 4#Макс контур 4",     SSSpS},
	/*4 Клапан 5*/          {"Max pipe 5#Макс контур 5",     SSSpS},
	/*5 Клапан AHU*/        {"Max pipe AHU#Макс конт AHU",   SSSpS},
	/*6 Фрамуга СЕВЕР*/     {"Max vent UnW#Макс Фрам подв",  SSSS},
	/*7 Фрамуга ЮГ*/        {"Max vent OnW#Макс Фрам нав",   SSSS},
	/*8 Минимум контура 3*/ {"Min temp pipe 3#Мин контур 3", SSSpS},
	/*9 Минимум контура 4*/ {"Min temp pipe 4#Мин контур 4", SSSpS},
	/*10 Держать разн. давл*/{"Diff pressure#Разница давл",  SSSpS},
	/*11 Оптимум контура 3*/{"Optimal pipe3#Оптимум конт 3", SSSpS},
	/*12 Оптимум контура 4*/{"Optimal pipe4#Оптимум конт 4", SSSpS},
	/*13 И- поправка вент*/ {"Vent I-band#Фрамуги И-коэф",   SSpS0},
	/*14 П- поправка отопл*/{"Heat P-band#Тепло П-коэф",     SSpS0},
	/*15 И- поправка отопл*/{"Heat I-band#Тепло И-коэф",     SSpS0},
	/*16 П- поправка вент*/ {"Vent P-band#Фрамуги П-коэф",   SSpS0},
	/*17 Нет*/              {"Unused#Не используем",         SSSS},
	/*18 Экран*/            {"Max screen T#Макс экран T",    SSSS},
	/*19  Экран затеняющ*/  {"Max screen S#Макс экран З",    SSSS},
	/*20  CO2*/             {"CO2 type#Тин CO2",             SSSS},
	/*21  SIO*/             {"SIO#SIO",                      SSSS},
	/*22  Досветка*/        {"Light#Light",                  SSSS},
	/*23  T heat\vent*/     {"T heat vent #T heat vent",     SSSS},

};

const eNameConfig NameInputConfig[cConfSInputs]={
	/*0 Авария отопления*/          {"Heating alarm#Авария отоплен", tpLEVEL, 0, MAX_SUM_RELAY},
	/*1 Авария вентиляции*/         {"Vent alarm#Авария вентил",     tpLEVEL, 0, MAX_SUM_RELAY},
	/*2 Досветка 50%*/              {"Light 50%#50% досвет",         tpLEVEL, 0, MAX_SUM_RELAY},
	/*3 Досветка 100%*/             {"Light 100%#100% досвет",       tpLEVEL, 0, MAX_SUM_RELAY},
	/*4 Осадки*/                    {"Rain#Осадки",                  tpLEVEL, 0, MAX_SUM_RELAY},
	/*5 Авария СО2*/                {"CO2 alarm#Авария CO2",         tpLEVEL, 0, MAX_SUM_RELAY},
	/*6 Авария света*/              {"Light alarm#Авария досвет",    tpLEVEL, 0, MAX_SUM_RELAY},
};

const eNameConfig NameOutputConfig[cConfSOutput]={
	/*0 Клапан 1*/          {"Mixing valve 1#Смес клапан 1", tpRELAY, 0, MAX_SUM_RELAY},
	/*1 Клапан 2*/          {"Mixing valve 2#Смес клапан 2", tpRELAY, 0, MAX_SUM_RELAY},
	/*2 Клапан 3*/          {"Mixing valve 3#Смес клапан 3", tpRELAY, 0, MAX_SUM_RELAY},
	/*3 Клапан 4*/          {"Mixing valve 4#Смес клапан 4", tpRELAY, 0, MAX_SUM_RELAY},
	/*4 Клапан 5*/          {"Mixing valve 5#Смес клапан 5", tpRELAY, 0, MAX_SUM_RELAY},
	/*5 Клапан AHU*/        {"Mix valve AHU#См клапан AHU",  tpRELAY, 0, MAX_SUM_RELAY},
	/*6 Фрамуга СЕВЕР*/     {"Window NORTH#Фрамуга СЕВЕР",   tpRELAY, 0, MAX_SUM_RELAY},
	/*7 Фрамуга ЮГ*/        {"Window SOUTH#Фрамуга ЮГ",      tpRELAY, 0, MAX_SUM_RELAY},
	/*8 Клапан UC*/         {"UC valve#UC клапан",           tpRELAY, 0, MAX_SUM_RELAY},
	/*9 Скорость AHU*/      {"AHU speed 1#Скорость AHU 1",   tpRELAY, 0, MAX_SUM_RELAY},
	/*10 Экран*/            {"Screen thermal#Экран термич",  tpRELAY, 0, MAX_SUM_RELAY},
	/*11  Экран затеняющ*/  {"Screen shade#Экран зетен",     tpRELAY, 0, MAX_SUM_RELAY},
	/*12  Верт. экран 1*/   {"Screen vert 1#Экран верт 1",   tpRELAY, 0, MAX_SUM_RELAY},
	/*13 Верт. экран 2 */   {"Screen vert 2#Экран верт 2",   tpRELAY, 0, MAX_SUM_RELAY},
	/*14 Верт. экран 3 */   {"Screen vert 3#Экран верт 3",   tpRELAY, 0, MAX_SUM_RELAY},
	/*15 Верт. экран 4*/    {"Screen vert 4#Экран верт 4",   tpRELAY, 0, MAX_SUM_RELAY},
	/*16 СО2 дозатор*/      {"CO2 valve#CO2 клапан",         tpRELAY, 0, MAX_SUM_RELAY},
	/*17 Регулятор давл*/   {"Pressure reg#Регулятор давл",  tpRELAY, 0, MAX_SUM_RELAY},
	/*18 Досветка */        {"Lighting#Досветка",            tpRELAY, 0, MAX_SUM_RELAY},
	/*19 Скорость AHU*/     {"AHU speed 2#Скорость AHU 2",   tpRELAY, 0, MAX_SUM_RELAY},
	/*20 Резерв*/           {"Rezerv#Не использ",            tpRELAY, 0, MAX_SUM_RELAY},
	/*21 Резерв*/           {"Rezerv#Не использ",            tpRELAY, 0, MAX_SUM_RELAY},
	/*22 Резерв*/           {"Rezerv#Не использ",            tpRELAY, 0, MAX_SUM_RELAY},
	/*23 Резерв*/           {"Rezerv#Не использ",            tpRELAY, 0, MAX_SUM_RELAY},

	/*24 Насос 1*/          {"Heat pump 1#Насос 1",          tpRELAY, 0, MAX_SUM_RELAY},
	/*25 Насос 2*/          {"Heat pump 2#Насос 2",          tpRELAY, 0, MAX_SUM_RELAY},
	/*26 Насос 3*/          {"Heat pump 3#Насос 3",          tpRELAY, 0, MAX_SUM_RELAY},
	/*27 Насос 4*/          {"Heat pump 4#Насос 4",          tpRELAY, 0, MAX_SUM_RELAY},
	/*28 Насос 5*/          {"Heat pump 5#Насос 5",          tpRELAY, 0, MAX_SUM_RELAY},
	/*29 Насос 5*/          {"Pump AHU#Насос AHU",           tpRELAY, 0, MAX_SUM_RELAY},
	/*30 Вентиляторы*/      {"Fans#Вентиляторы",             tpRELAY, 0, MAX_SUM_RELAY},
	/*31 Нагреватель*/      {"Heater#Нагреватель",           tpRELAY, 0, MAX_SUM_RELAY},
	/*32 Охладитель*/       {"Cooler#Охладитель",            tpRELAY, 0, MAX_SUM_RELAY},
	/*33 Насос СИО*/        {"Misting pump#Насос СИО",       tpRELAY, 0, MAX_SUM_RELAY},
	/*34 Клапан СИО*/       {"Mist valve start#Клапаны СИО", tpRELAY, 0, MAX_SUM_RELAY},
	/*35 Клапан AHU*/       {"Mist AHU valves#Клапаны AHU",  tpRELAY, 0, MAX_SUM_RELAY},

	/*36 Авария*/           {"Alarm#Авария",                 tpRELAY, 0, MAX_SUM_RELAY},

	/*37 Регулятор 1*/      {"Regulator 1#Регулятор 1",      tpRELAY, 0, MAX_SUM_RELAY},
	/*38 Регулятор 2*/      {"Regulator 2#Регулятор 2",      tpRELAY, 0, MAX_SUM_RELAY},
	/*39 Регулятор 3*/      {"Regulator 3#Регулятор 3",      tpRELAY, 0, MAX_SUM_RELAY},
	/*40 Регулятор 4*/      {"Regulator 4#Регулятор 4",      tpRELAY, 0, MAX_SUM_RELAY},
	/*41 Регулятор 5 */     {"Regulator 5#Регулятор 5",      tpRELAY, 0, MAX_SUM_RELAY},

	/*42 Регулятор 1*/      {"Rez 1#Рез 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*43 Регулятор 1*/      {"Rez 1#Рез 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*44 Регулятор 1*/      {"Rez 1#Рез 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*45 Регулятор 1*/      {"Rez 1#Рез 1",                  tpRELAY, 0, MAX_SUM_RELAY},
};
