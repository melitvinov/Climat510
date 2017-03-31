#define _FRIEND_OF_CONTROL_

#include "syntax.h"
#include "control_gd.h"

#include "modules_master.h"

void write_output_bit(char fnTepl, char fnMech, char fnclr, char fnSm)
{
    uint16_t nBit;
    if (fnTepl == -1)
        nBit=fnMech;
    else
        nBit=_GD.MechConfig[fnTepl].RNum[fnMech];
    if (!nBit) return;

    if (addr2base(nBit))
        SetOutIPCDigit(!fnclr,nBit+fnSm);
}

void write_output_register(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr)
{
    SetOutIPCReg(How, fType, nAddress, nErr);
}
