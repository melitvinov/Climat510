#ifndef _HAL_H_
#define _HAL_H_

//--- hal_beep.h

void HAL_beep_init(void);

void HAL_beep_on(u32 hz);
void HAL_beep_off(void);


//--- hal_tty.h

void HAL_tty_init(uint baud);
void HAL_tty_putc(u8 chr);
void HAL_tty_puts(const char *str);
int HAL_tty_getc(void);


//--- hw_flags.h

// these are for compatibility with the fwd network stack

// PHY flags are in bits 15-0
#define PHY_HALF_DUPLEX        0
#define PHY_10MBIT             0
#define PHY_100MBIT            (1 << 0)
#define PHY_FULL_DUPLEX        (1 << 1)
#define PHY_AUTO_NEGOTIATION   (1 << 2)
#define PHY_LOOPBACK           (1 << 3)

// MAC flags are in bits 31-16
#define MAC_ACCEPT_BROADCAST    (1 << 16)
#define MAC_ACCEPT_MULTICAST    (1 << 17)
#define MAC_ACCEPT_ANY_UNICAST  (1 << 18)


//--- hal_mac.h

// MTU of ethernet
#define HAL_ETH_MTU             1500
#define HAL_ETH_HDR_SIZE        14
// reserved bytes at the end of buffer (for appending zero-terminators to http steam etc)
#define HAL_ETH_BUF_EXTRA_TAIL  4

void HAL_mac_init(const u8 *mac_addr, u32 flags);
void *HAL_mac_read_packet(uint *len);
uint HAL_mac_write_packet(void *data, uint len);
void HAL_mac_periodic(void);

void *HAL_mac_alloc_buf(void);
void HAL_mac_free_buf(void *addr);


//--- hal_debug.h

void HAL_assert(const char *str) __noreturn;
void hal_exception(u32 pc, const u32 *sp);


//--- hal_lcd.h

// for the graphic mode
#define HAL_LCD_XSIZE   240
#define HAL_LCD_YSIZE   64

// for the text mode
#define HAL_LCD_NCOLS   (HAL_LCD_XSIZE / 6)
#define HAL_LCD_NROWS   (HAL_LCD_YSIZE / 8)

typedef union
{
    u8 raw[HAL_LCD_NCOLS * HAL_LCD_NROWS];
    u8 lines[HAL_LCD_NROWS][HAL_LCD_NCOLS];
} hal_lcd_text_screen_t;

typedef union
{
    u8 raw[HAL_LCD_YSIZE * HAL_LCD_XSIZE / 8];
    u8 lines[HAL_LCD_YSIZE][HAL_LCD_XSIZE / 8];
} hal_lcd_graph_screen_t;

void HAL_lcd_init(void);
bool HAL_lcd_render_text_screen(const hal_lcd_text_screen_t *screen);
bool HAL_lcd_render_graph_screen(const hal_lcd_graph_screen_t *screen);
bool HAL_lcd_position_cursor(uint col, uint row, uint size, bool is_blinking);


//--- hal_nvmem.h

#define HAL_NVMEM_SIZE  32768

void HAL_nvmem_init(void);
bool HAL_nvmem_read(u32 base, void *dst, uint size);
bool HAL_nvmem_write(u32 base, const void *src, uint size);


//--- hal_systimer.h

void HAL_systimer_init(void);

u32 HAL_systimer_get(void);
void HAL_systimer_sleep(u32 ms);
void hal_systimer_sleep_us(u32 us);


//--- hal_fieldbus.h

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


//--- hal_rtc.h

void HAL_rtc_init(void);
u32 HAL_rtc_get_timestamp(void);
void HAL_rtc_set_timestamp(u32 timestamp);


//--- hal_keyb.h

// key numbers aka bit positions
enum hal_keys_e
{
    HAL_KEY_0,
    HAL_KEY_4,
    HAL_KEY_8,
    HAL_KEY_LEFT,
    HAL_KEY_1,
    HAL_KEY_5,
    HAL_KEY_9,
    HAL_KEY_UP,
    HAL_KEY_2,
    HAL_KEY_6,
    HAL_KEY_EDIT,
    HAL_KEY_RIGHT,
    HAL_KEY_3,
    HAL_KEY_7,
    _HAL_KEY_NC,
    HAL_KEY_DOWN,
};

void HAL_keyb_init(void);
u32 HAL_keyb_read(void);


#endif