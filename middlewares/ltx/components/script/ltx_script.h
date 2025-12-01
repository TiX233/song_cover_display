/**
 * @file ltx_script.h
 * @author realTiX
 * @brief ltx 可选脚本组件。用于动画或者初始化硬件之类的执行序列，可在自定义回调设置下一 step 类型实现分支结构，每个 step 间可自定义延时。步骤可等待事件并设置超时时间。（状态机协程疑似
 * @version 0.2
 * @date 2025-11-25 (0.1，初步完成功能设计)
 *       2025-12-01 (0.2，增加等待事件步骤类型，重构所有内容)
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
    SC_TYPE_WAIT_TOPIC, // 等待某个事件话题发布，超时时间设为 0 将一直等待直到 TickType_t 溢出（50 来天），该类型步骤执行完会立即执行下一步骤
    SC_TYPE_OVER,       // 序列结束，不再执行
} ltx_Script_step_type_e;

// 等待话题步骤回调被触发的类型
typedef enum {
    SC_TT_UNKNOWN = 0,  // 未知
    SC_TT_ALARM,        // 因为超时而被触发
    SC_TT_TOPIC,        // 因为话题发布而被触发
} ltx_Script_triger_type_e;

// 脚本结构体
struct ltx_Script_stu {
    uint32_t step_now; // 当前执行到的步骤，0 起，下一步骤编号需要用户在自定义回调里更新，不能设置为 0
    ltx_Script_step_type_e step_type; // 当前步骤的类型
    ltx_Script_triger_type_e triger_type; // 等待话题步骤被触发的原因
    
    TickType_t delay_ticks; // TYPE_RUN 类型下置为 0 时，与下一步骤间会让出一次调度轮询以给其他任务执行机会，ltx 会在下一次调度轮询立即调用下一步骤，而非下一 tick

    void (*callback)(struct ltx_Script_stu *script); // 自定义回调

    struct ltx_Topic_stu *topic_wait_for; // 某一步骤所等待事件的话题指针
    struct ltx_Topic_subscriber_stu subscriber_wait_for; // 管理脚本等待事件的订阅者
    struct ltx_Alarm_stu alarm_next_run; // 管理脚本下次运行/超时时间的闹钟
    struct ltx_Topic_stu *_last_topic_wait_for; // 上次等待事件的话题指针
};

int ltx_Script_init(struct ltx_Script_stu *script, void (*callback)(struct ltx_Script_stu *), ltx_Script_step_type_e step_type, TickType_t delay_ticks, struct ltx_Topic_stu *topic_wait_for);

int ltx_Script_set_next_step(struct ltx_Script_stu *script, uint32_t step_next, ltx_Script_step_type_e step_type, TickType_t delay_ticks, struct ltx_Topic_stu *topic_wait_for);

ltx_Script_triger_type_e ltx_Script_get_triger_type(struct ltx_Script_stu *script);

void ltx_Script_pause(struct ltx_Script_stu *script);
void ltx_Script_resume(struct ltx_Script_stu *script);

#endif // __LTX_SCRIPT_H__
