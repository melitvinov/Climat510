#ifndef _MODULE_H_
#define _MODULE_H_

#define MODULE_MAX_MAX_N_INPUTS	    32
#define MODULE_MAX_N_OUTPUTS	    8

#define MODULE_ERR_LINK				0x02
#define MODULE_ERR_CHECKSUM			0x04

#define MODULE_ERR_RESET			0x40

// actions are sorted by the execution order
enum module_action_idx_e
{
    _MODULE_ACTION_IDX_PUSH_INPUT_CONFIG,
    _MODULE_ACTION_IDX_PULL_STATUS,
    _MODULE_ACTION_IDX_PUSH_DISCRETE_OUTPUTS,
    _MODULE_ACTION_IDX_PUSH_OUTPUTS,
    _MODULE_ACTION_IDX_PULL_INPUTS,
    _MODULE_ACTION_IDX_LAST,
};


enum module_action_e
{
    MODULE_ACTION_PUSH_INPUT_CONFIG     = (1U << _MODULE_ACTION_IDX_PUSH_INPUT_CONFIG),
    MODULE_ACTION_PULL_STATUS           = (1U << _MODULE_ACTION_IDX_PULL_STATUS),
    MODULE_ACTION_PUSH_DISCRETE_OUTPUTS = (1U << _MODULE_ACTION_IDX_PUSH_DISCRETE_OUTPUTS),
    MODULE_ACTION_PUSH_OUTPUTS          = (1U << _MODULE_ACTION_IDX_PUSH_OUTPUTS),
    MODULE_ACTION_PULL_INPUTS           = (1U << _MODULE_ACTION_IDX_PULL_INPUTS),
};


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

// array of pointers to the configs
typedef struct __packed
{
    const module_input_cfg_t *p[MODULE_MAX_MAX_N_INPUTS];
} module_input_cfg_links_t;

typedef struct __packed
{
    u8 type;
    u16 val;
} module_output_register_t;




typedef struct
{
    // 'module mem'
    module_output_register_t outputs[MODULE_MAX_N_OUTPUTS];         // outputs registers (analog/continuous outputs ?)
    module_input_cfg_links_t input_cfg_links;                       // pointers to input configs
    u32 discrete_outputs;                                           // bitmap of discrete outputs (relays)
    u16 inputs[MODULE_MAX_MAX_N_INPUTS];                            // input values
    u16 status_word;                                                // module status word
    u16 addr;                                                       // module base address
} module_t;


void module_sync_run(module_t *module, u8 requested_actions);
bool module_sync_is_busy(void);
uint module_sync_get_err(void);
void module_sync_abort(void);

#endif
