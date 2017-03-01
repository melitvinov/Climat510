#define _DEBUG_C_

#include "syntax.h"
//#include "hal_sys.h"
#include "hal_debug.h"
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

//void read_stack(wstream_t *resp)
//{
//    u32 size;
//    u32 used;
//    const char *name;
//
//    uint i = 0;
//    while ((name = HAL_sys_get_stackifo(i++, &size, &used)))
//    {
//        fw_stream_format(resp, "%s stack usage: \n %8lu of %8lu bytes are used \n\t (%lu%%)", name, used, size, (used * 100) / size);
//    }
//}
