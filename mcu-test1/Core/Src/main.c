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
#include "fatfs.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "pedalboard_min.h"
//#include "cs43l22.h"
#include "epd_driver.h"
#include "EPD_2_9.h"
#include "painter.h"
#include "rotary_encoder.h"


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
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi3_tx;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

extern ApplicationTypeDef Appli_state;
FATFS myUsbFatFS;
extern char USBHPath[4];

union _ADC_BUFF {
	uint8_t ADC8[32];
	uint16_t ADC16[16];
	int32_t ADC32[8];
} ADC_BUFF;

int32_t BUFF_CONV[4];

union _DAC_BUFF {
	uint8_t DAC8[32];
	uint16_t DAC16[16];
	int32_t DAC32[8];
} DAC_BUFF;

uint16_t display_counter = 0;
float display_array[296];

float RAW_SAMPLES[2];
float PROC_SAMPLES[2];

uint8_t image[EPD_BYTES];

pedalboard_t pedalboard;

RE_HandleTypeDef hre1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void Conv_ADC(uint8_t *buf, int32_t *result) {
	*result = 0xFF000000 * ((buf[1] >> 7) & 1) + (buf[1] << 16) + (buf[0] << 8) + buf[3];
}

uint32_t rxHalfCpltCounter = 0;
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if (hi2s->Instance == SPI2)
	{
		Conv_ADC(&ADC_BUFF.ADC8[0], &BUFF_CONV[0]); // LEFT
		Conv_ADC(&ADC_BUFF.ADC8[4], &BUFF_CONV[1]); // RIGHT
		RAW_SAMPLES[0] = (float)BUFF_CONV[0];
		RAW_SAMPLES[1] = (float)BUFF_CONV[1];
		rxHalfCpltCounter++;
		if (display_counter < 296) {
			display_array[display_counter] = BUFF_CONV[0];
			display_counter++;
		}
	}
}

uint32_t rxCpltCounter = 0;
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if (hi2s->Instance == SPI2)
	{
		Conv_ADC(&ADC_BUFF.ADC8[8], &BUFF_CONV[2]); // LEFT
		Conv_ADC(&ADC_BUFF.ADC8[12], &BUFF_CONV[3]); // RIGHT
		RAW_SAMPLES[0] = (float)BUFF_CONV[2];
		RAW_SAMPLES[1] = (float)BUFF_CONV[3];
		rxCpltCounter++;
		if (display_counter < 296) {
			display_array[display_counter] = BUFF_CONV[2];
			display_counter++;
		}
	}
}

uint32_t txHalfCpltCounter = 0;
/*void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if (hi2s->Instance == SPI3)
	{
		PROC_SAMPLES[0] = pedalboard_process(&pedalboard, (float)RAW_SAMPLES[0]);
		PROC_SAMPLES[0] /= 1.0F;
		PROC_SAMPLES[1] = PROC_SAMPLES[0];
		//PROC_SAMPLES[0] = RAW_SAMPLES[0] * 1;
		//PROC_SAMPLES[0] = wave_gen('s', txHalfCpltCounter, 440.0F) * 10000;
		DAC_BUFF.DAC16[__HAL_DMA_GET_COUNTER(hi2s->hdmatx)] = (int16_t)PROC_SAMPLES[0];
		txHalfCpltCounter++;
	}
}*/

uint32_t txCpltCounter = 0;
/*void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if (hi2s->Instance == SPI3)
	{
		//PROC_SAMPLES[1] = wave_gen('s', txCpltCounter, 440.0F) * 10000;
		DAC_BUFF.DAC16[__HAL_DMA_GET_COUNTER(hi2s->hdmatx)] = (int16_t)PROC_SAMPLES[1];
		txCpltCounter++;
	}
}*/

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

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_I2S2_Init();
	MX_I2S3_Init();
	MX_USB_HOST_Init();
	MX_FATFS_Init();
	MX_SPI1_Init();
	/* USER CODE BEGIN 2 */

	HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, GPIO_PIN_SET);

	pedalboard.active_pedals = 0;
	pedalboard_append(&pedalboard, LPF);

	//CS43_Init(hi2c1, MODE_I2S);
	//CS43_SetVolume(255);
	//CS43_Enable_RightLeft(CS43_RIGHT_LEFT);
	//CS43_Start();

	HAL_I2S_Receive_DMA(&hi2s2, &ADC_BUFF.ADC16[0], 4);
	HAL_I2S_Transmit_DMA(&hi2s3, &DAC_BUFF.DAC16[0], 8);

	EPD_Init();
	EPD_Clear();

	char row[16+1];
	draw_clean(image);
	draw_rectangle(image, 36, 56, 88, 20);

	sprintf(row, "g33ky toad");
	draw_text(image, row, 40, 60);

	sprintf(row, "digital pedal");
	draw_text(image, row, 20, 90);

	EPD_Display(image);
	EPD_Sleep();

	RE_Init(&hre1, EncA_GPIO_Port, EncB_GPIO_Port, EncA_Pin, EncB_Pin);

	HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, GPIO_PIN_RESET);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		MX_USB_HOST_Process();

		/* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

		RE_Process(&hre1);

		int interval = 5;
		if (HAL_GetTick() % (interval * 1000) == 0) {
			HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_SET);
			EPD_Init();
			draw_clean(image);

			sprintf(row, "RxH %d LR", rxHalfCpltCounter / interval);
			draw_text(image, row, 0, 0);
			sprintf(row, "RxC %d LR", rxCpltCounter / interval);
			draw_text(image, row, 0, 20);
			sprintf(row, "TxH %d L", txHalfCpltCounter / interval);
			draw_text(image, row, 0, 40);
			sprintf(row, "TxC %d R", txCpltCounter / interval);
			draw_text(image, row, 0, 60);

			sprintf(row, "Enc %d", hre1.counter);
			draw_text(image, row, 0, 80);

			rxHalfCpltCounter = 0;
			rxCpltCounter = 0;
			txHalfCpltCounter = 0;
			txCpltCounter = 0;

			for (int i = 0; i < 296; i++) {
				float val = (float)display_array[i] / 32768.0F * 10.0F;
				if (val > 63) val = 63;
				if (val < -64) val = -64;
				toggle_single_pixel(image, 64 + (int)val, i);
			}
			display_counter = 0;

			EPD_Display(image);


			EPD_Sleep();
			HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_RESET);
		}

		//GPIO_PinState btn_states[4];
		//btn_states[0] = HAL_GPIO_ReadPin(Btn0_GPIO_Port, Btn0_Pin);
		//btn_states[1] = !HAL_GPIO_ReadPin(Btn1_GPIO_Port, Btn1_Pin);
		//btn_states[2] = !HAL_GPIO_ReadPin(Btn2_GPIO_Port, Btn2_Pin);
		//btn_states[3] = !HAL_GPIO_ReadPin(Btn3_GPIO_Port, Btn3_Pin);

		//HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, btn_states[0] || btn_states[1] || btn_states[2] || btn_states[3]);

		/*if (Appli_state == APPLICATION_READY)
		{
			//HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET);
			if(HAL_GPIO_ReadPin(Btn1_GPIO_Port, Btn1_Pin) == GPIO_PIN_SET) {

				// USE USB
				FIL inFile, outFile;
				FRESULT resRead, resWrite;
				int32_t bytesRead, bytesWritten;
				int32_t inSample, outSample;
				char writeBuffer[16];

				resRead = f_open(&inFile, "in1.txt", FA_READ);
				resWrite = f_open(&outFile, "out1.txt", FA_CREATE_ALWAYS);
				f_close(&outFile);
				resWrite = f_open(&outFile, "out1.txt", FA_WRITE);
				if(resRead == FR_OK && resWrite == FR_OK)
				{
					// succ
					HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET);
					do {
						inSample = readSampleFile(&inFile, &bytesRead);
						outSample = (int32_t)pedalboard_process(&pedalboard, (float)inSample);
						sprintf(writeBuffer, "%ld\n", outSample);
						resWrite = f_write(&outFile, (uint8_t *)writeBuffer, strlen(writeBuffer), &bytesWritten);
					} while(bytesRead > 0);

					HAL_Delay(1000);
					HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_RESET);
				} else {
					// wtf
					HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_SET);
					HAL_Delay(1000);
					HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_RESET);
				}
				f_close(&inFile);
				f_close(&outFile);
			}
		}
		else if (Appli_state == APPLICATION_DISCONNECT || Appli_state == APPLICATION_IDLE)
		{
			HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, GPIO_PIN_RESET);

			if(HAL_GPIO_ReadPin(Btn1_GPIO_Port, Btn1_Pin) == GPIO_PIN_SET) {

				float f_a = 1.F, f_b = 1.F;
				float f_ops[6] = {12.1F, -53.4F, 76.1F, 11.9F, 6.F, 14.F};
				HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET);
				for (int32_t i = 0; i < 44100 * 30; i++) {
					f_b = f_ops[i % 6];
					switch(i % 4) {
					case 0: f_a += f_b; break;
					case 1: f_a -= f_b; break;
					case 2: f_a *= f_b; break;
					case 3: f_a /= f_b; break;
					}
					f_b = pedalboard_process(&pedalboard, f_a);

				}
				HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_RESET);
			}
		}
		else if (Appli_state == APPLICATION_START)
		{
			if (f_mount(&myUsbFatFS, (TCHAR const*)USBHPath, 0) == FR_OK)
			{
				HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_SET);
			}
		}*/
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

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 181;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2S2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S2_Init(void)
{

	/* USER CODE BEGIN I2S2_Init 0 */

	/* USER CODE END I2S2_Init 0 */

	/* USER CODE BEGIN I2S2_Init 1 */

	/* USER CODE END I2S2_Init 1 */
	hi2s2.Instance = SPI2;
	hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
	hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s2.Init.DataFormat = I2S_DATAFORMAT_24B;
	hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_44K;
	hi2s2.Init.CPOL = I2S_CPOL_LOW;
	hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2S2_Init 2 */

	/* USER CODE END I2S2_Init 2 */

}

/**
 * @brief I2S3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S3_Init(void)
{

	/* USER CODE BEGIN I2S3_Init 0 */

	/* USER CODE END I2S3_Init 0 */

	/* USER CODE BEGIN I2S3_Init 1 */

	/* USER CODE END I2S3_Init 1 */
	hi2s3.Instance = SPI3;
	hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
	hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_44K;
	hi2s3.Init.CPOL = I2S_CPOL_LOW;
	hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2S3_Init 2 */

	/* USER CODE END I2S3_Init 2 */

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
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
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
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, EPD_RST_Pin|EPD_DC_Pin|EPD_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(AudioPower_GPIO_Port, AudioPower_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin : OtgPower_Pin */
	GPIO_InitStruct.Pin = OtgPower_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(OtgPower_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : EncB_Pin EncA_Pin */
	GPIO_InitStruct.Pin = EncB_Pin|EncA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : Btn0_Pin EPD_BUSY_Pin */
	GPIO_InitStruct.Pin = Btn0_Pin|EPD_BUSY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : EPD_RST_Pin EPD_DC_Pin EPD_CS_Pin */
	GPIO_InitStruct.Pin = EPD_RST_Pin|EPD_DC_Pin|EPD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : Btn3_Pin Btn2_Pin */
	GPIO_InitStruct.Pin = Btn3_Pin|Btn2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : Btn1_Pin */
	GPIO_InitStruct.Pin = Btn1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Btn1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : Led1_Pin Led2_Pin Led3_Pin Led4_Pin
                           AudioPower_Pin */
	GPIO_InitStruct.Pin = Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin
			|AudioPower_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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

