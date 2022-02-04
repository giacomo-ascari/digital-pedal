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
#include "pedalboard_min.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

extern ApplicationTypeDef Appli_state;
FATFS myUsbFatFS;
extern char USBHPath[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
/*u_int16_t UsbTest_Write(void)
{
	//Open/Create file for Writing
	if(f_open(&myFile, "TEST.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		return 0; //error
	}
	else
	{
		//Set text string to write to file
		sprintf(rwtext, "Hello world from Mohamed Yaqoob!");
		//write to file
		res = f_write(&myFile, (uint8_t *)rwText, strlen(rwText), &bytesWritten);
		if((bytesWritten == 0) || (res != FR_OK))
		{
			return 0; //error
		}
	}

	//Close file
	f_close(&myFile);
	return 1; //success
}*/

/*bool UsbTest_Read(void)
{
	//Open file for Reading
	if(f_open(&myFile, "TEST.TXT", FA_READ) != FR_OK)
	{
		return 0; //error
	}
	else
	{
		//Read text from files until NULL
		for(uint8_t i=0; i<100; i++)
		{
			res = f_read(&myFile, (uint8_t*)&rwtext[i], 1, &bytesread);
			if(rwtext[i] == 0x00)
			{
				bytesread = i;
				break;
			}
		}
		//Reading error handling
		if(bytesread==0) return 0;
	}

	//Close file
	f_close(&myFile);
	return 1; //success
}*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
	MX_FATFS_Init();
	MX_USB_HOST_Init();
	/* USER CODE BEGIN 2 */
	pedalboard_t pedalboard;
	pedalboard_append(&pedalboard, BITCRUSHER_RS);
	pedalboard_append(&pedalboard, LPF);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		MX_USB_HOST_Process();

		/* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

		if (Appli_state == APPLICATION_READY)
		{
			//HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET);
			if(HAL_GPIO_ReadPin(Btn1_GPIO_Port, Btn1_Pin) == GPIO_PIN_SET) {

				FIL inFile, outFile;
				FRESULT resRead, resWrite;
				int32_t bytesRead, bytesWritten;
				int32_t inSample, outSample, tmp;
				int32_t count = 0;
				char readBuffer[16], writeBuffer[16];
				char sampleBuffer[16];

				resRead = f_open(&inFile, "in1.txt", FA_READ);
				resWrite = f_open(&outFile, "out1.txt", FA_CREATE_ALWAYS);
				f_close(&outFile);
				resWrite = f_open(&outFile, "out1.txt", FA_WRITE);
				if(resRead == FR_OK && resWrite == FR_OK)
				{
					// succ
					HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET);
					do {

						resRead = f_read(&inFile, &readBuffer, 1, &bytesRead);
						if (bytesRead) {
							sampleBuffer[count] = readBuffer[0];
							if (sampleBuffer[count] == '\n') {
								sampleBuffer[count] == '\0';
								tmp = sscanf(sampleBuffer, "%d", &inSample);
								outSample = (int32_t)pedalboard_process(&pedalboard, (float)inSample);
								sprintf(writeBuffer, "%d\n", outSample);
								resWrite = f_write(&outFile, (uint8_t *)writeBuffer, strlen(writeBuffer), &bytesWritten);
								count = 0;
							} else {
								count++;
							}
						}

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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
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
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(OtgPower_GPIO_Port, OtgPower_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : OtgPower_Pin */
	GPIO_InitStruct.Pin = OtgPower_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(OtgPower_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : Btn1_Pin */
	GPIO_InitStruct.Pin = Btn1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(Btn1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : Led1_Pin Led2_Pin Led3_Pin Led4_Pin */
	GPIO_InitStruct.Pin = Led1_Pin|Led2_Pin|Led3_Pin|Led4_Pin;
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

