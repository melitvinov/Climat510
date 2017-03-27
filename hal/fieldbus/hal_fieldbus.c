#define _HAL_FIELDBUS_C_

#include "syntax.h"
#include "stm32f10x.h"

#include "hal_priorities.h"
#include "hal_atomic.h"
#include "hal_sys.h"
#include "hal_pincfg.h"
#include "hal_fieldbus.h"

#include "debug.h"

/*
   legacy fieldbus with the 9-bit uart.
   address byte (accessing bus) has a nine bit set, data bytes has now
   responce to the address bus has a nine bit set too, address should match

   data length is fixed

*/

enum fieldbus_ev_t
{
    EV_ENTRY,
    EV_EXIT,
    EV_BYTE_RECEIVED,
    EV_TXBUF_EMPTY,
    EV_SHIFTREG_EMPTY,
    EV_OVERRUN,
    EV_TOUT,
};

typedef void (*state_t)(uint ev, uint data);


static void state_sending_addr(uint ev, uint data);
static void state_receiving_addr(uint ev, uint data);
static void state_sending_data(uint ev, uint data);
static void state_receiving_data(uint ev, uint data);


typedef struct
{
    volatile state_t state;
    volatile hal_fieldbus_status_t status;
    volatile uint byte_idx;

    u32 rxtout;

    const u8 *tx;
    u8 *rx;
    uint txlen;
    uint rxlen;
    u8 addr;
} hal_fieldbus_rt_t;


static USART_TypeDef *const uart = UART4;
static TIM_TypeDef * const timer = TIM7;
static const u32 active_cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_M;

static hal_fieldbus_rt_t rt;

static inline void switch_to_tx(void)   { GPIOC->BSRR = 1 << 12; }
static inline void switch_to_rx(void)   { GPIOC->BSRR = 1 << (12 + 16); }

static void trans(state_t dst)
{
    dst(EV_EXIT, 0);
    rt.state = dst;
    dst(EV_ENTRY, 0);
}

static void start_timer(u32 ms)
{
    timer->CNT = 0;
    timer->ARR = ms * 10;
    timer->CR1 = TIM_CR1_CEN;
}

static void stop_timer(void)
{
    timer->CR1 = 0;
    timer->SR = 0;
    __NOP();        // sr write may lag, give nvic a time to react
    __NOP();
    NVIC_ClearPendingIRQ(TIM7_IRQn);
}


//-- state handlers

static void state_idling(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        uart->CR1 = active_cr1; // disable all interrupts
        return;
    }
}

static void state_sending_addr(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_tx();
        rt.status = HAL_FIELDBUS_BUSY;

        while (uart->SR & (USART_SR_RXNE | USART_SR_ORE))
            uart->DR;

        uart->SR = 0;
        uart->DR = rt.addr | 0x100;
        uart->CR1 = active_cr1 | USART_CR1_TCIE;  // wait for transmission complete
        return;

    case EV_SHIFTREG_EMPTY:
        trans(state_receiving_addr);
        return;
    }
}


static void state_receiving_addr(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        if (rt.rxtout)
            start_timer(rt.rxtout);
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data != (rt.addr | 0x100))
            rt.status = HAL_FIELDBUS_ERR_ADDRESS_MISMATCH;
        else
            rt.status = HAL_FIELDBUS_IDLE;
        trans(state_idling);
        return;

    case EV_TOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_EXIT:
        stop_timer();
        return;
    }
}


static void state_sending_data(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_tx();
        REQUIRE(rt.tx && rt.txlen);
        rt.status = HAL_FIELDBUS_BUSY;
        rt.byte_idx = 0;
        uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_TCIE;  // leave only transmit interrupts
        return;

    case EV_SHIFTREG_EMPTY:
    case EV_TXBUF_EMPTY:
        {
            uint idx = rt.byte_idx;
            if (idx < rt.txlen)
            {
                uart->DR = rt.tx[idx++];
                rt.byte_idx = idx;
                return;
            }

            if (ev == EV_TXBUF_EMPTY)
            {
                uart->CR1 = active_cr1 | USART_CR1_TCIE;    // leave only transmit complete interrupt to reduce load
                return;
            }

            if (rt.rxlen)
            {
                trans(state_receiving_data);
            }
            else
            {
                rt.status = HAL_FIELDBUS_IDLE;
                trans(state_idling);
            }
        }
        return;
    }
}


static void state_receiving_data(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        REQUIRE(rt.rx && rt.rxlen);
        rt.status = HAL_FIELDBUS_BUSY;
        rt.byte_idx = 0;
        if (rt.rxtout)
            start_timer(rt.rxtout);
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data & 0x100)
        {
            rt.status = HAL_FIELDBUS_ERR_COLLISION;
            trans(state_idling);
        }
        else
        {
            uint idx = rt.byte_idx;
            rt.rx[idx++] = data;

            if (idx < rt.rxlen)
            {
                rt.byte_idx = idx;
            }
            else
            {
                rt.status = HAL_FIELDBUS_IDLE;
                trans(state_idling);
            }
        }
        return;

    case EV_TOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_EXIT:
        stop_timer();
        return;
    }
}

// -- event dispatchers

void timer7_isr(void)
{
    timer->CR1 = 0;
    timer->SR = 0;
    rt.state(EV_TOUT, 0);
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


void HAL_fieldbus_init(uint baud)
{
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

    hal_pincfg_out_alt(GPIOC, 10);  // tx

    GPIOC->BSRR = 1 << 11;          // rx with pull-up
    hal_pincfg_in(GPIOC, 11);

    GPIOC->BSRR = 1 << (12 + 16);    // dir, cleared by default (rx)
    hal_pincfg_out(GPIOC, 12);

    // NOTE: priorities of uart and timer should be the same to avoid races
    NVIC_SetPriority(UART4_IRQn, HAL_IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(UART4_IRQn);
    NVIC_EnableIRQ(UART4_IRQn);

    uart->CR1 = 0;
    uart->CR2 = 0;
    uart->CR3 = 0;
    uart->BRR = (HAL_SYS_F_CPU + baud/2 - 1) / baud;

    timer->CR1 = 0;
    timer->CNT = 0;
    timer->PSC = HAL_SYS_F_CPU * 0.1E-3 - 1;    // 0.1 ms prescaler. this way 1-2 ms timeouts error would be negligible
    timer->ARR = 0;
    timer->DIER = TIM_DIER_UIE;
    timer->EGR = TIM_EGR_UG;                    // load prescaler/etc
    timer->SR = 0;

    NVIC_SetPriority(TIM7_IRQn, HAL_IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(TIM7_IRQn);
    NVIC_EnableIRQ(TIM7_IRQn);


    rt.state = state_idling;
    trans(state_idling);
}


bool HAL_fieldbus_address_slave(u8 slave_addr, u32 tout)
{
    if (rt.state != state_idling)
        return 0;

    rt.addr = slave_addr;
    rt.rxtout = tout;

    trans(state_sending_addr);
    return 1;
}

bool HAL_fieldbus_request_xfer(const u8 *tx, uint txlen, u8 *rx, uint rxlen, u32 rx_tout)
{
    if (rt.state != state_idling)
        return 0;

    rt.tx = tx;
    rt.rx = rx;
    rt.txlen = txlen;
    rt.rxlen = rxlen;
    rt.rxtout = rx_tout;

    if (txlen)
        trans(state_sending_data);
    else if (rxlen)
        trans(state_receiving_data);
    else
        ;   // stay idle, transaction is degenerate
    return 1;
}


void HAL_fieldbus_abort(void)
{
    if (rt.state != state_idling)
    {
        HAL_BLOCK_INTERRUPTS_LEQ_TO(HAL_IRQ_PRIORITY_NORMAL)
        {
            trans(state_idling);
        }

        while (! (uart->SR & USART_SR_TC)); // wait for the end of transmission
        uart->SR = 0;
    }

    rt.status = HAL_FIELDBUS_IDLE;
}


hal_fieldbus_status_t HAL_fieldbus_get_status(void)
{
    return rt.status;
}

bool HAL_fieldbus_is_busy(void)
{
    return rt.status == HAL_FIELDBUS_BUSY;
}
