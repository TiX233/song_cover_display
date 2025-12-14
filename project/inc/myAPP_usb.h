#ifndef __MYAPP_USB_H__
#define __MYAPP_USB_H__

#include "ltx_app.h"

extern struct ltx_App_stu app_usb;

void send_str_2_usb(uint8_t *str, uint8_t len);

#endif // __MYAPP_USB_H__
