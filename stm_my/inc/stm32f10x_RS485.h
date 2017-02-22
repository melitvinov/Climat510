#include "simple_server.h"
//–абота порта RS-485

#ifndef __STM32F10X_RS485_H
#define __STM32F10X_RS485_H

//Ќомер используемого USART
#define USART_PC1				USART1
//#define USART_PC2				USART2
//#define USART_PC3				USART3
//#define USART_PC4				USART4





/*------------ ѕризнаки обмена данных --------*/
#define OUT_UNIT        0x50
#define IN_UNIT         0xa0
#define WORK_UNIT       0x70


void USART_PC_Configuration(const uint8_t *fNFCtr,eAdrGD* fADRGD,uint8_t* fSostRS,uint8_t* fNumBlock,uint16_t fbrate);
/*
 * fNCtr - указатель на номер контроллера &GD.Control.NFCtr
 * fADRGD - указатель на структуру адресов блоков пам€ти ArdGD
 * fSostRS - указатель на переменну состо€ние передатчика &GD.Hot.SostRS
 * fNumBlock - номер блока, который был прин€т или передан &NumBlock
 * fbrate - скорость обмена 9600

 */
void CheckRSTime(void);
/*ƒл обнулени€ передатчика, если завис*/


#endif

