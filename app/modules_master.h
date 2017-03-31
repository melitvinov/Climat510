#ifndef _MODULES_MASTER_H
#define _MODULES_MASTER_H

#include "module.h"

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



uint addr2base(uint nAddress);
char GetOutIPCDigit(uint16_t nAddress);
void SetOutIPCDigit(bool set, uint nAddress);

void SetOutIPCReg(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr);
void ClrAllOutIPCDigit(void);
uint16_t GetInIPC(uint16_t nAddress, s8 *nErr);
uint16_t GetDiskrIPC(uint16_t nAddress, s8 *nErr);
void UpdateInIPC(uint16_t nAddress, const module_input_cfg_t *cfg);
void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn, const uint16_t **fInputs);


void SendIPC(s8 *fErrModule);


#endif

