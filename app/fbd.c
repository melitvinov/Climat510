#include "syntax.h"

#include "timers.h"
#include "fieldbus.h"
#include "module.h"
#include "fbd.h"
#include "debug.h"


#define FULL_SYNC ((1 << _MODULE_ACTION_IDX_LAST) - 1)

struct module_entry_t
{
    module_t module;
    module_stat_t stat;

    u8 access_cycle;

    u8 requested_actions;       // bitmap of actions requested for module before the sync cycle
    u8 pending_actions;         // bitmap of pending (uncompleted) actions for module
};

typedef struct
{
    timer_t poll_timer;
    module_entry_t entries[N_MAX_MODULES];
    u32 used_entries;
    int active_idx;
    u8 last_bad_module;
} fbd_rt_t;

static fbd_rt_t rt;

// make sure we fit u32 bitmap
PANIC_IF(N_MAX_MODULES >= 32);

static int next_entry(int start)
{
    uint entries = rt.used_entries;

    if (start >= 0)
    {
        start += 1;
        entries = (entries >> start) << start;    // clear all bits before (and including) the start
    }

    if (! entries)
        return -1;

    return ctz(entries);
}


module_entry_t *fbd_find_module_by_addr(uint addr)
{
    if (addr == 0) return NULL;

    for (u32 used = rt.used_entries; used; used &= used - 1)
    {
        uint pos = ctz(used);

        if (pos >= N_MAX_MODULES)
            break;

        if (rt.entries[pos].module.addr == addr)
            return &rt.entries[pos];
    }

    return NULL;
}

static void prepare_fresh_entry(module_entry_t *e)
{
    e->requested_actions |= FULL_SYNC;

    e->stat.err_cnt = 0;
    e->stat.reset_cnt = 0;
    e->stat.status = 0;

    e->access_cycle = 0;
}

module_entry_t *fbd_mount_module(uint addr)
{
    if (addr == 0) return NULL;

    module_entry_t *e = fbd_find_module_by_addr(addr);
    if (e)
        return e;

    u32 used = rt.used_entries;
    REQUIRE(used != ~0U);

    u32 pos = ctz(~used);
    if (pos >= N_MAX_MODULES)
    {
        WARN("no mem to mount module");
        return NULL;
    }

    rt.used_entries |= 1U << pos;
    e = &rt.entries[pos];
    prepare_fresh_entry(e);
    return e;
}


module_entry_t *fbd_next_module(module_entry_t *m)
{
    int start = m ? (m - rt.entries) : -1;
    int pos = next_entry(start);
    return pos >= 0 ? &rt.entries[pos] : NULL;
}


void fbd_unmount_module(module_entry_t *m)
{
    if (! m)
        return;

    uint pos = m - rt.entries;
    REQUIRE(pos < N_MAX_MODULES);

    rt.used_entries &= ~(1U << pos);

    // brutally abort transaction if we're processing the entry in question now
    if (rt.active_idx == (int)pos && module_sync_is_busy())
    {
        module_sync_abort();
        rt.active_idx = -1;
    }
}


void fbd_write_discrete_outputs(module_entry_t *m, u32 val, u32 mask)
{
    u32 oldval = m->module.discrete_outputs;
    u32 newval = (oldval & ~mask) | (val & mask);

    if (newval != oldval)
    {
        m->module.discrete_outputs = newval;
        m->requested_actions |= MODULE_ACTION_PUSH_DISCRETE_OUTPUTS;
    }
}


void fbd_write_register(module_entry_t *m, uint reg_idx, uint type, uint val)
{
    if (reg_idx >= MODULE_MAX_N_OUTPUTS)
    {
        WARN("attempt to drive register >= MODULE_MAX_N_OUTPUTS");
        return;
    }

    m->module.outputs[reg_idx].type = type;
    m->module.outputs[reg_idx].val = val;

    m->requested_actions |= MODULE_ACTION_PUSH_OUTPUTS;
}


int fbd_read_input(module_entry_t *m, uint input_idx, u16 *val)
{
    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to read input >= MODULE_MAX_MAX_N_INPUTS");
        *val = 0;
        return -1;
    }

    *val = m->module.inputs[input_idx];
    return m->stat.err_cnt;
}

void fbd_configure_input(module_entry_t *m, uint input_idx, const module_input_cfg_t *cfg)
{
    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to configure input >= MODULE_MAX_MAX_N_INPUTS");
        return;
    }

    LOG("updating input");

    m->module.input_cfg_links.p[input_idx] = cfg;
    m->requested_actions |= MODULE_ACTION_PUSH_INPUT_CONFIG;
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
        e->stat.status &= ~MODULE_ERR_RESET; // this error is transient, clear after succesful sync
    }
    else
    {
        LOG("err status 0x%02x", err);

        // error, so do a full sync next time around
        e->requested_actions |= FULL_SYNC;

        // stat resets
        if (err & MODULE_ERR_RESET)
        {
            e->stat.reset_cnt++;
            if (e->stat.reset_cnt > iMODULE_MAX_FAILURES)
                e->stat.reset_cnt = iMODULE_MAX_FAILURES;
        }

        // stat link errors
        if (err & (MODULE_ERR_CHECKSUM | MODULE_ERR_LINK))
        {
            e->stat.err_cnt++;
            if (e->stat.err_cnt > iMODULE_MAX_ERR)
            {
                e->stat.err_cnt = iMODULE_MAX_ERR;
                rt.last_bad_module = e->module.addr;
            }
        }

        // latch errors
        e->stat.status |= err;
    }

    #warning "ccycle is seriously broken"

    e->access_cycle = (e->access_cycle + 1) % 10;
}


static void fbd_task(timer_t *dummy)
{
    int idx = rt.active_idx;

    if (idx >= 0)
    {
        if (module_sync_is_busy())
            return;

        end_sync(&rt.entries[idx]);
    }

    idx = next_entry(idx);
    if (idx < 0)
        idx = next_entry(-1);

    rt.active_idx = idx;

    if (idx < 0)
        return;

    start_sync(&rt.entries[idx]);
}


void fbd_start(void)
{
    fieldbus_init();
    rt.active_idx = -1;

    timer_start(&rt.poll_timer, 50, 1, fbd_task);
}


const module_stat_t *fbd_get_stat(const module_entry_t *m)
{
    return &m->stat;
}

uint fbd_get_addr(const module_entry_t *m)
{
    return m->module.addr;
}


u8 fbd_get_last_bad_module(void)
{
    return rt.last_bad_module;
}
