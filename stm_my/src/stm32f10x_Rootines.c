
#include "syntax.h"

#include "stm32f10x_Define.h"
#include "stm32f10x_Rootines.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_clock.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_iwdg.h"
#include "I2CSoft.h"

// XXX: isolation
#include "405_memory.h"
#include "65_gd.h"
#include "climdefstuff.h"
#include "stm32f10x_RS485Master.h"

// XXX: this stuff is from climdef.h
static uchar   nSensor;
static uint8_t* mymac = 0x1FFFF7EE;
static unsigned char myip[4] = {192,168,1,231};
static uint16_t    NMinPCOut;
static uint16_t* IWDG_Reset;

#warning "this crap is for isolation"
extern eTepl *pGD_Hot_Tepl;
extern eTControlTepl *pGD_TControl_Tepl;
extern eSensLevel *pGD_Level_Tepl;

void stm32f10x_Rootines_reset_NMinPCOut()
{
    NMinPCOut = 0;
}

void CheckWithoutPC(void)
{
    if (NMinPCOut>3)
    {
        NMinPCOut=0;
        USART_PC_Configuration(&GD.Control.NFCtr, WTF0.AdrGD,&GD.SostRS,&WTF0.NumBlock,9600);
        simple_server(WTF0.AdrGD,&GD.SostRS,&WTF0.NumBlock,GD.Control.IPAddr,mymac,(uint8_t*)&WTF0.PORTNUM);
        GD.TControl.Tepl[0].WithoutPC++;
    }
    NMinPCOut++;
}

void Init_STM32(void)
{

    //I2CRel_MEM_Configuration();

    //I2C_BLOCK_Configuration();
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    i2_fm_Init();

    ReadFromFRAM();

    InitLCD();


//		USART_PC_Configuration(&GD.Control.NFCtr,AdrGD,&GD.SostRS,&NumBlock,9600);


    InitMainTimer();
    SETEA;


    Keyboard_Init();
    InitRTC();
    WTF0.PORTNUM=DEF_PORTNUM;

    simple_server(WTF0.AdrGD,&GD.SostRS,&WTF0.NumBlock,GD.Control.IPAddr,mymac, (uint8_t*)&WTF0.PORTNUM);


    w1Init();
    Init_MEAS_INPUT();
//    Init_IWDG(&GD.TControl.Tepl[0].nReset);
    Check_IWDG();
    USART_OUT_Configuration(9600);

    USART_OUT2_Configuration(9600);
    //I2C_DMAMem_Transfer(I2C1_Buffer_Tx,8,DMA_DIR_PeripheralDST);
    //������
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

    CheckInputConfig();
    InitIPCTimer();
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
        u16Temp |=  0x000f&~(1<<(i%4));//����������� ���� ����� �� 0xff ����� ��������������
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
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC->APB1ENR |= RCC_APB1Periph_TIM3;


    TIM3->PSC = 8000-1; // Clock prescaler;

    TIM3->ARR = 50;//33 // Auto reload value
    TIM3->SR = 0; // Clean interrups & events flag

    TIM3->DIER = TIM_DIER_UIE; // Enable update interrupts

    /* NVIC_SetPriority & NVIC_EnableIRQ defined in core_cm3.h */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM3->EGR = TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN; // Enable timer
}

void TIM3_IRQHandler(void)
{

    Reg48ToI2C();

    TIM3->SR=0;

}

void InitMainTimer(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC->APB1ENR |= RCC_APB1Periph_TIM2;


    TIM2->PSC = 8000-1; // Clock prescaler;

    TIM2->ARR = 100; // Auto reload value
    TIM2->SR = 0; // Clean interrups & events flag

    TIM2->DIER = TIM_DIER_UIE; // Enable update interrupts

    /* NVIC_SetPriority & NVIC_EnableIRQ defined in core_cm3.h */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

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

}

void InitRTC(void)
{
    int i;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    // XXX: here was an unworking delay

//	RCC_LSEConfig(RCC_LSE_ON);
//	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) { ; }
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    RCC_LSICmd(ENABLE);  // ������������� ��� ���������� ������

//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);



    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A6)
    {
        /* Backup data register value is not correct or not yet programmed (when
           the first time the program is executed) */

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /* Reset Backup Domain */
        BKP_DeInit();

        RCC_LSEConfig(RCC_LSE_OFF);

        RCC_LSICmd(ENABLE);  // ������������� ��� ���������� ������
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

/*		// Enable LSE
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) { ; }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);*/
        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Set RTC prescaler: set RTC period to 1sec */
        RTC_SetPrescaler(40000); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Set initial value */
        RTC_SetCounter( (uint32_t)((11*60+55)*60) ); // here: 1st January 2000 11:55:00
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A6);
        /* Lock access to BKP Domain */
        PWR_BackupAccessCmd(DISABLE);

    }
    else
    {

        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
    }
    RTC_ITConfig(RTC_IT_SEC, ENABLE);




}


void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_WaitForLastTask();
        #warning "this baby should be volatile !"
        WTF0.bSec=1;
        WTF0.Second++;
        //SumAnswers=IntCount;
        //IntCount=0;
    }
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
    SendIPC(&GD.Hot.Tepl[0].ConnectionStatus);
}

void OutReg()
{
}


void WriteToFRAM()
{
    InitBlockEEP();
    SendBlockFRAM((uint32_t)(&GD.TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM),(uchar*)(&GD.Hot),sizeof(GD.Hot));
    SendBlockFRAM((uint32_t)(&GD.TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM)+sizeof(GD.Hot),(uchar*)(&GD.TControl),sizeof(eTControl)+sizeof(eLevel));
}

void ReadFromFRAM()
{
    InitBlockEEP();
    RecvBlockFRAM((uint32_t)(&GD.TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM),(uchar*)(&GD.Hot),sizeof(GD.Hot));
    RecvBlockFRAM((uint32_t)(&GD.TControl)-(uint32_t)(BlockEEP[0].AdrCopyRAM)+sizeof(GD.Hot),(uchar*)(&GD.TControl),sizeof(GD.TControl)+sizeof(eLevel));

}


void SetRTC(void)
{
    eDateTime   fDateTime;
    fDateTime.sec=WTF0.Second;
    fDateTime.min=CtrTime%60;
    fDateTime.hour=CtrTime/60;
    fDateTime.mday=CtrData&0xff;
    fDateTime.month=CtrData>>8;
    fDateTime.year=CtrYear+2000;
    WriteDateTime(&fDateTime);
}

void GetRTC(void)
{
    eDateTime   fDateTime;
    ReadDateTime(&fDateTime); //CtrTime=0;

    //Second=DateTime.Sec&0x0F;
    //Second+=(DateTime.Sec>>4)*10;
    CtrTime=fDateTime.min;
    CtrTime+=fDateTime.hour*60;
    CtrData=fDateTime.mday;
    CtrData+=fDateTime.month<<8;
    CtrYear=fDateTime.year-2000;
    NowDayOfWeek=fDateTime.wday;
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

    // ��������� ����������:
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
    // �� ���� � ���, �� �� �����:

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

void CheckDigitMidl(eSensing *ftemp,int16_t* Mes, int16_t* ValueS, uint8_t* tPause, uint16_t tFilter)
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
    int16_t         *uS;
    eNameASens  *nameS;
    eSensing    *valueS;
    int16_t         *llS;
    int16_t         *lS;
    int16_t         *levelS;
    uint8_t         *tPause;
    uS=&GD.uInTeplSens[fnTepl][fnSens];
    nameS=&NameSensConfig[fnSens];
    SetPointersOnTepl(fnTepl);
    valueS=&(pGD_Hot_Tepl->InTeplSens[fnSens]);
    llS=&(pGD_TControl_Tepl->LastLastInTeplSensing[fnSens]);
    lS=&(pGD_TControl_Tepl->LastInTeplSensing[fnSens]);
    levelS=pGD_Level_Tepl[fnSens];
    tPause=&pGD_TControl_Tepl->TimeInTepl[fnSens];
    if (met)
    {
        uS=&GD.uMeteoSens[fnSens];
        nameS=&NameSensConfig[fnSens+cConfSSens];
        valueS=&GD.Hot.MeteoSensing[fnSens];
        //llS=&GD.TControl.LastLastMeteoSensing[fnSens];
        //lS=&GD.TControl.LastMeteoSensing[fnSens];
        levelS=GD.Level.MeteoSens[fnSens];
        tPause=&GD.TControl.TimeMeteoSensing[fnSens];
    }
    if (full)
    {
        if (((*uS)<nameS->uMin)||((*uS)>nameS->uMax))
            valueS->RCS |= cbMinMaxUSens;
    }
    if (Mes < nameS->Min)
    {
        if ((nameS->TypeSens==cTypeSun)||(nameS->TypeSens==cTypeRain)||(nameS->TypeSens==cTypeFram)||(nameS->TypeSens==cTypeScreen))
            Mes=nameS->Min;
        else
        {
            valueS->RCS |= cbMinMaxVSens;
            Mes=0;
        }
    }
    if (Mes > nameS->Max)
    {
        if ((nameS->TypeSens==cTypeRain)||(nameS->TypeSens==cTypeRH)||(nameS->TypeSens==cTypeFram)||(nameS->TypeSens==cTypeScreen))
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
    if (nameS->TypeSens==cTypeFram)
    {
        if (! (pGD_TControl_Tepl->MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS & cMSBusyMech))
            pGD_TControl_Tepl->MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS |= cMSFreshSens;

    }
    if (nameS->TypeSens==cTypeScreen)
    {
        if (!YesBit(pGD_TControl_Tepl->MechBusy[cHSmScrTH].RCS,cMSBusyMech))
            pGD_TControl_Tepl->MechBusy[cHSmScrTH].RCS |= cMSFreshSens;

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
    eNameASens  *fNameSens;
    int16_t     *fuSens;
    eCalSensor  *fCalSens;
    char        met=0;
    if (nSArea)
    {
        fSens=&GD.Hot.Tepl[nTepl].InTeplSens[nSens];
        fuSens=&GD.uInTeplSens[nTepl][nSens];
        fCalSens=&GD.Cal.InTeplSens[nTepl][nSens];
        fNameSens=&NameSensConfig[nSens];
        met=0;
    }
    else
    {
        fSens=&GD.Hot.MeteoSensing[nSens];
        fuSens=&GD.uMeteoSens[nSens];
        fCalSens=&GD.Cal.MeteoSens[nSens];
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
    InitBlockEEP();  /*������������ � GD */
    ButtonReset();
//	   TipReset=2;
/*------ �������� ����� ����� ����� CONTROL ---------------------------*/
    if (TipReset>5) InitGD(5);
    if ((!WTF0.Menu) && TestRAM0())
        TipReset=2;
    if (!TipReset) return;
/*------ �������� ����� ����� ���  -------------------------------*/

    if ((TipReset==1)&& TestRAM())
        TipReset++;
    if (TipReset<2) return;
    WTF0.Menu=0;

/*-- �������������� �� EEPROM, � ��� ������ ���������� � EEPROM------*/
    TestFRAM(TipReset);
    ButtonReset();
    GetRTC();
}

/*-- �������������� �� EEPROM, � ��� ������ ���������� � EEPROM------*/
void    TestFRAM(char EraseBl)
{
    uint16_t cSum;
    uint8_t nBlFRAM;
    for (nBlFRAM=0;nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++)
    {
        RecvBlockFRAM(BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
        RecvBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&BlockEEP[nBlFRAM].CSum,2);
        cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
        if ((CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size)!=BlockEEP[nBlFRAM].CSum ) || ( BlockEEP[nBlFRAM].Erase == 1))
        {
            InitGD(5);
            SendBlockFRAM(BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            SendBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&cSum,2);
            BlockEEP[nBlFRAM].CSum=cSum;
            BlockEEP[nBlFRAM].Erase=0; //++
        }
        /* ������������ ����� ����� ��� ����� ������������� */
    }
}

void Measure()
{
    char tTepl,nSens;
    uint16_t    tSensVal;
    int nModule;
    int8_t ErrModule;
    for (tTepl=0;tTepl<cSTepl;tTepl++)
    {
        for (nSens=0;nSens<cConfSSens;nSens++)
        {
            tSensVal=GetInIPC(GetSensConfig(tTepl,nSens),&ErrModule);
            if (ErrModule<0)
            {
                GD.Hot.Tepl[tTepl].InTeplSens[nSens].RCS=cbNoWorkSens;
                GD.Hot.Tepl[tTepl].InTeplSens[nSens].Value=0;
                GD.uInTeplSens[tTepl][nSens]=0;
                continue;
            }
            if (ErrModule>=iMODULE_MAX_ERR) tSensVal=0;
            CalibrNew(1,tTepl,nSens,tSensVal);
        }
    }
    for (nSens=0;nSens<cConfSMetSens;nSens++)
    {
        tSensVal=GetInIPC(GetMetSensConfig(nSens),&ErrModule);
        if (ErrModule<0)
        {
            GD.Hot.MeteoSensing[nSens].RCS=cbNoWorkSens;
            GD.uMeteoSens[nSens]=0;
            continue;
        }
        if (ErrModule>=iMODULE_MAX_ERR) tSensVal=0;
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
    tTempConf.V1=0;
    tTempConf.V2=0;
    tTempConf.U1=0;
    tTempConf.U2=0;

    for (tTepl=0;tTepl<cSTepl;tTepl++)
        for (nSens=0;nSens<cConfSInputs;nSens++)
        {
            tTempConf.Input=GetInputConfig(tTepl,nSens)%100;
            UpdateInIPC(GetInputConfig(tTepl,nSens),&tTempConf);
        }
    for (tTepl=0;tTepl<cSTepl;tTepl++)
        for (nSens=0;nSens<cConfSSens;nSens++)
            UpdateInIPC(GetSensConfig(tTepl,nSens), &GD.Cal.InTeplSens[tTepl][nSens]);
    for (nSens=0;nSens<cConfSMetSens;nSens++)
        UpdateInIPC(GetMetSensConfig(nSens),&GD.Cal.MeteoSens[nSens]);

}

void SetDiskrSens(void)
{
    char fnTepl,nSens,nErr;
    for (fnTepl=0;fnTepl<cSTepl;fnTepl++)
    {
        SetPointersOnTepl(fnTepl);
        for (nSens=0;nSens<cConfSInputs;nSens++)
            if (GetDiskrIPC(GetInputConfig(fnTepl,nSens),&nErr))
                pGD_Hot_Tepl->DiskrSens[0]|=1<<nSens;
/*		if (YesBit(RegLEV,(cSmLightLev1<<fnTepl)))
            pGD_Hot_Tepl->DiskrSens[0] |= cSmLightDiskr;
*/
    }

}
