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
#include "lines.h"
#include "drummachine.h"
#include "menu.h"
#include "w25qxx.h"

//tutorial for CMSIS DSP
//https://stm32f4-discovery.net/2014/10/stm32f4-fft-example/
//https://community.st.com/s/article/configuring-dsp-libraries-on-stm32cubeide

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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi3_tx;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

// VOLUME KNOV
volatile float volume_knob;
volatile float knob_rot;

// TIMER MICROSECOND COUNTER
uint16_t timer_start;
uint16_t timer_stop;
uint16_t timer_elapsed;

// FLASH
#define SAVE_SLOTS 4

// COMMANDER
Commander_HandleTypeDef hcommander;

// LINES
lines_conf_t lines;
// PEDALBOARD
Pedalboard_Handler hpedalboard;
// DRUM MACHINE
drum_conf_t drummachine;

#define SAMPLES_QUANTITY 128 // two halves combined
#define HALF_QUANTITY 64 // which 32 are left and 32 are right

// PLOT
volatile uint8_t plot_xscale = 1;
volatile uint8_t plot_yscale = 1;
volatile uint16_t signal_index = 0;
volatile uint32_t signal_samples = 0;
int8_t signal_in[SIGNAL_SIZE];
int8_t signal_out[SIGNAL_SIZE];

// SPECTRUM
#define FFT_SAMPLES_COUNT 4096
#define FFT_SIZE (FFT_SAMPLES_COUNT / 2)
volatile uint32_t spectrum_index = FFT_SAMPLES_COUNT;
static float32_t spectrum_output[FFT_SIZE];
static float32_t spectrum_input[FFT_SAMPLES_COUNT];
uint32_t spectrum_reduced[PAYLOAD_BYTESIZE];
float32_t frequency;
arm_cfft_radix4_instance_f32 S;

// DAC
//extern AUDIO_DrvTypeDef cs43l22_drv;
int16_t DAC_BUFF[SAMPLES_QUANTITY + SAMPLES_QUANTITY];
// second SAMPLES_QUANTITY is just padding because DMA stuff

// INTERMEDIATE
uint16_t inter_dac_index = 0;
int16_t INTER_DAC_BUFF[HALF_QUANTITY];
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

// USB
extern ApplicationTypeDef Appli_state;
//File IO Variables
FIL myFile;
FRESULT res;
uint32_t byteswritten, bytesread;
char rwtext[100];
uint8_t usb_mounted;
uint8_t usb_ready;
FATFS myUsbFatFS;

// TEST PER AUDIO STREAM
uint8_t dummy_1[4096];
uint8_t dummy_2[4096];
uint32_t start, stop, elapsed;

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
static void MX_TIM6_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM7_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t copy_usb_to_flash3(void)
{
	//Open file for Reading
	if(f_open(&myFile, "SAMPLES.BIN", FA_READ) != FR_OK)
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
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2) {

		static uint32_t samples[5];

		for (uint8_t i = 0; i < 5; i++) {
			HAL_ADC_Start(&hadc1);
			if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
				samples[i] = HAL_ADC_GetValue(&hadc1);
			}
			HAL_ADC_Stop(&hadc1);
		}

		for (uint8_t i = 1; i < 5; i++) {
			uint8_t j = i;
			while (j > 0 && samples[j-1] > samples[j]) {
				uint32_t support = samples[j];
				samples[j] = samples[j-1];
				samples[j-1] = support;
				j -= 1;
			}
		}

		volume_knob = (float)samples[2];

		//float x = - (volume_knob - 4095.3F) / 66.3F;
		knob_rot = log10f(- (volume_knob - 4095.3F) / 66.3F);

		// -135 -> 4095 = B + A 10^(-135)
		// 0    -> 4029 = B + A 10^(0)
		// +135 -> 2979 = B + A 10^(+135)

	}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	memcpy(&(hcommander.in_command), hcommander.uart_rx_buffer, COMMAND_BYTESIZE);
	hcommander.command_to_process = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	memcpy(&(hcommander.in_command), hcommander.uart_rx_buffer + COMMAND_BYTESIZE, COMMAND_BYTESIZE);
	hcommander.command_to_process = 1;
}

void flash_save(uint32_t slot_index, Pedalboard_Handler *hpb) {
	// one sector per save
	if (slot_index < SAVE_SLOTS) {
		W25qxx_WriteSector((uint8_t *)hpb, slot_index, 0, PEDALBOARD_HANDLER_SIZE);
	}
}

uint8_t flash_isempty(uint32_t slot_index) {
	if (slot_index < SAVE_SLOTS) {
		return W25qxx_IsEmptySector(slot_index, 0, PEDALBOARD_HANDLER_SIZE);
	}
	return 0;
}

void flash_load(uint32_t slot_index, Pedalboard_Handler *hpb) {
	// one sector per save
	if (slot_index < SAVE_SLOTS && !flash_isempty(slot_index)) {
		hpedalboard.active = 0;
		W25qxx_ReadSector((uint8_t *)hpb, slot_index, 0, PEDALBOARD_HANDLER_SIZE);
		hpedalboard.active = 1;
	}
}

void flash_delete(uint32_t slot_index) {
	if (slot_index < SAVE_SLOTS) {
		W25qxx_EraseSector(slot_index);
	}
}

void command_callback() {

	Command *in_command = &(hcommander.in_command);
	Command *out_command = &(hcommander.out_command);

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

	out_command->header = in_command->header;
	out_command->param = in_command->param;

	if (in_command->header == GET_PB) {

		// assumes PEDALBOARD_HANDLER_SIZE <= PAYLOAD_BYTESIZE
		memcpy(out_command->payload.bytes, (uint8_t *)&hpedalboard, PEDALBOARD_HANDLER_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == SET_PB) {

		// assumes PEDALBOARD_HANDLER_SIZE <= PAYLOAD_BYTESIZE
		memcpy((uint8_t *)&hpedalboard, in_command->payload.bytes, PEDALBOARD_HANDLER_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == GET_SIGNALS) {

		plot_xscale = in_command->payload.bytes[0] ? in_command->payload.bytes[0] : 1;
		plot_yscale = in_command->payload.bytes[1];
		signal_index = 0;
		while (signal_index < SIGNAL_SIZE);
		memcpy(out_command->payload.bytes, signal_in, SIGNAL_SIZE);
		memcpy(out_command->payload.bytes + SIGNAL_SIZE, signal_out, SIGNAL_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == GET_SPECTRUM) {

		//signal_index = 0;
		//while (signal_index < SIGNAL_SIZE);
		//memcpy(out_command->payload.bytes, signal_in, SIGNAL_SIZE);
		//memcpy(out_command->payload.bytes + SIGNAL_SIZE, signal_out, SIGNAL_SIZE);
		//Commander_Send(&hcommander);

		spectrum_index = 0;
		while (spectrum_index < FFT_SAMPLES_COUNT);
		float32_t maxValue;
		uint32_t maxIndex;
		arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1); //ifftFlag, doBitReverse
		arm_cfft_radix4_f32(&S, spectrum_input);
		arm_cmplx_mag_f32(spectrum_input, spectrum_output, FFT_SIZE);
		arm_max_f32(spectrum_output, FFT_SIZE, &maxValue, &maxIndex);
		//frequency = ((48000/FFT_SIZE) * (maxIndex));

		uint32_t max_value = 0;
		uint16_t window_size = FFT_SIZE / PAYLOAD_BYTESIZE;
		for (uint16_t i = 0; i < PAYLOAD_BYTESIZE; i++) {
			uint32_t value = 0;
			for (uint16_t j = 0; j < window_size; j++) value += spectrum_output[i * window_size + j];
			value /= window_size;
			if (value > max_value) max_value = value;
			spectrum_reduced[i] = value;
		}

		uint32_t scale = (uint32_t)max_value / 256;
		for (uint16_t i = 0; i < PAYLOAD_BYTESIZE; i++) {
			uint32_t value = spectrum_reduced[i];
			value /= scale;
			out_command->payload.bytes[i] = value;
		}

		Commander_Send(&hcommander);

	} else if (in_command->header == GET_LOAD) {

		memcpy(out_command->payload.bytes, (uint8_t *)&timer_elapsed, 2);
		Commander_Send(&hcommander);

	} else if (in_command->header == GET_FLASH) {

		out_command->subheader = SAVE_SLOTS;
		// assuming real payload is always smaller than max capacity
		// assert ( SAVE_SLOTS * (MAX_EFFECTS_COUNT+1) < PAYLOAD_BYTESIZE )
		// first byte per slot in payload mean if it is full or empty
		for (uint8_t i = 0; i < SAVE_SLOTS; i++) {
			Pedalboard_Handler temp;
			if (flash_isempty(i)) {
				out_command->payload.bytes[i*(EFFECT_SLOTS_COUNT+1)+0] = 0;
			} else {
				out_command->payload.bytes[i*(EFFECT_SLOTS_COUNT+1)+0] = 1;
				flash_load(i, &temp);
				for (uint8_t j = 0; j < EFFECT_SLOTS_COUNT; j++) {
					out_command->payload.bytes[i*(EFFECT_SLOTS_COUNT+1)+j+1] = temp.effects[j].type;
				}
			}
		}
		Commander_Send(&hcommander);

	} else if (in_command->header == SAVE_FLASH) {

		flash_save(in_command->subheader, &hpedalboard);
		Commander_Send(&hcommander);

	} else if (in_command->header == LOAD_FLASH) {

		flash_load(in_command->subheader, &hpedalboard);
		Commander_Send(&hcommander);

	} else if (in_command->header == DEL_FLASH) {

		flash_delete(in_command->subheader);
		Commander_Send(&hcommander);

	} else if (in_command->header == GET_DRUMS) {

		memcpy(out_command->payload.bytes, (uint8_t *)&drummachine, DRUMS_CONF_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == SET_DRUMS) {

		memcpy((uint8_t *)&drummachine, in_command->payload.bytes, DRUMS_CONF_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == GET_LINES) {

		memcpy(out_command->payload.bytes, (uint8_t *)&lines, LINES_CONF_SIZE);
		Commander_Send(&hcommander);

	} else if (in_command->header == SET_LINES) {

		memcpy((uint8_t *)&lines, in_command->payload.bytes, LINES_CONF_SIZE);
		Commander_Send(&hcommander);

	}

	// surely faulty command
	//Commander_Pause(&hcommander);
	//HAL_Delay(5000);
	//Commander_Resume(&hcommander);

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

// 0 medium significant byte
// 1 most significant byte
// 2 empty byte
// 3 least significant byte

void DSP(uint8_t * buf, int16_t * out) {

	// RAW (int16) and OUT (int16*) are input and output

	// conversion of terrible adc i2s frame
	uint32_t _rearranged = 0x00000000 + (buf[1]<<24) + (buf[0]<<16) + (buf[3]<<8);

	int32_t rearranged = _rearranged;

	int32_t raw = rearranged >> 8;

	float mid = (float)raw;

	// processing intermediate value
	Pedalboard_Process(&hpedalboard, &mid);
	Drum_Playback(&mid, &drummachine);

	// casting float to 16bits
	*out = (int16_t)(mid / 256);

	// saving for debugging thingies
	if (signal_index < SIGNAL_SIZE && signal_samples % plot_xscale == 0) {
		signal_in[signal_index] = (raw * plot_yscale) >> 16;
		signal_out[signal_index] = (*out * plot_yscale) >> 8;
		signal_index++;
	}
	signal_samples++;

	if (spectrum_index < FFT_SAMPLES_COUNT) {
		//float val = 0;
		//wave_gen(&val, 's', spectrum_index / 2, 440.0);
		spectrum_input[spectrum_index] = (float)raw / 256;
		//spectrum_input[spectrum_index] = val;
		spectrum_input[spectrum_index+1] = 0;
		spectrum_index += 2;
	}
}

// TS: Tip Sleeve
// TRS: Tip Ring Sleeve
// TIP is RIGHT
// RING is LEFT

void Mode_N_DSP(uint8_t * buf_tip, uint8_t * buf_ring, int16_t * out_tip, int16_t * out_ring) {

	if (lines.input_line == TS && lines.output_line == TS) {
		DSP(buf_tip, out_tip);
		*out_ring = 0;

	} else if (lines.input_line == TS && lines.output_line == RS) {
		DSP(buf_tip, out_ring);
		*out_tip = 0;

	} else if (lines.input_line == RS && lines.output_line == TS) {
		DSP(buf_ring, out_tip);
		*out_ring = 0;

	} else if (lines.input_line == RS && lines.output_line == RS) {
		DSP(buf_ring, out_ring);
		*out_tip = 0;

	} else if (lines.input_line == TS && lines.output_line == TRS_B) {
		DSP(buf_tip, out_tip);
		*out_ring = - *out_tip;

	} else if (lines.input_line == TS && lines.output_line == TRS_U) {
		DSP(buf_tip, out_tip);
		*out_ring = *out_tip;

	} else if (lines.input_line == RS && lines.output_line == TRS_B) {
		DSP(buf_ring, out_tip);
		*out_ring = *out_tip;

	} else if (lines.input_line == RS && lines.output_line == TRS_U) {
		DSP(buf_ring, out_tip);
		*out_ring = *out_tip;
	}
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI2) {
		if (inter_dac_index < HALF_QUANTITY) {
			timer_start = __HAL_TIM_GET_COUNTER(&htim6);
			Mode_N_DSP(&ADC_BUFF.ADC8[4], &ADC_BUFF.ADC8[0], &INTER_DAC_BUFF[inter_dac_index + 1], &INTER_DAC_BUFF[inter_dac_index]);
			timer_stop = __HAL_TIM_GET_COUNTER(&htim6);
			timer_elapsed = timer_stop - timer_start;
			//DSP(&ADC_BUFF.ADC8[0], &DSP_BUFF[inter_dac_index]); // ring (left)
			//DSP(&ADC_BUFF.ADC8[4], &DSP_BUFF[inter_dac_index + 1]); // tip (right)
			//INTER_DAC_BUFF[inter_dac_index] = -DSP_BUFF[inter_dac_index + 1];
			inter_dac_index += 2;
		}
	}
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI2) {
		if (inter_dac_index < HALF_QUANTITY) {
			Mode_N_DSP(&ADC_BUFF.ADC8[12], &ADC_BUFF.ADC8[8], &INTER_DAC_BUFF[inter_dac_index + 1], &INTER_DAC_BUFF[inter_dac_index]);
			//DSP(&ADC_BUFF.ADC8[8], &DSP_BUFF[inter_dac_index]); // ring (left)
			//DSP(&ADC_BUFF.ADC8[12], &DSP_BUFF[inter_dac_index + 1]); // tip (right)
			//DSP_BUFF[inter_dac_index] = -DSP_BUFF[inter_dac_index + 1];
			inter_dac_index += 2;
		}

	}
}

void AUDIO_OUT_HalfTransfer_CallBack() {
	for (uint16_t i = 0; i < HALF_QUANTITY; i++) {
		DAC_BUFF[i] = INTER_DAC_BUFF[i];
	}
	inter_dac_index = 0;
}

void AUDIO_OUT_TransferComplete_CallBack() {
	for (uint16_t i = 0; i < HALF_QUANTITY; i++) {
		DAC_BUFF[HALF_QUANTITY + i] = INTER_DAC_BUFF[i];
	}
	inter_dac_index = 0;
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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_I2S3_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_TIM6_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  /* USER CODE BEGIN 2 */

	// LINES
	Lines_Init(&lines);

	// PEDALBOARD
	Pedalboard_Init(&hpedalboard);

	// DRUM MACHINE
	Drum_Init(&drummachine);

	// COMMANDER
	Commander_Init(&hcommander, &huart1, &hdma_usart1_rx, command_callback);
	Commander_Start(&hcommander);

	// DAC
	HAL_GPIO_WritePin(SPKRPower_GPIO_Port, SPKRPower_Pin, RESET);
	cs43l22_Init(AUDIO_I2C_ADDRESS, OUTPUT_DEVICE_HEADPHONE, 200, AUDIO_FREQUENCY_48K);
	cs43l22_Play(AUDIO_I2C_ADDRESS, (uint16_t *)DAC_BUFF, SAMPLES_QUANTITY);
	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)DAC_BUFF, SAMPLES_QUANTITY);
	cs43l22_SetVolume(AUDIO_I2C_ADDRESS, 220);

	// VOLUME KNOB
	//HAL_TIM_Base_Start_IT(&htim2);

	// ADC
	HAL_I2S_Receive_DMA(&hi2s2, ADC_BUFF.ADC16, 4);

	// tests on FLASH
	//	----	Size	Count
	//	Block	65536	1024
	//	Sector	4096	16384
	//	Page	256		262144
	W25qxx_Init();

	// TIMER
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */

		Commander_ProcessIncoming(&hcommander);

		for (uint8_t i = 0; i < 10; i++) {
			uint32_t offset = 26;

			if (W25qxx_IsEmptySector(offset + i, 0, PEDALBOARD_HANDLER_SIZE)) {
				W25qxx_WriteSector((uint8_t *)dummy_1, offset + i, 0, 4096);
			}

			start = __HAL_TIM_GET_COUNTER(&htim7);
			W25qxx_ReadSector((uint8_t *)dummy_2, offset + i, 0, 4096);
			stop = __HAL_TIM_GET_COUNTER(&htim7);
			elapsed = stop - start;

			if (!W25qxx_IsEmptySector(offset + i, 0, PEDALBOARD_HANDLER_SIZE)) {
				W25qxx_EraseSector(offset + i);
			}

		}

		if (Appli_state == APPLICATION_START) {

			if(f_mount(&myUsbFatFS, (TCHAR const*)USBHPath, 0) == FR_OK) {
				usb_mounted = 1;
			}

		} else if (Appli_state == APPLICATION_READY) {

			usb_ready = 1;

			// do the thing

		} else if (Appli_state == APPLICATION_DISCONNECT) {

			usb_mounted = 0;
			usb_ready = 0;

		} // APPLICATION_IDLE

		/* USER CODE END WHILE */

		/*if (samplesCount == FFT_SAMPLES_COUNT) {

			uint32_t ifftFlag = 0; //forward, backward
			uint32_t doBitReverse = 1;
			arm_cfft_radix4_instance_f32 S;
			float32_t maxValue;
			uint32_t maxIndex;

			arm_cfft_radix4_init_f32(&S, FFT_SIZE, ifftFlag, doBitReverse);

			arm_cfft_radix4_f32(&S, Input);

			arm_cmplx_mag_f32(Input, Output, FFT_SIZE);

			arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);

			frequency = ((48000/FFT_SIZE) * (maxIndex));;

			samplesCount = 0;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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
  htim2.Init.Prescaler = 168-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100000;
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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 168-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65536-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 168-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 2, 0);
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USBPower_GPIO_Port, USBPower_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPKRPower_GPIO_Port, SPKRPower_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, FLASH_CS_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USBPower_Pin */
  GPIO_InitStruct.Pin = USBPower_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USBPower_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPKRPower_Pin */
  GPIO_InitStruct.Pin = SPKRPower_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPKRPower_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLASH_CS_Pin LD1_Pin LD2_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin|LD1_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
