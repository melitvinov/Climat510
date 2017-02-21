#ifndef _CLIMDEFSTUFF_H_
#define _CLIMDEFSTUFF_H_

// XXX: for eAddrGd
#include "simple_server.h"

typedef struct
{
    bool    bSec;
    bool    Menu;

    unsigned char   Second;

    uint16_t    PORTNUM;
    eAdrGD      AdrGD[15];
    uchar       NumBlock;
} climdefstuff_t;

extern climdefstuff_t WTF0;

#endif