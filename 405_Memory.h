#ifndef _405_MEMORY_H_
#define _405_MEMORY_H_

#define SUM_BLOCK_EEP	8

#define ADDRESS_FRAM_SUM	31000 //(sizeof(GD)+sizeof(eBlockEEP)*SUM_BLOCK_EEP)

#ifndef NULL
    #define NULL	((void*) 0)
#endif

typedef struct
{
    void*      AdrCopyRAM;
    uint16_t    Size;
    uint16_t    CSum;
    int8_t      Erase;
} eBlockEEP;

extern eBlockEEP BlockEEP[SUM_BLOCK_EEP];

void MemClr(void *pp1,uint32_t n);
uint16_t CalcRAMSum(uchar* fAddr,uint32_t fSize);

void SetInSaveRam(void *addr, uint SizeEEP);
char TestRAM(void);
void ReWriteFRAM(int numblock);

void SendBlockFRAM(uint16_t fStartAddr, const void *AdrBlock,uint16_t sizeBlock);
void RecvBlockFRAM(uint16_t fStartAddr, void *AdrBlock,uint16_t sizeBlock);

#endif
