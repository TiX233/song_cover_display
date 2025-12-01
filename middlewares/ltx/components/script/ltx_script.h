/**
 * @file ltx_script.h
 * @author realTiX
 * @brief ltx 可选脚本组件。用于动画或者初始化硬件之类的执行序列，可通过自定义回调设置下一 step 编号实现分支结构，每个 step 间可自定义延时。（搞半天好像还是状态机？
 * @version 0.1
 * @date 2025-11-25 (0.1，初步完成功能设计)
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __LTX_SCRIPT_H__
#define __LTX_SCRIPT_H__

#include "ltx.h"

// 脚本步骤类型
typedef enum {
    SC_TYPE_RUN_IN = 0, // 从该步骤开始运行到下一步骤开始运行在处于一定时间内，适合动画场景
    SC_TYPE_RUN_DELAY,  // 从该步骤运行完到下一步骤开始运行间隔一定时间，适合硬件初始化场景
    SC_TYPE_OVER,       // 序列结束，不再执行
} ltx_Script_step_type_e;

// 脚本步骤结构体
struct ltx_Script_step_stu {
    uint8_t step_type; // 当前步骤的类型，可配置为 ltx_Script_step_type_e 列出类型

    TickType_t delay_ticks; // 可置为 0，与下一步骤间会让出一次调度轮询以给其他任务执行机会，ltx 会在下一次调度轮询立即调用下一步骤，而非下一 tick
};

// 脚本结构体
struct ltx_Script_stu {
    uint32_t step_now; // 当前执行到的步骤，0 起，下一步骤编号需要用户在自定义回调里更新（PC 指针说是

    struct ltx_Script_step_stu *step_list; // 步骤数组

    void (*callback)(struct ltx_Script_stu *script); // 自定义回调

    struct ltx_Alarm_stu alarm_next_run; // 管理脚本下次运行的闹钟，内部使用
};

int ltx_Script_init(struct ltx_Script_stu *script, struct ltx_Script_step_stu *step_list, void (*callback)(struct ltx_Script_stu *));
void ltx_Script_run(struct ltx_Script_stu *script);

void ltx_Script_step_jump(struct ltx_Script_stu *script, uint32_t next_step);
void ltx_Script_step_next(struct ltx_Script_stu *script);
void ltx_Script_pause(struct ltx_Script_stu *script);
void ltx_Script_resume(struct ltx_Script_stu *script);

#endif // __LTX_SCRIPT_H__
