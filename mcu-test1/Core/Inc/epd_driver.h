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

#define INIT_STATE 0
#define CLEAR_STATE 1
#define DISPLAY_STATE 2
#define DISPLAY_PARTIAL_STATE 3
#define SLEEP_STATE 4

void EPD_Init(void);
void EPD_Clear(void);
void EPD_Display(uint8_t *image);
void EPD_Display_Partial(uint8_t *image);
void EPD_Sleep(void);

#endif
