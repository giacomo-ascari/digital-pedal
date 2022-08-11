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
#include "mode.h"

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

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

Commander_HandleTypeDef hcommander;

Menu_HandleTypeDef hmenu;

uint16_t btn_pins[8] = { BTN1_Pin, BTN2_Pin, BTN3_Pin, BTN4_Pin, BTN5_Pin, BTN6_Pin, BTN_ENC1_Pin, BTN_ENC2_Pin };
GPIO_TypeDef * btn_ports[8] = { BTN1_GPIO_Port, BTN2_GPIO_Port, BTN3_GPIO_Port, BTN4_GPIO_Port, BTN5_GPIO_Port, BTN6_GPIO_Port, BTN_ENC1_GPIO_Port, BTN_ENC2_GPIO_Port };
uint8_t btn_flags[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint16_t led_pins[6] = { LD1_Pin, LD2_Pin, LD3_Pin, LD4_Pin, LD5_Pin, LD6_Pin };
GPIO_TypeDef * led_ports[6] = { LD1_GPIO_Port, LD2_GPIO_Port, LD3_GPIO_Port, LD4_GPIO_Port, LD5_GPIO_Port, LD6_GPIO_Port };

//float display_array[296];
EPD_HandleTypeDef hepd1;

RE_HandleTypeDef hre1;
RE_HandleTypeDef hre2;

#define EPD_WAIT 3000

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (uint8_t i = 0; i < 8; i++) {
		if(btn_pins[i] == GPIO_Pin) {
			btn_flags[i] = 1;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim3)
	{
		RE_Process(&hre1);
		RE_Process(&hre2);
	}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	memcpy(&(hcommander.in_command), hcommander.uart_rx_buffer, COMMAND_BYTESIZE);
	hcommander.awaiting_response = 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	memcpy(&(hcommander.in_command), hcommander.uart_rx_buffer + COMMAND_BYTESIZE, COMMAND_BYTESIZE);
	hcommander.awaiting_response = 0;
}

void command_callback() {
	// do nothing, F103 is not supposed to answer
	for (uint8_t i = 0; i < 6; i++)
	HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);
	HAL_Delay(500);
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
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */

	// Turning on all LEDs
	for (uint8_t i = 0; i < 6; i++)
		HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);

	// Commander
	Commander_Init(&hcommander, &huart3, &hdma_usart3_rx, command_callback);
	Commander_Start(&hcommander);

	// Rotary encoders
	RE_Init(&hre1, ENC1A_GPIO_Port, ENC1B_GPIO_Port, ENC1A_Pin, ENC1B_Pin);
	RE_Init(&hre2, ENC2A_GPIO_Port, ENC2B_GPIO_Port, ENC2A_Pin, ENC2B_Pin);
	HAL_TIM_Base_Start_IT(&htim3);

	// Display and splash screen
	EPD_Init(&hepd1);
	EPD_Clear(&hepd1);
	Painter_Clean(hepd1.image);
	Painter_WriteString(hepd1.image, "G33KY TOAD", (296 - 10 * 12) / 2, 30, BOT_LEFT, LARGE);
	Painter_WriteString(hepd1.image, "@", 50, (296 - 1 * 18) / 2, TOP_LEFT, LARGE);
	Painter_WriteString(hepd1.image, "BY GIACOMO ASCARI", (296 - 17 * 10) / 2, 70, BOT_LEFT, MEDIUM);
	Painter_WriteString(hepd1.image, "AND EVGENY DEMENEV", (296 - 18 * 10) / 2, 90, BOT_LEFT, MEDIUM);
	EPD_Display(&hepd1);
	EPD_Sleep(&hepd1);

	// Menu
	uint8_t new_page;
	Menu_Init(&hmenu, &hcommander, &hepd1);
	Menu_RetrieveData(&hmenu, FIRST);
	Menu_Render(&hmenu, FULL);

	// Turning off
		for (uint8_t i = 0; i < 6; i++)
			HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_RESET);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		new_page = hmenu.selected_page;
		for (uint8_t i = 0; i < 6; i++) {
			if (btn_flags[i]) {
				btn_flags[i] = 0;
				new_page = i;
			}
		}

		for (uint8_t i = 0; i < 6; i++)  {
			HAL_GPIO_WritePin(led_ports[i], led_pins[i], i == new_page ? GPIO_PIN_SET : GPIO_PIN_RESET);
		}

		if (Menu_GoTo(&hmenu, new_page)) {
			// MENU PAGE BUTTON
			Menu_RetrieveData(&hmenu, FIRST);
			RE_Reset(&hre1);
			RE_Reset(&hre2);
			btn_flags[6] = 0;
			btn_flags[7] = 0;
			Menu_Render(&hmenu, PARTIAL);
		}  else {

			// MENU PERIODIC AND USER EVENTS
			if (hmenu.selected_page == PLOT) {
				hmenu.plot_xscale = RE_GetCount(&hre1) + 1;
				hmenu.plot_yscale = RE_GetCount(&hre2) + 1;
				if (hmenu.tick + 3000 < HAL_GetTick()) {
					hmenu.tick = HAL_GetTick();
					Menu_RetrieveData(&hmenu, PERIODIC);
					Menu_Render(&hmenu, PARTIAL);
				}

			} else if (hmenu.selected_page == EDIT) {
				if (btn_flags[6]) {
					// if low btn pressed
					btn_flags[6] = 0;
					if (!hmenu.edit_active) {
						// going to edit mode
						hmenu.edit_active = 1;
						hmenu.edit_cursor = 0;
					} else {
						// exiting edit mode
						hmenu.edit_active = 0;
					}
					RE_Reset(&hre1);
					Menu_RetrieveData(&hmenu, USER);
				}
				if (hmenu.edit_active) {
					// if edit is active
					if (RE_GetCount(&hre1) != 0) {
						// if something happened
						uint8_t type = hmenu.effects[hmenu.edit_index2].effect_formatted.type;
						effect_config_t *conf = &(hmenu.effects[hmenu.edit_index2].effect_formatted.config);
						if (hmenu.edit_index1 < INT_PARAMS_TYPES) {
							//if (Effects_Manifest[type].params_manifest.int_params_manifest[])
							//conf->int_params[hmenu.edit_index1] +=
						} else if (hmenu.edit_index1 >= INT_PARAMS_TYPES && hmenu.edit_index1 < INT_PARAMS_TYPES + FLOAT_PARAMS_TYPES) {

						}
					}
				} else {
					hmenu.edit_index1 = RE_GetCount(&hre1);
					if (RE_GetCount(&hre2) >= 0) hmenu.edit_index2 = RE_GetCount(&hre2) % MAX_EFFECTS_COUNT;
					else hmenu.edit_index2 = MAX_EFFECTS_COUNT - (-RE_GetCount(&hre2) % MAX_EFFECTS_COUNT);
				}

				if (btn_flags[7]) {
					btn_flags[7] = 0;
					hmenu.effects[hmenu.edit_index2].effect_formatted.type += 1;
					hmenu.effects[hmenu.edit_index2].effect_formatted.type %= EFFECT_TYPES;
					Menu_RetrieveData(&hmenu, USER);
				}

				Menu_Render(&hmenu, PARTIAL);

			} else if (hmenu.selected_page == MODE) {
				hmenu.mode_selected = RE_GetCount(&hre2) % MODE_TYPES;
				if (btn_flags[7]) {
					btn_flags[7] = 0;
					hmenu.mode_active = hmenu.mode_selected;
					Menu_RetrieveData(&hmenu, USER);
				} else {
					Menu_Render(&hmenu, PARTIAL);
				}
			} else if (hmenu.selected_page == TUNER) {
				Menu_RetrieveData(&hmenu, PERIODIC);
				Menu_Render(&hmenu, PARTIAL);
			} else if (hmenu.selected_page == FILES) {
				hmenu.usb_selected = RE_GetCount(&hre2) % 2;
				if (btn_flags[7]) {
					btn_flags[7] = 0;
					if (hmenu.usb_ready) {
						Menu_RetrieveData(&hmenu, USER);
					}
				} else {
					Menu_RetrieveData(&hmenu, PERIODIC);
				}
				Menu_Render(&hmenu, PARTIAL);
			}
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
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 72;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 10000-1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

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
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_ENC1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : ENC2B_Pin ENC2A_Pin */
	GPIO_InitStruct.Pin = ENC2B_Pin|ENC2A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LD6_Pin LD5_Pin LD4_Pin EPD_CS_Pin */
	GPIO_InitStruct.Pin = LD6_Pin|LD5_Pin|LD4_Pin|EPD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : BTN6_Pin BTN5_Pin BTN1_Pin */
	GPIO_InitStruct.Pin = BTN6_Pin|BTN5_Pin|BTN1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : BTN4_Pin BTN3_Pin BTN2_Pin BTN_ENC2_Pin */
	GPIO_InitStruct.Pin = BTN4_Pin|BTN3_Pin|BTN2_Pin|BTN_ENC2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
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

	/*Configure GPIO pins : EPD_BUSY_Pin ENC1B_Pin ENC1A_Pin */
	GPIO_InitStruct.Pin = EPD_BUSY_Pin|ENC1B_Pin|ENC1A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
