#ifndef __STM32F10X_LCD240X64_H
#define	__STM32F10X_LCD240X64_H

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

