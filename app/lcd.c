#include "syntax.h"

#include "hal.h"
#include "lcd.h"
#include "debug.h"


#define Tire	128	//0x8D
#define DTire	129

#define BufSize			240	//160

#define PozTime			23


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

static hal_lcd_text_screen_t textbuf;

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
    for (ic=Str2d; ic < Str2; ic++)
        lcdbuf[ic]=' ';

    pmInfoProg405();
    SendBlock(&lcdbuf[Str2d], DisplCols*(SUM_LINE_DISP-1),DisplCols);
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
    SendBlock(&lcdbuf[0], 0,DisplCols);

    // draw line 2, 3, 4, 5
    SendBlock(&lcdbuf[Str2], DisplCols*2,DisplCols*(SUM_LINE_DISP-4));

//-- установить курсор --
    CurCol=(AdinB+Mark) % DisplCols;
    CurRow=(AdinB+Mark) / DisplCols;// + 2;

    HAL_lcd_position_cursor(CurCol, CurRow, wtf0.Menu ? 8 : 3, 1);

    // draw line 6 (last line) - sensors values
    VideoSost();

    HAL_lcd_render_text_screen(&textbuf);
}
