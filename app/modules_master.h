#ifndef _MODULES_MASTER_H
#define _MODULES_MASTER_H

#include "module.h"

#define ERR_MASTER_NOANSWER		1
#define ERR_MASTER_WRONGANSWER	2
#define ERR_MASTER_TXHEAD		3
#define ERR_MASTER_HEADNOSUM	4
#define ERR_MASTER_HEADWRSUM	5
#define ERR_MASTER_TXDATA		6
#define ERR_MASTER_DATANOSUM	7
#define ERR_MASTER_DATAWRSUM	8
#define ERR_MASTER_RXDATA		9
#define ERR_MASTER_RXDATA		9

#define ERR_MODULE_NOSENSOR        	0x01
#define ERR_MODULE_INNUM			0x08
//Все остальные аварии не стираются

#define iMODULE_MAX_ERR				100
#define iMODULE_MAX_FAILURES		125


#define		cmt0_5V		0
#define 	cmt4_20mA	1
#define		cmtAD592	2
#define		cmt1Wire	3
#define 	cmtEC		4
#define		cmtT_EC		5
#define		cmtPH1		6
#define		cmtPH2		7
#define		cmt0_3V		8
#define		cmtWater	9
#define		cmtWeigth	10
#define		cmtSun		11

#define N_MAX_MODULES 	30

/*------------ Признаки обмена данных --------*/
#define OUT_UNIT        0x50
#define IN_UNIT         0xa0
#define WORK_UNIT       0x70




typedef void(*CallBackRS)(void);

uint16_t GetIPCComMod(uint16_t nAddress);
uint16_t GetIPCNum(uint16_t nAddress);
char GetOutIPCDigit(uint16_t nAddress);
void SetOutIPCDigit(char How, uint16_t nAddress);

void SetOutIPCReg(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr, module_fandata_t *fandata);
void ClrAllOutIPCDigit(void);
uint16_t GetInIPC(uint16_t nAddress, s8 *nErr);
uint16_t GetDiskrIPC(uint16_t nAddress, s8 *nErr);
void UpdateInIPC(uint16_t nAddress, const module_input_cfg_t *cfg);
void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn,uint16_t **fInputs);

int16_t IMOD_WriteOutput(char COMPort,int nModule, uint32_t Values);





void SendIPC(s8 *fErrModule);



#endif

