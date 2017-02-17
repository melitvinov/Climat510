
// XXX: for eAdrGD
//#include "simple_server.h"


/* Ќазначение битов регистра новостей - GD.SostPol.News */
#define SumTeplZones	GD.Control.ConfSTepl

#define bOperator       0x01
#define bEdit           0x02
#define bKlTest         0x04
#define bReset          0x20
#define bResRam         0x08
//#define bResEEP0        0x10
//#define bResEEP1        0x20
//#define bResEEP2        0x40
//#define bWriEEP         0x80
// дл€ климата
//#define bReseting       0x04
#define bInClock		0x80

uint16_t    PORTNUM;
//uchar   Y_menu,Y_menu2,x_menu;
uint16_t Y_menu,Y_menu2,x_menu;
uchar   Ad_Buf=0;
uchar   AdinB=0;
uchar   xdata Form=0;
uchar   Mark=0;

#warning !!!!!!!!!!!!!!!!!!!!!!!!!! ON
//uchar   SIM;



//uchar   buf[90];
void    *AdrVal;
uchar   TecChan;
bit     ReadyIZ;
bit     FalseIZ;
bit     bSec;
//bit     BITKL;
bit     Menu;
bit     B_video;
bit     B_input;
bit     EndInput;
bit     BlkW;
unsigned char   Second;

#warning air heat working time
static int16_t  airHeatPause[8];
static int16_t  airHeatTimeWork[8];
static int16_t  airHeatOnOff[8];


long        LngX,LngY;

int16_t     IntX,IntY,IntZ;
int16_t     SaveInt;

int8_t      SaveChar;
int8_t      bWaterReset[16];
//uchar   	ByteW,ByteY,ByteX,ByteZ;
int16_t     ByteW,ByteY,ByteX,ByteZ;
uchar       nReset;
uchar       NumBlock;

uchar       not=230,ton=3,ton_t=15;

uchar       bNight;
eAdrGD      AdrGD[15];

const uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};

