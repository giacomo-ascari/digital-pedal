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
	hm->hcommander = hcommander;
	hm->hepd = hepd;

	hm->plot_xscale = 1;
	hm->plot_yscale = 1;
	hm->edit_index1 = 0;
	hm->edit_index2 = 0;
	hm->edit_active = 0;
	hm->edit_oldvalue1 = 0;
	hm->edit_oldvalue2 = 0;
	hm->edit_initialvalue1 = 0;
	hm->edit_initialvalue2 = 0;
	hm->usb_ready = 0;
	hm->usb_selected = 0;
	hm->usb_result = -1;
	hm->tick = 0;

	hm->debug = 0;
	hm->mode_active = 0;
	hm->mode_selected = 0;
	Mode_Manifest_Init(hm->mode_manifest);
	Pedalboard_Init(&(hm->pedalboard));
}

uint8_t Menu_GoTo(Menu_HandleTypeDef *hm, enum page_types new_page) {
	if (hm->selected_page != new_page) {
		hm->selected_page = new_page;
		hm->tick = HAL_GetTick();
		return 1;
	} else {
		return 0;
	}
}

void Menu_RetrieveData(Menu_HandleTypeDef *hm, enum message_types type) {

	Command *out_command = &(hm->hcommander->out_command);

	out_command->header = hm->selected_page;
	out_command->subheader = type;

	if (hm->selected_page == OVERVIEW) {

		if (type == FIRST) {
			for (uint8_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
				out_command->param = i;
				Commander_SendAndWait(hm->hcommander);
				memcpy(hm->pedalboard.effects[i].effect_raw, hm->hcommander->in_command.payload.bytes, RAW_EFFECT_SIZE);
			}
		}

	} else if (hm->selected_page == PLOT) {

		if (type == FIRST || type == PERIODIC) {
			out_command->payload.bytes[0] = hm->plot_xscale;
			out_command->payload.bytes[1] = hm->plot_yscale;
			Commander_SendAndWait(hm->hcommander);
			memcpy(hm->signal_in, hm->hcommander->in_command.payload.bytes, SIGNAL_SIZE);
			memcpy(hm->signal_out, hm->hcommander->in_command.payload.bytes + SIGNAL_SIZE, SIGNAL_SIZE);
		}

	} else if (hm->selected_page == EDIT) {

		if (type == FIRST) {
			for (uint8_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
				out_command->param = i;
				Commander_SendAndWait(hm->hcommander);
				memcpy(hm->pedalboard.effects[i].effect_raw, hm->hcommander->in_command.payload.bytes, RAW_EFFECT_SIZE);
			}
		} else if (type == USER) {
			out_command->param = hm->edit_index2;
			memcpy(out_command->payload.bytes, hm->pedalboard.effects[hm->edit_index2].effect_raw, RAW_EFFECT_SIZE);
			Commander_SendAndWait(hm->hcommander);
		}

	} else if (hm->selected_page == MODE) {

		if (type == FIRST) {
			Commander_SendAndWait(hm->hcommander);
		} else if (type == USER) {
			out_command->param = hm->mode_selected;
			Commander_SendAndWait(hm->hcommander);
		}

	} else if (hm->selected_page == TUNER) {

		if (type == FIRST || type == PERIODIC) {
			Commander_SendAndWait(hm->hcommander);
		}

	} else if (hm->selected_page == FILES) {

		if (type == FIRST || type == PERIODIC) {
			Commander_SendAndWait(hm->hcommander);
			hm->usb_ready = hm->hcommander->in_command.param;
		} else if (type == USER) {
			out_command->param = hm->usb_selected % 2;
			Commander_SendAndWait(hm->hcommander);
			hm->usb_result = hm->hcommander->in_command.param;
			if (hm->usb_selected % 2 == 0) {
				out_command->header = OVERVIEW;
				out_command->subheader = FIRST;
				for (uint8_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
					out_command->param = i;
					Commander_SendAndWait(hm->hcommander);
					memcpy(hm->pedalboard.effects[i].effect_raw, hm->hcommander->in_command.payload.bytes, RAW_EFFECT_SIZE);
				}
			}
		}
	}
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
		for (uint16_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
			uint8_t t = hm->pedalboard.effects[i].effect_formatted.type;
			uint16_t width = CANVAS_HEIGHT / MAX_EFFECTS_COUNT;
			if (t == BYPASS) {
				Painter_ToggleDottedRectangle(image, width * i + 4, 35, width * (i+1) - 4, 100, BOT_LEFT);
			} else {
				active_pedals++;
				Painter_ToggleRectangle(image, width * i + 4, 35, width * (i+1) - 4, 100, BOT_LEFT);
				Painter_WriteString(image, Effects_Manifest[t].short_name, 35, width * i + width / 2 - 9, BOT_RIGHT, LARGE);
			}
		}
		sprintf(row, "%d/%d", active_pedals, MAX_EFFECTS_COUNT);
		Painter_WriteString(image, row, 260, 0, BOT_LEFT, SMALL);

	} else if (hm->selected_page == PLOT) {

		// title
		Painter_WriteString(image, "plot", 20, 0, BOT_LEFT, LARGE);

		// commands
		sprintf(row, "y< %d", hm->plot_yscale);
		Painter_WriteString(image, row, 0, 36, BOT_LEFT, SMALL);
		Painter_WriteString(image, ".", 18, 33, BOT_LEFT, SMALL);
		sprintf(row, ">x %d", hm->plot_xscale);
		Painter_WriteString(image, row, 0, 86, BOT_LEFT, SMALL);
		Painter_WriteString(image, ".", 18, 83, BOT_LEFT, SMALL);

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

		// content - effects rectangles, and cursor 2
		uint8_t type;
		uint16_t upper = 20;
		uint8_t selected = hm->edit_index2;
		uint8_t height = (CANVAS_WIDTH - upper) / MAX_EFFECTS_COUNT;
		uint16_t left = 80;
		for (uint16_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
			type = hm->pedalboard.effects[i].effect_formatted.type;
			if (type == BYPASS) {
				Painter_ToggleDottedRectangle(image, 2, upper + height * i + 2, 44, upper + height * (i+1) - 2, BOT_LEFT);
			} else {
				Painter_ToggleRectangle(image, 2, upper + height * i + 2, 44, upper + height * (i+1) - 2, BOT_LEFT);
			}
		}

		type = hm->pedalboard.effects[selected].effect_formatted.type;
		if (!hm->edit_active) Painter_WriteString(image, "<", 44, upper + height * selected + height / 2 - 6, BOT_LEFT, SMALL);
		Painter_WriteString(image, Effects_Manifest[type].long_name, 110, 2, BOT_LEFT, SMALL);
		// param selection
		uint8_t row_index = 0;

		for (uint8_t j = 0; j < INT_PARAM_TYPES; j++) {
			if (Effects_Manifest[type].params_manifest.int_params_manifest[j].active) {
				int_params_manifest_t *manifest = &(Effects_Manifest[type].params_manifest.int_params_manifest[j]);
				Painter_WriteString(image, manifest->name, left, upper + row_index * 14, BOT_LEFT, SMALL);
				if (manifest->qual == PERCENTAGE) {
					sprintf(row, "%l %%", hm->pedalboard.effects[selected].effect_formatted.config.int_params[j] * 100 / (manifest->max - manifest->min));
				} else if (manifest->qual == VALUE) {
					sprintf(row, "%l", hm->pedalboard.effects[selected].effect_formatted.config.int_params[j]);
				} else if (manifest->qual == FREQUENCY) {
					sprintf(row, "%l Hz", hm->pedalboard.effects[selected].effect_formatted.config.int_params[j]);
				}
				Painter_WriteString(image, row, left + 140, upper + row_index * 14, BOT_LEFT, SMALL);
				row_index++;
			}
		}

		for (uint8_t j = 0; j < FLOAT_PARAM_TYPES; j++) {
			if (Effects_Manifest[type].params_manifest.float_params_manifest[j].active) {
				float_params_manifest_t *manifest = &(Effects_Manifest[type].params_manifest.float_params_manifest[j]);
				Painter_WriteString(image, manifest->name, left, upper + row_index * 14, BOT_LEFT, SMALL);
				if (manifest->qual == PERCENTAGE) {
					sprintf(row, "%.2f %%", hm->pedalboard.effects[selected].effect_formatted.config.float_params[j] * 100 / (manifest->max - manifest->min));
				} else if (manifest->qual == VALUE) {
					sprintf(row, "%.2f", hm->pedalboard.effects[selected].effect_formatted.config.float_params[j]);
				} else if (manifest->qual == FREQUENCY) {
					sprintf(row, "%.2f Hz", hm->pedalboard.effects[selected].effect_formatted.config.float_params[j]);
				}
				Painter_WriteString(image, row, left + 140, upper + row_index * 14, BOT_LEFT, SMALL);
				row_index++;
			}
		}

		if (type != BYPASS) {
			if (!hm->edit_active) Painter_WriteString(image, ">", left - 8, upper + (hm->edit_index1 % row_index) * 14, BOT_LEFT, SMALL);
			if (hm->edit_active) Painter_ToggleRectangle(image, left, upper + (hm->edit_index1 % row_index) * 14 - 1, CANVAS_HEIGHT, upper + (hm->edit_index1 % row_index + 1) * 14 - 1, BOT_LEFT);
		}


	} else if (hm->selected_page == MODE) {

		// title
		Painter_WriteString(image, "mode", 20, 0, BOT_LEFT, LARGE);

		// content
		for (uint16_t i = 0; i < MODE_TYPES; i++) {
			Painter_WriteString(image, hm->mode_manifest[i].desc, i%2?158:10, i/2*18+30, BOT_LEFT, SMALL);
			if (i == hm->mode_active) {
				Painter_ToggleRectangle(image, i%2?157:9, i/2*18+28, i%2?294:146, i/2*18+44, BOT_LEFT);
			}
			if (i == hm->mode_selected) {
				Painter_WriteString(image, ">", i%2?148:0, i/2*18+30, BOT_LEFT, SMALL);
			}
		}

	} else if (hm->selected_page == TUNER) {

		// title
		Painter_WriteString(image, "tuner", 20, 0, BOT_LEFT, LARGE);


	} else if (hm->selected_page == FILES) {

		// title
		Painter_WriteString(image, "usb", 20, 0, BOT_LEFT, LARGE);

		// content
		if (hm->usb_result == -1) {
			if (hm->usb_ready) {
				if (hm->usb_selected == 1) {
					Painter_WriteString(image, ">", 294/3*2 - 36, 55, BOT_LEFT, LARGE);
				} else if (hm->usb_selected == 0) {
					Painter_WriteString(image, ">", 294/3 - 36, 55, BOT_LEFT, LARGE);
				}
				Painter_WriteString(image, "load", 294/3 - 24, 55, BOT_LEFT, LARGE);
				Painter_WriteString(image, "save", 294/3*2 - 24, 55, BOT_LEFT, LARGE);
			} else {
				Painter_WriteString(image, "disconnected", 294/2 - 72, 55, BOT_LEFT, LARGE);
			}
		} else {
			if (hm->usb_result == 1) {
				Painter_WriteString(image, "ok", 294/2 - 12, 55, BOT_LEFT, LARGE);
			} else {
				Painter_WriteString(image, "error", 294/2 - 30, 55, BOT_LEFT, LARGE);
			}
			hm->usb_result = -1;
		}

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

#endif
