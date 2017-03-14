#include "syntax.h"

#include "stm32f10x_LCD240x64.h"

#include "hal_systimer.h"
#include "hal_lcd.h"

#include "debug.h"


#define Tire	128	//0x8D
#define DTire	129


#include "control_gd.h"
#include "wtf.h"

// XXX: WTFS
extern uint16_t x_menu;
extern bool BlkW;
extern uchar Ad_Buf;
extern uchar AdinB;
extern uchar Mark;

extern char GrafView;

char lcdbuf[BufSize+10];  //TempEnd+6];

static hal_lcd_text_buf_t textbuf;

// refactoring for hal abstraction

void SendBlock(const char *Src, int offset, int Size)
{
    uint end = offset + Size;
    REQUIRE(end <= sizeof(textbuf.raw));
    memcpy(&textbuf.raw[offset], Src, Size);
}

void FillLine(char vSim, char NumStr)
{
    memset(&textbuf.lines[NumStr], vSim, sizeof(textbuf.lines[0]));
}

void SendFirstScreen(char tmSec)
{
}

void ClearGraf(void)
{
}


//-------------- Инициализировать дисплей -------

void IniLCDMem(void)
{
    FillLine(DTire,(SUM_LINE_DISP-2));   //=

    if (x_menu)
        FillLine(Tire,1);

}
void InitLCD(void)
{
    IniLCDMem();
    FillLine(Tire,1);    //-
    FillLine(0x00,7);    //Пробел
}
/*---------------------------------------------------
        Очистка буфера
----------------------------------------------------*/
void clear_d(void)
{
    unsigned char ic;
    BlkW=0;
    for (ic=0;ic<BufSize;ic++) lcdbuf[ic]=' ';
}
//---------------------------------------------------
//        Вывод буфера в индикатор
//---------------------------------------------------
void TimeToBuf(void)
{
    BlkW=1;
    Ad_Buf=PozTime;
//    buf[Ad_Buf++]=' ';
    w_int(&gd()->Hot.Time,SSdSS, 0);
    lcdbuf[Ad_Buf++]=':';
    lcdbuf[Ad_Buf++]=wtf0.Second/10+'0';
    lcdbuf[Ad_Buf++]=wtf0.Second%10+'0';
    Ad_Buf++;
    w_int(&gd()->Hot.Date,DsMsY, 0);
}

void VideoSost(void)
{
    int ic;

    BlkW=1;
    Ad_Buf=Str2d;
    for (ic=Str2d; ic < Str2; ic++) lcdbuf[ic]=' ';
    // Ad_Buf=Str4;
/*    if (ds18b20_ReadROM())
        Ad_Buf+=2;
    w_int(&Buf1W[0],StStStS, 0);
        buf[Ad_Buf++]='-';
    w_int(&Buf1W[4],StStStS, 0);
    w_int(&SumAnswers,SSSS, 0);*/
    pmInfoProg405();
    SendBlock(&lcdbuf[Str2d],TxtHomeAddr+DisplCols*(SUM_LINE_DISP-1),DisplCols);
//    pmInfoProg405();

//	w_txt(I2C1_Buffer_Tx);
//    w_txt(I2C1_Buffer_Rx);

//    w_int(&GlobData,SSSS, 0);
    //w_txt()
    //SendBlock(&buf[Str2d],TxtHomeAddr+DisplCols*(SUM_LINE_DISP-1),DisplCols);
}

void Video(void)
{
    uchar   CurRow,CurCol;
    //  if (CheckKeyboardXMEGA()) return;
    if (GrafView)
    {
        GrafView--;
        if (!GrafView)
            ClearGraf();
        else
            return;
    }

    // draw hbars at line 1, line 6
    IniLCDMem();

    // draw line 0 - time etc
    TimeToBuf();
    SendBlock(&lcdbuf[0],TxtHomeAddr,DisplCols);

    // draw line 2, 3, 4, 5
    SendBlock(&lcdbuf[Str2],TxtHomeAddr+DisplCols*2,DisplCols*(SUM_LINE_DISP-4));

//-- установить курсор --
    CurCol=(AdinB+Mark) % DisplCols;
    CurRow=(AdinB+Mark) / DisplCols;// + 2;

    HAL_lcd_position_cursor(CurCol, CurRow, wtf0.Menu ? 8 : 3, 1);

    // draw line 6 (last line) - sensors values
    VideoSost();

    HAL_lcd_render_text(&textbuf);
}
