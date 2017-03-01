#ifndef _HAL_DEBUG_H_
#define _HAL_DEBUG_H_

void HAL_assert(const char *str) __noreturn;
void hal_exception(u32 pc, const u32 *sp);

#endif
