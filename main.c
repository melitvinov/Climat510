
#include "syntax.h"

#include "keyboard.h"

#include "control_gd.h"
#include "unsorted.h"

#include "wtf.h"

static char volatile konturMax[6];
static char volatile mecPosArray[7];

static uchar not=230;
static uchar ton=3;
static uchar ton_t=15;

uchar nReset=25;

void saveMech(char tCTepl)
{
    mecPosArray[0] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].Position;
    mecPosArray[1] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].Position;
    mecPosArray[2] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].Position;
    mecPosArray[3] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].Position;
    mecPosArray[4] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].Position;
    mecPosArray[5] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].Position;
    mecPosArray[6] = gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmLight].Position;
}

void loadKontur(char tCTepl)
{
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[0] = konturMax[0]*10;
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[1] = konturMax[1]*10;
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[2] = konturMax[2]*10;
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[3] = konturMax[3]*10;
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[4] = konturMax[4]*10;
    gd_rw()->Control.Tepl[tCTepl].c_MaxTPipe[5] = konturMax[5]*10;
}

void saveKontur(char tCTepl)
{
    konturMax[0] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[0]/10;
    konturMax[1] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[1]/10;
    konturMax[2] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[2]/10;
    konturMax[3] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[3]/10;
    konturMax[4] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[4]/10;
    konturMax[5] = gd()->Control.Tepl[tCTepl].c_MaxTPipe[5]/10;
}

void loadMech(char tCTepl)
{
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].RCS = 1;
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS = 1;

    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH].Position = mecPosArray[0];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrSH].Position = mecPosArray[1];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S1].Position = mecPosArray[2];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S2].Position = mecPosArray[3];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S3].Position = mecPosArray[4];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrV_S4].Position = mecPosArray[5];
    gd_rw()->Hot.Tepl[tCTepl].HandCtrl[cHSmLight].Position =  mecPosArray[6];
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
            if (gd()->Control.Tepl[tCTepl].c_MaxTPipe[sys] > 1300)   // темп заданная в мониторе *10
                checkKontur = 1;
        }
        for (sys=0;sys<6;sys++)
        {
            if (gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmScrTH+sys].RCS == 0)
                checkMech = 1;
        }
        if (gd()->Hot.Tepl[tCTepl].HandCtrl[cHSmLight].RCS == 0)
            checkMech = 1;

        if (checkMech == 1)
        {
            gd_rw()->Hot.Tepl[tCTepl].newsZone = 0x0A;
            loadMech(tCTepl);
            repeatNews[tCTepl] = 4;
        }
        else
        {
            saveMech(tCTepl);
        }
        if (checkKontur == 1)
        {
            gd_rw()->Hot.Tepl[tCTepl].newsZone = 0x0F;
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
            gd_rw()->Hot.Tepl[tCTepl].newsZone = 0;
    }

}

static void do_sound_stuff(void)
{
    if (not)
    {
        if (!ton_t--)
        {
            ton_t=ton;
            not--;
            GPIOA->ODR^=GPIO_Pin_4;
        }
    }
    if (!not && nReset)
    {
        ton=(nReset--)+2;not=80;
    }
}

void DoBeepy(int not_, int ton_)
{
    not = not_;
    ton = ton_;
}

static void periodic_task(void)
{
    if (wtf0.Second == 58)
    {
        CheckWithoutPC();
        CheckInputConfig();
    }
    CheckRSTime();
#ifndef NOTESTMEM

    if (wtf0.SostRS == OUT_UNIT)
        TestMem(0);
#endif

    // so control is firing every second

    // XXX: moved here from control.c
    LoadDiscreteInputs();
    GetRTC(&gd_rw()->Hot.Time, &gd_rw()->Hot.Date, &gd_rw()->Hot.Year, &NowDayOfWeek);

    ClrAllOutIPCDigit();

    Control_pre();

    #warning "so these IPC fucks are transferred from interrupts. so lame"

    ResumeOutIPCDigit();


    // xxx: sweet as a fuck
    if (wtf0.Second == 20)
    {
        InitLCD();
    }

    Control_post(wtf0.Second, wtf0.SostRS == WORK_UNIT);

    if (wtf0.Second >= 60)
    {
        wtf0.Second = 0;

        WriteToFRAM();
#ifndef NOTESTMEM
        if ((!wtf0.Menu)&&(wtf0.SostRS == OUT_UNIT))
            TestMem(1);
#endif
        DoBeepy(220, 10);
    }
    else
    {
        if (!(wtf0.Second%9))
            Measure();
    }
}

static void init(void)
{
    keyboardSetBITKL(0);

    ClrAllOutIPCDigit();
    Init_STM32();

    InitLCD();

#ifdef SumExtCG
    SendFirstScreen(1);
#endif
    clear_d();
    wtf0.Menu=0;
    nReset=3;
    w_txt("\252\245TO F405 (c)APL&DAL");
    Delay(1000000);
    Video();
    gd_rw()->Hot.News |= bKlTest;

    int byte_x=1;
    wtf0.SostRS=OUT_UNIT;
    KeyboardProcess();
    if (keyboardGetBITKL())
        byte_x=6;
    TestMem(byte_x);
    wtf0.Second=38;
    ClearAllAlarms();
    siodInit();
    airHeatInit();   // airHeat
    initCheckConfig();
}

static void process_pc_input(void)
{
    if (wtf0.SostRS != (uchar)IN_UNIT)  /*Если приняли блок с ПК */
        return;

    // XXX: isolation
    stm32f10x_Rootines_reset_NMinPCOut();


    #warning "strange logic we got here"
    if (!wtf0.NumBlock && (gd()->Hot.News & bWriEEP))
        SetRTC();
    /*-- Была запись с ПК в блок NumBlock, переписать в EEPROM ------*/

    checkConfig();

    if (wtf0.NumBlock)
        ReWriteFRAM(wtf0.NumBlock);

    wtf0.SostRS=OUT_UNIT;
    // what ?
    keyboardSetSIM(105);
}

void main(void)
{
    init();

    while (1)
    {
        do_sound_stuff();
        process_pc_input();

        bool should_show_video = 0;

        if (wtf0.bSec)
        {
            wtf0.bSec=0;
            periodic_task();
            should_show_video = 1;
        }

        if (keyboardGetBITKL())
        {
            gd_rw()->Hot.News |= bOperator;
            if (wtf0.Menu)
                gd_rw()->Hot.News |= bEdit;
            KeyBoard();
            should_show_video = 1;
        }

        if (should_show_video)
        {
            GMenu();
            Video();
            should_show_video = 0;
        }

        simple_servercycle();
    }
}
