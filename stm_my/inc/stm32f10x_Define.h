//����������� ��������� ��� ���������� STM32 � ������

#ifndef __STM32F10X_DEFINE_H
#define __STM32F10X_DEFINE_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_iwdg.h"

#define SETEA	__enable_irq()
#define CLREA	__disable_irq()


/* ���������� ����� �������� �������� - GD.SostPol.News */
#define bOperator       0x01
#define bEdit           0x02
#define bKlTest         0x04
#define bResRam         0x08
#define bReset          0x20
#define bWriEEP         0x80


void CrcCalc(void);



#endif
