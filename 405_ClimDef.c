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


uchar   Y_menu,Y_menu2,x_menu;
uchar   Ad_Buf=0;
uchar   AdinB=0;
uchar   xdata Form=0;
uchar   TecChan;
bit     ReadyIZ;
bit     FalseIZ;
bit     bSec;
bit     Menu;
bit     B_video;

bit     EndInput;
bit     BlkW;
unsigned char   Second;

int16_t    	MesINT0;
long 		LngX,LngY;
uchar   	NDat;
uchar   	NumIzm;
int16_t    	IntX,IntY,IntZ;
int16_t    	SaveInt;
int16_t		SaveInt2;
int8_t    	SaveChar;
int8_t		nPortSave;

int8_t		bWaterReset[16];
int8_t   	ByteW,ByteY,ByteX,ByteZ;
uchar   	nReset;
uchar   	NumBlock;

uchar   	not=230,ton=3,ton_t=15;

uchar   	bNight;

uchar   	RegLEV;
eAdrGD     	AdrGD[15];
code uchar   Mon[]={31,28,31,30,31,30,31,31,30,31,30,31};
uchar		SendByte1W;
uchar		Buf1W[10];
uchar*		pBuf;
int8_t		ToHiTime;
int8_t		ToLowTime;

