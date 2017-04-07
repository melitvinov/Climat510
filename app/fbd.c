#include "syntax.h"

#include "timers.h"
#include "fieldbus.h"
#include "module.h"
#include "fbd.h"
#include "debug.h"


#define FULL_SYNC ((1 << _MODULE_ACTION_IDX_LAST) - 1)

typedef struct module_entry_t module_entry_t;

struct module_entry_t
{
    module_t module;

    u8 access_cycle;

    u8 status;

    u8 err_cnt;                 // count of comm error
    u8 reset_cnt;               // count of seen resets

    u8 requested_actions;       // bitmap of actions requested for module before the sync cycle
    u8 pending_actions;         // bitmap of pending (uncompleted) actions for module
};


typedef struct
{
    timer_t poll_timer;
    module_entry_t entries[N_MAX_MODULES];
    module_entry_t *active_entry;
    bool is_fieldbus_inited;
    u8 last_bad_module;
} fbd_rt_t;

static fbd_rt_t rt;

void ClrAllOutIPCDigit(void)
{
    int i;
    for (i=0; i< N_MAX_MODULES; i++)
    {
        // ok, so the zero module terminates the list
        if (! rt.entries[i].module.addr)
            return;
        rt.entries[i].module.discrete_outputs = 0;
        rt.entries[i].requested_actions |= MODULE_ACTION_PUSH_DISCRETE_OUTPUTS;
    }
}


static module_entry_t *find_entry_by_addr(uint addr)
{
    if (addr == 0) return NULL;

    for (uint i = 0; i < countof(rt.entries); i++)
    {
        module_entry_t *e =  &rt.entries[i];

        if (e->module.addr == addr)
            return e;
    }

    return NULL;
}

static void prepare_fresh_entry(module_entry_t *e)
{
    e->requested_actions |= FULL_SYNC;

    e->err_cnt = 0;
    e->reset_cnt = 0;
    e->access_cycle = 0;
    e->status = 0;
}

static module_entry_t *alloc_entry(uint addr)
{
    if (addr == 0) return NULL;

    module_entry_t *e = find_entry_by_addr(addr);
    if (e)
        return e;

    for (uint i=0; i < countof(rt.entries); i++)
    {
        module_entry_t *e =  &rt.entries[i];

        if (e->module.addr == 0)
        {
            LOG("allocated entry for module %d", addr);

            e->module.addr = addr;
            prepare_fresh_entry(e);
            return e;
        }
    }

    return NULL;
}


void SetOutIPCDigit(uint addr, uint output_idx, bool set)
{
    if (addr == 0)
        return;

    if (output_idx > 31)
    {
        WARN("attempt to drive discrete output > 31");
        return;
    }

    module_entry_t *e = alloc_entry(addr);
    if (! e)
        return;

    LOG("updating bit");

    u32 mask = 1U << output_idx;

    if (set)
        e->module.discrete_outputs |= mask;
    else
        e->module.discrete_outputs &= ~(mask);

    e->requested_actions |= MODULE_ACTION_PUSH_DISCRETE_OUTPUTS;
}


void SetOutIPCReg(uint addr, uint reg_idx, uint type, uint val)
{
    if (addr == 0)
        return;

    if (reg_idx >= MODULE_MAX_N_OUTPUTS)
    {
        WARN("attempt to drive register >= MODULE_MAX_N_OUTPUTS");
        return;
    }

    module_entry_t *e = alloc_entry(addr);
    if (! e)
        return;

    e->module.outputs[reg_idx].type = type;
    e->module.outputs[reg_idx].val = val;

    e->requested_actions |= MODULE_ACTION_PUSH_OUTPUTS;

    return;
}


// GUESS: read binary value from the cached data (relay pin output)
char GetOutIPCDigit(uint addr, uint output_idx)
{
    if (addr == 0)
        return -1;
    if (output_idx > 31)
        return -1;

    const module_entry_t *e = find_entry_by_addr(addr);
    if (! e)
        return -1;

    return (e->module.discrete_outputs & (1U << output_idx)) ? 1 : 0;
}


// GUESS: read analog values from the cached data
uint16_t GetInIPC(uint addr, uint input_idx, s8 *nErr)
{
    if (addr == 0)
    {
        *nErr = -1;
        return 0;
    }

    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to read input >= MODULE_MAX_MAX_N_INPUTS");
        *nErr = -1;
        return 0;
    }

    const module_entry_t *e = alloc_entry(addr);

    #warning "4444 ? WTF ?"
    if (! e)
    {
        *nErr=0;
        return 4444;
    }

    *nErr = e->err_cnt;
    return e->module.inputs[input_idx];
}


// GUESS: read the the discrete input from cached data

#warning "totally remove it"
uint16_t GetDiskrIPC(uint addr, uint input_idx)
{
    if (addr == 0)
        return 0;

    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to read input >= MODULE_MAX_MAX_N_INPUTS");
        return 0;
    }

    const module_entry_t *e = alloc_entry(addr);

    if (! e)
        return 0;

    #warning "> 2500 ? wtf ?"s
    if ((e->module.inputs[input_idx] > 2500) && (e->err_cnt < iMODULE_MAX_ERR))
        return 1;
    else
        return 0;
}


// GUESS: resets the input config in cached data and schedule the module update
void UpdateInputConfig(uint addr, uint input_idx, const module_input_cfg_t *cfg)
{
    if (addr == 0)
        return;

    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to configure input >= MODULE_MAX_MAX_N_INPUTS");
        return;
    }

    module_entry_t *e = alloc_entry(addr);

    if (! e)
        return;

    LOG("updating input");

    e->module.input_cfg_links.p[input_idx] = cfg;
    e->requested_actions |= MODULE_ACTION_PUSH_INPUT_CONFIG;
}


void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn, const uint16_t **fInputs)
{
    REQUIRE(nMod < countof(rt.entries));
    const module_entry_t *e = &rt.entries[nMod];

    *fCpM=e->module.addr;
    #warning "disabled n_used_inputs"
//  *fMaxIn=e->module.n_used_inputs;
    *fInputs=e->module.inputs;

    #warning "status report is broken"
    *fCond=e->status;
    *fErr=e->err_cnt;
    *fFail=e->reset_cnt;
}


static module_entry_t *next_entry(module_entry_t *e)
{
    if (! e)
    {
        e = &rt.entries[0];
    }
    else
    {
        e++;
        if (e >= &endof(rt.entries) || e->module.addr == 0)
            e = &rt.entries[0];
    }

    return e->module.addr != 0 ? e : NULL;
}


static void start_sync(module_entry_t *e)
{
    if (! e)
        return;

    if (e->access_cycle == 0)
        e->requested_actions |= MODULE_ACTION_PULL_STATUS | MODULE_ACTION_PULL_INPUTS;

    // put requested actions to 'fifo'
    e->pending_actions |= e->requested_actions;
    e->requested_actions = 0;

    module_sync_run(&e->module, e->pending_actions);
}


static void end_sync(module_entry_t *e)
{
    uint err = module_sync_get_err();

    if (err == 0)
    {
        // dequeue requests
        e->pending_actions = 0;
        e->status &= ~MODULE_ERR_RESET; // this error is transient, clear after succesful sync
    }
    else
    {
        LOG("err status 0x%02x", err);

        // error, so do a full sync next time around
        e->requested_actions |= FULL_SYNC;

        // stat resets
        if (err & MODULE_ERR_RESET)
        {
            e->reset_cnt++;
            if (e->reset_cnt > iMODULE_MAX_FAILURES)
                e->reset_cnt = iMODULE_MAX_FAILURES;
        }

        // stat link errors
        if (err & (MODULE_ERR_CHECKSUM | MODULE_ERR_LINK))
        {
            e->err_cnt++;
            if (e->err_cnt > iMODULE_MAX_ERR)
            {
                e->err_cnt = iMODULE_MAX_ERR;
                rt.last_bad_module = e->module.addr;
            }
        }

        // latch errors
        e->status |= err;
    }

    #warning "ccycle is seriously broken"

    e->access_cycle = (e->access_cycle + 1) % 10;
}


static void fbd_task(timer_t *dummy)
{
    if (rt.active_entry)
    {
        if (module_sync_is_busy())
            return;

        end_sync(rt.active_entry);
    }

    rt.active_entry = next_entry(rt.active_entry);

    if (! rt.active_entry)
        return;

    start_sync(rt.active_entry);
}

void fbd_output_discrete_val(uint address, bool val)
{

}

void fbd_start(void)
{
    if (! rt.is_fieldbus_inited)
    {
         fieldbus_init();
         rt.is_fieldbus_inited = 1;
    }

    timer_start(&rt.poll_timer, 50, 1, fbd_task);
}


u8 fbd_get_last_bad_module(void)
{
    return rt.last_bad_module;
}
