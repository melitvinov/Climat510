#ifndef _HAL_NVMEM_H_
#define _HAL_NVMEM_H_

#define HAL_NVMEM_SIZE  32768

void HAL_nvmem_init(void);
bool HAL_nvmem_read(u32 base, void *dst, uint size);
bool HAL_nvmem_write(u32 base, const void *src, uint size);

#ifdef _HAL_NVMEM_C_

#define FSM24V02_ADDR   0x50

#endif
#endif
