//;======= I2C Soft ========================================
//========================================================================
#include "stm32f10x_gpio.h"
#include <stm32f10x_tim.h>
#include "stm32f10x_rcc.h"
#include "misc.h"
//#include "DefTypes.h"
//*********************************************************************************************
//#define	GET_COLS
//TPortPin GPIO_Cols={GPIOA,GPIO_Pin_11|GPIO_Pin_11|GPIO_Pin_11|GPIO_Pin_11};

#define 	i2_Port 	GPIOB
#define 	i2_SCL_Pin		GPIO_Pin_10
// ####need 10
#define 	i2_SDA_Pin		GPIO_Pin_11
#define 	FM_ADR			0xA0
#define 	PCF8574	0x40
#define 	PCF8574A 0x70

void i2_fm_Init(void);

u8 fm_Read(u16 adr,u8* buf,u16 size,u8* csum);

u8 fm_Write(u16 adr,u8* buf,u16 size,u8* csum);
