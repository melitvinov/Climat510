#ifndef _HAL_FIELDBUS_H_
#define _HAL_FIELDBUS_H_

#define FIELDBUS_MAX_DATALEN    384

typedef enum
{
    FIELDBUS_BUSY = -1,
    FIELDBUS_IDLE = 0,
    FIELDBUS_ERR_ADDRESS_MISMATCH,
    FIELDBUS_ERR_BAD_CHECKSUM,
    FIELDBUS_ERR_INTERNAL_ERROR,
    FIELDBUS_ERR_TIMEOUT,
    FIELDBUS_ERR_COLLISION,
} fieldbus_status_t;

void fieldbus_init(void);
bool fieldbus_request_write(u8 slave_addr, uint reg_addr, uint blocknum, const void *data, uint datalen);
bool fieldbus_request_read(u8 slave_addr, uint reg_addr, uint blocknum, void *data, uint datalen);
fieldbus_status_t fieldbus_get_status(void);
void fieldbus_abort(void);

#ifdef _FIELDBUS_C_
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
    EV_TIMER_EXPIRED,
    EV_XFER_COMPLETED,
    EV_XFER_FAILED,
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
    timer_t timer;

    hdr_t hdr;

    void *rx_dst;
    state_t state;
    fieldbus_status_t status;

    u8 addr;
    u8 buf[FIELDBUS_MAX_DATALEN + 2];   // + 2 is for the checksums

} fieldbus_rt_t;

static void state_idling(uint ev, uint data);
static void addressing_slave(uint ev, uint arg);
static void state_waiting_before_header_send(uint ev, uint data);
static void state_sending_read_header_and_receiving_data(uint ev, uint data);

static void state_sending_write_header_and_receiving_checksum(uint ev, uint data);
static void state_waiting_before_data_write(uint ev, uint data);
static void state_writing_data_and_receiving_checksum(uint ev, uint data);

static void on_timer_expired(timer_t *dummy);
static void check_xfer(timer_t *dummy);

#endif
#endif
