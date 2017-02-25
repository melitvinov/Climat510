#ifndef _65_SUBR_H_
#define _65_SUBR_H_

#ifdef _FRIEND_OF_CONTROL_

int CorrectionRule(int fStartCorr,int fEndCorr, int fCorrectOnEnd, int fbSet);
void WindDirect(void);

int16_t getTempHeat(int fnTepl);

void CheckMidlSr(void);
char CheckSeparate (const contour_t *ctx);
char CheckMain(const contour_t *ctr);

int8_t getTempHeatAlarm(int fnTepl);
int8_t getTempVentAlarm(int fnTepl);

int16_t getTempVent(int fnTepl);
void SetPointersOnKontur(int fnKontur);
void SetPointersOnTepl(int fnTepl);

bool SameSign(int Val1,int Val2);

#endif

#endif
