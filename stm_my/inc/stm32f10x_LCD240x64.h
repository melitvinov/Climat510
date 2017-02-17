//������������ ������������ ���� ����������
#ifndef __STM32F10X_LCD240X64_H
#define	__STM32F10X_LCD240X64_H

#define PORT_IND1	GPIOB
#define PORT_IND2	GPIOB

#define CE_ind				GPIO_Pin_8
#define Cmd_Dat 			GPIO_Pin_9
#define PORT_IND_CMD_CE		GPIOC
#define WR_ind				GPIO_Pin_6
#define RD_ind				GPIO_Pin_7
#define PORT_IND_WR_RD		GPIOC

#define MASK_IND1	0x003c
#define N_IND1		2
#define MASK_IND2	0xf000
#define N_IND2		8

#ifdef DEBUG
#define LCD_STARTUP //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#else
#define LCD_STARTUP RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif


#define	SumExtCG	96
#define	SUM_LINE_DISP	8

const char ExtCG[SumExtCG*8]={
    0x3F,0x31,0x30,0x3E,0x31,0x31,0x3E,0x20,    //� - \240
    0x3F,0x31,0x30,0x30,0x30,0x30,0x30,0x20,    //� - \241
    0x20,0x20,0x1F,0x20,0x1F,0x20,0x20,0x20,    //==  \242
    0x35,0x35,0x35,0x2E,0x35,0x35,0x35,0x20,    //� - \243
    0x3E,0x21,0x21,0x2E,0x21,0x21,0x3E,0x20,    //� - \244
    0x31,0x31,0x33,0x35,0x39,0x31,0x31,0x20,    //� - \245
    0x2A,0x24,0x31,0x33,0x35,0x39,0x31,0x20,    //� - \246
    0x2F,0x25,0x25,0x25,0x25,0x35,0x29,0x20,    //� - \247
    0x3F,0x31,0x31,0x31,0x31,0x31,0x31,0x20,    //� - \250
    0x31,0x31,0x31,0x2A,0x24,0x28,0x30,0x20,    //� - \251
    0x24,0x2E,0x35,0x35,0x35,0x2E,0x24,0x20,    //�
    0x31,0x31,0x31,0x2F,0x21,0x21,0x21,0x20,    //�
    0x35,0x35,0x35,0x35,0x35,0x35,0x3F,0x20,    //�
    0x38,0x28,0x28,0x2E,0x29,0x29,0x2E,0x20,    //�
    0x31,0x31,0x31,0x39,0x35,0x35,0x39,0x20,    //�
    0x2E,0x31,0x21,0x27,0x21,0x31,0x2E,0x20,    //� - \257

    0x32,0x35,0x35,0x39,0x35,0x35,0x32,0x20,    //� - \260
    0x2F,0x31,0x31,0x2F,0x25,0x29,0x31,0x20,    //�
    0x23,0x2C,0x30,0x3E,0x31,0x31,0x2E,0x20,    //�
    0x20,0x20,0x3C,0x32,0x3C,0x32,0x3C,0x20,    //�
    0x20,0x20,0x3F,0x31,0x30,0x30,0x30,0x20,    //�
    0x2F,0x31,0x31,0x2F,0x25,0x29,0x31,0x20,    //�???
    0x20,0x20,0x35,0x35,0x2E,0x35,0x35,0x20,    //�
    0x20,0x20,0x3E,0x21,0x26,0x21,0x3E,0x20,    //�
    0x20,0x20,0x31,0x33,0x35,0x39,0x31,0x20,    //�
    0x20,0x2A,0x24,0x31,0x33,0x35,0x39,0x20,    //�
    0x20,0x20,0x32,0x34,0x38,0x34,0x32,0x20,    //�
    0x20,0x20,0x2F,0x25,0x25,0x35,0x29,0x20,    //�
    0x20,0x20,0x31,0x3B,0x35,0x31,0x31,0x20,    //�
    0x20,0x20,0x31,0x31,0x3F,0x31,0x31,0x20,    //�
    0x20,0x20,0x3F,0x31,0x31,0x31,0x31,0x20,    //�
    0x20,0x20,0x3F,0x24,0x24,0x24,0x24,0x20,    //� - \277

    0x20,0x20,0x31,0x31,0x2F,0x21,0x21,0x20,    //� - \300
    0x20,0x20,0x35,0x35,0x35,0x35,0x3F,0x20,    //�
    0x20,0x20,0x38,0x08,0x2E,0x29,0x2E,0x20,    //�
    0x20,0x20,0x31,0x31,0x39,0x35,0x39,0x20,    //�
    0x20,0x20,0x30,0x30,0x3C,0x32,0x3C,0x20,    //�
    0x20,0x20,0x2E,0x31,0x27,0x31,0x2E,0x20,    //�
    0x20,0x20,0x32,0x35,0x3D,0x35,0x32,0x20,    //�
    0x20,0x20,0x2F,0x31,0x2F,0x25,0x29,0x20,    //�
    0x20,0x20,0x3C,0x32,0x3C,0x30,0x30,0x20,    //�
    0x20,0x20,0x31,0x31,0x2F,0x21,0x3E,0x20,    //�
    0x1F,0x1F,0x1B,0x1D,0x20,0x1D,0x1B,0x1F,    //-left	 \312
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x1F,0x20,0x20,0x20,0x20,    //--
    0x20,0x26,0x26,0x20,0x20,0x20,0x20,0x20,    //grad
    0x2E,0x21,0x22,0x24,0x2F,0x20,0x20,0x20,    //m2
    0x2E,0x21,0x26,0x21,0x2E,0x20,0x20,0x20,    //m3 \317

    0x20,0x20,0x3F,0x24,0x24,0x25,0x22,0x20,    // ���������- ��� \320
    0x20,0x20,0x32,0x31,0x31,0x32,0x2C,0x20,    //
    0x20,0x39,0x25,0x26,0x2C,0x2A,0x33,0x20,    //
    0x20,0x24,0x35,0x35,0x2E,0x24,0x24,0x20,    // ��
    0x20,0x20,0x2A,0x31,0x35,0x35,0x2A,0x20,    // �����
//0x3F,0x31,0x30,0x30,0x30,0x30,0x30,0x20,	//�
    0x24,0x24,0x2A,0x2A,0x31,0x31,0x3F,0x20,    //
    0x2E,0x31,0x31,0x3F,0x31,0x31,0x2E,0x20,    //
    0x24,0x24,0x2A,0x2A,0x31,0x31,0x31,0x20,    //
    0x3F,0x31,0x20,0x2E,0x20,0x31,0x3F,0x20,    //
    0x3F,0x2A,0x2A,0x2A,0x2A,0x2A,0x2A,0x20,    // ��
    0x3F,0x28,0x24,0x22,0x24,0x28,0x3F,0x20,    //
    0x2A,0x35,0x35,0x24,0x24,0x24,0x24,0x20,    //
    0x24,0x2E,0x35,0x35,0x35,0x2E,0x24,0x20,    //�
    0x35,0x35,0x35,0x35,0x2E,0x24,0x24,0x20,    //
    0x2E,0x31,0x31,0x31,0x2A,0x2A,0x3B,0x20,    //
    0x20,0x20,0x2D,0x32,0x32,0x32,0x2D,0x20,    //����� - �����

    0x2F,0x25,0x25,0x25,0x29,0x31,0x3F,0x31,    //� - \340
    0x31,0x31,0x31,0x31,0x31,0x31,0x3F,0x21,    //�
    0x35,0x35,0x35,0x35,0x35,0x35,0x3F,0x21,    //�
    0x20,0x20,0x2F,0x25,0x29,0x31,0x3F,0x31,    //�
    0x20,0x24,0x24,0x2E,0x35,0x35,0x2E,0x24,    //�
    0x20,0x20,0x31,0x31,0x31,0x31,0x3F,0x21,    //�
    0x20,0x20,0x35,0x35,0x35,0x35,0x3F,0x21,    //�
    0x20,0x22,0x35,0x35,0x2E,0x24,0x24,0x20,    //���������- �� \347
    0x20,0x2E,0x31,0x30,0x2C,0x22,0x2C,0x20,    //���������- �����(���) \350 ����� \377 0x20,0x20,0x2F,0x32,0x32,0x32,0x2C,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,    //

    0x2C,0x32,0x32,0x36,0x31,0x31,0x36,0x30,    // ��������� ����� \360 beta
    0x20,0x20,0x39,0x29,0x29,0x2A,0x24,0x24,    //	gamma
    0x26,0x29,0x24,0x2E,0x31,0x31,0x2E,0x20,    //
    0x20,0x20,0x2F,0x30,0x3E,0x30,0x2F,0x20,    //
    0x2F,0x22,0x2C,0x28,0x26,0x21,0x26,0x20,    //
    0x20,0x36,0x29,0x29,0x29,0x29,0x21,0x20,    //
    0x26,0x29,0x29,0x2F,0x29,0x29,0x26,0x20,    //
    0x20,0x20,0x24,0x24,0x24,0x24,0x23,0x20,    //
    0x20,0x20,0x32,0x34,0x38,0x34,0x32,0x20,    //�
    0x2C,0x24,0x24,0x24,0x2A,0x2A,0x31,0x20,    //
    0x20,0x20,0x32,0x32,0x32,0x3D,0x30,0x20,    //
    0x20,0x20,0x33,0x31,0x32,0x34,0x28,0x20,    //
    0x3F,0x24,0x28,0x27,0x28,0x30,0x2E,0x21,    //
    0x20,0x20,0x3F,0x2A,0x2A,0x2A,0x33,0x20,    //
    0x20,0x2E,0x31,0x31,0x31,0x3E,0x30,0x30,    // - ro
    0x20,0x20,0x2F,0x32,0x32,0x32,0x2C,0x20     // ���������- �����(���)\377
};

//  ������� ������ �� �������
#define TuStr   40
#define oS      1       // 1 ��������� �� ����������
#define bS      16+1    // 1 ��������� �� ����������
#define SRelay  32+1    // 1 ��������� �� ����������
#define SS      2       // 2 ��������� �� ����������
#define SSS     3       // 3 ��������� �� ����������
#define SSSi    16+3    // 3 ��������� �� ����������
#define SdS     32+3    // (3) �������� ��������� �� ����������
#define SpS     64+3    // 3 ��������� �� ����������
#define SSSS    4       // 4 ��������� �� ����������
#define SSpS0   16+4    // 4 ��������� �� ����������
#define SSpSS   5       // 5 ��������� �� ����������
#define SSdSS   16+5    // 5 ��������� �� ����������
#define SSsSS   32+5    // 5 ��������� �� ����������
#define SSSpS   16+32+5 // 5 ��������� �� ����������
#define SSSSS   48+64+5 // 5 ��������� �� ����������
#define S7B		7       // 7 ��������� �� ����������
#define SpSSpSS 16+7    // 7 ��������� �� ����������
#define DsMsY   8       // 8 ��������� �� ����������
#define mSSpS0  16+32+64+4
#define SpSSS   64+5
#define StStStS 15       /* 15 ��������� �� ����������*/


#define Str1    0
#define Str2d   40
#define Str2    80	//40
#define Str3    120	//80
#define Str4    160	//120
#define Str5    200

#define PozTime			23

#define MaxTimeOut		10	 //5
#define	DisplCols		40
#define TxtHomeAddr		0x0000
#define TxtArea			DisplCols		//40 �������
#define GrfHomeAddr		0x0200
#define GrfArea			DisplCols		//40 �������


#define BufSize			240	//160

char   lcdbuf[BufSize+10];  //TempEnd+6];
char GrafView;


void SendFirstScreen(char tmSec);
void ClearGraf(void);
void clear_d(void);
void InitLCD(void);
void Video(void);

const uchar FirstScreen[40*64]={
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


#endif

