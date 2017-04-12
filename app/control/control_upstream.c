#define _FRIEND_OF_CONTROL_

#include "syntax.h"
#include "control_gd.h"
#include "fbd.h"
#include "debug.h"

static void write_output_bit(uint zone_idx, uint mech_idx, bool set, uint addr_offset)
{
    uint addr = _GD.MechConfig[zone_idx].RNum[mech_idx];

    if (! addr)
        return;

    addr += addr_offset;

    board_t *b = fbd_find_board_by_addr(addr / 100);

    if (! b)
        return;

    uint bit_idx = addr % 100 - 1;
    if (bit_idx > 31)
    {
        WARN("attempt to set bit > 31");
        return;
    }

    fbd_write_discrete_outputs(b, set ? ~0U : 0, 1U << bit_idx);
}

void output_on(uint zone_idx, uint mech_idx, uint addr_offset)
{
    write_output_bit(zone_idx, mech_idx, 1, addr_offset);
}

void output_off(uint zone_idx, uint mech_idx, uint addr_offset)
{
    write_output_bit(zone_idx, mech_idx, 0, addr_offset);
}

void write_output_register(uint addr, uint type, uint val)
{
    board_t *b = fbd_find_board_by_addr(addr / 100);
    if (! b)
        return;

    fbd_write_register(b, addr % 100 - 1, type, val);
}
