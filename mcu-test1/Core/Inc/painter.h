/*
 * painter.h
 *
 *  Created on: Mar 7, 2022
 *      Author: giaco
 */

#ifndef INC_PAINTER_H_
#define INC_PAINTER_H_

#include "main.h"

void draw_text(uint8_t *image, char *text, uint16_t x, uint16_t y);

void draw_rectangle(uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void draw_float_number(uint8_t *image, float val, uint16_t x, uint16_t y);

void draw_clean(uint8_t *image);

#endif /* INC_PAINTER_H_ */
