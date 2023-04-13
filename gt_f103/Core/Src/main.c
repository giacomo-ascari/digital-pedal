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
#include "ili9341.h"
#include "commander.h"
#include "lcd_painter.h"
#include "rencoder.h"
#include "menu.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

Commander_HandleTypeDef hcommander;

Menu_Data menu_data;

uint16_t btn_pins[8] = { BTN1_Pin, BTN2_Pin, BTN3_Pin, BTN4_Pin, BTN5_Pin, BTN6_Pin, BTN_ENC1_Pin, BTN_ENC2_Pin };
GPIO_TypeDef * btn_ports[8] = { BTN1_GPIO_Port, BTN2_GPIO_Port, BTN3_GPIO_Port, BTN4_GPIO_Port, BTN5_GPIO_Port, BTN6_GPIO_Port, BTN_ENC1_GPIO_Port, BTN_ENC2_GPIO_Port };
uint8_t btn_flags[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint16_t led_pins[6] = { LD1_Pin, LD2_Pin, LD3_Pin, LD4_Pin, LD5_Pin, LD6_Pin };
GPIO_TypeDef * led_ports[6] = { LD1_GPIO_Port, LD2_GPIO_Port, LD3_GPIO_Port, LD4_GPIO_Port, LD5_GPIO_Port, LD6_GPIO_Port };

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
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_TIM_Base_Start_IT(&htim2);
	//for (uint8_t i = 0; i < 8; i++) {
	//	if(btn_pins[i] == GPIO_Pin) {
	//		btn_flags[i] = 1;
	//	}
	//}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2) {

		for (uint8_t i = 0; i < 8; i++) {
			if(HAL_GPIO_ReadPin(btn_ports[i], btn_pins[i]) == GPIO_PIN_RESET) {
				btn_flags[i] = 1;
			}
		}
		HAL_TIM_Base_Stop_IT(&htim2);

	} else if (htim == &htim3) {

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
  MX_TIM2_Init();
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

	ILI9341_Init(&hspi1, LCD_CS_GPIO_Port, LCD_CS_Pin, LCD_DC_GPIO_Port, LCD_DC_Pin, LCD_RST_GPIO_Port, LCD_RST_Pin);
	ILI9341_setRotation(4);
	ILI9341_Fill(COLOR_WHITE);

	LCD_Painter_DrawText("G33KY TOAD", (320 - 10 * 12) / 2, 48, COLOR_BLACK, COLOR_WHITE, LARGE);
	LCD_Painter_DrawText("@", (320 - 1 * 18) / 2, 78, COLOR_BLACK, COLOR_WHITE, LARGE);
	LCD_Painter_DrawText("BY GIACOMO ASCARI", (320 - 17 * 10) / 2, 108, COLOR_BLACK, COLOR_WHITE, MEDIUM);
	LCD_Painter_DrawText("AND EVGENY DEMENEV", (320 - 18 * 10) / 2, 124, COLOR_BLACK, COLOR_WHITE, MEDIUM);

	// Now ready
	HAL_Delay(1000);
	//LCD_Painter_EraseText(40, 0, 48, COLOR_WHITE, LARGE);
	//LCD_Painter_EraseText(40, 0, 78, COLOR_WHITE, LARGE);
	//LCD_Painter_EraseText(40, 0, 108, COLOR_WHITE, MEDIUM);
	//LCD_Painter_EraseText(40, 0, 124, COLOR_WHITE, MEDIUM);

	// Menu
	enum Menu_PageType new_page;
	Menu_Init(&menu_data, &hcommander);
	Menu_Sync(&menu_data, GET_PB);
	Menu_Sync(&menu_data, GET_LOAD);

	Menu_GoTo(&menu_data, OVERVIEW, 1);
	Menu_Render(&menu_data);

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

		// assume new_page will be the same as the current one
		new_page = menu_data.page;
		// change in case the buttons did the thing
		for (uint8_t i = 0; i < 6; i++) {
			if (btn_flags[i]) {
				btn_flags[i] = 0;
				new_page = i;
			}
		}

		// change page
		if (Menu_GoTo(&menu_data, new_page, 0)) {
			// if page did change, reset interface
			RE_Reset(&hre1);
			RE_Reset(&hre2);
			btn_flags[6] = 0;
			btn_flags[7] = 0;
			// PAGE CHANGE EVENT (one time)
			if (menu_data.page == OVERVIEW) {
				Menu_Sync(&menu_data, GET_PB);
			} else if (menu_data.page == PLOT) {
				// nothin'
			} else if (menu_data.page == EDIT) {
				Menu_Sync(&menu_data, GET_PB);
			} else if (menu_data.page == MODE) {
				Menu_Sync(&menu_data, GET_PB);
			} else if (menu_data.page == SPECTRUM) {
				// nothin'
			} else if (menu_data.page == FILES) {
				Menu_Sync(&menu_data, GET_FLASH);
			}
		}

		// activate led corresponding to the page
		for (uint8_t i = 0; i < 6; i++)  {
			HAL_GPIO_WritePin(led_ports[i], led_pins[i], i == menu_data.page ? GPIO_PIN_SET : GPIO_PIN_RESET);
		}

		if (RE_ChangeFromLastChange(&hre1)) menu_data.cursor1_changed = 1;
		if (RE_ChangeFromLastChange(&hre2)) menu_data.cursor2_changed = 1;

		// MENU PERIODIC AND USER EVENTS
		if (menu_data.page == OVERVIEW) {

			// periodic event
			if (HAL_GetTick() > menu_data.tick + sync_interval[OVERVIEW]) {
				menu_data.tick = HAL_GetTick();
				Menu_Sync(&menu_data, GET_LOAD);
			}

		} else if (menu_data.page == PLOT) {

			// user events
			menu_data.plot_data.xscale = RE_GetCount(&hre1, 100) + 1;
			menu_data.plot_data.yscale = RE_GetCount(&hre2, 100) + 1;


			// periodic
			if (HAL_GetTick() > menu_data.tick + sync_interval[PLOT]) {
				menu_data.tick = HAL_GetTick();
				Menu_Sync(&menu_data, GET_SIGNALS);
			}

		} else if (menu_data.page == EDIT) {

			//if (RE_ChangeFromLastChange(&hre1)) menu_data.cursor1_changed = 1;
			//if (RE_ChangeFromLastChange(&hre2)) menu_data.cursor2_changed = 1;

			// user event (to and from edit mode)
			if (btn_flags[6]) {
				// if low btn pressed
				btn_flags[6] = 0;
				btn_flags[7] = 0;
				if (!menu_data.edit_data.active) {
					// going to edit mode
					menu_data.edit_data.active = 1;
					menu_data.edit_data.oldvalue1 = hre1.counter;
					menu_data.edit_data.oldvalue2 = hre2.counter;
					menu_data.edit_data.initialvalue1 = hre1.counter;
					menu_data.edit_data.initialvalue2 = hre2.counter;
					menu_data.cursor1_changed = 1;
					menu_data.cursor2_changed = 1;
				} else {
					// exiting edit mode
					menu_data.edit_data.active = 0;
					hre1.counter = menu_data.edit_data.initialvalue1;
					hre2.counter = menu_data.edit_data.initialvalue2;
					Menu_Sync(&menu_data, SET_PB);
					menu_data.cursor1_changed = 1;
					menu_data.cursor2_changed = 1;
					menu_data.pedalboard_changed = 1;
				}
			}

			// user event (if edit mode is active)
			if (menu_data.edit_data.active) {
				// if edit is active
				uint8_t type = menu_data.pedalboard.effects[menu_data.edit_data.index2].effect_formatted.type;
				uint8_t index, _int;
				Pedalboard_GetActiveParamsByType(menu_data.edit_data.index1, type, &_int, &index);
				effect_config_t *conf = &(menu_data.pedalboard.effects[menu_data.edit_data.index2].effect_formatted.config);
				if (_int) {
					// integer parameter
					int_params_manifest_t *manifest = &(Effects_Manifest[type].params_manifest.int_params_manifest[index]);
					int32_t micro_step = 1, macro_step = 1;
					if (manifest->qual == FREQUENCY) {
						micro_step = 1;
						macro_step = 10;
					} else if (manifest->qual == PERCENTAGE) {
						micro_step = (manifest->max - manifest->min) / 100;
						macro_step = (manifest->max - manifest->min) / 10;
					} else if (manifest->qual == VALUE) {
						micro_step = max(1, (manifest->max - manifest->min) / 100);
						macro_step = max(2, (manifest->max - manifest->min) / 10);
					} else if (manifest->qual == TIME) {
						micro_step = 5;
						macro_step = 50;
					}
					conf->int_params[index] += micro_step * (hre1.counter - menu_data.edit_data.oldvalue1);
					conf->int_params[index] += macro_step * (hre2.counter - menu_data.edit_data.oldvalue2);
					if (conf->int_params[index] > manifest->max) conf->int_params[index] = manifest->max;
					if (conf->int_params[index] < manifest->min) conf->int_params[index] = manifest->min;
					menu_data.edit_data.oldvalue1 = hre1.counter;
					menu_data.edit_data.oldvalue2 = hre2.counter;
				} else {
					// float parameter
					float_params_manifest_t *manifest = &(Effects_Manifest[type].params_manifest.float_params_manifest[index]);
					float micro_step = 1.F, macro_step = 1.F;
					if (manifest->qual == FREQUENCY) {
						micro_step = 1;
						macro_step = 10;
					} else if (manifest->qual == PERCENTAGE) {
						micro_step = (manifest->max - manifest->min) / 100;
						macro_step = (manifest->max - manifest->min) / 10;
					} else if (manifest->qual == VALUE) {
						micro_step = (manifest->max - manifest->min) / 100;
						macro_step = (manifest->max - manifest->min) / 10;
					} else if (manifest->qual == TIME) {
						micro_step = 5;
						macro_step = 50;
					}
					conf->float_params[index] += micro_step * (hre1.counter - menu_data.edit_data.oldvalue1);
					conf->float_params[index] += macro_step * (hre2.counter - menu_data.edit_data.oldvalue2);
					if (conf->float_params[index] > manifest->max) conf->float_params[index] = manifest->max;
					if (conf->float_params[index] < manifest->min) conf->float_params[index] = manifest->min;
					menu_data.edit_data.oldvalue1 = hre1.counter;
					menu_data.edit_data.oldvalue2 = hre2.counter;
				}
			} else {
				// update indexes
				menu_data.edit_data.index1 = RE_GetCount(&hre1, Pedalboard_CountActiveParamsByType(menu_data.pedalboard.effects[menu_data.edit_data.index2].effect_formatted.type));
				menu_data.edit_data.index2 = RE_GetCount(&hre2, MAX_EFFECTS_COUNT);
				if (btn_flags[7]) {
					// go to next effect
					btn_flags[7] = 0;
					Pedalboard_SetEffect(
							&(menu_data.pedalboard),
							(menu_data.pedalboard.effects[menu_data.edit_data.index2].effect_formatted.type + 1) % EFFECT_TYPES,
							menu_data.edit_data.index2);
					Menu_Sync(&menu_data, SET_PB);
					menu_data.pedalboard_changed = 1;
				}
			}

		} else if (menu_data.page == MODE) {

			// user event
			menu_data.mode_data.input_selected = RE_GetCount(&hre1, MODE_TYPES);
			menu_data.mode_data.output_selected = RE_GetCount(&hre2, MODE_TYPES);
			//if (RE_ChangeFromLastChange(&hre1)) menu_data.cursor1_changed = 1;
			//if (RE_ChangeFromLastChange(&hre2)) menu_data.cursor2_changed = 1;
			if (btn_flags[6]) {
				btn_flags[6] = 0;
				menu_data.mode_data.input_active = menu_data.mode_data.input_selected;
				Menu_Sync(&menu_data, SET_PB);
				menu_data.text_changed = 1;
			}
			if (btn_flags[7]) {
				btn_flags[7] = 0;
				menu_data.mode_data.output_active = menu_data.mode_data.output_selected;
				Menu_Sync(&menu_data, SET_PB);
				menu_data.text_changed = 1;
			}

		} else if (menu_data.page == SPECTRUM) {

			//if (HAL_GetTick() > menu_data.tick + sync_interval[OVERVIEW]) {
			//	menu_data.tick = HAL_GetTick();
			//	Menu_Sync(&menu_data, GET_SPECTRUM);
			//}

		} else if (menu_data.page == FILES) {

			// user event
			menu_data.files_data.slot_selected = RE_GetCount(&hre1, menu_data.files_data.save_slots);
			menu_data.files_data.action_selected = RE_GetCount(&hre2, 3);
			//if (RE_ChangeFromLastChange(&hre1)) menu_data.cursor1_changed = 1;
			//if (RE_ChangeFromLastChange(&hre2)) menu_data.cursor2_changed = 1;
			if (btn_flags[7]) {
				btn_flags[7] = 0;
				if (menu_data.files_data.action_selected == 0) {
					Menu_Sync(&menu_data, SAVE_FLASH);
				} else if (menu_data.files_data.action_selected == 1) {
					Menu_Sync(&menu_data, LOAD_FLASH);
				} else if (menu_data.files_data.action_selected == 2) {
					Menu_Sync(&menu_data, DEL_FLASH);
				}
				Menu_Sync(&menu_data, GET_FLASH);
				menu_data.text_changed = 1;
			}
		}

		Menu_Render(&menu_data);

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
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, LD6_Pin|LD5_Pin|LD4_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin|LD1_Pin|LCD_RST_Pin
                          |LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BTN_ENC1_Pin */
  GPIO_InitStruct.Pin = BTN_ENC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_ENC1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC2B_Pin ENC2A_Pin */
  GPIO_InitStruct.Pin = ENC2B_Pin|ENC2A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD6_Pin LD5_Pin LD4_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LD6_Pin|LD5_Pin|LD4_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN6_Pin BTN5_Pin BTN1_Pin */
  GPIO_InitStruct.Pin = BTN6_Pin|BTN5_Pin|BTN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN4_Pin BTN3_Pin BTN2_Pin BTN_ENC2_Pin */
  GPIO_InitStruct.Pin = BTN4_Pin|BTN3_Pin|BTN2_Pin|BTN_ENC2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin LD1_Pin LCD_RST_Pin
                           LCD_DC_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin|LD1_Pin|LCD_RST_Pin
                          |LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC1B_Pin ENC1A_Pin */
  GPIO_InitStruct.Pin = ENC1B_Pin|ENC1A_Pin;
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
