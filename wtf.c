
#include "syntax.h"

#include "debug.h"
#include "wtf.h"

wtf0_t wtf0;

void show_native_addr(void)
{
    LOG("showing native addresses");
    LOG("wtf 0x%08x", (int)wtf0.AdrGD);
    LOG("block 0 0x%x", (int)wtf0.AdrGD[0].Adr);
    LOG("block 1 0x%x", (int)wtf0.AdrGD[1].Adr);
    LOG("block 2 0x%x", (int)wtf0.AdrGD[2].Adr);
    LOG("block 3 0x%x", (int)wtf0.AdrGD[3].Adr);
}

