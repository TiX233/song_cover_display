#include "myAPP_system.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_param.h"
#include "ltx_log.h"
#include "ltx_cmd.h"

void task_func_heart_beat(void *param);
void task_func_cmd(void *param);


// 心拍周期任务对象
struct ltx_Task_stu task_heart_beat;

// 命令处理周期任务对象
struct ltx_Task_stu task_cmd;


int myApp_system_init(struct ltx_App_stu *app){
    // 创建心拍周期任务
    ltx_Task_set_name(&task_heart_beat, "heart_beat");
    ltx_Task_set_period(&task_heart_beat, 1000, 0);
    ltx_Task_set_callback(&task_heart_beat, task_func_heart_beat);
    ltx_Task_init(&task_heart_beat, app);

    // 创建命令处理周期任务
    ltx_Task_set_name(&task_cmd, "cmd");
    ltx_Task_set_period(&task_cmd, 200, 0);
    ltx_Task_set_callback(&task_cmd, task_func_cmd);
    ltx_Task_init(&task_cmd, app);

    return 0;
}

int myApp_system_pause(struct ltx_App_stu *app){

    return 0;
}

int myApp_system_resume(struct ltx_App_stu *app){

    return 0;
}

int myApp_system_destroy(struct ltx_App_stu *app){

    // free...

    return 0;
}


struct ltx_App_stu app_system = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "system",

    .init = myApp_system_init,
    .pause = myApp_system_pause,
    .resume = myApp_system_resume,
    .destroy = myApp_system_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


// 心拍任务
uint32_t heart_beat_count = 0;
void task_func_heart_beat(void *param){

    heart_beat_count ++;
    // LOG_FMT("Heartbeat: %d\n", heart_beat_count);

}


uint8_t cmd_buffer[CMD_BUF_SIZE];
// 处理命令任务
void task_func_cmd(void *param){

    // 读取命令
    if(SEGGER_RTT_HasData(0)){
        int len = SEGGER_RTT_Read(0, cmd_buffer, CMD_BUF_SIZE - 1);
        if (len > 0) {
            cmd_buffer[len] = '\0'; // 添加字符串终止符
            for(uint8_t i = len - 1; i > 0; i --){ // 去除尾追回车
                if(cmd_buffer[i] == '\n' || cmd_buffer[i] == '\r'){
                    cmd_buffer[i] = 0;
                }else {
                    break;
                }
            }

            // LOG_FMT(PRINT_DEBUG"cmd len: %d\n", len);
            ltx_Cmd_process((char *)cmd_buffer); // 处理命令
        }
    }
}
