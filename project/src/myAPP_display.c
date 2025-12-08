#include "myAPP_display.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "GC9A01.h"
#include "myAPP_device_init.h"
#include "myAPP_system.h"
#include "math.h"

void script_cb_pic_rotate(struct ltx_Script_stu *script);
void script_cb_pic_rotate_sub(struct ltx_Script_stu *script);

void script_cb_pic_down(struct ltx_Script_stu *script);
void script_cb_pic_down_sub(struct ltx_Script_stu *script);

// 图片缓存
uint16_t picture_buffer[240*240] = {0};
// 显示缓存
uint16_t pfb_0[240*24];
uint16_t pfb_1[240*24];
uint16_t *pfb_s[] = {pfb_0, pfb_1};

// dma 发送完成话题
struct ltx_Topic_stu topic_spi_tx_over = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

// 单帧发送完成话题
struct ltx_Topic_stu topic_draw_frame_over = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

// 图片下落完成话题
struct ltx_Topic_stu topic_pic_down_over = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

// 旋转图片脚本
struct ltx_Script_stu script_pic_rotate;
struct ltx_Script_stu script_pic_rotate_sub;
// 下落图片脚本
struct ltx_Script_stu script_pic_down;
struct ltx_Script_stu script_pic_down_sub;

int myAPP_display_init(struct ltx_App_stu *app){
    
    uint16_t i, j;
    
    for(i = 0; i < 240/2; i ++){
        for(j = 0; j < 240/2; j ++)
        picture_buffer[i*240 + j] = (RGB565_RED>>8) | (RGB565_RED<<8);
    }
    for(i = 0; i < 240/2; i ++){
        for(j = 240/2; j < 240; j ++)
        picture_buffer[i*240 + j] = (RGB565_GREEN>>8) | (RGB565_GREEN<<8);
    }
    for(i = 240/2; i < 240; i ++){
        for(j = 0; j < 240/2; j ++)
        picture_buffer[i*240 + j] = (RGB565_BLUE>>8) | (RGB565_BLUE<<8);
    }
    for(i = 240/2; i < 240; i ++){
        for(j = 240/2; j < 240; j ++)
        picture_buffer[i*240 + j] = (RGB565_YELLOW>>8) | (RGB565_YELLOW<<8);
    }
    /*
    for(uint32_t x = 0; x < 240*240; x ++){
        picture_buffer[x] = 0x00F8;
    }*/
    // 创建封面旋转脚本
    // if(ltx_Script_init(&script_pic_rotate, script_cb_pic_rotate, SC_TYPE_RUN_DELAY, 0, NULL)){
    //     _SYS_ERROR(0xFFFF, "Pic rotate script create Failed!");

    //     return -1;
    // }
    // 创建封面下落脚本
    if(ltx_Script_init(&script_pic_down, script_cb_pic_down, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Pic down script create Failed!");

        return -1;
    }

    // 注册话题
    ltx_Topic_add(&topic_spi_tx_over);
    ltx_Topic_add(&topic_draw_frame_over);
    ltx_Topic_add(&topic_pic_down_over);
    
    return 0;
}

int myAPP_display_pause(struct ltx_App_stu *app){

    // ltx_Script_pause(&script_pic_down);
    // ltx_Script_pause(&script_pic_rotate);

    return 0;
}

int myAPP_display_resume(struct ltx_App_stu *app){

    // ltx_Script_resume(&script_pic_down);
    // ltx_Script_resume(&script_pic_rotate);

    return 0;
}

int myAPP_display_destroy(struct ltx_App_stu *app){

    // ltx_Script_pause(&script_pic_down);
    // ltx_Script_pause(&script_pic_rotate);

    // free...

    return 0;
}

struct ltx_App_stu app_display = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "display",

    .init = myAPP_display_init,
    .pause = myAPP_display_pause,
    .resume = myAPP_display_resume,
    .destroy = myAPP_display_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


#define PI 3.141592653589793

uint8_t is_point_in_display_circle(int x, int y) {
    // 计算(x-120)² + (y-120)²
    int dx = x - 120;
    int dy = y - 120;
    int dist_sq = dx * dx + dy * dy;
    
    // 检查是否小于等于14400
    return (dist_sq <= 14400) ? 1 : 0;
}

static inline double deg2rad(double degree) {
    return degree * PI / 180.0;
}

static uint16_t disp_angle = 0;
static uint16_t disp_block = 0;

/*
双缓存刷新
       |                           第一帧                                                    |         空闲           |      第二帧      | ......
时间轴：-------------------------------------------------------------------------------------------------------------------------------------------->
子脚本：| 写 buf1  | 写 buf2  |   空闲  |  写 buf1 |   空闲   | 写 buf2  |   空闲    | ....... |                       | ......
       | 占有 cpu | 占有 cpu | 出让 cpu | 占有 cpu | 出让 cpu | 占有 cpu | 出让 cpu  | ....... |                       | ......
       |          |    dma 刷 buf1     |    dma 刷 buf2      |    dma 刷 buf1       | ....... |                       | ......
       |                               |                     |                                                       |
       |                               /\                    /\                                                      |
       |                       ( dma 发送完成中断 )   ( dma 发送完成中断 )                                             |
       |\                                                                                                            /\
主脚本：|创建第一帧刷新子脚本|                                                                                   | 创建第二帧刷新子脚本 |
*/

// 旋转图片单帧刷新子脚本
void script_cb_pic_rotate_sub(struct ltx_Script_stu *script){
    uint16_t x, y;
    double angle;
    double cos_angle;
    double sin_angle;
    
    double dx;
    double dy;
    double src_x;
    double src_y;
    int src_x_int;
    int src_y_int;

    switch(script->step_now){
        case 1: // 初始化变量
            disp_block = 0;
            angle = deg2rad(disp_angle);
            cos_angle = cos(angle);
            sin_angle = sin(angle);

            if(disp_angle == 0){ // 0 度，直接将原图数据转到 pfb
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        pfb_0[y*240 + x] = picture_buffer[y*240 + x];
                    }
                }
            }else { // 换算旋转后的数据转到 pfb
                // 角度转弧度
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        // if(is_point_in_display_circle(x, y)){ // 在圆内
                        // 坐标换算
                        dx = x - 195.5;
                        dy = y - 195.5;
                        src_x = dx * cos_angle + dy * sin_angle + 195.5;
                        src_y = -dx * sin_angle + dy * cos_angle + 195.5;
                        // 最近邻插值
                        src_x_int = (int)(src_x + 0.5);  // 四舍五入
                        src_y_int = (int)(src_y + 0.5);  // 四舍五入

                        if (src_x_int >= 0 && src_x_int < 240 && 
                            src_y_int >= 0 && src_y_int < 240) { // 换算后的点在范围内
                            pfb_1[y*240 + x] = picture_buffer[src_y_int*240 + src_x_int];
                        }
                    }
                }
            }
            // 发送数据
            gc9a01_set_window(&myLCD, 0, 0, 240-1, 24-1);
            gc9a01_write_data_dma(&myLCD, (uint8_t *)pfb_0, 240*24*2);
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_WAIT_TOPIC, 10, &topic_spi_tx_over);

            disp_block = 1;

            if(disp_angle == 0){ // 0 度，直接将原图数据转到 pfb
                for(y = disp_block*24; y < (disp_block + 1)*24; y ++){
                    for(x = 0; x < 240; x ++){
                        pfb_s[disp_block%2][y*240 + x] = picture_buffer[y*240 + x];
                    }
                }
            }else { // 换算旋转后的数据转到 pfb
                for(y = disp_block*24; y < (disp_block + 1)*24; y ++){
                    for(x = 0; x < 240; x ++){
                        // if(is_point_in_display_circle(x, y)){ // 在圆内
                        // 坐标换算
                        dx = x - 195.5;
                        dy = y - 195.5;
                        src_x = dx * cos_angle + dy * sin_angle + 195.5;
                        src_y = -dx * sin_angle + dy * cos_angle + 195.5;
                        // 最近邻插值
                        src_x_int = (int)(src_x + 0.5);  // 四舍五入
                        src_y_int = (int)(src_y + 0.5);  // 四舍五入

                        if (src_x_int >= 0 && src_x_int < 240 && 
                            src_y_int >= 0 && src_y_int < 240) { // 换算后的点在范围内
                            pfb_1[y*240 + x] = picture_buffer[src_y_int*240 + src_x_int];
                        }
                    }
                }
            }

            break;

        case 2: // 等待 buffer 传输完成后写 buffer
            if(disp_block == 10){ // 单帧传输完毕
                ltx_Script_set_next_step(script, 3, SC_TYPE_OVER, 0, &topic_spi_tx_over);
                ltx_Topic_publish(&topic_draw_frame_over);

                return ;
            }
            gc9a01_set_window(&myLCD, 0, 24*disp_block, 240-1, (24-1)*(disp_block+1));
            gc9a01_write_data_dma(&myLCD, (uint8_t *)pfb_s[disp_block%2], 240*24*2);

            ltx_Script_set_next_step(script, 2, SC_TYPE_WAIT_TOPIC, 10, &topic_spi_tx_over);

            disp_block ++;

            if(disp_block == 10){
                return ;
            }

            if(disp_angle == 0){ // 0 度，直接将原图数据转到 pfb
                for(y = disp_block*24; y < (disp_block + 1)*24; y ++){
                    for(x = 0; x < 240; x ++){
                        pfb_s[disp_block%2][y*240 + x] = picture_buffer[y*240 + x];
                    }
                }
            }else { // 换算旋转后的数据转到 pfb
                for(y = disp_block*24; y < (disp_block + 1)*24; y ++){
                    for(x = 0; x < 240; x ++){
                        // if(is_point_in_display_circle(x, y)){ // 在圆内
                        // 坐标换算
                        dx = x - 195.5;
                        dy = y - 195.5;
                        src_x = dx * cos_angle + dy * sin_angle + 195.5;
                        src_y = -dx * sin_angle + dy * cos_angle + 195.5;
                        // 最近邻插值
                        src_x_int = (int)(src_x + 0.5);  // 四舍五入
                        src_y_int = (int)(src_y + 0.5);  // 四舍五入

                        if (src_x_int >= 0 && src_x_int < 240 && 
                            src_y_int >= 0 && src_y_int < 240) { // 换算后的点在范围内
                            pfb_1[y*240 + x] = picture_buffer[src_y_int*240 + src_x_int];
                        }
                    }
                }
            }

            break;

        case 3:

            break;

        default:

            break;
    }
}

// 旋转图片脚本
void script_cb_pic_rotate(struct ltx_Script_stu *script){

    switch(script->step_now){
        case 1: // 初始化变量
            disp_angle = 0;

            // 创建封面旋转子脚本
            ltx_Script_init(&script_pic_rotate_sub, script_cb_pic_rotate_sub, SC_TYPE_RUN_DELAY, 0, NULL);
            ltx_Script_resume(&script_pic_rotate_sub);

            // 等待单帧画完，超时时间 50ms
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_WAIT_TOPIC, 50, &topic_draw_frame_over);
            break;
            
        case 2:
            if(disp_angle >= 360){
                disp_angle = 0;
            }
            // 判断是由于单帧画完还是超时导致的回调
            if(ltx_Script_get_triger_type(script) == SC_TT_TOPIC){ // 单帧画完
                disp_angle ++;
                ltx_Script_init(&script_pic_rotate_sub, script_cb_pic_rotate_sub, SC_TYPE_RUN_DELAY, 0, NULL);
            ltx_Script_resume(&script_pic_rotate_sub);
            }else { // 超时
                ltx_Script_pause(&script_pic_rotate_sub);
                LOG_STR(PRINT_WARNNING"Draw frame timeout!\n");
                disp_angle += 2;
                ltx_Script_init(&script_pic_rotate_sub, script_cb_pic_rotate_sub, SC_TYPE_RUN_DELAY, 0, NULL);
                ltx_Script_resume(&script_pic_rotate_sub);
            }
            ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 50, &topic_draw_frame_over);

            break;

        default:

            break;
    }
}


static uint16_t target_y = 0;
uint8_t i = 0;


// 下落图片单帧刷新子脚本
void script_cb_pic_down_sub(struct ltx_Script_stu *script){
    uint16_t x, y;
    // static TickType_t counter = 0;

    switch(script->step_now){
        case 1:
            // LOG_FMT(PRINT_DEBUG"i: %d\n", i++);
            if(target_y > 119){ // 快超出 dma 单次发送长度，分两次刷新
                // LOG_STR(PRINT_DEBUG"a\n");
                // 先刷上半
                gc9a01_set_window(&myLCD, 0, 0, 240-1, 119);
                gc9a01_write_data_dma(&myLCD, (uint8_t *)(&picture_buffer[240*(239-target_y)]), 240*120*2);

                // 等待上半刷完刷下半
                ltx_Script_set_next_step(script, 3, SC_TYPE_WAIT_TOPIC, 30, &topic_spi_tx_over);
            }else { // 一次刷新即可
                // LOG_STR(PRINT_DEBUG"b\n");
                gc9a01_set_window(&myLCD, 0, 0, 240-1, target_y);
                gc9a01_write_data_dma(&myLCD, (uint8_t *)(&picture_buffer[240*(239-target_y)]), 240*(target_y+1)*2);
                ltx_Script_set_next_step(script, 2, SC_TYPE_WAIT_TOPIC, 30, &topic_spi_tx_over);
            }
            // counter = ltx_Sys_get_tick();

            break;

        case 2: // buffer 传输完成
            // counter = ltx_Sys_get_tick() - counter;
            // LOG_FMT("\tt:%d\n", counter);
            ltx_Topic_publish(&topic_draw_frame_over);

            break;

        case 3: // 刷新下半
            gc9a01_set_window(&myLCD, 0, 120, 240-1, target_y);
            gc9a01_write_data_dma(&myLCD, (uint8_t *)(&picture_buffer[240*(239-target_y+120)]), 240*(target_y+1-120)*2);
            ltx_Script_set_next_step(script, 2, SC_TYPE_WAIT_TOPIC, 30, &topic_spi_tx_over);

            break;

        default:

            break;
    }
}

// 下落图片脚本
void script_cb_pic_down(struct ltx_Script_stu *script){

    switch(script->step_now){
        case 1: // 初始化变量
            target_y = 9;

            // 创建下落图片子脚本
            if(ltx_Script_init(&script_pic_down_sub, script_cb_pic_down_sub, SC_TYPE_RUN_DELAY, 0, NULL)){
                _SYS_ERROR(0xFFFF, "Pic down sub script create Failed!");

                return ;
            }
            ltx_Script_resume(&script_pic_down_sub);

            // ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 30, NULL);
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_WAIT_TOPIC, 30, &topic_draw_frame_over);
            break;
            
        case 2: // 图片下落
            target_y += 10;
            // LOG_FMT("Y:%d\n", target_y);
            ltx_Script_pause(&script_pic_down_sub);
            ltx_Script_init(&script_pic_down_sub, script_cb_pic_down_sub, SC_TYPE_RUN_DELAY, 0, NULL);
            ltx_Script_resume(&script_pic_down_sub);

            if(target_y >= 239){ // 画最后一帧
                target_y = 239;
                ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_WAIT_TOPIC, 30, &topic_draw_frame_over);
            }else {
                // ltx_Script_set_next_step(script, script->step_now, SC_TYPE_RUN_DELAY, 30, NULL);
                ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 30, &topic_draw_frame_over);
            }

            break;

        case 3: // 画完最后一帧
            ltx_Topic_publish(&topic_pic_down_over);
            break;

        default:

            break;
    }
}

void disp_pic_down(void){

    ltx_Script_pause(&script_pic_down);
    ltx_Script_init(&script_pic_down, script_cb_pic_down, SC_TYPE_RUN_DELAY, 0, NULL);
    ltx_Script_resume(&script_pic_down);
}

// spi dma 发送完成回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
    // LOG_STR("\td\n");
    ltx_Topic_publish(&topic_spi_tx_over);
}
