#include "stm32f10x_Define.h"
#include "keyboard.h"

#define PORT_KEYB_IN	GPIOA
#define PORT_KEYB_OUT	GPIOC
#define KEYB_IN		0x000f
#define KEYB_OUT	0x000f
#define KEYB_STARTUP RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

static uint8_t KeyDelay;
static bit BITKL;
static KEY_PRESSED SIM;

#define NOP asm("nop")

void keyboardSetBITKL(uchar press)
{
    BITKL = press;
}

uchar keyboardGetBITKL(void)
{
    return BITKL;
}

void keyboardSetSIM(KEY_PRESSED key)
{
    SIM = key;
}

KEY_PRESSED keyboardGetSIM(void)
{
    return SIM;
}

void Keyboard_Init(void)
{
    KEYB_STARTUP;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = KEYB_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT_KEYB_OUT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEYB_IN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(PORT_KEYB_IN, &GPIO_InitStructure);
}

// непонятно зачем вообще что то возвращать, если это не используется
char KeyboardProcess(void)
{
    int i;
    uint16_t sendB;
    uint16_t u16Temp=0;
    Keyboard_Init();
    if (KeyDelay>1)
    {
        KeyDelay--;return 0;
    }

    for (i=0;i<16;i++)
    {

        //GPIO_Write(PORT_KEYB_OUT, u16Temp);
        u16Temp = GPIO_ReadOutputData(PORT_KEYB_OUT)&(~KEYB_OUT);
        u16Temp |=  0x000f&~(1<<(i%4));//Внимательно если маска не 0xff нужно преобразование
        GPIO_Write(PORT_KEYB_OUT, u16Temp);
        NOP;NOP;

        sendB=i/4;
        sendB=0x01<<sendB;
        u16Temp= GPIO_ReadInputData(PORT_KEYB_IN)&KEYB_IN;
        if (!(u16Temp&sendB))
        {
            SIM=(KEY_PRESSED)i; BITKL=1;
            KeyDelay=4;
            return 1;
        }

    }
    return 0;
}

