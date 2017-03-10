#ifndef _HAL_LCD_H_
#define _HAL_LCD_H_

// for the graphic mode
#define HAL_LCD_XSIZE   240
#define HAL_LCD_YSIZE   64

// for text mode
#define HAL_LCD_NCOLS   40
#define HAL_LCD_NROWS   8

void HAL_lcd_init(void);

#ifdef _HAL_LCD_C_

#define LCD_SRAM_SIZE        0x8000
#define TEXT_PAGE_ADDR       0
#define GRAPHIC_PAGE_ADDR    0x1000
#define CG_BASE_ADDR         0x2000

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
                                    //
    SET_DATA_AUTO_WRITE = 0xB0,
    SET_DATA_AUTO_READ = 0xB1,
    AUTO_RESET = 0xB2,
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
