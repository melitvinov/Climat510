#ifndef _65_SCREEN_H_
#define _65_SCREEN_H_

#ifdef _FRIEND_OF_CONTROL_

void SetPosScreen(const gh_t *gh, char screen_type);
void SetReg(const gh_t *gh, char fHSmReg,int DoValue,int MeasValue);
void LaunchVent(const gh_t *gh);
void LaunchCalorifer(const gh_t *gh);
void InitScreen(const gh_t *gh, char typScr);

void RegWorkDiskr(const gh_t *gh, char fHSmReg);
void MidlWindAndSr(void);

#endif


#endif
