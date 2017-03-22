#define NOLOG

#include "syntax.h"

#include "hal.h"
#include "module.h"

#include "debug.h"

enum module_proc_ev_e
{
    EV_ENTRY,
    EV_PERIODIC,
    EV_XFER_OK,
};

typedef void (* state_t)(uint ev);

typedef struct
{
    state_t state;
    module_t *data;
    bool is_reduced_sync;
    uint shadow_len;
    u8 nfan;

    union
    {
        u16 status;
        u8 buf[384];
    };
} module_rt_t;

static void state_idling(uint ev);

static module_rt_t rt = {.state = state_idling};


PANIC_IF(sizeof(rt.buf) < sizeof(rt.data->outputs_cfg));
PANIC_IF(sizeof(rt.buf) < sizeof(rt.data->outputs));
PANIC_IF(sizeof(rt.buf) < sizeof(rt.data->discrete_outputs));
PANIC_IF(sizeof(rt.buf) < sizeof(rt.data->inputs_cfg));
PANIC_IF(sizeof(rt.buf) < sizeof(rt.data->inputs));
PANIC_IF(sizeof(rt.buf) < sizeof(module_fandata_t));


static u8 get_maddr(void)
{
    if (! rt.data->max_n_outputs)
        return rt.data->CpM % 100 + 120;
    return rt.data->CpM % 100+140;
}

static void trans(state_t dst)
{
    rt.state = dst;
    dst(EV_ENTRY);
}

static void state_pulling_cond(uint ev);
static void state_pushing_input_config(uint ev);
static void state_pushing_discrete_outputs(uint ev);
static void state_pushing_out_config(uint ev);
static void state_pulling_inputs(uint ev);
static void state_pushing_out_regs(uint ev);
static void state_pushing_fandata(uint ev);

static void state_top(uint ev)
{
    switch (ev)
    {
    case EV_PERIODIC:
        {
            hal_fieldbus_status_t status = HAL_fieldbus_get_status();
            if (status == HAL_FIELDBUS_BUSY)
                return;
            if (status == HAL_FIELDBUS_IDLE)
            {
                rt.data->cond &= ~MODULE_TRANSIENT_ERR_MASK;
                rt.state(EV_XFER_OK);
                return;
            }
            if (status == HAL_FIELDBUS_ERR_BAD_CHECKSUM)
                rt.data->cond |= MODULE_ERR_CHECKSUM;
            else
                rt.data->cond |= MODULE_ERR_LINK;
            trans(state_idling);
        }
        return;
    }
}

static void state_idling(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        LOG("idling");
        return;

    case EV_PERIODIC:
        return;
    }

    state_top(ev);
}

static void state_pulling_cond(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        if (rt.is_reduced_sync)
        {
            trans(state_pushing_discrete_outputs);
        }
        else
        {
            LOG("pulling cond");
            bool is_ok = HAL_fieldbus_request_read(get_maddr(), 0, 0, &rt.status, sizeof(rt.status));
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        if (rt.status & 0x01)
            rt.data->cond |= MODULE_ERR_RESET;

        if (rt.data->cond & (MODULE_ERR_RESET | MODULE_ERR_NEED_RESET))
        {
            rt.data->fail_cnt++;
            trans(state_pushing_input_config);
        }
        else
        {
            trans(state_pushing_discrete_outputs);
        }
        return;
    }

    state_top(ev);
}

static void state_pushing_input_config(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            LOG("pulling input config");
            uint len = sizeof(rt.data->inputs_cfg);
            memcpy(rt.buf, &rt.data->inputs_cfg, len);
            bool is_ok = HAL_fieldbus_request_write(get_maddr(), 0, 2, rt.buf, len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        trans(state_pushing_out_regs);
        return;
    }

    state_top(ev);
}

static void state_pushing_discrete_outputs(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            LOG("pushing out values");
            uint len = sizeof(rt.data->discrete_outputs);
            memcpy(rt.buf, &rt.data->discrete_outputs, len);
            bool is_ok = HAL_fieldbus_request_write(get_maddr(), 0, 3, rt.buf, len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        trans(state_pushing_out_config);
        return;
    }

    state_top(ev);
}


static void state_pushing_out_config(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        if (rt.data->max_n_outputs == 0)
        {
            trans(state_pulling_inputs);
        }
        else
        {
            LOG("pushing out config");
            uint len = sizeof(rt.data->outputs_cfg);
            memcpy(rt.buf, &rt.data->outputs_cfg, len);
            bool is_ok = HAL_fieldbus_request_write(get_maddr(), 4, 3, rt.buf, len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        trans(state_pulling_inputs);
        return;
    }

    state_top(ev);
}


static void state_pulling_inputs(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        if (rt.is_reduced_sync)
        {
            trans(state_pushing_out_regs);
        }
        else if (rt.data->max_n_inputs == 0)
        {
            WARN("max in == 0, ignoring transfer");
            trans(state_pushing_out_regs);
        }
        else
        {
            LOG("pulling inputs");
            uint cnt = rt.data->max_n_inputs;
            if (cnt > countof(rt.data->inputs))
            {
                WARN("max in > inValues, trimming transfer");
                cnt = countof(rt.data->inputs);
            }
            // XXX: shadow len used to make sure sure maxin in unchanged during the transfer
            rt.shadow_len = sizeof(rt.data->inputs[0]) * cnt;
            bool is_ok = HAL_fieldbus_request_read(get_maddr(), 2, 0, rt.buf, rt.shadow_len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        memcpy(&rt.data->inputs, rt.buf, rt.shadow_len);
        trans(state_pushing_out_regs);
        return;
    }

    state_top(ev);
}


static void state_pushing_out_regs(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        if (rt.data->outputs[0].type == 0)
        {
            trans(state_pushing_fandata);
        }
        else
        {
            LOG("pushing out regs");
            uint len = sizeof(rt.data->outputs);
            memcpy(rt.buf, &rt.data->outputs, len);
            bool is_ok = HAL_fieldbus_request_write(get_maddr(), 0, 8, rt.buf, len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        trans(state_pushing_fandata);
        return;
    }

    state_top(ev);
}


static void state_pushing_fandata(uint ev)
{
    switch (ev)
    {
    case EV_ENTRY:
        if (! rt.data->fandata)
        {
            trans(state_idling);
        }
        else
        {
            LOG("pushing fandata");
            uint len = sizeof(module_fandata_t);
            memcpy(rt.buf, &rt.data->fandata[rt.nfan], len);
            bool is_ok = HAL_fieldbus_request_write(get_maddr(), 3 + sizeof(module_fandata_t) * rt.nfan, 9, rt.buf, len);
            REQUIRE(is_ok);
        }
        return;

    case EV_XFER_OK:
        trans(state_idling);
        return;
    }

    state_top(ev);
}


void module_processor_run(module_t *module, bool is_reduced_sync, u8 nfan)
{
    REQUIRE(! module_processor_is_busy());

    rt.data = module;
    rt.is_reduced_sync = is_reduced_sync;
    rt.nfan = nfan;

    trans(state_pulling_cond);
}

void module_processor_periodic(void)
{
    rt.state(EV_PERIODIC);
}


bool module_processor_is_busy(void)
{
    return rt.state != state_idling;
}
