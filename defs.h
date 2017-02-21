#ifndef _DEFS_H_
#define _DEFS_H_

#define cConfSTepl		6
//#define BIGLCD
#ifdef AHU
#define cModification	16+4
#else
#define cModification	16+1
#endif

/*Модификация ПО контроллера*/

#define NumCtr          1

#define cDefLanguage	1


#ifndef cLightDelay
    #define cLightDelay		40
#endif

#endif
