/*
 * EPD_2_9.h
 *
 *  Created on: Apr 10, 2022
 *      Author: asky
 */

#ifndef INC_EPD_2_9_H_
#define INC_EPD_2_9_H_

#include "main.h"
#include "stm32f4xx_hal.h"

#define EPD_WIDTH 128
#define EPD_HEIGHT 296
#define EPD_PIXELS 37888
#define EPD_BYTES 4736

#define FONT_WIDTH 8
#define FONT_HEIGHT 12

#define EPD_INIT_STATE 0
#define EPD_CLEAR_STATE 1
#define EPD_DISPLAY_STATE 2
#define EPD_DISPLAY_PARTIAL_STATE 3
#define EPD_SLEEP_STATE 4
#define EPD_IDLE_STATE 5

typedef struct _EPD_HandleTypeDef {
	uint16_t state;
	uint16_t phase;
	uint8_t image[EPD_BYTES];
} EPD_HandleTypeDef;

void EPD_Process(EPD_HandleTypeDef *hepd);

#endif /* INC_EPD_2_9_H_ */
