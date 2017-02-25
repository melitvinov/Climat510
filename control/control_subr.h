#ifndef _65_SUBR_H_
#define _65_SUBR_H_

#ifdef _FRIEND_OF_CONTROL_

int CorrectionRule(int fStartCorr,int fEndCorr, int fCorrectOnEnd, int fbSet);
void WindDirect(void);

void CheckMidlSr(void);
char CheckSeparate (const contour_t *ctx);
char CheckMain(const contour_t *ctr);

int16_t getTempHeat(const gh_t *gh, int fnTepl);
int8_t getTempHeatAlarm(const gh_t *gh, int fnTepl);
int8_t getTempVentAlarm(const gh_t *gh, int fnTepl);

int16_t getTempVent(const gh_t *gh, int fnTepl);

bool SameSign(int Val1,int Val2);

#endif

#endif
