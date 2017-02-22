#ifndef _65_SUBR_H_
#define _65_SUBR_H_

int CorrectionRule(int fStartCorr,int fEndCorr, int fCorrectOnEnd, int fbSet);
void WindDirect(void);
void airHeatTimers(void);

void CheckMidlSr(void);
char CheckSeparate (char fnKontur);
char CheckMain(char fnTepl);

void airHeat(char fnTepl);

int8_t getTempHeatAlarm(char fnTepl);
int8_t getTempVentAlarm(char fnTepl);

int16_t getTempHeat(char fnTepl);
int16_t getTempVent(char fnTepl);
void SetPointersOnKontur(char fnKontur);
void SetPointersOnTepl(char fnTepl);

int clamp_min(int f_in, int f_gr);
int clamp_max(int f_in, int f_gr);
bool SameSign(int Val1,int Val2);

#endif
