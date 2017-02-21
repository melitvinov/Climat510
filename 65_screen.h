#ifndef _65_SCREEN_H_
#define _65_SCREEN_H_

typedef struct eeScreen
{
    int16_t             Value;
    int8_t              Mode;
    int8_t              OldMode;
    int16_t             Pause;
    int16_t             PauseMode;
//		int16_t				TimeChangeMode;
//		int16_t				TempStart;
} eScreen;




#endif
