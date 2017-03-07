#define _HAL_NVMEM_C_

#include "syntax.h"
#include "hal_i2c.h"
#include "hal_nvmem.h"

#include "debug.h"


void HAL_nvmem_init(void)
{
    hal_i2c_init();
}

bool HAL_nvmem_read(u32 base, void *dst, uint size)
{
    u32 end = base + size;
    REQUIRE(base < HAL_NVMEM_SIZE);
    REQUIRE(end <= HAL_NVMEM_SIZE);

    const u8 addr[2] = {base >> 8, base};
    return hal_i2c_read(FSM24V02_ADDR, addr, sizeof(addr), dst, size);
}

bool HAL_nvmem_write(u32 base, const void *src, uint size)
{
    u32 end = base + size;
    REQUIRE(base < HAL_NVMEM_SIZE);
    REQUIRE(end <= HAL_NVMEM_SIZE);

    const u8 addr[2] = {base >> 8, base};
    return hal_i2c_write(FSM24V02_ADDR, addr, sizeof(addr), src, size);
}


void HAL_nvmem_smoke(void)
{
    HAL_nvmem_init();

    const u32 pagesize = 256;

    LOG("writing ... ");
    for (uint page = 0; page < HAL_NVMEM_SIZE / pagesize; page++)
    {
        u8 buf[pagesize];
        memset(buf, page, sizeof(buf));
        bool is_ok = HAL_nvmem_write(page * pagesize, buf, sizeof(buf));
        REQUIRE(is_ok);
    }

    LOG("reading ... ");
    for (uint page = 0; page < HAL_NVMEM_SIZE / pagesize; page++)
    {
        u8 buf[pagesize];
        bool is_ok = HAL_nvmem_read(page * pagesize, buf, sizeof(buf));
        REQUIRE(is_ok);
        for (uint i = 0; i < sizeof(buf); i++)
            REQUIRE(buf[i] == page);
    }
}
