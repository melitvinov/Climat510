#ifndef _HAL_RS485_H_
#define _HAL_RS485_H_

enum hal_rs485_err_e
{
    HAL_RS485_ERR_OVERRUN = 1,
    HAL_RS485_ERR_TIMEOUT,
    HAL_RS485_ERR_OUT_OF_SYNC,
    HAL_RS485_ERR_ABORTED,
};

typedef struct
{
    const void *req;
    uint req_size;
    void *resp;
    uint resp_size;
    u32 timeout;
    bool should_set_ninebit;
    bool expect_ninebit;
    volatile u8 err;
    volatile u8 is_done;
} HAL_rs485_transfer_t;

void HAL_rs485_init(uint baud);
bool HAL_rs485_transfer(HAL_rs485_transfer_t *xfer);
bool HAL_rs485_is_busy(void);
void HAL_rs485_abort(void);

#endif
