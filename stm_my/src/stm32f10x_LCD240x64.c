#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_LCD240x64.h"
#include "stm32f10x_i2c.h"

#define SetCE	PORT_IND_CMD_CE->BSRR=CE_ind // GPIO_WriteBit(PORT_IND_CMD_CE, CE_ind, Bit_SET)
#define ClrCE	PORT_IND_CMD_CE->BRR=CE_ind //GPIO_WriteBit(PORT_IND_CMD_CE, CE_ind, Bit_RESET)

#define SetCMD	PORT_IND_CMD_CE->BSRR=Cmd_Dat // GPIO_WriteBit(PORT_IND_CMD_CE, Cmd_Dat, Bit_SET)
#define ClrCMD	PORT_IND_CMD_CE->BRR=Cmd_Dat //GPIO_WriteBit(PORT_IND_CMD_CE, Cmd_Dat, Bit_RESET)

#define SetRD	PORT_IND_WR_RD->BSRR=RD_ind //GPIO_WriteBit(PORT_IND_WR_RD, RD_ind, Bit_SET)
#define ClrRD	PORT_IND_WR_RD->BRR=RD_ind //GPIO_WriteBit(PORT_IND_WR_RD, RD_ind, Bit_RESET)

#define SetWR	PORT_IND_WR_RD->BSRR=WR_ind //GPIO_WriteBit(PORT_IND_WR_RD, WR_ind, Bit_SET)
#define ClrWR	PORT_IND_WR_RD->BRR=WR_ind //GPIO_WriteBit(PORT_IND_WR_RD, WR_ind, Bit_RESET)

#define Tire	0xAC	//0x8D
#define DTire	0x82



#define cmdTxtHome		0x40
#define cmdTxtArea		0x41
#define cmdGrfHome		0x42
#define cmdGrfArea		0x43
#define cmdPozCurs		0x21
#define cmdOffSet		0x22
#define cmdAddrPtr		0x24
#define cmd2LineCurs	0xA1
#define cmd3LineCurs	0xA2
#define cmd8LineCurs	0xA7
#define cmdOnAutoWr		0xB0
#define cmdOffAuto		0xB2
#define cmdModeOR		0x80
#define cmdModeEXOR		0x81
#define cmdTxtOnGrafOn	0x9F
#define cmdTxtOffGrafOn 0x98
#define cmdTxtOnGrafOff 0x97

#define OffSet			0x02
#define CGHomeAddr		0x1400


/***************************************************************************//**
 * @brief  Setting CONTROLS pins to output mode
 ******************************************************************************/
void LCD_CTRL_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = CE_ind | Cmd_Dat;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT_IND_CMD_CE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = WR_ind | RD_ind;
    GPIO_Init(PORT_IND_WR_RD, &GPIO_InitStructure);

}


/***************************************************************************//**
 * @brief  Setting DATA pins to input mode
 ******************************************************************************/
void LCD_DATA_DIR_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = MASK_IND1;
    GPIO_Init(PORT_IND1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MASK_IND2;
    GPIO_Init(PORT_IND2, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Setting DATA pins to output mode
 ******************************************************************************/
void LCD_DATA_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MASK_IND1;
    GPIO_Init(PORT_IND1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MASK_IND2;
    GPIO_Init(PORT_IND2, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Reading DATA pins
 * @return the data value.
 ******************************************************************************/
unsigned char LCD_DATA_IN(void)
{
    //uint16_t u16Temp;
    //u16Temp = GPIO_ReadInputData(PORT_IND1)&MASK_IND1;
    //u16Temp>>=N_IND1;
    //u16Temp|= ((GPIO_ReadInputData(PORT_IND2)&MASK_IND2)>>N_IND2);
    //u8Tmp=u16Temp;
    return ((PORT_IND1->IDR&MASK_IND1)>>N_IND1)|((PORT_IND2->IDR&MASK_IND2)>>N_IND2);//u8Tmp;//SWAP_DATA[u16Temp];
}

/***************************************************************************//**
 * @brief  Write DATA to LCD
 ******************************************************************************/
void LCD_DATA_OUT(uint8_t u8Tmp)
{
/*	uint16_t u16Temp=0;
    u16Temp = GPIO_ReadOutputData(PORT_IND1)&(~MASK_IND1);
    u16Temp |=  (u8Tmp&0x0f)<<N_IND1;//Внимательно если маска не 0xff нужно преобразование
    GPIO_Write(PORT_IND1, u16Temp);
    u16Temp = GPIO_ReadOutputData(PORT_IND2)&(~MASK_IND2);
    u16Temp |=  (u8Tmp&0xf0)<<N_IND2;//Внимательно если маска не 0xff нужно преобразование
    GPIO_Write(PORT_IND2, u16Temp);*/
	PORT_IND1->ODR=(PORT_IND1->IDR&(~MASK_IND1))|((u8Tmp&0x0f)<<N_IND1);
	PORT_IND2->ODR=(PORT_IND2->IDR&(~MASK_IND2))|((u8Tmp&0xf0)<<N_IND2);

}



/***************************************************************************//**
 * @brief Delay some time
 ******************************************************************************/
void Delay(vu32 nCount)
{
    for(; nCount != 0; nCount--);
}



uint8_t  ReadStatus (char Need){
uint8_t res, TimeOut;
res=0;
TimeOut=0;
while (1) {

//	CLREA;
	LCD_DATA_DIR_IN();
	SetCMD;
	ClrRD;				//Разрешение дисплея
	ClrCE;
	res=LCD_DATA_IN();		//Прием статуса из дисплея
	SetRD;
	SetCE;
	LCD_DATA_DIR_OUT();
//    SETEA;

	if ((res & Need) ==Need) return 1;
	TimeOut++;
	if (TimeOut > MaxTimeOut) {/*not=100;ton=6;*/
		return 0;
	}
	}
}

void SendCmd(uchar Cmd){
	ReadStatus(3);

//    CLREA;
	LCD_DATA_OUT(Cmd);
	SetCMD;
	ClrCE;				//Разрешение записи в дисплей
	ClrWR;
	Delay(1);
	SetWR;
	SetCE;				//Запрещение записи в дисплей
//    SETEA;

}

void SendByte(uchar Data){
	ReadStatus(3);
//    CLREA;
	LCD_DATA_OUT(Data);
	ClrCMD;
	ClrCE;				//Разрешение записи в дисплей
	ClrWR;
	Delay(1);
	SetWR;
	SetCE;				//Запрещение записи в дисплей
//    SETEA;

}

void Send2(uchar Cmd, int Data){
//	uchar snd;
	//snd=(uchar)(Data%256);
	SendByte(Data%256);
	//snd=(uchar)(Data/256);
	SendByte(Data/256);
	SendCmd(Cmd);
}

void SendBlock(char *Src, int Dst, int Size){
char ch;
	Send2(cmdAddrPtr,Dst);
	SendCmd(cmdOnAutoWr);
	while (Size--) {
		ch=(*Src) - 0x20;
		Src++;
		ReadStatus(8);
//		CLREA;
		LCD_DATA_OUT(ch);
		ClrCMD;
		ClrCE;				//Разрешение записи в дисплей
		ClrWR;
		Delay(1);
		SetWR;

		SetCE;				//Запрещение записи в дисплей
//		SETEA;

		}
	SendCmd(cmdOffAuto);
}

void SendBlockPM(char *Src, int Dst, int Size){
char ch;
	Send2(cmdAddrPtr,Dst);
	SendCmd(cmdOnAutoWr);
	while (Size--) {
		ch=Src - 0x20;
		Src++;
		ReadStatus(8);
//		CLREA;
		LCD_DATA_OUT(ch);
		ClrCMD;
		ClrCE;				//Разрешение записи в дисплей
		ClrWR;
		Delay(1);
		SetWR;
		SetCE;				//Запрещение записи в дисплей
//		SETEA;
		}
	SendCmd(cmdOffAuto);
}

void SendSim(char vSim, char NumStr){
char vDisplCols;
	vDisplCols=DisplCols;
	Send2(cmdAddrPtr,TxtHomeAddr+NumStr*DisplCols);
	SendCmd(cmdOnAutoWr);
	while (vDisplCols--) {
		ReadStatus(8);
//		CLREA;
		LCD_DATA_OUT(vSim);
		ClrCMD;
		ClrCE;				//Разрешение записи в дисплей
		ClrWR;
		Delay(1);
		SetWR;
		SetCE;				//Запрещение записи в дисплей
//		SETEA;

		}
	SendCmd(cmdOffAuto);
}

//-------------- Инициализировать дисплей -------

void IniLCDMem(void) {
	Send2(cmdTxtHome,TxtHomeAddr);
	Send2(cmdGrfHome,GrfHomeAddr);
	Send2(cmdTxtArea,TxtArea);
	Send2(cmdGrfArea,GrfArea);
	SendCmd(cmdModeEXOR);
	Send2(cmdOffSet,OffSet);
	SendCmd(cmdTxtOnGrafOff);
//	SendCmd(cmdTxtOnGrafOn);

	SendSim(DTire,(SUM_LINE_DISP-2));	//=

	if (x_menu) SendSim(Tire,1);

	}
void InitLCD(void) {
    LCD_STARTUP;
    LCD_CTRL_DIR_OUT();
	SetWR;
	SetRD;
	SetCE;
	IniLCDMem();
	SendBlock(&(ExtCG[0]),CGHomeAddr,SumExtCG*8);
	SendSim(Tire,1);	//-
	SendSim(0x00,7);	//Пробел
}
/*---------------------------------------------------
        Очистка буфера
----------------------------------------------------*/
void clear_d(void) {
        unsigned char ic;
        BlkW=0;
        for (ic=0;ic<BufSize;ic++) buf[ic]=' ';
}
//---------------------------------------------------
//        Вывод буфера в индикатор
//---------------------------------------------------
void TimeToBuf(void) {
	BlkW=1;
	Ad_Buf=PozTime;
//    buf[Ad_Buf++]=' ';
	w_int(&CtrTime,SSdSS);
    buf[Ad_Buf++]=':';
    buf[Ad_Buf++]=Second/10+'0';
    buf[Ad_Buf++]=Second%10+'0';
	Ad_Buf++;
	w_int(&CtrData,DsMsY);
}
void VideoSost(void) {
int ic;

	BlkW=1;
    Ad_Buf=Str2d;
    for (ic=Str2d; ic < Str2; ic++) buf[ic]=' ';
   // Ad_Buf=Str4;
/*    if (ds18b20_ReadROM())
    	Ad_Buf+=2;
	w_int(&Buf1W[0],StStStS);
		buf[Ad_Buf++]='-';
	w_int(&Buf1W[4],StStStS);
    w_int(&SumAnswers,SSSS);*/
    pmInfoProg405();
	SendBlock(&buf[Str2d],TxtHomeAddr+DisplCols*(SUM_LINE_DISP-1),DisplCols);
//    pmInfoProg405();

//	w_txt(I2C1_Buffer_Tx);
//    w_txt(I2C1_Buffer_Rx);

//    w_int(&GlobData,SSSS);
    //w_txt()
	//SendBlock(&buf[Str2d],TxtHomeAddr+DisplCols*(SUM_LINE_DISP-1),DisplCols);
}

void Video(void) {
uchar	CurRow,CurCol;
  //  if (CheckKeyboardXMEGA()) return;
	if (GrafView) {
		GrafView--;
		if (!GrafView) ClearGraf();
		else return;
		}

	IniLCDMem();
	TimeToBuf();

//---- output CharSet ----------------

	SendBlock(&buf[0],TxtHomeAddr,DisplCols);
	SendBlock(&buf[Str2],TxtHomeAddr+DisplCols*2,DisplCols*(SUM_LINE_DISP-4));
    if(Menu) SendCmd(cmd8LineCurs);
       else  SendCmd(cmd3LineCurs);
//-- установить курсор --
	CurCol=(AdinB+Mark) % DisplCols;
	CurRow=(AdinB+Mark) / DisplCols;// + 2;
	Send2(cmdPozCurs,((int)CurRow * 256)+ CurCol); //0x0101);
    VideoSost();



}

void SendFirstScreen(char tmSec){
int i;
	if(!tmSec) return;
	GrafView=tmSec;
	SendCmd(cmdTxtOffGrafOn);
	Send2(cmdAddrPtr,GrfHomeAddr);
	SendCmd(cmdOnAutoWr);
	for(i=0; i < 40*64; i++) {
		ReadStatus(8);

//		CLREA;
		LCD_DATA_OUT(FirstScreen[i]);
		ClrCMD;
		ClrCE;				//Разрешение записи в дисплей
		ClrWR;
		Delay(1);
		SetWR;
		SetCE;				//Запрещение записи в дисплей
//    	SETEA;
		}
	SendCmd(cmdOffAuto);
}

void ClearGraf(void){
int i;
	InitLCD();
	Send2(cmdAddrPtr,GrfHomeAddr);
	SendCmd(cmdOnAutoWr);
//---- очистка графического экрана -------
	for(i=0; i < 40*64; i++) {
		ReadStatus(8);
//		CLREA;
		LCD_DATA_OUT(0);
		ClrCMD;
		ClrCE;				//Разрешение записи в дисплей
		ClrWR;
		Delay(1);
		SetWR;
		SetCE;				//Запрещение записи в дисплей
//    	SETEA;
		}
	SendCmd(cmdOffAuto);
}
