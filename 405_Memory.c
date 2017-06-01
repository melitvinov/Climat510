#define ADDRESS_FRAM_SUM	31000 //(sizeof(GD)+sizeof(eBlockEEP)*SUM_BLOCK_EEP)
//================= ПРОЕКТ 403 =====================
/*-------------------------------------------------
        Начальна инициализаци при старте процессора
---------------------------------------------------*/
#ifndef STM32_UNIT
void init8051(void) {
        ClrDog;
        P2=0;
        TMOD=0x21;      /* перегружаемый 8 разрдный T1 и 16 разрд T0 */
        TCON=0x55;      /* запуск таймеров T0,T1 и по фронту INT0,INT1 */
        IP=0x15;        /* приоритет PS,INT0,INT1 */
        TH0=0xff;
/* Инициализаци последовательного порта */
 /*        ET1=0;         запрет прерываний таймера 1*/
        SCON=0xF0;      /*режим 3 (9бит УАПП)*/
        TMOD=0x21;      /*Таймер 1 с перезагрузкой , Таймер 0 - 16 разр*/
        TH1=0xFD;       /*Скорость 9600bps с кварцем 11.059 МГц */
        TR1=1;          /*Таймер 1 - включить счет*/
        PS=1;           /*Приоритет порта высший*/
/*        ES=1;           Разрешит прерыван послед порта*/
/* установить порт1 на вывод и С0-С7 на ввод и порт2 на вывод */
        Port1=0;
        adru=u_port1;
        adra=0;
        Port1=1;
        Port2=0;
        adru=u_port2;
        Port2=1;
        ClrDog;
}
#endif 

//=================================================
//          Подпрограммы работы с памятью
//=================================================
// ------------------------------------------------
//               Очистка памти
//-------------------------------------------------
void MemClr(void *pp1,uint32_t n) {
        char *p;
        p=pp1;
        while(n--) *p++=0;
        ClrDog;
		}
void MemCopy(char *pp1, char *pp2, uint32_t n) { //dest,source,number
        while(n--) *pp1++ = *pp2++;
        ClrDog;
		}

uchar   nBlEEP;

#ifndef STM32_UNIT
typedef struct eBlockEEP {
        int AdrCopyRAM;
        uint    Size;
        uint    CSum;
        char    Erase;
        };
		

int		AdrRAM;	  //int
#else
uchar*	AdrRAM;

#endif
	
uint    AdrEEP;
uint    SizeEEP;
char    WriteEEP;


uint16_t CalcRAMSum(uchar* fAddr,uint32_t fSize)
{
	uint tSum,i;
	tSum=0x0110;
	for (i = 0; i < fSize; i++)
	{
		tSum+=fAddr[i];
		tSum%=65000;
	}
	return tSum;
}


/* Ввод с клавиатуры - проверка адреса в области сохранения
   и запись в EEPROM с контрольной суммой*/
void    SetInSaveRam(void) {
    	uint16_t cSum;
    	uint16_t     vVal;
		uint8_t	nBlFRAM;
		if(!SizeEEP) return;
         AdrEEP=1;
         for(nBlFRAM=0; nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++){
          ClrDog;
          vVal=AdrRAM-BlockEEP[nBlFRAM].AdrCopyRAM;
          if((vVal>=0)&&(vVal<BlockEEP[nBlFRAM].Size)){
              AdrEEP+=vVal;  /* если в области вычисляем адрес*/
              SendBlockFRAM((uint32_t)(AdrRAM)-(uint32_t)(BlockEEP[0].AdrCopyRAM),AdrRAM,SizeEEP);
              //I2C_Mem_Write(0,);
              cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
              //I2C_Mem_Write(0,);
              SendBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&cSum,2);
              BlockEEP[nBlFRAM].CSum=cSum;
              BlockEEP[nBlFRAM].Erase=2; /* обнулить счетчик сбросов*/
              ClrDog;
              return;
              }
          AdrEEP+=(BlockEEP[nBlFRAM].Size+2);
          }
     	//SendBlockFRAM(sizeof(GD),&BlockEEP,sizeof(BlockEEP));
}

/*------ проверка контр суммы блока CONTROL ---------------------------*/
char	TestRAM0(void) {
        ClrDog;
        uint16_t cSum;
    	RecvBlockFRAM(ADDRESS_FRAM_SUM,&BlockEEP[0].CSum,2);
        cSum=CalcRAMSum(BlockEEP[0].AdrCopyRAM,BlockEEP[0].Size);
        if(cSum!=BlockEEP[0].CSum) return 2;
/*если контр сумма неверна, то установить признак обнуления*/
        return 0;
}

/*------ проверка контр суммы ОЗУ и при ошибке return 2 --*/
char TestRAM(void) {
    	uint8_t nBlFRAM;
        for(nBlFRAM=0; nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++){
          ClrDog;
 /*---если неверна контр сумма установить сброс----*/
          RecvBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&BlockEEP[nBlFRAM].CSum,2);
          if(BlockEEP[nBlFRAM].CSum!=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size)) return 2;
          }
		  return 0;
}

/*-- Восстановление из EEPROM, а при ошибке перезапись в EEPROM------*/
void	TestFRAM(char EraseBl)
{
	    uint16_t cSum;
	    uint8_t nBlFRAM;
        for(nBlFRAM=0;nBlFRAM < SUM_BLOCK_EEP; nBlFRAM++)
        {
          RecvBlockFRAM(BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
          RecvBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&BlockEEP[nBlFRAM].CSum,2);
          cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
          ClrDog;
          if ( (CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size)!=BlockEEP[nBlFRAM].CSum ) || ( BlockEEP[nBlFRAM].Erase == 1) )
          {
            InitGD(5);
            SendBlockFRAM(BlockEEP[nBlFRAM].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            cSum=CalcRAMSum(BlockEEP[nBlFRAM].AdrCopyRAM,BlockEEP[nBlFRAM].Size);
            SendBlockFRAM(ADDRESS_FRAM_SUM+nBlFRAM*2,&cSum,2);
            BlockEEP[nBlFRAM].CSum=cSum;
            BlockEEP[nBlFRAM].Erase=0; //++
          }
          	  /* формирование контр суммы ОЗУ после инициализации */
        }
}

void ReadBlockFRAM(int block)
{
	uint16_t cSum;
	//RecvBlockFRAM(BlockEEP[block].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[block].AdrCopyRAM,BlockEEP[block].Size);

	RecvBlockFRAM(BlockEEP[block].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[block].AdrCopyRAM,BlockEEP[block].Size);
	//RecvBlockFRAM((uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[0].AdrCopyRAM,BlockEEP[0].Size);

	//cSum=CalcRAMSum(BlockEEP[block].AdrCopyRAM,BlockEEP[block].Size);
	//RecvBlockFRAM(ADDRESS_FRAM_SUM+block*2,&BlockEEP[block].CSum,2);
    ClrDog;
    BlockEEP[block].CSum=cSum;
    BlockEEP[block].Erase=0;
}

/*-- Была запись с ПК в блок NumBlock ,
     цикл перезаписи блока куда была передача ----*/
void ReWriteFRAM(void) {

	     uint16_t cSum;
         ClrDog;
         ByteX=NumBlock-1;
         SendBlockFRAM(BlockEEP[ByteX].AdrCopyRAM-(uint32_t)(BlockEEP[0].AdrCopyRAM),BlockEEP[ByteX].AdrCopyRAM,BlockEEP[ByteX].Size);
         cSum=CalcRAMSum(BlockEEP[ByteX].AdrCopyRAM,BlockEEP[ByteX].Size);
         SendBlockFRAM(ADDRESS_FRAM_SUM+ByteX*2,&cSum,2);
         BlockEEP[ByteX].CSum=cSum;
     	//SendBlockFRAM(sizeof(GD),&BlockEEP,sizeof(BlockEEP));

}/*------------------------------------------------
        Тестирование и установка данных при сбросе "Тест"
        и ежесекундно
TipRes
0-мягкий старт-уст адресов и проверка Control
1-кнопка ТЕСТ и вкл питания
2-инициализация RAM - GD
10+2-инициализация EEP- Программы
10+1-инициализация EEP- Калибровки
10+0-инициализация EEP- Параметры
--------------------------------------------------*/
void TestMem(uchar TipReset) {
	   ClrDog;
       InitBlockEEP();  /*подпрограмма в GD */
	   ButtonReset();
//	   TipReset=2;
/*------ проверка контр суммы блока CONTROL ---------------------------*/
       if(TipReset>5) InitGD(5);
       if((!Menu) && TestRAM0())
    	   TipReset=2;
       if(!TipReset) return;
/*------ проверка контр суммы ОЗУ  -------------------------------*/

       ClrDog;
       if((TipReset==1)&& TestRAM())
    	   TipReset++;
       if (TipReset<2) return;
       Menu=0;
       ClrDog;

/*-- Восстановление из EEPROM, а при ошибке перезапись в EEPROM------*/
       TestFRAM(TipReset);
       ClrDog;
       ButtonReset();
       GetRTC();
}



