
// XXX: WTF ?
#pragma pack(1)

#include "syntax.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_Define.h"
#include "stm32f10x_RS485Master.h"
#include "misc.h"

#include "65_gd.h"

#define USART_MASTER_RX     			fTimeout=10000; while((USART_GetFlagStatus(USART_OUT,USART_FLAG_RXNE)==RESET)&&(fTimeout)) fTimeout--;
#define USART_MASTER_TX     			fTimeout=10000; while(!(USART_GetFlagStatus(USART_OUT,USART_FLAG_TC))&&(fTimeout)) fTimeout--;
#define USART_MASTER_TXE     			fTimeout=10000; while(!(USART_GetFlagStatus(USART_OUT,USART_FLAG_TXE))&&(fTimeout)) fTimeout--;

#define USART_MASTER_STOPSEND			GPIO_WriteBit(USART_OUT_DIR_PORT,USART_OUT_DIR_PIN,Bit_RESET);
#define USART_MASTER_STARTSEND			for(i=0;i<500;i++); GPIO_WriteBit(USART_OUT_DIR_PORT,USART_OUT_DIR_PIN,Bit_SET);


#define USART_OUT2_RX     			fTimeout=10000; while((USART_GetFlagStatus(USART_OUT2,USART_FLAG_RXNE)==RESET)&&(fTimeout)) fTimeout--;
#define USART_OUT2_TX     			fTimeout=10000; while(!(USART_GetFlagStatus(USART_OUT2,USART_FLAG_TC))&&(fTimeout)) fTimeout--;
#define USART_OUT2_TXE     			fTimeout=10000; while(!(USART_GetFlagStatus(USART_OUT2,USART_FLAG_TXE))&&(fTimeout)) fTimeout--;

#define USART_OUT2_STOPSEND			GPIO_WriteBit(USART_OUT2_DIR_PORT,USART_OUT2_DIR_PIN,Bit_RESET);
#define USART_OUT2_STARTSEND		for(i=0;i<500;i++); GPIO_WriteBit(USART_OUT2_DIR_PORT,USART_OUT2_DIR_PIN,Bit_SET);


#define HEAD_SIZE				5
#define IDENT_SIZE				7

#define MODULE_IS_BUSY			1
#define MODULE_IS_OK			0


#define MAX_SUM_TRY				4

#define MAX_IN_SENS		32
#define MAX_OUT_RELS	32
#define MAX_OUT_REG		8

typedef struct
{
    uint32_t    Type;
    uint8_t     Pulse[MAX_OUT_RELS][2];
}TOModulConf;

typedef struct
{
    uint8_t     Type;
    uint16_t    Value;
}TOModulReg;


typedef struct
{
    uint8_t     Cond;
    uint16_t    CpM;
    uint8_t     MaxIn; //Максимальный номер входа, используемый в модуле
    uint8_t     MaxOut; //Максимальный номер импульсного регулятора,используемого в модуле
    uint32_t    OutValues;
    TOModulReg  OutReg[MAX_OUT_REG];
    TOModulConf OutConfig;
    uint16_t    InValues[MAX_IN_SENS];
    TIModulConf InConfig[MAX_IN_SENS];
    char        Err;
    char        Failures;
    eFanData*   DataPtr;
} TModulData;

static TModulData ModulData[OUT_MODUL_SUM];


static unsigned char   HeadOUT[10];
static uint8_t  ReadBuf[1000];
static char PHASE_RS_OUT;
static int   RSOutTime;
static int  ptrUARTOUT;
static uint8_t chSumUARTOUT;
//uint8_t	pSostRS485OUT;
static uint8_t *pDataRSOUT;
static uint16_t StatusByte;
static uint8_t  cOperInModule;
static uint8_t cCycle;
static uint8_t  ncFan;
static uint8_t  cModule;
static CallBackRS GLF;
static uint8_t bOutIPCBlock;


static uint8_t*    GLData;
static uint16_t    GLSize;
static uint16_t    GLCtr;
static uint8_t     GLDir;
static uint8_t*    GLCond;



#define MAX_IN_SENS		32
#define MAX_OUT_REGS	32


#define RSOUT_INIT					0
#define RSOUT_START					1
#define RSOUT_HEAD					2
#define RSOUT_CHK					3
#define RSOUT_RECV					4
#define RSOUT_SEND					5
#define RSOUT_SENDCHK				6


//#define IS_OK					5
//#define IS_FAILURE				6
//#define IS_WORK					7

#define MAX_RSOUT_TIME					5 //10*33mil sec=330mil sec 9600bod is 1.2kbyte/sec  per 100 byte










void CpyBuf(uint8_t *pp1,uint8_t *pp2, uint16_t size)
{
    int16_t i;
    for (i=0;i<size;i++)
        *(pp1++)=*(pp2++);

}

uint16_t NoSameBuf(uint8_t *pp1,uint8_t *pp2, uint16_t size)
{
    int16_t i;
    for (i=0;i<size;i++)
        if ((*(pp1++))!=(*(pp2++)))
            return i;
    return 0;

}

void USART_OUT2_Configuration(uint16_t fbrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    USART_OUT2_STARTUP;



    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_OUT2_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_OUT2_TX_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART_OUT2_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(USART_OUT2_DIR_PORT, &GPIO_InitStructure);


    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_OUT2_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_OUT2_RX_PORT, &GPIO_InitStructure);


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
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART_OUT2, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART_OUT2, ENABLE);


    USART_OUT2_STOPSEND;


}



void USART_OUT_Configuration(uint16_t fbrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    USART_OUT_STARTUP;

    NVIC_InitStructure.NVIC_IRQChannel = USART_OUT_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_OUT_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_OUT_TX_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART_OUT_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(USART_OUT_DIR_PORT, &GPIO_InitStructure);


    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_OUT_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_OUT_RX_PORT, &GPIO_InitStructure);


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

    USART_Init(USART_OUT, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART_OUT, ENABLE);

    USART_ITConfig(USART_OUT, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART_OUT, USART_IT_TC, ENABLE);

    USART_MASTER_STOPSEND;

    PHASE_RS_OUT=RSOUT_INIT;

}


USART_OUT_INT_VECT
{
    uint16_t retByte,i;
    //uint16_t i;

    if ((USART_OUT->SR & USART_FLAG_RXNE)!=0)
    {
//			USART_PC->SR=0;
        retByte = USART_ReceiveData(USART_OUT);
//			if (retByte&0x100) PHASE_RS_OUT=RSOUT_START;
        switch (PHASE_RS_OUT)
        {
        case RSOUT_HEAD:
            if ((retByte&0xff)!=GLCtr) return;
            //for (i=0;i<100;i++);
            USART_MASTER_STARTSEND;
            ptrUARTOUT=5-1;
            pDataRSOUT=HeadOUT;
            chSumUARTOUT=*pDataRSOUT;
            USART_ITConfig(USART_OUT, USART_IT_TXE, ENABLE);
            USART_SendData(USART_OUT,*pDataRSOUT);
            pDataRSOUT++;
            return;
        case RSOUT_CHK:
            if (retByte!=chSumUARTOUT)
            {
                (*GLCond)|=ERR_MODULE_CHKSUM;
                PHASE_RS_OUT=RSOUT_INIT;
                return;
            }
            if (GLDir==OUT_UNIT)
            {
                chSumUARTOUT=0;
                PHASE_RS_OUT=RSOUT_RECV;
                ptrUARTOUT=0;
                pDataRSOUT=ReadBuf;
            }
            else
            {

                PHASE_RS_OUT=RSOUT_SEND;
                pDataRSOUT=ReadBuf;
                ptrUARTOUT=GLSize-1;
                //for (i=0;i<100;i++);
                USART_MASTER_STARTSEND;
                chSumUARTOUT=*pDataRSOUT;

                USART_ITConfig(USART_OUT, USART_IT_TXE, ENABLE);
                USART_SendData(USART_OUT,*pDataRSOUT);
                pDataRSOUT++;
            }
            return;
        case RSOUT_RECV:
            {
                if (ptrUARTOUT==GLSize)
                {
                    if (retByte==chSumUARTOUT)
                    {
                        CpyBuf(GLData,ReadBuf,GLSize);
                        //for(i=0;i<GLSize;i++)
                        //GLData[i]=0x0f;
                        if (GLF)
                            GLF();
                        (*GLCond)&=ERR_MASK_CLEARED;
                    }
                    else
                        (*GLCond)|=ERR_MODULE_CHKSUM;

                    PHASE_RS_OUT=RSOUT_INIT;
                    return;
                }
                *pDataRSOUT=retByte;
                pDataRSOUT++;
                ptrUARTOUT++;
                chSumUARTOUT+=retByte;
                return;
            }
        case RSOUT_SENDCHK:
            {
                if (retByte!=55)
                    (*GLCond)|=ERR_MODULE_CHKSUM;
                else
                {
                    (*GLCond)&=ERR_MASK_CLEARED;
                    if (GLF)
                        GLF();
                }
                //Sound;
                PHASE_RS_OUT=RSOUT_INIT;
                break;
            }
        default:
            {

                //PHASE_RS_OUT=RSOUT_START;

                //*pSostRS485OUT=OUT_UNIT;
            }

        }
        return;
    }
    if ((USART_OUT->SR & USART_FLAG_TXE)!=0)
    {
        if (ptrUARTOUT>0)
        {
            //for (i=0;i<10;i++);
            USART_SendData(USART_OUT,*pDataRSOUT);
            chSumUARTOUT+=*pDataRSOUT;
            pDataRSOUT++;
            ptrUARTOUT--;
            return;
        }
        USART_ITConfig(USART_OUT, USART_IT_TXE, DISABLE);
    }
    if ((USART_OUT->SR & USART_FLAG_TC)!=0)
    {
        //USART_ITConfig(USART_OUT, USART_IT_TXE, DISABLE);
        USART_ClearITPendingBit(USART_OUT,USART_IT_TC);
        //if (PHASE_RS_OUT==RSOUT_INIT) {USART_MASTER_STOPSEND; return;}
        if ((PHASE_RS_OUT==RSOUT_START)||(PHASE_RS_OUT==RSOUT_HEAD))
        {
            //for (i=0;i<10;i++);
            USART_MASTER_STOPSEND;
            PHASE_RS_OUT++;
//				return;
        }
        if ((PHASE_RS_OUT==RSOUT_SENDCHK))
        {
            //for (i=0;i<10;i++);
            USART_MASTER_STOPSEND;
//				return;
        }
        if ((PHASE_RS_OUT==RSOUT_SEND))
        {
            //USART_ITConfig(USART_OUT, USART_IT_TXE, ENABLE);
            PHASE_RS_OUT++;
            chSumUARTOUT=55-chSumUARTOUT;
            retByte=chSumUARTOUT;
            USART_SendData(USART_OUT,retByte);
//				return;
        }
/*			else
            {
                PHASE_RS_OUT=RSOUT_INIT;
                ptrUARTOUT=0;
                USART_MASTER_STOPSEND;
            }*/
        return;

    }
    //USART_ITConfig(USART_PC, USART_IT_TXE, DISABLE);
    //USART_PC->SR=0;

//	USART_PC->SR=0;
}


uint8_t RS485_Master_ExchangeDataIRQ(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock, uint8_t Dir,uint8_t *fCond, CallBackRS pF)
{
    uint16_t    retByte,i;
    char* fPtr;
    retByte=fNCtr|0x100;
    if (!fNBytes) return 0;
    HeadOUT[2]=fNBytes%256;
    HeadOUT[3]=fNBytes/256;
    HeadOUT[0]=fAdrSend%256;
    HeadOUT[1]=fAdrSend/256;
    HeadOUT[4]=Dir+fNBlock;
    GLData=fData;
    GLSize=fNBytes;
    GLCtr=fNCtr;
    GLCond=fCond;
    GLF=pF;
    RSOutTime=((MAX_RSOUT_TIME*GLSize)/100)+2;
    USART_MASTER_STARTSEND;
    //(*GLCond)&=0x80;
    CpyBuf(ReadBuf,GLData,GLSize);
/*	for(i=0;i<GLSize;i++)
        ReadBuf[i]=GLData[i];*/
    GLDir=Dir;
    ptrUARTOUT=0;
    PHASE_RS_OUT=RSOUT_START;
    for (i=0;i<100;i++);

    USART_SendData(USART_OUT,retByte);
    return MODULE_IS_OK;
}

uint8_t RS485_Master_WriteDataIRQ(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock, uint8_t *fCond, CallBackRS pF)
{

    return RS485_Master_ExchangeDataIRQ(fNCtr,fAdrSend,fNBytes,fData,fNBlock,IN_UNIT,fCond,pF);
}

uint8_t RS485_Master_ReadDataIRQ(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock, uint8_t *fCond, CallBackRS pF)
{
    return RS485_Master_ExchangeDataIRQ(fNCtr,fAdrSend,fNBytes,fData,fNBlock,OUT_UNIT,fCond,pF);
}



int16_t RS485_Out2_Transmit(uint16_t fNCtr, uint32_t fSend)
{
    int32_t fTimeout,i;
    uint8_t chSumUARTOUT;
    uint16_t retByte;
    uint8_t Head2[10];
//	USART_MASTER_RX;
    retByte='!';
    USART_OUT2_STARTSEND;
    //USART_MASTER_TX;

    USART_SendData(USART_OUT2,retByte);
    USART_OUT2_TX;
    Head2[0]=fNCtr;
    Head2[1]=fSend%256;
    Head2[2]=(fSend>>8)%256;
    Head2[3]=(fSend>>16);
    Head2[4]=0x55;
    chSumUARTOUT=0;
    i=0;
    while (i<HEAD_SIZE)
    {
//		chSumUARTOUT+=Head2[i];
        USART_OUT2_TXE;
        if (!fTimeout)
        {
            USART_OUT2_STOPSEND;return ERR_MASTER_TXHEAD;
        }
        USART_SendData(USART_OUT2,Head2[i]);
//		for (i=0;i<200;i++);
        i++;
    }
    USART_OUT2_TX;
    USART_OUT2_STOPSEND;
    return -1;
}

/*
int16_t RS485_Master_WriteData(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock)
{
    int	fTimeout,i,j,Result;
    char* fPtr;
    uint8_t chSumUARTOUT;
    int16_t retByte;
    if (!fNBytes) return 0;
    //Result=0;
    for (j=0;j<MAX_SUM_TRY;j++)
    {
        retByte=RS485_Master_InitTransmit(fNCtr,fAdrSend,fNBytes, IN_UNIT+fNBlock,j);
        if (retByte>=0) {Result=retByte; continue;}
        chSumUARTOUT=0;
        fPtr=fData;
        for (i=0;i<500;i++);
        USART_MASTER_STARTSEND;
        i=0;
        while(i<fNBytes)
        {
            retByte=*(fPtr++);
            chSumUARTOUT+=retByte;
            USART_SendData(USART_OUT,retByte);
            USART_MASTER_TX;
            if (!fTimeout) {USART_MASTER_STOPSEND;Result=ERR_MASTER_TXDATA;continue;}
            i++;
        }
        chSumUARTOUT=55-chSumUARTOUT;
        USART_SendData(USART_OUT,chSumUARTOUT);
        USART_MASTER_TX;
        if (!fTimeout) {Result=ERR_MASTER_TXDATA;continue;}
        USART_MASTER_STOPSEND;
        USART_MASTER_RX;
        if (!fTimeout) {Result=ERR_MASTER_DATANOSUM; continue;}
        retByte=USART_ReceiveData(USART_OUT);
        if (retByte!=55) {Result=ERR_MASTER_DATAWRSUM; continue;}
        Result=0;
        break;
    }
    return Result;

}

int16_t RS485_Master_ReadData(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock)
{
    int	fTimeout,i,j,Result;
    char* fPtr;
    uint8_t chSumUARTOUT;
    int16_t retByte;
    if (!fNBytes) return 0;
    for (j=0;j<MAX_SUM_TRY;j++)
    {
        retByte=RS485_Master_InitTransmit(fNCtr,fAdrSend,fNBytes, OUT_UNIT+fNBlock,j);
        if (retByte>=0) {Result=retByte;continue;}
        i=0;
        chSumUARTOUT=0;
        fPtr=fData;
        while(i<fNBytes)
        {
            USART_MASTER_RX;
            if (!fTimeout) {Result=ERR_MASTER_RXDATA;continue;}
            retByte=USART_ReceiveData(USART_OUT);
            *(fPtr++)=retByte;
            chSumUARTOUT+=retByte;
            i++;
        }
        USART_MASTER_RX;
        if (!fTimeout) {Result=ERR_MASTER_DATANOSUM;continue;}
        retByte=USART_ReceiveData(USART_OUT);
        if (retByte!=chSumUARTOUT) {Result=ERR_MASTER_DATAWRSUM;continue;}
        Result=0;
        break;
    }
    return Result;
}


int16_t RS485_Master_ReadType(uint8_t fNCtr, uint8_t*  fIdent)
{
    return RS485_Master_ReadData(fNCtr,0,IDENT_SIZE,fIdent,1);
}
*/

uint16_t GetIPCComMod(uint16_t nAddress) {
    return nAddress/100;
}
uint16_t GetIPCNum(uint16_t nAddress) {
    return nAddress%100;
}




void ClrAllOutIPCDigit(void)
{
    int i;
    bOutIPCBlock=1;
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) return;
        ModulData[i].OutValues=0;
    }
}


void ResumeOutIPCDigit(void)
{
    bOutIPCBlock=0;
}

void SetOutIPCDigit(char How, uint16_t nAddress,char* nErr)
{
    uint32_t vCpM,bOut,i;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return;
    if (vCpM/100==6) return;

    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
            bOut=1;
            bOut <<= GetIPCNum(nAddress)-1;

            *nErr=ModulData[i].Err;
            if (How) ModulData[i].OutValues |= bOut;
            else ModulData[i].OutValues &= ~(bOut);
            return;
        }
    }
}


void SetOutIPCReg(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr,void* Ptr)
{
    uint32_t vCpM,bOut,i;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return;
    if (vCpM/100==6) return;

    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
            bOut=GetIPCNum(nAddress)-1;
            if (bOut>=MAX_OUT_REG)
                return;
            *nErr=ModulData[i].Err;
            ModulData[i].OutReg[bOut].Value=How;
            ModulData[i].OutReg[bOut].Type= fType;
            ModulData[i].DataPtr=Ptr;
            return;
        }
    }
}


char GetOutIPCDigit(uint16_t nAddress, char* nErr)
{
    uint32_t vCpM,bIn,i;
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return -1;
    if (vCpM/100==6) return 1;
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
//		if(!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
            bIn=1;
            bIn <<= GetIPCNum(nAddress)-1;
            *nErr=ModulData[i].Err;
            if (ModulData[i].OutValues & bIn) return 1;
            else return 0;
        }
    }
    return -1;
}


uint16_t GetInIPC(uint16_t nAddress,char* nErr)
{
    uint16_t vCpM,bOut,i,j,vInput;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1; return 0;
    }
    if (vCpM/100==6)
    {
        *nErr=0; return 0;
    }
    vInput=GetIPCNum(nAddress);
    if (!vInput)
    {
        *nErr=-1; return 0;
    }
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
            *nErr=ModulData[i].Err;
            return ModulData[i].InValues[vInput-1];
        }
    }
    *nErr=0;
    return 4444;
}


uint16_t GetDiskrIPC(uint16_t nAddress,char* nErr)
{
    uint16_t vCpM,bOut,i,j,vInput;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1; return 0;
    }
    if (vCpM/100==6)
    {
        *nErr=0; return 1;
    }
    vInput=GetIPCNum(nAddress);
    if (!vInput)
    {
        *nErr=-1; return 0;
    }
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
            *nErr=ModulData[i].Err;
            if ((ModulData[i].InValues[vInput-1]>2500)&&(ModulData[i].Err<iMODULE_MAX_ERR))
                return 1;
            else
                return 0;
        }
    }
    *nErr=0;
    return 0;
}



void UpdateInIPC(uint16_t nAddress,TIModulConf* ModulConf)
{
    uint16_t vCpM,i,j,k,vInput;
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return;
    if (vCpM/100==6) return;
    vInput=GetIPCNum(nAddress);
    if (!vInput) return;
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM == ModulData[i].CpM)
        {
//			if ((NoSameBuf(((char*)(&ModulData[i].InConfig[vInput-1]))+2,((char*)ModulConf)+2,2/*sizeof(TIModulConf)-2*/)) //без калибровок
//				||(ModulData[i].InConfig[vInput-1].Type!=ModulConf->Type))
            {
                ModulData[i].Cond|=NEED_MODULE_RESET;
                memcpy(&ModulData[i].InConfig[vInput-1],ModulConf,sizeof(TIModulConf));
            }
/*			for (j=0;j<sizeof(TIModulConf);j++)
            {
                if (((char*)(&ModulData[i].InConfig[vInput-1]))[j]!=((char*)(ModulConf))[j])
                {
                    ((char*)(&ModulData[i].InConfig[vInput-1]))[j]=((char*)(ModulConf))[j];
                }
            }*/
            //********************** НАДО УБРАТЬ *****************************
            for (k=0;k<32;k++)
            {

                //ModulData[i].InConfig[k].Type=3;
                ModulData[i].InConfig[k].Input=k+1;
            }
            //****************************************************************

            if (ModulData[i].MaxIn<vInput) ModulData[i].MaxIn=vInput;
        }
    }
}

/*int16_t IMOD_WriteOutput(char COMPort,int nModule, uint32_t Values)
{
    uint8_t fIdent[8];
    nModule+=120;
    if (RS485_Master_ReadType(nModule,&(fIdent[0]))) return ERR_MODULE_LINK;
    if(fIdent[4]!=120) return ERR_MODULE_TYPE;
    if (RS485_Master_WriteData(nModule,0,4,&Values,3)) return ERR_MODULE_LINK;
    return 0;
}
*/
void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn,uint16_t **fInputs)
{
    *fCpM=ModulData[nMod].CpM;
    *fErr=ModulData[nMod].Err;
    *fFail=ModulData[nMod].Failures;
    *fCond=ModulData[nMod].Cond;
    *fMaxIn=ModulData[nMod].MaxIn;
    *fInputs=ModulData[nMod].InValues;
}

void ResMod(void)
{
    (*GLCond)&=~(NEED_MODULE_RESET+ERR_MODULE_RESET);
}

uint8_t IMOD_Exchange(TModulData*   fModule)
{
    uint8_t fIdent[8];
    uint8_t nModule;
    uint8_t Res;
    nModule=fModule->CpM%100+120;
    if (fModule->MaxOut)
        nModule=fModule->CpM%100+140;


    switch (cOperInModule)
    {

    case 0:
        StatusByte=0;
        return RS485_Master_ReadDataIRQ(nModule,0,2,&StatusByte,0,&fModule->Cond,0);
    case 1:
        if (StatusByte&0x01)
        {
            fModule->Cond|=ERR_MODULE_RESET;
        }
        if (fModule->Cond&(NEED_MODULE_RESET+ERR_MODULE_RESET))
        {
            fModule->Failures++;
            cOperInModule=4;
            return RS485_Master_WriteDataIRQ(nModule,0,sizeof(fModule->InConfig),&fModule->InConfig,2,&fModule->Cond,&ResMod);
        }
        cOperInModule++;
    case 2:
        //fModule->OutValues=0x0f0f0f;
        return RS485_Master_WriteDataIRQ(nModule,0,4,&fModule->OutValues,3,&fModule->Cond,0);
    case 3:
        return RS485_Master_WriteDataIRQ(nModule,4,sizeof(fModule->OutConfig),&fModule->OutConfig,3,&fModule->Cond,0);
    case 4:
        return RS485_Master_ReadDataIRQ(nModule,2,sizeof(uint16_t)*fModule->MaxIn,&fModule->InValues[0],0,&fModule->Cond,0);
    case 5:
        return RS485_Master_WriteDataIRQ(nModule,0,sizeof(TOModulReg)*MAX_OUT_REG,&fModule->OutReg[0].Type,8,&fModule->Cond,0);
    case 6:
        if (fModule->DataPtr)
            return RS485_Master_ReadDataIRQ(nModule,3+sizeof(eFanData)*ncFan,sizeof(eFanData),&fModule->DataPtr[ncFan],9,&fModule->Cond,0);


    }
    return 0;
}


void SendIPC(uint8_t *fErrModule)
{
/*	if (ModulData[0].OutValues)
        ModulData[0].OutValues=0;
    else
        ModulData[0].OutValues=0x0f0f1f;
    RS485_Master_WriteDataIRQ(122,0,4,&ModulData[0].OutValues,3,&ModulData[0].Cond,0);
    return;*/
    if (bOutIPCBlock) return;
    if (PHASE_RS_OUT!=RSOUT_INIT)
    {
        if (!RSOutTime)
        {
            ModulData[cModule].Cond|=ERR_MODULE_LINK;
            PHASE_RS_OUT=RSOUT_INIT;
        }
        else
        {
            RSOutTime--;
            return;
        }
    }
    ncFan%=MAX_FAN_COUNT;
    cCycle%=10;
    cOperInModule%=8;
    if ((cOperInModule==3)&&(!ModulData[cModule].MaxOut))
        cOperInModule++;
    if ((cOperInModule==4)&&(cCycle!=cModule%10))
        cOperInModule++;
    if ((cOperInModule==5)&&(!ModulData[cModule].OutReg[0].Type))
        cOperInModule++;
    if ((cOperInModule==6)&&(!ModulData[cModule].DataPtr))
        cOperInModule++;
    if (cOperInModule==7)
    {
        StatusByte=0;
        cModule++;
        cOperInModule=0;
        if (cCycle!=cModule%10) cOperInModule=2;
    }

    cModule%=OUT_MODUL_SUM;
    if (!ModulData[cModule].CpM)
    {
        cModule=0;
        cCycle++;
        ncFan++;
        cOperInModule=0;
        if ((cCycle%10)!=cModule%10) cOperInModule=2;
        StatusByte=0;
        return;
    }
    if (ModulData[cModule].Cond)
        ModulData[cModule].Err++;
#ifndef DEBUG
    else
    {
        if (ModulData[cModule].Err)
        {
//####			ModulData[cModule].Failures++;
            ModulData[cModule].Err=0;
        }
    }
#endif
    if (ModulData[cModule].Failures>iMODULE_MAX_FAILURES)
    {
        ModulData[cModule].Failures=iMODULE_MAX_FAILURES;
    }

    if (ModulData[cModule].Err>iMODULE_MAX_ERR)
    {
        ModulData[cModule].Err=iMODULE_MAX_ERR;
        *fErrModule=ModulData[cModule].CpM%100;
    }
/*	if ((ModulData[cModule].CpM/100)==6)
    {
        ModulData[cModule].Err=0;
        cModule++;
        return;
    }*/
    if (ModulData[cModule].CpM/100)
    {
        ModulData[cModule].Cond=0;
        RS485_Out2_Transmit(120+ModulData[cModule].CpM%100,ModulData[cModule].OutValues);
        cOperInModule=7;
        return;
    }
    if (!IMOD_Exchange(&ModulData[cModule]))

        cOperInModule++;
}




