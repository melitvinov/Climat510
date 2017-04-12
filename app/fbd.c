#define _FBD_C_

#include "syntax.h"

#include "timers.h"
#include "fieldbus.h"
//#include "module.h"
#include "fbd.h"
#include "debug.h"


static fbd_rt_t rt;

// make sure we fit u32 bitmap
PANIC_IF(N_MAX_MODULES >= 32);


static const sync_task_t sync_tasks_table[] =
{
    [SYNC_TASK_PULL_STATUS] =           {.req = req_pull_status,            .on_done = on_pull_status_done},
    [SYNC_TASK_PUSH_INPUT_CONFIG] =     {.req = req_push_input_config},
    [SYNC_TASK_PUSH_DISCRETE_OUTPUTS] = {.req = req_push_discrete_outputs},
    [SYNC_TASK_PULL_INPUTS] =           {.req = req_pull_inputs},
    [SYNC_TASK_PUSH_OUTPUTS] =          {.req = req_push_outputs},
};

// --- sync tasks AKA the channel programs

static u8 get_active_maddr(void)
{
    return rt.active_entry->addr % 100 + 120;
}


static void req_pull_status(void)
{
    LOG("pulling status");
    bool is_ok = fieldbus_request_read(get_active_maddr(), 0, 0, &rt.active_entry->status_word, sizeof(rt.active_entry->status_word));
    REQUIRE(is_ok);
}


static bool on_pull_status_done(void)
{
    if (rt.active_entry->status_word & 0x01)
    {
        rt.active_entry->sync_errs |= MODULE_ERR_RESET;
        return 0;
    }

    return 1;
}

static void req_push_input_config(void)
{
    LOG("pushing input config");

    // create linear array of inputs
    module_input_cfg_t buf[MODULE_MAX_MAX_N_INPUTS];

    for (uint i = 0; i < MODULE_MAX_MAX_N_INPUTS; i++)
    {
        const module_input_cfg_t *src = rt.active_entry->input_cfg_links.p[i];
        module_input_cfg_t *dst = &buf[i];
        if (src == NULL)
        {
            memclr(dst, sizeof(module_input_cfg_t));
        }
        else
        {
            *dst = *src;
            dst->output = 0;    // XXX: purpose of this field is unknown. set to 0
            #warning "investigate this field"
            dst->input = i + 1; // XXX: look like this field should be the 1-based number of input
        }
    }

    // make sure there is no alignment effects
    PANIC_IF(sizeof(buf) != (sizeof(module_input_cfg_t) * MODULE_MAX_MAX_N_INPUTS));

    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 2, &buf, sizeof(buf));
    REQUIRE(is_ok);
}


static void req_push_discrete_outputs(void)
{
    LOG("pushing out values");
    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 3, &rt.active_entry->discrete_outputs, sizeof(rt.active_entry->discrete_outputs));
    REQUIRE(is_ok);
}


static void req_push_outputs(void)
{
    LOG("pushing out regs");

    if (rt.active_entry->outputs[0].type == 0)
    {
        // just auto-complete transfer
        return;
    }

    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 8, &rt.active_entry->outputs, sizeof(rt.active_entry->outputs));
    REQUIRE(is_ok);
}


static void req_pull_inputs(void)
{
    LOG("pulling inputs");

    int last_configured_input = -1;

    for (uint i = 0; i < MODULE_MAX_MAX_N_INPUTS; i++)
    {
        if (rt.active_entry->input_cfg_links.p[i])
            last_configured_input = i;
    }

    if (last_configured_input < 0)
    {
        WARN("no inputs configured, ignoring read action");
        return;
    }

    bool is_ok = fieldbus_request_read(get_active_maddr(), 2, 0, rt.active_entry->inputs, sizeof(rt.active_entry->inputs[0]) * (last_configured_input + 1));
    REQUIRE(is_ok);
}

//--- interface to the module sync

static void finish(void)
{
    rt.is_syncing = 0;
}


static void do_next_action()
{
    // update requests
    u8 actions = rt.active_entry->pending_tasks;

    if (! actions)
    {
        finish();
        return;
    }

    // limit the number of actions per session to avoid the module blocking the bus for too long

//  LOG("actions is %d", actions);

    const sync_task_t *action = &sync_tasks_table[ctz(actions)];
    action->req();
    timer_start(&rt.sync_timer,  5, 1, check_progress);   // 5 ms poll period is ok
}


static void on_done(void)
{
    u8 actions = rt.active_entry->pending_tasks;
    const sync_task_t *action = &sync_tasks_table[ctz(actions)];

    rt.active_entry->pending_tasks = actions & (actions - 1); // remove completed action from list

    if (action->on_done)
    {
        bool should_continue = action->on_done();
        if (! should_continue)
        {
            finish();
            return;
        }
    }

    do_next_action();
}


static void check_progress(timer_t *dummy)
{
    fieldbus_status_t status = fieldbus_get_status();
    if (status == FIELDBUS_BUSY)
        return;

    timer_stop(&rt.sync_timer);

    if (status == FIELDBUS_IDLE)
    {
        on_done();
    }
    else
    {
        WARN("fieldbus err status %d", status);
        if (status == FIELDBUS_ERR_BAD_CHECKSUM)
            rt.active_entry->sync_errs |= MODULE_ERR_CHECKSUM;
        else
            rt.active_entry->sync_errs |= MODULE_ERR_LINK;
        finish();
    }
}


static void abort_sync(void)
{
    if (! rt.is_syncing)
        return;

    timer_stop(&rt.sync_timer);

    fieldbus_status_t status;

    while((status = fieldbus_get_status()) == FIELDBUS_BUSY);

    if (status == FIELDBUS_ERR_BAD_CHECKSUM)
        rt.active_entry->sync_errs |= MODULE_ERR_CHECKSUM;
    else
        rt.active_entry->sync_errs |= MODULE_ERR_LINK;
    finish();
}

static module_entry_t *next_entry(module_entry_t *after)
{
    uint entries = rt.used_entries;

    if (after)
    {
        uint idx = (after - rt.entries) + 1;
        entries = (entries >> idx) << idx;    // clear all bits before (and including) the start
    }

    if (! entries)
        return NULL;

    return &rt.entries[ctz(entries)];
}


static void prepare_fresh_entry(module_entry_t *e)
{
    e->pending_tasks = 0;
    e->requested_tasks = FULL_SYNC;

    e->stat.err_cnt = 0;
    e->stat.reset_cnt = 0;
    e->stat.status = 0;

    e->access_cycle = 0;
}


static void start_sync(module_entry_t *e)
{
    if (! e)
        return;

    if (e->access_cycle == 0)
        e->requested_tasks |= (1 << SYNC_TASK_PULL_STATUS) | (1 << SYNC_TASK_PULL_INPUTS);

    // put requested actions to 'fifo'
    e->pending_tasks |= e->requested_tasks;
    e->requested_tasks = 0;

    REQUIRE(! rt.is_syncing);

    rt.active_entry = e;
    rt.active_entry->sync_errs = 0;
    rt.is_syncing = 1;

    do_next_action();
}


static void end_sync(module_entry_t *e)
{
    uint err = e->sync_errs;

    if (err == 0)
    {
        // dequeue requests
        REQUIRE(e->pending_tasks == 0);
        e->stat.status &= ~MODULE_ERR_RESET; // this error is transient, clear after succesful sync
    }
    else
    {
        LOG("err status 0x%02x", err);

        // error, so do a full sync next time around
        // leave pending actions unchanged
        e->requested_tasks |= FULL_SYNC;

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
                rt.last_bad_module = e->addr;
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
    module_entry_t *e = rt.active_entry;

    if (e)
    {
        if (rt.is_syncing)
            return;

        end_sync(e);
    }

    e = next_entry(e);
    if (! e)
        e = next_entry(NULL);

    rt.active_entry = e;

    if (! e)
        return;

    start_sync(e);
}


module_entry_t *fbd_find_module_by_addr(uint addr)
{
    if (addr == 0) return NULL;

    for (u32 used = rt.used_entries; used; used &= used - 1)
    {
        uint pos = ctz(used);

        if (pos >= N_MAX_MODULES)
            break;

        if (rt.entries[pos].addr == addr)
            return &rt.entries[pos];
    }

    return NULL;
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


module_entry_t *fbd_next_module(module_entry_t *e)
{
    return next_entry(e);
}


void fbd_unmount_module(module_entry_t *m)
{
    if (! m)
        return;

    uint pos = m - rt.entries;
    REQUIRE(pos < N_MAX_MODULES);

    rt.used_entries &= ~(1U << pos);

    // brutally abort transaction if we're processing the entry in question now
    if ((rt.is_syncing) && (rt.active_entry) && ((uint)(rt.active_entry - rt.entries) == pos))
    {
        abort_sync();
        rt.active_entry = NULL;
    }
}


void fbd_write_discrete_outputs(module_entry_t *m, u32 val, u32 mask)
{
    u32 oldval = m->discrete_outputs;
    u32 newval = (oldval & ~mask) | (val & mask);

    if (newval != oldval)
    {
        m->discrete_outputs = newval;
        m->requested_tasks |= 1 << SYNC_TASK_PUSH_DISCRETE_OUTPUTS;
    }
}


void fbd_write_register(module_entry_t *m, uint reg_idx, uint type, uint val)
{
    if (reg_idx >= MODULE_MAX_N_OUTPUTS)
    {
        WARN("attempt to drive register >= MODULE_MAX_N_OUTPUTS");
        return;
    }

    m->outputs[reg_idx].type = type;
    m->outputs[reg_idx].val = val;

    m->requested_tasks |= 1 << SYNC_TASK_PUSH_OUTPUTS;
}


int fbd_read_input(module_entry_t *m, uint input_idx, u16 *val)
{
    if (input_idx >= MODULE_MAX_MAX_N_INPUTS)
    {
        WARN("attempt to read input >= MODULE_MAX_MAX_N_INPUTS");
        *val = 0;
        return -1;
    }

    *val = m->inputs[input_idx];
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

    m->input_cfg_links.p[input_idx] = cfg;
    m->requested_tasks |= 1 << SYNC_TASK_PUSH_INPUT_CONFIG;
}


void fbd_start(void)
{
    fieldbus_init();
    rt.active_entry = NULL;

    timer_start(&rt.poll_timer, 50, 1, fbd_task);
}


const module_stat_t *fbd_get_stat(const module_entry_t *m)
{
    return &m->stat;
}

uint fbd_get_addr(const module_entry_t *m)
{
    return m->addr;
}


u8 fbd_get_last_bad_module(void)
{
    return rt.last_bad_module;
}
