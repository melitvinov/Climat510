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

// XXX: just for now
typedef struct module_entry_t module_entry_t;

typedef struct
{
    u8 status;
    u8 err_cnt;
    u8 reset_cnt;
} module_stat_t;


void fbd_start(void);
u8 fbd_get_last_bad_module(void);

module_entry_t *fbd_mount_module(uint addr);
void fbd_unmount_module(module_entry_t *m);

module_entry_t *fbd_find_module_by_addr(uint addr);
module_entry_t *fbd_next_module(module_entry_t *m);

void fbd_write_discrete_outputs(module_entry_t *m, u32 val, u32 mask);
void fbd_write_register(module_entry_t *m, uint reg_idx, uint type, uint val);

int fbd_read_input(module_entry_t *m, uint input_idx, u16 *val);
void fbd_configure_input(module_entry_t *m, uint input_idx, const module_input_cfg_t *cfg);

const module_stat_t *fbd_get_stat(const module_entry_t *m);
uint fbd_get_addr(const module_entry_t *m);

#endif

