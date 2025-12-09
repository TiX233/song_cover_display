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

extern uint16_t tix_color;

extern uint16_t T_start_x;
extern uint16_t T_start_y;
extern uint16_t T_heng_w ;
extern uint16_t T_heng_h ;

extern uint16_t T_shu_w;
extern uint16_t T_shu_h;

extern uint16_t I_start_x;
extern uint16_t I_start_y;
extern uint16_t I_w      ;
extern uint16_t I_shu_sy ;
extern uint16_t I_shu_h  ;

extern uint16_t X_start_x;
extern uint16_t X_start_y;
extern uint16_t X_w      ;
extern uint16_t X_h      ;
extern uint16_t X_line_w ;

// tix_color
void param_read_tix_color(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", tix_color);
}
void param_write_tix_color(struct param_stu *param, const char *new_val){
    uint16_t new_tix_color;

    sscanf(new_val, "%hd", &new_tix_color);

    tix_color = new_tix_color;

    LOG_FMT(PRINT_LOG"Set tix_color to %d\n", tix_color);
}

// T_start_x
void param_read_T_start_x(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_start_x);
}
void param_write_T_start_x(struct param_stu *param, const char *new_val){
    uint16_t new_T_start_x;

    sscanf(new_val, "%hd", &new_T_start_x);

    T_start_x = new_T_start_x;

    LOG_FMT(PRINT_LOG"Set T_start_x to %d\n", T_start_x);
}

// T_start_y
void param_read_T_start_y(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_start_y);
}
void param_write_T_start_y(struct param_stu *param, const char *new_val){
    uint16_t new_T_start_y;

    sscanf(new_val, "%hd", &new_T_start_y);

    T_start_y = new_T_start_y;

    LOG_FMT(PRINT_LOG"Set T_start_y to %d\n", T_start_y);
}

// T_heng_w
void param_read_T_heng_w(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_heng_w);
}
void param_write_T_heng_w(struct param_stu *param, const char *new_val){
    uint16_t new_T_heng_w;

    sscanf(new_val, "%hd", &new_T_heng_w);

    T_heng_w = new_T_heng_w;

    LOG_FMT(PRINT_LOG"Set  to %d\n", T_heng_w);
}

// T_heng_h
void param_read_T_heng_h(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_heng_h);
}
void param_write_T_heng_h(struct param_stu *param, const char *new_val){
    uint16_t new_T_heng_h;

    sscanf(new_val, "%hd", &new_T_heng_h);

    T_heng_h = new_T_heng_h;

    LOG_FMT(PRINT_LOG"Set T_heng_h to %d\n", T_heng_h);
}

// T_shu_w
void param_read_T_shu_w(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_shu_w);
}
void param_write_T_shu_w(struct param_stu *param, const char *new_val){
    uint16_t new_T_shu_w;

    sscanf(new_val, "%hd", &new_T_shu_w);

    T_shu_w = new_T_shu_w;

    LOG_FMT(PRINT_LOG"Set T_shu_w to %d\n", T_shu_w);
}

// T_shu_h
void param_read_T_shu_h(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", T_shu_h);
}
void param_write_T_shu_h(struct param_stu *param, const char *new_val){
    uint16_t new_T_shu_h;

    sscanf(new_val, "%hd", &new_T_shu_h);

    T_shu_h = new_T_shu_h;

    LOG_FMT(PRINT_LOG"Set  to %d\n", T_shu_h);
}

// I_start_x
void param_read_I_start_x(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", I_start_x);
}
void param_write_I_start_x(struct param_stu *param, const char *new_val){
    uint16_t new_I_start_x;

    sscanf(new_val, "%hd", &new_I_start_x);

    I_start_x = new_I_start_x;

    LOG_FMT(PRINT_LOG"Set I_start_x to %d\n", I_start_x);
}

// I_start_y
void param_read_I_start_y(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", I_start_y);
}
void param_write_I_start_y(struct param_stu *param, const char *new_val){
    uint16_t new_I_start_y;

    sscanf(new_val, "%hd", &new_I_start_y);

    I_start_y = new_I_start_y;

    LOG_FMT(PRINT_LOG"Set I_start_y to %d\n", I_start_y);
}

// I_w
void param_read_I_w(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", I_w);
}
void param_write_I_w(struct param_stu *param, const char *new_val){
    uint16_t new_I_w;

    sscanf(new_val, "%hd", &new_I_w);

    I_w = new_I_w;

    LOG_FMT(PRINT_LOG"Set I_w to %d\n", I_w);
}

// I_shu_sy
void param_read_I_shu_sy(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", I_shu_sy);
}
void param_write_I_shu_sy(struct param_stu *param, const char *new_val){
    uint16_t new_I_shu_sy;

    sscanf(new_val, "%hd", &new_I_shu_sy);

    I_shu_sy = new_I_shu_sy;

    LOG_FMT(PRINT_LOG"Set I_shu_sy to %d\n", I_shu_sy);
}

// I_shu_h
void param_read_I_shu_h(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", I_shu_h);
}
void param_write_I_shu_h(struct param_stu *param, const char *new_val){
    uint16_t new_I_shu_h;

    sscanf(new_val, "%hd", &new_I_shu_h);

    I_shu_h = new_I_shu_h;

    LOG_FMT(PRINT_LOG"Set I_shu_h to %d\n", I_shu_h);
}


// X_start_x
void param_read_X_start_x(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", X_start_x);
}
void param_write_X_start_x(struct param_stu *param, const char *new_val){
    uint16_t new_X_start_x;

    sscanf(new_val, "%hd", &new_X_start_x);

    X_start_x = new_X_start_x;

    LOG_FMT(PRINT_LOG"Set X_start_x to %d\n", X_start_x);
}

// X_start_y
void param_read_X_start_y(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", X_start_y);
}
void param_write_X_start_y(struct param_stu *param, const char *new_val){
    uint16_t new_X_start_y;

    sscanf(new_val, "%hd", &new_X_start_y);

    X_start_y = new_X_start_y;

    LOG_FMT(PRINT_LOG"Set X_start_y to %d\n", X_start_y);
}

// X_w
void param_read_X_w(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", X_w);
}
void param_write_X_w(struct param_stu *param, const char *new_val){
    uint16_t new_X_w;

    sscanf(new_val, "%hd", &new_X_w);

    X_w = new_X_w;

    LOG_FMT(PRINT_LOG"Set X_w to %d\n", X_w);
}

// X_h
void param_read_X_h(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", X_h);
}
void param_write_X_h(struct param_stu *param, const char *new_val){
    uint16_t new_X_h;

    sscanf(new_val, "%hd", &new_X_h);

    X_h = new_X_h;

    LOG_FMT(PRINT_LOG"Set X_h to %d\n", X_h);
}

// X_line_w
void param_read_X_line_w(struct param_stu *param){
    LOG_FMT(PRINT_LOG": %d\n", X_line_w);
}
void param_write_X_line_w(struct param_stu *param, const char *new_val){
    uint16_t new_X_line_w;

    sscanf(new_val, "%hd", &new_X_line_w);

    X_line_w = new_X_line_w;

    LOG_FMT(PRINT_LOG"Set X_line_w to %d\n", X_line_w);
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

    { // 旋转中心 y
        .param_name = "rotate_center_y",
        .param_read = param_read_rotate_center_y,
        .param_write = param_write_rotate_center_y,
    },

    { // tix_color
        .param_name = "tix_color",
        .param_read = param_read_tix_color,
        .param_write = param_write_tix_color,
    },

    { // T_start_x
        .param_name = "T_start_x",
        .param_read = param_read_T_start_x,
        .param_write = param_write_T_start_x,
    },

    { // T_start_y
        .param_name = "T_start_y",
        .param_read = param_read_T_start_y,
        .param_write = param_write_T_start_y,
    },

    { // T_heng_w
        .param_name = "T_heng_w",
        .param_read = param_read_T_heng_w,
        .param_write = param_write_T_heng_w,
    },

    { // T_heng_h
        .param_name = "T_heng_h",
        .param_read = param_read_T_heng_h,
        .param_write = param_write_T_heng_h,
    },

    { // T_shu_w
        .param_name = "T_shu_w",
        .param_read = param_read_T_shu_w,
        .param_write = param_write_T_shu_w,
    },

    { // T_shu_h
        .param_name = "T_shu_h",
        .param_read = param_read_T_shu_h,
        .param_write = param_write_T_shu_h,
    },

    { // I_start_x
        .param_name = "I_start_x",
        .param_read = param_read_I_start_x,
        .param_write = param_write_I_start_x,
    },

    { // I_start_y
        .param_name = "I_start_y",
        .param_read = param_read_I_start_y,
        .param_write = param_write_I_start_y,
    },

    { // I_w
        .param_name = "I_w",
        .param_read = param_read_I_w,
        .param_write = param_write_I_w,
    },

    { // I_shu_sy
        .param_name = "I_shu_sy",
        .param_read = param_read_I_shu_sy,
        .param_write = param_write_I_shu_sy,
    },

    { // I_shu_h
        .param_name = "I_shu_h",
        .param_read = param_read_I_shu_h,
        .param_write = param_write_I_shu_h,
    },

    { // X_start_x
        .param_name = "X_start_x",
        .param_read = param_read_X_start_x,
        .param_write = param_write_X_start_x,
    },

    { // X_start_y
        .param_name = "X_start_y",
        .param_read = param_read_X_start_y,
        .param_write = param_write_X_start_y,
    },

    { // X_w
        .param_name = "X_w",
        .param_read = param_read_X_w,
        .param_write = param_write_X_w,
    },

    { // X_h
        .param_name = "X_h",
        .param_read = param_read_X_h,
        .param_write = param_write_X_h,
    },

    { // X_line_w
        .param_name = "X_line_w",
        .param_read = param_read_X_line_w,
        .param_write = param_write_X_line_w,
    },


    // 末尾项
    {
        .param_name = " ",
    },
};
