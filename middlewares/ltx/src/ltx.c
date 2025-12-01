#include "ltx.h"

volatile TickType_t realTicks; // 系统时间，溢出处理 todo
uint8_t flag_schedule = 0; // 调度开关标志位

struct ltx_Topic_stu ltx_sys_topic_list = {
    .subscriber = NULL,

    .next = NULL,
};

struct ltx_Timer_stu ltx_sys_timer_list = {
    .next = NULL,
};

struct ltx_Alarm_stu ltx_sys_alarm_list = {
    .next = NULL,
};

// 定时器
void ltx_Timer_add(struct ltx_Timer_stu *timer){

    if(timer->next != NULL){ // 已经存在，不重复加入
        return ;
    }

    struct ltx_Timer_stu **pTimer = &(ltx_sys_timer_list.next);
    while((*pTimer) != NULL){
        pTimer = &((*pTimer)->next);
    }
    *pTimer = timer;
    timer->next = NULL;
}

void ltx_Timer_remove(struct ltx_Timer_stu *timer){
    struct ltx_Timer_stu *pTimer = ltx_sys_timer_list.next;

    if(pTimer == NULL){ // 定时器不在活跃列表中，不做操作
        return ;
    }

    if(pTimer == timer){
        ltx_sys_timer_list.next = timer->next;
        timer->next = NULL;

        return ;
    }

    while(pTimer->next != timer && pTimer->next != NULL){
        pTimer = pTimer->next;
    }

    if(pTimer->next == timer){
        pTimer->next = timer->next;
        timer->next = NULL;
    }

}

// 闹钟
void ltx_Alarm_add(struct ltx_Alarm_stu *alarm){
    
    if(alarm->next != NULL){ // 已经存在，不重复加入
        return ;
    }

    struct ltx_Alarm_stu **pAlarm = &(ltx_sys_alarm_list.next);
    while((*pAlarm) != NULL){
        pAlarm = &((*pAlarm)->next);
    }
    *pAlarm = alarm;
    alarm->next = NULL;
}

void ltx_Alarm_remove(struct ltx_Alarm_stu *alarm){
    struct ltx_Alarm_stu *pAlarm = ltx_sys_alarm_list.next;

    if(pAlarm == NULL){ // 闹钟不在活跃列表中，不做操作
        return ;
    }

    if(pAlarm == alarm){
        ltx_sys_alarm_list.next = alarm->next;
        alarm->next = NULL;
        alarm->flag = 0;

        return ;
    }

    while(pAlarm->next != alarm && pAlarm->next != NULL){
        pAlarm = pAlarm->next;
    }

    if(pAlarm->next == alarm){
        pAlarm->next = alarm->next;
        alarm->next = NULL;
        alarm->flag = 0;
    }

}

void ltx_Alarm_set_count(struct ltx_Alarm_stu *alarm, TickType_t tick_count_down){
    alarm->tick_count_down = tick_count_down;
}

// 话题，感觉可以用字符串去匹配，就不用全局变量满天飞了
void ltx_Topic_add(struct ltx_Topic_stu *topic){

    if(topic->next != NULL){ // 已经存在，不重复加入
        return ;
    }

    struct ltx_Topic_stu **pTopic = &(ltx_sys_topic_list.next);
    while((*pTopic) != NULL){
        pTopic = &((*pTopic)->next);
    }
    *pTopic = topic;
    topic->next = NULL;
}

void ltx_Topic_remove(struct ltx_Topic_stu *topic){
    struct ltx_Topic_stu *pTopic = ltx_sys_topic_list.next;

    if(pTopic == NULL){ // 话题没有在活动列表中，不做操作
        return ;
    }

    if(pTopic == topic){
        ltx_sys_topic_list.next = topic->next;
        topic->next = NULL;

        return ;
    }

    while(pTopic->next != topic && pTopic->next != NULL){
        pTopic = pTopic->next;
    }

    if(pTopic->next == topic){
        pTopic->next = topic->next;
        topic->next = NULL;
    }

}

void ltx_Topic_subscribe(struct ltx_Topic_stu *topic, struct ltx_Topic_subscriber_stu *subscriber){

    if(subscriber->next != NULL){ // 已经存在，不重复添加
        // 但是不添加额外的成员变量的话，只能遍历所有话题才能避免一个订阅者订阅多个话题，先不管
        return ;
    }
    
    struct ltx_Topic_subscriber_stu **pSub = &(topic->subscriber);
    while((*pSub) != NULL){
        pSub = &((*pSub)->next);
    }
    *pSub = subscriber;
    subscriber->next = NULL;
}

void ltx_Topic_unsubscribe(struct ltx_Topic_stu *topic, struct ltx_Topic_subscriber_stu *subscriber){
    struct ltx_Topic_subscriber_stu *pSub = topic->subscriber;
    
    if(pSub == NULL){ // 话题没有订阅者，不做操作
        return ;
    }

    if(pSub == subscriber){
        topic->subscriber = subscriber->next;
        subscriber->next = NULL;
        
        return ;
    }
    
    while(pSub->next != subscriber && pSub->next != NULL){
        pSub = pSub->next;
    }
    
    if(pSub->next == subscriber){
        pSub->next = subscriber->next;
        subscriber->next = NULL;
    }
}

void ltx_Topic_publish(struct ltx_Topic_stu *topic){
    topic->flag = 1;

    /*
    struct ltx_Topic_subscriber_stu *pSub = topic->subscriber;
    while(pSub != NULL){
        pSub->flag ++;
        pSub = pSub->next;
    }
    */
}

// 系统调度相关
void ltx_Sys_tick_tack(void){
    realTicks ++;
    if(!flag_schedule){
        return ;
    }

    struct ltx_Alarm_stu *pAlarm = ltx_sys_alarm_list.next;
    while(pAlarm != NULL){
        if(0 == --pAlarm->tick_count_down){
            pAlarm->flag = 1;
        }
        pAlarm = pAlarm->next;
    }

    struct ltx_Timer_stu *pTimer = ltx_sys_timer_list.next;
    while(pTimer != NULL){
        if(0 == -- pTimer->tick_counts){
            pTimer->tick_counts = pTimer->tick_reload;
            ltx_Topic_publish(pTimer->topic);
        }
        pTimer = pTimer->next;
    }
}

TickType_t ltx_Sys_get_tick(void){
    return realTicks;
}

void ltx_Sys_scheduler(void){
    struct ltx_Topic_stu *pTopic;
    struct ltx_Topic_subscriber_stu *pSubscriber;
    struct ltx_Alarm_stu *pAlarm;

    while(1){
        // 处理订阅
        pTopic = ltx_sys_topic_list.next;
        while(pTopic != NULL){
            /*
            pSubscriber = pTopic->subscriber;
            while(pSubscriber != NULL){
                if(pSubscriber->flag){
                    pSubscriber->callback_func(NULL);
                    pSubscriber->flag = 0;
                }

                pSubscriber = pSubscriber->next;
            }
            */
            if(pTopic->flag){
                pSubscriber = pTopic->subscriber;
                while(pSubscriber != NULL){
                    pSubscriber->callback_func(pSubscriber);

                    pSubscriber = pSubscriber->next;
                }

                pTopic->flag = 0;
            }

            pTopic = pTopic->next;
        }

        // 处理闹钟
        pAlarm = ltx_sys_alarm_list.next;
        while(pAlarm != NULL){
            if(pAlarm->flag){
                ltx_Alarm_remove(pAlarm); // 必须先移除闹钟再调用回调，不然无法在回调中创建调用自己的闹钟
                pAlarm->callback_alarm(pAlarm);
            }

            pAlarm = pAlarm->next;
        }
    }
}

void ltx_Sys_schedule_start(void){
    flag_schedule = 1;
}

void ltx_Sys_schedule_stop(void){
    flag_schedule = 0;
}
