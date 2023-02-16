/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define W_Car_Pin GPIO_PIN_4
#define W_Car_GPIO_Port GPIOC
#define W_Car_EXTI_IRQn EXTI4_IRQn
#define STCP595_Pin GPIO_PIN_12
#define STCP595_GPIO_Port GPIOB
#define S_Car_Pin GPIO_PIN_13
#define S_Car_GPIO_Port GPIOB
#define S_Car_EXTI_IRQn EXTI15_10_IRQn
#define E_Car_Pin GPIO_PIN_14
#define E_Car_GPIO_Port GPIOB
#define E_Car_EXTI_IRQn EXTI15_10_IRQn
#define Enable595_Pin GPIO_PIN_7
#define Enable595_GPIO_Port GPIOC
#define Reset595_Pin GPIO_PIN_9
#define Reset595_GPIO_Port GPIOA
#define N_Car_Pin GPIO_PIN_10
#define N_Car_GPIO_Port GPIOA
#define N_Car_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define S_Btn_Pin GPIO_PIN_15
#define S_Btn_GPIO_Port GPIOA
#define S_Btn_EXTI_IRQn EXTI15_10_IRQn
#define SHCP595_Pin GPIO_PIN_10
#define SHCP595_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define DS595_Pin GPIO_PIN_5
#define DS595_GPIO_Port GPIOB
#define N_Btn_Pin GPIO_PIN_7
#define N_Btn_GPIO_Port GPIOB
#define N_Btn_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
