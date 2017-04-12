#define _FIELDBUS_C_

#include "syntax.h"
#include "hal.h"
#include "timers.h"
#include "fieldbus.h"

#include "debug.h"

/*
    fieldbus is very legacy, not easy to integrate it above the primitive serial read/write
    so the implementation isolate the complexies inside the hal module
    there is some logic, a sequence of requests and responses embedded in low-level code.
    non-standart 9-bit serial format is exploited for signalling the start of transaction - address byte has msbit set,
    while data bytes has not

    sequence:
    write:
    0) send module address with msbit set
    1) receive same address with msbit set
    2) wait a little to make sure slave is set up for header
    3) send header
    4) receive checksum (of header ?)
    5) wait a little to make sure slave is set up for data
    6) send data
    7) send checksum (of data ?)
    8) receive checksum (0x55 ?)

    read:
    0) send module address with msbit set
    1) receive same address with msbit set
    2) wait a little to make sure slave is addressed
    3) send header
    4) receive checksum of header
    5) receive data
    6) receive checksum of data

    data length is fixed, transaction size is exact, no need to detect end of frame

*/

PANIC_IF(sizeof(hdr_t) != 5);

static fieldbus_rt_t rt;

static void trans(state_t dst)
{
    rt.state(EV_EXIT, 0);
    rt.state = dst;
    dst(EV_ENTRY, 0);
}

static u32 calc_transfer_time(uint size)
{
    return (size * 10 * 1000 + BAUDRATE - 1) /  BAUDRATE;
}

static u8 calc_checksum(const void *src, uint len)
{
    u8 out = 0;
    for (const u8 *p = src; len; len--, p++)
        out += *p;
    return out;
}


//-- state handlers

static void state_idling(uint ev, uint arg)
{
}

static void addressing_slave(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            bool is_ok = HAL_fieldbus_address_slave(rt.addr, ADDRESSING_TIMEOUT);
            REQUIRE(is_ok);
            timer_start(&rt.timer, 1, 1, check_xfer);
        }
        return;

    case EV_XFER_COMPLETED:
        trans(state_waiting_before_header_send);
        return;

    case EV_XFER_FAILED:
        rt.status = arg;
        trans(state_idling);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;
    }
}


static void state_waiting_before_header_send(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        timer_start(&rt.timer, SLAVE_SETUP_TIMEOUT, 0, on_timer_expired);
        return;

    case EV_TIMER_EXPIRED:
        if ((rt.hdr.dir_and_blocknum & DIR_MASK) == DIR_READ_BITS)
            trans(state_sending_read_header_and_receiving_data);
        else
            trans(state_sending_write_header_and_receiving_checksum);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;
    }
}

static void state_sending_read_header_and_receiving_data(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            uint txlen = 1 + rt.hdr.datalen + 1;    // +2 checksums - header and data
            bool is_ok = HAL_fieldbus_request_xfer(rt.hdr.raw, sizeof(hdr_t),
                                                   rt.buf, txlen,
                                                   calc_transfer_time(txlen) * 2);
            REQUIRE(is_ok);
            timer_start(&rt.timer, 1, 1, check_xfer);
        }
        return;

    case EV_XFER_COMPLETED:
        {
            uint datalen = rt.hdr.datalen;

            if (rt.buf[0] != calc_checksum(&rt.hdr, sizeof(rt.hdr)))
            {
                rt.status = FIELDBUS_ERR_BAD_CHECKSUM;
            }
            else if (rt.buf[1 + datalen] != calc_checksum(&rt.buf[1], datalen))
            {
                rt.status = FIELDBUS_ERR_BAD_CHECKSUM;
            }
            else
            {
                rt.status = FIELDBUS_IDLE;
                memcpy(rt.rx_dst, &rt.buf[1], datalen);
            }
        }
        trans(state_idling);
        return;

    case EV_XFER_FAILED:
        rt.status = arg;
        trans(state_idling);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;
    }
}


static void state_sending_write_header_and_receiving_checksum(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            bool is_ok = HAL_fieldbus_request_xfer(rt.hdr.raw, sizeof(hdr_t),
                                                   &lastof(rt.buf), 1,
                                                   REPLY_TIMEOUT);
            REQUIRE(is_ok);
            timer_start(&rt.timer, 1, 1, check_xfer);
        }
        return;

    case EV_XFER_COMPLETED:
        if (lastof(rt.buf) != calc_checksum(&rt.hdr, sizeof(rt.hdr)))
        {
            rt.status = FIELDBUS_ERR_BAD_CHECKSUM;
            trans(state_idling);
        }
        else
        {
            trans(state_waiting_before_data_write);
        }
        return;

    case EV_XFER_FAILED:
        rt.status = arg;
        trans(state_idling);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;

    }
}


static void state_waiting_before_data_write(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        timer_start(&rt.timer, SLAVE_SETUP_TIMEOUT, 0, on_timer_expired);
        return;

    case EV_TIMER_EXPIRED:
        trans(state_writing_data_and_receiving_checksum);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;
    }
}

static void state_writing_data_and_receiving_checksum(uint ev, uint arg)
{
    switch (ev)
    {
    case EV_ENTRY:
        {
            uint datalen = rt.hdr.datalen;
            rt.buf[datalen] = 55 - calc_checksum(rt.buf, datalen);

            bool is_ok = HAL_fieldbus_request_xfer(rt.buf, datalen + 1,
                                                   &lastof(rt.buf), 1,
                                                   REPLY_TIMEOUT);
            REQUIRE(is_ok);
            timer_start(&rt.timer, 1, 1, check_xfer);
        }
        return;

    case EV_XFER_COMPLETED:
        if (lastof(rt.buf) != 55)
            rt.status = FIELDBUS_ERR_BAD_CHECKSUM;
        else
            rt.status = FIELDBUS_IDLE;
        trans(state_idling);
        return;

    case EV_XFER_FAILED:
        rt.status = arg;
        trans(state_idling);
        return;

    case EV_EXIT:
        timer_stop(&rt.timer);
        return;
    }
}


// -- event dispatchers

static void on_timer_expired(timer_t *dummy)
{
    rt.state(EV_TIMER_EXPIRED, 0);
}

static void check_xfer(timer_t *dummy)
{
    hal_fieldbus_status_t status = HAL_fieldbus_get_status();

    if (status == HAL_FIELDBUS_BUSY)    // wait more
        return;

    if (status == HAL_FIELDBUS_IDLE)
    {
        rt.state(EV_XFER_COMPLETED, 0);
    }
    else
    {
        uint err;
        if (status == HAL_FIELDBUS_ERR_ADDRESS_MISMATCH)
            err = FIELDBUS_ERR_ADDRESS_MISMATCH;
        else if (status == HAL_FIELDBUS_ERR_COLLISION)
            err = FIELDBUS_ERR_COLLISION;
        else if (status == HAL_FIELDBUS_ERR_DATA_LOST)
            err = FIELDBUS_ERR_INTERNAL_ERROR;
        else // if (status == HAL_FIELDBUS_ERR_TIMEOUT)
            err = FIELDBUS_ERR_TIMEOUT;
        rt.state(EV_XFER_FAILED, err);
    }
}


void fieldbus_init(void)
{
    HAL_fieldbus_init(BAUDRATE);

    rt.state = state_idling;
    trans(state_idling);
}


bool fieldbus_request_write(u8 slave_addr, uint reg_addr, uint blocknum, const void *data, uint datalen)
{
    REQUIRE(data && datalen);
    REQUIRE((blocknum & DIR_MASK) == 0);
    REQUIRE(datalen <= FIELDBUS_MAX_DATALEN);  // make sure we fit (including checksum)

    if (rt.state != state_idling)
        return 0;

    memcpy(rt.buf, data, datalen);

    rt.addr = slave_addr;

    rt.hdr.reg_addr = reg_addr;
    rt.hdr.datalen = datalen;
    rt.hdr.dir_and_blocknum = DIR_WRITE_BITS | blocknum;

    rt.status = FIELDBUS_BUSY;

    trans(addressing_slave);

    return 1;
}


bool fieldbus_request_read(u8 slave_addr, uint reg_addr, uint blocknum, void *data, uint datalen)
{
    REQUIRE(data && datalen);
    REQUIRE((blocknum & DIR_MASK) == 0);
    REQUIRE(datalen <= FIELDBUS_MAX_DATALEN);  // make sure we fit (including checksum)

    if (rt.state != state_idling)
        return 0;

    rt.addr = slave_addr;
    rt.rx_dst = data;

    rt.hdr.reg_addr = reg_addr;
    rt.hdr.datalen = datalen;
    rt.hdr.dir_and_blocknum = DIR_READ_BITS | blocknum;

    rt.status = FIELDBUS_BUSY;

    trans(addressing_slave);

    return 1;
}


void fieldbus_abort(void)
{
    if (rt.state != state_idling)
    {
        while (HAL_fieldbus_get_status() == HAL_FIELDBUS_BUSY);
    }
    rt.status = FIELDBUS_IDLE;
    trans(state_idling);
}


fieldbus_status_t fieldbus_get_status(void)
{
    return rt.status;
}

bool fieldbus_is_busy(void)
{
    return rt.status == FIELDBUS_BUSY;
}

//-- tests
#if 1

static void smoke_read_status(uint addr)
{
    u16 slave_status = 0xFFFF;

    bool is_ok = fieldbus_request_read(addr, 0, 0, &slave_status, sizeof(slave_status));

    if (! is_ok)
    {
        WARN("failed to request read");
        return;
    }

    while (fieldbus_is_busy())
    {
        timers_process();
    }

    int status = fieldbus_get_status();
    if (status != FIELDBUS_IDLE)
        WARN("fieldbus status is %d", status);
    else
        LOG("slave condition is %d", slave_status);
}


static void smoke_write_output(uint addr, u32 output)
{
    bool is_ok = fieldbus_request_write(addr, 0, 3, &output, sizeof(output));

    if (! is_ok)
    {
        WARN("failed to request write");
        return;
    }

    while (fieldbus_is_busy())
    {
        timers_process();
    }

    int status = fieldbus_get_status();
    if (status != FIELDBUS_IDLE)
        WARN("fieldbus status is %d", status);
    else
        LOG("write ok");
}

void fieldbus_smoke(void)
{
    fieldbus_init();

    u32 output = 0x0000FF00;
    u8 i = 0;
    while (1)
    {
        smoke_read_status(121);
        HAL_systimer_sleep(100);
        smoke_read_status(121);
        HAL_systimer_sleep(100);
        smoke_write_output(121, output);
        HAL_systimer_sleep(10000);

        output ^= (i++ & 0xFF) << 8;
    }

}

#endif
