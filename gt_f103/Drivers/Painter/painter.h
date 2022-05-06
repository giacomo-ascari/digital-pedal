/*
 * painter.h
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#ifndef PAINTER_PAINTER_H_
#define PAINTER_PAINTER_H_


#include "main.h"

void toggle_single_pixel(uint8_t *image, uint16_t x, uint16_t y);

void toggle_image(uint8_t *image);

void draw_text(uint8_t *image, char *text, uint16_t x, uint16_t y);

void draw_rectangle(uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void draw_clean(uint8_t *image);


#endif /* PAINTER_PAINTER_H_ */
