#ifndef __STM32F10X_LCD240X64_H
#define	__STM32F10X_LCD240X64_H

#define PORT_IND1	GPIOB
#define PORT_IND2	GPIOB

#define CE_ind				GPIO_Pin_8
#define Cmd_Dat 			GPIO_Pin_9
#define PORT_IND_CMD_CE		GPIOC
#define WR_ind				GPIO_Pin_6
#define RD_ind				GPIO_Pin_7
#define PORT_IND_WR_RD		GPIOC

#define MASK_IND1	0x003c
#define N_IND1		2
#define MASK_IND2	0xf000
#define N_IND2		8

#ifdef DEBUG
#define LCD_STARTUP //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#else
#define LCD_STARTUP RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif


#define	SumExtCG	96
#define	SUM_LINE_DISP	8

#define PozTime			23

#define MaxTimeOut		10	 //5
#define	DisplCols		40
#define TxtHomeAddr		0x0000
#define TxtArea			DisplCols		//40 колонок
#define GrfHomeAddr		0x0200
#define GrfArea			DisplCols		//40 колонок


#define BufSize			240	//160


void SendFirstScreen(char tmSec);
void ClearGraf(void);
void clear_d(void);
void InitLCD(void);
void Video(void);


#endif

