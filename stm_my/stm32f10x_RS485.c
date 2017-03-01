#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_RS485.h"
#include "misc.h"


#define RS_START				0
#define RS_HEAD					1
#define RS_RECV					2
#define RS_SEND					3
#define RS_CHECKSUM				4

#define MAX_RS_TIME				6

#define HEAD_SIZE				5


#ifdef USART_PC1
#define USART_PC				USART1
//Ножка передатчика USART
#define USART_PC_TX_PIN			GPIO_Pin_9
//Ножка приемника USART
#define USART_PC_RX_PIN			GPIO_Pin_10
//Ножка направления данных USART
#define USART_PC_DIR_PIN		GPIO_Pin_8
//Обработчик прерывания USART
#define USART_PC_INT_VECT		void USART1_IRQHandler(void)
//Вектор прерывания USART
#define USART_PC_IRQ			USART1_IRQn
//Имя порта, на котором находится USART
#define USART_PC_DIR_PORT			GPIOA

#define USART_PC_TX_PORT			GPIOA

#define USART_PC_RX_PORT			GPIOA
//Инициализационная строка USART
#define USART_PC_STARTUP    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE)
#endif

#ifdef USART_PC2
#define USART_PC				USART2
//Ножка передатчика USART
#define USART_PC_TX_PIN			GPIO_Pin_2
//Ножка приемника USART
#define USART_PC_RX_PIN			GPIO_Pin_3
//Ножка направления данных USART
#define USART_PC_DIR_PIN		GPIO_Pin_4
//Обработчик прерывания USART
#define USART_PC_INT_VECT		void USART2_IRQHandler(void)
//Вектор прерывания USART
#define USART_PC_IRQ			USART2_IRQn
//Имя порта, на котором находится USART
#define USART_PC_PORT			GPIOA
//Инициализационная строка USART
#define USART_PC_STARTUP    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

#endif

#ifdef USART_PC3
#define USART_PC				USART3

//Ножка передатчика USART
#define USART_PC_TX_PIN			GPIO_Pin_10
//Ножка приемника USART
#define USART_PC_RX_PIN			GPIO_Pin_11
//Ножка направления данных USART
#define USART_PC_DIR_PIN		GPIO_Pin_12
//Обработчик прерывания USART
#define USART_PC_INT_VECT		void USART3_IRQHandler(void)
//Вектор прерывания USART
#define USART_PC_IRQ			USART3_IRQn
//Имя порта, на котором находится USART
#define USART_PC_PORT			GPIOB
//Инициализационная строка USART
#define USART_PC_STARTUP    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)

#endif

#ifdef USART_PC4
#define USART_PC				UART4
//Ножка передатчика USART
#define USART_PC_TX_PIN			GPIO_Pin_10
//Ножка приемника USART
#define USART_PC_RX_PIN			GPIO_Pin_11
//Ножка направления данных USART
#define USART_PC_DIR_PIN		GPIO_Pin_12
//Обработчик прерывания USART
#define USART_PC_INT_VECT		void UART4_IRQHandler(void)
//Вектор прерывания USART
#define USART_PC_IRQ			UART4_IRQn
//Имя порта, на котором находится USART
#define USART_PC_DIR_PORT			GPIOC

#define USART_PC_TX_PORT			GPIOC

#define USART_PC_RX_PORT			GPIOC

//Инициализационная строка USART
#define USART_PC_STARTUP		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
#endif


#ifdef USART_PC5
#define USART_PC				UART5
//Ножка передатчика USART
#define USART_PC_TX_PIN			GPIO_Pin_12
//Ножка приемника USART
#define USART_PC_RX_PIN			GPIO_Pin_2
//Ножка направления данных USART
#define USART_PC_DIR_PIN		GPIO_Pin_4
//Обработчик прерывания USART
#define USART_PC_INT_VECT		void UART5_IRQHandler(void)
//Вектор прерывания USART
#define USART_PC_IRQ			UART5_IRQn
//Имя порта, на котором находится USART
#define USART_PC_DIR_PORT			GPIOB

#define USART_PC_TX_PORT			GPIOC

#define USART_PC_RX_PORT			GPIOD

//Инициализационная строка USART
#define USART_PC_STARTUP    	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE); RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOC, ENABLE)
#endif


#define USART_PC_STOPSEND			GPIO_WriteBit(USART_PC_DIR_PORT,USART_PC_DIR_PIN,Bit_RESET);

#define USART_PC_STARTSEND			GPIO_WriteBit(USART_PC_DIR_PORT,USART_PC_DIR_PIN,Bit_SET);
#define USART_PC_SET9BIT(Val)	(Val |=0x0100)
#define USART_PC_CLR9BIT(Val)	(Val &=0x00ff)


static char PHASE_RS;
static int   RSTime;
static unsigned char    Head[10];
static int  ptrUART;
static uint8_t chSumUART;
static uint16_t  fSendByte;
static uint16_t fAdrSend;
static uint8_t  *pSostRS485;
static eAdrGD   *pADRGD;
static const uint8_t  *pNFCtr;
static uint8_t *pNumBlock;
static uint8_t *pDataRS;

/**********************************************************************************************************/
/*            STM32F10x Прерывание, обрабатывающее запросы последовательного порта                        */
/**********************************************************************************************************/


void CheckRSTime()
{
    RSTime--;
    if (RSTime<0)   RSTime=0;
    if ((PHASE_RS>RS_START)&&(!RSTime))
    {
        PHASE_RS=RS_START;
        USART_PC_STOPSEND;
    }

}


USART_PC_INT_VECT
{
    uint16_t retByte;
    //uint16_t i;

    if ((USART_PC->SR & USART_FLAG_RXNE)!=0)
    {
//			USART_PC->SR=0;
        retByte = USART_ReceiveData(USART_PC);
//			if (retByte&0x100) PHASE_RS=RS_START;
        switch (PHASE_RS)
        {
        case RS_START:
            if (retByte!=(*pNFCtr|0x100)) return;
            *pSostRS485=WORK_UNIT;
            USART_PC_STARTSEND;
            PHASE_RS=RS_HEAD;
            ptrUART=0;
            RSTime=MAX_RS_TIME;
            USART_PC_SET9BIT(retByte);
            USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);
            USART_SendData(USART_PC,retByte);
            chSumUART=0;
            return;
        case RS_HEAD:
            Head[ptrUART]=retByte;
            chSumUART+=retByte;
            ptrUART++;
            if (ptrUART == HEAD_SIZE)
            {
                USART_PC_STARTSEND;
                fSendByte=Head[2]+Head[3]*256;
                fAdrSend=Head[0]+Head[1]*256;
                *pNumBlock=Head[4]&0x0f;
                retByte=chSumUART;
                if ((Head[4]&0xf0) == OUT_UNIT)
                {
                    chSumUART=0;
                    pDataRS=pADRGD[*pNumBlock].Adr+fAdrSend;
                    ptrUART=fSendByte;
//							USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);
                    PHASE_RS=RS_SEND;
                    *pSostRS485=OUT_UNIT;
                }
                else if ((Head[4]&0xf0) == IN_UNIT)
                {
                    PHASE_RS=RS_RECV;
                    ptrUART=0;
                    chSumUART=0;
                }
                else
                {
                    PHASE_RS=RS_START;
                    USART_PC_STOPSEND;
                    return;
                }
//						USART_PC_CLR9BIT(retByte);
                USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);
                USART_SendData(USART_PC,retByte);
            }
            return;
        case RS_RECV:
            {
                chSumUART+=retByte;
                if (ptrUART == fSendByte)
                {
                    if (chSumUART == 55)
                    {
                        *pSostRS485=IN_UNIT;
                    }
                    else
                    {
                        *pSostRS485=OUT_UNIT;
                        //Sound;
                    }
                    USART_PC_STARTSEND;
                    retByte=chSumUART;
                    ptrUART=0;
                    USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);
                    USART_SendData(USART_PC,retByte);
                    PHASE_RS=RS_START;
                    break;
                }
                *(uint8_t*)(pADRGD[*pNumBlock].Adr+fAdrSend+ptrUART)=retByte;
                ptrUART++;
                return;
            }
        default:
            {
                //PHASE_RS=RS_START;

                //*pSostRS485=OUT_UNIT;
            }

        }
    }
    if ((USART_PC->SR & USART_FLAG_TXE)!=0)
    {
        if (ptrUART>0)
        {
            USART_SendData(USART_PC,*pDataRS);
            chSumUART+=*pDataRS;
            pDataRS++;
            ptrUART--;
            return;
        }
        if ((USART_PC->SR & USART_FLAG_TC)!=0)
        {
            USART_ITConfig(USART_PC, USART_IT_TXE, DISABLE);
            USART_PC->SR=0;
            if (PHASE_RS<RS_SEND)
            {
                USART_PC_STOPSEND;
                return;
            }
            PHASE_RS=RS_START;
            ptrUART=0;
            USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);
            retByte=chSumUART;
            USART_SendData(USART_PC,retByte);
            return;
        }

    }
    //USART_ITConfig(USART_PC, USART_IT_TXE, DISABLE);
    //USART_PC->SR=0;

//	USART_PC->SR=0;
}


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USART_PC_Configuration(const uint8_t *fNFCtr,eAdrGD* fADRGD,uint8_t* fSostRS,uint8_t* fNumBlock,uint16_t fbrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    USART_PC_STARTUP;

    pSostRS485=fSostRS;
    pADRGD=fADRGD;
    pNFCtr=fNFCtr;
    pNumBlock=fNumBlock;

    /* Enable the USARTx Interrupt */
    NVIC_SetPriority(USART_PC_IRQ, 5);
    NVIC_ClearPendingIRQ(USART_PC_IRQ);
    NVIC_EnableIRQ(USART_PC_IRQ);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_PC_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_PC_TX_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART_PC_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(USART_PC_DIR_PORT, &GPIO_InitStructure);


    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_PC_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_PC_RX_PORT, &GPIO_InitStructure);


/* USART1 configuration ------------------------------------------------------*/
    /* USART1 configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
          - USART Clock disabled
          - USART CPOL: Clock is active low
          - USART CPHA: Data is captured on the middle
          - USART LastBit: The clock pulse of the last data bit is not output to
                           the SCLK pin
    */

    USART_InitStructure.USART_BaudRate = fbrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART_PC, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART_PC, ENABLE);

    USART_ITConfig(USART_PC, USART_IT_RXNE, ENABLE);
//  USART_ITConfig(USART_PC, USART_IT_TC, ENABLE);
//  USART_ITConfig(USART_PC, USART_IT_TXE, ENABLE);

    USART_PC_STOPSEND;
    PHASE_RS=RS_START;

}




