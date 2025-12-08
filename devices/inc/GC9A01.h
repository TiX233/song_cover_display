/**
 * @file GC9A01.h
 * @author realTiX
 * @brief GC9A01 240*240 圆形 lcd 显示屏驱动库
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __GC9A01_H__
#define __GC9A01_H__

#include "main.h"

#define LCD_WIDTH   240
#define LCD_HEIGHT  240

#define RGB565_WHITE       0xFFFF
#define RGB565_BLACK       0x0000
#define RGB565_BLUE        0x001F
#define RGB565_RED         0xF800
#define RGB565_MAGENTA     0xF81F
#define RGB565_GREEN       0x07E0
#define RGB565_CYAN        0x7FFF
#define RGB565_YELLOW      0xFFE0
#define RGB565_GRAY        0X8430
#define RGB565_BRED        0XF81F
#define RGB565_GRED        0XFFE0
#define RGB565_GBLUE       0X07FF
#define RGB565_BROWN       0XBC40
#define RGB565_BRRED       0XFC07
#define RGB565_DARKBLUE    0X01CF
#define RGB565_LIGHTBLUE   0X7D7C
#define RGB565_GRAYBLUE    0X5458

#define RGB565_LIGHTGREEN  0X841F
#define RGB565_LGRAY       0XC618
#define RGB565_LGRAYBLUE   0XA651
#define RGB565_LBBLUE      0X2B12

#define GC9A01_PIN_LEVEL_DC_DATA    1
#define GC9A01_PIN_LEVEL_DC_CMD     0

typedef enum {
    LCD_CTRL_WRITE_CMD = 0,
    LCD_CTRL_WRITE_DATA,
    LCD_CTRL_DELAY = 0x25,
    LCD_CTRL_OVER = 0x68,
} LCD_CTRL_e;

typedef struct{
    uint8_t ctrl;
    union {
        uint16_t data_len;
        uint16_t delay_ms;
    };
    const uint8_t *data_buf;
} lcd_init_seq_stu;

struct rgb_565_stu {
    union {
        uint16_t rgb;
        struct {
            uint16_t r:5;
            uint16_t g:6;
            uint16_t b:5;
        } one;
    } color;
};

struct gc9a01_stu {
    uint8_t is_initialized;

    void (*write_cs)(uint8_t pin_level);
    void (*write_dc)(uint8_t pin_level);
    void (*write_rst)(uint8_t pin_level);

    void (*set_backlight)(uint8_t level);

    void (*transmit_data)(const uint8_t *buf, uint16_t len);
    void (*transmit_data_dma)(const uint8_t *buf, uint16_t len);

    void (*delay_ms)(uint32_t ms);
    void (*pin_init)(void);
};

int gc9a01_init(struct gc9a01_stu *lcd);
void gc9a01_clear(struct gc9a01_stu *lcd, uint16_t color);
void gc9a01_fill_pixel(struct gc9a01_stu *lcd, uint16_t x, uint16_t y, uint16_t color);
void gc9a01_fill_color(struct gc9a01_stu *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);


void gc9a01_write_cmd(struct gc9a01_stu *lcd, uint8_t cmd);
void gc9a01_write_data(struct gc9a01_stu *lcd, const uint8_t *data, uint16_t len);
uint8_t gc9a01_write_data_dma(struct gc9a01_stu *lcd, const uint8_t *data, uint16_t len);
void gc9a01_set_window(struct gc9a01_stu *lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

#endif // __GC9A01_H__
