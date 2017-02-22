#ifndef _65_SCREEN_H_
#define _65_SCREEN_H_

typedef struct
{
    int16_t             Value;
    int8_t              Mode;
    int8_t              OldMode;
    int16_t             Pause;
    int16_t             PauseMode;
//		int16_t				TimeChangeMode;
//		int16_t				TempStart;
} eScreen;


void SetPosScreen(char typScr);
void SetReg(char fHSmReg,int DoValue,int MeasValue);
void LaunchVent(char fnTepl);
void LaunchCalorifer(char fnTepl);
void InitScreen(char typScr, char fnTepl);

void RegWorkDiskr(char fHSmReg);
void MidlWindAndSr(void);


#endif
