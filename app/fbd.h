#ifndef _FBD_H_
#define _FBD_H_

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


void SetOutIPCDigit(uint addr, uint output_idx, bool set);
void SetOutIPCReg(uint addr, uint reg_idx, uint type, uint val);
char GetOutIPCDigit(uint addr, uint output_idx);
uint16_t GetInIPC(uint addr, uint input_idx, s8 *nErr);
void ClrAllOutIPCDigit(void);
uint16_t GetDiskrIPC(uint addr, uint input_idx);
void UpdateInputConfig(uint addr, uint input_idx, const module_input_cfg_t *cfg);

void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn, const uint16_t **fInputs);


void fbd_start(void);
u8 fbd_get_last_bad_module(void);

#endif

