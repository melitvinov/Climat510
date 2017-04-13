
#include "syntax.h"

#include "stm32f10x_Rootines.h"

// XXX: isolation
#include "405_memory.h"
#include "control_gd.h"
#include "wtf.h"
#include "fbd.h"

#include "control_gd.h"

#include "rtc.h"

#include "keyboard.h"
#include "lcd.h"
#include "unsorted.h"

#include "hal.h"

#include "debug.h"

// XXX: this stuff is from climdef.h
static uint8_t* mymac = 0x1FFFF7EE;



//static unsigned char myip[4] = {192,168,1,231};
static uint16_t    NMinPCOut;

static void ReadFromFRAM(void);

void stm32f10x_Rootines_reset_NMinPCOut()
{
    NMinPCOut = 0;
}

void CheckWithoutPC(void)
{
    if (NMinPCOut>3)
    {
        NMinPCOut=0;
        simple_server(wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac, &wtf0.PORTNUM);
        gd_rw()->TControl.Zones[0].WithoutPC++;
    }
    NMinPCOut++;
}

void Init_STM32(void)
{

    //I2CRel_MEM_Configuration();

    //I2C_BLOCK_Configuration();
    //i2_fm_Init();

    LOG("reading from fram");

    //ReadFromFRAM();

    LOG("read from fram");

    wtf0.PORTNUM=DEF_PORTNUM;

    simple_server(wtf0.AdrGD, &wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac, &wtf0.PORTNUM);
    LOG("started web");

    #warning" init of usart2 is disabled"
//    USART_OUT2_Configuration(9600);
//    LOG("inited uart1");

    UpdateInputConfigs();
    LOG("checked input config");
}


void WriteToFRAM()
{
    return;

    InitBlockEEP();
    #warning "maybe these addresses are wrong"
    SendBlockFRAM((uint32_t)(&gd()->TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM), &gd()->Hot, sizeof(gd()->Hot));
    SendBlockFRAM((uint32_t)(&gd()->TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM)+sizeof(gd()->Hot), &gd()->TControl,sizeof(eTControl)+sizeof(eLevel));
}

static void ReadFromFRAM()
{
    return;

    InitBlockEEP();
    #warning "maybe these addresses are wrong"
    RecvBlockFRAM((uint32_t)(&gd()->TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM), &gd_rw()->Hot, sizeof(gd()->Hot));
    RecvBlockFRAM((uint32_t)(&gd()->TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM)+sizeof(gd()->Hot), &gd_rw()->TControl, sizeof(gd()->TControl)+sizeof(eLevel));

}


void SetRTC(void)
{
    eDateTime   fDateTime;
    fDateTime.sec=wtf0.Second;
    fDateTime.min=gd()->Hot.Time%60;
    fDateTime.hour=gd()->Hot.Time/60;
    fDateTime.mday=gd()->Hot.Date&0xff;
    fDateTime.month=gd()->Hot.Date>>8;
    fDateTime.year=gd()->Hot.Year+2000;
    WriteDateTime(&fDateTime);
}

void GetRTC(uint16_t *time, uint16_t *date, uint8_t *year, u8 *day_of_week)
{
    eDateTime   fDateTime;
    ReadDateTime(&fDateTime); //CtrTime=0;

    *time = fDateTime.min + fDateTime.hour*60;
    *date = fDateTime.mday + (fDateTime.month<<8);
    *year = fDateTime.year-2000;
    *day_of_week = fDateTime.wday;
}


/*------------------------------------------------
        Тестирование и установка данных при сбросе "Тест"
        и ежесекундно
TipRes
0-мягкий старт-уст адресов и проверка Control
1-кнопка ТЕСТ и вкл питания
2-инициализация RAM - GD
10+2-инициализация EEP- Программы
10+1-инициализация EEP- Калибровки
10+0-инициализация EEP- Параметры
--------------------------------------------------*/
void TestMem(uchar TipReset)
{
    #warning "disabled"
    return;

    InitBlockEEP();
    ButtonReset();
//	   TipReset=2;
/*------ проверка контр суммы блока CONTROL ---------------------------*/
    if (TipReset>5)
    {
        InitAllThisThings(5);
    }
    if ((!wtf0.Menu) && TestRAM0())
        TipReset=2;
    if (!TipReset) return;
/*------ проверка контр суммы ОЗУ  -------------------------------*/

    if ((TipReset == 1)&& TestRAM())
        TipReset++;
    if (TipReset<2) return;
    wtf0.Menu=0;

/*-- Восстановление из EEPROM, а при ошибке перезапись в EEPROM------*/
    TestFRAM();
    ButtonReset();
    GetRTC(&gd_rw()->Hot.Time, &gd_rw()->Hot.Date, &gd_rw()->Hot.Year, &NowDayOfWeek);
}

/*-- Восстановление из EEPROM, а при ошибке перезапись в EEPROM------*/
void    TestFRAM()
{
    uint16_t cSum;
    uint8_t nBlFRAM;
    for (nBlFRAM=0;nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++)
    {
        RecvBlockFRAM((u32)BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
        RecvBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&BlockEEP[nBlFRAM].CSum,2);
        cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
        if ((CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size)!=BlockEEP[nBlFRAM].CSum ) || ( BlockEEP[nBlFRAM].Erase  ==  1))
        {
            keyboardSetSIM(100);
            InitAllThisThings(5);
            SendBlockFRAM((u32)BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            SendBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&cSum,2);
            BlockEEP[nBlFRAM].CSum=cSum;
            BlockEEP[nBlFRAM].Erase=0; //++
        }
        /* формирование контр суммы ОЗУ после инициализации */
    }
}
