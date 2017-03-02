#ifndef _CLIMDEFSTUFF_H_
#define _CLIMDEFSTUFF_H_

// XXX: for eAddrGd
#include "simple_server.h"

typedef struct
{
    bool    Menu;

    unsigned char   Second;

    uint16_t    PORTNUM;
    eAdrGD      AdrGD[15];
    uchar       NumBlock;

    uchar SostRS;
} wtf0_t;

extern wtf0_t wtf0;

#endif
