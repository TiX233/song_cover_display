#include "myAPP_usb.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"

void task_func_usb_get(void *param);

struct ltx_Task_stu task_usb_get;

int myAPP_usb_init(struct ltx_App_stu *app){
    // 创建数据接收轮询任务
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
extern cdc_struct_type cdc_struct;
uint8_t *usbd_rx_buffer0 = cdc_struct.g_rx_buff;
uint8_t usbd_rx_buffer1[64] = {0, 0, 0};
uint8_t *usb_rx_buffers[] = {cdc_struct.g_rx_buff, usbd_rx_buffer1};
uint8_t usb_rx_buffer_now = 0;
// uint8_t *flag_usb_rx_cplt = &cdc_struct.g_rx_completed;
// 在 usb 中断里直接置 1 usb 接收任务运行标志位，主循环立即运行读取任务而无需 1ms 后轮询
uint8_t *flag_usb_task_run = &task_usb_get.topic.flag;

extern uint16_t picture_buffer[240*240];
uint16_t pic_buf_update_counter;

void task_func_usb_get(void *param){
    uint16_t length;
    uint8_t buffer_index;
    uint16_t pic_update_index;
    // static uint32_t counter_get_times = 0;

    // static uint8_t send_zero_packet = 0;
    if(cdc_struct.g_rx_completed){
        cdc_struct.g_rx_completed = 0;
        length = cdc_struct.g_rxlen;

        buffer_index = usb_rx_buffer_now;

        // 切换 buffer 立即接收下一包
        usb_rx_buffer_now = (usb_rx_buffer_now+1)%2;
        usbd_ept_recv(&usb_core_dev, USBD_CDC_BULK_OUT_EPT, usb_rx_buffers[usb_rx_buffer_now], USBD_CDC_OUT_MAXPACKET_SIZE);

        LOG_FMT("B%d, %d\n", buffer_index, length);
        // 处理接收到的数据
        switch(usb_rx_buffers[buffer_index][0]){
            case '/': // 上位机命令包头
                LOG_FMT("Get cmd:%s\n", usb_rx_buffers[buffer_index]);
                break;

            case '#': // 图片数据包头
                pic_update_index = ((uint16_t)usb_rx_buffers[buffer_index][1] << 8) | usb_rx_buffers[buffer_index][2];
                
                LOG_FMT("Get pic:%d\n", pic_update_index);
                break;

            default:

                break;
        }
        usb_rx_buffers[buffer_index][0] = 0;
        usb_rx_buffers[buffer_index][1] = 0;
        usb_rx_buffers[buffer_index][2] = 0;

        // counter_get_times ++;
        // LOG_FMT("U %d, %d\n", counter_get_times, length);
    }

#if 0
    length = usb_vcp_get_rxdata(&usb_core_dev, (uint8_t *)usbd_app_buffer_fs1);
    if(length > 0){
        usbd_app_buffer_fs1[length] = '\0';
        LOG_FMT("USB_GET: %d, %s\n", length, usbd_app_buffer_fs1);
    }
#endif

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
