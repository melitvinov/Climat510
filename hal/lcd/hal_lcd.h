#ifndef _HAL_LCD_H_
#define _HAL_LCD_H_

// for the graphic mode
#define HAL_LCD_XSIZE   240
#define HAL_LCD_YSIZE   64

// for text mode
#define HAL_LCD_NCOLS   (HAL_LCD_XSIZE / 6)
#define HAL_LCD_NROWS   (HAL_LCD_YSIZE / 8)

typedef union
{
    u8 raw[HAL_LCD_NCOLS * HAL_LCD_NROWS];
    u8 lines[HAL_LCD_NROWS][HAL_LCD_NCOLS];
} hal_lcd_text_buf_t;

typedef union
{
    u8 raw[HAL_LCD_YSIZE * HAL_LCD_XSIZE / 8];
    u8 lines[HAL_LCD_YSIZE][HAL_LCD_XSIZE / 8];
} hal_lcd_graph_buf_t;

void HAL_lcd_init(void);
bool HAL_lcd_render_text(const hal_lcd_text_buf_t *buf);
bool HAL_lcd_render_ugly_encoded_text(const hal_lcd_text_buf_t *buf);
bool HAL_lcd_render_graphic(const hal_lcd_graph_buf_t *buf);
bool HAL_lcd_position_cursor(uint col, uint row, uint size, bool is_blinking);

#ifdef _HAL_LCD_C_

// NOTE: there is strange aliasing in memory.
// 0x1000-0x1FFF maps to 0x0000-0x0FFF
// so runtime data is placed in the region below first 8k.
// characted generator, being a const data, is placed above 16k
#define LCD_RAM_SIZE        0x2000
#define GRAPHIC_PAGE_ADDR   0x0000
#define ATTRIBUTE_PAGE_ADDR 0x0A00
#define TEXT_PAGE_ADDR      0x1400
#define CG_BASE_ADDR        0x4000

#define STATUS_CHECK_N_TRIES 1024
#define CONFIGURE_ATTEMPTS 5

enum ra6963_cmd_e
{
    SET_CURSOR_POINTER = 0x21,
    SET_OFFSET_REGISTER = 0x22,
    SET_ADDRRESS_POINTER = 0x24,

    SET_TEXT_HOME_ADDRESS = 0x40,
    SET_TEXT_AREA = 0x41,
    SET_GRAPHIC_HOME_ADDRESS = 0x42,
    SET_GRAPHIC_AREA = 0x43,

    SET_MODE_PREFIX = 0x80,         // 4 msbits
    SET_DISPLAY_MODE_PREFIX = 0x90, // 4 msbits

    SET_DATA_AUTO_WRITE = 0xB0,
    SET_DATA_AUTO_READ = 0xB1,
    AUTO_RESET = 0xB2,

    SET_CURSOR_SIZE_PREFIX = 0xA0,  // 5 msbits
};

enum ra6932_set_mode_flags_e
{
    F_MODE_OR = 0x00,
    F_MODE_EXOR = 0x01,
    F_MODE_AND = 0x03,
    F_MODE_TEXT_ATTRIBUTE = 0x04,
    F_MODE_EXT_CG = 0x08,
};

enum ra6932_set_display_mode_flags_e
{
    F_DISPLAY_MODE_OFF = 0,
    F_DISPLAY_MODE_CURSOR_ON_BLINK_OFF = 0x02,
    F_DISPLAY_MODE_CURSOR_ON_BLINK_ON = 0x03,
    F_DISPLAY_MODE_TEXT_ON_GRAPHIC_OFF = 0x04,
    F_DISPLAY_MODE_TEXT_OFF_GRAPHIC_ON = 0x08,
    F_DISPLAY_MODE_TEXT_ON_GRAPHIC_ON = 0x0C,
};

#endif
#endif
