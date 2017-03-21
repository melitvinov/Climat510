
#include "syntax.h"

#include "stm32f10x_Rootines.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_clock.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_iwdg.h"

// XXX: isolation
#include "405_memory.h"
#include "control_gd.h"
#include "wtf.h"
#include "modules_master.h"

#include "control_gd.h"

#include "keyboard.h"
#include "stm32f10x_LCD240x64.h"
#include "unsorted.h"

#include "hal_systimer.h"

#include "debug.h"

// XXX: this stuff is from climdef.h
static uint8_t* mymac = 0x1FFFF7EE;
static unsigned char myip[4] = {192,168,1,231};
static uint16_t    NMinPCOut;
static uint16_t* IWDG_Reset;

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
        #warning "init of pc uart is diabled"
        //USART_PC_Configuration(&gd()->Control.NFCtr, wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock,9600);
        simple_server(wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac,(uint8_t*)&wtf0.PORTNUM);
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

    simple_server(wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac, (uint8_t*)&wtf0.PORTNUM);
    LOG("started web");

//    Init_MEAS_INPUT();
//    Init_IWDG(&GD.TControl.Tepl[0].nReset);
    Check_IWDG();

    #warning" init of usart2 is disabled"
//    USART_OUT2_Configuration(9600);
//    LOG("inited uart1");

    CheckInputConfig();
    LOG("checked input config");
}

void process_legacy_timers(void)
{
    static u32 ipc_timestamp = 0;

    u32 time = HAL_systimer_get();

    if (time - ipc_timestamp > 50)  // was 50ms in legacy code
    {
        ipc_timestamp = time;
        SendIPC(&gd_rw()->Hot.Zones[0].ConnectionStatus);
    }
}

void OutReg()
{
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

void Init_IWDG(uint16_t* fIWDG_Reset)
{
    IWDG_Reset=fIWDG_Reset;
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload(65000);
    IWDG_ReloadCounter();
#ifndef DEBUG
    IWDG_Enable();
#endif
}

void Check_IWDG(void)
{
//	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        (*IWDG_Reset)++;
//		RCC_ClearFlag();
    }

}

/*------------------------------------------------
        ������������ � ��������� ������ ��� ������ "����"
        � �����������
TipRes
0-������ �����-��� ������� � �������� Control
1-������ ���� � ��� �������
2-������������� RAM - GD
10+2-������������� EEP- ���������
10+1-������������� EEP- ����������
10+0-������������� EEP- ���������
--------------------------------------------------*/
void TestMem(uchar TipReset)
{
    #warning "disabled"
    return;

    InitBlockEEP();
    ButtonReset();
//	   TipReset=2;
/*------ �������� ����� ����� ����� CONTROL ---------------------------*/
    if (TipReset>5)
    {
        InitAllThisThings(5);
    }
    if ((!wtf0.Menu) && TestRAM0())
        TipReset=2;
    if (!TipReset) return;
/*------ �������� ����� ����� ���  -------------------------------*/

    if ((TipReset == 1)&& TestRAM())
        TipReset++;
    if (TipReset<2) return;
    wtf0.Menu=0;

/*-- �������������� �� EEPROM, � ��� ������ ���������� � EEPROM------*/
    TestFRAM(TipReset);
    ButtonReset();
    GetRTC(&gd_rw()->Hot.Time, &gd_rw()->Hot.Date, &gd_rw()->Hot.Year, &NowDayOfWeek);
}

/*-- �������������� �� EEPROM, � ��� ������ ���������� � EEPROM------*/
void    TestFRAM(char EraseBl)
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
        /* ������������ ����� ����� ��� ����� ������������� */
    }
}
