/*
 * lcd_painter.h
 *
 *  Created on: 27 mar 2023
 *      Author: Giacomo
 */

#ifndef INC_LCD_PAINTER_H_
#define INC_LCD_PAINTER_H_

#include "ili9341.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum Font {
	SMALL,
	MEDIUM,
	LARGE
};

void LCD_Painter_DrawChar(char c, uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, enum Font font);

void LCD_Painter_DrawText(const char text[], uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, enum Font font);

void LCD_Painter_EraseText(uint16_t chars, uint16_t x, uint16_t y, uint16_t bg_color, enum Font font);

#endif /* INC_LCD_PAINTER_H_ */
