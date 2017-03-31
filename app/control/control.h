#ifndef _65_CONTROL_H_
#define _65_CONTROL_H_

#include "control_gd.h"

#ifdef _FRIEND_OF_CONTROL_

const zone_t make_zone_ctx(int zone_idx);
const contour_t make_contour_ctx(const zone_t *zone, int contour_idx);

#endif


void control_init(void);
void control_pre(void);
void control_post(int second, bool is_transfer_in_progress);

#endif
