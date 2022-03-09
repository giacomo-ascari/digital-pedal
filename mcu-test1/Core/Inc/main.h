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
#include "stm32f4xx_hal.h"

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
#define OtgPower_Pin GPIO_PIN_0
#define OtgPower_GPIO_Port GPIOC
#define Btn0_Pin GPIO_PIN_0
#define Btn0_GPIO_Port GPIOA
#define EPD_RST_Pin GPIO_PIN_1
#define EPD_RST_GPIO_Port GPIOA
#define EPD_DC_Pin GPIO_PIN_2
#define EPD_DC_GPIO_Port GPIOA
#define EPD_BUSY_Pin GPIO_PIN_3
#define EPD_BUSY_GPIO_Port GPIOA
#define EPD_CS_Pin GPIO_PIN_6
#define EPD_CS_GPIO_Port GPIOA
#define Btn3_Pin GPIO_PIN_11
#define Btn3_GPIO_Port GPIOB
#define Btn2_Pin GPIO_PIN_14
#define Btn2_GPIO_Port GPIOB
#define Btn1_Pin GPIO_PIN_8
#define Btn1_GPIO_Port GPIOD
#define Led1_Pin GPIO_PIN_12
#define Led1_GPIO_Port GPIOD
#define Led2_Pin GPIO_PIN_13
#define Led2_GPIO_Port GPIOD
#define Led3_Pin GPIO_PIN_14
#define Led3_GPIO_Port GPIOD
#define Led4_Pin GPIO_PIN_15
#define Led4_GPIO_Port GPIOD
#define AudioPower_Pin GPIO_PIN_4
#define AudioPower_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
