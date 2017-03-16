#ifndef _HAL_FIELDBUS_H_
#define _HAL_FIELDBUS_H_

typedef enum
{
    HAL_FIELDBUS_BUSY = -1,
    HAL_FIELDBUS_IDLE = 0,
    HAL_FIELDBUS_ERR_ADDRESS_MISMATCH,
    HAL_FIELDBUS_ERR_BAD_CHECKSUM,
    HAL_FIELDBUS_ERR_DATA_LOST,
    HAL_FIELDBUS_ERR_TIMEOUT,
    HAL_FIELDBUS_ERR_COLLISION,
    HAL_FIELDBUS_ERR_ABORTED,
} hal_fieldbus_status_t;

void HAL_fieldbus_init(void);
bool HAL_fieldbus_request_write(uint addr, const void *hdr, uint hdrlen, const void *data, uint datalen);
bool HAL_fieldbus_request_read(uint addr, const void *hdr, uint hdrlen, void *data, uint datalen);
hal_fieldbus_status_t HAL_fieldbus_get_status(void);
void HAL_fieldbus_abort(void);

#endif
