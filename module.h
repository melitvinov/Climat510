#ifndef _MODULE_H_
#define _MODULE_H_

#define MODULE_MAX_N_RELAYS	        32
#define MODULE_MAX_MAX_N_INPUTS	    32
#define MODULE_MAX_N_OUTPUTS	    8

#define MODULE_ERR_LINK				0x02
#define MODULE_ERR_CHECKSUM			0x04

#define MODULE_TRANSIENT_ERR_MASK	0xF0
#define MODULE_ERR_RESET			0x40
#define MODULE_ERR_NEED_RESET		0x80

typedef struct __packed
{
    u32 type;
    u8 pulse[MODULE_MAX_N_RELAYS][2];
} module_output_cfg_t;

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

typedef struct __packed
{
    u8 actual;
    u16 cond;
    u16 actual_speed;
    u16 actual_power;
    u16 actual_pwm;
} module_fandata_t;

typedef struct __packed
{
    u8 type;
    u16 val;
} module_output_register_t;

typedef struct
{
    module_output_cfg_t outputs_cfg;                            // outputs config
    module_output_register_t outputs[MODULE_MAX_N_OUTPUTS];     // outputs registers (analog/continuous outputs ?)
    module_input_cfg_t inputs_cfg[MODULE_MAX_MAX_N_INPUTS];     // inputs config
    module_fandata_t *fandata;

    u32 discrete_outputs;       // bitmap of discrete outputs (relays)
    u16 CpM;                                // some kind of module address
    u16 inputs[MODULE_MAX_MAX_N_INPUTS];    // input values
    u8 cond;                    // some conditions aka status
    u8 max_n_inputs;            // max number of used inputs
    u8 max_n_outputs;           // max number of used outputs
    u8 err_cnt;                 // error counter
    u8 fail_cnt;                // failures counter
} module_t;


void module_processor_run(module_t *module, bool is_reduced_sync, u8 nfan);
void module_processor_periodic(void);
bool module_processor_is_busy(void);

#endif
