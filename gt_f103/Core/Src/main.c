/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "epddriver.h"
#include "commander.h"
#include "painter2.h"
#include "rencoder.h"
#include "menu.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

Commander_HandleTypeDef hcommander;

Menu_HandleTypeDef hmenu;

uint16_t btn_pins[8] = { BTN1_Pin, BTN2_Pin, BTN3_Pin, BTN4_Pin, BTN5_Pin, BTN6_Pin, BTN_ENC1_Pin, BTN_ENC2_Pin };
GPIO_TypeDef * btn_ports[8] = { BTN1_GPIO_Port, BTN2_GPIO_Port, BTN3_GPIO_Port, BTN4_GPIO_Port, BTN5_GPIO_Port, BTN6_GPIO_Port, BTN_ENC1_GPIO_Port, BTN_ENC2_GPIO_Port };

uint16_t led_pins[6] = { LD1_Pin, LD2_Pin, LD3_Pin, LD4_Pin, LD5_Pin, LD6_Pin };
GPIO_TypeDef * led_ports[6] = { LD1_GPIO_Port, LD2_GPIO_Port, LD3_GPIO_Port, LD4_GPIO_Port, LD5_GPIO_Port, LD6_GPIO_Port };

//float display_array[296];
EPD_HandleTypeDef hepd1;

RE_HandleTypeDef hre1;
RE_HandleTypeDef hre2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void blink() {
	for (uint8_t i = 0; i < 6; i++) HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);
	for (uint32_t i = 0; i < 1000000; i++) {};
	for (uint8_t i = 0; i < 6; i++) HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_RESET);
	for (uint32_t i = 0; i < 1000000; i++) {}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	Command command;
	memcpy(&command, hcommander.uart_rx_buffer, COMMAND_BYTESIZE);
	Commander_Enqueue(&hcommander, &command);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	Command command;
	memcpy(&command, hcommander.uart_rx_buffer + COMMAND_BYTESIZE, COMMAND_BYTESIZE);
	Commander_Enqueue(&hcommander, &command);
}



void command_callback(Command command) {

	// MENU RESPONSE EVENTS

	if (command.header == 1) {

		memcpy(hmenu.pedals[command.subheader].pedal_raw, command.payload.bytes, RAW_PEDAL_SIZE);
		if (command.subheader == MAX_PEDALS_COUNT - 1) {
			EPD_Init(&hepd1);
			Menu_Render(&hmenu, hepd1.image);
			EPD_Display(&hepd1);
			EPD_Sleep(&hepd1);
		}

	} else if (command.header == 2) {

		// plot signal
		memcpy(hmenu.signal_in, command.payload.bytes, SIGNAL_SIZE);
		memcpy(hmenu.signal_out, command.payload.bytes + SIGNAL_SIZE, SIGNAL_SIZE);
		if (command.subheader == 1) {
			EPD_Init(&hepd1);
			Menu_Render(&hmenu, hepd1.image);
			EPD_Display(&hepd1);
			EPD_Sleep(&hepd1);
		} else {
			EPD_Init(&hepd1);
			Menu_Render(&hmenu, hepd1.image);
			Painter_ToggleCanvas(hepd1.image);
			EPD_Display_Partial(&hepd1);
			Painter_ToggleCanvas(hepd1.image);
			EPD_Display_Partial(&hepd1);
			EPD_Sleep(&hepd1);
		}
	} else if (command.header == 3) {

		// edit pedals
		memcpy(hmenu.pedals[command.subheader].pedal_raw, command.payload.bytes, RAW_PEDAL_SIZE);
		if (command.subheader == MAX_PEDALS_COUNT - 1) {
			EPD_Init(&hepd1);
			Menu_Render(&hmenu, hepd1.image);
			EPD_Display(&hepd1);
			EPD_Sleep(&hepd1);
		}
	}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  	for (uint8_t i = 0; i < 6; i++) HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);

	Commander_Init(&hcommander, &huart3, &hdma_usart3_rx, command_callback);
	Commander_Start(&hcommander);

	Menu_Init(&hmenu);
	hmenu.hcommander = &hcommander;
	uint8_t new_page;

	RE_Init(&hre1, ENC1A_GPIO_Port, ENC1B_GPIO_Port, ENC1A_Pin, ENC1B_Pin, 1);
	RE_Init(&hre2, ENC2A_GPIO_Port, ENC2B_GPIO_Port, ENC2A_Pin, ENC2B_Pin, 1);
	uint8_t enc1_old = 0;
	uint8_t enc2_old = 0;

	EPD_Init(&hepd1);
	EPD_Clear(&hepd1);

	Painter_Clean(hepd1.image);

	char row[24];
	sprintf(row, "G33KY TOAD");
	Painter_WriteString(hepd1.image, row, (296 - 10 * 12) / 2, 30, BOT_LEFT, LARGE);
	sprintf(row, "@");
	Painter_WriteString(hepd1.image, row, 50, (296 - 1 * 18) / 2, TOP_LEFT, LARGE);
	sprintf(row, "BY GIACOMO ASCARI");
	Painter_WriteString(hepd1.image, row, (296 - 17 * 8) / 2, 70, BOT_LEFT, SMALL);
	sprintf(row, "AND EVGENY DEMENEV");
	Painter_WriteString(hepd1.image, row, (296 - 18 * 8) / 2, 90, BOT_LEFT, SMALL);

	EPD_Display(&hepd1);
	EPD_Sleep(&hepd1);

	for (uint8_t i = 0; i < 6; i++) HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_RESET);

	HAL_Delay(50);

	Menu_GoTo(&hmenu, OVERVIEW, 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Commander_Process(&hcommander);

		new_page = hmenu.selected_page;
		for (uint8_t i = 0; i < 6; i++) {
			if (HAL_GPIO_ReadPin(btn_ports[i], btn_pins[i]) == GPIO_PIN_RESET) {
				new_page = i;
			}
		}

		if (new_page != hmenu.selected_page) {
			// MENU PAGE BUTTON EVENT
			Menu_GoTo(&hmenu, new_page, VISIBLE);
			hmenu.tick = HAL_GetTick();
			enc1_old = RE_GetCount(&hre1);
			enc2_old = RE_GetCount(&hre2);
		} else {
			// MENU TIMER EVENTS
			if (hmenu.selected_page == PLOT) {
				if (RE_Process(&hre1)) {
					hmenu.plot_xscale += RE_GetCount(&hre1) - enc1_old;
					enc1_old = RE_GetCount(&hre1);
				}
				if (RE_Process(&hre2)) {
					hmenu.plot_yscale += RE_GetCount(&hre2) - enc2_old;
					enc2_old = RE_GetCount(&hre2);
				}
				if (hmenu.tick + 3000 < HAL_GetTick()) {
					hmenu.tick = HAL_GetTick();
					Menu_GoTo(&hmenu, PLOT, HIDDEN);
				}
			}
		}

		for (uint8_t i = 0; i < 6; i++)  {
			HAL_GPIO_WritePin(led_ports[i], led_pins[i], i == hmenu.selected_page ? GPIO_PIN_SET : GPIO_PIN_RESET);
		}
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD6_Pin|LD5_Pin|LD4_Pin|EPD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin|LD1_Pin|EPD_RST_Pin
                          |EPD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BTN_ENC1_Pin */
  GPIO_InitStruct.Pin = BTN_ENC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_ENC1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC2B_Pin ENC2A_Pin BTN6_Pin BTN5_Pin
                           BTN1_Pin */
  GPIO_InitStruct.Pin = ENC2B_Pin|ENC2A_Pin|BTN6_Pin|BTN5_Pin
                          |BTN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD6_Pin LD5_Pin LD4_Pin EPD_CS_Pin */
  GPIO_InitStruct.Pin = LD6_Pin|LD5_Pin|LD4_Pin|EPD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN4_Pin BTN3_Pin BTN2_Pin BTN_ENC2_Pin
                           EPD_BUSY_Pin ENC1B_Pin ENC1A_Pin */
  GPIO_InitStruct.Pin = BTN4_Pin|BTN3_Pin|BTN2_Pin|BTN_ENC2_Pin
                          |EPD_BUSY_Pin|ENC1B_Pin|ENC1A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin LD1_Pin EPD_RST_Pin
                           EPD_DC_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin|LD1_Pin|EPD_RST_Pin
                          |EPD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
