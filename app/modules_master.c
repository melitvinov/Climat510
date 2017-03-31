#include "syntax.h"

#include "module.h"
#include "modules_master.h"
#include "debug.h"


#define FULL_SYNC ((1 << _MODULE_ACTION_IDX_LAST) - 1)

typedef struct
{
    module_t module;

    u8 is_first_sync;           // just a hacky flag to ignore a first reset

    u8 access_cycle;

    u8 status;

    u8 err_cnt;                 // count of comm error
    u8 reset_cnt;               // count of seen resets

    u8 requested_actions;       // bitmap of actions requested for module before the sync cycle
    u8 pending_actions;         // bitmap of pending (uncompleted) actions for module
} module_entry_t;


typedef struct
{
     module_entry_t entries[N_MAX_MODULES];
     module_entry_t *active_entry;
} master_rt_t;

static master_rt_t rt;

uint addr2base(uint nAddress)
{
    return nAddress/100;
}

static uint addr2resource(uint nAddress)
{
    return nAddress%100;
}


void ClrAllOutIPCDigit(void)
{
    int i;
    for (i=0; i< N_MAX_MODULES; i++)
    {
        // ok, so the zero module terminates the list
        if (! rt.entries[i].module.base)
            return;
        rt.entries[i].module.discrete_outputs = 0;
        rt.entries[i].requested_actions |= MODULE_ACTION_PUSH_DISCRETE_OUTPUTS;
    }
}


static module_entry_t *find_entry_by_base(uint base)
{
    if (base == 0) return 0;

    for (uint i = 0; i < countof(rt.entries); i++)
    {
        module_entry_t *e =  &rt.entries[i];

        if (e->module.base == base)
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
    e->is_first_sync = 1;
}

static module_entry_t *alloc_entry(uint base)
{
    if (base == 0) return NULL;

    module_entry_t *e = find_entry_by_base(base);
    if (e)
        return e;

    for (uint i=0; i < countof(rt.entries); i++)
    {
        module_entry_t *e =  &rt.entries[i];

        if (e->module.base == 0)
        {
            LOG("allocated entry for module %d", base);

            e->module.base = base;
            prepare_fresh_entry(e);
            return e;
        }
    }

    return NULL;
}


void SetOutIPCDigit(bool set, uint nAddress)
{
    uint base = addr2base(nAddress);

    module_entry_t *e = alloc_entry(base);
    if (! e)
        return;

    LOG("updating bit");

    u32 mask = 1U << (addr2resource(nAddress) - 1);

    if (set)
        e->module.discrete_outputs |= mask;
    else
        e->module.discrete_outputs &= ~(mask);

    e->requested_actions |= MODULE_ACTION_PUSH_DISCRETE_OUTPUTS;
}


void SetOutIPCReg(uint16_t How, uint8_t fType, uint16_t nAddress, char* nErr)
{
    uint base = addr2base(nAddress);
    module_entry_t *e = alloc_entry(base);
    if (! e)
        return;

    u32 idx = addr2resource(nAddress)-1;
    if (idx >= MODULE_MAX_N_OUTPUTS)
        return;

    *nErr=e->err_cnt;

    e->module.outputs[idx].val=How;
    e->module.outputs[idx].type= fType;

    e->requested_actions |= MODULE_ACTION_PUSH_OUTPUTS;

    return;
}


// GUESS: read binary value from the cached data (relay pin output)
char GetOutIPCDigit(uint16_t nAddress)
{
    uint base = addr2base(nAddress);
    if (! base) return -1;

    const module_entry_t *e = find_entry_by_base(base);
    if (! e)
        return -1;

    u32 mask = 1U << (addr2resource(nAddress) - 1);
    if (e->module.discrete_outputs & mask)
        return 1;
    else
        return 0;
}


// GUESS: read analog values from the cached data
uint16_t GetInIPC(uint16_t nAddress, s8 *nErr)
{
//TODO
    uint base=addr2base(nAddress);
    if (!base)
    {
        *nErr=-1;
        return 0;
    }

    uint idx = addr2resource(nAddress);
    if (!idx)
    {
        *nErr=-1;
        return 0;
    }

    const module_entry_t *e = alloc_entry(base);

    #warning "4444 ? WTF ?"
    if (! e)
    {
        *nErr=0;
        return 4444;
    }

    *nErr = e->err_cnt;
    return e->module.inputs[idx-1];
}


// GUESS: read the the discrete input from cached data
uint16_t GetDiskrIPC(uint16_t nAddress, s8 *nErr)
{
    uint base=addr2base(nAddress);
    if (!base)
    {
        *nErr=-1;
        return 0;
    }

    uint idx = addr2resource(nAddress);
    if (!idx)
    {
        *nErr=-1;
        return 0;
    }

    const module_entry_t *e = alloc_entry(base);

    if (! e)
    {
        *nErr=0;
        return 0;
    }

    *nErr=e->err_cnt;
    if ((e->module.inputs[idx-1]>2500) && (e->err_cnt < iMODULE_MAX_ERR))
        return 1;
    else
        return 0;
}


// GUESS: resets the input config in cached data and schedule the module update
void UpdateInIPC(uint16_t nAddress, const module_input_cfg_t *cfg)
{
    uint vInput = addr2resource(nAddress);
    if (! vInput)
        return;

    uint base = addr2base(nAddress);
    module_entry_t *e = alloc_entry(base);

    if (! e)
        return;

    LOG("updating input");

//			if ((NoSameBuf(((char*)(&ModulData[i].InConfig[vInput-1]))+2,((char*)ModulConf)+2,2/*sizeof(TIModulConf)-2*/)) //·ÂÁ Í‡ÎË·Ó‚ÓÍ
//				||(ModulData[i].InConfig[vInput-1].Type!=ModulConf->Type))
    memcpy(&e->module.inputs_cfg[vInput-1], cfg, sizeof(module_input_cfg_t));
    //********************** Õ¿ƒŒ ”¡–¿“‹ *****************************
    for (uint k=0;k < 32;k++)
    {

        //ModulData[i].InConfig[k].Type=3;
        e->module.inputs_cfg[k].input=k+1;
    }
    //****************************************************************

    if (e->module.max_n_inputs<vInput)
        e->module.max_n_inputs = vInput;

    e->requested_actions |= MODULE_ACTION_PUSH_INPUT_CONFIG;
}


void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn, const uint16_t **fInputs)
{
    REQUIRE(nMod < countof(rt.entries));
    const module_entry_t *e = &rt.entries[nMod];

    *fCpM=e->module.base;
    *fMaxIn=e->module.max_n_inputs;
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
        if (e >= &endof(rt.entries) || e->module.base == 0)
            e = &rt.entries[0];
    }

    return e->module.base != 0 ? e : NULL;
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


static void end_sync(module_entry_t *e, s8 *exterr)
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
            if (e->is_first_sync)
            {
                err &= ~MODULE_ERR_RESET;   // ignore reset for the just-allocated module, it's expected
            }
            else
            {
                e->reset_cnt++;
                if (e->reset_cnt > iMODULE_MAX_FAILURES)
                    e->reset_cnt = iMODULE_MAX_FAILURES;
            }
        }

        // stat link errors
        if (err & (MODULE_ERR_CHECKSUM | MODULE_ERR_LINK))
        {
            e->err_cnt++;
            if (e->err_cnt > iMODULE_MAX_ERR)
            {
                e->err_cnt = iMODULE_MAX_ERR;
                *exterr = e->module.base % 100;
            }
        }

        // latch errors
        e->status |= err;
    }

    e->is_first_sync = 0;

    #warning "ccycle is seriously broken"
    #warning "exterr is broken too"

    e->access_cycle = (e->access_cycle + 1) % 10;
}


void SendIPC(s8 *fErrModule)
{
    if (rt.active_entry)
    {
        if (module_sync_is_busy())
            return;

        end_sync(rt.active_entry, fErrModule);
    }

    rt.active_entry = next_entry(rt.active_entry);

    if (! rt.active_entry)
        return;

    start_sync(rt.active_entry);
}
