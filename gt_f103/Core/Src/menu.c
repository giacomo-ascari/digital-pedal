/*
 * menu.c
 *
 *  Created on: Jun 14, 2022
 *      Author: asky
 */

#include "menu.h"
#include "painter2.h"
#include <string.h>
#include <stdio.h>

void Menu_Init(Menu_HandleTypeDef *hm) {
	hm->selected_page = OVERVIEW;
}

void Menu_ParseSignal(Menu_HandleTypeDef *hm, uint8_t *data) {

	uint8_t *buffer;

	if (data[255] == 0) buffer = hm->signal_in;
	else buffer = hm->signal_out;

	for (uint16_t i = 0; i < SIGNAL_SIZE; i++) {
		buffer[i] = data[i];
	}
}

void Menu_Render(Menu_HandleTypeDef *hm, uint8_t *image) {
	char row[16+1];

	Painter_Clean(image);

	if (hm->selected_page == OVERVIEW) {

		sprintf(row, "overview");
		Painter_WriteString(image, row, 10, 10, BOT_LEFT, LARGE);
		uint8_t active_pedals = 0;

		for (uint16_t i = 0; i < MAX_PEDALS_COUNT; i++) {
			enum pedal_types t = hm->pedals[i].pedal_formatted.type;
			uint16_t width = CANVAS_HEIGHT / MAX_PEDALS_COUNT;
			if (t == BYPASS) {
				Painter_ToggleRectangle(image, width * i + 4, 35, width * (i+1) - 4, 95, BOT_LEFT);
				Painter_ToggleRectangle(image, width * i + 5, 36, width * (i+1) - 5, 94, BOT_LEFT);
			} else {
				active_pedals++;
				Painter_ToggleRectangle(image, width * i + 4, 35, width * (i+1) - 4, 95, BOT_LEFT);
				switch(t) {
				case AMPLIFIER: sprintf(row, "amp"); break;
				case BITCRUSHER_RS: sprintf(row, "bit"); break;
				case FUZZ: sprintf(row, "fzz"); break;
				case LPF: sprintf(row, "lpf"); break;
				case NOISE_GATE: sprintf(row, "ngt"); break;
				case OVERDRIVE: sprintf(row, "ovr"); break;
				case OVERDRIVE_SQRT: sprintf(row, "ovrs"); break;
				case TREMOLO: sprintf(row, "trm"); break;
				default: sprintf(row, "@@@");
				}
				Painter_WriteString(image, row, 40, width * i + width / 2 - 6, BOT_RIGHT, SMALL);
			}
		}

		sprintf(row, "%d/%d", active_pedals, MAX_PEDALS_COUNT);
		Painter_WriteString(image, row, 260, 14, BOT_LEFT, SMALL);

	} else {
		sprintf(row, "what");
		Painter_WriteString(image, row, 120, 60, BOT_LEFT, LARGE);
	}
}
