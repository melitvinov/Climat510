#ifndef _LCD_H_
#define	_LCD_H_

#define	SUM_LINE_DISP	8

#define	DisplCols		40


void SendBlock(const char *Src, int offset, int Size);

void SendFirstScreen(char tmSec);
void ClearGraf(void);
void clear_d(void);
void InitLCD(void);
void Video(void);


#endif

