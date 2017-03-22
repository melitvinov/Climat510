#define _DEBUG_C_

#include "syntax.h"
#include "hal.h"
#include "debug.h"

void assertion(const char *desc)
{
    HAL_assert(desc);
}

void print_dump(const void *data, uint len)
{
    const u8 *p = data;
    uint col = 0;
    const uint max_cols = 4;

    while (col < len)
    {
        if (!(col % max_cols))
            printf("\n%08x:", col);
        printf(" %02x", *p);
        p++;
        col++;
    }
    printf("\n");
}
