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
	hm->edit_index1 = 0;
	hm->edit_index2 = 0;
	hm->edit_active = 0;
	hm->usb_ready = 0;
	hm->debug = 0;
	hm->mode_active = 0;
	hm->mode_selected = 0;
	Mode_Manifest_Init(hm->mode_manifest);
	Pedal_Manifest_Init(hm->pedal_manifest);
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
				Painter_WriteString(image, hm->pedal_manifest[t].short_name, 35, width * i + width / 2 - 9, BOT_RIGHT, LARGE);
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

	} else if (hm->selected_page == EDIT) {

		// title
		sprintf(row, "EDIT");
		Painter_WriteString(image, row, 20, 0, BOT_LEFT, LARGE);

		// content
		for (uint16_t i = 0; i < MAX_PEDALS_COUNT; i++) {
			// pedal selection
			enum pedal_types t = hm->pedals[i].pedal_formatted.type;
			uint16_t width = CANVAS_HEIGHT / MAX_PEDALS_COUNT;
			if (t == BYPASS) Painter_ToggleDottedRectangle(image, width * i + 4, 20, width * (i+1) - 4, 30, BOT_LEFT);
			else Painter_ToggleRectangle(image, width * i + 4, 20, width * (i+1) - 4, 30, BOT_LEFT);
			if (i == hm->edit_index2 % MAX_PEDALS_COUNT) {
				Painter_ToggleRectangle(image, width * i + 4, 31, width * (i+1) - 4, 35, BOT_LEFT);
				Painter_WriteString(image, hm->pedal_manifest[t].long_name, 110, 2, BOT_LEFT, SMALL);
				// param selection

			}
		}

	} else if (hm->selected_page == MODE) {

		// title
		sprintf(row, "MODE");
		Painter_WriteString(image, row, 20, 0, BOT_LEFT, LARGE);

		// content
		for (uint16_t i = 0; i < MODE_TYPES; i++) {
			Painter_WriteString(image, hm->mode_manifest[i].desc, i%2?158:10, i/2*18+30, BOT_LEFT, SMALL);
			if (i == hm->mode_active) {
				Painter_ToggleRectangle(image, i%2?156:8, i/2*18+28, i%2?294:146, i/2*18+44, BOT_LEFT);
			}
			if (i == hm->mode_selected % MODE_TYPES) {
				sprintf(row, ">");
				Painter_WriteString(image, row, i%2?150:2, i/2*18+30, BOT_LEFT, SMALL);
			}
		}

	} else {

		sprintf(row, "what the %d", hm->selected_page);
		Painter_WriteString(image, row, 120, 60, BOT_LEFT, LARGE);
	}
}

void Menu_SendMessage(Menu_HandleTypeDef *hm, enum update_type update) {
	if (hm->selected_page == OVERVIEW) {
		hm->command.header = 1;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (hm->selected_page == PLOT) {
		hm->command.header = 2;
		hm->command.update = update;
		hm->command.payload.bytes[0] = hm->plot_xscale;
		hm->command.payload.bytes[1] = hm->plot_yscale;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (hm->selected_page == EDIT) {
		hm->command.header = 3;
		hm->command.update = update;
		Commander_Send(hm->hcommander, &(hm->command));
	} else if (hm->selected_page == MODE) {
		hm->command.header = 4;
		hm->command.header = 1;
		hm->command.update = update;
		Commander_Send(hm->hcommander, &(hm->command));
	}
}
