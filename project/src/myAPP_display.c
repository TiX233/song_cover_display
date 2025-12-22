#include "myAPP_display.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "GC9A01.h"
#include "myAPP_device_init.h"
#include "myAPP_system.h"
#include "math.h"
// #include "ltx_lock.h"
#include "tonearm.h"
#include "myAPP_usb.h"


void script_cb_pic_rotate(struct ltx_Script_stu *script);
void script_cb_pic_rotate_sub(struct ltx_Script_stu *script);

void script_cb_pic_down(struct ltx_Script_stu *script);
void script_cb_pic_down_sub(struct ltx_Script_stu *script);

void script_cb_script_player(struct ltx_Script_stu *script);

// 锁超时回调
// void lock_cb_spi(struct ltx_Lock_stu *lock);

// 图片缓存
uint16_t picture_buffer[240*240] = {0};
// 显示缓存
uint16_t pfb_0[240*24];
uint16_t pfb_1[240*24];
uint16_t *pfb_s[] = {pfb_0, pfb_1};

// spi 锁
// struct ltx_Lock_stu lock_spi = {.lock = 1};

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

// 唱片机播放话题
struct ltx_Topic_stu topic_player_resume = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

// 唱片机暂停话题
struct ltx_Topic_stu topic_player_pause = {
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

// 唱片机总状态机脚本
struct ltx_Script_stu script_player;


void reset_pic_buf(uint16_t color){
    color = (color >> 8) | (color << 8);
    for(uint16_t y = 0; y < 240; y ++){
        for(uint16_t x = 0; x < 240; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }
}


void reset_pic_buf_ms(void){
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
}

uint16_t tix_color = 0xEE96;

uint16_t T_start_x = 22;
uint16_t T_start_y = 70;
uint16_t T_heng_w  = 65;
uint16_t T_heng_h  = 18;

uint16_t T_shu_w   = 20;
uint16_t T_shu_h   = 82;

uint16_t I_start_x = 110;
uint16_t I_start_y = 70;
uint16_t I_w       = 20;
uint16_t I_shu_sy  = 100;
uint16_t I_shu_h   = 70;

uint16_t X_start_x = 153;
uint16_t X_start_y = 70;
uint16_t X_w       = 65;
uint16_t X_h       = 100;
uint16_t X_line_w  = 18;

void reset_pic_buf_tix(uint16_t color){
    color = (color >> 8) | (color << 8);
    // T
    // 横
    for(uint16_t y = T_start_y; y < T_start_y + T_heng_h; y ++){
        for(uint16_t x = T_start_x; x < T_start_x+T_heng_w; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }
    // 竖
    for(uint16_t y = T_start_y + T_heng_h; y < (T_start_y + T_heng_h + T_shu_h); y ++){
        for(uint16_t x = T_start_x+(T_heng_w-T_shu_w)/2; x < (T_start_x+(T_heng_w-T_shu_w)/2 + T_shu_w); x ++){
            picture_buffer[y*240 + x] = color;
        }
    }

    // i
    // 点
    for(uint16_t y = I_start_y; y < I_start_y + I_w; y ++){
        for(uint16_t x = I_start_x; x < I_start_x + I_w; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }
    // 竖
    for(uint16_t y = I_shu_sy; y < I_shu_sy + I_shu_h; y ++){
        for(uint16_t x = I_start_x; x < I_start_x + I_w; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }

    // X
    // 捺
    uint16_t offset_x = 0;
    for(uint16_t y = X_start_y; y < X_h + X_start_y; y ++){
        offset_x = (y-X_start_y)*(X_w - X_line_w)/X_h;
        for(uint16_t x = X_start_x + offset_x; x < X_start_x + X_line_w + offset_x; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }

    //撇
    offset_x = 0;
    for(uint16_t y = X_start_y; y < X_h + X_start_y; y ++){
        offset_x = (y-X_start_y)*(X_w - X_line_w)/X_h;
        for(uint16_t x = X_start_x + X_w - X_line_w - offset_x; x < X_start_x + X_w - offset_x; x ++){
            picture_buffer[y*240 + x] = color;
        }
    }
}


// APP 相关
int myAPP_display_init(struct ltx_App_stu *app){
    // reset_pic_buf_ms();
    // reset_pic_buf(0x0000);
    // reset_pic_buf_tix(tix_color);
    /*
    for(uint32_t x = 0; x < 240*240; x ++){
        picture_buffer[x] = 0x00F8;
    }*/
    // 创建封面旋转脚本
    if(ltx_Script_init(&script_pic_rotate, script_cb_pic_rotate, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Pic rotate script create Failed!");

        return -1;
    }
    // 创建封面下落脚本
    if(ltx_Script_init(&script_pic_down, script_cb_pic_down, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Pic down script create Failed!");

        return -1;
    }

    // 创建唱片机总状态机脚本
    if(ltx_Script_init(&script_player, script_cb_script_player, SC_TYPE_RUN_DELAY, 0, NULL)){
        _SYS_ERROR(0xFFFF, "Pic down script create Failed!");

        return -1;
    }

    // 注册话题
    ltx_Topic_add(&topic_spi_tx_over);
    ltx_Topic_add(&topic_draw_frame_over);
    ltx_Topic_add(&topic_pic_down_over);
    ltx_Topic_add(&topic_player_resume);
    ltx_Topic_add(&topic_player_pause);

    // 初始化 spi 锁
    // ltx_Lock_init(&lock_spi, lock_cb_spi);
    
    return 0;
}

int myAPP_display_pause(struct ltx_App_stu *app){

    ltx_Script_pause(&script_player);

    return 0;
}

int myAPP_display_resume(struct ltx_App_stu *app){

    ltx_Script_resume(&script_player);

    return 0;
}

int myAPP_display_destroy(struct ltx_App_stu *app){

    ltx_Script_pause(&script_player);

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


#define PI       3.141592653589793f
float CENTER_X = 119.5f;
float CENTER_Y = 119.5f;

// 舍入因数，七舍八入。0.5 为四舍五入，但是边缘会看到白点。以及改成宏定义后转速变快了，神奇优化
#define ROUNDING_FACTOR 0.2f


uint8_t is_point_in_display_circle(int x, int y){
    // 计算(x-120)² + (y-120)²
    int dx = x - 120;
    int dy = y - 120;
    int dist_sq = dx * dx + dy * dy;
    
    // 检查是否小于等于14400
    return (dist_sq <= 14400) ? 1 : 0;
}

static inline float deg2rad(float degree){
    return degree * PI / 180.0f;
}


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

uint16_t disp_angle = 0;
static uint16_t disp_block = 0;
// 旋转图片单帧刷新子脚本
void script_cb_pic_rotate_sub(struct ltx_Script_stu *script){
    uint16_t x, y;
    static float angle;
    static float cos_angle;
    static float sin_angle;
    
    float dx;
    float dy;
    float src_x;
    float src_y;
    int src_x_int;
    int src_y_int;

    float sccs;

    // static uint8_t flag_1 = 1;
    // static TickType_t time_cost = 0;

    switch(script->step_now){
        case 1:
            disp_block = 0;
            angle = deg2rad(disp_angle);
            cos_angle = cosf(angle);
            sin_angle = sinf(angle);

            if(disp_angle == 0){ // 0 度，直接将原图数据转到 pfb
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        pfb_0[y*240 + x] = picture_buffer[y*240 + x];
                    }
                }
            }else { // 换算旋转后的数据转到 pfb
                // 角度转弧度
                // if(flag_1){
                //     time_cost = ltx_Sys_get_tick();
                // }
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        // if(is_point_in_display_circle(x, y)){ // 在圆内
                        // 坐标换算
                        dx = x - CENTER_X;
                        dy = y - CENTER_Y;
                        src_x = dx * cos_angle + dy * sin_angle + CENTER_X;
                        src_y = -dx * sin_angle + dy * cos_angle + CENTER_Y;
                        // 最近邻插值
                        src_x_int = (int)(src_x + ROUNDING_FACTOR);  // 四舍五入
                        src_y_int = (int)(src_y + ROUNDING_FACTOR);  // 四舍五入

                        if (src_x_int >= 0 && src_x_int < 240 && 
                            src_y_int >= 0 && src_y_int < 240) { // 换算后的点在范围内
                            pfb_0[y*240 + x] = picture_buffer[src_y_int*240 + src_x_int];
                        }
                    }
                }
                // if(flag_1){
                //     flag_1 = 0;
                //     time_cost = ltx_Sys_get_tick() - time_cost;
                //     LOG_FMT("ddd: %d\n", time_cost);
                // }
            }
            ltx_Script_set_next_step(script, script->step_now + 1, SC_TYPE_RUN_DELAY, 0, NULL);

            break;

        case 2:
            HAL_SPI_DMAStop(&spi1_handler);
            gc9a01_set_window(&myLCD, 0, 24*disp_block, 240-1, (24)*(disp_block+1)-1);
            gc9a01_write_data_dma(&myLCD, (uint8_t *)pfb_s[disp_block%2], 240*24*2);

            disp_block ++;
            if(disp_block == 10){ // 已经画完最后一个 block，只等刷完最后一个 block
                ltx_Script_set_next_step(script, 3, SC_TYPE_WAIT_TOPIC, 30, &topic_spi_tx_over);

                return ;
            }
            ltx_Script_set_next_step(script, 2, SC_TYPE_WAIT_TOPIC, 30, &topic_spi_tx_over);
            // 计算并写缓存
            if(disp_angle == 0){ // 0 度，直接将原图数据转到 pfb
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        pfb_s[disp_block%2][y*240 + x] = picture_buffer[(y+disp_block*24)*240 + x];
                    }
                }
            }else { // 换算旋转后的数据转到 pfb
                for(y = 0; y < 24; y ++){
                    for(x = 0; x < 240; x ++){
                        // if(is_point_in_display_circle(x, y)){ // 在圆内
                        // 坐标换算
                        dx = x - CENTER_X;
                        dy = disp_block*24 + y - CENTER_Y;
                        src_x = dx * cos_angle + dy * sin_angle + CENTER_X;
                        src_y = -dx * sin_angle + dy * cos_angle + CENTER_Y;
                        // 最近邻插值
                        src_x_int = (int)(src_x + ROUNDING_FACTOR);  // 四舍五入
                        src_y_int = (int)(src_y + ROUNDING_FACTOR);  // 四舍五入

                        if (src_x_int >= 0 && src_x_int < 240 && 
                            src_y_int >= 0 && src_y_int < 240) { // 换算后的点在范围内
                            pfb_s[disp_block%2][y*240 + x] = picture_buffer[src_y_int*240 + src_x_int];
                        }
                    }
                }
            }

            break;

        case 3: // 最后一个 block dma 发送完毕
            ltx_Script_set_next_step(script, 3, SC_TYPE_OVER, 0, NULL);
            ltx_Topic_publish(&topic_draw_frame_over);

            break;

        default:

            break;
    }
}

// 旋转图片脚本
void script_cb_pic_rotate(struct ltx_Script_stu *script){

    // static TickType_t time_cost = 0;

    switch(script->step_now){
        case 1: // 初始化变量
            // disp_angle = 0;

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
            // time_cost = ltx_Sys_get_tick() - time_cost;
            // LOG_FMT("tc:%d\n", time_cost);
            // time_cost = ltx_Sys_get_tick();

            break;

        default:

            break;
    }
}


static uint16_t target_y = 0;
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

    HAL_SPI_DMAStop(&spi1_handler);

    ltx_Script_pause(&script_pic_rotate);
    ltx_Script_pause(&script_pic_down);
    ltx_Script_init(&script_pic_down, script_cb_pic_down, SC_TYPE_RUN_DELAY, 0, NULL);
    ltx_Script_resume(&script_pic_down);
}

void disp_pic_rotate(uint8_t on_off){

    ltx_Script_pause(&script_pic_rotate_sub);
    ltx_Script_pause(&script_pic_rotate);
    if(on_off){
        HAL_SPI_DMAStop(&spi1_handler);
        ltx_Script_pause(&script_pic_down);
        ltx_Script_init(&script_pic_rotate, script_cb_pic_rotate, SC_TYPE_RUN_DELAY, 0, NULL);
        ltx_Script_resume(&script_pic_rotate);
    }
}

// spi dma 发送完成回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
    // ltx_Lock_off(&lock_spi);
    ltx_Topic_publish(&topic_spi_tx_over);
}

// 锁超时回调
// void lock_cb_spi(struct ltx_Lock_stu *lock){
//     HAL_SPI_DMAStop(&spi1_handler);
//     ltx_Lock_off(lock);
// }

#define TONEARM_MAX_NOT_MOVE_COUNT          50
extern struct tonearm_stu myTonearm;
// 唱片机总状态机，也许更应该算唱臂状态机
void script_cb_script_player(struct ltx_Script_stu *script){
    static uint8_t tonearm_not_move_counter = 0;
    static uint8_t _tm_duty = 0;

    switch(script->step_now){
        case 1: // 刚开机
            // 播放下落动画
            // disp_pic_down();

            tonearm_leave(&myTonearm, 0);

            // 下落图片动画可能会被上位机用 usb 关掉，所以放在 usb app 初始化前
            // 在下落动画播放完成后进入暂停预备态
            // ltx_Script_set_next_step(script, 2, SC_TYPE_WAIT_TOPIC, 1000, &topic_pic_down_over);

            // 直接进暂停预备态
            ltx_Script_set_next_step(script, 4, SC_TYPE_WAIT_TOPIC, 25, &topic_player_resume);

            break;

        case 2:

            break;

        case 3: 

            break;

        case 4: // 暂停预备态，等待 usb 或唱臂离开完成
            if(ltx_Script_get_triger_type(script) == SC_TT_TOPIC){ // 被话题触发，usb 要求播放

                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_CLOSE){ // 已靠近，直接进播放态
                    ltx_Script_set_next_step(script, 8, SC_TYPE_WAIT_TOPIC, 200, &topic_player_pause);

                    _tm_duty = 0;
                    tonearm_leave(&myTonearm, _tm_duty);


                }else { // 否则进播放预备态
                    ltx_Script_set_next_step(script, 7, SC_TYPE_WAIT_TOPIC, 25, &topic_player_pause);

                    _tm_duty >>= 1; // duty 砍半
                    tonearm_leave(&myTonearm, _tm_duty);
                    _tm_duty = 0;

                }
                // 清零唱臂停滞计数
                tonearm_not_move_counter = 0;

                return ;

            }else { // 等待超时而触发，判断唱臂是否完成离开
                tonearm_detect(&myTonearm);
                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_LEAVE){ // 唱臂已离开，进入暂停态
                    _tm_duty = 0;
                    tonearm_leave(&myTonearm, _tm_duty);
                    tonearm_not_move_counter = 0;

                    ltx_Script_set_next_step(script, 5, SC_TYPE_WAIT_TOPIC, 123, &topic_player_resume);
                    return ;
                }else {
                    if(++tonearm_not_move_counter > TONEARM_MAX_NOT_MOVE_COUNT){ // 超过次数还是没有离开，报告上位机
                        send_str_2_usb("/el\n", 4);

                        // 5s 后继续尝试
                        _tm_duty = 0;
                        tonearm_leave(&myTonearm, _tm_duty);
                        tonearm_not_move_counter = 0;
                        ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 5000, &topic_player_resume);
                        return ;
                    }else {
                        _tm_duty += 20;
                        if(_tm_duty >= 100){
                            _tm_duty = 100;
                            ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 330, &topic_player_resume); // 等待一段时间，避免抖动
                        }else {
                            ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 20, &topic_player_resume);
                        }
                        tonearm_leave(&myTonearm, _tm_duty);
                    }
                }
            }

            break;

        case 5: // 暂停态，等待 usb 或拨动唱臂
            if(ltx_Script_get_triger_type(script) == SC_TT_TOPIC){ // 被话题触发，usb 要求播放
                // 进入播放预备态
                ltx_Script_set_next_step(script, 7, SC_TYPE_WAIT_TOPIC, 25, &topic_player_pause);

                return ;

            }else { // 等待超时而触发，判断是否唱臂变化

                ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 123, &topic_player_resume);

                tonearm_detect(&myTonearm);
                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_CLOSE){ // 唱臂靠近，请求上位机播放
                    send_str_2_usb("/q1\n", 4);
                }
            }

            break;

        case 6:

            break;

        case 7: // 播放预备态，等待 usb 或唱臂靠近完成
            if(ltx_Script_get_triger_type(script) == SC_TT_TOPIC){ // 被话题触发，usb 要求暂停

                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_LEAVE){ // 已离开，直接进暂停态
                    ltx_Script_set_next_step(script, 5, SC_TYPE_WAIT_TOPIC, 200, &topic_player_resume);

                    _tm_duty = 0;
                    tonearm_close(&myTonearm, _tm_duty);


                }else { // 否则进暂停预备态
                    ltx_Script_set_next_step(script, 4, SC_TYPE_WAIT_TOPIC, 25, &topic_player_resume);

                    _tm_duty >>= 1; // duty 砍半
                    tonearm_close(&myTonearm, _tm_duty);
                    _tm_duty = 0;

                }
                // 清零唱臂停滞计数
                tonearm_not_move_counter = 0;

                return ;

            }else { // 等待超时而触发，判断唱臂是否完成靠近
                tonearm_detect(&myTonearm);
                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_CLOSE){ // 唱臂已靠近，进入播放态
                    _tm_duty = 0;
                    tonearm_close(&myTonearm, _tm_duty);
                    tonearm_not_move_counter = 0;
                    ltx_Script_set_next_step(script, 8, SC_TYPE_WAIT_TOPIC, 200, &topic_player_pause);
                    return ;

                }else {
                    if(++tonearm_not_move_counter > TONEARM_MAX_NOT_MOVE_COUNT){ // 超过次数还是没有靠近，报告上位机
                        send_str_2_usb("/ec\n", 4);

                        // 5s 后继续尝试
                        _tm_duty = 0;
                        tonearm_close(&myTonearm, _tm_duty);
                        tonearm_not_move_counter = 0;
                        ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 5000, &topic_player_pause);
                        return ;

                    }else {
                        _tm_duty += 10;
                        if(_tm_duty >= 100){
                            _tm_duty = 100;
                            ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 330, &topic_player_pause); // 等待一段时间，避免抖动
                        }else {
                            ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 20, &topic_player_pause);
                        }
                        tonearm_close(&myTonearm, _tm_duty);
                    }
                }
            }

            break;

        case 8: // 播放态，等待 usb 或拨动唱臂
            if(ltx_Script_get_triger_type(script) == SC_TT_TOPIC){ // 被话题触发，usb 要求暂停
                // 进入暂停预备态
                ltx_Script_set_next_step(script, 4, SC_TYPE_WAIT_TOPIC, 25, &topic_player_resume);

                return ;

            }else { // 等待超时而触发，判断是否唱臂变化

                ltx_Script_set_next_step(script, script->step_now, SC_TYPE_WAIT_TOPIC, 123, &topic_player_pause);

                tonearm_detect(&myTonearm);
                if(tonearm_get_status(&myTonearm) == TONEARM_STATUS_LEAVE){ // 唱臂离开，请求上位机暂停
                    send_str_2_usb("/q0\n", 4);
                }
            }

            break;

        case 9: // 

            break;

        default:
            break;
    }
}
