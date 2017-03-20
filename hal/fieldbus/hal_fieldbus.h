#ifndef _HAL_FIELDBUS_H_
#define _HAL_FIELDBUS_H_
// public

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
bool HAL_fieldbus_request_write(u8 slave_addr, uint reg_addr, uint blocknum, const void *data, uint datalen);
bool HAL_fieldbus_request_read(u8 slave_addr, uint reg_addr, uint blocknum, void *data, uint datalen);
hal_fieldbus_status_t HAL_fieldbus_get_status(void);
void HAL_fieldbus_abort(void);

#ifdef _HAL_FIELDBUS_C_
// private

#define BAUDRATE                    9600

#define ADDRESSING_TIMEOUT          100
#define REPLY_TIMEOUT               50
#define SLAVE_SETUP_TIMEOUT         2

#define DIR_WRITE_BITS  0xA0
#define DIR_READ_BITS   0x50
#define DIR_MASK        0xF0

enum fieldbus_ev_t
{
    EV_ENTRY,
    EV_EXIT,
    EV_OVERRUN,
    EV_BYTE_RECEIVED,
    EV_TXBUF_EMPTY,
    EV_SHIFTREG_EMPTY,
    EV_TIMEOUT,
};

typedef union __packed
{
    struct __packed
    {
        u16 reg_addr;
        u16 datalen;
        u8 dir_and_blocknum;
    };
    u8 raw[0];
} hdr_t;


typedef void (*state_t)(uint ev, uint data);

// XXX: a few more volatiles must be there
typedef struct
{
    hdr_t hdr;
    void *data;
    u8 *p;
    uint remain;
    volatile state_t state;
    volatile hal_fieldbus_status_t status;
    u8 checksum;
    volatile u8 addr;
} fieldbus_rt_t;

static void state_idling(uint ev, uint data);
static void state_sending_addr(uint ev, uint data);
static void state_receiving_addr(uint ev, uint data);
static void state_waiting_before_hdr_send(uint ev, uint data);
static void state_sending_hdr(uint ev, uint data);
static void state_receiving_hdr_checksum(uint ev, uint data);
static void state_waiting_before_data_send(uint ev, uint data);
static void state_sending_data(uint ev, uint data);
static void state_sending_data_checksum(uint ev, uint data);
static void state_receiving_data(uint ev, uint data);
static void state_receiving_data_checksum(uint ev, uint data);

#endif
#endif
