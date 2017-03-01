#ifndef _HAL_RTC_H_
#define _HAL_RTC_H_

void HAL_rtc_init(void);
u32 HAL_rtc_get(void);
void HAL_rtc_set(u32 timestamp);

#endif
