#include "tonearm.h"

void tonearm_detect(struct tonearm_stu *tonearm){
    tonearm->status = tonearm->read_pin();
}

tonearm_status_e tonearm_get_status(struct tonearm_stu *tonearm){
    return tonearm->status;
}

void tonearm_leave(struct tonearm_stu *tonearm, uint8_t duty){
    duty = duty>100 ? 100 : duty;
    switch(tonearm->type){
        case TONEARM_DT_A:
            tonearm->set_duty_a(0);
            tonearm->set_duty_b(duty);
            break;

        case TONEARM_DT_B:
            tonearm->set_duty_a(duty);
            tonearm->set_duty_b(0);
            break;

        default:

            break;        
    }
}

void tonearm_close(struct tonearm_stu *tonearm, uint8_t duty){
    duty = duty>100 ? 100 : duty;
    switch(tonearm->type){
        case TONEARM_DT_A:
            tonearm->set_duty_a(duty);
            tonearm->set_duty_b(0);
            break;

        case TONEARM_DT_B:
            tonearm->set_duty_a(0);
            tonearm->set_duty_b(duty);
            break;

        default:

            break;        
    }
}
