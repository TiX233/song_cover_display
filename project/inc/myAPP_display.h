#ifndef __MYAPP_DISPLAY_H__
#define __MYAPP_DISPLAY_H__

#include "ltx_app.h"

extern struct ltx_App_stu app_display;
extern struct ltx_Topic_stu topic_player_resume;
extern struct ltx_Topic_stu topic_player_pause;

extern uint16_t disp_angle;
extern struct ltx_Topic_stu topic_pic_down_over;

void disp_pic_down(void);
void disp_pic_rotate(uint8_t on_off);

#endif // __MYAPP_DISPLAY_H__
