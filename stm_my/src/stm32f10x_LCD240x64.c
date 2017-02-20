#include "syntax.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_LCD240x64.h"
#include "stm32f10x_i2c.h"

#define SetCE	PORT_IND_CMD_CE->BSRR=CE_ind // GPIO_WriteBit(PORT_IND_CMD_CE, CE_ind, Bit_SET)
#define ClrCE	PORT_IND_CMD_CE->BRR=CE_ind //GPIO_WriteBit(PORT_IND_CMD_CE, CE_ind, Bit_RESET)

#define SetCMD	PORT_IND_CMD_CE->BSRR=Cmd_Dat // GPIO_WriteBit(PORT_IND_CMD_CE, Cmd_Dat, Bit_SET)
#define ClrCMD	PORT_IND_CMD_CE->BRR=Cmd_Dat //GPIO_WriteBit(PORT_IND_CMD_CE, Cmd_Dat, Bit_RESET)

#define SetRD	PORT_IND_WR_RD->BSRR=RD_ind //GPIO_WriteBit(PORT_IND_WR_RD, RD_ind, Bit_SET)
#define ClrRD	PORT_IND_WR_RD->BRR=RD_ind //GPIO_WriteBit(PORT_IND_WR_RD, RD_ind, Bit_RESET)

#define SetWR	PORT_IND_WR_RD->BSRR=WR_ind //GPIO_WriteBit(PORT_IND_WR_RD, WR_ind, Bit_SET)
#define ClrWR	PORT_IND_WR_RD->BRR=WR_ind //GPIO_WriteBit(PORT_IND_WR_RD, WR_ind, Bit_RESET)

#define Tire	0xAC	//0x8D
#define DTire	0x82



#define cmdTxtHome		0x40
#define cmdTxtArea		0x41
#define cmdGrfHome		0x42
#define cmdGrfArea		0x43
#define cmdPozCurs		0x21
#define cmdOffSet		0x22
#define cmdAddrPtr		0x24
#define cmd2LineCurs	0xA1
#define cmd3LineCurs	0xA2
#define cmd8LineCurs	0xA7
#define cmdOnAutoWr		0xB0
#define cmdOffAuto		0xB2
#define cmdModeOR		0x80
#define cmdModeEXOR		0x81
#define cmdTxtOnGrafOn	0x9F
#define cmdTxtOffGrafOn 0x98
#define cmdTxtOnGrafOff 0x97

#define OffSet			0x02
#define CGHomeAddr		0x1400

#include "65_gd.h"
#include "climdefstuff.h"

// XXX: WTFS
extern uint16_t x_menu;
extern bool BlkW;
extern uchar Ad_Buf;
extern eGData GD;
extern uchar AdinB;
extern uchar Mark;

extern char GrafView;

char lcdbuf[BufSize+10];  //TempEnd+6];

static const uchar FirstScreen[40*64]={
    31,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,62,
    63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
    53,63,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,23,59,
    63,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,62,42,43,
    63,63,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,31,59,
    63,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,42,42,43,
    62,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,14,43,
    58,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,10,43,
    62,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,43,
    58,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,55,
    58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,43,
    58,32,0,1,62,12,31,63,60,63,60,3,56,8,7,62,1,62,12,7,33,60,3,63,63,63,56,15,48,60,7,62,63,59,63,0,0,0,1,23,
    58,0,0,0,24,22,4,24,24,24,28,3,32,24,4,50,0,24,22,24,24,28,3,33,33,33,32,3,3,3,4,50,12,8,49,32,0,0,0,43,
    58,0,0,0,24,38,4,24,24,24,22,5,32,28,4,50,0,24,38,24,24,22,5,33,33,33,32,3,3,3,4,50,12,8,49,32,0,0,1,23,
    48,0,0,0,24,32,4,24,24,56,22,5,32,44,0,48,0,24,32,48,12,22,5,33,33,33,32,3,6,1,32,48,12,32,49,32,0,0,0,3,
    58,0,0,0,25,0,4,24,25,24,22,5,32,38,0,48,0,25,0,48,12,22,5,33,33,33,32,3,6,1,32,48,12,32,49,32,0,0,1,23,
    48,0,0,0,31,32,4,24,25,24,19,9,32,38,0,48,0,31,32,48,12,19,9,33,33,33,63,3,62,1,32,48,15,32,63,0,0,0,0,3,
    48,0,0,0,25,32,4,24,26,24,19,9,33,3,0,48,0,25,32,48,12,19,9,33,33,33,33,35,6,1,32,48,12,32,48,0,0,0,1,23,
    48,0,0,0,24,48,4,24,28,24,19,17,33,63,0,48,60,24,48,48,12,19,17,33,33,33,33,35,6,1,32,48,12,32,48,0,0,0,0,3,
    48,0,0,0,24,49,36,24,24,24,17,49,33,3,32,48,0,24,48,24,24,17,49,33,33,33,33,35,3,3,0,48,12,8,48,0,0,0,0,7,
    48,0,0,0,24,25,40,24,24,24,17,33,34,1,32,48,0,24,24,24,24,17,33,33,33,33,33,35,3,3,0,48,12,8,48,0,0,0,0,3,
    48,0,0,1,62,31,49,63,60,63,60,39,63,35,51,60,1,62,30,7,33,60,39,63,51,63,63,15,48,60,3,60,63,59,60,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,1,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,0,14,0,62,0,63,32,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,3,39,51,32,0,0,0,0,0,0,0,0,0,0,0,3,46,6,0,30,3,39,0,63,32,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,7,3,35,48,0,0,0,0,0,0,0,0,0,0,0,6,14,6,0,60,3,3,33,32,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,14,3,33,48,56,56,28,57,48,15,48,0,0,0,0,12,28,0,1,60,7,3,33,32,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,28,3,33,51,56,56,63,63,48,25,48,0,0,0,0,24,28,0,3,60,14,3,33,48,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,28,7,1,54,57,49,63,63,48,48,56,0,0,0,0,24,28,0,7,28,14,3,33,60,0,0,0,0,0,0,0,3,
    58,32,0,0,0,0,0,0,56,7,1,48,49,48,28,57,49,48,56,0,0,0,0,24,28,24,6,56,14,3,32,60,0,0,0,0,0,0,0,3,
    48,0,0,0,0,0,0,0,56,7,1,49,49,48,61,59,33,32,56,0,0,0,0,24,63,56,12,56,28,3,32,30,0,0,0,0,0,0,0,3,
    58,32,0,0,0,0,0,0,56,14,3,33,49,48,57,51,35,32,56,0,0,0,0,28,56,48,24,56,28,3,32,14,0,0,0,0,0,0,0,3,
    52,0,0,0,0,0,0,0,56,14,3,33,35,32,57,51,35,32,48,0,0,0,0,28,56,48,56,56,28,7,0,14,0,0,0,0,0,0,1,23,
    62,40,0,0,0,0,0,0,56,14,7,3,39,32,57,55,27,33,48,0,0,0,0,0,56,0,49,56,28,7,0,14,0,0,0,0,0,0,0,3,
    52,0,0,0,0,0,0,0,28,14,6,3,47,44,57,55,51,33,32,0,0,0,0,1,48,1,63,60,28,7,0,12,0,0,0,0,0,0,5,23,
    58,32,0,0,0,0,0,0,28,30,12,3,63,57,51,39,35,51,0,0,0,0,0,1,48,0,1,48,28,14,0,28,0,0,0,0,0,0,0,3,
    55,63,0,0,0,0,0,0,15,63,56,3,51,49,51,39,1,62,0,0,0,0,0,1,48,0,1,48,12,12,0,24,0,0,0,0,0,0,5,23,
    58,32,0,0,0,0,0,0,0,56,0,0,0,0,0,0,0,0,0,0,0,0,0,3,48,0,1,48,14,28,24,48,0,0,0,0,0,0,0,3,
    63,63,0,0,0,0,0,0,56,56,0,0,0,0,0,0,0,0,0,0,0,0,0,15,60,0,3,32,7,48,31,32,0,0,0,0,0,0,5,31,
    58,32,0,0,0,0,0,0,61,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    63,63,48,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,63,63,
    58,33,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,23,
    63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,63,59,
    58,45,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,23,
    63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,63,59,
    58,37,63,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,56,5,23,
    63,63,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,63,63,
    58,39,63,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,63,58,47,31,
    63,63,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,40,15,63,63,
    58,39,63,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,63,62,47,63,
    63,63,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,40,0,51,63,
    58,42,58,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,63,58,42,59,
    63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
    63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
};

const char ExtCG[SumExtCG*8]={
    0x3F,0x31,0x30,0x3E,0x31,0x31,0x3E,0x20,    //Б - \240
    0x3F,0x31,0x30,0x30,0x30,0x30,0x30,0x20,    //Г - \241
    0x20,0x20,0x1F,0x20,0x1F,0x20,0x20,0x20,    //==  \242
    0x35,0x35,0x35,0x2E,0x35,0x35,0x35,0x20,    //Ж - \243
    0x3E,0x21,0x21,0x2E,0x21,0x21,0x3E,0x20,    //З - \244
    0x31,0x31,0x33,0x35,0x39,0x31,0x31,0x20,    //И - \245
    0x2A,0x24,0x31,0x33,0x35,0x39,0x31,0x20,    //й - \246
    0x2F,0x25,0x25,0x25,0x25,0x35,0x29,0x20,    //Л - \247
    0x3F,0x31,0x31,0x31,0x31,0x31,0x31,0x20,    //П - \250
    0x31,0x31,0x31,0x2A,0x24,0x28,0x30,0x20,    //У - \251
    0x24,0x2E,0x35,0x35,0x35,0x2E,0x24,0x20,    //Ф
    0x31,0x31,0x31,0x2F,0x21,0x21,0x21,0x20,    //Ч
    0x35,0x35,0x35,0x35,0x35,0x35,0x3F,0x20,    //Ш
    0x38,0x28,0x28,0x2E,0x29,0x29,0x2E,0x20,    //Ъ
    0x31,0x31,0x31,0x39,0x35,0x35,0x39,0x20,    //Ы
    0x2E,0x31,0x21,0x27,0x21,0x31,0x2E,0x20,    //Э - \257

    0x32,0x35,0x35,0x39,0x35,0x35,0x32,0x20,    //Ю - \260
    0x2F,0x31,0x31,0x2F,0x25,0x29,0x31,0x20,    //Я
    0x23,0x2C,0x30,0x3E,0x31,0x31,0x2E,0x20,    //б
    0x20,0x20,0x3C,0x32,0x3C,0x32,0x3C,0x20,    //в
    0x20,0x20,0x3F,0x31,0x30,0x30,0x30,0x20,    //г
    0x2F,0x31,0x31,0x2F,0x25,0x29,0x31,0x20,    //ё???
    0x20,0x20,0x35,0x35,0x2E,0x35,0x35,0x20,    //ж
    0x20,0x20,0x3E,0x21,0x26,0x21,0x3E,0x20,    //з
    0x20,0x20,0x31,0x33,0x35,0x39,0x31,0x20,    //и
    0x20,0x2A,0x24,0x31,0x33,0x35,0x39,0x20,    //й
    0x20,0x20,0x32,0x34,0x38,0x34,0x32,0x20,    //к
    0x20,0x20,0x2F,0x25,0x25,0x35,0x29,0x20,    //л
    0x20,0x20,0x31,0x3B,0x35,0x31,0x31,0x20,    //м
    0x20,0x20,0x31,0x31,0x3F,0x31,0x31,0x20,    //н
    0x20,0x20,0x3F,0x31,0x31,0x31,0x31,0x20,    //п
    0x20,0x20,0x3F,0x24,0x24,0x24,0x24,0x20,    //т - \277

    0x20,0x20,0x31,0x31,0x2F,0x21,0x21,0x20,    //ч - \300
    0x20,0x20,0x35,0x35,0x35,0x35,0x3F,0x20,    //ш
    0x20,0x20,0x38,0x08,0x2E,0x29,0x2E,0x20,    //ъ
    0x20,0x20,0x31,0x31,0x39,0x35,0x39,0x20,    //ы
    0x20,0x20,0x30,0x30,0x3C,0x32,0x3C,0x20,    //ь
    0x20,0x20,0x2E,0x31,0x27,0x31,0x2E,0x20,    //э
    0x20,0x20,0x32,0x35,0x3D,0x35,0x32,0x20,    //ю
    0x20,0x20,0x2F,0x31,0x2F,0x25,0x29,0x20,    //я
    0x20,0x20,0x3C,0x32,0x3C,0x30,0x30,0x20,    //р
    0x20,0x20,0x31,0x31,0x2F,0x21,0x3E,0x20,    //у
    0x1F,0x1F,0x1B,0x1D,0x20,0x1D,0x1B,0x1F,    //-left	 \312
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x1F,0x20,0x20,0x20,0x20,    //--
    0x20,0x26,0x26,0x20,0x20,0x20,0x20,0x20,    //grad
    0x2E,0x21,0x22,0x24,0x2F,0x20,0x20,0x20,    //m2
    0x2E,0x21,0x26,0x21,0x2E,0x20,0x20,0x20,    //m3 \317

    0x20,0x20,0x3F,0x24,0x24,0x25,0x22,0x20,    // греческий- тау \320
    0x20,0x20,0x32,0x31,0x31,0x32,0x2C,0x20,    //
    0x20,0x39,0x25,0x26,0x2C,0x2A,0x33,0x20,    //
    0x20,0x24,0x35,0x35,0x2E,0x24,0x24,0x20,    // фи
    0x20,0x20,0x2A,0x31,0x35,0x35,0x2A,0x20,    // омега
//0x3F,0x31,0x30,0x30,0x30,0x30,0x30,0x20,	//Г
    0x24,0x24,0x2A,0x2A,0x31,0x31,0x3F,0x20,    //
    0x2E,0x31,0x31,0x3F,0x31,0x31,0x2E,0x20,    //
    0x24,0x24,0x2A,0x2A,0x31,0x31,0x31,0x20,    //
    0x3F,0x31,0x20,0x2E,0x20,0x31,0x3F,0x20,    //
    0x3F,0x2A,0x2A,0x2A,0x2A,0x2A,0x2A,0x20,    // ПИ
    0x3F,0x28,0x24,0x22,0x24,0x28,0x3F,0x20,    //
    0x2A,0x35,0x35,0x24,0x24,0x24,0x24,0x20,    //
    0x24,0x2E,0x35,0x35,0x35,0x2E,0x24,0x20,    //Ф
    0x35,0x35,0x35,0x35,0x2E,0x24,0x24,0x20,    //
    0x2E,0x31,0x31,0x31,0x2A,0x2A,0x3B,0x20,    //
    0x20,0x20,0x2D,0x32,0x32,0x32,0x2D,0x20,    //альфа - малая

    0x2F,0x25,0x25,0x25,0x29,0x31,0x3F,0x31,    //Д - \340
    0x31,0x31,0x31,0x31,0x31,0x31,0x3F,0x21,    //Ц
    0x35,0x35,0x35,0x35,0x35,0x35,0x3F,0x21,    //Щ
    0x20,0x20,0x2F,0x25,0x29,0x31,0x3F,0x31,    //д
    0x20,0x24,0x24,0x2E,0x35,0x35,0x2E,0x24,    //ф
    0x20,0x20,0x31,0x31,0x31,0x31,0x3F,0x21,    //ц
    0x20,0x20,0x35,0x35,0x35,0x35,0x3F,0x21,    //щ
    0x20,0x22,0x35,0x35,0x2E,0x24,0x24,0x20,    //греческий- фи \347
    0x20,0x2E,0x31,0x30,0x2C,0x22,0x2C,0x20,    //греческий- сигма(мал) \350 дубль \377 0x20,0x20,0x2F,0x32,0x32,0x32,0x2C,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //

    0x2C,0x32,0x32,0x36,0x31,0x31,0x36,0x30,    // греческие малые \360 beta
    0x20,0x20,0x39,0x29,0x29,0x2A,0x24,0x24,    //	gamma
    0x26,0x29,0x24,0x2E,0x31,0x31,0x2E,0x20,    //
    0x20,0x20,0x2F,0x30,0x3E,0x30,0x2F,0x20,    //
    0x2F,0x22,0x2C,0x28,0x26,0x21,0x26,0x20,    //
    0x20,0x36,0x29,0x29,0x29,0x29,0x21,0x20,    //
    0x26,0x29,0x29,0x2F,0x29,0x29,0x26,0x20,    //
    0x20,0x20,0x24,0x24,0x24,0x24,0x23,0x20,    //
    0x20,0x20,0x32,0x34,0x38,0x34,0x32,0x20,    //к
    0x2C,0x24,0x24,0x24,0x2A,0x2A,0x31,0x20,    //
    0x20,0x20,0x32,0x32,0x32,0x3D,0x30,0x20,    //
    0x20,0x20,0x33,0x31,0x32,0x34,0x28,0x20,    //
    0x3F,0x24,0x28,0x27,0x28,0x30,0x2E,0x21,    //
    0x20,0x20,0x3F,0x2A,0x2A,0x2A,0x33,0x20,    //
    0x20,0x2E,0x31,0x31,0x31,0x3E,0x30,0x30,    // - ro
    0x20,0x20,0x2F,0x32,0x32,0x32,0x2C,0x20     // греческий- сигма(мал)\377
};

/***************************************************************************//**
 * @brief  Setting CONTROLS pins to output mode
 ******************************************************************************/
void LCD_CTRL_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = CE_ind | Cmd_Dat;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT_IND_CMD_CE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = WR_ind | RD_ind;
    GPIO_Init(PORT_IND_WR_RD, &GPIO_InitStructure);

}


/***************************************************************************//**
 * @brief  Setting DATA pins to input mode
 ******************************************************************************/
void LCD_DATA_DIR_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = MASK_IND1;
    GPIO_Init(PORT_IND1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MASK_IND2;
    GPIO_Init(PORT_IND2, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Setting DATA pins to output mode
 ******************************************************************************/
void LCD_DATA_DIR_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MASK_IND1;
    GPIO_Init(PORT_IND1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MASK_IND2;
    GPIO_Init(PORT_IND2, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Reading DATA pins
 * @return the data value.
 ******************************************************************************/
unsigned char LCD_DATA_IN(void)
{
    //uint16_t u16Temp;
    //u16Temp = GPIO_ReadInputData(PORT_IND1)&MASK_IND1;
    //u16Temp>>=N_IND1;
    //u16Temp|= ((GPIO_ReadInputData(PORT_IND2)&MASK_IND2)>>N_IND2);
    //u8Tmp=u16Temp;
    return((PORT_IND1->IDR&MASK_IND1)>>N_IND1)|((PORT_IND2->IDR&MASK_IND2)>>N_IND2);//u8Tmp;//SWAP_DATA[u16Temp];
}

/***************************************************************************//**
 * @brief  Write DATA to LCD
 ******************************************************************************/
void LCD_DATA_OUT(uint8_t u8Tmp)
{
/*	uint16_t u16Temp=0;
    u16Temp = GPIO_ReadOutputData(PORT_IND1)&(~MASK_IND1);
    u16Temp |=  (u8Tmp&0x0f)<<N_IND1;//Внимательно если маска не 0xff нужно преобразование
    GPIO_Write(PORT_IND1, u16Temp);
    u16Temp = GPIO_ReadOutputData(PORT_IND2)&(~MASK_IND2);
    u16Temp |=  (u8Tmp&0xf0)<<N_IND2;//Внимательно если маска не 0xff нужно преобразование
    GPIO_Write(PORT_IND2, u16Temp);*/
    PORT_IND1->ODR=(PORT_IND1->IDR&(~MASK_IND1))|((u8Tmp&0x0f)<<N_IND1);
    PORT_IND2->ODR=(PORT_IND2->IDR&(~MASK_IND2))|((u8Tmp&0xf0)<<N_IND2);

}



/***************************************************************************//**
 * @brief Delay some time
 ******************************************************************************/
void Delay(vu32 nCount)
{
    for (; nCount != 0; nCount--);
}

uint8_t  ReadStatus (char Need)
{
    uint8_t res, TimeOut;
    res=0;
    TimeOut=0;
    while (1)
    {

//	CLREA;
        LCD_DATA_DIR_IN();
        SetCMD;
        ClrRD;              //Разрешение дисплея
        ClrCE;
        res=LCD_DATA_IN();      //Прием статуса из дисплея
        SetRD;
        SetCE;
        LCD_DATA_DIR_OUT();
//    SETEA;

        if ((res & Need) ==Need) return 1;
        TimeOut++;
        if (TimeOut > MaxTimeOut)/*not=100;ton=6;*/
        {
            return 0;
        }
    }
}

void SendCmd(uchar Cmd)
{
    ReadStatus(3);

//    CLREA;
    LCD_DATA_OUT(Cmd);
    SetCMD;
    ClrCE;              //Разрешение записи в дисплей
    ClrWR;
    Delay(1);
    SetWR;
    SetCE;              //Запрещение записи в дисплей
//    SETEA;

}

void SendByte(uchar Data)
{
    ReadStatus(3);
//    CLREA;
    LCD_DATA_OUT(Data);
    ClrCMD;
    ClrCE;              //Разрешение записи в дисплей
    ClrWR;
    Delay(1);
    SetWR;
    SetCE;              //Запрещение записи в дисплей
//    SETEA;

}

void Send2(uchar Cmd, int Data)
{
//	uchar snd;
    //snd=(uchar)(Data%256);
    SendByte(Data%256);
    //snd=(uchar)(Data/256);
    SendByte(Data/256);
    SendCmd(Cmd);
}

void SendBlock(const char *Src, int Dst, int Size)
{
    char ch;
    Send2(cmdAddrPtr,Dst);
    SendCmd(cmdOnAutoWr);
    while (Size--)
    {
        ch=(*Src) - 0x20;
        Src++;
        ReadStatus(8);
//		CLREA;
        LCD_DATA_OUT(ch);
        ClrCMD;
        ClrCE;              //Разрешение записи в дисплей
        ClrWR;
        Delay(1);
        SetWR;

        SetCE;              //Запрещение записи в дисплей
//		SETEA;

    }
    SendCmd(cmdOffAuto);
}

void SendBlockPM(char *Src, int Dst, int Size)
{
    char ch;
    Send2(cmdAddrPtr,Dst);
    SendCmd(cmdOnAutoWr);
    while (Size--)
    {
        ch=Src - 0x20;
        Src++;
        ReadStatus(8);
//		CLREA;
        LCD_DATA_OUT(ch);
        ClrCMD;
        ClrCE;              //Разрешение записи в дисплей
        ClrWR;
        Delay(1);
        SetWR;
        SetCE;              //Запрещение записи в дисплей
//		SETEA;
    }
    SendCmd(cmdOffAuto);
}

void SendSim(char vSim, char NumStr)
{
    char vDisplCols;
    vDisplCols=DisplCols;
    Send2(cmdAddrPtr,TxtHomeAddr+NumStr*DisplCols);
    SendCmd(cmdOnAutoWr);
    while (vDisplCols--)
    {
        ReadStatus(8);
//		CLREA;
        LCD_DATA_OUT(vSim);
        ClrCMD;
        ClrCE;              //Разрешение записи в дисплей
        ClrWR;
        Delay(1);
        SetWR;
        SetCE;              //Запрещение записи в дисплей
//		SETEA;

    }
    SendCmd(cmdOffAuto);
}

//-------------- Инициализировать дисплей -------

void IniLCDMem(void)
{
    Send2(cmdTxtHome,TxtHomeAddr);
    Send2(cmdGrfHome,GrfHomeAddr);
    Send2(cmdTxtArea,TxtArea);
    Send2(cmdGrfArea,GrfArea);
    SendCmd(cmdModeEXOR);
    Send2(cmdOffSet,OffSet);
    SendCmd(cmdTxtOnGrafOff);
//	SendCmd(cmdTxtOnGrafOn);

    SendSim(DTire,(SUM_LINE_DISP-2));   //=

    if (x_menu) SendSim(Tire,1);

}
void InitLCD(void)
{
    LCD_STARTUP;
    LCD_CTRL_DIR_OUT();
    SetWR;
    SetRD;
    SetCE;
    IniLCDMem();
    SendBlock(&(ExtCG[0]),CGHomeAddr,SumExtCG*8);
    SendSim(Tire,1);    //-
    SendSim(0x00,7);    //Пробел
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
    w_int(&CtrTime,SSdSS);
    lcdbuf[Ad_Buf++]=':';
    lcdbuf[Ad_Buf++]=WTF0.Second/10+'0';
    lcdbuf[Ad_Buf++]=WTF0.Second%10+'0';
    Ad_Buf++;
    w_int(&CtrData,DsMsY);
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
    w_int(&Buf1W[0],StStStS);
        buf[Ad_Buf++]='-';
    w_int(&Buf1W[4],StStStS);
    w_int(&SumAnswers,SSSS);*/
    pmInfoProg405();
    SendBlock(&lcdbuf[Str2d],TxtHomeAddr+DisplCols*(SUM_LINE_DISP-1),DisplCols);
//    pmInfoProg405();

//	w_txt(I2C1_Buffer_Tx);
//    w_txt(I2C1_Buffer_Rx);

//    w_int(&GlobData,SSSS);
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
        if (!GrafView) ClearGraf();
        else return;
    }

    IniLCDMem();
    TimeToBuf();

//---- output CharSet ----------------

    SendBlock(&lcdbuf[0],TxtHomeAddr,DisplCols);
    SendBlock(&lcdbuf[Str2],TxtHomeAddr+DisplCols*2,DisplCols*(SUM_LINE_DISP-4));
    if (WTF0.Menu) SendCmd(cmd8LineCurs);
    else  SendCmd(cmd3LineCurs);
//-- установить курсор --
    CurCol=(AdinB+Mark) % DisplCols;
    CurRow=(AdinB+Mark) / DisplCols;// + 2;
    Send2(cmdPozCurs,((int)CurRow * 256)+ CurCol); //0x0101);
    VideoSost();



}

void SendFirstScreen(char tmSec)
{
    int i;
    if (!tmSec) return;
    GrafView=tmSec;
    SendCmd(cmdTxtOffGrafOn);
    Send2(cmdAddrPtr,GrfHomeAddr);
    SendCmd(cmdOnAutoWr);
    for (i=0; i < 40*64; i++)
    {
        ReadStatus(8);

//		CLREA;
        LCD_DATA_OUT(FirstScreen[i]);
        ClrCMD;
        ClrCE;              //Разрешение записи в дисплей
        ClrWR;
        Delay(1);
        SetWR;
        SetCE;              //Запрещение записи в дисплей
//    	SETEA;
    }
    SendCmd(cmdOffAuto);
}

void ClearGraf(void)
{
    int i;
    InitLCD();
    Send2(cmdAddrPtr,GrfHomeAddr);
    SendCmd(cmdOnAutoWr);
//---- очистка графического экрана -------
    for (i=0; i < 40*64; i++)
    {
        ReadStatus(8);
//		CLREA;
        LCD_DATA_OUT(0);
        ClrCMD;
        ClrCE;              //Разрешение записи в дисплей
        ClrWR;
        Delay(1);
        SetWR;
        SetCE;              //Запрещение записи в дисплей
//    	SETEA;
    }
    SendCmd(cmdOffAuto);
}
