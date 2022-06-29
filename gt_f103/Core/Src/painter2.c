/*
 * painter2.c
 *
 *  Created on: May 18, 2022
 *      Author: asky
 */

#include "painter2.h"

uint16_t Translate(char c) {
	static uint16_t map[256];
	static uint8_t map_init = 0;

	if (!map_init) {

		// SYMBOLS
		map[32] = 36; // space
		map[33] = 37; // !
		map[34] = 38; // @
		map[35] = 39; // #
		map[36] = 40; // $
		map[37] = 41; // %
		map[38] = 42; // &
		map[39] = 43; // '
		map[40] = 44; // (
		map[41] = 45; // )
		map[42] = 46; // *
		map[43] = 47; // +
		map[44] = 48; // ,
		map[45] = 49; // -
		map[46] = 50; // .
		map[47] = 51; // /
		map[58] = 52; // :
		map[59] = 53; // ;
		map[60] = 54; // <
		map[61] = 55; // =
		map[62] = 56; // >
		map[63] = 57; // ?
		map[91] = 58; // [
		map[92] = 59; /* \ */
		map[93] = 60; // ]
		map[94] = 61; // ^
		map[95] = 62; // _
		map[64] = 63; // @ special

		// NUMBERS
		for (uint8_t i = 0; i < 10; i++) {
			map[48 + i] = 26 + i;
		}

		// LETTERS
		for (uint8_t i = 0; i < 26; i++) {
			map[65 + i] = i; // uppercase
			map[97 + i] = i; // lowercase
		}

		map_init = 1;
	}

	return map[(uint8_t)c];
}

void Painter_TogglePixel(uint8_t *image, uint16_t *x, uint16_t *y, enum Origin origin) {
	uint16_t i;
	uint16_t _x, _y;

	if (origin == TOP_LEFT) {
		_x = *x;
		_y = *y;
	} else if (origin == TOP_RIGHT) {
		_x = CANVAS_WIDTH - *y -1;
		_y = *x;
	} else if (origin == BOT_RIGHT) {
		_x = CANVAS_WIDTH - *x -1;
		_y = CANVAS_HEIGHT - *y -1;
	} else if (origin == BOT_LEFT) {
		_x = *y;
		_y = CANVAS_HEIGHT - *x -1;
	}

	if (_x < CANVAS_WIDTH && _y < CANVAS_HEIGHT) {
		// normalized to be top_left
		i = (_x +  _y * CANVAS_WIDTH) / 8;
		image[i] ^= (0x01 << (7 - _x % 8));
	}
}

void Painter_ToggleRectangle(uint8_t *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, enum Origin origin) {
	for (uint16_t x = x1; x < x2; x++) {
		for (uint16_t y = y1; y < y2; y++) {
			Painter_TogglePixel(image, &x, &y, origin);
		}
	}
}

void Painter_ToggleDottedRectangle(uint8_t *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, enum Origin origin) {
	for (uint16_t x = x1; x < x2; x++) {
		for (uint16_t y = y1; y < y2; y++) {
			if ((x + y) % 2) Painter_TogglePixel(image, &x, &y, origin);
		}
	}
}

void Painter_WriteString(uint8_t *image, char *text, uint16_t x, uint16_t y, enum Origin origin, enum Font font) {

	uint16_t i = 0;
	uint16_t fwidth = 0;
	if (font == SMALL) {
		fwidth = 8;
	} else if (font == LARGE) {
		fwidth = 12;
	}

	while(text[i] > 0) {
		Painter_WriteChar(image, text[i], x + fwidth * i, y, origin, font);
		i++;
	}
}

uint8_t font_8_12[768] = { 0x0, 0x18, 0x3c, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x0, 0x0, 0x0, 0x7e, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x7e, 0x0, 0x0, 0x0, 0x7e, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x60, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x60, 0x60, 0x6e, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x0, 0x0, 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x66, 0x66, 0x6c, 0x6c, 0x78, 0x6c, 0x6c, 0x66, 0x66, 0x0, 0x0, 0x0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x0, 0x0, 0x0, 0x63, 0x63, 0x77, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 0x63, 0x0, 0x0, 0x0, 0x63, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 0x63, 0x63, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x60, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0xc, 0x6, 0x0, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x60, 0x30, 0x18, 0xc, 0x6, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x0, 0x0, 0x0, 0x63, 0x63, 0x63, 0x63, 0x6b, 0x6b, 0x3e, 0x36, 0x36, 0x0, 0x0, 0x0, 0x66, 0x66, 0x34, 0x18, 0x18, 0x2c, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x7e, 0x6, 0x6, 0xc, 0x18, 0x30, 0x60, 0x60, 0x7e, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x6e, 0x6e, 0x66, 0x76, 0x76, 0x66, 0x3c, 0x0, 0x0, 0x0, 0xc, 0x1c, 0x7c, 0xc, 0xc, 0xc, 0xc, 0xc, 0x1e, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x6, 0xc, 0x18, 0x30, 0x60, 0x7e, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x6, 0x1c, 0x6, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x30, 0x36, 0x36, 0x36, 0x36, 0x66, 0x7f, 0x6, 0x6, 0x0, 0x0, 0x0, 0x7e, 0x60, 0x60, 0x60, 0x3c, 0x6, 0x6, 0x6, 0x7c, 0x0, 0x0, 0x0, 0x1c, 0x38, 0x30, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x7e, 0x6, 0xc, 0xc, 0x18, 0x18, 0x30, 0x30, 0x30, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x76, 0x3c, 0x6e, 0x66, 0x66, 0x3c, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3e, 0xc, 0x1c, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x3c, 0x3c, 0x3c, 0x18, 0x18, 0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x66, 0x66, 0xff, 0x66, 0x66, 0x66, 0xff, 0x66, 0x66, 0x0, 0x0, 0x18, 0x18, 0x3c, 0x66, 0x60, 0x38, 0x1c, 0x6, 0x66, 0x3c, 0x18, 0x18, 0x70, 0xd8, 0xda, 0x76, 0xc, 0x18, 0x30, 0x6e, 0x5b, 0x1b, 0xe, 0x0, 0x0, 0x38, 0x6c, 0x6c, 0x38, 0x60, 0x6f, 0x66, 0x66, 0x3b, 0x0, 0x0, 0x0, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0xc, 0x0, 0x30, 0x18, 0x18, 0xc, 0xc, 0xc, 0xc, 0xc, 0x18, 0x18, 0x30, 0x0, 0x0, 0x0, 0x0, 0x36, 0x1c, 0x7f, 0x1c, 0x36, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x18, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x0, 0x0, 0x0, 0x6, 0x6, 0xc, 0xc, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x0, 0x0, 0x0, 0x38, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x38, 0x0, 0x0, 0x0, 0x38, 0x38, 0x18, 0x30, 0x0, 0x6, 0xc, 0x18, 0x30, 0x60, 0x30, 0x18, 0xc, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x7e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x60, 0x30, 0x18, 0xc, 0x6, 0xc, 0x18, 0x30, 0x60, 0x0, 0x0, 0x0, 0x3c, 0x66, 0x66, 0xe, 0x18, 0x18, 0x0, 0x18, 0x18, 0x0, 0x0, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x0, 0x0, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0xc, 0xc, 0x6, 0x6, 0x0, 0x3c, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0x3c, 0x0, 0x18, 0x3c, 0x66, 0xc3, 0x81, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x18, 0x1c, 0x1e, 0x1f, 0x1b, 0x1b, 0x1a, 0x18, 0x78, 0xf8, 0xf8, 0x70, };
uint8_t font_12_18[1728] = { 0x0, 0x0, 0x60, 0xf, 0x1, 0xf8, 0x39, 0xc7, 0xe, 0x60, 0x67, 0xfe, 0x7f, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xf8, 0x7f, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0xe7, 0xfc, 0x7f, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0xe7, 0xfc, 0x7f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xf8, 0x7f, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0xe7, 0xfc, 0x7f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x7, 0xe0, 0x7e, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x7, 0xfe, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x7, 0xe0, 0x7e, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x0, 0x60, 0x6, 0x3e, 0x63, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x67, 0xfe, 0x7f, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x1f, 0x80, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x1, 0xf8, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0xe, 0x61, 0xc6, 0x38, 0x67, 0x7, 0xe0, 0x7e, 0x6, 0x70, 0x63, 0x86, 0x1c, 0x60, 0xe6, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x7, 0xfe, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x67, 0xe, 0x79, 0xe7, 0xfe, 0x6f, 0x66, 0x66, 0x66, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x67, 0x6, 0x78, 0x67, 0xc6, 0x6e, 0x66, 0x76, 0x63, 0xe6, 0x1e, 0x60, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xf8, 0x7f, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0xe7, 0xfc, 0x7f, 0x86, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x38, 0x1, 0xe0, 0xe, 0x0, 0x7, 0xf8, 0x7f, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0xe7, 0xfc, 0x7f, 0x86, 0x38, 0x61, 0xc6, 0xe, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x0, 0x70, 0x3, 0xf8, 0x1f, 0xc0, 0xe, 0x0, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0x9c, 0x1f, 0x80, 0xf0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x66, 0x66, 0x66, 0xf6, 0x7f, 0xe7, 0x9e, 0x70, 0xe6, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x67, 0xe, 0x39, 0xc1, 0xd8, 0xe, 0x0, 0x70, 0x1b, 0x83, 0x9c, 0x70, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x60, 0x66, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0x9c, 0x1f, 0x80, 0xf0, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe0, 0x6, 0x0, 0xe0, 0x1c, 0x3, 0x80, 0x70, 0xe, 0x1, 0xc0, 0x38, 0x7, 0x0, 0x60, 0x7, 0xfe, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0x1e, 0x61, 0xe6, 0x36, 0x63, 0x66, 0x66, 0x66, 0x66, 0xc6, 0x6c, 0x67, 0x86, 0x78, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x3, 0x80, 0xf8, 0xf, 0x80, 0x18, 0x1, 0x80, 0x18, 0x1, 0x80, 0x18, 0x1, 0x80, 0x18, 0x1, 0x80, 0x7e, 0x7, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x0, 0xe1, 0xfc, 0x3f, 0x87, 0x0, 0x60, 0x6, 0x0, 0x60, 0x7, 0xfe, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x0, 0xe0, 0xfc, 0xf, 0xc0, 0xe, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1c, 0x3, 0xc0, 0x7c, 0xe, 0xc1, 0xcc, 0x38, 0xc7, 0xc, 0x60, 0xc6, 0xc, 0x7f, 0xe7, 0xfe, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe6, 0x0, 0x60, 0x6, 0x0, 0x70, 0x3, 0xf8, 0x1f, 0xc0, 0xe, 0x0, 0x60, 0x6, 0x0, 0xe7, 0xfc, 0x7f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x1f, 0x83, 0x80, 0x70, 0x6, 0x0, 0x7f, 0x87, 0xfc, 0x70, 0xe6, 0x6, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe0, 0x6, 0x0, 0x60, 0xe, 0x1, 0xc0, 0x38, 0x7, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x70, 0xe3, 0xfc, 0x1f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x60, 0x67, 0xe, 0x3f, 0xe1, 0xfe, 0x0, 0x60, 0xe, 0x1, 0xc1, 0xf8, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x60, 0x6, 0x0, 0xf0, 0xf, 0x0, 0xf0, 0xf, 0x0, 0x60, 0x6, 0x0, 0x60, 0x0, 0x0, 0x0, 0x6, 0x0, 0xf0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x18, 0x31, 0x83, 0x9c, 0x18, 0xc1, 0x8c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x98, 0x19, 0x81, 0x98, 0x7f, 0xe7, 0xfe, 0x19, 0x81, 0x98, 0x19, 0x81, 0x98, 0x7f, 0xe7, 0xfe, 0x19, 0x81, 0x98, 0x19, 0x80, 0x0, 0x0, 0x0, 0x0, 0x6, 0x0, 0x60, 0x1f, 0x83, 0xfc, 0x76, 0xe6, 0x66, 0x70, 0x3, 0xf8, 0x1f, 0xc0, 0xe, 0x66, 0x67, 0x6e, 0x3f, 0xc1, 0xf8, 0x6, 0x0, 0x60, 0x0, 0x0, 0x0, 0x7c, 0xe, 0xe6, 0xc6, 0x6e, 0xe6, 0x7c, 0xe0, 0x1c, 0x3, 0x80, 0x70, 0xe, 0x1, 0xc0, 0x38, 0x7, 0x3e, 0x67, 0x76, 0x63, 0x67, 0x70, 0x3e, 0x0, 0x0, 0x0, 0x0, 0x1, 0xe0, 0x3f, 0x7, 0x38, 0x61, 0x86, 0x18, 0x73, 0x3, 0xe0, 0x3e, 0x7, 0x76, 0x63, 0xa6, 0x1c, 0x70, 0xe3, 0xf6, 0x1f, 0xa0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xe, 0x0, 0xf0, 0x7, 0x0, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x80, 0x70, 0xe, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xe0, 0x7, 0x0, 0x38, 0x0, 0x0, 0x0, 0x1c, 0x0, 0xe0, 0x7, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x70, 0xe, 0x1, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xc, 0x39, 0xc1, 0xf8, 0xf, 0x7, 0xfe, 0x7f, 0xe0, 0xf0, 0x1f, 0x83, 0x9c, 0x30, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x7, 0xfe, 0x7f, 0xe0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1e, 0x1, 0xe0, 0xc, 0x1, 0xc0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfe, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1e, 0x1, 0xe0, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x0, 0xc0, 0x18, 0x1, 0x80, 0x30, 0x3, 0x0, 0x60, 0x6, 0x0, 0xc0, 0xc, 0x1, 0x80, 0x18, 0x3, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x1, 0xe0, 0x1e, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1e, 0x1, 0xe0, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x1, 0xe0, 0x1e, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1e, 0x1, 0xe0, 0xc, 0x1, 0xc0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x1c, 0x3, 0x80, 0x70, 0xe, 0x1, 0xc0, 0x38, 0x7, 0x0, 0x70, 0x3, 0x80, 0x1c, 0x0, 0xe0, 0x7, 0x0, 0x38, 0x1, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0xfc, 0x7f, 0xc0, 0x0, 0x0, 0x7, 0xfc, 0x7f, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x0, 0x38, 0x1, 0xc0, 0xe, 0x0, 0x70, 0x3, 0x80, 0x1c, 0x1, 0xc0, 0x38, 0x7, 0x0, 0xe0, 0x1c, 0x3, 0x80, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xf8, 0x3f, 0xc7, 0xe, 0x60, 0x66, 0x6, 0x1, 0xe0, 0x3c, 0x7, 0x0, 0x60, 0x6, 0x0, 0x0, 0x6, 0x0, 0xf0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x0, 0xe0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc0, 0xe, 0x0, 0xf0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x30, 0x1, 0x80, 0x18, 0x0, 0xc0, 0xc, 0x0, 0x60, 0x6, 0x0, 0x30, 0x3, 0x0, 0x18, 0x1, 0x80, 0xc, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x0, 0x70, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x3, 0x0, 0x30, 0x7, 0x0, 0xf0, 0x0, 0x0, 0x0, 0x6, 0x0, 0xf0, 0x1f, 0x83, 0x9c, 0x70, 0xee, 0x7, 0xc0, 0x38, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0xff, 0xff, 0xf0, 0x0, 0x0, 0x0, 0x30, 0x7, 0x83, 0xec, 0x6e, 0xe4, 0xee, 0x7e, 0xe3, 0xae, 0x3f, 0x63, 0xf6, 0x3a, 0xe7, 0xee, 0x4e, 0xe6, 0xee, 0x3e, 0xc0, 0x78, 0x3, 0x0, 0x0, };

void Painter_WriteChar(uint8_t *image, char c, uint16_t x, uint16_t y, enum Origin origin, enum Font font) {

	uint16_t selected = Translate(c);
	uint16_t fwidth = 0, fheight = 0;
	uint16_t finalx, finaly;
	uint8_t *font_x_y;

	if (font == SMALL) {
		fwidth = 8;
		fheight = 12;
		font_x_y = font_8_12;
	} else if (font == LARGE) {
		fwidth = 12;
		fheight = 18;
		font_x_y = font_12_18;
	}

	for (uint16_t _y = 0; _y < fheight; _y++) {
		for (uint16_t _x = 0; _x < fwidth; _x++) {
			uint16_t bit_offset = selected * fwidth * fheight;
			uint16_t bit_index = _x + _y * fwidth;
			if ((font_x_y[(bit_offset + bit_index) / 8] >> (7 - bit_index % 8)) & 0x01) {
				finalx = _x + x;
				finaly = _y + y;
				Painter_TogglePixel(image, &finalx, &finaly, origin);
			}
		}
	}

	/*for (uint16_t i = 0; i < fwidth * fheight; i++) {
		uint16_t _x = i % fwidth;
		uint16_t _y = i / fwidth;
		uint16_t index = (selected * fwidth * fheight + _x + _y * fwidth) / 8;
		if ((font_x_y[index] >> (7 - _x % 8)) & 0x01) {
			_x += x;
			_y += y;
			Painter_TogglePixel(image, &_x, &_y, origin);
		}
	}*/
}


void Painter_Clean(uint8_t *image) {
	uint16_t bytes = CANVAS_WIDTH * CANVAS_HEIGHT / 8;
	for (uint16_t i = 0; i < bytes; i++) image[i] = 0xFF;
}

void Painter_ToggleCanvas(uint8_t *image) {
	uint16_t bytes = CANVAS_WIDTH * CANVAS_HEIGHT / 8;
	for (uint16_t i = 0; i < bytes; i++) image[i] = ~image[i];
}
