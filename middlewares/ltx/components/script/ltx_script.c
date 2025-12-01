#include "ltx_script.h"
#include "ltx.h"

/**
 * @brief   脚本执行函数，内部调用，在初始化后由用户调用一次 ltx_Script_resume 便会自动执行，直到遇到 SC_TYPE_OVER 步骤
 * @param   script: 脚本对象指针
 * @retval  无
 */
void _ltx_Script_run(struct ltx_Script_stu *script){
    switch(script->step_type){
        case SC_TYPE_RUN_IN:
            ltx_Alarm_set_count(&(script->alarm_next_run), script->delay_ticks);
            ltx_Alarm_add(&(script->alarm_next_run));
            if(script->delay_ticks == 0){ // 下次调度立即执行，无需等到下一个 tick
                script->alarm_next_run.flag = 1;
            }

            script->callback(script);

            break;
            
        case SC_TYPE_RUN_DELAY:
            TickType_t delay_ticks = script->delay_ticks;

            script->callback(script);

            ltx_Alarm_set_count(&(script->alarm_next_run), delay_ticks);
            ltx_Alarm_add(&(script->alarm_next_run));

            if(delay_ticks == 0){ // 下次调度立即执行，无需等到下一个 tick
                script->alarm_next_run.flag = 1;
            }

            break;
            
        case SC_TYPE_WAIT_TOPIC:
            if(script->_last_topic_wait_for != NULL){
                ltx_Topic_unsubscribe(script->_last_topic_wait_for, &(script->subscriber_wait_for));
            }
            script->_last_topic_wait_for = script->topic_wait_for;
            ltx_Topic_subscribe(script->topic_wait_for, &(script->subscriber_wait_for));

            ltx_Alarm_set_count(&(script->alarm_next_run), script->delay_ticks);
            ltx_Alarm_add(&(script->alarm_next_run));

            if(script->delay_ticks == 0){ // 下次调度立即执行，无需等到下一个 tick
                script->alarm_next_run.flag = 1;
            }

            break;

        case SC_TYPE_OVER:

            break;

        default:

            break;
    }

}

void _ltx_Script_alarm_cb(void *param){
    struct ltx_Script_stu *pScript = container_of(param, struct ltx_Script_stu, alarm_next_run);
    
    if(pScript->triger_type == 68){ // 下一步骤为等待事件步骤
        pScript->triger_type = SC_TT_ALARM;
        _ltx_Script_run(pScript);

        return ;
    }
    pScript->triger_type = SC_TT_ALARM;

    if(pScript->step_type == SC_TYPE_WAIT_TOPIC){ // 等待事件超时
        // 取消订阅该事件，调用回调供用户处理
        ltx_Topic_unsubscribe(pScript->topic_wait_for, &(pScript->subscriber_wait_for));
        pScript->callback(pScript);
    }else { // 步骤延时到时
        _ltx_Script_run(pScript);
    }
}

void _ltx_Script_subscriber_cb(void *param){
    struct ltx_Script_stu *pScript = container_of(param, struct ltx_Script_stu, subscriber_wait_for);
    // 关闭超时闹钟
    ltx_Alarm_remove(&(pScript->alarm_next_run));
    pScript->alarm_next_run.flag = 0;

    pScript->triger_type = SC_TT_TOPIC;

    // 调用回调
    pScript->callback(pScript);
}

/**
 * @brief   初始化脚本函数，初始化后需要用户调用一次 ltx_Script_resume 函数该脚本才会开始运行
 * @param   script: 脚本对象指针
 * @param   callback: 脚本回调
 * @param   step_type: 第一次执行的步骤的类型
 * @param   delay_ticks: 该步骤的延时或超时时间
 * @param   topic_wait_for: 该步骤等待的事件，仅 step_type 为 SC_TYPE_WAIT_TOPIC 有效，非该类型则可传入 NULL
 * @retval  非 0 代表初始化失败
 */
int ltx_Script_init(struct ltx_Script_stu *script, void (*callback)(struct ltx_Script_stu *), ltx_Script_step_type_e step_type, TickType_t delay_ticks, struct ltx_Topic_stu *topic_wait_for){

    if(script == NULL || callback == NULL){
        return -1;
    }

    script->step_now = 0;
    script->triger_type = SC_TT_UNKNOWN;
    script->callback = callback;
    script->step_type = step_type;
    script->delay_ticks = delay_ticks;

    script->alarm_next_run.flag = 0;
    script->alarm_next_run.callback_alarm = _ltx_Script_alarm_cb;
    script->alarm_next_run.next = NULL;

    script->subscriber_wait_for.callback_func = _ltx_Script_subscriber_cb;
    script->subscriber_wait_for.next = NULL;
    script->_last_topic_wait_for = NULL;

    if(step_type == SC_TYPE_WAIT_TOPIC){
        if(topic_wait_for == NULL){
            return -2;
        }else {
            // ltx_Topic_subscribe(topic_wait_for, &(script->subscriber_wait_for));
        }
    }else {
        script->topic_wait_for = NULL;
    }
	
	return 0;
}

/**
 * @brief   设置脚本下一执行步骤的函数，由用户在回调函数中调用
 * @param   script: 脚本对象指针
 * @param   step_next: 下一步骤编号，不可设置为 0
 * @param   step_type: 第一次执行的步骤的类型
 * @param   delay_ticks: 该步骤的延时或超时时间
 * @param   topic_wait_for: 该步骤等待的事件，仅 step_type 为 SC_TYPE_WAIT_TOPIC 有效，非该类型则可传入 NULL
 * @retval  非 0 代表设置失败
 */
int ltx_Script_set_next_step(struct ltx_Script_stu *script, uint32_t step_next, ltx_Script_step_type_e step_type, TickType_t delay_ticks, struct ltx_Topic_stu *topic_wait_for){
    uint8_t flag = 0;
    if(step_next == 0){
        return -1;
    }
    
    if(script->step_type == SC_TYPE_WAIT_TOPIC){ // 是被话题步骤调用的
        flag = 1;
    }else { // 不是被话题步骤调用的
        if(step_type == SC_TYPE_WAIT_TOPIC){ // 下一步骤是话题步骤
            script->triger_type = 68;
        }
    }
    
    script->step_now = step_next;
    script->step_type = step_type;
    script->delay_ticks = delay_ticks;

    if(step_type == SC_TYPE_WAIT_TOPIC){
        if(topic_wait_for == NULL){
            return -2;
        }
        script->topic_wait_for = topic_wait_for;
    }

    if(flag){
        _ltx_Script_run(script);
    }

    return 0;
}

/**
 * @brief   获取当前步骤回调是由什么原因被调用，由用户在回调函数中调用
 * @param   script: 脚本对象指针
 * @retval  调用原因
 */
ltx_Script_triger_type_e ltx_Script_get_triger_type(struct ltx_Script_stu *script){

    return script->triger_type;
}

void ltx_Script_pause(struct ltx_Script_stu *script){
    ltx_Alarm_remove(&(script->alarm_next_run));
    script->alarm_next_run.flag = 0;
    if(script->step_type == SC_TYPE_WAIT_TOPIC){
        ltx_Topic_unsubscribe(script->topic_wait_for, &(script->subscriber_wait_for));
    }
}

void ltx_Script_resume(struct ltx_Script_stu *script){
    if(script->step_now == 0){
        _ltx_Script_run(script);

        return ;
    }
    ltx_Alarm_add(&(script->alarm_next_run));
    if(script->step_type == SC_TYPE_WAIT_TOPIC){
        ltx_Topic_subscribe(script->topic_wait_for, &(script->subscriber_wait_for));
    }
}
