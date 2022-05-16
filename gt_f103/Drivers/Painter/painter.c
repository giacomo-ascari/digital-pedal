/*
 * painter.c
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */


#include "painter.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 12

#define CANVAS_WIDTH 128
#define CANVAS_HEIGHT 296

uint8_t font[516] = { 0x0, 0x18, 0x3C, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x78, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x0, 0x0, 0x0, 0x7E, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x7E, 0x0, 0x0, 0x0, 0x7E, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x60, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x60, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x0, 0x0, 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x66, 0x66, 0x6C, 0x6C, 0x78, 0x6C, 0x6C, 0x66, 0x66, 0x0, 0x0, 0x0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x0, 0x0, 0x0, 0x63, 0x63, 0x77, 0x7F, 0x6B, 0x6B, 0x63, 0x63, 0x63, 0x0, 0x0, 0x0, 0x63, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63, 0x63, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x60, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0xC, 0x6, 0x0, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x64, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x60, 0x30, 0x18, 0xC, 0x6, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x0, 0x0, 0x0, 0x63, 0x63, 0x63, 0x63, 0x6B, 0x6B, 0x36, 0x36, 0x36, 0x0, 0x0, 0x0, 0x66, 0x66, 0x34, 0x18, 0x18, 0x2C, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x7E, 0x6, 0x6, 0xC, 0x18, 0x30, 0x60, 0x60, 0x7E, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x6E, 0x6E, 0x66, 0x76, 0x76, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x6, 0xE, 0x3E, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x6, 0xC, 0x18, 0x30, 0x60, 0x7E, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x6, 0x1C, 0x6, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x30, 0x30, 0x36, 0x36, 0x36, 0x66, 0x7F, 0x6, 0x6, 0x0, 0x0, 0x0, 0x7E, 0x60, 0x60, 0x60, 0x7C, 0x6, 0x6, 0xC, 0x78, 0x0, 0x0, 0x0, 0x1C, 0x38, 0x30, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x7E, 0x6, 0xC, 0xC, 0x18, 0x18, 0x30, 0x30, 0x30, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x76, 0x3C, 0x6E, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3E, 0xC, 0x1C, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x18, 0x30, 0x0, 0x0, 0x0, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7E, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0xC, 0x18, 0x30, 0x60, 0x30, 0x18, 0xC, 0x6, 0x0, 0x0, 0x0, 0x60, 0x30, 0x18, 0xC, 0x6, 0xC, 0x18, 0x30, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

void map_as_bottom_left_corner(uint16_t *x, uint16_t *y) {
	uint16_t _x = *x;
	*x = *y;
	*y = CANVAS_HEIGHT - _x;
}

uint8_t read_font_pixel(uint8_t *image, uint16_t x, uint16_t y) {
	uint16_t i = (x +  y * FONT_WIDTH) / 8;
	uint8_t pixel_group = image[i];
	return ((pixel_group >> (7 - x % 8)) & 0x01);
}

void toggle_single_pixel(uint8_t *image, uint16_t x, uint16_t y) {
	uint16_t i;
	if (x < CANVAS_WIDTH && y < CANVAS_HEIGHT) {
		i = (x +  y * CANVAS_WIDTH) / 8;
		image[i] ^= (0x01 << (7 - x % 8));
	}
}

void toggle_image(uint8_t *image) {
	for (uint16_t i = 0; i < CANVAS_WIDTH * CANVAS_HEIGHT / 8; i++) {
		image[i] = ~image[i];
	}
}

void draw_rectangle(uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	for (uint16_t i = 0; i < h; i++) {
		for (uint16_t j = 0; j < w; j++) {
			toggle_single_pixel(image, x + j, y + i);
		}
	}
}

void draw_char(uint8_t *image, uint16_t x, uint16_t y, uint16_t c) {
	uint16_t font_offset = c * FONT_HEIGHT;
	for (uint16_t i = 0; i < FONT_HEIGHT; i++) {
		for (uint16_t j = 0; j < FONT_WIDTH; j++) {
			if (read_font_pixel(font, j, i + font_offset)) {
				toggle_single_pixel(image, x + j, y + i);
			}
		}
	}
}

uint16_t translate(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= 'a' && c <= 'z') {
		return c - 'a';
	} else if (c >= '0' && c <= '9') {
		return c - '0' + 26;
	} else if (c == '.') {
		return 36;
	} else if (c == ',') {
		return 37;
	} else if (c == '+') {
		return 38;
	} else if (c == '-') {
		return 39;
	} else if (c == '<') {
		return 40;
	} else if (c == '>') {
		return 41;
	} else {
		return 42; // ' '
	}
}

void draw_text(uint8_t *image, char *text, uint16_t x, uint16_t y) {
	uint16_t i = 0;
	while(text[i] > 0) {
		draw_char(image, x + FONT_WIDTH * i, y, translate(text[i]));
		i++;
	}
}

void draw_clean(uint8_t *image) {
	uint16_t bytes = CANVAS_WIDTH * CANVAS_HEIGHT / 8;
	for (uint16_t i = 0; i < bytes; i++) image[i] = 0xFF;
}
