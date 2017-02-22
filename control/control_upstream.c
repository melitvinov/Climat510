#include "syntax.h"
#include "control_gd.h"

#include "stm32f10x_RS485Master.h"

void write_output_bit(char fnTepl, char fnMech, char fnclr, char fnSm)
{
    uint16_t nBit,nByte;
    u8 Mask;
    if (fnTepl == -1)
        nBit=fnMech;
    else
        nBit=GD.MechConfig[fnTepl].RNum[fnMech];
    if (!nBit) return;
    if (GetIPCComMod(nBit))
    {
        SetOutIPCDigit(!fnclr,nBit+fnSm,&Mask);
        return;
    }
}

void write_output_register(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr,void* Ptr)
{
    SetOutIPCReg(How, fType, nAddress, nErr, Ptr);
}
