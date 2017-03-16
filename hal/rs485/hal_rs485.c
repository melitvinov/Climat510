#include "syntax.h"
#include "stm32f10x.h"

#include "hal_priorities.h"
#include "hal_sys.h"
#include "hal_pincfg.h"
#include "hal_fastloop.h"
#include "hal_rs485.h"

#include "debug.h"

// hardware uart on pc10 tx (usart4_tx), pc11 rx (usart4_rx), pc12 dir (usart4_ck)
//
// uart setting is unusual: 9 bit, transaction starts from address with 9 bit set.
// we provide a method for setting msbit in (all bytes of !) transaction.
// hacky, but cover the case of single-byte start-of-transaction nicely
//
// number of data to transfer is fixed, no need to detect end of frame
// request data are required, response may be omitted

#warning "ninebit API is ugly. do a something better !"

enum rs_ev_t
{
    EV_ENTRY,
    EV_OVERRUN,
    EV_BYTE_RECEIVED,
    EV_TXBUF_EMPTY,
    EV_SHIFTREG_EMPTY,
    EV_TIMEOUT,
};

typedef void (*state_t)(uint ev, uint data);


typedef struct
{
    volatile state_t state;
    HAL_rs485_transfer_t * volatile xfer;
    union
    {
        const u8 * volatile tx;
        u8 * volatile rx;
    };
    volatile uint remain;

    struct
    {
        hal_fasttask_t timer;
        uint ticks_till_timeout;
    };
} rs_rt_t;

static USART_TypeDef *const uart = UART4;
static const u32 active_cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_M;

static rs_rt_t rt;

static inline void switch_to_tx(void)   { GPIOC->BSRR = 1 << 12; }
static inline void switch_to_rx(void)   { GPIOC->BSRR = 1 << (12 + 16); }

static void state_idling(uint ev, uint data);
static void state_sending(uint ev, uint data);
static void state_receiving(uint ev, uint data);

static void trans(state_t dst)
{
    rt.state = dst;
    dst(EV_ENTRY, 0);
}

static void timer_handler(void)
{
    if (rt.ticks_till_timeout--)
        return;

    rt.state(EV_TIMEOUT, 0);
}

static void state_idling(uint ev, uint data)
{
    HAL_rs485_transfer_t *xfer = rt.xfer;

    switch (ev)
    {
    case EV_ENTRY:
        hal_fastloop_remove_task(&rt.timer);
        switch_to_rx();
        if (xfer)
            xfer->is_done = 1;
        rt.xfer = NULL;
        uart->CR1 = active_cr1; // disable all interrupts
        return;
    }
}

static void state_sending(uint ev, uint data)
{
    HAL_rs485_transfer_t *xfer = rt.xfer;

    switch (ev)
    {
    case EV_ENTRY:
        {
            switch_to_tx();

            while (uart->SR & (USART_SR_RXNE | USART_SR_ORE))
                uart->DR;

            uart->SR = 0;

            xfer->err = 0;
            xfer->is_done = 0;
            const u8 *tx = xfer->req;
            rt.remain = xfer->req_size - 1;
            uart->DR = *tx | (xfer->should_set_ninebit ? 0x100 : 0);
            rt.tx = tx + 1;
            uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_TCIE;  // leave only transmit interrupts
        }
        return;

    case EV_SHIFTREG_EMPTY:
        {
            uint remain = rt.remain;
            if (remain)
            {
                uart->DR = *rt.tx++;
                rt.remain = remain - 1;
            }
            else
            {
                if (xfer->resp && xfer->resp_size)
                    trans(state_receiving);
                else
                    trans(state_idling);
            }
        }
        return;

    case EV_TXBUF_EMPTY:
        {
            uint remain = rt.remain;
            if (remain)
            {
                uart->DR = *rt.tx++;
                rt.remain = remain - 1;
            }
            else
            {
                uart->CR1 = active_cr1 | USART_CR1_TCIE;    // leave only transmit complete interrupt to reduce load
            }
        }
        return;
    }
}


static void state_receiving(uint ev, uint data)
{
    HAL_rs485_transfer_t *xfer = rt.xfer;

    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        rt.rx = xfer->resp;
        rt.remain = xfer->resp_size;
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        rt.ticks_till_timeout = xfer->timeout;
        hal_fastloop_add_task(&rt.timer, timer_handler);
        return;

    case EV_OVERRUN:
        xfer->err = HAL_RS485_ERR_OVERRUN;
        trans(state_idling);
        return;

    case EV_TIMEOUT:
        xfer->err = HAL_RS485_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        {
            if ((data >> 8) != xfer->expect_ninebit)
            {
                xfer->err = HAL_RS485_ERR_OUT_OF_SYNC;
                trans(state_idling);
                return;
            }

            uint remain = rt.remain - 1;
            *rt.rx++ = data;
            if (remain)
                rt.remain = remain;
            else
                trans(state_idling);
        }
        return;
    }
}


void uart4_isr(void)
{
    u32 sr = uart->SR & (USART_SR_ORE | USART_SR_RXNE | USART_SR_TXE | USART_SR_TC);

    if (sr & USART_SR_ORE)
    {
        rt.state(EV_OVERRUN, uart->DR);
    }
    else if (sr & USART_SR_RXNE)
    {
        rt.state(EV_BYTE_RECEIVED, uart->DR);
    }

    if (sr & USART_SR_TC)
        rt.state(EV_SHIFTREG_EMPTY, 0);
    else if (sr & USART_SR_TXE)
        rt.state(EV_TXBUF_EMPTY, 0);
}

bool HAL_rs485_transfer(HAL_rs485_transfer_t *xfer)
{
    if (rt.state != state_idling)
        return 0;

    REQUIRE(xfer->req && xfer->req_size);

    rt.xfer = xfer;

    trans(state_sending);

    return 1;
}

void HAL_rs485_init(uint baud)
{
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

    hal_pincfg_out_alt(GPIOC, 10);  // tx

    GPIOC->BSRR = 1 << 11;          // rx with pull-up
    hal_pincfg_in(GPIOC, 11);

    GPIOC->BSRR = 1 << (12 + 16);    // dir, cleared by default (rx)
    hal_pincfg_out(GPIOC, 12);

    NVIC_SetPriority(UART4_IRQn, HAL_IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(UART4_IRQn);
    NVIC_EnableIRQ(UART4_IRQn);

    uart->CR1 = 0;
    uart->CR2 = 0;
    uart->CR3 = 0;
    uart->BRR = (HAL_SYS_F_CPU + baud/2 - 1) / baud;

    trans(state_idling);
}

bool HAL_rs485_is_busy(void)
{
    return rt.state != state_idling;
}

void HAL_rs485_abort(void)
{
    if (rt.state == state_idling)
        return;

    uart->CR1 = active_cr1;
    NVIC_DisableIRQ(UART4_IRQn);

    uart->SR = 0;
    if (rt.xfer)
        rt.xfer->err = HAL_RS485_ERR_ABORTED;
    trans(state_idling);
    NVIC_EnableIRQ(UART4_IRQn);
}

// tests

#if 1

static void ping(uint idx)
{
    u8 id = idx;
    u8 req = id;
    u8 resp = 0;

    HAL_rs485_transfer_t xfer =
    {
        .req = &req,
        .req_size = sizeof(req),
        .resp = &resp,
        .resp_size = 1,
        .should_set_ninebit = 1,
        .expect_ninebit = 1,
        .timeout = 200,
    };
    bool is_ok = HAL_rs485_transfer(&xfer);

    LOG("id = %d, 485 is ok = %d", id, is_ok);

    while (HAL_rs485_is_busy());

    LOG("err = %d, resp = %d", xfer.err, resp);
}

void HAL_rs485_smoke(void)
{
    HAL_rs485_init(9600);

    while(1)
    {
        ping(121);
        ping(122);
        ping(123);
        ping(124);
    }
}

#endif
