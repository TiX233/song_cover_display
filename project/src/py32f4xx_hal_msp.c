/**
 ******************************************************************************
 * @file    py32f4xx_hal_msp.c
 * @author  MCU Application Team
 * @brief   This file provides code for the MSP Initialization
 *          and de-Initialization codes.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by Puya under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ltx_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

void my_SPI_io_init(void);

/**
 * @brief Initialize the MSP
 */
void HAL_MspInit(void)
{
    // __HAL_RCC_SYSCFG_CLK_ENABLE();
    // __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief Initialize SPI related MSP
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    /* Initialize SPI1 */
    if (hspi->Instance == SPI1)
    {
        __HAL_RCC_SYSCFG_CLK_ENABLE();                  /* Enable SYSCFG clock */
        __HAL_RCC_SPI1_CLK_ENABLE();                    /* Enable SPI1 clock */
        __HAL_RCC_DMA1_CLK_ENABLE();                    /* Enable DMA clock */

        my_SPI_io_init();

        /* Interrupt configuration */
        HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
        /* DMA_CH1 configuration */
        dma1ch1_handler.Instance = DMA1_Channel1;
        dma1ch1_handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
        dma1ch1_handler.Init.PeriphInc = DMA_PINC_DISABLE;
        dma1ch1_handler.Init.MemInc = DMA_MINC_ENABLE;
        if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
        {
            dma1ch1_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dma1ch1_handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        }
        else
        {
            dma1ch1_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            dma1ch1_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        }
        dma1ch1_handler.Init.Mode = DMA_NORMAL;
        dma1ch1_handler.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        /* Initialize DMA */
        if(HAL_DMA_Init(&dma1ch1_handler) != HAL_OK){
            while(1){
                LOG_STR(PRINT_ERROR"DMA1 CH1 init Failed!\n");
                HAL_Delay(1000);
            }
        }
        /* DMA handle is associated with SPI handle */
        __HAL_LINKDMA(hspi, hdmatx, dma1ch1_handler);

        /* Set DMA channel map. */
        HAL_DMA_ChannelMap(&dma1ch1_handler, DMA_CHANNEL_MAP_SPI1_WR); /* SPI1_TX DMA1_CH1 */

        /* DMA interrupt configuration*/
        HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    }
}

// #define __HAL_RCC_SPI1_FORCE_RESET()         (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
// #define __HAL_RCC_SPI1_RELEASE_RESET()       (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))

// #include "at32f403a_407_crm.h"

// #define __HAL_RCC_SPI1_FORCE_RESET()         crm_periph_clock_enable(CRM_SPI1_PERIPH_RESET, FALSE)
// #define __HAL_RCC_SPI1_RELEASE_RESET()       crm_periph_clock_enable(CRM_SPI1_PERIPH_RESET, TRUE)

/**
 * @brief Deinit SPI MSP
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        /* Reset SPI peripheral */
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();

        /* Disable SPI and GPIO clock */
        /* Deinit SPI SCK */

        HAL_NVIC_DisableIRQ(SPI1_IRQn);

        HAL_DMA_DeInit(&dma1ch1_handler);
        HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
    }
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
