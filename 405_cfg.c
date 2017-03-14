

#include "syntax.h"

//#include "control_const.h"
#include "405_ConfigEnRuSR.h"

const eNameASens NameSensConfig[cConfSSens+cConfSMetSens]={
    /*
      � - ����� �����
      � - ����� �����
      ��� - ����������� ���������� ��������
      ���� - ������������ ���������� ��������
      ��1 - ���������� ������1
      ��2 - ���������� ������2
      ��1 - �������� ������1
      ��2 - �������� ������2
      ���� - ����������� ����������
      ���� - ������������ ����������
      � - ��� ���������� �������*/
/*�������� �������               ������	��.���	��� ����  �  ��� ��� ���� 		��1		��2	 	��1  	��2  	����	���� 	���������� 		�������� ������*/
//0
	{"Temp air1#���� ������� 1", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* ����������� ������� 1*/
	{"Temp air2#���� ������� 2", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* ����������� �������� 2*/
	{"Temp air3#���� ������� 3", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* ����������� �������� 3*/
	{"Temp air4#���� ������� 4", SSpS0, cuT,   cTypeAnal, 3,  0, 0,   9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,   50},   /* ����������� �������� 4*/
	{"Humidity 1#��������� 1",   SSpS0, cuPr,  cTypeRH,   0,  0, 200, 9800, {878,  3097}, {0,    7530}, 800,  5000, cExpMidlSens, 1000}, /* ��������� ������� 1*/
	{"Humidity 2#��������� 2",   SSpS0, cuPr,  cTypeRH,   0,  0, 200, 9800, {878,  3097}, {0,    7530}, 800,  5000, cExpMidlSens, 1000}, /* ��������� ������� 2*/
	{"Inside light#���� ����",   SSSS,  cuBt,  cTypeAnal, 11, 0, 0,   1500, {15,   2250}, {0,    1000}, 0,    5000, c3MidlSens,   100},  /* ���������� ����*/
	{"CO2#CO2 �������",          SSSS,  cuPpm, cTypeRH,   0,  0, 100, 2000, {0,    5000}, {0,    2000}, 50,   5000, c3MidlSens,   80},   /*������������ ��2*/
	 //8
//8
	{"Temp evap 1#���� ������� 1", SSpS0, cuT, cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* ����������� ���� ������� 1*/
	{"Temp evap 2#���� ������� 2", SSpS0, cuT, cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* ����������� ���� ������� 2*/

//10
	{"Temp cool #���� ��������",  SSpS0, cuT,  cTypeAnal, 3, 0, 1, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* ����������� ����������� ����*/
	{"Overpressure#����� ������", SSpS0, cuPa, cTypeAnal, 0, 0, 1, 9900, {0,    3000}, {0,    3000}, 0,    5000, c3MidlSens, 80}, /* ���������� ��������*/

//12
	{"Temp roof#���� ������",        SSpS0, cuT, cTypeAnal, 3, 0, 0,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* ����������� ������*/
	{"Temp soil#���� �����",         SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /* ����������� �����*/
	{"Temp glass#���� ������",       SSpS0, cuT, cTypeAnal, 3, 0, -2000, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*����������� ������*/
	{"Temp in AHU#���� ����� AHU",   SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*� ����� � AHU*/
	{"Temp out AHU#���� ������ AHU", SSpS0, cuT, cTypeAnal, 3, 0, 1,     9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens, 80}, /*� ������ �� AHU*/

//17
	{"N vent pos#����� ������� �", SSSpS, cuPr, cTypeFram,   1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*������������*/
	{"S vent pos#����� ������� �", SSSpS, cuPr, cTypeFram,   1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*������ �����*/
	{"Screen pos#����� �����",     SSSpS, cuPr, cTypeScreen, 1, 0, 0, 1000, {0, 5000}, {0, 1000}, 0, 5000, c3MidlSens, 400}, /*������������*/

//20
	{"Temp pipe1#���� ������� 1",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ���� �������1*/
	{"Temp pipe2#���� ������� 2",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ���� �������2*/
	{"Temp pipe3#���� ������� 3",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ���� �������3*/
	{"Temp pipe4#���� ������� 4",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ���� �������4*/
	{"Temp pipe5#���� ������� 5",   SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ���� �������5*/
	{"Temp pipe AHU#���� ���� AHU", SSSpS, cuT, cTypeAnal, 3, 0, 10, 1500, {2930, 3230}, {200, 500}, 2000, 5000, c3MidlSens, 100}, /*� ������� AHU*/

//26
	{"Temp out#���� ��������",      SSpS0, cuT,    cTypeAnal,  3,  0, -6000, 9900, {2930, 3230}, {2000, 5000}, 2000, 5000, c3MidlSens,  80},  /*����������� ��������*/
	{"FAR#������������� ������",    SSSS,  cuBt,   cTypeSun,   11, 0, 0,     1500, {15,   2250}, {0,    5000}, 0,    5000, c3MidlSens,  200}, /*��������� ��������*/
	{"Wind speed#�������� �����",   SSpS0, cuMSec, cTypeMeteo, 2,  0, 0,     9900, {60,   0},    {1,    0},    0,    0,    cNoMidlSens, 100}, /*�������� �����*/
	{"Wind direct#������� �����",   SSSi,  cuGr,   cTypeNULL,  2,  0, 0,     360,  {0,    4520}, {0,    360},  0,    0,    cNoMidlSens, 40},  /*����������� �����*/
	{"Rain#������",                 SSSi,  cu,     cTypeRain,  0,  0, 0,     10,   {250,  650},  {-10,  10},   0,    5000, cNoMidlSens, 20},  /*������*/
	{"Humidity out#���������",      SSpS0, cuPr,   cTypeAnal,  0,  0, 200,   9900, {800,  5000}, {0,    0},    0,    5000, c3MidlSens,  100}, /*��������� ��������*/
	{"T from boil#���� ������",     SSSpS, cuT,    cTypeAnal,  3,  0, 10,    1500, {2930, 3230}, {200,  500},  0,    0,    c3MidlSens,  100}, /*����������� ���� ������*/
	{"T to boil#���� �������",      SSSpS, cuT,    cTypeAnal,  3,  0, 10,    1500, {2930, 3230}, {200,  500},  0,    0,    c3MidlSens,  100}, /*����������� ���� ��������*/
	{"P from boil#�������� ������", SSpS0, cuPa,   cTypeAnal,  0,  0, 0,     1500, {500,  4500}, {0,    1464}, 0,    5000, c3MidlSens,  100}, /*�������� ���� ������*/
	{"P to boil#�������� �������",  SSpS0, cuPa,   cTypeAnal,  0,  0, 0,     1500, {500,  4500}, {0,    1464}, 0,    5000, c3MidlSens,  100}, /*�������� ���� ��������*/
	{"Quantity#������",             SSSpS, cuPa,   cTypeAnal,  0,  0, 0,     9000, {500,  4500}, {0,    105},  0,    5000, c3MidlSens,  100}, /*������*/
};


const eNameTimer NameTimer[SUM_NAME_TIMER]={
/*������	������	*/
	{"Zone number#����� ����",        oS,    0},
	{"Time start#����� ������",       SSdSS, 1},
	{"Type start#��� ������",         SS,    3},
	{"Heat temp#���� ���������",      SSpS0, 5},
	{"Vent temp#���� ����������",     SSpS0, 7},
	{"Humidity#���������",            SS,    29},
	{"CO2 level#������� CO2",         SSSS,  9},
	{"Light power#������� ��������",  SSS,   26},
	{"Light mode#����� ��������",     oS,    27},

	{"Min temp pipe1#��� ������ 1",   SS,    16},
	{"Min temp pipe2#��� ������ 2",   SS,    17},
	{"Min temp pipe3#��� ������ 3",   SS,    28},
	{"Min temp pipe5#��� ������ 5",   SS,    15},

	{"Optimal pipe1#������� ������1", SS,    11},
	{"Optimal pipe2#������� ������2", SS,    12},

	{"Temp pipe 3#���� ������� 3",    SS,    13},
	{"Temp pipe 4#���� ������� 4",    SS,    14},

	{"Vent mode#����� ������",        oS,    19},
	{"Min vent pos#��� ������",       SS,    18},

	{"Mist pulse#��������� ���",      SSSS,  20},

	{"T screen mode#����� ����� T",   oS,    22},
	{"S screen mode#����� ����� �",   oS,    23},
	{"V screen mode#����� ����� �",   oS,    24},
	{"Fans mode#����� ����������",    oS,    25},
};

// XXX: should be const ?
const eNameConfig NameSystemConfig[cConfSSystem]={
	/*0 ������� ���������*/             {"Heating system",     tpSUM, 0, MAX_SUM_RELAY},
	/*1 ������� ����������*/            {"Ventilation system", tpSUM, 0, MAX_SUM_RELAY},
	/*2 ������� ������������*/          {"Screening system",   tpSUM, 0, MAX_SUM_RELAY},
	/*3 ������� �����������*/           {"UltraClima system",  tpSUM, 0, MAX_SUM_RELAY},
	/*4 ������� ��2*/                   {"CO2 system",         tpSUM, 0, MAX_SUM_RELAY},
	/*5 ������� ������������*/          {"Lighting system",    tpSUM, 0, MAX_SUM_RELAY},
	/*6 ������� ����*/                  {"Misting system",     tpSUM, 0, MAX_SUM_RELAY},
	/*7 ������� ������������*/          {"Fans&Heaters",       tpSUM, 0, MAX_SUM_RELAY},
	/*8 ������1*/                       {"Rezerved",           tpSUM, 0, MAX_SUM_RELAY},
	/*9 ������2*/                       {"Rezerved",           tpSUM, 0, MAX_SUM_RELAY},

};

const eNameParUpr NameParUpr[SUM_NAME_PARS]={
	/*0 ������ 1*/          {"Max pipe 1#���� ������ 1",     SSSpS},
	/*1 ������ 2*/          {"Max pipe 2#���� ������ 2",     SSSpS},
	/*2 ������ 3*/          {"Max pipe 3#���� ������ 3",     SSSpS},
	/*3 ������ 4*/          {"Max pipe 4#���� ������ 4",     SSSpS},
	/*4 ������ 5*/          {"Max pipe 5#���� ������ 5",     SSSpS},
	/*5 ������ AHU*/        {"Max pipe AHU#���� ���� AHU",   SSSpS},
	/*6 ������� �����*/     {"Max vent UnW#���� ���� ����",  SSSS},
	/*7 ������� ��*/        {"Max vent OnW#���� ���� ���",   SSSS},
	/*8 ������� ������� 3*/ {"Min temp pipe 3#��� ������ 3", SSSpS},
	/*9 ������� ������� 4*/ {"Min temp pipe 4#��� ������ 4", SSSpS},
	/*10 ������� ����. ����*/{"Diff pressure#������� ����",  SSSpS},
	/*11 ������� ������� 3*/{"Optimal pipe3#������� ���� 3", SSSpS},
	/*12 ������� ������� 4*/{"Optimal pipe4#������� ���� 4", SSSpS},
	/*13 �- �������� ����*/ {"Vent I-band#������� �-����",   SSpS0},
	/*14 �- �������� �����*/{"Heat P-band#����� �-����",     SSpS0},
	/*15 �- �������� �����*/{"Heat I-band#����� �-����",     SSpS0},
	/*16 �- �������� ����*/ {"Vent P-band#������� �-����",   SSpS0},
	/*17 ���*/              {"Unused#�� ����������",         SSSS},
	/*18 �����*/            {"Max screen T#���� ����� T",    SSSS},
	/*19  ����� ��������*/  {"Max screen S#���� ����� �",    SSSS},
	/*20  CO2*/             {"CO2 type#��� CO2",             SSSS},
	/*21  SIO*/             {"SIO#SIO",                      SSSS},
	/*22  ��������*/        {"Light#Light",                  SSSS},
	/*23  T heat\vent*/     {"T heat vent #T heat vent",     SSSS},

};

const eNameConfig NameInputConfig[cConfSInputs]={
	/*0 ������ ���������*/          {"Heating alarm#������ �������", tpLEVEL, 0, MAX_SUM_RELAY},
	/*1 ������ ����������*/         {"Vent alarm#������ ������",     tpLEVEL, 0, MAX_SUM_RELAY},
	/*2 �������� 50%*/              {"Light 50%#50% ������",         tpLEVEL, 0, MAX_SUM_RELAY},
	/*3 �������� 100%*/             {"Light 100%#100% ������",       tpLEVEL, 0, MAX_SUM_RELAY},
	/*4 ������*/                    {"Rain#������",                  tpLEVEL, 0, MAX_SUM_RELAY},
	/*5 ������ ��2*/                {"CO2 alarm#������ CO2",         tpLEVEL, 0, MAX_SUM_RELAY},
	/*6 ������ �����*/              {"Light alarm#������ ������",    tpLEVEL, 0, MAX_SUM_RELAY},
};

const eNameConfig NameOutputConfig[cConfSOutput]={
	/*0 ������ 1*/          {"Mixing valve 1#���� ������ 1", tpRELAY, 0, MAX_SUM_RELAY},
	/*1 ������ 2*/          {"Mixing valve 2#���� ������ 2", tpRELAY, 0, MAX_SUM_RELAY},
	/*2 ������ 3*/          {"Mixing valve 3#���� ������ 3", tpRELAY, 0, MAX_SUM_RELAY},
	/*3 ������ 4*/          {"Mixing valve 4#���� ������ 4", tpRELAY, 0, MAX_SUM_RELAY},
	/*4 ������ 5*/          {"Mixing valve 5#���� ������ 5", tpRELAY, 0, MAX_SUM_RELAY},
	/*5 ������ AHU*/        {"Mix valve AHU#�� ������ AHU",  tpRELAY, 0, MAX_SUM_RELAY},
	/*6 ������� �����*/     {"Window NORTH#������� �����",   tpRELAY, 0, MAX_SUM_RELAY},
	/*7 ������� ��*/        {"Window SOUTH#������� ��",      tpRELAY, 0, MAX_SUM_RELAY},
	/*8 ������ UC*/         {"UC valve#UC ������",           tpRELAY, 0, MAX_SUM_RELAY},
	/*9 �������� AHU*/      {"AHU speed 1#�������� AHU 1",   tpRELAY, 0, MAX_SUM_RELAY},
	/*10 �����*/            {"Screen thermal#����� ������",  tpRELAY, 0, MAX_SUM_RELAY},
	/*11  ����� ��������*/  {"Screen shade#����� �����",     tpRELAY, 0, MAX_SUM_RELAY},
	/*12  ����. ����� 1*/   {"Screen vert 1#����� ���� 1",   tpRELAY, 0, MAX_SUM_RELAY},
	/*13 ����. ����� 2 */   {"Screen vert 2#����� ���� 2",   tpRELAY, 0, MAX_SUM_RELAY},
	/*14 ����. ����� 3 */   {"Screen vert 3#����� ���� 3",   tpRELAY, 0, MAX_SUM_RELAY},
	/*15 ����. ����� 4*/    {"Screen vert 4#����� ���� 4",   tpRELAY, 0, MAX_SUM_RELAY},
	/*16 ��2 �������*/      {"CO2 valve#CO2 ������",         tpRELAY, 0, MAX_SUM_RELAY},
	/*17 ��������� ����*/   {"Pressure reg#��������� ����",  tpRELAY, 0, MAX_SUM_RELAY},
	/*18 �������� */        {"Lighting#��������",            tpRELAY, 0, MAX_SUM_RELAY},
	/*19 �������� AHU*/     {"AHU speed 2#�������� AHU 2",   tpRELAY, 0, MAX_SUM_RELAY},
	/*20 ������*/           {"Rezerv#�� �������",            tpRELAY, 0, MAX_SUM_RELAY},
	/*21 ������*/           {"Rezerv#�� �������",            tpRELAY, 0, MAX_SUM_RELAY},
	/*22 ������*/           {"Rezerv#�� �������",            tpRELAY, 0, MAX_SUM_RELAY},
	/*23 ������*/           {"Rezerv#�� �������",            tpRELAY, 0, MAX_SUM_RELAY},

	/*24 ����� 1*/          {"Heat pump 1#����� 1",          tpRELAY, 0, MAX_SUM_RELAY},
	/*25 ����� 2*/          {"Heat pump 2#����� 2",          tpRELAY, 0, MAX_SUM_RELAY},
	/*26 ����� 3*/          {"Heat pump 3#����� 3",          tpRELAY, 0, MAX_SUM_RELAY},
	/*27 ����� 4*/          {"Heat pump 4#����� 4",          tpRELAY, 0, MAX_SUM_RELAY},
	/*28 ����� 5*/          {"Heat pump 5#����� 5",          tpRELAY, 0, MAX_SUM_RELAY},
	/*29 ����� 5*/          {"Pump AHU#����� AHU",           tpRELAY, 0, MAX_SUM_RELAY},
	/*30 �����������*/      {"Fans#�����������",             tpRELAY, 0, MAX_SUM_RELAY},
	/*31 �����������*/      {"Heater#�����������",           tpRELAY, 0, MAX_SUM_RELAY},
	/*32 ����������*/       {"Cooler#����������",            tpRELAY, 0, MAX_SUM_RELAY},
	/*33 ����� ���*/        {"Misting pump#����� ���",       tpRELAY, 0, MAX_SUM_RELAY},
	/*34 ������ ���*/       {"Mist valve start#������� ���", tpRELAY, 0, MAX_SUM_RELAY},
	/*35 ������ AHU*/       {"Mist AHU valves#������� AHU",  tpRELAY, 0, MAX_SUM_RELAY},

	/*36 ������*/           {"Alarm#������",                 tpRELAY, 0, MAX_SUM_RELAY},

	/*37 ��������� 1*/      {"Regulator 1#��������� 1",      tpRELAY, 0, MAX_SUM_RELAY},
	/*38 ��������� 2*/      {"Regulator 2#��������� 2",      tpRELAY, 0, MAX_SUM_RELAY},
	/*39 ��������� 3*/      {"Regulator 3#��������� 3",      tpRELAY, 0, MAX_SUM_RELAY},
	/*40 ��������� 4*/      {"Regulator 4#��������� 4",      tpRELAY, 0, MAX_SUM_RELAY},
	/*41 ��������� 5 */     {"Regulator 5#��������� 5",      tpRELAY, 0, MAX_SUM_RELAY},

	/*42 ��������� 1*/      {"Rez 1#��� 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*43 ��������� 1*/      {"Rez 1#��� 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*44 ��������� 1*/      {"Rez 1#��� 1",                  tpRELAY, 0, MAX_SUM_RELAY},
	/*45 ��������� 1*/      {"Rez 1#��� 1",                  tpRELAY, 0, MAX_SUM_RELAY},
};
