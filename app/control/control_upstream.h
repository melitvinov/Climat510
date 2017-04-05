#ifndef _CONTROL_UPSTREAM_H_
#define _CONTROL_UPSTREAM_H_

void output_on(uint zone_idx, uint mech_idx, uint addr_offset);
void output_off(uint zone_idx, uint mech_idx, uint addr_offset);
void write_output_register(uint val, uint type, uint addr);

#endif
