#include "ltx_param.h"
#include "ltx_app.h"
#include "myAPP_system.h"
#include "ltx_log.h"

// 心拍频率
uint16_t heart_beat_Hz = 1;
void param_read_heart_beat(struct param_stu *param){
    LOG_FMT(PRINT_LOG"Heart beat: %d Hz\n", heart_beat_Hz);
}
void param_write_heart_beat(struct param_stu *param, const char *new_val){
    uint16_t new_Hz;

    sscanf(new_val, "%hd", &new_Hz);

    if(new_Hz > 1000 || new_Hz < 1){
        LOG_FMT(PRINT_WARNNING"New value(%d) not in range(1~1000)!\n", new_Hz);
        return ;
    }

    heart_beat_Hz = new_Hz;
    
    ltx_Task_set_period(&task_heart_beat, 1000/heart_beat_Hz, 0);

    LOG_FMT(PRINT_LOG"Set heart beat to %d Hz\n", heart_beat_Hz);
}

extern float CENTER_X;
extern float CENTER_Y;

void param_read_rotate_center_x(struct param_stu *param){
    LOG_FMT(PRINT_LOG"rotate_center_x: %f\n", CENTER_X);
}
void param_write_rotate_center_x(struct param_stu *param, const char *new_val){
    uint16_t new_x;

    sscanf(new_val, "%hd", &new_x);

    CENTER_X = new_x;

    LOG_FMT(PRINT_LOG"Set rotate_center_x to %d\n", new_x);
}

void param_read_rotate_center_y(struct param_stu *param){
    LOG_FMT(PRINT_LOG"rotate_center_y: %f\n", CENTER_Y);
}
void param_write_rotate_center_y(struct param_stu *param, const char *new_val){
    uint16_t new_y;

    sscanf(new_val, "%hd", &new_y);

    CENTER_Y = new_y;

    LOG_FMT(PRINT_LOG"Set rotate_center_y to %d\n", new_y);
}

struct param_stu param_list[] = {
    { // 心拍任务频率
        .param_name = "heart_beat_Hz",
        .param_read = param_read_heart_beat,
        .param_write = param_write_heart_beat,
    },

    { // 旋转中心 x
        .param_name = "rotate_center_x",
        .param_read = param_read_rotate_center_x,
        .param_write = param_write_rotate_center_x,
    },

    { // 旋转中心 x
        .param_name = "rotate_center_y",
        .param_read = param_read_rotate_center_y,
        .param_write = param_write_rotate_center_y,
    },


    // 末尾项
    {
        .param_name = " ",
    },
};
