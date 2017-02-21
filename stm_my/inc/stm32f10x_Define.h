//Стандартные константы для процессора STM32 в проект

#ifndef __STM32F10X_DEFINE_H
#define __STM32F10X_DEFINE_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_iwdg.h"

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

#define SetBit(Val,NBit)        (Val |= (NBit))
#define ClrBit(Val,NBit)        (Val &=~(NBit))
#define YesBit(Val,NBit)        (Val & (NBit))

void GetRTC(void);
void SetRTC(void);

void w_int(void *bu, char frmt, int16_t param);
void in_val (void);
// Прототипы функций
void InitBlockEEP(void);
void ButtonReset(void);
void w_txt(const char *bu);
void CrcCalc(void);



#endif
