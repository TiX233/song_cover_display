#include "myAPP_usb.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"

void task_func_usb_get(void *param);

struct ltx_Task_stu task_usb_get;

int myAPP_usb_init(struct ltx_App_stu *app){
    // 创建心拍周期任务
    ltx_Task_set_name(&task_usb_get, "usb_get");
    ltx_Task_set_period(&task_usb_get, 1, 1);
    ltx_Task_set_callback(&task_usb_get, task_func_usb_get);
    ltx_Task_init(&task_usb_get, app);

    return 0;
}

int myAPP_usb_pause(struct ltx_App_stu *app){

    return 0;
}

int myAPP_usb_resume(struct ltx_App_stu *app){

    return 0;
}

int myAPP_usb_destroy(struct ltx_App_stu *app){

    // free...

    return 0;
}

struct ltx_App_stu app_usb = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "usb",

    .init = myAPP_usb_init,
    .pause = myAPP_usb_pause,
    .resume = myAPP_usb_resume,
    .destroy = myAPP_usb_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


#include "usb_conf.h"
#include "wk_system.h"

#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"

extern usbd_core_type usb_core_dev;
uint8_t usbd_app_buffer_fs1[128 * 4];

void task_func_usb_get(void *param){
    uint32_t length = 0;

    // static uint8_t send_zero_packet = 0;

    length = usb_vcp_get_rxdata(&usb_core_dev, (uint8_t *)usbd_app_buffer_fs1);
    if(length > 0){
        usbd_app_buffer_fs1[length] = '\0';
        LOG_FMT("USB_GET: %d, %s\n", length, usbd_app_buffer_fs1);
    }

    #if 0
    if(length > 0 || send_zero_packet == 1)
    {
        if(length > 0)
            send_zero_packet = 1;

        if(length == 0)
        send_zero_packet = 0;
        
        do
        {
        /* send data to host */
        if(usb_vcp_send_data(&usb_core_dev, (uint8_t *)usbd_app_buffer_fs1, length) == SUCCESS)
        {
            break;
        }
        }while(timeout --);
    }
    #endif
}
