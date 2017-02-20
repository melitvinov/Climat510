/*============================================
        ---Проект 324 с 17.11.2003
        --- Проект F319 ---
        Файл "k_main.c"
        Главный цикл программы
----------------------------------------------*/
#include "keyboard.h"

int  DestSize;
int  DestAdr;

char volatile konturMax[6];
char volatile mecPosArray[7];

bool     B_video;

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
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].RCS = 1;
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].RCS = 1;
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].RCS = 1;
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].RCS = 1;
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS = 1;

    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].Position = mecPosArray[0];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].Position = mecPosArray[1];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].Position = mecPosArray[2];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].Position = mecPosArray[3];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].Position = mecPosArray[4];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].Position = mecPosArray[5];
    GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].Position =  mecPosArray[6];
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
        for (sys=0;sys<6;sys++)
        {
            if (GD.Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH+sys].RCS == 0)
                checkMech = 1;
        }
        if (GD.Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS == 0)
            checkMech = 1;

        if (checkMech == 1)
        {
            GD.Hot.Tepl[tCTepl].newsZone = 0x0A;
            loadMech(tCTepl);
            repeatNews[tCTepl] = 4;
        }
        else
        {
            saveMech(tCTepl);
        }
        if (checkKontur == 1)
        {
            GD.Hot.Tepl[tCTepl].newsZone = 0x0F;
            loadKontur(tCTepl);
            repeatNews[tCTepl] = 4;
        }
        else
        {
            saveKontur(tCTepl);
        }
        if (repeatNews[tCTepl])
            repeatNews[tCTepl]--;
        if (repeatNews[tCTepl] <= 0)
            GD.Hot.Tepl[tCTepl].newsZone = 0;
    }

}

#define Sound   GPIOA->ODR^=GPIO_Pin_4;

void main(void)
{
    char    timeDog;
    keyboardSetBITKL(0);
    //BITKL=0;

    ClrAllOutIPCDigit();
    Init_STM32();

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

#ifdef SumExtCG
    SendFirstScreen(1);
#endif
    clear_d();
    WTF0.Menu=0;
    EndInput=0;
    nReset=3;
    w_txt("\252\245TO F405 (c)APL&DAL");
    Delay(1000000);
    Video();
    GD.Hot.News|=bKlTest;
    ByteX=1;
    GD.SostRS=OUT_UNIT;
    KeyboardProcess();
    if (keyboardGetBITKL())
        ByteX=6;
    TestMem(ByteX);
    WTF0.Second=38;
    ClearAllAlarms();
    siodInit();
    airHeatInit();   // airHeat
    initCheckConfig();
    start:

    if (not)
    {
        if (!ton_t--)
        {
            ton_t=ton; not--; Sound;
        }
    }
    if (!not && nReset)
    {
        ton=(nReset--)+2;not=80;
    }

    if (!timeDog--)
    {
        timeDog=7;
    }

    if (GD.SostRS == (uchar)IN_UNIT)  /*Если приняли блок с ПК */
    {
        /*--Если запись 0бл и признак времени то установить время */
//            if(PlaceBuf()) {

        // XXX: isolation
        stm32f10x_Rootines_reset_NMinPCOut();

        if (!WTF0.NumBlock && (GD.Hot.News&0x80)) SetRTC();
        /*-- Была запись с ПК в блок NumBlock, переписать в EEPROM ------*/
#warning Изменение блока
        //убрать, тестовая вещь показывает прием пакета

        checkConfig();

        if (WTF0.NumBlock)
            ReWriteFRAM(WTF0.NumBlock);
//				}
        GD.SostRS=OUT_UNIT;
        keyboardSetSIM(105);
    }
    if (WTF0.bSec)
    {
        if (WTF0.Second==58)
        {
            CheckWithoutPC();
            CheckInputConfig();
        }
        CheckRSTime();
#ifndef NOTESTMEM

        if (GD.SostRS==OUT_UNIT) TestMem(0);
#endif
        WTF0.bSec=0;
        Control();
        B_video=1;
        if (!(WTF0.Second%9))
            Measure();
    }
    if (keyboardGetBITKL())
    {
        GD.Hot.News|=bOperator;
        if (WTF0.Menu) GD.Hot.News|=bEdit;
        KeyBoard();
        B_video=1;
    }
    //CheckReadyMeasure();
    if (B_video)
    {
        GMenu();
        Video();
        B_video=0;
    }
    simple_servercycle(); //Перенесено в прерывание клавиатуры

    goto start;
}
