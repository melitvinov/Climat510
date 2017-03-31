#ifndef _65_SCREEN_H_
#define _65_SCREEN_H_

#ifdef _FRIEND_OF_CONTROL_

void SetPosScreen(const zone_t *zone, char screen_type);
void SetReg(const zone_t *zone, char fHSmReg,int DoValue,int MeasValue);
void LaunchVent(const zone_t *zone);
void LaunchCalorifer(const zone_t *zone);
void InitScreen(const zone_t *zone, char typScr);

void RegWorkDiskr(const zone_t *zone, char fHSmReg);
void MidlWindAndSr(void);

#endif


#endif
