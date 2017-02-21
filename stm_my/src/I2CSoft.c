//;======= I2C Soft ========================================
//========================================================================
#include "stm32f10x_gpio.h"
#include <stm32f10x_tim.h>
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "I2CSoft.h"

#define 	i2_SetSDA		i2_Port->BSRR = i2_SDA_Pin
#define 	i2_ClrSDA		i2_Port->BRR  = i2_SDA_Pin
#define 	i2_SetSCL		i2_Port->BSRR = i2_SCL_Pin
#define 	i2_ClrSCL		i2_Port->BRR  = i2_SCL_Pin
#define		i2_GetSDA	   (i2_Port->IDR & i2_SDA_Pin)

#define I2C_GETACK   timeout=100; while((i2_GetSDA)&&(timeout)) timeout--;

//*********************************************************************************************


//TMems* pMems;

void i2_fm_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = i2_SCL_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(i2_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = i2_SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//GPIO_Mode_IPU;
    GPIO_Init(i2_Port, &GPIO_InitStructure);

}

void i2_Start(void)
{
    i2_SetSDA;
    i2_SetSCL;
    i2_ClrSDA;
    i2_ClrSCL;
}
void i2_Stop(void)
{
    i2_ClrSDA;
    i2_SetSCL;
    i2_SetSDA;
}

void i2_ToSDA(u8 data)
{
    if (data) i2_SetSDA;
    else i2_ClrSDA;
}

void i2_SetASK(u8 ask)
{
    i2_ClrSCL;
//	i2_ClrSDA;
    if (ask) i2_ClrSDA;
    else i2_SetSDA;
    i2_SetSCL;
//	i2_SetSCL;
    i2_ClrSCL;
}

u16 i2_GetASK(void)
{
    u8 timeout;
//	i2_ClrSDA;
    i2_ClrSCL;
    i2_SetSDA;
    i2_SetSCL;
//	i2_SetSCL;
    I2C_GETACK;
    //u16 res=i2_GetSDA;
    i2_ClrSCL;
    if (!timeout) return 1;
    return 0;
}

void i2_WriteByte(int8_t data)
{
    u8 i;
    i2_ClrSCL;
    for (i=0;i<8;i++)
    {
        i2_ToSDA(data & 0x80);
        data<<=1;
        i2_SetSCL;
        i2_ClrSCL;
    }
//	i2_SetSDA;
}
u8 i2_ReadByte(void)
{
    u8 i,j,data=0;
    i2_ClrSCL;
    i2_SetSDA;
    for (i=0;i<8;i++)
    {
        i2_SetSCL;
        data<<=1;
//Задержка чтения
        for (j=0;j<4;j++)
            if (i2_GetSDA) data|=1;
//Окончание задержки чтения
        i2_ClrSCL;
    }
//	i2_ClrSDA;
    return data;
}

void fm_Read(u16 adr, void *buf,u16 size)
{
    u16 i;
    u8 d,sum;

    u8 *p = buf;

    if (!size) return;
    i2_Start();
    i2_WriteByte(FM_ADR);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
//Исправлена передача старшего бита адреса в память

    i2_WriteByte(adr>>8);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
//Исправлена передача младшего бита адреса в память
    i2_WriteByte(adr&0xff);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
    i2_Start();
    i2_WriteByte(FM_ADR|1);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
    for (i=0;i<size;i++)
    {
        d=i2_ReadByte();
        *(p++)=d;
        if (i==size-1)
            i2_SetASK(0);
        else
            i2_SetASK(1);//!(i==size));
    }
    i2_Stop();
}

void fm_Write(u16 adr, const void *buf,u16 size)
{

    const u8 *p = buf;

    u16 i;
    u8 d;
    if (!size) return;
    i2_Start();
    i2_WriteByte(FM_ADR);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
//Исправлена передача старшего бита адреса в память
    i2_WriteByte(adr>>8);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
//Исправлена передача младшего бита адреса в память
    i2_WriteByte(adr&0xff);
    if (i2_GetASK())
    {
        i2_Stop();
        return;
    }
    for (i=0;i<size;i++)
    {
        d=*(p++);
        i2_WriteByte(d);
        if (i2_GetASK())
        {
            i2_Stop();
            return;
        }
    }
    i2_Stop();
}
