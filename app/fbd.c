#define _FBD_C_

#include "syntax.h"

#include "timers.h"
#include "fieldbus.h"
#include "fbd.h"
#include "debug.h"


static fbd_rt_t rt;

// make sure we fit u32 bitmap
PANIC_IF(MAX_N_BOARDS >= 32);


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
    return rt.active_board->addr % 100 + 120;
}

static void req_pull_status(void)
{
    LOG(" -> status");
    bool is_ok = fieldbus_request_read(get_active_maddr(), 0, 0, &rt.active_board->status_word, sizeof(rt.active_board->status_word));
    REQUIRE(is_ok);
}


static bool on_pull_status_done(void)
{
    if (rt.active_board->status_word & 0x01)
    {
        rt.active_board->last_sync_errs |= ERR_RESET;
        return 0;
    }

    return 1;
}

static void req_push_input_config(void)
{
    LOG(" <- input config");

    // create linear array of inputs
    board_input_cfg_t buf[MAX_N_INPUTS];

    for (uint i = 0; i < MAX_N_INPUTS; i++)
    {
        const board_input_cfg_t *src = rt.active_board->input_cfgs[i];
        board_input_cfg_t *dst = &buf[i];
        if (src == NULL)
        {
            memclr(dst, sizeof(board_input_cfg_t));
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
    PANIC_IF(sizeof(buf) != (sizeof(board_input_cfg_t) * MAX_N_INPUTS));

    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 2, &buf, sizeof(buf));
    REQUIRE(is_ok);
}


static void req_push_discrete_outputs(void)
{
    LOG(" <- relays (0x%04x)", (uint)rt.active_board->discrete_outputs);
    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 3, &rt.active_board->discrete_outputs, sizeof(rt.active_board->discrete_outputs));
    REQUIRE(is_ok);
}


static void req_push_outputs(void)
{
    LOG(" <- outputs");

    if (rt.active_board->outputs[0].type == 0)
    {
        // just auto-complete transfer
        return;
    }

    bool is_ok = fieldbus_request_write(get_active_maddr(), 0, 8, &rt.active_board->outputs, sizeof(rt.active_board->outputs));
    REQUIRE(is_ok);
}


static void req_pull_inputs(void)
{
    LOG(" -> inputs");

    int last_configured_input = -1;

    // XXX: maybe this check is useless, just fire full read and forget ?
    for (uint i = 0; i < MAX_N_INPUTS; i++)
    {
        if (rt.active_board->input_cfgs[i])
            last_configured_input = i;
    }

    if (last_configured_input < 0)
    {
        WARN("no inputs configured, ignoring read task");
        return;
    }

    bool is_ok = fieldbus_request_read(get_active_maddr(), 2, 0, rt.active_board->inputs, sizeof(rt.active_board->inputs[0]) * (last_configured_input + 1));
    REQUIRE(is_ok);
}

//--- interface to the board sync

static void finish_sync(void)
{
    LOG("end sync board %d", rt.active_board->addr);
    rt.is_syncing = 0;
}


static void do_next_sync_task()
{
    // update requests
    u8 tasks = rt.active_board->pending_tasks;

    if (! tasks)
    {
        finish_sync();
        return;
    }

    const sync_task_t *task = &sync_tasks_table[ctz(tasks)];
    task->req();
    timer_start(&rt.sync_timer,  5, 0, check_sync_progress);   // check progress after the 5 ms
}


static void on_done(void)
{
    u8 tasks = rt.active_board->pending_tasks;
    const sync_task_t *task = &sync_tasks_table[ctz(tasks)];

    rt.active_board->pending_tasks = tasks & (tasks - 1); // remove completed task from list

    if (task->on_done)
    {
        bool should_continue = task->on_done();
        if (! should_continue)
        {
            finish_sync();
            return;
        }
    }

    do_next_sync_task();
}


static void check_sync_progress(timer_t *dummy)
{
    fieldbus_status_t status = fieldbus_get_status();
    if (status == FIELDBUS_BUSY)
    {
        timer_start(&rt.sync_timer,  5, 0, check_sync_progress);   // check progress after the 5 ms again
        return;
    }

    timer_stop(&rt.sync_timer);

    if (status == FIELDBUS_IDLE)
    {
        on_done();
    }
    else
    {
        WARN("fieldbus err status %d", status);
        if (status == FIELDBUS_ERR_BAD_CHECKSUM)
            rt.active_board->last_sync_errs |= ERR_CHECKSUM;
        else
            rt.active_board->last_sync_errs |= ERR_LINK;
        finish_sync();
    }
}


static void abort_sync(void)
{
    LOG("aborting sync");
    timer_stop(&rt.sync_timer);

    fieldbus_abort();
    finish_sync();
}

//--- board sequencer routines and helpers

static board_t *next_entry(board_t *after)
{
    uint entries = rt.used_entries;

    if (after)
    {
        uint idx = (after - rt.pool) + 1;
        entries = (entries >> idx) << idx;    // clear all bits before (and including) the start
    }

    if (! entries)
        return NULL;

    return &rt.pool[ctz(entries)];
}

static board_t *next_dirty_entry(board_t *after)
{
    if (after)
    {
        // check entries from after to last
        for (board_t *b = next_entry(after); b; b = next_entry(b))
        {
            if (b->pending_tasks || b->requested_tasks)
                return b;
        }
    }

    // check entries from start to last (actually to 'after' including)
    for (board_t *b = next_entry(NULL); b; b = next_entry(b))
    {
        if (b->pending_tasks || b->requested_tasks)
            return b;
    }
    return NULL;
}


static void init_fresh_entry(board_t *b, uint addr)
{
    memclr(b, sizeof(board_t));
    b->requested_tasks = FULL_SYNC;
    b->addr = addr;
}


static void start_sync(board_t *b)
{
    REQUIRE(! rt.is_syncing);

    LOG("start sync board %d", b->addr);

    // put requested tasks to 'fifo'
    b->pending_tasks |= b->requested_tasks;
    b->requested_tasks = 0;

    rt.active_board = b;
    rt.active_board->last_sync_errs = 0;
    rt.is_syncing = 1;

    do_next_sync_task();
}


static void end_sync(board_t *b)
{
    uint errs = b->last_sync_errs;

    if (errs == 0)
    {
        // dequeue requests
        REQUIRE(b->pending_tasks == 0);
    }
    else
    {
        WARN("err 0x%02x", errs);

        // error, so do a full sync next time around
        // leave pending tasks unchanged
        b->requested_tasks |= FULL_SYNC;

        // stat resets
        if (errs & ERR_RESET)
        {
            b->stat.reset_cnt++;

            if (b->stat.reset_cnt > RESET_ERRS_THRES)
            {
                b->stat.reset_cnt = RESET_ERRS_THRES;
                b->stat.permanent_errs |= ERR_RESET;
            }
        }

        uint link_errs = errs & (ERR_CHECKSUM | ERR_LINK);

        // stat link errors
        if (link_errs)
        {
            b->stat.err_cnt++;
            if (b->stat.err_cnt > LINK_ERRS_THRES)
            {
                b->stat.err_cnt = LINK_ERRS_THRES;
                b->stat.permanent_errs |= link_errs;
                rt.last_bad_board = b->addr;
            }
        }
    }
}

static void schedule_keepalives(void)
{
    if (rt.keepalive_delay && --rt.keepalive_delay)
        return;

    rt.keepalive_delay = KEEPALIVE_PERIOD_MS / SEQ_PERIOD_MS - 1;

    for (board_t *b = next_entry(NULL); b; b = next_entry(b))
        b->requested_tasks |= 1 << SYNC_TASK_PULL_STATUS;
}


static void schedule_reads(void)
{
    if (rt.read_delay && --rt.read_delay)
        return;

    rt.read_delay = READ_PERIOD_MS / SEQ_PERIOD_MS - 1;

    for (board_t *b = next_entry(NULL); b; b = next_entry(b))
        b->requested_tasks |= 1 << SYNC_TASK_PULL_INPUTS;
}


static void fbd_periodic(timer_t *dummy)
{
    schedule_reads();
    schedule_keepalives();

    board_t *b = rt.active_board;

    if (b)
    {
        if (rt.is_syncing)
            return;

        end_sync(b);
    }

    b = next_dirty_entry(b);
    rt.active_board = b;

    if (b)
        start_sync(b);
}

//--- fbd api

board_t *fbd_find_board_by_addr(uint addr)
{
    if (addr == 0) return NULL;

    for (u32 used = rt.used_entries; used; used &= used - 1)
    {
        uint pos = ctz(used);

        if (pos >= MAX_N_BOARDS)
            break;

        if (rt.pool[pos].addr == addr)
            return &rt.pool[pos];
    }

    return NULL;
}


board_t *fbd_mount(uint addr)
{
    if (addr == 0) return NULL;

    board_t *b = fbd_find_board_by_addr(addr);
    if (b)
        return b;

    u32 used = rt.used_entries;
    REQUIRE(used != ~0U);

    uint pos = ctz(~used);
    if (pos >= MAX_N_BOARDS)
    {
        WARN("no mem to mount board");
        return NULL;
    }

    if (! rt.used_entries)
        timer_start(&rt.seq_timer, SEQ_PERIOD_MS, 1, fbd_periodic);

    rt.used_entries |= 1U << pos;
    b = &rt.pool[pos];
    init_fresh_entry(b, addr);

    LOG("mounted board %d (slot %d)", addr, pos);

    return b;
}


board_t *fbd_next_board(board_t *board)
{
    return next_entry(board);
}


void fbd_unmount(board_t *board)
{
    if (! board)
        return;

    uint pos = board - rt.pool;
    REQUIRE(pos < MAX_N_BOARDS);

    rt.used_entries &= ~(1U << pos);

    // brutally abort transaction if we're processing the entry in question now
    if ((rt.is_syncing) && (rt.active_board) && ((uint)(rt.active_board - rt.pool) == pos))
    {
        abort_sync();
        rt.active_board = NULL;
    }

    if (! rt.used_entries)
        timer_stop(&rt.seq_timer);

    LOG("unmounted board %d (slot %d)", board->addr, pos);
}


void fbd_write_discrete_outputs(board_t *board, u32 val, u32 mask)
{
    u32 oldval = board->discrete_outputs;
    u32 newval = (oldval & ~mask) | (val & mask);

    if (newval != oldval)
    {
        board->discrete_outputs = newval;
        board->requested_tasks |= 1 << SYNC_TASK_PUSH_DISCRETE_OUTPUTS;
    }
}


void fbd_write_register(board_t *board, uint reg_idx, uint type, uint val)
{
    if (reg_idx >= MAX_N_OUTPUTS)
    {
        WARN("attempt to drive register >= MAX_N_OUTPUTS");
        return;
    }

    board->outputs[reg_idx].type = type;
    board->outputs[reg_idx].val = val;

    board->requested_tasks |= 1 << SYNC_TASK_PUSH_OUTPUTS;
}


u16 fbd_read_input(board_t *board, uint input_idx)
{
    if (input_idx >= MAX_N_INPUTS)
    {
        WARN("attempt to read input >= MAX_N_INPUTS");
        return 0;
    }

    if (! board->input_cfgs[input_idx])
    {
        WARN("read of unconfigured input");
        return 0;
    }

    return board->inputs[input_idx];
}

void fbd_register_input_config(board_t *board, uint input_idx, const board_input_cfg_t *cfg)
{
    if (input_idx >= MAX_N_INPUTS)
    {
        WARN("attempt to configure input >= MAX_N_INPUTS");
        return;
    }

    board->input_cfgs[input_idx] = cfg;
    board->requested_tasks |= 1 << SYNC_TASK_PUSH_INPUT_CONFIG;
}


void fbd_init(void)
{
    fieldbus_init();
}

const board_stat_t *fbd_get_stat(const board_t *board)
{
    return &board->stat;
}

uint fbd_get_addr(const board_t *board)
{
    return board->addr;
}


void fbd_reset_errors(board_t *board)
{
    board->stat.err_cnt = 0;
    board->stat.reset_cnt = 0;
    board->stat.permanent_errs = 0;
}


u8 fbd_get_last_bad_board(void)
{
    return rt.last_bad_board;
}
