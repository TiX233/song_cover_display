#include "myAPP_tonearm.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "myAPP_device_init.h"
#include "tonearm.h"

void task_func_stylus_detect(void *param);

// 唱针位置检测周期任务
struct ltx_Task_stu task_stylus_detect;

// 唱针位置变化话题
struct ltx_Topic_stu topic_stylus_change = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

int myAPP_tonearm_init(struct ltx_App_stu *app){
    // 创建唱针位置检测周期任务
    ltx_Task_set_name(&task_stylus_detect, "stylus_detect");
    ltx_Task_set_period(&task_stylus_detect, 123, 1);
    ltx_Task_set_callback(&task_stylus_detect, task_func_stylus_detect);
    ltx_Task_init(&task_stylus_detect, app);

    return 0;
}

int myAPP_tonearm_pause(struct ltx_App_stu *app){

    return 0;
}

int myAPP_tonearm_resume(struct ltx_App_stu *app){


    return 0;
}

int myAPP_tonearm_destroy(struct ltx_App_stu *app){

    // free...

    return 0;
}

struct ltx_App_stu app_tonearm = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "tonearm",

    .init = myAPP_tonearm_init,
    .pause = myAPP_tonearm_pause,
    .resume = myAPP_tonearm_resume,
    .destroy = myAPP_tonearm_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


// 唱针位置检测周期任务
void task_func_stylus_detect(void *param){
    static uint8_t now_status = TONEARM_STATUS_CLOSE;

    tonearm_detect(&myTonearm);
    if(now_status != tonearm_get_status(&myTonearm)){ // 唱针位置变化
        now_status = tonearm_get_status(&myTonearm);
        // 发布话题
        ltx_Topic_publish(&topic_stylus_change);
    }
}
