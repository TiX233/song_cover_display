#ifndef __TONEARM_H__
#define __TONEARM_H__

#include "ltx.h"

typedef enum {
    TONEARM_DT_A = 0, // 正电压唱臂靠近胶片
    TONEARM_DT_B,     // 负电压唱臂靠近胶片  
} tonearm_driver_type_e;


typedef enum {
    TONEARM_STATUS_CLOSE = 0, // 唱臂靠近胶片
    TONEARM_STATUS_LEAVE,     // 唱臂离开胶片  
} tonearm_status_e;


struct tonearm_stu {
    tonearm_driver_type_e type;
    tonearm_status_e status;

    uint8_t (*read_pin)(void);
    void (*set_duty_a)(uint8_t duty);
    void (*set_duty_b)(uint8_t duty);
};

void tonearm_detect(struct tonearm_stu *tonearm);
tonearm_status_e tonearm_get_status(struct tonearm_stu *tonearm);

void tonearm_leave(struct tonearm_stu *tonearm, uint8_t duty);
void tonearm_close(struct tonearm_stu *tonearm, uint8_t duty);

#endif // __TONEARM_H__
