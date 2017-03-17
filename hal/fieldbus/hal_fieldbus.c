#define _HAL_FIELDBUS_C_
#define _HAL_

#include "syntax.h"
#include "stm32f10x.h"

#include "hal_priorities.h"
#include "hal_atomic.h"
#include "hal_sys.h"
#include "hal_pincfg.h"
#include "hal_systimer.h"
#include "hal_fieldbus.h"

#include "debug.h"

/*
    fieldbus is very legacy, not easy to integrate it above the primitive serial read/write
    so the implementation isolate the complexies inside the hal module
    there is some logic, a sequence of requests and responses embedded in low-level code.
    non-standart 9-bit serial format is exploited for signalling the start of transaction - address byte had msbit set,
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

static USART_TypeDef *const uart = UART4;
static const u32 active_cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_M;

static fieldbus_rt_t rt;

static inline void switch_to_tx(void)   { GPIOC->BSRR = 1 << 12; }
static inline void switch_to_rx(void)   { GPIOC->BSRR = 1 << (12 + 16); }

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
        rt.ticks_till_timeout = ADDRESSING_TIMEOUT;
        HAL_BLOCK_INTERRUPTS_LEQ_TO(HAL_IRQ_PRIORITY_HIGH)
        {
            hal_systimer_add_task(&rt.timer, timer_handler);
        }
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_TIMEOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data != (rt.addr | 0x100))
        {
            rt.status = HAL_FIELDBUS_ERR_ADDRESS_MISMATCH;
            trans(state_idling);
        }
        else
        {
            trans(state_waiting_before_hdr_send);
        }
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_waiting_before_hdr_send(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        rt.ticks_till_timeout = SLAVE_SETUP_TIMEOUT;        // 2-3 ms depending on a fastloop clock phase
        hal_systimer_add_task(&rt.timer, timer_handler);
        uart->CR1 = active_cr1;                             // no uart activity
        return;

    case EV_TIMEOUT:
        trans(state_sending_hdr);
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_sending_hdr(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_tx();
        rt.p = rt.hdr.raw;
        rt.remain = sizeof(hdr_t);
        rt.checksum = 0;
        uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_TCIE;  // leave only transmit interrupts
        return;

    case EV_SHIFTREG_EMPTY:
    case EV_TXBUF_EMPTY:
        if (rt.remain)
        {
            uart->DR = *rt.p;
            rt.checksum += *rt.p;
            rt.p += 1;
            rt.remain -= 1;
        }
        else
        {
            if (ev == EV_TXBUF_EMPTY)
                uart->CR1 = active_cr1 | USART_CR1_TCIE;    // leave only transmit complete interrupt to reduce load
            else
                trans(state_receiving_hdr_checksum);
        }
        return;
    }
}


static void state_receiving_hdr_checksum(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        rt.ticks_till_timeout = REPLY_TIMEOUT;
        hal_systimer_add_task(&rt.timer, timer_handler);
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_TIMEOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data & 0x100)
        {
            rt.status = HAL_FIELDBUS_ERR_COLLISION;
            trans(state_idling);
        }
        else if (data != rt.checksum)
        {
            rt.status = HAL_FIELDBUS_ERR_BAD_CHECKSUM;
            trans(state_idling);
        }
        else if ((rt.hdr.dir_and_blocknum & DIR_MASK) == DIR_READ_BITS)
        {
            trans(state_receiving_data);
        }
        else
        {
            trans(state_waiting_before_data_send);
        }
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_receiving_data(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        rt.p = rt.data;
        rt.remain = rt.hdr.datalen;
        rt.checksum = 0;
        rt.ticks_till_timeout = REPLY_TIMEOUT + calc_transfer_time(rt.remain) * 2;
        hal_systimer_add_task(&rt.timer, timer_handler);
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_TIMEOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data & 0x100)
        {
            rt.status = HAL_FIELDBUS_ERR_COLLISION;
            trans(state_idling);
            return;
        }

        *rt.p++ = data;
        rt.checksum += data;

        if (! --rt.remain)
            trans(state_receiving_data_checksum);
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_receiving_data_checksum(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_rx();
        rt.ticks_till_timeout = REPLY_TIMEOUT;
        hal_systimer_add_task(&rt.timer, timer_handler);
        uart->CR1 = active_cr1 | USART_CR1_RXNEIE;  // leave only rx interrupt
        return;

    case EV_OVERRUN:
        rt.status = HAL_FIELDBUS_ERR_DATA_LOST;
        trans(state_idling);
        return;

    case EV_TIMEOUT:
        rt.status = HAL_FIELDBUS_ERR_TIMEOUT;
        trans(state_idling);
        return;

    case EV_BYTE_RECEIVED:
        if (data & 0x100)
            rt.status = HAL_FIELDBUS_ERR_COLLISION;
        else if (data != rt.checksum)
            rt.status = HAL_FIELDBUS_ERR_BAD_CHECKSUM;
        else
            rt.status = HAL_FIELDBUS_IDLE;
        trans(state_idling);
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_waiting_before_data_send(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        rt.ticks_till_timeout = SLAVE_SETUP_TIMEOUT;        // 2-3 ms depending on a fastloop clock phase
        hal_systimer_add_task(&rt.timer, timer_handler);
        uart->CR1 = active_cr1;                             // no uart activity
        return;

    case EV_TIMEOUT:
        trans(state_sending_data);
        return;

    case EV_EXIT:
        hal_systimer_remove_task(&rt.timer);
        return;
    }
}


static void state_sending_data(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_tx();
        rt.p = rt.data;
        rt.remain = rt.hdr.datalen;
        rt.checksum = 0;
        uart->CR1 = active_cr1 | USART_CR1_TXEIE | USART_CR1_TCIE;  // leave only transmit interrupts
        return;

    case EV_TXBUF_EMPTY:
    case EV_SHIFTREG_EMPTY:
        if (rt.remain)
        {
            uart->DR = *rt.p;
            rt.checksum += *rt.p;
            rt.p += 1;
            rt.remain -= 1;
        }
        else
        {
            trans(state_sending_data_checksum);
        }
        return;
    }
}


static void state_sending_data_checksum(uint ev, uint data)
{
    switch (ev)
    {
    case EV_ENTRY:
        switch_to_tx();
        uart->DR = (55 - rt.checksum) & 0xFF;
        uart->CR1 = active_cr1 | USART_CR1_TCIE;  // wait for transmission complete
        return;

    case EV_SHIFTREG_EMPTY:
        rt.checksum = 55;
        trans(state_receiving_data_checksum);
        return;
    }
}


// -- event dispatchers

static void timer_handler(void)
{
    if (rt.ticks_till_timeout--)
        return;

    rt.state(EV_TIMEOUT, 0);
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


void HAL_fieldbus_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

    hal_pincfg_out_alt(GPIOC, 10);  // tx

    GPIOC->BSRR = 1 << 11;          // rx with pull-up
    hal_pincfg_in(GPIOC, 11);

    GPIOC->BSRR = 1 << (12 + 16);    // dir, cleared by default (rx)
    hal_pincfg_out(GPIOC, 12);

    // NOTE: priorities of uart and timer should be the same to avoid races
    PANIC_IF(HAL_SYSTIMER_IRQ_PRIORITY != HAL_IRQ_PRIORITY_NORMAL);

    NVIC_SetPriority(UART4_IRQn, HAL_IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(UART4_IRQn);
    NVIC_EnableIRQ(UART4_IRQn);

    uart->CR1 = 0;
    uart->CR2 = 0;
    uart->CR3 = 0;
    uart->BRR = (HAL_SYS_F_CPU + BAUDRATE/2 - 1) / BAUDRATE;

    rt.state = state_idling;
    trans(state_idling);
}


bool HAL_fieldbus_request_write(u8 slave_addr, uint reg_addr, uint blocknum, const void *data, uint datalen)
{
    REQUIRE(data && datalen);
    REQUIRE((blocknum & DIR_MASK) == 0);

    if (rt.state != state_idling)
        return 0;

    rt.addr = slave_addr;
    rt.data = (u8 *)data;  // won't change it, promise !

    rt.hdr.reg_addr = reg_addr;
    rt.hdr.datalen = datalen;
    rt.hdr.dir_and_blocknum = DIR_WRITE_BITS | blocknum;

    rt.status = HAL_FIELDBUS_BUSY;

    trans(state_sending_addr);

    return 1;
}


bool HAL_fieldbus_request_read(u8 slave_addr, uint reg_addr, uint blocknum, void *data, uint datalen)
{
    REQUIRE(data && datalen);
    REQUIRE((blocknum & DIR_MASK) == 0);

    if (rt.state != state_idling)
        return 0;

    rt.addr = slave_addr;
    rt.data = data;

    rt.hdr.reg_addr = reg_addr;
    rt.hdr.datalen = datalen;
    rt.hdr.dir_and_blocknum = DIR_READ_BITS | blocknum;

    rt.status = HAL_FIELDBUS_BUSY;

    trans(state_sending_addr);

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

    rt.status = HAL_FIELDBUS_ERR_ABORTED;
}


hal_fieldbus_status_t HAL_fieldbus_get_status(void)
{
    return rt.status;
}

bool HAL_fieldbus_is_busy(void)
{
    return rt.status == HAL_FIELDBUS_BUSY;
}

//-- tests
#if 1

static void smoke_read_status(uint addr)
{
    u16 slave_status = 0xFFFF;

    bool is_ok = HAL_fieldbus_request_read(addr, 0, 0, &slave_status, sizeof(slave_status));

    if (! is_ok)
    {
        WARN("failed to request read");
        return;
    }

    while (HAL_fieldbus_is_busy());

    int status = HAL_fieldbus_get_status();
    if (status != HAL_FIELDBUS_IDLE)
        WARN("fieldbus status is %d", status);
    else
        LOG("slave condition is %d", slave_status);
}


static void smoke_write_output(uint addr, u32 output)
{
    bool is_ok = HAL_fieldbus_request_write(addr, 0, 3, &output, sizeof(output));

    if (! is_ok)
    {
        WARN("failed to request write");
        return;
    }

    while (HAL_fieldbus_is_busy());

    int status = HAL_fieldbus_get_status();
    if (status != HAL_FIELDBUS_IDLE)
        WARN("fieldbus status is %d", status);
    else
        LOG("write ok");
}

void HAL_fieldbus_smoke(void)
{
    HAL_fieldbus_init();

    u32 output = 0x0000FF00;
    u8 i = 0;
    while (1)
    {
        smoke_read_status(121);
        HAL_systimer_sleep(100);
        smoke_write_output(121, output);
        HAL_systimer_sleep(1000);

        output ^= (i++ & 0xFF) << 8;
    }

}

#endif
