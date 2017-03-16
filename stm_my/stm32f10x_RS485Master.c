
// XXX: WTF ?
#pragma pack(1)

#include "syntax.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_Define.h"
#include "stm32f10x_RS485Master.h"
#include "misc.h"

#warning "rollback this file after the investigation"

#include "control_gd.h"

#include "debug.h"

void usart_master_stopsend(void)
{
	GPIO_WriteBit(USART_OUT_DIR_PORT,USART_OUT_DIR_PIN,Bit_RESET);
}

void usart_master_startsend(void)
{
    // unworking delay here
    for(uint i=0;i<500;i++);

    GPIO_WriteBit(USART_OUT_DIR_PORT,USART_OUT_DIR_PIN,Bit_SET);
}


#define HEAD_SIZE				5

#define MODULE_IS_OK			0


#define MAX_IN_SENS		32
#define MAX_OUT_RELS	32
#define MAX_OUT_REG		8


#define ACTION_PULL_COND                                        0
#define ACTION_CHECK_STATUS_AND_OPTIONALLY_PUSH_INPUT_CONFIG    1
#define ACTION_PUSH_OUT_VALUES                                  2
#define ACTION_PUSH_OUT_CONFIG                                  3
#define ACTION_PULL_INPUTS                                      4
#define ACTION_PUSH_OUT_REGS                                    5
#define ACTION_PUSH_FANDATA                                     6
#define ACTION_TOUCH_UART2                                      7

typedef struct
{
    uint32_t    Type;
    uint8_t     Pulse[MAX_OUT_RELS][2];
}TOModulConfig;

typedef struct
{
    uint8_t     Type;
    uint16_t    Value;
}TOModuleRegister;


// guesswork is following:

typedef struct
{
    uint8_t     Cond;                   // some conditions ?
    uint16_t    CpM;
    uint8_t     MaxIn;                  // max number of used inputs
    uint8_t     MaxOut;                 // max number of used outputs // Максимальный номер импульсного регулятора,используемого в модуле
    uint32_t    OutValues;
    TOModuleRegister  OutReg[MAX_OUT_REG];    // outputs registers
    TOModulConfig OutConfig;
    uint16_t    InValues[MAX_IN_SENS];      // input values
    TIModulConf InConfig[MAX_IN_SENS];      // input configs
    char        Err;                        // number of errors ?
    char        Failures;                   // number of failures ?
    eFanData*   fandata;
} TModuleData;

static TModuleData ModulData[OUT_MODUL_SUM];


static u8    HeadOUT[10];
static uint8_t  ReadBuf[1000];

static char phase_rs_out;
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

#define MAX_RSOUT_TIME					5 //10*33mil sec=330mil sec 9600bod is 1.2kbyte/sec  per 100 byte



void USART_OUT_Configuration(uint16_t fbrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

    NVIC_SetPriority(USART_OUT_IRQ, 5);
    NVIC_ClearPendingIRQ(USART_OUT_IRQ);
    NVIC_EnableIRQ(USART_OUT_IRQ);


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

    usart_master_stopsend();

    phase_rs_out=RSOUT_INIT;

}


// GUESS:
// lame fsm is like that:
//
// addressing:
// 1) send module address
// 2) receive some ack (module address in 8 msbits)
// 3) send header (4 bytes ??? 5 bytes ???)
// 4) receive byte of (header ?) checksum
// if push:
// 5) send data
// 6) send data checksum minus 55
// 7) receive checksum (55)
// if pull:
// 5) receive data (this bit is time-critical ?)
// 6) receive checksum
//
// 1kb buffer should be enough
//

void UART4_IRQHandler(void)
{
    uint16_t rxbyte;

    // received something
    if ((USART_OUT->SR & USART_FLAG_RXNE)!=0)
    {

        rxbyte = USART_ReceiveData(USART_OUT);


        switch (phase_rs_out)
        {
        case RSOUT_HEAD:

            // some check
            if ((rxbyte&0xff)!=GLCtr) return;


            usart_master_startsend();

            ptrUARTOUT=5-1;
            pDataRSOUT=HeadOUT;
            chSumUARTOUT=*pDataRSOUT;
            USART_ITConfig(USART_OUT, USART_IT_TXE, ENABLE);
            USART_SendData(USART_OUT,*pDataRSOUT);
            pDataRSOUT++;
            return;
        case RSOUT_CHK:
            if (rxbyte!=chSumUARTOUT)
            {
                (*GLCond) |= ERR_MODULE_CHKSUM;
                phase_rs_out=RSOUT_INIT;
                return;
            }
            if (GLDir == OUT_UNIT)
            {
                chSumUARTOUT=0;
                phase_rs_out=RSOUT_RECV;
                ptrUARTOUT=0;
                pDataRSOUT=ReadBuf;
            }
            else
            {

                phase_rs_out=RSOUT_SEND;
                pDataRSOUT=ReadBuf;
                ptrUARTOUT=GLSize-1;
                //for (i=0;i<100;i++);
                usart_master_startsend();
                chSumUARTOUT=*pDataRSOUT;

                USART_ITConfig(USART_OUT, USART_IT_TXE, ENABLE);
                USART_SendData(USART_OUT,*pDataRSOUT);
                pDataRSOUT++;
            }
            return;
        case RSOUT_RECV:
            {
                if (ptrUARTOUT == GLSize)
                {
                    if (rxbyte == chSumUARTOUT)
                    {
                        memcpy(GLData, ReadBuf, GLSize);
                        //for(i=0;i<GLSize;i++)
                        //GLData[i]=0x0f;
                        if (GLF)
                            GLF();
                        (*GLCond) &= ERR_MASK_CLEARED;
                    }
                    else
                        (*GLCond) |= ERR_MODULE_CHKSUM;

                    phase_rs_out=RSOUT_INIT;
                    return;
                }
                *pDataRSOUT=rxbyte;
                pDataRSOUT++;
                ptrUARTOUT++;
                chSumUARTOUT+=rxbyte;
                return;
            }
        case RSOUT_SENDCHK:
            {
                if (rxbyte!=55)
                    (*GLCond) |= ERR_MODULE_CHKSUM;
                else
                {
                    (*GLCond) &= ERR_MASK_CLEARED;
                    if (GLF)
                        GLF();
                }
                //Sound;
                phase_rs_out=RSOUT_INIT;
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

    if (USART_OUT->SR & USART_FLAG_TXE)
    {
        // more data wanted

        if (ptrUARTOUT>0)
        {
            // had data to send, send it
            USART_SendData(USART_OUT,*pDataRSOUT);
            chSumUARTOUT+=*pDataRSOUT;
            pDataRSOUT++;
            ptrUARTOUT--;
            return;
        }

        // ... otherwise stop nagging
        USART_ITConfig(USART_OUT, USART_IT_TXE, DISABLE);
    }

    if (USART_OUT->SR & USART_FLAG_TC)
    {
        // transmission complete, shift register empty

        //USART_ITConfig(USART_OUT, USART_IT_TXE, DISABLE);
        USART_ClearITPendingBit(USART_OUT,USART_IT_TC);

        // sent start or send header ?
        if (phase_rs_out == RSOUT_START || phase_rs_out == RSOUT_HEAD)
        {
            // switch to reception
            usart_master_stopsend();

            // start -> header, header -> chk
            if (phase_rs_out == RSOUT_START)
                phase_rs_out = RSOUT_HEAD;
            else
                phase_rs_out = RSOUT_CHK;
        }

        if ((phase_rs_out == RSOUT_SENDCHK))
        {
            // switch to reception, expect some reply checksum ?
            usart_master_stopsend();
        }

        if ((phase_rs_out == RSOUT_SEND))
        {
            // sent -> send checksum

            phase_rs_out = RSOUT_SENDCHK;
            chSumUARTOUT=55-chSumUARTOUT;
            rxbyte=chSumUARTOUT;
            USART_SendData(USART_OUT,rxbyte);
        }
        return;

    }
}


// guesswork:
// module is addressed with (number of module | 0x100)
// header contains at least 5 bytes:
// two bytes of register (or memory) address,
// two bytes of data size
// one byte of direction magic (0x50: out, 0xa0: in, 0x70: work (wtf?))
//

uint8_t RS485_Master_ExchangeDataIRQ(uint8_t fNCtr, uint16_t fAdrSend, uint16_t fNBytes, void* fData, uint8_t fNBlock, uint8_t Dir,uint8_t *fCond, CallBackRS pF)
{
    uint16_t    txbyte;

    txbyte=fNCtr|0x100;

    if (!fNBytes) return 0;

    HeadOUT[0]=fAdrSend;
    HeadOUT[1]=fAdrSend >> 8;
    HeadOUT[2]=fNBytes;
    HeadOUT[3]=fNBytes >> 8;
    HeadOUT[4]=Dir + fNBlock;

    GLData=fData;
    GLSize=fNBytes;
    GLCtr=fNCtr;
    GLCond=fCond;
    GLF=pF;

    RSOutTime=((MAX_RSOUT_TIME*GLSize)/100)+2;

    usart_master_startsend();

    //(*GLCond) &= 0x80;

    memcpy(ReadBuf, GLData, GLSize);

    GLDir=Dir;
    ptrUARTOUT=0;
    phase_rs_out=RSOUT_START;

    USART_SendData(USART_OUT, txbyte);

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
    if (vCpM/100 == 6) return;

    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM)
            ModulData[i].CpM=vCpM;
        if (vCpM  ==  ModulData[i].CpM)
        {
            bOut=1;
            bOut <<= GetIPCNum(nAddress)-1;

            *nErr=ModulData[i].Err;
            if (How)
                ModulData[i].OutValues |= bOut;
            else
                ModulData[i].OutValues &= ~(bOut);
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
    if (vCpM/100 == 6) return;

    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM  ==  ModulData[i].CpM)
        {
            bOut=GetIPCNum(nAddress)-1;
            if (bOut>=MAX_OUT_REG)
                return;
            *nErr=ModulData[i].Err;
            ModulData[i].OutReg[bOut].Value=How;
            ModulData[i].OutReg[bOut].Type= fType;
            ModulData[i].fandata=Ptr;
            return;
        }
    }
}


char GetOutIPCDigit(uint16_t nAddress, char* nErr)
{
    uint32_t vCpM,bIn,i;
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return -1;
    if (vCpM/100 == 6) return 1;
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
//		if(!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM  ==  ModulData[i].CpM)
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
    uint16_t vCpM, i, vInput;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1; return 0;
    }
    if (vCpM/100 == 6)
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
        if (!ModulData[i].CpM)
            ModulData[i].CpM=vCpM;
        if (vCpM  ==  ModulData[i].CpM)
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
    uint16_t vCpM,i,vInput;
//TODO
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1; return 0;
    }
    if (vCpM/100 == 6)
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
        if (vCpM  ==  ModulData[i].CpM)
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
    uint16_t vCpM,i,k,vInput;
    vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return;
    if (vCpM/100 == 6) return;
    vInput=GetIPCNum(nAddress);
    if (!vInput) return;
    for (i=0; i< OUT_MODUL_SUM; i++)
    {
        if (!ModulData[i].CpM) ModulData[i].CpM=vCpM;
        if (vCpM  ==  ModulData[i].CpM)
        {
//			if ((NoSameBuf(((char*)(&ModulData[i].InConfig[vInput-1]))+2,((char*)ModulConf)+2,2/*sizeof(TIModulConf)-2*/)) //без калибровок
//				||(ModulData[i].InConfig[vInput-1].Type!=ModulConf->Type))
            {
                ModulData[i].Cond |= NEED_MODULE_RESET;
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
    (*GLCond) &= ~(NEED_MODULE_RESET+ERR_MODULE_RESET);
}

uint8_t IMOD_Exchange(TModuleData*   fModule)
{
    uint8_t nModule;
    nModule=fModule->CpM%100+120;
    if (fModule->MaxOut)
        nModule=fModule->CpM%100+140;


    // GUESS: defined operations:
    // 0: read status (condition)
    // 1: reset module if status is magic, push inputs config, jump to 4
    // 2: push 4 bytes of output values (bitmap ?)
    // 3: push output config
    // 4: pull input values (size = ???)
    // 5: push output registers
    // 6: push fan data (they are special ?)
    switch (cOperInModule)
    {
    case ACTION_PULL_COND:
        StatusByte=0;
        return RS485_Master_ReadDataIRQ(nModule,0,2,&StatusByte,0,&fModule->Cond,0);
    case ACTION_CHECK_STATUS_AND_OPTIONALLY_PUSH_INPUT_CONFIG:
        if (StatusByte&0x01)
        {
            fModule->Cond |= ERR_MODULE_RESET;
        }
        if (fModule->Cond&(NEED_MODULE_RESET | ERR_MODULE_RESET))
        {
            fModule->Failures++;
            cOperInModule=4;
            return RS485_Master_WriteDataIRQ(nModule,0,sizeof(fModule->InConfig),&fModule->InConfig,2,&fModule->Cond,&ResMod);
        }
        cOperInModule = ACTION_PUSH_OUT_VALUES;
    case ACTION_PUSH_OUT_VALUES:
        //fModule->OutValues=0x0f0f0f;
        return RS485_Master_WriteDataIRQ(nModule,0,4,&fModule->OutValues,3,&fModule->Cond,0);
    case ACTION_PUSH_OUT_CONFIG:
        return RS485_Master_WriteDataIRQ(nModule,4,sizeof(fModule->OutConfig),&fModule->OutConfig,3,&fModule->Cond,0);
    case ACTION_PULL_INPUTS:
        return RS485_Master_ReadDataIRQ(nModule,2,sizeof(uint16_t)*fModule->MaxIn,&fModule->InValues[0],0,&fModule->Cond,0);
    case ACTION_PUSH_OUT_REGS:
        return RS485_Master_WriteDataIRQ(nModule,0,sizeof(TOModuleRegister)*MAX_OUT_REG,&fModule->OutReg[0].Type,8,&fModule->Cond,0);
    case ACTION_PUSH_FANDATA:
        if (fModule->fandata)
            return RS485_Master_ReadDataIRQ(nModule,3+sizeof(eFanData)*ncFan,sizeof(eFanData),&fModule->fandata[ncFan],9,&fModule->Cond,0);


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
    if (phase_rs_out!=RSOUT_INIT)
    {
        if (!RSOutTime)
        {
            ModulData[cModule].Cond |= ERR_MODULE_LINK;
            phase_rs_out=RSOUT_INIT;
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
    if ((cOperInModule == ACTION_PUSH_OUT_CONFIG)&&(!ModulData[cModule].MaxOut))
        cOperInModule = ACTION_PULL_INPUTS;
    if ((cOperInModule == ACTION_PULL_INPUTS)&&(cCycle!=cModule%10))
        cOperInModule = ACTION_PUSH_OUT_REGS;
    if ((cOperInModule == ACTION_PUSH_OUT_REGS)&&(!ModulData[cModule].OutReg[0].Type))
        cOperInModule = ACTION_PUSH_FANDATA;
    if ((cOperInModule == ACTION_PUSH_FANDATA)&&(!ModulData[cModule].fandata))
        cOperInModule = ACTION_TOUCH_UART2;
    if (cOperInModule == ACTION_TOUCH_UART2)
    {
        StatusByte=0;
        cModule++;
        cOperInModule= ACTION_PULL_COND;
        if (cCycle != cModule%10)
            cOperInModule= ACTION_PUSH_OUT_VALUES;
    }

    cModule%=OUT_MODUL_SUM;
    if (!ModulData[cModule].CpM)
    {
        cModule=0;
        cCycle++;
        ncFan++;
        cOperInModule = ACTION_PULL_COND;
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
/*	if ((ModulData[cModule].CpM/100) == 6)
    {
        ModulData[cModule].Err=0;
        cModule++;
        return;
    }*/
    if (ModulData[cModule].CpM/100)
    {
        WARN("attempt to send something over uart2. ignored");
        ModulData[cModule].Cond=0;
        //RS485_Out2_Transmit(120+ModulData[cModule].CpM%100,ModulData[cModule].OutValues);
        cOperInModule= ACTION_TOUCH_UART2;
        return;
    }

    if (IMOD_Exchange(&ModulData[cModule]) == 0)
        cOperInModule++;
}
