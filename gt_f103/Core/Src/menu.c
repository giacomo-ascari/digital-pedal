/*
 * menu.c
 *
 *  Created on: Jun 14, 2022
 *      Author: asky
 */

#include "menu.h"
#include <string.h>
#include <stdio.h>

void Menu_Init(Menu_HandleTypeDef *hm) {
	hm->selected_page = OVERVIEW;
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

	} else if (hm->selected_page == PLOT) {

		sprintf(row, "PLOT");
		Painter_WriteString(image, row, 10, 10, BOT_LEFT, LARGE);

	} else {

		sprintf(row, "what the %d", hm->selected_page);
		Painter_WriteString(image, row, 120, 60, BOT_LEFT, LARGE);
	}
}

void Menu_GoTo(Menu_HandleTypeDef *hm, enum page_types new_page) {
	if (new_page == OVERVIEW) {
		hm->command.header = 1;
		hm->command.subheader = 1;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (new_page == PLOT) {
		hm->command.header = 2;
		hm->command.subheader = 1;
		Commander_Send(hm->hcommander, &(hm->command));
	}

	hm->selected_page = new_page;
}
