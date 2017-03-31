#define NOLOG

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

static const action_t actions_table[_MODULE_ACTION_LAST] =
{
    [MODULE_ACTION_PULL_STATUS] =           {.req = req_pull_status,            .on_done = on_pull_status_done},
    [MODULE_ACTION_PUSH_INPUT_CONFIG] =     {.req = req_push_input_config},
    [MODULE_ACTION_PUSH_DISCRETE_OUTPUTS] = {.req = req_push_discrete_outputs},
    [MODULE_ACTION_PULL_INPUTS] =           {.req = req_pull_inputs},
    [MODULE_ACTION_PUSH_OUTPUTS] =          {.req = req_push_outputs},
};

static void check_progress(timer_t *dummy);


PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->outputs));
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->discrete_outputs));
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->inputs_cfg));
PANIC_IF(FIELDBUS_MAX_DATALEN < sizeof(rt.data->inputs));
PANIC_IF(_MODULE_ACTION_LAST > 0x100);  // to fit u8


static u8 get_maddr(void)
{
    return rt.data->base % 100 + 120;
}


static void req_pull_status(void)
{
    LOG("pulling cond");
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
    bool is_ok = fieldbus_request_write(get_maddr(), 0, 2, &rt.data->inputs_cfg, sizeof(rt.data->inputs_cfg));
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
    if (rt.data->outputs[0].type == 0)
    {
        // just auto-complete transfer
        return;
    }

    LOG("pushing out regs");
    bool is_ok = fieldbus_request_write(get_maddr(), 0, 8, &rt.data->outputs, sizeof(rt.data->outputs));
    REQUIRE(is_ok);
}


static void req_pull_inputs(void)
{
    uint cnt = rt.data->max_n_inputs;

    if (cnt == 0)
    {
        WARN("max in == 0, ignoring transfer");
        return;
    }

    if (cnt > countof(rt.data->inputs))
    {
        WARN("max in > inValues, trimming transfer");
        cnt = countof(rt.data->inputs);
    }
    bool is_ok = fieldbus_request_read(get_maddr(), 2, 0, rt.data->inputs, sizeof(rt.data->inputs[0]) * cnt);
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
        if (status == FIELDBUS_ERR_BAD_CHECKSUM)
            rt.err |= MODULE_ERR_CHECKSUM;
        else
            rt.err |= MODULE_ERR_LINK;
        finish();
    }
}


void module_sync_run(module_t *module, u8 requested_actions)
{
    LOG("running module processor");

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

