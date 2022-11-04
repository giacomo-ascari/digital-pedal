/*
 * painter2.h
 *
 *  Created on: May 18, 2022
 *      Author: asky
 */

#ifndef PAINTER2_PAINTER2_H_
#define PAINTER2_PAINTER2_H_

#include "stm32f1xx_hal.h"

#define CANVAS_WIDTH 128
#define CANVAS_HEIGHT 296


// remember: origin is a combination of
// the 0-corner and the screen orientation
enum Origin {
	TOP_LEFT,
	TOP_RIGHT,
	BOT_LEFT,
	BOT_RIGHT
};

enum Font {
	SMALL,
	MEDIUM,
	LARGE
};

// TOP LEFT
// 0-------X
// |       |
// |       |
// |       |
// |       |
// |       |
// |       |
// Y-------+
//   |||||

// TOP RIGHT
// Y-------0
// |       |
// |       |
// |       |
// |       |
// |       |
// |       |
// +-------X
//   |||||

// BOT RIGHT
// +-------Y
// |       |
// |       |
// |       |
// |       |
// |       |
// |       |
// X-------0
//   |||||

// BOT LEFT
// X-------+
// |       |
// |       |
// |       |
// |       |
// |       |
// |       |
// 0-------Y
//   |||||



void Painter_TogglePixel(uint8_t *image, uint16_t *x, uint16_t *y, enum Origin origin);

void Painter_ToggleRectangle(uint8_t *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, enum Origin origin);

void Painter_ToggleRectangleRelative(uint8_t *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, enum Origin origin);

void Painter_ToggleDottedRectangle(uint8_t *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, enum Origin origin);

void Painter_WriteString(uint8_t *image, char *text, uint16_t x, uint16_t y, enum Origin origin, enum Font font);

void Painter_WriteChar(uint8_t *image, char c, uint16_t x, uint16_t y, enum Origin origin, enum Font font);

void Painter_Clean(uint8_t *image);

void Painter_ToggleCanvas(uint8_t *image);

#endif /* PAINTER2_PAINTER2_H_ */
