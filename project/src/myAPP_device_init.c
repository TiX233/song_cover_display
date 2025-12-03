#include "myAPP_device_init.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_param.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ltx_event_group.h"
#include "GC9A01.h"
#include "myAPP_system.h"

// 所需初始化外部硬件完成事件
#define EVENT_INIT_LCD_OVER     0x0001
#define EVENT_INIT_MOTOR_OVER   0x0002

// 函数声明
// 显示屏回调
void myLCD_write_cs(uint8_t pin_level);
void myLCD_write_dc(uint8_t pin_level);
void myLCD_write_rst(uint8_t pin_level);
void myLCD_set_backlight(uint8_t level);
void myLCD_transmit_data(const uint8_t *buf, uint16_t len);
void myLCD_transmit_data_dma(const uint8_t *buf, uint16_t len);
void myLCD_before_dma_trans(void);
void myLCD_pin_init(void);
// lcd 初始化脚本回调
void script_cb_lcd_init(struct ltx_Script_stu *script);
// 外部硬件初始化完成事件组回调
void event_cb_device_init_over(struct ltx_Event_stu *event);

// 所有外部硬件初始化完成事件组
struct ltx_Event_stu event_device_init_over;


// 显示屏对象结构体
struct gc9a01_stu myLCD = {
    .flag_dma_lock = 0,

    .write_cs = myLCD_write_cs,
    .write_dc = myLCD_write_dc,
    .write_rst = myLCD_write_rst,

    .set_backlight = myLCD_set_backlight,

    .transmit_data = myLCD_transmit_data,
    .transmit_data_dma = myLCD_transmit_data_dma,
    .before_dma_trans = myLCD_before_dma_trans,

    .delay_ms = HAL_Delay,
    .pin_init = myLCD_pin_init,
};

// gc9a01 初始化脚本对象结构体
struct ltx_Script_stu script_lcd_init;


int myAPP_device_init_init(struct ltx_App_stu *app){
    // 创建显示屏初始脚本
    if(ltx_Script_init(&script_lcd_init, script_cb_lcd_init, SC_TYPE_RUN_DELAY, 10, NULL)){
        _SYS_ERROR(0xFFFF, "Lcd init script create Failed!");

        return -1;
    }

    // 创建所有外部硬件初始化完成事件组
    if(ltx_Event_init(&event_device_init_over, event_cb_device_init_over,
                    EVENT_INIT_LCD_OVER | EVENT_INIT_MOTOR_OVER,
                    10000)){ // 10s 超时时间
        _SYS_ERROR(0xFFFF, "Device event group create Failed!");

        return -3;
    }

    return 0;
}

int myAPP_device_init_pause(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);

    return 0;
}

int myAPP_device_init_resume(struct ltx_App_stu *app){

    ltx_Script_resume(&script_lcd_init);

    return 0;
}

int myAPP_device_init_destroy(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);
    ltx_Event_cancel(&event_device_init_over);

    return 0;
}

struct ltx_App_stu app_device_init = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "device_init",

    .init = myAPP_device_init_init,
    .pause = myAPP_device_init_pause,
    .resume = myAPP_device_init_resume,
    .destroy = myAPP_device_init_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


extern lcd_init_seq_stu gc9a01_init_table[];
// lcd 初始化脚本回调
void script_cb_lcd_init(struct ltx_Script_stu *script){
    switch(script->step_now){
        case 0:
            // 准备复位
            LOG_STR(PRINT_LOG"Start init LCD...\n");
            myLCD.is_initialized = 0;
            myLCD.set_backlight(0);
            myLCD.write_cs(1);
            myLCD.write_rst(1);

            // 10ms 后复位
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 10, NULL);
            break;

        case 1:
            // 复位
            myLCD.write_rst(0);

            // 50ms 后释放
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 50, NULL);

            break;

        case 2:
            // 释放复位
            myLCD.write_rst(0);

            // 100ms 后初始化
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 100, NULL);

            break;

        case 3:
            // 拉低片选
            myLCD.write_rst(0);

            // 进入初始化
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);

            break;

        default:
            // 初始化显示屏
            uint32_t i = script->step_now - 4;
            switch(gc9a01_init_table[i].ctrl){
                case LCD_CTRL_WRITE_CMD:
                    gc9a01_write_cmd(&myLCD, gc9a01_init_table[i].data_buf[0]);
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);
                    break;
                    
                case LCD_CTRL_WRITE_DATA:
                    gc9a01_write_data(&myLCD, gc9a01_init_table[i].data_buf, gc9a01_init_table[i].data_len);
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);
                    break;
                    
                case LCD_CTRL_DELAY:
                    // lcd->delay_ms(gc9a01_init_table[i].delay_ms);
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, gc9a01_init_table[i].delay_ms, NULL);
                    break;
                    
                case LCD_CTRL_OVER:
                    // 初始化完成
                    myLCD.is_initialized = 1;
                    myLCD.set_backlight(100);
                    LOG_FMT(PRINT_LOG"LCD init Okay at step: %d, at %dms\n", script->step_now, ltx_Sys_get_tick());
                    // 结束脚本
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_OVER, 0, NULL);
                    // 发布 lcd 初始化完成事件
                    ltx_Event_publish(&event_device_init_over, EVENT_INIT_LCD_OVER);

                    break;

                default:
                    LOG_FMT(PRINT_ERROR"LCD init Error at step: %d, case: %d\n", script->step_now, gc9a01_init_table[i].ctrl);
                    // 结束脚本
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_OVER, 0, NULL);
                    _SYS_ERROR(SYS_ERROR_LCD | SYS_ERROR_LCD_INIT, "LCD init Failed!");

                    break;
            }

            break;
    }

}



// 显示屏回调
void myLCD_write_cs(uint8_t pin_level){
    if(pin_level){
        GPIOA->scr = GPIO_PINS_6;
    }else {
        GPIOA->clr = GPIO_PINS_6;
    }
}

void myLCD_write_dc(uint8_t pin_level){
    if(pin_level){
        GPIOA->scr = GPIO_PINS_4;
    }else {
        GPIOA->clr = GPIO_PINS_4;
    }
}

void myLCD_write_rst(uint8_t pin_level){
    if(pin_level){
        GPIOB->scr = GPIO_PINS_0;
    }else {
        GPIOB->clr = GPIO_PINS_0;
    }
}

void myLCD_set_backlight(uint8_t level){
    level = (level > 100) ? 100 : level;
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, level * 10);
    LOG_FMT(PRINT_DEBUG"Set backlight to %d\n", level);
}

void myLCD_transmit_data(const uint8_t *buf, uint16_t len){
    HAL_StatusTypeDef spi_tx_status = HAL_SPI_Transmit(&spi1_handler, buf, len, 1000);
    if(spi_tx_status != HAL_OK){
        LOG_FMT(PRINT_ERROR"Spi tx Error: %d\n", spi1_handler.ErrorCode);
        ltx_Script_pause(&script_lcd_init);
    }
}

void myLCD_transmit_data_dma(const uint8_t *buf, uint16_t len){
    HAL_StatusTypeDef spi_tx_status = HAL_SPI_Transmit_DMA(&spi1_handler, buf, len);
    if(spi_tx_status != HAL_OK){
        LOG_FMT(PRINT_ERROR"Spi dma tx Error: %d\n", spi1_handler.ErrorCode);
    }
}

void myLCD_before_dma_trans(void){

}

void myLCD_pin_init(void){

}


// 所有外部硬件初始化完成或者初始化超时回调
void event_cb_device_init_over(struct ltx_Event_stu *event){
    if(ltx_Event_is_timeout(event)){
        LOG_STR(PRINT_ERROR"Device init Timeout!\n");
        LOG_FMT(PRINT_ERROR"events: 0x%08x\n", event->events);

        return ;
    }

    // 所有外部硬件均初始化完成
    LOG_STR(PRINT_LOG"All devices init over.\n");
    // 关闭 device_init app
    ltx_App_destroy(&app_device_init);
    // 启动业务 app
    // todo

}
