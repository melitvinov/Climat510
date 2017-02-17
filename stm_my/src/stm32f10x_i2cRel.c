#include "stm32f10x_i2cRel.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "misc.h"

#define	I2C_INIT		0
#define I2C_HEAD		1
#define I2C_ADDR		2
#define I2C_SENDDATA	3
#define I2C_RECVDATA	4
#define I2C_START		5

//Таймауты работы в синхронном режиме
#define I2C_FLAGBUSY     timeout=100; while((I2C_GetFlagStatus(I2C_Rel, I2C_FLAG_BUSY))&&(timeout)) timeout--;
#define I2C_EV_MASTER    timeout=100; while((!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_MODE_SELECT))&&(timeout)) timeout--;
#define I2C_EV_TR_MODE   timeout=100; while((!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(timeout)) timeout--;
#define I2C_EV_TR_BYTE   timeout=100; while((!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(timeout)) timeout--;
#define I2C_EV_RV_BYTE   timeout=100; while((!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_BYTE_RECEIVED))&&(timeout)) timeout--;



//Сброс I2C

void ResetI2C()
{
    I2C_BLOCK_Configuration();
}

//Конфигурация I2C в синхронном режиме

void I2C_BLOCK_Configuration(void)
{

    int i,timeout;
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO , ENABLE);//

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_10 | GPIO_Pin_11,Bit_SET);
    for (i=0;i<100;i++);
    GPIO_WriteBit(GPIOB, GPIO_Pin_10 | GPIO_Pin_11,Bit_RESET);


    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C_Rel);
    I2C_FLAGBUSY;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = 1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100kHz */

    I2C_Cmd(I2C_Rel, ENABLE);
    I2C_Init(I2C_Rel, &I2C_InitStructure);
    I2C_FLAGBUSY;
    //I2C_AcknowledgeConfig(I2C_Rel, ENABLE);

/*           NVIC_InitTypeDef NVIC_InitStructure;
           NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
           NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
           NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
           NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
           NVIC_Init(&NVIC_InitStructure);
           NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
           NVIC_Init(&NVIC_InitStructure);
*/

}

/*Синхронная запись в реле
pState - регистр для записи
NumRel - адрес микросхемы реле
*/


void I2C_Rel_Write(uint8_t pState, uint8_t NumRel)
{
    uint16_t    timeout;
    /* Send STRAT condition */
    I2C_GenerateSTART(I2C_Rel, ENABLE);

    /* Test on EV5 and clear it */
    I2C_EV_MASTER;
    if (!timeout)
    {
        ResetI2C();return;
    }
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_Rel, REL_BASE_ADDRESS+(NumRel<<1), I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    I2C_EV_TR_MODE;
    if (!timeout)
    {
        ResetI2C();return;
    }

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_Rel, ~pState);

    /* Test on EV8 and clear it */
    I2C_EV_TR_BYTE;
    if (!timeout)
    {
        ResetI2C();return;
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_Rel, ENABLE);
}



/*Синхронная запись во внешний EEPROM/FRAM
fAddrDev - номер микросхемы от 0
fAddrMem - адрес памяти в EEPROM
fData - адрес памяти в ОЗУ
fptrData - размер передачи*/



void I2C_Mem_Write(uint8_t fAddrDev,uint16_t fAddrMem,uint8_t* fData,uint16_t fptrData)
{
    uint16_t    timeout,i;
#ifdef MEM_TYPE_EEPROM
    while (fptrData)
    {
        I2C_GenerateSTART(I2C_Rel, ENABLE);

        /* Test on EV5 and clear it */
        I2C_EV_MASTER;
        if (!timeout)
        {
            ResetI2C();return 0;
        }
        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C_Rel, MEM_BASE_ADDRESS+fAddrDev, I2C_Direction_Transmitter);

        /* Test on EV6 and clear it */
        I2C_EV_TR_MODE;
        if (!timeout)
        {
            ResetI2C();return 0;
        }

        /* Send the EEPROM's internal address to write to : MSB of the address first */
        I2C_SendData(I2C_Rel, fAddrMem/256);

        /* Test on EV8 and clear it */
        I2C_EV_TR_BYTE;
        if (!timeout)
        {
            ResetI2C();return 0;
        }

        I2C_SendData(I2C_Rel, fAddrMem%256);

        I2C_EV_TR_BYTE;
        if (!timeout)
        {
            ResetI2C();return 0;
        }
        while (fptrData)
        {
            I2C_SendData(I2C_Rel, *fData);
            I2C_EV_TR_BYTE;
            if (!timeout)
            {
                ResetI2C();return 0;
            }
            fData++;
            fptrData--;
            fAddrMem++;
            if (!(fAddrMem%128)) break;

        }
        //Sound;
        /* Send STOP condition */
        I2C_GenerateSTOP(I2C_Rel, ENABLE);
        for (i=0;i<15000;i++);
    }
    return 1;
#else
    /* Send STRAT condition */
    I2C_GenerateSTART(I2C_Rel, ENABLE);

    /* Test on EV5 and clear it */
    I2C_EV_MASTER;
    if (!timeout)
    {
        ResetI2C();return;
    }
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_Rel, MEM_BASE_ADDRESS+fAddrDev, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    I2C_EV_TR_MODE;
    if (!timeout)
    {
        ResetI2C();return;
    }

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_Rel, fAddrMem/256);

    /* Test on EV8 and clear it */
    I2C_EV_TR_BYTE;
    if (!timeout)
    {
        ResetI2C();return;
    }

    I2C_SendData(I2C_Rel, fAddrMem%256);

    I2C_EV_TR_BYTE;
    if (!timeout)
    {
        ResetI2C();return;
    }

    while (fptrData)
    {
        I2C_SendData(I2C_Rel, *fData);
        I2C_EV_TR_BYTE;
        if (!timeout)
        {
            ResetI2C();return;
        }
        fData++;
        fptrData--;
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_Rel, ENABLE);
    return 1;
#endif
}

/*Синхронное чтение области памяти из внешнего EEPROM/FRAM
fAddrDev - номер микросхемы от 0
fAddrMem - адрес памяти в EEPROM
fData - адрес памяти в ОЗУ
fptrData - размер передачи*/

void I2C_Mem_Read(uint8_t fAddrDev,uint16_t fAddrMem,uint8_t* fData,uint16_t fptrData)
{
    uint16_t    timeout;
    /* Send STRAT condition */
    I2C_GenerateSTART(I2C_Rel, ENABLE);

    /* Test on EV5 and clear it */
    I2C_EV_MASTER;
    if (!timeout)
    {
        ResetI2C();return 0;
    }
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_Rel, MEM_BASE_ADDRESS+fAddrDev, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    I2C_EV_TR_MODE;
    if (!timeout)
    {
        ResetI2C();return 0;
    }

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_Rel, fAddrMem/256);

    /* Test on EV8 and clear it */
    I2C_EV_TR_BYTE;
    if (!timeout)
    {
        ResetI2C();return 0;
    }

    I2C_SendData(I2C_Rel, fAddrMem%256);

    I2C_EV_TR_BYTE;
    if (!timeout)
    {
        ResetI2C();return 0;
    }

    //I2C_GenerateSTOP(I2C_Rel, ENABLE);

    I2C_AcknowledgeConfig(I2C_Rel, ENABLE);

    I2C_GenerateSTART(I2C_Rel, ENABLE);

    /* Test on EV5 and clear it */
    I2C_EV_MASTER;
    if (!timeout)
    {
        ResetI2C();return 0;
    }

    I2C_Send7bitAddress(I2C_Rel, MEM_BASE_ADDRESS+fAddrDev, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while (fptrData)
    {
        if (fptrData==1)
        {
            I2C_AcknowledgeConfig(I2C_Rel, DISABLE);
        }
        I2C_EV_RV_BYTE;
        if (!timeout)
        {
            ResetI2C();return 0;
        }
        *fData=I2C_ReceiveData(I2C_Rel);
        fData++;
        fptrData--;
    }
    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_Rel, ENABLE);
    return 1;

}
// Асинхронная работа - недоделана

typedef struct
{
    uint16_t    Status;
    uint8_t     AddrDev;
    uint16_t    AddrMem;
    uint8_t     *Data;
    uint8_t      Type;
    uint16_t    ptrData;
    uint16_t    LastError;
    uint8_t     Dir;

} eI2CFITO;

static eI2CFITO I2C_MAIN;

uint8_t I2C_MainLoad(uint8_t fAddrDev,uint16_t fAddrMem,uint8_t* fData,uint8_t fType, uint16_t fptrData, uint8_t fDir)
{
//	I2C_SoftwareResetCmd(I2C_Rel,DISABLE);
    if (I2C_MAIN.Status) return 0;
    I2C_ITConfig(I2C_Rel,I2C_IT_EVT|I2C_IT_ERR,ENABLE);
    I2C_MAIN.Status=I2C_HEAD;
    I2C_MAIN.AddrDev=fAddrDev;
    I2C_MAIN.AddrMem=MEM_BASE_ADDRESS+fAddrMem;
    I2C_MAIN.Data=fData;
    I2C_MAIN.Type=fType;
    I2C_MAIN.ptrData=fptrData;
    I2C_MAIN.Dir=fDir;
    while (I2C_GetFlagStatus(I2C_Rel, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C_Rel, ENABLE);
    return 1;

}

void I2C2_EV_IRQHandler (void)
{
/*	if (I2C_GetITStatus(I2CRel, USART_IT_RXNE) )
         {
          I2C_ClearITPendingBit(I2CRel, USART_IT_RXNE); //Сбрасываем флаг*/
    switch (I2C_MAIN.Status)
    {
    case I2C_HEAD:
        if (I2C_GetITStatus(I2C_Rel, I2C_IT_SB))
        {
            if (I2C_MAIN.Type==I2C_TP_MEM)
            {
                I2C_Send7bitAddress(I2C_Rel, I2C_MAIN.AddrDev, I2C_Direction_Transmitter);
                I2C_MAIN.Status=I2C_ADDR;
            }
            else
            {

                I2C_Send7bitAddress(I2C_Rel, I2C_MAIN.AddrDev, I2C_MAIN.Dir);
                I2C_MAIN.Status=I2C_SENDDATA;
                if (I2C_MAIN.Dir!=I2C_Direction_Transmitter)
                {
                    I2C_AcknowledgeConfig(I2C_Rel, DISABLE);
                    I2C_MAIN.Status=I2C_RECVDATA;
                }
            }

        }
        else
            I2C_MAIN.Status=10;
        I2C_ReceiveData(I2C_Rel);
        break;
    case I2C_ADDR:
        if (I2C_GetITStatus(I2C_Rel, I2C_IT_ADDR))
        {
            I2C_MAIN.LastError=I2C_GetLastEvent(I2C_Rel);
            I2C_SendData(I2C_Rel,I2C_MAIN.AddrMem/256);

        }
        else if (I2C_GetITStatus(I2C_Rel, I2C_IT_BTF))
        {
            I2C_SendData(I2C_Rel,I2C_MAIN.AddrMem%256);
            I2C_MAIN.Status=I2C_SENDDATA;
            if (I2C_MAIN.Dir==I2C_Direction_Receiver)
                I2C_MAIN.Status=I2C_START;

        }
        else
            I2C_MAIN.Status=11;


        break;
    case I2C_SENDDATA:

        if (I2C_GetITStatus(I2C_Rel, I2C_IT_BTF|I2C_IT_ADDR))
        {
            I2C_MAIN.LastError=I2C_GetLastEvent(I2C_Rel);
            if (!I2C_MAIN.ptrData)
            {
                I2C_GenerateSTOP(I2C_Rel, ENABLE);
                I2C_MAIN.Status=0;
                break;
            }
            I2C_SendData(I2C_Rel,*I2C_MAIN.Data);
            I2C_MAIN.Data++;
            I2C_MAIN.ptrData--;
        }
        else
            I2C_MAIN.Status=12;
        break;
    case I2C_RECVDATA:
        if (I2C_GetITStatus(I2C_Rel,I2C_IT_ADDR))
        {
            I2C_MAIN.LastError=I2C_GetLastEvent(I2C_Rel);
        }
        else if (I2C_GetITStatus(I2C_Rel,I2C_IT_BTF))
        {
            *I2C_MAIN.Data=I2C_ReceiveData(I2C_Rel);
            I2C_MAIN.Data++;
            I2C_MAIN.ptrData--;
            I2C_GenerateSTOP(I2C_Rel, ENABLE);
            if (I2C_MAIN.ptrData)
                I2C_MAIN.Status=I2C_START;
            else
                I2C_MAIN.Status=I2C_INIT;
        }
        else
            I2C_MAIN.Status=15;
        break;
    case I2C_START:
        if (I2C_GetITStatus(I2C_Rel, I2C_IT_BTF|I2C_IT_STOPF))
        {
            I2C_ReceiveData(I2C_Rel);
            I2C_GenerateSTART(I2C_Rel, ENABLE);
            I2C_MAIN.Status=I2C_HEAD;
            I2C_MAIN.Type=0;
        }
        else
            I2C_MAIN.Status=0;
        break;
    default:
//		return;
        I2C_GetITStatus(I2C_Rel, I2C_IT_STOPF);
//		I2C_Cmd(I2C_Rel,DISABLE);
        I2C_GetLastEvent(I2C_Rel);
        I2C_MAIN.Status=0;
        I2C_ReceiveData(I2C_Rel);
        I2C_ITConfig(I2C_Rel,I2C_IT_EVT|I2C_IT_ERR,DISABLE);
        //I2C_SoftwareResetCmd(I2C_Rel,ENABLE);
        break;

    }
}

void I2C2_ER_IRQHandler (void)
{
    I2C_GetITStatus(I2C_Rel,I2C_IT_TIMEOUT);
    I2C_MAIN.LastError=I2C_GetLastEvent(I2C_Rel);
    I2C_MAIN.Status=I2C_INIT;
    I2C_ITConfig(I2C_Rel,I2C_IT_EVT|I2C_IT_ERR,DISABLE);
    //I2C_SoftwareResetCmd(I2C_Rel,ENABLE);
}


uint8_t I2CRel_Busy(void)
{
    return I2C_MAIN.Status;
}

void I2CRel_MEM_Configuration(void)
{

    int i;
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO , ENABLE);//

    /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_10 | GPIO_Pin_11,Bit_SET);
    for (i=0;i<1000;i++);
    GPIO_WriteBit(GPIOB, GPIO_Pin_10 | GPIO_Pin_11,Bit_RESET);
    //           I2C_SoftwareResetCmd(I2C_Rel,ENABLE);
//           while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C_Rel);
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = 1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100kHz */

    I2C_Cmd(I2C_Rel, ENABLE);
    I2C_Init(I2C_Rel, &I2C_InitStructure);
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
    //I2C_AcknowledgeConfig(I2C_Rel, ENABLE);
    I2C_ITConfig(I2C_Rel,I2C_IT_EVT|I2C_IT_ERR,ENABLE);


    /* Enable DMA1 channel6 IRQ
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_EnableIRQ (DMA1_Channel5_IRQn);*/
    I2C_MAIN.Status=0;


}



void I2C_DMAMem_Transfer(uint8_t* I2CBuf,uint16_t I2CMemSize,uint32_t DMA_dir)
{
//	int i;

    DMA_InitTypeDef  DMA_InitStructure;

    I2C_DMACmd(I2C2, DISABLE);
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(I2C_Rel->DR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2CBuf;
    DMA_InitStructure.DMA_DIR = DMA_dir;//DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = I2CMemSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC|DMA_IT_HT|DMA_IT_TE,ENABLE);
//	DMA_SetCurrDataCounter(DMA1_Channel5,I2CMemSize);
    /* Send STRAT condition */
    I2C_GenerateSTART(I2C_Rel, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_Rel, MEM_BASE_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_Rel, 0);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C_Rel, 0);

    while (!I2C_CheckEvent(I2C_Rel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_DMACmd(I2C2, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);


}

void DMA1_Channel5_IRQHandler (void)
{
    //Если обмен завершен
    if ((DMA1->ISR & DMA_ISR_TCIF5)||(DMA1->ISR & DMA_ISR_TEIF5))
    {
        I2C_DMACmd(I2C2, DISABLE);
        DMA_Cmd(DMA1_Channel5, DISABLE);
        I2C_GenerateSTOP(I2C2, ENABLE);

    }      //что-то делаем

}
