
#include "syntax.h"

#include "keyboard.h"

#include "control_gd.h"
#include "unsorted.h"

#include "wtf.h"

#include "debug.h"

#include "hal.h"
#include "sound.h"
#include "fieldbus.h"

static int16_t konturMax[6];
static int8_t mecPosArray[7];

uchar nReset=25;

void saveMech(char tCTepl)
{
    mecPosArray[0] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrTH].Position;
    mecPosArray[1] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrSH].Position;
    mecPosArray[2] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S1].Position;
    mecPosArray[3] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S2].Position;
    mecPosArray[4] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S3].Position;
    mecPosArray[5] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S4].Position;
    mecPosArray[6] = gd()->Hot.Zones[tCTepl].HandCtrl[cHSmLight].Position;
}

void loadMech(char tCTepl)
{
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrTH].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrSH].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S1].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S2].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S3].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S4].RCS = 1;
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmLight].RCS = 1;

    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrTH].Position = mecPosArray[0];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrSH].Position = mecPosArray[1];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S1].Position = mecPosArray[2];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S2].Position = mecPosArray[3];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S3].Position = mecPosArray[4];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmScrV_S4].Position = mecPosArray[5];
    gd_rw()->Hot.Zones[tCTepl].HandCtrl[cHSmLight].Position =  mecPosArray[6];
}

void loadKontur(char tCTepl)
{
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[0] = konturMax[0];
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[1] = konturMax[1];
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[2] = konturMax[2];
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[3] = konturMax[3];
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[4] = konturMax[4];
    gd_rw()->Control.Zones[tCTepl].c_MaxTPipe[5] = konturMax[5];
}

void saveKontur(char tCTepl)
{
    konturMax[0] = gd()->Control.Zones[tCTepl].c_MaxTPipe[0];
    konturMax[1] = gd()->Control.Zones[tCTepl].c_MaxTPipe[1];
    konturMax[2] = gd()->Control.Zones[tCTepl].c_MaxTPipe[2];
    konturMax[3] = gd()->Control.Zones[tCTepl].c_MaxTPipe[3];
    konturMax[4] = gd()->Control.Zones[tCTepl].c_MaxTPipe[4];
    konturMax[5] = gd()->Control.Zones[tCTepl].c_MaxTPipe[5];
}


static char repeatNews[8];

void initCheckConfig()
{
    int i;
    for (i=0;i<8;i++)
        repeatNews[i] = 0;
}

void checkConfig(void)
{
    for (int zone_idx=0; zone_idx<NZONES; zone_idx++)
    {
        bool checkKontur = 0;
        bool checkMech = 0;
        for (int sys=0;sys<6;sys++)
        {
            if (gd()->Control.Zones[zone_idx].c_MaxTPipe[sys] > 1300)   // темп заданная в мониторе *10
                checkKontur = 1;
        }
        for (int sys=0;sys<6;sys++)
        {
            if (gd()->Hot.Zones[zone_idx].HandCtrl[cHSmScrTH+sys].RCS == 0)
                checkMech = 1;
        }
        if (gd()->Hot.Zones[zone_idx].HandCtrl[cHSmLight].RCS == 0)
            checkMech = 1;

        if (checkMech == 1)
        {
            gd_rw()->Hot.Zones[zone_idx].newsZone = 0x0A;
            loadMech(zone_idx);
            repeatNews[zone_idx] = 4;
        }
        else
        {
            saveMech(zone_idx);
        }
        if (checkKontur == 1)
        {
            gd_rw()->Hot.Zones[zone_idx].newsZone = 0x0F;
            loadKontur(zone_idx);
            repeatNews[zone_idx] = 4;
        }
        else
        {
            saveKontur(zone_idx);
        }
        if (repeatNews[zone_idx])
            repeatNews[zone_idx]--;
        if (repeatNews[zone_idx] <= 0)
            gd_rw()->Hot.Zones[zone_idx].newsZone = 0;
    }

}


void DoBeepy(void)
{
    static const hal_sound_note_t simple_beep[] = {{1000, 1}, {0, 0}};
    //static const hal_sound_note_t simple_beep[] = {{1000, 1}, {0, 1}, {2000, 2}, {0, 2}, {3000, 3}, {0, 3}, {4000, 4}, {0, 0},};
    sound_play(simple_beep);
}

static void periodic_task(void)
{
    if (wtf0.Second == 58)
    {
        LOG("checking input config");
        CheckWithoutPC();
        CheckInputConfig();
    }
    //CheckRSTime();
#ifndef NOTESTMEM

    if (wtf0.SostRS == OUT_UNIT)
        TestMem(0);
#endif

    // so control is firing every second

    //LOG("running periodic control");

    // XXX: moved here from control.c
    LoadDiscreteInputs();
    GetRTC(&gd_rw()->Hot.Time, &gd_rw()->Hot.Date, &gd_rw()->Hot.Year, &NowDayOfWeek);

    ClrAllOutIPCDigit();

    control_pre();

    // xxx: sweet as a fuck
//  if (wtf0.Second == 20)
//  {
//      LOG("reiniting lcd");
//      InitLCD();
//  }

    control_post(wtf0.Second, wtf0.SostRS == WORK_UNIT);

    if (wtf0.Second >= 60)
    {
        wtf0.Second = 0;

        WriteToFRAM();
#ifndef NOTESTMEM
        if ((!wtf0.Menu)&&(wtf0.SostRS == OUT_UNIT))
            TestMem(1);
#endif
        DoBeepy();
    }
    else
    {
        if (!(wtf0.Second%9))
        {
            LOG("doing measure");
            Measure();
        }
    }
}

static void init(void)
{
    HAL_systimer_init();
    HAL_tty_init(115200);

    LOG("initing RTC...");
    HAL_rtc_init();

    LOG("initing keyboard...");
    Keyboard_Init();

    LOG("initing nvmem ...");
    HAL_nvmem_init();

    LOG("initing lcd ...");
    HAL_lcd_init();

    LOG("initing sound ...");
    sound_init();

    LOG("initing fieldbus ...");
    fieldbus_init();

    //fieldbus_smoke();


    keyboardSetBITKL(0);

    ClrAllOutIPCDigit();


    setup_scatter();
    Init_STM32();

    LOG("initing lcd");

    InitLCD();

    LOG("inited lcd");

//#ifdef SumExtCG
    SendFirstScreen(1);
//#endif
    clear_d();
    wtf0.Menu=0;
    nReset=3;
    w_txt("FITO F405 (c)APL&DAL");
    //Delay(1000000);
    Video();
    gd_rw()->Hot.News |= bKlTest;

    int byte_x=1;
    wtf0.SostRS=OUT_UNIT;
    //KeyboardProcess();

    if (keyboardGetBITKL())
        byte_x=6;
    TestMem(byte_x);

    InitAllThisThings(5);
    ButtonReset();

    wtf0.Second=38;
    control_init();

    LOG("inited control");

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

    u32 prev_time = 0;

    DoBeepy();

    //hal_beep_on(50);

//  while(1)
//  {
//      ;
//  }

    while (1)
    {
        process_pc_input();
        process_legacy_timers();
        timers_process();

        bool should_show_video = 0;

        u32 time = HAL_rtc_get_timestamp();

        if (time != prev_time)
        {
            //LOG("time: %d", time);
            prev_time = time;
            wtf0.Second++;

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

void assert_param(bool wtf)
{
    ;
}
