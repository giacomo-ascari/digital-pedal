/*
 * menu.c
 *
 *  Created on: Jun 14, 2022
 *      Author: asky
 */

#ifdef F103

#include "menu.h"
#include <string.h>
#include <stdio.h>

void Menu_Init(Menu_HandleTypeDef *hm, Commander_HandleTypeDef *hcommander, EPD_HandleTypeDef *hepd) {
	hm->selected_page = OVERVIEW;
	hm->page_state = BUSY;

	hm->hcommander = hcommander;
	hm->hepd = hepd;

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

void Menu_Render(Menu_HandleTypeDef *hm, enum render_types render) {

	char row[20];
	uint8_t *image = hm->hepd->image;
	Painter_Clean(image);

	if (hm->selected_page == OVERVIEW) {

		// title
		Painter_WriteString(image, "overview", 20, 0, BOT_LEFT, LARGE);
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
		Painter_WriteString(image, "plot", 20, 0, BOT_LEFT, LARGE);

		// commands
		sprintf(row, "y %d", hm->plot_yscale);
		Painter_WriteString(image, row, 0, 36, BOT_LEFT, SMALL);
		Painter_WriteString(image, ".", 8, 33, BOT_LEFT, SMALL);
		sprintf(row, "x %d", hm->plot_xscale);
		Painter_WriteString(image, row, 0, 86, BOT_LEFT, SMALL);
		Painter_WriteString(image, ".", 8, 83, BOT_LEFT, SMALL);

		// content
		uint16_t x, y;
		for (uint8_t i = 0; i < SIGNAL_SIZE; i++) {
			// IN
			x = (CANVAS_HEIGHT - 2 * SIGNAL_SIZE - 3) + i;
			y = CANVAS_WIDTH / 2 + hm->signal_in[i] / 2;
			Painter_TogglePixel(image, &x, &y, BOT_LEFT);
			// OUT
			x = (CANVAS_HEIGHT - SIGNAL_SIZE) + i;
			y = CANVAS_WIDTH / 2 + hm->signal_out[i] / 2;
			Painter_TogglePixel(image, &x, &y, BOT_LEFT);
		}
		Painter_ToggleDottedRectangle(image, CANVAS_HEIGHT - SIGNAL_SIZE - 2, 0, CANVAS_HEIGHT - SIGNAL_SIZE - 1, CANVAS_WIDTH, BOT_LEFT);

	} else if (hm->selected_page == EDIT) {

		// title
		Painter_WriteString(image, "edit", 20, 0, BOT_LEFT, LARGE);

		// content
		for (uint16_t i = 0; i < MAX_PEDALS_COUNT; i++) {
			// pedal selection
			enum pedal_types t = hm->pedals[i].pedal_formatted.type;
			uint16_t width = CANVAS_HEIGHT / MAX_PEDALS_COUNT;
			if (t == BYPASS) Painter_ToggleDottedRectangle(image, width * i + 4, 20, width * (i+1) - 4, 30, BOT_LEFT);
			else Painter_ToggleRectangle(image, width * i + 4, 20, width * (i+1) - 4, 30, BOT_LEFT);
			if (i == hm->edit_index2) {
				Painter_WriteString(image, "^", width * i + width / 2 - 4, 31, BOT_LEFT, SMALL);
				Painter_WriteString(image, hm->pedal_manifest[t].long_name, 110, 2, BOT_LEFT, SMALL);
				// param selection

			}
		}

	} else if (hm->selected_page == MODE) {

		// title
		Painter_WriteString(image, "mode", 20, 0, BOT_LEFT, LARGE);

		// content
		for (uint16_t i = 0; i < MODE_TYPES; i++) {
			Painter_WriteString(image, hm->mode_manifest[i].desc, i%2?158:10, i/2*18+30, BOT_LEFT, SMALL);
			if (i == hm->mode_active) {
				Painter_ToggleRectangle(image, i%2?156:8, i/2*18+28, i%2?294:146, i/2*18+44, BOT_LEFT);
			}
			if (i == hm->mode_selected % MODE_TYPES) {
				Painter_WriteString(image, ">", i%2?148:0, i/2*18+30, BOT_LEFT, SMALL);
			}
		}

	} else {

		sprintf(row, "what the %d", hm->selected_page);
		Painter_WriteString(image, row, 120, 60, BOT_LEFT, LARGE);
	}

	if (render == FULL) {
		EPD_Init(hm->hepd);
		EPD_Display(hm->hepd);
		EPD_Sleep(hm->hepd);
	} else if (render == PARTIAL) {
		EPD_Init(hm->hepd);
		Painter_ToggleCanvas(image);
		EPD_Display_Partial(hm->hepd);
		Painter_ToggleCanvas(image);
		EPD_Display_Partial(hm->hepd);
		EPD_Sleep(hm->hepd);
	}
}

void Menu_SendMessage(Menu_HandleTypeDef *hm, enum message_types type) {

	hm->command.header = hm->selected_page;
	hm->command.subheader = type;

	if (hm->selected_page == OVERVIEW) {
		if (type != FIRST) {
			return;
		}

	} else if (hm->selected_page == PLOT) {
		if (type == FIRST || type == PERIODIC) {
			hm->command.payload.bytes[0] = hm->plot_xscale;
			hm->command.payload.bytes[1] = hm->plot_yscale;
		} else {
			return;
		}

	} else if (hm->selected_page == EDIT) {
		if (type == FIRST) {
			//nada
		} else if (type == USER) {
			hm->command.payload.bytes[RAW_PEDAL_SIZE] = hm->edit_index2;
			memcpy(hm->command.payload.bytes, hm->pedals[hm->edit_index2].pedal_raw, RAW_PEDAL_SIZE);
		} else {
			return;
		}

	} else if (hm->selected_page == MODE) {
		if (type == FIRST) {
			//nada
		} else if (type == USER) {
			hm->command.payload.bytes[0] = hm->mode_selected;
		} else {
			return;
		}

	}

	// finally sending thank god
	Commander_Send(hm->hcommander, &(hm->command));
}

void Menu_UpdatePage(Menu_HandleTypeDef *hm) {
	Menu_Render(hm, PARTIAL);
}

#endif
