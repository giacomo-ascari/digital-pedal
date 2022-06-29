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
	hm->plot_xscale = 1;
	hm->plot_yscale = 1;
	hm->usb_ready = 0;
	hm->debug++;
}

void Menu_Render(Menu_HandleTypeDef *hm, uint8_t *image) {
	char row[16+1];

	Painter_Clean(image);

	if (hm->selected_page == OVERVIEW) {

		// title
		sprintf(row, "overview");
		Painter_WriteString(image, row, 20, 0, BOT_LEFT, LARGE);
		uint8_t active_pedals = 0;

		// content
		for (uint16_t i = 0; i < MAX_PEDALS_COUNT; i++) {
			enum pedal_types t = hm->pedals[i].pedal_formatted.type;
			uint16_t width = CANVAS_HEIGHT / MAX_PEDALS_COUNT;
			if (t == BYPASS) {
				Painter_ToggleDottedRectangle(image, width * i + 4, 35, width * (i+1) - 4, 100, BOT_LEFT);
			} else {
				active_pedals++;
				Painter_ToggleRectangle(image, width * i + 4, 35, width * (i+1) - 4, 100, BOT_LEFT);
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
				Painter_WriteString(image, row, 35, width * i + width / 2 - 9, BOT_RIGHT, LARGE);
			}
		}
		sprintf(row, "%d/%d", active_pedals, MAX_PEDALS_COUNT);
		Painter_WriteString(image, row, 260, 0, BOT_LEFT, SMALL);

	} else if (hm->selected_page == PLOT) {

		// title
		sprintf(row, "PLOT");
		Painter_WriteString(image, row, 20, 0, BOT_LEFT, LARGE);

		// commands
		sprintf(row, "^y");
		Painter_WriteString(image, row, 0, 0, BOT_LEFT, SMALL);
		sprintf(row, "<x");
		Painter_WriteString(image, row, 0, 116, BOT_LEFT, SMALL);

		// content
		uint16_t x, y;
		for (uint8_t i = 0; i < SIGNAL_SIZE; i++) {
			// IN
			x = (CANVAS_HEIGHT / 2 - SIGNAL_SIZE) / 2 + i;
			y = CANVAS_WIDTH / 2 + hm->signal_in[i] / 2;
			Painter_TogglePixel(image, &x, &y, BOT_LEFT);
			// OUT
			x = CANVAS_HEIGHT / 2 + (CANVAS_HEIGHT / 2 - SIGNAL_SIZE) / 2 + i;
			y = CANVAS_WIDTH / 2 + hm->signal_out[i] / 2;
			Painter_TogglePixel(image, &x, &y, BOT_LEFT);
		}
		sprintf(row, "x*%d y*%d", hm->plot_xscale, hm->plot_yscale);
		Painter_WriteString(image, row, 210, 0, BOT_LEFT, SMALL);
		Painter_ToggleDottedRectangle(image, CANVAS_HEIGHT / 2 - 1, 0, CANVAS_HEIGHT / 2 + 1, CANVAS_WIDTH, BOT_LEFT);

	} else {

		sprintf(row, "what the %d", hm->selected_page);
		Painter_WriteString(image, row, 120, 60, BOT_LEFT, LARGE);
	}
}

void Menu_GoTo(Menu_HandleTypeDef *hm, enum page_types new_page, enum update_type update) {
	if (new_page == OVERVIEW) {
		hm->command.header = 1;
		hm->command.subheader = 1;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (new_page == PLOT) {
		hm->command.header = 2;
		if (update == VISIBLE) hm->command.subheader = 1;
		else hm->command.subheader = 2;
		hm->command.payload.bytes[0] = hm->plot_xscale;
		hm->command.payload.bytes[1] = hm->plot_yscale;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (new_page == EDIT) {
		hm->command.header = 3;
		hm->command.subheader = 1;
		Commander_Send(hm->hcommander, &(hm->command));
	}

	hm->selected_page = new_page;
}
