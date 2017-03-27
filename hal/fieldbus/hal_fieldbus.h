#ifndef _HAL_FIELDBUS_H_
#define _HAL_FIELDBUS_H_

typedef enum
{
    HAL_FIELDBUS_BUSY = -1,
    HAL_FIELDBUS_IDLE = 0,
    HAL_FIELDBUS_ERR_ADDRESS_MISMATCH,
    HAL_FIELDBUS_ERR_TIMEOUT,
    HAL_FIELDBUS_ERR_DATA_LOST,
    HAL_FIELDBUS_ERR_COLLISION,
} hal_fieldbus_status_t;

void HAL_fieldbus_init(uint baud);
bool HAL_fieldbus_address_slave(u8 slave_addr, u32 tout);
bool HAL_fieldbus_request_xfer(const u8 *tx, uint tx_len, u8 *rx, uint rx_len, u32 tout);
hal_fieldbus_status_t HAL_fieldbus_get_status(void);
void HAL_fieldbus_abort(void);

#ifdef _HAL_FIELDBUS_C_

#define BAUDRATE                    9600

#endif
#endif
