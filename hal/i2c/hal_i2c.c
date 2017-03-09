#define _HAL_I2C_C_

#include "syntax.h"
#include "stm32f10x.h"
#include "hal_pincfg.h"
#include "hal_sys.h"
#include "hal_i2c.h"

#include "debug.h"

// pins are pb10 - scl, pb11 - sda

static I2C_TypeDef * const i2c = I2C2;
static GPIO_TypeDef * const port = GPIOB;
static const uint scl_pin_idx = 10;
static const uint sda_pin_idx = 11;

// the stm i2c is totally idiotic. There is a lot of little timing quirks and gotchas
// I'm forced to use bitbang i2c. at least it would be reliable
//
// NOTE: the used fram is not clock stretching, so it is not analyzed

static inline void scl_up(void)   {port->BSRR = 1 << scl_pin_idx;}
static inline void scl_down(void)   {port->BSRR = 1 << (scl_pin_idx + 16);}
static inline void sda_up(void)   {port->BSRR = 1 << sda_pin_idx;}
static inline void sda_down(void)   {port->BSRR = 1 << (sda_pin_idx + 16);}
static inline bool scl_is_hold(void)    {return ! (port->IDR & (1 << (scl_pin_idx)));}
static inline bool sda_is_hold(void)    {return ! (port->IDR & (1 << (sda_pin_idx)));}

static void wait_100ns(void)
{
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

static bool i2c_start( void)
{
    wait_100ns();   // 600 ns buffer time
    wait_100ns();
    wait_100ns();
    wait_100ns();
    wait_100ns();
    wait_100ns();

    if (scl_is_hold() || sda_is_hold())
        return 0;

    sda_down();
    wait_100ns();   // 300 ns
    wait_100ns();
    wait_100ns();

    scl_down();
    wait_100ns();   // provide a 100 ns before a data

    return 1;
}

static void i2c_stop( void)
{
    sda_down();
    wait_100ns();   //
    wait_100ns();   //
    wait_100ns();   //
    wait_100ns();   //

    scl_up();
    wait_100ns();   // 300 ns stop time
    wait_100ns();
    wait_100ns();

    sda_up();
}

static void i2c_restart( void)
{
    wait_100ns();   // data setup, low time - 500 ns
    wait_100ns();   //
    wait_100ns();   //
    wait_100ns();   //

    scl_up();
    wait_100ns();   // 300 ns stop time
    wait_100ns();
    wait_100ns();

    sda_down();
    wait_100ns();   // 300 ns start time

    wait_100ns();
    wait_100ns();

    scl_down();
    wait_100ns();   // provide a 100 ns before a data
}

static uint i2c_send( uint Data)
{
    bool ack;

    uint cnt = 8;
    do
    {
        // setup data
        if ( Data & 0x0080)
            sda_up();
        else
            sda_down();

        Data <<= 1;

        wait_100ns();   // clock low - 500 ns + data setup (100 ns)
        wait_100ns();   //
        wait_100ns();   //
        wait_100ns();   //

        scl_up();
        wait_100ns();   // clock high > 260 ns. take 300 ns
        wait_100ns();   //
        wait_100ns();   //

        scl_down();
        wait_100ns();   // provide a 100 ns before a data

    } while ( --cnt );

    sda_up();       // prepairing for ack
    wait_100ns();   // clock low - 500 ns
    wait_100ns();   //
    wait_100ns();   //
    wait_100ns();       // data setup time, 100 ns

    scl_up();
    wait_100ns();   // clock high = 300 ns
    wait_100ns();   //
    ack = sda_is_hold();    // take ack sample

    scl_down();
    wait_100ns();   // wait 100 ns before a data

    return ack;
}

static uint i2c_receive(uint should_ack)
{
    uint data = 0;

    uint cnt = 8;
    do
    {
        wait_100ns(); // wait 500 ns to get 600 ns low time
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();

        scl_up();
        wait_100ns();   // wait ~300 ns
        wait_100ns();
        data <<= 1;
        data |= sda_is_hold() ? 0 : 1;

        scl_down();
        wait_100ns();   // data hold time
    } while ( --cnt);   // 8 bits shifted in allright

    if ( should_ack)
        sda_down();
    else
        sda_up();

    wait_100ns();       // clock low - 500 ns
    wait_100ns();       //
    wait_100ns();       //
    wait_100ns();       //

    scl_up();
    wait_100ns();   // wait 300 ns
    wait_100ns();
    wait_100ns();

    scl_down();
    wait_100ns();

    sda_up();
    return data;
}


static bool i2c_exec_write(uint addr, const u8 *preamble, uint preamble_len, const u8 *data, uint data_len)
{
    if (! i2c_start())
        return 0;

    if (! i2c_send(addr))
        return 0;

    for (; preamble_len; preamble_len--)
    {
        if (! i2c_send(*preamble++))
            return 0;
    }

    for (; data_len; data_len--)
    {
        if (! i2c_send(*data++))
            return 0;
    }

    i2c_stop();
    return 1;
}


static bool i2c_exec_read(uint addr, const u8 *preamble, uint preamble_len, u8 *data, uint data_len)
{
    if (! i2c_start())
        return 0;

    if (preamble_len)
    {
        if (! i2c_send(addr))
            return 0;

        for (; preamble_len; preamble_len--)
        {
            if (! i2c_send(*preamble++))
                return 0;
        }
        i2c_restart();
    }

    if (! i2c_send(addr | 1))
        return 0;

    for (; data_len > 1; data_len--)
    {
        *data++ = i2c_receive(1);
    }

    if (data)
    {
        *data = i2c_receive(0);
    }

    i2c_stop();
    return 1;
}


bool hal_i2c_read(uint addr, const void *preamble, uint preamble_len, void *data, uint data_len)
{
    for (uint trans_cnt = I2C_TRANSACTION_RETRY; trans_cnt; trans_cnt--)
    {
        if (i2c_exec_read(addr << 1, preamble, preamble_len, data, data_len))
            return 1;

        WARN("failed to exec i2c read. trying again");
        hal_i2c_init();
    }
    ERR("failed to exec i2c write. giving up");
    return 0;
}


bool hal_i2c_write(uint addr, const void *preamble, uint preamble_len, const void *data, uint data_len)
{
    for (uint trans_cnt = I2C_TRANSACTION_RETRY; trans_cnt; trans_cnt--)
    {
        if (i2c_exec_write(addr << 1, preamble, preamble_len, data, data_len))
            return 1;

        WARN("failed to exec i2c write. trying again");
        hal_i2c_init();
    }
    ERR("failed to exec i2c write. giving up");
    return 0;
}

void hal_i2c_init(void)
{
    REQUIRE((uint) HAL_SYS_F_CPU == 36000000);

    const u32 scl_pin_mask = 1 << scl_pin_idx;
    const u32 sda_pin_mask = 1 << sda_pin_idx;

    port->BSRR = scl_pin_mask | sda_pin_mask;
    hal_pincfg_od(port, scl_pin_idx);
    hal_pincfg_od(port, sda_pin_idx);

    // give a lot of clocks in case peripheral stuck
    for (uint i = 9; i; i--)
    {
        port->BRR = scl_pin_mask;
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
        port->BSRR = scl_pin_mask;
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
        wait_100ns();
    }
}
