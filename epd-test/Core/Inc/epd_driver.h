#ifndef __EPD_H_
#define __EPD_H_

#include "stm32f4xx_hal.h"

#define EPD_WIDTH 128
#define EPD_HEIGHT 296

void EPD_Init(void);
void EPD_Clear(void);
void EPD_Display(uint8_t *image);
void EPD_Display_Base(uint8_t *image);
void EPD_Display_Partial(uint8_t *image);
void EPD_Sleep(void);

#endif

/*

#define EPD_RST_PIN     RST_GPIO_Port, RST_Pin
#define EPD_DC_PIN      DC_GPIO_Port, DC_Pin
#define EPD_CS_PIN      SPI_CS_GPIO_Port, SPI_CS_Pin
#define EPD_BUSY_PIN    BUSY_GPIO_Port, BUSY_Pin


#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)


#define DEV_Delay_ms(__xms) HAL_Delay(__xms);

void DEV_SPI_WriteByte(UBYTE value);

int DEV_Module_Init(void);
void DEV_Module_Exit(void);


*/

/*
#include "DEV_Config.h"
#include "stm32f1xx_hal_spi.h"

extern SPI_HandleTypeDef hspi1;
void DEV_SPI_WriteByte(UBYTE value)
{
    HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
}

int DEV_Module_Init(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_RST_PIN, 1);
		return 0;
}

void DEV_Module_Exit(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);

    //close 5V
    DEV_Digital_Write(EPD_RST_PIN, 0);
}

*/
