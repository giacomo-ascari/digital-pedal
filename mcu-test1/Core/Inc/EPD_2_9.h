#ifndef __EPD_H_
#define __EPD_H_

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
	uint8_t state;
	uint8_t phase;
	uint8_t image[EPD_BYTES];
} EPD_HandleTypeDef;

void EPD_Init(EPD_HandleTypeDef *hepd);
void EPD_Clear(EPD_HandleTypeDef *hepd);
void EPD_Display(EPD_HandleTypeDef *hepd);
void EPD_Display_Partial(EPD_HandleTypeDef *hepd);
void EPD_Sleep(EPD_HandleTypeDef *hepd);

void EPD_AsyncState(EPD_HandleTypeDef *hepd, uint8_t state);
void EPD_AsyncProcess(EPD_HandleTypeDef *hepd);

#endif
