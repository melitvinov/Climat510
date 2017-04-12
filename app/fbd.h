#ifndef _FBD_H_
#define _FBD_H_

//#include "module.h"

#define N_MAX_MODULES 	30

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

typedef struct
{
    u8 status;
    u8 err_cnt;
    u8 reset_cnt;
} module_stat_t;

typedef struct __packed
{
    u8 type;
    u8 input;
    u8 output;
    u8 corr;
    u16 u0;
    u16 v0;
    u16 u1;
    u16 v1;
} module_input_cfg_t;

// XXX: just for now
typedef struct module_entry_t module_entry_t;

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

#ifdef _FBD_C_
// private

#define FULL_SYNC ((1 << _SYNC_TASK_LAST) - 1)

#define MODULE_MAX_MAX_N_INPUTS	    32
#define MODULE_MAX_N_OUTPUTS	    8

#define MODULE_ERR_LINK				0x02
#define MODULE_ERR_CHECKSUM			0x04

#define MODULE_ERR_RESET			0x40

// sync tasks are sorted by the execution order
enum sync_tasks_e
{
    SYNC_TASK_PUSH_INPUT_CONFIG,
    SYNC_TASK_PULL_STATUS,
    SYNC_TASK_PUSH_DISCRETE_OUTPUTS,
    SYNC_TASK_PUSH_OUTPUTS,
    SYNC_TASK_PULL_INPUTS,
    _SYNC_TASK_LAST,
};


typedef struct __packed
{
    const module_input_cfg_t *p[MODULE_MAX_MAX_N_INPUTS];
} module_input_cfg_links_t;

typedef struct __packed
{
    u8 type;
    u16 val;
} module_output_register_t;

struct module_entry_t
{
    module_output_register_t outputs[MODULE_MAX_N_OUTPUTS];         // outputs registers (analog/continuous outputs ?)
    module_input_cfg_links_t input_cfg_links;                       // pointers to input configs
    u32 discrete_outputs;                                           // bitmap of discrete outputs (relays)
    u16 addr;                                                       // module base address
    u16 status_word;                                                // module status word
    u16 inputs[MODULE_MAX_MAX_N_INPUTS];                            // input values

    module_stat_t stat;

    u8 access_cycle;
    u8 sync_errs;

    u8 requested_tasks;       // bitmap of tasks requested for module before the sync cycle
    u8 pending_tasks;         // bitmap of pending (uncompleted) tasks for module
};

typedef struct
{
    void (* const req)(void);
    bool (* const on_done)(void);
} sync_task_t;


typedef struct
{
    timer_t poll_timer;

    module_entry_t entries[N_MAX_MODULES];
    u32 used_entries;

    module_entry_t *active_entry;
    timer_t sync_timer;

    bool is_syncing;
    u8 last_bad_module;
} fbd_rt_t;

static void req_pull_status(void);
static bool on_pull_status_done(void);
static void req_push_input_config(void);
static void req_push_discrete_outputs(void);
static void req_push_outputs(void);
static void req_pull_inputs(void);

static void check_progress(timer_t *dummy);

static void abort_sync(void);

#endif
#endif

