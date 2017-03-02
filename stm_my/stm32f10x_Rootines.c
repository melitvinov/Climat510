
#include "syntax.h"

#include "stm32f10x_Define.h"
#include "stm32f10x_Rootines.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_clock.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_RS485.h"
#include "I2CSoft.h"

// XXX: isolation
#include "405_memory.h"
#include "control_gd.h"
#include "wtf.h"
#include "stm32f10x_RS485Master.h"

#include "control_gd.h"

#include "keyboard.h"
#include "stm32f10x_LCD240x64.h"
#include "unsorted.h"

#include "debug.h"

#define GetSensConfig(nTepl,nSens)	(gd()->MechConfig[nTepl].RNum[nSens+SUM_NAME_INSENS])
#define GetInputConfig(nTepl,nSens)	(gd()->MechConfig[nTepl].RNum[nSens+SUM_NAME_INPUTS])

// XXX: this stuff is from climdef.h
static uchar   nSensor;
static uint8_t* mymac = 0x1FFFF7EE;
static unsigned char myip[4] = {192,168,1,231};
static uint16_t    NMinPCOut;
static uint16_t* IWDG_Reset;

static void ReadFromFRAM(void);
static void Init_MEAS_INPUT(void);

void stm32f10x_Rootines_reset_NMinPCOut()
{
    NMinPCOut = 0;
}

void CheckWithoutPC(void)
{
    if (NMinPCOut>3)
    {
        NMinPCOut=0;
        USART_PC_Configuration(&gd()->Control.NFCtr, wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock,9600);
        simple_server(wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac,(uint8_t*)&wtf0.PORTNUM);
        gd_rw()->TControl.Zones[0].WithoutPC++;
    }
    NMinPCOut++;
}

void Init_STM32(void)
{

    //I2CRel_MEM_Configuration();

    //I2C_BLOCK_Configuration();
    i2_fm_Init();

    LOG("reading from fram");

    ReadFromFRAM();

    LOG("read from fram");

    InitLCD();

    LOG("inited lcd");


//		USART_PC_Configuration(&GD.Control.NFCtr,AdrGD,&GD.SostRS,&NumBlock,9600);


//    InitMainTimer();

    LOG("inited main timer");


    Keyboard_Init();

    LOG("inited keyboard");

    wtf0.PORTNUM=DEF_PORTNUM;

    simple_server(wtf0.AdrGD,&wtf0.SostRS,&wtf0.NumBlock, gd()->Control.IPAddr,mymac, (uint8_t*)&wtf0.PORTNUM);
    LOG("started web");


    w1Init();
//    Init_MEAS_INPUT();
//    Init_IWDG(&GD.TControl.Tepl[0].nReset);
    Check_IWDG();
    USART_OUT_Configuration(9600);
    LOG("inited uart0");

    USART_OUT2_Configuration(9600);
    LOG("inited uart1");
    //I2C_DMAMem_Transfer(I2C1_Buffer_Tx,8,DMA_DIR_PeripheralDST);
    //Музыка
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

    CheckInputConfig();
    LOG("checked input config");
    //InitIPCTimer();
    LOG("inited ipc timer");
}

/***************************************************************************//**
 * @brief  Setting MEASUREMENTS
 ******************************************************************************/



/***************************************************************************//**
 * @brief  Setting DATA pins to input mode
 ******************************************************************************/
/*void Keyboard_Init(void)
{
    KEYB_STARTUP;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = KEYB_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT_KEYB_OUT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEYB_IN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(PORT_KEYB_IN, &GPIO_InitStructure);

}*/

/*
char CheckKeyboardSTM()
{
    int i;
    uint16_t sendB;
    uint16_t u16Temp=0;
    Keyboard_Init();
    if(KeyDelay>1) {KeyDelay--;return 0;}

    for (i=0;i<16;i++)
    {

        //GPIO_Write(PORT_KEYB_OUT, u16Temp);
        u16Temp = GPIO_ReadOutputData(PORT_KEYB_OUT)&(~KEYB_OUT);
        u16Temp |=  0x000f&~(1<<(i%4));//Внимательно если маска не 0xff нужно преобразование
        GPIO_Write(PORT_KEYB_OUT, u16Temp);
        NOP;NOP;

        sendB=i/4;
        sendB=0x01<<sendB;
        u16Temp= GPIO_ReadInputData(PORT_KEYB_IN)&KEYB_IN;
        if (!(u16Temp&sendB))
        {
            SIM=i; BITKL=1;
            KeyDelay=4;
            return 1;
        }

    }
    return 0;
}
*/

void InitIPCTimer(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


    TIM3->PSC = 8000-1; // Clock prescaler;

    // 50 ms
    TIM3->ARR = 50;//33 // Auto reload value
    TIM3->SR = 0; // Clean interrups & events flag

    TIM3->DIER = TIM_DIER_UIE; // Enable update interrupts

    NVIC_SetPriority(TIM3_IRQn, 10);
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM3->EGR = TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN; // Enable timer
}

void TIM3_IRQHandler(void)
{

    Reg48ToI2C();

    TIM3->SR=0;
    asm volatile ("dmb":::);
    asm volatile ("dmb":::);
}

void InitMainTimer(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


    TIM2->PSC = 8000-1; // Clock prescaler;
    // 8MHz / 8000 = 1kHz (1 ms)

    TIM2->ARR = 100; // Auto reload value
    // 100 ms
    TIM2->SR = 0; // Clean interrups & events flag

    TIM2->DIER = TIM_DIER_UIE; // Enable update interrupts

    NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN; // Enable timer
}

void TIM2_IRQHandler(void)
{
    //SumAnswers=IntCount;
/*	CLREA;
    Mes=IntCount;
    //SumAnswers++;
    //nPort=0;
    GPIOB->ODR=(GPIOB->ODR&(~0x100))|((nPort%2)<<8);
    nPort>>=1;
    GPIOA->ODR=(GPIOA->ODR&(~0x800))|((nPort%2)<<11);
    nPort>>=1;
    GPIOA->ODR=(GPIOA->ODR&(~0x700))|((nPort%8)<<8);
    IntCount=0;
    SETEA;
    ReadyIZ=1;*/

#warning !!!!!!!!!!!!!!!!!!!!!!!!!!!! ON
    //CheckKeyboardSTM();
    KeyboardProcess();


    //Reg48ToI2C();
    //SendCharPC(22);
    /*if (Second%2)
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
    else
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);*/
    TIM2->SR=0;
    asm volatile ("dmb":::);
    asm volatile ("dmb":::);
}

#define PORT1WIRE	GPIOB
#define PIN1WIRE	GPIO_Pin_12

void w1Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN1WIRE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT1WIRE, &GPIO_InitStructure);
}

void Reg48ToI2C()
{
    uint16_t i;
//	for (i=0;i<8;i++)
//		I2C_Rel_Write(OutR[i],i);
    SendIPC(&gd_rw()->Hot.Zones[0].ConnectionStatus);
}

void OutReg()
{
}


void WriteToFRAM()
{
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

void Init_MEAS_INPUT()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Настройка прерывания:
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);

    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line9) != RESET)
    {

    }
    // Да если и нет, то всё равно:

    EXTI_ClearITPendingBit(EXTI_Line9);
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

static void CheckDigitMidl(eSensing *ftemp,int16_t* Mes, int16_t* ValueS, uint8_t* tPause, uint16_t tFilter)
{
    if (((*Mes<*ValueS+tFilter)&&(*Mes>*ValueS-tFilter))||(*tPause>30)||(!*ValueS))
        *tPause=0;
    else
    {
        *Mes=*ValueS;
        if (*tPause<120) (*tPause)++;
    }

/*	if ((YesBit(ftemp->RCS,cbNotGoodSens)))
        ClrBit(ftemp->RCS,cbNotGoodSens);
    else
    {
        if ((*Mes>*ValueS+50)||(*Mes<*ValueS-50))
        {
            ftemp->RCS |= cbNotGoodSens;
            *Mes=*ValueS;
        }
    }*/
}

void CheckSensLevsNew(char fnTepl,uint8_t fnSens,char full,char met,int16_t Mes)
{
    const int16_t         *uS;
    const eNameASens  *nameS;
    eSensing    *valueS;
    int16_t         *llS;
    int16_t         *lS;
    int16_t         *levelS;
    uint8_t         *tPause;
    uS=&sensdata.uInTeplSens[fnTepl][fnSens];
    nameS=&NameSensConfig[fnSens];

    valueS=&gd_rw()->Hot.Zones[fnTepl].InTeplSens[fnSens];
    llS=&gd_rw()->TControl.Zones[fnTepl].LastLastInTeplSensing[fnSens];
    lS=&gd_rw()->TControl.Zones[fnTepl].LastInTeplSensing[fnSens];
    levelS=gd_rw()->Level.InTeplSens[fnTepl][fnSens];
    tPause=&gd_rw()->TControl.Zones[fnTepl].TimeInTepl[fnSens];
    if (met)
    {
        uS=&sensdata.uMeteoSens[fnSens];
        nameS=&NameSensConfig[fnSens+cConfSSens];
        valueS=&gd_rw()->Hot.MeteoSensing[fnSens];
        //llS=&GD.TControl.LastLastMeteoSensing[fnSens];
        //lS=&GD.TControl.LastMeteoSensing[fnSens];
        levelS=gd_rw()->Level.MeteoSens[fnSens];
        tPause=&gd_rw()->TControl.TimeMeteoSensing[fnSens];
    }
    if (full)
    {
        if (((*uS)<nameS->uMin)||((*uS)>nameS->uMax))
            valueS->RCS |= cbMinMaxUSens;
    }
    if (Mes < nameS->Min)
    {
        if ((nameS->TypeSens == cTypeSun)||(nameS->TypeSens == cTypeRain)||(nameS->TypeSens == cTypeFram)||(nameS->TypeSens == cTypeScreen))
            Mes=nameS->Min;
        else
        {
            valueS->RCS |= cbMinMaxVSens;
            Mes=0;
        }
    }
    if (Mes > nameS->Max)
    {
        if ((nameS->TypeSens == cTypeRain)||(nameS->TypeSens == cTypeRH)||(nameS->TypeSens == cTypeFram)||(nameS->TypeSens == cTypeScreen))
            Mes=nameS->Max;
        else
        {
            valueS->RCS |= cbMinMaxVSens;
            Mes=0;
        }
    }

    switch (nameS->TypeMidl)
    {
    case cNoMidlSens:
        break;
    case c2MidlSens:
        (*llS)=0;
    case c3MidlSens:
        {
            if (met) break;

            int16_t int_x = (*llS);
            int16_t int_y = (*lS);
            (*llS)=int_y;
            (*lS)=Mes;
            int16_t int_z = 0;
            if (Mes) int_z++;
            if (int_x) int_z++;
            if (int_y) int_z++;
            if (int_z) Mes=(Mes+int_x+int_y)/int_z;

        }
        break;
    case cExpMidlSens:
        if (!(*lS)) (*lS)=Mes;
        Mes=(int)((((long int)(*lS))*(1000-cKExpMidl)+((long int)Mes)*cKExpMidl)/1000);
        (*lS)=Mes;
        break;
    }
    if (!met)
        CheckDigitMidl(valueS,&Mes,&valueS->Value,tPause,nameS->DigitMidl);
    if (nameS->TypeSens == cTypeFram)
    {
        if (! (gd()->TControl.Zones[fnTepl].MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[fnTepl].MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS |= cMSFreshSens;

    }
    if (nameS->TypeSens == cTypeScreen)
    {
        if (! (gd()->TControl.Zones[fnTepl].MechBusy[cHSmScrTH].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[fnTepl].MechBusy[cHSmScrTH].RCS |= cMSFreshSens;

    }
    valueS->Value=Mes;
    /*ClrBit(valueS->RCS,(cbDownAlarmSens+cbUpAlarmSens));
    if ((levelS[cSmDownCtrlLev])&&(Mes <= levelS[cSmDownCtrlLev]))
        valueS->RCS |= cbDownCtrlSens;
    if ((levelS[cSmUpCtrlLev])&&(Mes >= levelS[cSmUpCtrlLev]))
        valueS->RCS |= cbUpCtrlSens;
    if ((levelS[cSmDownAlarmLev])&&(Mes <= levelS[cSmDownAlarmLev]))
    {
        valueS->RCS |= cbDownAlarmSens;
        return;
    }
    if ((levelS[cSmUpAlarmLev])&&(Mes >= levelS[cSmUpAlarmLev]))
    {
        valueS->RCS |= cbUpAlarmSens;
        return;
    }*/
}

void  CalibrNew(char nSArea,char nTepl, char nSens,int16_t Mes)
{
    eSensing    *fSens;
    const eNameASens  *fNameSens;
    int16_t     *fuSens;
    eCalSensor  *fCalSens;
    char        met=0;
    if (nSArea)
    {
        fSens=&gd_rw()->Hot.Zones[nTepl].InTeplSens[nSens];
        fuSens=&sensdata.uInTeplSens[nTepl][nSens];
        fCalSens=&caldata.Cal.InTeplSens[nTepl][nSens];
        fNameSens=&NameSensConfig[nSens];
        met=0;
    }
    else
    {
        fSens=&gd_rw()->Hot.MeteoSensing[nSens];
        fuSens=&sensdata.uMeteoSens[nSens];
        fCalSens=&caldata.Cal.MeteoSens[nSens];
        fNameSens=&NameSensConfig[nSens+cConfSSens];
        met=1;
    }
    fSens->RCS=(fSens->RCS&(cbNotGoodSens+cbDownAlarmSens+cbUpAlarmSens));
    switch (fNameSens->TypeSens)
    {
    case cTypeFram:
    case cTypeScreen:
    case cTypeSun:
    case cTypeRain:
    case cTypeAnal:
    case cTypeRH:
    case cTypeMeteo:
        {
            //Mes=(int)((long int)Mes*(long int)1000/(long int)GD.Cal.Port);
            fuSens[0]=Mes;
//			if(Mes>5000)
//				Mes=0;
            long long_x = ((long)fCalSens->V1-(long)fCalSens->V0)
                 *((long)Mes-(long)fCalSens->U0);
            Mes=(int16_t)(long_x/((long)fCalSens->U1-(long)fCalSens->U0));
            Mes=Mes+fCalSens->V0;
            CheckSensLevsNew(nTepl,nSens,1,met,Mes);
            return;
        }
    }
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
    TestFRAM(TipReset);
    ButtonReset();
    GetRTC(&gd_rw()->Hot.Time, &gd_rw()->Hot.Date, &gd_rw()->Hot.Year, &NowDayOfWeek);
}

/*-- Восстановление из EEPROM, а при ошибке перезапись в EEPROM------*/
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
        /* формирование контр суммы ОЗУ после инициализации */
    }
}

void Measure()
{
    char tTepl,nSens;
    uint16_t    tSensVal;
    int nModule;
    int8_t ErrModule;
    for (tTepl=0;tTepl<NZONES;tTepl++)
    {
        for (nSens=0;nSens<cConfSSens;nSens++)
        {
            tSensVal=GetInIPC(GetSensConfig(tTepl,nSens),&ErrModule);
            if (ErrModule<0)
            {
                gd_rw()->Hot.Zones[tTepl].InTeplSens[nSens].RCS=cbNoWorkSens;
                gd_rw()->Hot.Zones[tTepl].InTeplSens[nSens].Value=0;
                sensdata.uInTeplSens[tTepl][nSens]=0;
                continue;
            }
            if (ErrModule>=iMODULE_MAX_ERR)
                tSensVal=0;
            CalibrNew(1,tTepl,nSens,tSensVal);
        }
    }
    for (nSens=0;nSens<cConfSMetSens;nSens++)
    {
        tSensVal=GetInIPC(GetMetSensConfig(nSens),&ErrModule);
        if (ErrModule<0)
        {
            gd_rw()->Hot.MeteoSensing[nSens].RCS=cbNoWorkSens;
            sensdata.uMeteoSens[nSens]=0;
            continue;
        }
        if (ErrModule>=iMODULE_MAX_ERR)
            tSensVal=0;
        CalibrNew(0,0,nSens,tSensVal);
    }
}

void CheckInputConfig()
{
    char tTepl,nSens;
    TIModulConf tTempConf;
    tTempConf.Corr=0;
    tTempConf.Output=0;
    tTempConf.Type=0;
    tTempConf.V1 = 0;
    tTempConf.V2 = 0;
    tTempConf.U1 = 0;
    tTempConf.U2 = 0;

    for (tTepl=0;tTepl<NZONES;tTepl++)
        for (nSens=0;nSens<cConfSInputs;nSens++)
        {
            tTempConf.Input=GetInputConfig(tTepl,nSens)%100;
            UpdateInIPC(GetInputConfig(tTepl,nSens),&tTempConf);
        }
    for (tTepl=0;tTepl<NZONES;tTepl++)
        for (nSens=0;nSens<cConfSSens;nSens++)
            UpdateInIPC(GetSensConfig(tTepl,nSens), &caldata.Cal.InTeplSens[tTepl][nSens]);
    for (nSens=0;nSens<cConfSMetSens;nSens++)
        UpdateInIPC(GetMetSensConfig(nSens),&caldata.Cal.MeteoSens[nSens]);

}

void LoadDiscreteInputs(void)
{
    for (int zone_idx=0; zone_idx<NZONES; zone_idx++)
    {
        eZone *zone = &gd_rw()->Hot.Zones[zone_idx];
        char nErr;
        for (int input_idx=0; input_idx < cConfSInputs; input_idx++)
        {
            if (GetDiskrIPC(GetInputConfig(zone_idx,input_idx),&nErr))
                zone->discrete_inputs[0] |= 1<<input_idx;
        }
    }
}
