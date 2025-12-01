#include "ltx_script.h"
#include "ltx.h"

void _ltx_Script_alarm_cb(void *param){
    struct ltx_Script_stu *pScript = container_of(param, struct ltx_Script_stu, alarm_next_run);
    ltx_Script_run(pScript);
}

int ltx_Script_init(struct ltx_Script_stu *script, struct ltx_Script_step_stu *step_list, void (*callback)(struct ltx_Script_stu *)){

    if(script == NULL || step_list == NULL || callback == NULL){
        return -1;
    }

    script->step_now = 0;

    script->step_list = step_list;
    script->callback = callback;

    script->alarm_next_run.flag = 0;
    script->alarm_next_run.callback_alarm = _ltx_Script_alarm_cb;
    script->alarm_next_run.next = NULL;
	
	return 0;
}

void ltx_Script_run(struct ltx_Script_stu *script){
    switch(script->step_list[script->step_now].step_type){
        case SC_TYPE_RUN_IN:
            ltx_Alarm_set_count(&(script->alarm_next_run), script->step_list[script->step_now].delay_ticks);
            ltx_Alarm_add(&(script->alarm_next_run));
            script->callback(script);

            if(script->step_list[script->step_now].delay_ticks == 0){ // 下次调度立即执行，无需等到下一个 tick
                script->alarm_next_run.flag = 1;
            }

            break;
            
        case SC_TYPE_RUN_DELAY:
            script->callback(script);
            ltx_Alarm_set_count(&(script->alarm_next_run), script->step_list[script->step_now].delay_ticks);
            ltx_Alarm_add(&(script->alarm_next_run));

            if(script->step_list[script->step_now].delay_ticks == 0){ // 下次调度立即执行，无需等到下一个 tick
                script->alarm_next_run.flag = 1;
            }

            break;
            
        case SC_TYPE_OVER:

            break;

        default:

            break;
    }

    // script->step_now ++;
    // step 回调内部用户自己决定下一个 step
}

// 下次需要跳转到的 step 编号
void ltx_Script_step_jump(struct ltx_Script_stu *script, uint32_t next_step){
    script->step_now = next_step;
}

// 下次需要跳转到的 step 编号为当前 step + 1
void ltx_Script_step_next(struct ltx_Script_stu *script){
    script->step_now ++;
}

void ltx_Script_pause(struct ltx_Script_stu *script){
    ltx_Alarm_remove(&(script->alarm_next_run));
}

void ltx_Script_resume(struct ltx_Script_stu *script){
    ltx_Alarm_add(&(script->alarm_next_run));
}
