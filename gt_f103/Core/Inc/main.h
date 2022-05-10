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
#include "stm32f1xx_hal.h"

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
#define BTN_ENC1_Pin GPIO_PIN_15
#define BTN_ENC1_GPIO_Port GPIOC
#define ENC2B_Pin GPIO_PIN_0
#define ENC2B_GPIO_Port GPIOA
#define ENC2A_Pin GPIO_PIN_1
#define ENC2A_GPIO_Port GPIOA
#define LD6_Pin GPIO_PIN_3
#define LD6_GPIO_Port GPIOA
#define BTN6_Pin GPIO_PIN_4
#define BTN6_GPIO_Port GPIOA
#define LD5_Pin GPIO_PIN_5
#define LD5_GPIO_Port GPIOA
#define BTN5_Pin GPIO_PIN_6
#define BTN5_GPIO_Port GPIOA
#define LD4_Pin GPIO_PIN_7
#define LD4_GPIO_Port GPIOA
#define BTN4_Pin GPIO_PIN_0
#define BTN4_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_1
#define LD3_GPIO_Port GPIOB
#define BTN3_Pin GPIO_PIN_2
#define BTN3_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_12
#define LD2_GPIO_Port GPIOB
#define BTN2_Pin GPIO_PIN_13
#define BTN2_GPIO_Port GPIOB
#define BTN_ENC2_Pin GPIO_PIN_14
#define BTN_ENC2_GPIO_Port GPIOB
#define LD1_Pin GPIO_PIN_15
#define LD1_GPIO_Port GPIOB
#define BTN1_Pin GPIO_PIN_8
#define BTN1_GPIO_Port GPIOA
#define EPD_CS_Pin GPIO_PIN_15
#define EPD_CS_GPIO_Port GPIOA
#define EPD_BUSY_Pin GPIO_PIN_4
#define EPD_BUSY_GPIO_Port GPIOB
#define ENC1B_Pin GPIO_PIN_6
#define ENC1B_GPIO_Port GPIOB
#define ENC1A_Pin GPIO_PIN_7
#define ENC1A_GPIO_Port GPIOB
#define EPD_RST_Pin GPIO_PIN_8
#define EPD_RST_GPIO_Port GPIOB
#define EPD_DC_Pin GPIO_PIN_9
#define EPD_DC_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
