#define _FRIEND_OF_CONTROL_

#include "syntax.h"
#include "control_gd.h"

#include "fbd.h"

static void write_output_bit(uint zone_idx, uint mech_idx, bool set, uint addr_offset)
{
    uint addr = _GD.MechConfig[zone_idx].RNum[mech_idx];

    if (! addr)
        return;

    addr += addr_offset;

    SetOutIPCDigit(addr / 100, addr % 100 - 1, set);
}

void output_on(uint zone_idx, uint mech_idx, uint addr_offset)
{
    write_output_bit(zone_idx, mech_idx, 0, addr_offset);
}

void output_off(uint zone_idx, uint mech_idx, uint addr_offset)
{
    write_output_bit(zone_idx, mech_idx, 1, addr_offset);
}

void write_output_register(uint addr, uint type, uint val)
{
    SetOutIPCReg(addr / 100, addr % 100 - 1, type, val);
}
