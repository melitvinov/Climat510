//Стандартные константы для процессора STM32 в проект

#ifndef __STM32F10X_DEFINE_H
#define __STM32F10X_DEFINE_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_iwdg.h"

//#define code

//typedef uint16_t uint;
//typedef uint8_t uchar;
//typedef uint8_t bit;


#define ClrDog  IWDG_ReloadCounter();
#define SETEA	__enable_irq()
#define CLREA	__disable_irq()

#pragma pack(1)


/* Назначение битов регистра новостей - GD.SostPol.News */
#define bOperator       0x01
#define bEdit           0x02
#define bKlTest         0x04
#define bResRam         0x08
#define bReset          0x20
#define bWriEEP         0x80

#define msNewDay		8
#define msInitGD		16
#define msReset			2
#define msOverFlow		4
#define msCountAlarm	32

#define SetBit(Val,NBit)        (Val |= (NBit))
#define ClrBit(Val,NBit)        (Val &=~(NBit))
#define YesBit(Val,NBit)        (Val & (NBit))

//----- Прототипы функций
void CopyEEP(void);
void CalcEEPSum(void);
void GetRTC(void);
void SetRTC(void);

void ReWriteEEP(void);
void SetIdent(void);
void SetValve(char nValve);
void SetNewVal(char newVal);
void FindNewVal(void);
void SetOutReg24(void);
//void SetOutReg32(void);
void SetOutReg64(void);
void InitBoil(void);
void SetBoilValve(void);
void DoBoilVal(void);
void w_int (void *bu,char frmt);
void in_val (void);
// Прототипы функций
void InitBlockEEP(void);
void ButtonReset(void);
void InitRegRetEC(void);
void w_txt(const char *bu);
void pmInfo(void);
int16_t w1reset(void);
void w1_wr(void);
void w1_rd(void);
void CrcCalc(void);
void InitP1(void);
//void InitRAM(char iBlock);
void InitSiod(void);
void    SetDreinCount(void);



#endif
