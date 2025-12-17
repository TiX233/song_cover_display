#include "myAPP_device_init.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_param.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ltx_event_group.h"
#include "GC9A01.h"
#include "myAPP_system.h"
#include "tonearm.h"
#include "myAPP_display.h"
#include "myAPP_usb.h"
#include "myAPP_tonearm.h"

// 所需初始化外部硬件完成事件
#define EVENT_INIT_LCD_OVER         0x0001
#define EVENT_INIT_TONEARM_OVER     0x0002

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

// 唱臂回调
uint8_t myTonearm_read_pin(void);
void myTonearm_set_duty_a(uint8_t duty);
void myTonearm_set_duty_b(uint8_t duty);
// 唱臂初始化脚本回调
void script_cb_tonearm_init(struct ltx_Script_stu *script);


// 外部硬件初始化完成事件组回调
void event_cb_device_init_over(struct ltx_Event_stu *event);



// 所有外部硬件初始化完成事件组
struct ltx_Event_stu event_device_init_over;


// 显示屏对象结构体
struct gc9a01_stu myLCD = {
    .is_initialized = 0,

    .write_cs = myLCD_write_cs,
    .write_dc = myLCD_write_dc,
    .write_rst = myLCD_write_rst,

    .set_backlight = myLCD_set_backlight,

    .transmit_data = myLCD_transmit_data,
    .transmit_data_dma = myLCD_transmit_data_dma,

    .delay_ms = HAL_Delay,
    .pin_init = myLCD_pin_init,
};

// gc9a01 初始化脚本对象结构体
struct ltx_Script_stu script_lcd_init;

// 唱臂对象结构体
struct tonearm_stu myTonearm = {
    .type = TONEARM_DT_A,
    .status = TONEARM_STATUS_LEAVE,

    .read_pin = myTonearm_read_pin,
    .set_duty_a = myTonearm_set_duty_a,
    .set_duty_b = myTonearm_set_duty_b,
};

// 唱臂初始化脚本对象结构体
struct ltx_Script_stu script_tonearm_init;


// app 相关
int myAPP_device_init_init(struct ltx_App_stu *app){
    // 创建显示屏初始脚本
    if(ltx_Script_init(&script_lcd_init, script_cb_lcd_init, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Lcd init script create Failed!");

        return -1;
    }

    // 创建唱臂初始脚本
    if(ltx_Script_init(&script_tonearm_init, script_cb_tonearm_init, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Tonearm init script create Failed!");

        return -1;
    }

    // 创建所有外部硬件初始化完成事件组
    if(ltx_Event_init(&event_device_init_over, event_cb_device_init_over,
                    EVENT_INIT_LCD_OVER | EVENT_INIT_TONEARM_OVER,
                    // EVENT_INIT_LCD_OVER,
                    10000)){ // 10s 超时时间
        _SYS_ERROR(0xFFFF, "Device event group create Failed!");

        return -3;
    }

    return 0;
}

int myAPP_device_init_pause(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);
    ltx_Script_pause(&script_tonearm_init);

    return 0;
}

int myAPP_device_init_resume(struct ltx_App_stu *app){

    ltx_Script_resume(&script_lcd_init);
    ltx_Script_resume(&script_tonearm_init);

    return 0;
}

int myAPP_device_init_destroy(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);
    ltx_Script_pause(&script_tonearm_init);

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
        case 1:
            // 准备复位
            LOG_STR(PRINT_LOG"Start init LCD...\n");
            myLCD.is_initialized = 0;
            myLCD.set_backlight(0);
            myLCD.write_cs(1);
            myLCD.write_rst(1);

            // 10ms 后复位
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 10, NULL);
            break;

        case 2:
            // 复位
            myLCD.write_rst(0);

            // 50ms 后释放
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 50, NULL);

            break;

        case 3:
            // 释放复位
            myLCD.write_rst(1);

            // 100ms 后初始化
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 100, NULL);

            break;

        case 4:
            // 拉低片选
            myLCD.write_cs(0);

            // 进入初始化
            ltx_Script_set_next_step(script, 9, SC_TYPE_RUN_DELAY, 0, NULL);

            break;

        default:
            // 初始化显示屏
            uint32_t i = script->step_now - 9;
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
                    LOG_FMT(PRINT_LOG"LCD init Okay at step: %d, at %dms\n", script->step_now, ltx_Sys_get_tick());
                    // 清空缓存
                    reset_pic_buf(0x0000);
                    // 初始化显示 app
                    ltx_App_init(&app_display);
                    // 进入开机动画刷新状态
                    ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 0, NULL);

                    break;

                default:
                    LOG_FMT(PRINT_ERROR"LCD init Error at step: %d, case: %d\n", script->step_now, gc9a01_init_table[i].ctrl);
                    // 结束脚本
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_OVER, 0, NULL);
                    _SYS_ERROR(SYS_ERROR_LCD | SYS_ERROR_LCD_INIT, "LCD init Failed!");

                    break;
            }

            break;

        case 5: // 准备刷黑屏
            // 刷黑屏上半
            gc9a01_set_window(&myLCD, 0, 0, 240-1, 119);
            gc9a01_write_data_dma(&myLCD, (uint8_t *)picture_buffer, 240*120*2);
            // 等待黑屏上半刷新完毕
            ltx_Script_set_next_step(script, 6, SC_TYPE_WAIT_TOPIC, 50, &topic_spi_tx_over);

            break;

        case 6: // 准备刷黑屏下半
            // 刷黑屏下半
            gc9a01_set_window(&myLCD, 0, 120, 240-1, 240-1);
            gc9a01_write_data_dma(&myLCD, (uint8_t *)picture_buffer, 240*120*2);
            // 等待黑屏下半刷新完毕
            ltx_Script_set_next_step(script, 7, SC_TYPE_WAIT_TOPIC, 50, &topic_spi_tx_over);

            break;

        case 7: // 画 logo
            myLCD.set_backlight(100);
            reset_pic_buf_tix(tix_color);

            // 播放下落动画
            disp_pic_down();
            ltx_Script_set_next_step(script, 8, SC_TYPE_WAIT_TOPIC, 500, &topic_pic_down_over);

            break;

        case 8: // 图片下落完成
            // 结束脚本
            ltx_Script_set_next_step(script, 0, SC_TYPE_OVER, 0, NULL);
            // 发布 lcd 初始化完成事件
            ltx_Event_publish(&event_device_init_over, EVENT_INIT_LCD_OVER);

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
        LOG_FMT(PRINT_ERROR"Spi tx Error: %d, %d\n", spi_tx_status, spi1_handler.ErrorCode);
        ltx_Script_pause(&script_lcd_init);
    }
}

void myLCD_transmit_data_dma(const uint8_t *buf, uint16_t len){
    HAL_StatusTypeDef spi_tx_status = HAL_SPI_Transmit_DMA(&spi1_handler, buf, len);
    if(spi_tx_status != HAL_OK){
        LOG_FMT(PRINT_ERROR"Spi dma tx Error: %d, %d\n", spi_tx_status, spi1_handler.ErrorCode);
        LOG_FMT(PRINT_ERROR"Dma: %d\n", dma1ch1_handler.State);
    }
}

void myLCD_pin_init(void){

}


// 唱臂初始化脚本回调
void script_cb_tonearm_init(struct ltx_Script_stu *script){
    static uint8_t try_times = 0; // 尝试次数
    static uint8_t wait_stable_count = 0; // 等待唱针位置平稳次数
    static uint8_t last_stylus_status = 0;

    static uint8_t duty_now = 0;
    static uint8_t flag_type_reversed = 0;
    static uint8_t flag_check_type_okay = 0;

    switch(script->step_now){
        case 1: // 初始化变量
            try_times = 0;
            wait_stable_count = 0;
            // 关闭 pwm 输出
            tonearm_leave(&myTonearm, 0);
            // 检测唱针位置
            tonearm_detect(&myTonearm);
            last_stylus_status = tonearm_get_status(&myTonearm);

            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 50, NULL);

            break;

        case 2: // 检测唱针位置，等待稳定
            tonearm_detect(&myTonearm);
            if(last_stylus_status != tonearm_get_status(&myTonearm)){ // 唱针位置有变化
                wait_stable_count = 0;
                try_times ++;
                last_stylus_status = tonearm_get_status(&myTonearm);
            }else { // 唱针位置没变化
                wait_stable_count ++;
            }

            if(wait_stable_count > 5){ // 连续五次采样都没位置变化，认为稳定，进入唱臂驱动类型判断
                ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);

            }else if(try_times > 10){ // 超过十次抖动
                LOG_STR(PRINT_ERROR"Tonearm init Failed!\n");
                _SYS_ERROR(SYS_ERROR_TONEARM | SYS_ERROR_TONEARM_INIT, "Stylus status not stable!");
            }else { // 50ms 后继续判断
                ltx_Script_set_next_step(script, script->step_now, SC_TYPE_RUN_DELAY, 50, NULL);
            }

            break;

        case 3: // 初始化变量
            duty_now = 0;
            try_times = 0;
            flag_type_reversed = 0;
            flag_check_type_okay = 0;

            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);

            break;
            
        case 4: // 渐渐增强驱动力
            if(duty_now == 100){ // 驱动力拉满后等待一小段时间
                try_times ++;
            }else {
                tonearm_close(&myTonearm, ++ duty_now);
            }

            if(try_times > 1){ // 驱动力已拉满，且已超过等待时间，检测唱针位置
                tonearm_detect(&myTonearm);
                if(last_stylus_status != tonearm_get_status(&myTonearm)){ // 唱针位置产生了变化
                    flag_check_type_okay = 1;
                    if(last_stylus_status == TONEARM_STATUS_LEAVE){ // 使用靠近函数，唱针由离开变为接近，正常反应，关闭输出后结束初始化
                        ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 0, NULL);
                    }else { // 使用靠近函数，唱针由接近变为离开，需要反转驱动类型
                        myTonearm.type = TONEARM_DT_A;
                        flag_type_reversed = 1;
                        // 关闭输出后结束初始化
                        ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 0, NULL);
                    }
                }else if(try_times > 15){ // 驱动力拉满等待后检测时间超过 300ms 唱针没有位置变化
                    if(flag_type_reversed == 0){ // 初次尝试，但是没有反应，尝试反转类型后再尝试一遍
                        // 先减小驱动力至关闭
                        ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 0, NULL); 
                    }else { // 反转后还是没反应，设备可能有问题
                        // 逐渐减小输出至关闭
                        flag_check_type_okay = 2;
                        ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 0, NULL);
                    }
                }else { // 驱动力已拉满，唱针位置没发生变化且未超时，20ms 后再检测一次
                    ltx_Script_set_next_step(script, 4, SC_TYPE_RUN_DELAY, 20, NULL);
                }
            }else if(try_times){ // 驱动力已拉满，但是没超过等待时间，等待一下，避免抖动
                ltx_Script_set_next_step(script, 4, SC_TYPE_RUN_DELAY, 390, NULL);
            }else { // 驱动力未拉满，每毫秒增加百分之一的推力
                ltx_Script_set_next_step(script, 4, SC_TYPE_RUN_DELAY, 1, NULL);
            }

            break;
            
        case 5: // 渐渐减小驱动力
            if(duty_now == 0){ // 驱动力减小完成
                if(flag_check_type_okay == 1){ // 结束初始化
                    LOG_FMT(PRINT_LOG"Tonearm init okay at %dms\n", ltx_Sys_get_tick());
                    // 结束脚本
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_OVER, 0, NULL);
                    // 发布唱臂初始化完成事件
                    ltx_Event_publish(&event_device_init_over, EVENT_INIT_TONEARM_OVER);
                }else if(flag_check_type_okay == 2){
                    LOG_STR(PRINT_ERROR"Tonearm init Failed!\n");
                    _SYS_ERROR(SYS_ERROR_TONEARM | SYS_ERROR_TONEARM_INIT, "Can not drive tonearm!");
                    // 结束脚本
                    ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_OVER, 0, NULL);
                }else { // 返回
                    try_times = 0;
                    flag_type_reversed = 1;
                    myTonearm.type = TONEARM_DT_B;
                    ltx_Script_set_next_step(script, 4, SC_TYPE_RUN_DELAY, 1, NULL);
                }
            }else { // 正在减小驱动力
                if(flag_type_reversed){
                    tonearm_leave(&myTonearm, -- duty_now);
                }else {
                    tonearm_close(&myTonearm, -- duty_now);
                }
                ltx_Script_set_next_step(script, 5, SC_TYPE_RUN_DELAY, 1, NULL);
            }

            break;

        default:

            break;
    }
}

// 唱臂回调
uint8_t myTonearm_read_pin(void){
    return gpio_input_data_bit_read(GPIOA, GPIO_PINS_1);
}

void myTonearm_set_duty_a(uint8_t duty){
    tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_3, duty*10);
}

void myTonearm_set_duty_b(uint8_t duty){
    tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_4, duty*10);
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
    // 启动显示 app
    // ltx_App_init(&app_display);
    ltx_App_resume(&app_display);
    // 启动 usb app
    ltx_App_init(&app_usb);
    ltx_App_resume(&app_usb);
    // 启动唱臂管理 app
    // ltx_App_init(&app_tonearm);
    // ltx_App_resume(&app_tonearm);
}
