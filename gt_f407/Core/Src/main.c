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
#include "commander.h"
#include "AUDIO.h"
#include "pedalboard.h"

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

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

// USB
FATFS usbFatFS;
extern char USBHPath[4];
extern ApplicationTypeDef Appli_state;

// COMMANDER
Commander_HandleTypeDef hcommander;

// PEDALBOARD
Pedalboard_Handler hpedalboard;

// DAC
extern AUDIO_DrvTypeDef cs43l22_drv;
int16_t DAC_BUFF[64];

// INTERMEDIATE
int16_t DSP_BUFF[4];
// 0: 1st right
// 1: 1st left
// 2: 2nd right
// 3: 2nd left

// ADC EVG
union _ADC_BUFF {
	int8_t _ADC8[16];
	uint8_t ADC8[16];
	uint16_t ADC16[8];
	int32_t ADC32[4];
} ADC_BUFF;
int32_t BUFF_CONV[4];

uint32_t debug_samples[128];
int8_t samples_count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2S3_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
	//states[0] = states[0] == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void Conv_ADC(uint8_t * buf, uint32_t *res){
	*res = 0x00000000 + (buf[1]<<24) + (buf[0]<<16) + (buf[3]<<8);
}

// 0 medium significant byte
// 1 most significant byte
// 2 empty byte
// 3 least significant byte

void ADC_Process(uint32_t *_raw, int16_t *out) {
	int16_t raw = (*_raw >> 16);
	float mid = (float)raw;
	Pedalboard_Process(&hpedalboard, &mid);
	*out = (int16_t) mid;
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI2) {
		//uint32_t left;
		uint32_t right;

		//Conv_ADC(&ADC_BUFF.ADC8[0], &left);
		Conv_ADC(&ADC_BUFF.ADC8[4], &right);

		//DSP_BUFF[0] = (left >> 16);
		ADC_Process(&right, &DSP_BUFF[1]);
		//DSP_BUFF[1] = (right >> 16);
		DSP_BUFF[0] = DSP_BUFF[1];
	}
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI2) {
		//uint32_t left;
		uint32_t right;

		//Conv_ADC(&ADC_BUFF.ADC8[0], &left);
		Conv_ADC(&ADC_BUFF.ADC8[4], &right);

		//DSP_BUFF[2] = (left >> 16);
		ADC_Process(&right, &DSP_BUFF[3]);
		//DSP_BUFF[3] = (right >> 16);
		DSP_BUFF[2] = DSP_BUFF[3];
	}
}

void AUDIO_OUT_HalfTransfer_CallBack() {
	DAC_BUFF[0] = DSP_BUFF[0];
	DAC_BUFF[1] = DSP_BUFF[1];
}

void AUDIO_OUT_TransferComplete_CallBack() {
	DAC_BUFF[2] = DSP_BUFF[2];
	DAC_BUFF[3] = DSP_BUFF[3];
}

/*
bool UsbTest_Read(void)
{
	//Open file for reading
	if(f_open(&myFile, "TEST2.TXT", FA_READ) != FR_OK)
	{
		return 0;
	}

	//Read text from files until NULL
	for(uint8_t i=0; i<100; i++)
	{
		res = f_read(&myFile, (uint8_t*)&rwtext[i], 1, &bytesread);
		if(rwtext[i] == 0x00) // NULL string
		{
			bytesread = i;
			break;
		}
	}
	//Reading error handling
	if(bytesread==0) return 0;

	//Close file
	f_close(&myFile);
	return 1;  // success

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
	MX_USART1_UART_Init();
	MX_FATFS_Init();
	MX_USB_HOST_Init();
	MX_I2S3_Init();
	MX_I2C1_Init();
	MX_I2S2_Init();
	/* USER CODE BEGIN 2 */

	// COMMANDER
	Commander_Init(&hcommander, &huart1, &hdma_usart1_rx, command_callback);
	Commander_Start(&hcommander);

	// PEDALBOARD
	Pedalboard_Init(&hpedalboard);
	Pedalboard_Append(&hpedalboard, LPF);


	// DAC
	HAL_GPIO_WritePin(SPKRPower_GPIO_Port, SPKRPower_Pin, RESET);
	cs43l22_Init(0x94, OUTPUT_DEVICE_HEADPHONE, 200, AUDIO_FREQUENCY_48K);
	cs43l22_Play(AUDIO_I2C_ADDRESS, (uint16_t *)DAC_BUFF, 4);
	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)DAC_BUFF, 4);
	cs43l22_SetVolume(AUDIO_I2C_ADDRESS, 230);

	// ADC
	HAL_I2S_Receive_DMA(&hi2s2, ADC_BUFF.ADC16, 4);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		MX_USB_HOST_Process();

		/* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

		if (Appli_state == APPLICATION_START) {
			if(f_mount(&usbFatFS, (TCHAR const*)USBHPath, 0) == FR_OK)
			{
				HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
			}
		} else if (Appli_state == APPLICATION_DISCONNECT || Appli_state == APPLICATION_IDLE) {
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		} else if (Appli_state == APPLICATION_READY) {
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		} else {
			//aaa
		}

		HAL_Delay(7000);

		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

		Command command;
		command.header = 0x05;
		for(uint8_t i = 0; i < 128; i++) {
			command.payload.bytes[i] = debug_samples[i];
		}
		Commander_Send(&hcommander, &command);
		samples_count = 0;

		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

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
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
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
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 196;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
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
	hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
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
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
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
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	/* DMA2_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

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
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SPKRPower_GPIO_Port, SPKRPower_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, LD1_Pin|LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : SPKRPower_Pin */
	GPIO_InitStruct.Pin = SPKRPower_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPKRPower_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : OtgPower_Pin */
	GPIO_InitStruct.Pin = OtgPower_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(OtgPower_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD1_Pin LD2_Pin */
	GPIO_InitStruct.Pin = LD1_Pin|LD2_Pin;
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
