/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f403a_407_wk_config.h"
#include "wk_acc.h"
#include "wk_adc.h"
#include "wk_debug.h"
#include "wk_spi.h"
#include "wk_tmr.h"
#include "wk_usbfs.h"
#include "wk_dma.h"
#include "wk_gpio.h"
#include "usb_app.h"
#include "wk_system.h"
#include "arm_math.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "main.h"

#include "ltx_log.h"
#include "ltx_app.h"

#include "myAPP_system.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
SPI_HandleTypeDef spi1_handler;
DMA_HandleTypeDef dma1ch1_handler;

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
    ltx_Log_init();
    LOG_STR("\n\nSYSTEM START\n\n");
  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init adc1 function. */
  wk_adc1_init();
#if 0
  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        (uint32_t)&SPI1->dt, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init spi1 function. */
  wk_spi1_init();
#endif
  /* init tmr2 function. */
  wk_tmr2_init();

  /* init tmr3 function. */
  wk_tmr3_init();

  /* init acc function. */
  wk_acc_init();

  /* init usbfs function. */
  wk_usbfs_init();

  /* init usb app function. */
  wk_usb_app_init();

  /* add user code begin 2 */



    LOG_FMT(PRINT_LOG"MCU init over in %dms\n", ltx_Sys_get_tick());

    // 创建系统基础 app 并设为运行态
    LOG_STR(PRINT_DEBUG"Create system basic app\n");
    ltx_App_init(&app_system);
    ltx_App_resume(&app_system);

    // 创建外部硬件初始化 app 并设为运行态
    LOG_STR(PRINT_DEBUG"Create device init app\n");
    // todo

    LOG_STR(PRINT_LOG"System running...\n");

    ltx_Sys_schedule_start(); // 开启调度器
    ltx_Sys_scheduler(); // 运行调度器

  /* add user code end 2 */

  while(1)
  {
     wk_usb_app_task();

    /* add user code begin 3 */

    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

void my_SPI_init(void){
    spi1_handler.Instance               = (SPI_TypeDef *)SPI1;                       /* SPI1 */
    spi1_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;    /* prescaler :2 */
    spi1_handler.Init.Direction         = SPI_DIRECTION_1LINE;
    spi1_handler.Init.CLKPolarity       = SPI_POLARITY_LOW;           /* SPI Clock Polarity: low */
    spi1_handler.Init.CLKPhase          = SPI_PHASE_1EDGE;            /* Data sampling starts at the first clock edge */
    spi1_handler.Init.DataSize          = SPI_DATASIZE_8BIT;          /* SPI Data Size is 8 bit */
    spi1_handler.Init.FirstBit          = SPI_FIRSTBIT_MSB;           /* SPI MSB Transmission */
    spi1_handler.Init.NSS               = SPI_NSS_SOFT;               /* NSS Hardware mode */
    spi1_handler.Init.Mode = SPI_MODE_MASTER;                         /* Configure as host */
    spi1_handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;    /* The CRC check is disabled */
    /* spi1_handler.Init.CRCPolynomial = 1; */                        /* CRC polynomial value */
    if (HAL_SPI_DeInit(&spi1_handler) != HAL_OK)
    {
        while(1){
            LOG_STR(PRINT_ERROR"SPI Deinit Failed!\n");
            wk_delay_ms(1000);
        }
    }
    
    /* Initialize SPI peripheral */
    if (HAL_SPI_Init(&spi1_handler) != HAL_OK)
    {
        while(1){
            LOG_STR(PRINT_ERROR"SPI Init Failed!\n");
            wk_delay_ms(1000);
        }
    }
  
}

// 扩展内存
void Extend_SRAM(void){
    // check if RAM has been set to 224K, if not, change EOPB0
    if(((USD->eopb0) & 0xFF) != 0xFE){
        /* Unlock Option Bytes Program Erase controller */
        flash_unlock();
        /* Erase Option Bytes */
        flash_user_system_data_erase();
        /* Change SRAM size to 224KB */
        flash_user_system_data_program((uint32_t)&USD->eopb0,0xFE);
        NVIC_SystemReset();
    }
}

  /* add user code end 4 */
