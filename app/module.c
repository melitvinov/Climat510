//#define NOLOG

#include "syntax.h"
#include "timers.h"
#include "fieldbus.h"
#include "module.h"

#include "debug.h"


typedef struct
{
    timer_t timer;
    module_t *data;
    bool is_busy;
    u8 pending_actions;
    u8 err;
} module_rt_t;

typedef struct
{
    void (* const req)(void);
    bool (* const on_done)(void);
} action_t;

static module_rt_t rt;

static void req_pull_status(void);
static bool on_pull_status_done(void);
static void req_push_input_config(void);
static void req_push_discrete_outputs(void);
static void req_pull_inputs(void);
static void req_push_outputs(void);

enum
{
    ACTION_IDX_PULL_STATUS,
    ACTION_IDX_PUSH_INPUT_CONFIG,
};


static const action_t actions_table[] =
{
    [_MODULE_ACTION_IDX_PULL_STATUS] =           {.req = req_pull_status,            .on_done = on_pull_status_done},
    [_MODULE_ACTION_IDX_PUSH_INPUT_CONFIG] =     {.req = req_push_input_config},
    [_MODULE_ACTION_IDX_PUSH_DISCRETE_OUTPUTS] = {.req = req_push_discrete_outputs},
    [_MODULE_ACTION_IDX_PULL_INPUTS] =           {.req = req_pull_inputs},
    [_MODULE_ACTION_IDX_PUSH_OUTPUTS] =          {.req = req_push_outputs},
};

static void check_progress(timer_t *dummy);


PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->outputs));
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->discrete_outputs));
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(module_input_cfg_t) * MODULE_MAX_MAX_N_INPUTS);
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->inputs));
PANIC_IF(_MODULE_ACTION_IDX_LAST > 8);  // to fit u8


static u8 get_maddr(void)
{
    return rt.data->addr % 100 + 120;
}


static void req_pull_status(void)
{
    LOG("pulling status");
    bool is_ok = fieldbus_request_read(get_maddr(), 0, 0, &rt.data->status_word, sizeof(rt.data->status_word));
    REQUIRE(is_ok);
}


static bool on_pull_status_done(void)
{
    if (rt.data->status_word & 0x01)
    {
        rt.err |= MODULE_ERR_RESET;
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
        const module_input_cfg_t *src = rt.data->input_cfg_links.p[i];
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

    bool is_ok = fieldbus_request_write(get_maddr(), 0, 2, &buf, sizeof(buf));
    REQUIRE(is_ok);
}


static void req_push_discrete_outputs(void)
{
    LOG("pushing out values");
    bool is_ok = fieldbus_request_write(get_maddr(), 0, 3, &rt.data->discrete_outputs, sizeof(rt.data->discrete_outputs));
    REQUIRE(is_ok);
}


static void req_push_outputs(void)
{
    LOG("pushing out regs");

    if (rt.data->outputs[0].type == 0)
    {
        // just auto-complete transfer
        return;
    }

    bool is_ok = fieldbus_request_write(get_maddr(), 0, 8, &rt.data->outputs, sizeof(rt.data->outputs));
    REQUIRE(is_ok);
}


static void req_pull_inputs(void)
{
    LOG("pulling inputs");

    int last_configured_input = -1;

    for (uint i = 0; i < MODULE_MAX_MAX_N_INPUTS; i++)
    {
        if (rt.data->input_cfg_links.p[i])
            last_configured_input = i;
    }

    if (last_configured_input < 0)
    {
        WARN("no inputs configured, ignoring read action");
        return;
    }

    bool is_ok = fieldbus_request_read(get_maddr(), 2, 0, rt.data->inputs, sizeof(rt.data->inputs[0]) * (last_configured_input + 1));
    REQUIRE(is_ok);
}


static void finish(void)
{
    rt.is_busy = 0;
}


static void do_next_action(void)
{
    // update requests
    u8 actions = rt.pending_actions;

    if (! actions)
    {
        finish();
        return;
    }

    // limit the number of actions per session to avoid the module blocking the bus for too long

//  LOG("actions is %d", actions);

    const action_t *action = &actions_table[ctz(actions)];
    action->req();
    timer_start(&rt.timer,  5, 1, check_progress);   // 5 ms poll period is ok
}


static void on_done(void)
{
    u8 actions = rt.pending_actions;
    const action_t *action = &actions_table[ctz(actions)];

    rt.pending_actions = actions & (actions - 1); // remove completed action from list

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

    timer_stop(&rt.timer);

    if (status == FIELDBUS_IDLE)
    {
        on_done();
    }
    else
    {
        WARN("fieldbus err status %d", status);
        if (status == FIELDBUS_ERR_BAD_CHECKSUM)
            rt.err |= MODULE_ERR_CHECKSUM;
        else
            rt.err |= MODULE_ERR_LINK;
        finish();
    }
}


void module_sync_run(module_t *module, u8 requested_actions)
{
    REQUIRE(! module_sync_is_busy());

    rt.data = module;
    rt.is_busy = 1;
    rt.err = 0;
    rt.pending_actions = requested_actions;

    do_next_action();
}

bool module_sync_is_busy(void)
{
    return rt.is_busy;
}

uint module_sync_get_err(void)
{
    return rt.err;
}

