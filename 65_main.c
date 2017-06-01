/*============================================
        ---Проект 324 с 17.11.2003
        --- Проект F319 ---
        Файл "k_main.c"
        Главный цикл программы
----------------------------------------------*/
#include "keyboard.h"

//++++++FullCheck ++++
int  DestSize;
int  DestAdr;
//char xdata	DestBuffer[7000];
//char* DestAdrBuf;
//char	BufCheckByte;

/*char PlaceBuf(void) {
int  i;
char xdata *p1;
char xdata *p2;
if (BufCheckByte != 55) return 0; //;неудачный прием
p1=(char xdata *)DestAdr;
p2=(char xdata *)&DestBuffer[0];
for (i=0;i < (DestSize-1);i++) {
    *(p1++)=*(p2++);
    }
return 1;
}
*/

volatile char crc;
volatile char crc1;
volatile char size;
volatile char numB;

char CheckSumMain(void)
{
  char Bl;
  switch (numB)
  {
	  case 2:
	  {
		  Bl = 0;
		  break;
	  }
	  case 4:
	  {
		  Bl = 1;
		  break;
	  }
	  case 6:
	  {
		  Bl = 2;
		  break;
	  }
  }

  int i;
  char res = 0;
  volatile int8_t r =0;
  for (i=0; i<cSHandCtrl; i++)
  {
	  r = GD.Hot.Tepl[Bl].HandCtrl[i].RCS;
	  res = res + r;
	  r = GD.Hot.Tepl[Bl].HandCtrl[i].Position;
	  res = res + r;
  }
  return res;
}

//;------FullCheck------------------
/*char volatile konturMax[6];
char volatile mecPosArray[7];

void saveMech(char tCTepl)
{
	mecPosArray[0] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].Position;
	mecPosArray[1] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].Position;
	mecPosArray[2] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].Position;
	mecPosArray[3] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].Position;
	mecPosArray[4] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].Position;
	mecPosArray[5] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].Position;
	mecPosArray[6] = GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].Position;
}

void loadKontur(char tCTepl)
{
	GD.Control.Tepl[tCTepl].c_MaxTPipe[0] = konturMax[0]*10;
	GD.Control.Tepl[tCTepl].c_MaxTPipe[1] = konturMax[1]*10;
	GD.Control.Tepl[tCTepl].c_MaxTPipe[2] = konturMax[2]*10;
	GD.Control.Tepl[tCTepl].c_MaxTPipe[3] = konturMax[3]*10;
	GD.Control.Tepl[tCTepl].c_MaxTPipe[4] = konturMax[4]*10;
	GD.Control.Tepl[tCTepl].c_MaxTPipe[5] = konturMax[5]*10;
}

void saveKontur(char tCTepl)
{
	konturMax[0] = GD.Control.Tepl[tCTepl].c_MaxTPipe[0]/10;
	konturMax[1] = GD.Control.Tepl[tCTepl].c_MaxTPipe[1]/10;
	konturMax[2] = GD.Control.Tepl[tCTepl].c_MaxTPipe[2]/10;
	konturMax[3] = GD.Control.Tepl[tCTepl].c_MaxTPipe[3]/10;
	konturMax[4] = GD.Control.Tepl[tCTepl].c_MaxTPipe[4]/10;
	konturMax[5] = GD.Control.Tepl[tCTepl].c_MaxTPipe[5]/10;
}

void loadMech(char tCTepl)
{
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].RCS = 1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].RCS = 1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].RCS =	1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].RCS =	1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].RCS =	1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].RCS =	1;
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS = 1;

	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].Position = mecPosArray[0];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].Position = mecPosArray[1];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].Position = mecPosArray[2];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].Position = mecPosArray[3];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].Position = mecPosArray[4];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].Position = mecPosArray[5];
	GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].Position =	mecPosArray[6];
}

char volatile repeatNews[8];

void initCheckConfig()
{
	int i;
	for (i=0;i<8;i++)
		repeatNews[i] = 0;
}

void checkConfig()
{
	char volatile tCTepl,sys;
	char volatile checkMech, checkKontur;
	for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
	{
		checkMech = 0;
		checkKontur = 0;
		for (sys=0;sys<6;sys++)
		{
		  if (GD.Control.Tepl[tCTepl].c_MaxTPipe[sys] > 1300)   // темп заданная в мониторе *10
		   	checkKontur = 1;
		}
		ClrDog;
		for (sys=0;sys<6;sys++)
		{
	        if (GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH+sys].RCS == 0)
	        	checkMech = 1;
		}
		ClrDog;
        if (GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS == 0)
        	checkMech = 1;

    	if (checkMech == 1)
    	{
    		GD.Hot.Tepl[tCTepl].newsZone = 0x0A;
    		loadMech(tCTepl);
    		repeatNews[tCTepl] = 4;
    	} else
    	{
    		saveMech(tCTepl);
    	}
    	ClrDog;
    	if (checkKontur == 1)
    	{
    		GD.Hot.Tepl[tCTepl].newsZone = 0x0F;
    		loadKontur(tCTepl);
    		repeatNews[tCTepl] = 4;
    	} else
    	{
    		saveKontur(tCTepl);
    	}
    	ClrDog;
    	if (repeatNews[tCTepl])
    		repeatNews[tCTepl]--;
    	if (repeatNews[tCTepl] <= 0)
    		GD.Hot.Tepl[tCTepl].newsZone = 0;
	}

}*/


main()
{
    char    timeDog;
    keyboardSetBITKL(0);
    //BITKL=0;

#ifndef STM32_UNIT
    init8051();
#else
    ClrAllOutIPCDigit();
    Init_STM32();
#endif

    ClrDog;
#ifdef SumRelay48
    //Reg48ToI2C();
    //OutRelay88();
#else
#ifdef SumRelay40
    OutRelay40();
#else
    OutRelay24();
#endif
#endif
    InitLCD();

    ClrDog;
#ifdef SumExtCG
    SendFirstScreen(1);
#endif
    clear_d();
    ClrDog;
    Menu=0;
    EndInput=0;
    nReset=3;
    w_txt("\252\245TO F405 (c)APL&DAL");
    Delay(1000000);
    ClrDog;
    Video();
    ClrDog;
    GD.Hot.News|=bKlTest;
    ByteX=1;
    GD.SostRS=OUT_UNIT;
    KeyboardProcess();
	if (keyboardGetBITKL())
        ByteX=6;
    ClrDog;
    TestMem(ByteX);
    Second=38;
    ClrDog;
    ClrDog;  /* разрешение прерываний RS и T0 из init8051()*/
    ClearAllAlarms();
    siodInit();
    airHeatInit();   // airHeat
    //initCheckConfig();
start:

    if (not) {
        if (!ton_t--) {
            ton_t=ton; not--; Sound;
        }
    }
    if (!not && nReset) {
        ton=(nReset--)+2;not=80;
    }

    if (!timeDog--) {
        timeDog=7;ClrDog;
    }

    if (GD.SostRS == (uchar)IN_UNIT) {  /*Если приняли блок с ПК */
        /*--Если запись 0бл и признак времени то установить время */
//            if(PlaceBuf()) {
#ifdef STM32_UNIT
        NMinPCOut=0;
#endif
        if (!NumBlock && (GD.Hot.News&0x80)) SetRTC();
        ClrDog;
        /*-- Была запись с ПК в блок NumBlock, переписать в EEPROM ------*/
#warning Изменение блока
        //убрать, тестовая вещь показывает прием пакета

       	//checkConfig();

    	if ( (NumBlock == 0) && (size == 92) )
    	{
    		crc1 = 55-CheckSumMain();
    		if (crc != crc1)
    			ReadFromFRAM();
    	}

    	if ( GD.Timer[0].crc != 0xAA )
    		ReadBlockFRAM(1);

    	if ( GD.Control.Tepl[0].crc != 0xAA )
    		ReadBlockFRAM(0);

        if (NumBlock)
        	ReWriteFRAM();
//				}
        GD.SostRS=OUT_UNIT;
        keyboardSetSIM(105);
    }
    if (bSec) {
#ifdef STM32_UNIT
        if (Second==58) {
            CheckWithoutPC();
            CheckInputConfig();
        }
        CheckRSTime();
#endif
#ifndef NOTESTMEM

        if (GD.SostRS==OUT_UNIT) TestMem(0);
#endif
        bSec=0;
        ClrDog;
        Control();
        ClrDog;
        B_video=1;
        if (!(Second%9))
            Measure();
    }
	if (keyboardGetBITKL()) {
        ClrDog;
        GD.Hot.News|=bOperator;
        if (Menu) GD.Hot.News|=bEdit;
        KeyBoard();
        B_video=1;
    }
    //CheckReadyMeasure();
    if (B_video) {
        ClrDog;
        GMenu();
        ClrDog;
        Video();
        ClrDog;
        B_video=0;
    }
    simple_servercycle(); //Перенесено в прерывание клавиатуры

    goto start;
}
