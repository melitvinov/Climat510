// ==  ==  ==  ==  ==  ==  ==  == = ПРОЕКТ 403  ==  ==  ==  ==  ==  ==  ==  ==  ==  == =

#include "syntax.h"
#include "405_memory.h"
#include "hal_nvmem.h"

eBlockEEP BlockEEP[SUM_BLOCK_EEP];

// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  == =
//          Подпрограммы работы с памятью
// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  == =

uint16_t CalcRAMSum(uchar* fAddr,uint32_t fSize)
{
    uint tSum,i;
    tSum = 0x0110;
    for (i = 0; i < fSize; i++)
    {
        tSum+=fAddr[i];
        tSum%=65000;
    }
    return tSum;
}


void SendBlockFRAM(uint16_t fStartAddr, const void *AdrBlock,uint16_t sizeBlock)
{
    uint16_t i,fSS;
    //I2C_Mem_Write(0,fStartAddr,AdrBlock,sizeBlock);
    for (i = 0;i<sizeBlock/2000+1;i++)
    {
        fSS = 2000;
        if (i == sizeBlock/2000)
            fSS = sizeBlock%2000;
        HAL_nvmem_write(fStartAddr+i*2000, (const u8 *)AdrBlock+i*2000,fSS);
    }

//	I2C_MainLoad(0,AdrBlock,AdrBlock,I2C_TP_MEM,sizeBlock,I2C_Direction_Transmitter);
}

void RecvBlockFRAM(uint16_t fStartAddr, void *AdrBlock,uint16_t sizeBlock)
{
    uint16_t i,fSS;

    //I2C_Mem_Read(0,fStartAddr,AdrBlock,sizeBlock);
    //I2C_MainLoad(0xA0,0,AdrBlock,I2C_TP_MEM,sizeBlock,I2C_Direction_Receiver);
    for (i = 0;i<sizeBlock/2000+1;i++)
    {
        fSS = 2000;
        if (i == sizeBlock/2000)
            fSS = sizeBlock%2000;

        HAL_nvmem_read(fStartAddr+i*2000, (u8 *)AdrBlock+i*2000,fSS);
    }
}

/* Ввод с клавиатуры - проверка адреса в области сохранения
   и запись в EEPROM с контрольной суммой*/
void SetInSaveRam(void *addr, uint SizeEEP)
{
    uint16_t cSum;
    uint16_t     vVal;
    uint8_t nBlFRAM;
    if (!SizeEEP) return;
    uint AdrEEP = 1;
    uchar *AdrRAM = addr;
    for (nBlFRAM = 0; nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++)
    {
        vVal = AdrRAM-(u8 *)BlockEEP[nBlFRAM].AdrCopyRAM;
        if ((vVal>=0)&&(vVal<BlockEEP[nBlFRAM].Size))
        {
            AdrEEP+=vVal;  /* если в области вычисляем адрес*/
            SendBlockFRAM((uint32_t)(AdrRAM)-(uint32_t)(BlockEEP[0].AdrCopyRAM),AdrRAM,SizeEEP);
            //I2C_Mem_Write(0,);
            cSum = CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            //I2C_Mem_Write(0,);
            SendBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&cSum,2);
            BlockEEP[nBlFRAM].CSum = cSum;
            BlockEEP[nBlFRAM].Erase = 2; /* обнулить счетчик сбросов*/
            return;
        }
        AdrEEP+=(BlockEEP[nBlFRAM].Size+2);
    }
    //SendBlockFRAM(sizeof(GD),&BlockEEP,sizeof(BlockEEP));
}

/*------ проверка контр суммы блока CONTROL ---------------------------*/
char TestRAM0(void)
{
    uint16_t cSum;
    RecvBlockFRAM(ADDRESS_FRAM_SUM,&BlockEEP[0].CSum,2);
    cSum = CalcRAMSum(BlockEEP[0].AdrCopyRAM,BlockEEP[0].Size);
    if (cSum!=BlockEEP[0].CSum) return 2;
/*если контр сумма неверна, то установить признак обнуления*/
    return 0;
}

/*------ проверка контр суммы ОЗУ и при ошибке return 2 --*/
char TestRAM(void)
{
    uint8_t nBlFRAM;
    for (nBlFRAM = 0; nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++)
    {
        /*---если неверна контр сумма установить сброс----*/
        RecvBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&BlockEEP[nBlFRAM].CSum,2);
        if (BlockEEP[nBlFRAM].CSum!=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size)) return 2;
    }
    return 0;
}


/*-- Была запись с ПК в блок NumBlock ,
     цикл перезаписи блока куда была передача ----*/
void ReWriteFRAM(int numblock)
{

    uint16_t cSum;
    int block_idx = numblock-1;
    SendBlockFRAM((uint32_t)BlockEEP[block_idx].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[block_idx].AdrCopyRAM,BlockEEP[block_idx].Size);
    cSum = CalcRAMSum(BlockEEP[block_idx].AdrCopyRAM,BlockEEP[block_idx].Size);
    SendBlockFRAM(ADDRESS_FRAM_SUM+block_idx*2,&cSum,2);
    BlockEEP[block_idx].CSum = cSum;
    //SendBlockFRAM(sizeof(GD),&BlockEEP,sizeof(BlockEEP));

}

