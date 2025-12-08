#include "GC9A01.h"

#define USE_SPI_DMA


lcd_init_seq_stu gc9a01_init_table[] = {
    // enable inter register
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xFE}},
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xEF}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xEB}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x14}},    
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x84}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x40}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x88}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x0A}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x89}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x21}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x8A}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x00}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x8B}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x80}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x8C}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x01}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x8D}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x01}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB6}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x20}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x36}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x48}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x3A}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x05}},
    
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x90}},
    {LCD_CTRL_WRITE_DATA, 4, (const uint8_t []){0x08, 0x08, 0x08, 0x08}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xBD}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x06}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xBC}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x00}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xFF}},
    {LCD_CTRL_WRITE_DATA, 3, (const uint8_t []){0x60, 0x01, 0x04}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC3}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x13}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC4}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x13}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC9}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x22}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xBE}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x11}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xE1}},
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0x10, 0x0E}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xDF}},
    {LCD_CTRL_WRITE_DATA, 3, (const uint8_t []){0x21, 0x0c, 0x02}},
    
    // set gamma
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xF0}},
    {LCD_CTRL_WRITE_DATA, 6, (const uint8_t []){0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xF1}},
    {LCD_CTRL_WRITE_DATA, 6, (const uint8_t []){0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}},
    
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xF2}},
    {LCD_CTRL_WRITE_DATA, 6, (const uint8_t []){0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xF3}},
    {LCD_CTRL_WRITE_DATA, 6, (const uint8_t []){0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xED}},
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0x1B, 0x0B}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xAE}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x74}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xCD}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x63}},
    
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x70}},
    {LCD_CTRL_WRITE_DATA, 9, (const uint8_t []){0x07, 0x09, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xE8}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x34}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x62}},
    {LCD_CTRL_WRITE_DATA, 12, (const uint8_t []){0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x63}},
    {LCD_CTRL_WRITE_DATA, 12, (const uint8_t []){0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x64}},
    {LCD_CTRL_WRITE_DATA, 7, (const uint8_t []){0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x66}},
    {LCD_CTRL_WRITE_DATA, 10, (const uint8_t []){0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x67}},
    {LCD_CTRL_WRITE_DATA, 10, (const uint8_t []){0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x74}},
    {LCD_CTRL_WRITE_DATA, 7, (const uint8_t []){0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x98}},
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0x3e, 0x07}},
    
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x35}},
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x21}},
    {LCD_CTRL_DELAY, 120},
    // end of gamma setting
    

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x11}},
    {LCD_CTRL_DELAY, 320},
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x29}},
    {LCD_CTRL_DELAY, 120},
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x2C}},

    // init over
    {LCD_CTRL_OVER, },
};

uint8_t lcd_disp_area[LCD_WIDTH * 2] = {0};


void gc9a01_write_cmd(struct gc9a01_stu *lcd, uint8_t cmd){
    lcd->write_dc(GC9A01_PIN_LEVEL_DC_CMD);
    lcd->transmit_data(&cmd, 1);
}

void gc9a01_write_data(struct gc9a01_stu *lcd, const uint8_t *data, uint16_t len){
    lcd->write_dc(GC9A01_PIN_LEVEL_DC_DATA);
    lcd->transmit_data(data, len);
}

uint8_t gc9a01_write_data_dma(struct gc9a01_stu *lcd, const uint8_t *data, uint16_t len){

    lcd->write_dc(GC9A01_PIN_LEVEL_DC_DATA);
    lcd->transmit_data_dma(data, len);

    return 0;
}


int gc9a01_init(struct gc9a01_stu *lcd){
    if( !lcd->write_cs || 
        !lcd->write_dc  || 
        !lcd->write_rst || 
        !lcd->set_backlight || 
        !lcd->transmit_data || 
        !lcd->transmit_data_dma || 
        !lcd->delay_ms || 
        !lcd->pin_init){

        return -1;
    }

    lcd->is_initialized = 0;
    lcd->pin_init();
    lcd->set_backlight(0);

lcd->write_cs(1);

    // 复位
    lcd->write_rst(1);
    lcd->delay_ms(10);
    lcd->write_rst(0);
    lcd->delay_ms(50);
    lcd->write_rst(1);
    lcd->delay_ms(100);

lcd->write_cs(0);

    for(uint16_t i = 0; ; i++){
        switch(gc9a01_init_table[i].ctrl){
            case LCD_CTRL_WRITE_CMD:
                gc9a01_write_cmd(lcd, gc9a01_init_table[i].data_buf[0]);
                break;
                
            case LCD_CTRL_WRITE_DATA:
                gc9a01_write_data(lcd, gc9a01_init_table[i].data_buf, gc9a01_init_table[i].data_len);
                break;
                
            case LCD_CTRL_DELAY:
                lcd->delay_ms(gc9a01_init_table[i].delay_ms);
                break;
                
            case LCD_CTRL_OVER:
                lcd->is_initialized = 1;
                return 0;
                break;

            default:
                return -2;
                break;
        }
    }

    return -3;
}

void gc9a01_set_window(struct gc9a01_stu *lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint8_t temp[4] = {0,x0,x1>>8,x1};

    gc9a01_write_cmd(lcd, 0x2A); // Column Address Set
    gc9a01_write_data(lcd, temp, 4);

    gc9a01_write_cmd(lcd, 0x2B); // Page Address Set
    temp[1] = y0;
    temp[3] = y1;
    gc9a01_write_data(lcd, temp, 4);

    gc9a01_write_cmd(lcd, 0x2C); // Memory Write
}

/**
 * @brief 画点
 */
void gc9a01_fill_pixel(struct gc9a01_stu *lcd, uint16_t x, uint16_t y, uint16_t color){
    uint8_t buf[2] = {(color >> 8) & 0xFF, color & 0xFF};

    gc9a01_set_window(lcd, x, y, x, y);
    gc9a01_write_data(lcd, buf, 2);
}

/**
 * @brief 画单色块
 */
void gc9a01_fill_color(struct gc9a01_stu *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    gc9a01_set_window(lcd, x, y, x + w - 1, y + h - 1);
    uint32_t size = (uint32_t)w * h;
    
    if(sizeof(lcd_disp_area) >= size){ // 缓存足够大
        for(uint16_t i = 0; i < size; i ++){
            lcd_disp_area[i] = color>>8;
            lcd_disp_area[i + 1] = color;
        }

        gc9a01_write_data_dma(lcd, lcd_disp_area, size);
    }else { // 缓存不够，分块发送
        #if 1
            while(sizeof(lcd_disp_area) < size){
                gc9a01_write_data(lcd, lcd_disp_area, sizeof(lcd_disp_area));
                size -= sizeof(lcd_disp_area);
            }
            gc9a01_write_data(lcd, lcd_disp_area, size);
        #else

        #endif
    }
}

/**
 * @brief 清屏函数，将全屏幕设置为一种颜色
 */
void gc9a01_clear(struct gc9a01_stu *lcd, uint16_t color){
    gc9a01_fill_color(lcd, 0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}
