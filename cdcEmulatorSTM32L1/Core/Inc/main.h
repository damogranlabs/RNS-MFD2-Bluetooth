/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_exti.h"
#include "stm32l1xx_ll_cortex.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_dma.h"
#include "stm32l1xx_ll_spi.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_usart.h"
#include "stm32l1xx.h"
#include "stm32l1xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PLAY_PAUSE_Pin LL_GPIO_PIN_0
#define PLAY_PAUSE_GPIO_Port GPIOA
#define NEXT_Pin LL_GPIO_PIN_1
#define NEXT_GPIO_Port GPIOA
#define PREV_Pin LL_GPIO_PIN_2
#define PREV_GPIO_Port GPIOA
#define DOUT_CLK_Pin LL_GPIO_PIN_5
#define DOUT_CLK_GPIO_Port GPIOA
#define DOUT_Pin LL_GPIO_PIN_7
#define DOUT_GPIO_Port GPIOA
#define LD5_Pin LL_GPIO_PIN_7
#define LD5_GPIO_Port GPIOC
#define LD4_Pin LL_GPIO_PIN_8
#define LD4_GPIO_Port GPIOC
#define LD3_Pin LL_GPIO_PIN_9
#define LD3_GPIO_Port GPIOC
#define SWDIO_Pin LL_GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin LL_GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define BT_PWR_Pin LL_GPIO_PIN_12
#define BT_PWR_GPIO_Port GPIOC
#define DIN_Pin LL_GPIO_PIN_4
#define DIN_GPIO_Port GPIOB
#define DIN_EXTI_IRQn EXTI4_IRQn
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */
void resetRequest(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
