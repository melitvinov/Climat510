
// XXX: for eAdrGD
#include "simple_server.h"




//uchar   Y_menu,Y_menu2,x_menu;
uint16_t Y_menu,Y_menu2,x_menu;
uchar   Ad_Buf=0;
uchar   AdinB=0;
uchar   Form=0;
uchar   Mark=0;


bool     EndInput;
bool     BlkW;


long        LngX,LngY;

int16_t     IntX,IntY,IntZ;

int8_t      bWaterReset[16];
//uchar   	ByteW,ByteY,ByteX,ByteZ;
int16_t     ByteW,ByteY,ByteX,ByteZ;
uchar       nReset;

uchar       not=230,ton=3,ton_t=15;

uchar       bNight;

const uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};


