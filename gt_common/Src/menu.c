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
#include <math.h>

void Menu_Init(Menu_Data *data, Commander_HandleTypeDef *_hcptr) {

	data->hcptr = _hcptr;
	data->tick = 0;
	data->debug = 0;
	data->page = OVERVIEW;

	//Pedalboard_Handler pedalboard;
	//drum_conf_t drums_conf;
	//lines_conf_t lines_conf;

	data->overview_data.elapsedus = 0;

	data->plot_data.xscale = 1;
	data->plot_data.yscale = 1;

	data->pedals_data.index1 = 0;
	data->pedals_data.index2 = 0;
	data->pedals_data.active = 0;
	data->pedals_data.oldvalue1 = 0;
	data->pedals_data.oldvalue2 = 0;
	data->pedals_data.initialvalue1 = 0;
	data->pedals_data.initialvalue2 = 0;

	data->lines_data.input_selected = 0;
	data->lines_data.output_selected = 0;

	data->drums_data.index1 = 0;
	data->drums_data.index2 = 0;
	data->drums_data.active = 0;
	data->drums_data.oldvalue1 = 0;
	data->drums_data.oldvalue2 = 0;
	data->drums_data.initialvalue1 = 0;
	data->drums_data.initialvalue2 = 0;

	data->files_data.save_slots = 0;
	data->files_data.slot_selected = 0;
	data->files_data.action_selected = 0;

	data->pedalboard_changed = 1; 	// changed by SYNC
	data->lines_changed = 1;
	data->drums_changed = 1;
	data->cursor1_changed = 1; 		// changed by MAIN LOOP
	data->cursor2_changed = 1; 		// changed by MAIN LOOP
	data->signal_changed = 1;  		// changed by SYNC
}

const char page_manifest[PAGE_TYPES][10] = {
		"overview",
		"plot",
		"pedals",
		"lines",
		"drums",
		"files",
};

// interval in ms for each page to issue a new command
//											OVERVIEW	PLOT	EDIT	MODE	SPECTRUM	FILES
const uint16_t sync_interval[PAGE_TYPES] = {1000,		1000,	0,		0,		1000,		1000};

void Menu_Sync(Menu_Data *data, enum Menu_CommandHeader type) {

	Command *out_command = &(data->hcptr->out_command);
	out_command->header = type;

	if (type == GET_PB) {

		// assumes PEDALBOARD_HANDLER_SIZE <= PAYLOAD_BYTESIZE
		Commander_SendAndWait(data->hcptr);
		memcpy((uint8_t *)&data->pedalboard, data->hcptr->in_command.payload.bytes, PEDALBOARD_HANDLER_SIZE);
		// changes
		data->pedalboard_changed = 1;

	} else if (type == SET_PB) {

		// assumes PEDALBOARD_HANDLER_SIZE <= PAYLOAD_BYTESIZE
		memcpy(out_command->payload.bytes, (uint8_t *)&data->pedalboard, PEDALBOARD_HANDLER_SIZE );
		Commander_SendAndWait(data->hcptr);
		// changes
		data->pedalboard_changed = 1;

	} else if (type == GET_SIGNALS) {

		out_command->payload.bytes[0] = data->plot_data.xscale;
		out_command->payload.bytes[1] = data->plot_data.yscale;
		Commander_SendAndWait(data->hcptr);
		memcpy(data->plot_data.signal_in, data->hcptr->in_command.payload.bytes, SIGNAL_SIZE);
		memcpy(data->plot_data.signal_out, data->hcptr->in_command.payload.bytes + SIGNAL_SIZE, SIGNAL_SIZE);
		// changes
		data->signal_changed = 1;

	} else if (type == GET_SPECTRUM) {

		//Commander_SendAndWait(data->hcptr);
		//memcpy(data->spectrum_data.signal, data->hcptr->in_command.payload.bytes, PAYLOAD_BYTESIZE);

	} else if (type == GET_LOAD) {

		Commander_SendAndWait(data->hcptr);
		memcpy((uint8_t *)&(data->overview_data.elapsedus), data->hcptr->in_command.payload.bytes, 2);
		// changes
		data->text_changed = 1;

	} else if (type == GET_FLASH) {

		Commander_SendAndWait(data->hcptr);
		data->files_data.save_slots = data->hcptr->in_command.subheader;
		for (uint8_t i = 0; i < data->files_data.save_slots; i++) {
			data->files_data.full[i] = data->hcptr->in_command.payload.bytes[i*(EFFECT_SLOTS_COUNT+1)+0];
			if (data->files_data.full[i]) {
				for (uint8_t j = 0; j < EFFECT_SLOTS_COUNT; j++) {
					data->files_data.saves[i][j] = data->hcptr->in_command.payload.bytes[i*(EFFECT_SLOTS_COUNT+1)+j+1];
				}
			}
		}
		// changes
		data->text_changed = 1;

	} else if (type == SAVE_FLASH) {

		data->hcptr->out_command.subheader = data->files_data.slot_selected;
		Commander_SendAndWait(data->hcptr);
		// changes
		data->text_changed = 1;

	} else if (type == LOAD_FLASH) {

		//if (data->files_data.full[data->files_data.slot_selected]) f407 accountable
		data->hcptr->out_command.subheader = data->files_data.slot_selected;
		Commander_SendAndWait(data->hcptr);
		// changes
		data->text_changed = 1;

	} else if (type == DEL_FLASH) {

		data->hcptr->out_command.subheader = data->files_data.slot_selected;
		Commander_SendAndWait(data->hcptr);
		// changes
		data->text_changed = 1;

	} else if (type == GET_DRUMS) {

		// assumes DRUMS_CONF_SIZE <= PAYLOAD_BYTESIZE
		Commander_SendAndWait(data->hcptr);
		memcpy((uint8_t *)&data->drums_conf, data->hcptr->in_command.payload.bytes, DRUMS_CONF_SIZE);
		// changes
		data->drums_changed = 1;

	} else if (type == SET_DRUMS) {

		// assumes DRUMS_CONF_SIZE <= PAYLOAD_BYTESIZE
		memcpy(out_command->payload.bytes, (uint8_t *)&data->drums_conf, DRUMS_CONF_SIZE );
		Commander_SendAndWait(data->hcptr);
		// changes
		data->drums_changed = 1;

	} else if (type == GET_LINES) {

		// assumes DRUMS_CONF_SIZE <= PAYLOAD_BYTESIZE
		Commander_SendAndWait(data->hcptr);
		memcpy((uint8_t *)&data->lines_conf, data->hcptr->in_command.payload.bytes, LINES_CONF_SIZE);
		// changes
		data->lines_changed = 1;

	} else if (type == SET_LINES) {

		// assumes LINES_CONF_SIZE <= PAYLOAD_BYTESIZE
		memcpy(out_command->payload.bytes, (uint8_t *)&data->lines_conf, LINES_CONF_SIZE );
		Commander_SendAndWait(data->hcptr);
		// changes
		data->lines_changed = 1;

	}
}

uint8_t Menu_GoTo(Menu_Data *data, enum Menu_PageType new_page, uint8_t forced) {

	if (new_page != data->page || forced) {

		data->page = new_page;

		// erase everything
		LCD_Painter_EraseText(10, 20, 10, COLOR_WHITE, LARGE);
		LCD_Painter_DrawText("...", 20, 10, COLOR_BLACK, COLOR_WHITE, LARGE);
		for (uint8_t i = 0; i < TEXT_CLASSES; i++) {
			Menu_EraseTexts(i);
		}
		Menu_ErasePlotpoints();

		ILI9341_Fill_Rect(0, 10+18+10, ILI9341_HEIGHT, ILI9341_WIDTH, COLOR_WHITE);

		// times are a'changin'
		data->tick = HAL_GetTick();

		// titles
		LCD_Painter_DrawText(page_manifest[new_page], 20, 10, COLOR_BLACK, COLOR_WHITE, LARGE);

		data->cursor1_changed = 1;
		data->cursor2_changed = 1;

		return 1;
	}

	return 0;
}

void Menu_Render(Menu_Data *data) {

	static char row[40+1];

	if (data->page == OVERVIEW) {

		if (data->pedalboard_changed) {
			// big rectangles of the pedals
			uint8_t active_pedals = 0;
			uint16_t width = ILI9341_HEIGHT / EFFECT_SLOTS_COUNT;
			for (uint16_t i = 0; i < EFFECT_SLOTS_COUNT; i++) {
				uint8_t t = data->pedalboard.effects[i].type;
				if (t == BYP) {
					ILI9341_Fill_Rect(width * i + 4, 66,  width * (i+1) - 4, 156, COLOR_LGRAY);
				} else {
					active_pedals++;
					ILI9341_Fill_Rect(width * i + 4, 66,  width * (i+1) - 4, 156, COLOR_ORANGE);
					for (uint8_t j = 0; j < strlen(Effects_Manifest[t].short_name); j++) {
						LCD_Painter_DrawChar(Effects_Manifest[t].short_name[j], width * i + width/2 - 6, 76+18*j, COLOR_BLACK, COLOR_ORANGE, LARGE);
					}
				}
			}

			sprintf(row, "pedals %d/%d ", active_pedals, EFFECT_SLOTS_COUNT);
			LCD_Painter_DrawText(row, 210, 40, COLOR_BLACK, COLOR_WHITE, MEDIUM);

			sprintf(row, "%s to %s", input_line_manifest[data->lines_conf.input_line], output_line_manifest[data->lines_conf.output_line]);
			LCD_Painter_DrawText(row, 10, 180, COLOR_BLACK, COLOR_WHITE, MEDIUM);

			if (data->drums_conf.active) {
				sprintf(row, "drums on, %.1f bpm", data->drums_conf.float_params[DFP1]);
				LCD_Painter_DrawText(row, 10, 204, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			} else {
				LCD_Painter_DrawText("drums off", 10, 204, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			}
		}

		if (data->text_changed) {
			float us_per_dsptick = 1000000.F / 48000.F;
			float us_elapsed = (float)data->overview_data.elapsedus;
			float load = 100 * us_elapsed / us_per_dsptick;

			sprintf(row, "load %.0f%%  ", load);
			LCD_Painter_DrawText(row, 230, 166, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

	} else if (data->page == PLOT) {

		if (data->cursor1_changed) {
			Menu_EraseTexts(0);
			LCD_Painter_DrawText(">x  ", 10, ILI9341_WIDTH/2 + 40, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			LCD_Painter_DrawText("x", 10+30, ILI9341_WIDTH/2 + 40 + 3, COLOR_BLACK, COLOR_WHITE, SMALL);
			sprintf(row, "%d", data->plot_data.xscale);
			Menu_DrawText(0, row, 10+40, ILI9341_WIDTH/2 + 40, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		if (data->cursor2_changed) {
			Menu_EraseTexts(1);
			LCD_Painter_DrawText("y<  ", 10, ILI9341_WIDTH/2 -  40, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			LCD_Painter_DrawText("x", 10+30, ILI9341_WIDTH/2 - 40 + 3, COLOR_BLACK, COLOR_WHITE, SMALL);
			sprintf(row, "%d", data->plot_data.yscale);
			Menu_DrawText(1, row, 10+40, ILI9341_WIDTH/2 - 40, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		if (data->signal_changed) {
			Menu_ErasePlotpoints();
			/*for (uint8_t i = 0; i < EFFECT_SLOTS_COUNT; i++) {
				if (data->pedalboard.effects[i].type == CMP) {
					float _threshold = MAX_VAL * powf(10.F, data->pedalboard.effects[i].config.float_params[FP0]*0.1F);
					uint32_t threshold = _threshold;
					uint16_t y = ILI9341_WIDTH / 2 - (data->plot_data.yscale * threshold >> 17);
					if (y >= ILI9341_WIDTH / 2 - 127 && y < ILI9341_WIDTH / 2 + 127) {
						ILI9341_drawLine(ILI9341_HEIGHT - 2 * SIGNAL_SIZE - 3, y, ILI9341_HEIGHT - 2 * SIGNAL_SIZE - 3 + 127, y, COLOR_BLUE);
						ILI9341_drawLine(ILI9341_HEIGHT - SIGNAL_SIZE, y, ILI9341_HEIGHT - SIGNAL_SIZE + 127, y, COLOR_BLUE);
					}
				}
			}*/
			uint16_t x, y;
			for (uint8_t i = 0; i < SIGNAL_SIZE; i++) {
				// IN
				x = (ILI9341_HEIGHT - 2 * SIGNAL_SIZE - 3) + i;
				y = ILI9341_WIDTH / 2 - data->plot_data.signal_in[i] / 2;
				Menu_DrawPlotpoint(x, y, COLOR_BLACK, COLOR_WHITE);
				// OUT
				x = (ILI9341_HEIGHT - SIGNAL_SIZE) + i;
				y = ILI9341_WIDTH / 2 - data->plot_data.signal_out[i] / 2;
				Menu_DrawPlotpoint(x, y, COLOR_BLACK, COLOR_WHITE);
			}
			ILI9341_drawLine(ILI9341_HEIGHT - SIGNAL_SIZE - 2, ILI9341_WIDTH / 2 - 64, ILI9341_HEIGHT - SIGNAL_SIZE - 2, ILI9341_WIDTH / 2 + 64, COLOR_LGRAY);
		}

	} else if (data->page == PEDALS) {

		// 0 cursor1, 1 cursor2, 2 content, 3 title

		// cursors move and disapper when the edit is active
		// rectangles changes color when pb changes
		// texts when pb changes
		// numbers when edit is active

		uint8_t selected = data->pedals_data.index2;
		uint8_t type = data->pedalboard.effects[selected].type;
		uint16_t upper = 70;
		uint8_t height = (ILI9341_WIDTH - upper) / EFFECT_SLOTS_COUNT;
		uint16_t left = 80;

		// just cursor 2
		if (data->cursor2_changed) {
			// cursor 2
			Menu_EraseTexts(1);
			Menu_DrawText(1, "<", 44, upper + height * data->pedals_data.index2 + height / 2 - 8, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		if (data->pedalboard_changed) {
			// rectangles on the side
			for (uint16_t i = 0; i < EFFECT_SLOTS_COUNT; i++) {
				if (data->pedalboard.effects[i].type == BYP) {
					ILI9341_Fill_Rect(2, upper + height * i + 2,  44 -1, upper + height * (i+1) - 2 -1, COLOR_LGRAY);
				} else {
					ILI9341_Fill_Rect(2, upper + height * i + 2,  44 -1, upper + height * (i+1) - 2 -1, COLOR_ORANGE);
				}
			}

		}

		if (data->pedalboard_changed || data->cursor2_changed) {
			// title of the selected effect
			Menu_EraseTexts(3);
			Menu_DrawText(3, Effects_Manifest[type].long_name, 110, 50, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		static uint8_t row_index = 0;

		if (data->pedalboard_changed || data->cursor2_changed || (data->cursor1_changed && data->pedals_data.active)) {
			Menu_EraseTexts(2);
			// param selection
			row_index = 0;
			for (uint8_t j = 0; j < INT_PARAMS_COUNT; j++) {
				if (Effects_Manifest[type].int_params_manifest[j].active) {
					const int_params_manifest_t *manifest = &(Effects_Manifest[type].int_params_manifest[j]);
					int32_t value = data->pedalboard.effects[selected].config.int_params[j];
					if (manifest->qual == PERCENTAGE) 		sprintf(row, "n/a");
					else if (manifest->qual == VALUE) 		sprintf(row, "%s: %ld", manifest->name, value);
					else if (manifest->qual == FREQUENCY) 	sprintf(row, "%s: %ld Hz", manifest->name, value);
					else if (manifest->qual == TIME) 		sprintf(row, "%s: %ld ms", manifest->name, value);
					else if (manifest->qual == DB)			sprintf(row, "n/a");
					else if (manifest->qual == KEY)			sprintf(row, "%s: %s", manifest->name, note_manifest[value]);
					Menu_DrawText(
							2,
							row,
							left,
							upper + row_index * 18,
							COLOR_BLACK,
							(data->pedals_data.index1 == row_index && data->pedals_data.active) ? COLOR_ORANGE :COLOR_WHITE,
							MEDIUM);
					row_index++;
				}
			}

			for (uint8_t j = 0; j < FLOAT_PARAMS_COUNT; j++) {
				if (Effects_Manifest[type].float_params_manifest[j].active) {
					const float_params_manifest_t *manifest = &(Effects_Manifest[type].float_params_manifest[j]);
					float value = data->pedalboard.effects[selected].config.float_params[j];
					if (manifest->qual == PERCENTAGE) 		sprintf(row, "%s: %.1f %%", manifest->name, value * 100);
					else if (manifest->qual == VALUE) 		sprintf(row, "%s: %.2f", manifest->name, value);
					else if (manifest->qual == FREQUENCY) 	sprintf(row, "%s: %.1f Hz", manifest->name, value);
					else if (manifest->qual == TIME) 		sprintf(row, "n/a");
					else if (manifest->qual == DB)			sprintf(row, "%s: %.1f dB", manifest->name, value);
					else if (manifest->qual == KEY)			sprintf(row, "n/a");
					Menu_DrawText(
							2,
							row,
							left,
							upper + row_index * 18,
							COLOR_BLACK,
							(data->pedals_data.index1 == row_index && data->pedals_data.active) ? COLOR_ORANGE :COLOR_WHITE,
							MEDIUM);
					row_index++;
				}
			}
		}


		// just cursor 1
		if (data->pedalboard_changed ||data->cursor1_changed || data->cursor2_changed) {
			Menu_EraseTexts(0);
			if (type != BYP && !data->pedals_data.active && Pedalboard_CountActiveParamsByType(type) > 0) {
				Menu_DrawText(0, ">", left - 10, upper + (data->pedals_data.index1 % row_index) * 18, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			}
		}



	} else if (data->page == LINES) {

		if (data->cursor1_changed) {
			Menu_EraseTexts(0);
			for (uint16_t i = 0; i < INPUT_LINE_TYPES; i++) {
				if (i == data->lines_data.input_selected) {
					Menu_DrawText(0, ">", ILI9341_HEIGHT/2 - 100 - 10, i*20+76, COLOR_BLACK, COLOR_WHITE, MEDIUM);
				}
			}
		}

		if (data->cursor2_changed) {
			Menu_EraseTexts(1);
			for (uint16_t i = 0; i < OUTPUT_LINE_TYPES; i++) {
				if (i == data->lines_data.output_selected) {
					Menu_DrawText(1, "<", ILI9341_HEIGHT/2 + 100, i*20+76, COLOR_BLACK, COLOR_WHITE, MEDIUM);
				}
			}
		}

		if (data->lines_changed) {
			LCD_Painter_DrawText("input", ILI9341_HEIGHT/2 - 5*10 - 30, 50, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			LCD_Painter_DrawText("output", ILI9341_HEIGHT/2 + 30, 50, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			for (uint16_t i = 0; i < INPUT_LINE_TYPES; i++) {
				LCD_Painter_DrawText(
						input_line_manifest[i],
						ILI9341_HEIGHT/2 - 8 * 10 - 15,
						i*20+76,
						COLOR_BLACK,
						(i == data->lines_conf.input_line) ? COLOR_ORANGE : COLOR_WHITE,
						MEDIUM);
			}
			for (uint16_t i = 0; i < OUTPUT_LINE_TYPES; i++) {
				LCD_Painter_DrawText(
						output_line_manifest[i],
						ILI9341_HEIGHT/2 + 15,
						i*20+76,
						COLOR_BLACK,
						(i == data->lines_conf.output_line) ? COLOR_ORANGE : COLOR_WHITE,
						MEDIUM);
			}
		}



	} else if (data->page == DRUMS) {

		// 0 cursor1, 1 cursor2, 2 content, 3 title

		// cursors move and disapper when the edit is active
		// rectangles changes color when pb changes
		// texts when pb changes
		// numbers when edit is active

		uint16_t upper = 76;
		uint16_t left = ILI9341_HEIGHT/2 - 120;

		static uint8_t row_index = 0;

		if (data->drums_changed || (data->cursor2_changed && data->drums_data.active) || (data->cursor1_changed && data->drums_data.active)) {
			Menu_EraseTexts(2);
			// param selection
			row_index = 0;
			for (uint8_t j = 0; j < DRUM_INT_PARAMS_COUNT; j++) {
				const drum_int_params_manifest_t *manifest = &(drum_params_manifest.int_params_manifest[j]);
				int32_t value = data->drums_conf.int_params[j];
				if (manifest->qual == DRUM_VALUE) 		sprintf(row, "%s: %ld", manifest->name, value);
				else if (manifest->qual == DRUM_DB)		sprintf(row, "n/a");
				else if (manifest->qual == DRUM_PS)		sprintf(row, "%s: %s", manifest->name, drum_presets_manifest[value].name);
				Menu_DrawText(
						2,
						row,
						left,
						upper + row_index * 20,
						COLOR_BLACK,
						(data->drums_data.index1 == row_index && data->drums_data.active) ? COLOR_ORANGE :COLOR_WHITE,
						MEDIUM);
				row_index++;
			}

			for (uint8_t j = 0; j < DRUM_FLOAT_PARAMS_COUNT; j++) {
				const drum_float_params_manifest_t *manifest = &(drum_params_manifest.float_params_manifest[j]);
				float value = data->drums_conf.float_params[j];
				if (manifest->qual == DRUM_VALUE) 		sprintf(row, "%s: %.2f", manifest->name, value);
				else if (manifest->qual == DRUM_DB)		sprintf(row, "%s: %.1f dB", manifest->name, value);
				else if (manifest->qual == DRUM_PS)		sprintf(row, "n/a");
				Menu_DrawText(
						2,
						row,
						left,
						upper + row_index * 20,
						COLOR_BLACK,
						(data->drums_data.index1 == row_index && data->drums_data.active) ? COLOR_ORANGE :COLOR_WHITE,
						MEDIUM);
				row_index++;
			}
			//recomm bpm
			sprintf(row, "rec. %.1f", drum_presets_manifest[data->drums_conf.int_params[DIP0]].default_bpm);
			Menu_DrawText(2, row, 180, upper + 40, COLOR_LGRAY, COLOR_WHITE, MEDIUM);
		}


		// just cursor 1
		if (data->drums_changed || data->cursor1_changed) {
			Menu_EraseTexts(0);
			if (!data->drums_data.active) {
				Menu_DrawText(0, ">", left - 10, upper + (data->drums_data.index1 % row_index) * 20, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			}
		}

		// on and off
		if (data->drums_changed) {
			Menu_EraseTexts(3);
			Menu_DrawText(
				3, "off", ILI9341_HEIGHT-80, 180, COLOR_BLACK,
				(!data->drums_conf.active && !data->drums_data.active) ? COLOR_ORANGE :COLOR_WHITE,
				MEDIUM);
			Menu_DrawText(
				3, "on", ILI9341_HEIGHT-80, 200, COLOR_BLACK,
				(data->drums_conf.active && !data->drums_data.active) ? COLOR_ORANGE :COLOR_WHITE,
				MEDIUM);
		}

		// just cursor 2
		if (data->cursor2_changed) {
			Menu_EraseTexts(1);
			if (!data->drums_data.active) {
				Menu_DrawText(1, "<", ILI9341_HEIGHT-80+30, 180 + (data->drums_data.index2 % 2) * 20, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			}
		}


	} else if (data->page == FILES) {

		if (data->cursor1_changed) {
			Menu_EraseTexts(0);
			Menu_DrawText(0, ">", 30-10, 50+20*data->files_data.slot_selected, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		if (data->cursor2_changed) {
			Menu_EraseTexts(1);
			Menu_DrawText(1, "<", ILI9341_HEIGHT-80+40, 160+20*data->files_data.action_selected, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

		if (data->text_changed) {
			Menu_EraseTexts(2);
			for (uint8_t i = 0; i < data->files_data.save_slots; i++) {
				if (data->files_data.full[i]) {
					for (uint8_t j = 0; j < EFFECT_SLOTS_COUNT; j++) {
						uint8_t type = data->files_data.saves[i][j] % EFFECT_TYPES;
						Menu_DrawText(2, Effects_Manifest[type].short_name, 30+j*(40+4), 50+20*i, COLOR_BLACK, COLOR_WHITE, MEDIUM);
					}
				} else {
					Menu_DrawText(2, "empty slot", 30, 50+20*i, COLOR_LGRAY, COLOR_WHITE, MEDIUM);
				}
			}

			LCD_Painter_DrawText("save", ILI9341_HEIGHT-80, 160, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			LCD_Painter_DrawText("load", ILI9341_HEIGHT-80, 180, COLOR_BLACK, COLOR_WHITE, MEDIUM);
			LCD_Painter_DrawText("del.", ILI9341_HEIGHT-80, 200, COLOR_BLACK, COLOR_WHITE, MEDIUM);
		}

	}

	data->pedalboard_changed = 0;
	data->lines_changed = 0;
	data->drums_changed = 0;
	data->cursor1_changed = 0;
	data->cursor2_changed = 0;
	data->text_changed = 0;
	data->signal_changed = 0;
}


void Menu_UpdateFloatValue(float *value, float value_min, float value_max, float micro_step, float macro_step, int32_t micro_counter_curr, int32_t *micro_counter_old, int32_t macro_counter_curr, int32_t *macro_counter_old) {
	*value += micro_step * (float)(micro_counter_curr - *micro_counter_old);
	*value += macro_step * (float)(macro_counter_curr - *macro_counter_old);
	if (*value > value_max) *value = value_max;
	if (*value < value_min) *value = value_min;
	*micro_counter_old = micro_counter_curr;
	*macro_counter_old = macro_counter_curr;
}

void Menu_UpdateIntValue(int32_t *value, int32_t value_min, int32_t value_max, int32_t micro_step, int32_t macro_step, int32_t micro_counter_curr, int32_t *micro_counter_old, int32_t macro_counter_curr, int32_t *macro_counter_old) {
	*value += micro_step * (micro_counter_curr - *micro_counter_old);
	*value += macro_step * (macro_counter_curr - *macro_counter_old);
	if (*value > value_max) *value = value_max;
	if (*value < value_min) *value = value_min;
	*micro_counter_old = micro_counter_curr;
	*macro_counter_old = macro_counter_curr;
}

uint16_t texts_count[TEXT_CLASSES];
Menu_TextToErase texts_to_erase[TEXT_CLASSES][MAX_TEXTS];

void Menu_DrawText(uint8_t class, const char text[], uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, enum Font font) {

	LCD_Painter_DrawText(text, x, y, color, bg_color, font);
	if (class < TEXT_CLASSES && texts_count[class] < MAX_TEXTS) {
		texts_to_erase[class][texts_count[class]].len = strlen(text);
		texts_to_erase[class][texts_count[class]].x = x;
		texts_to_erase[class][texts_count[class]].y = y;
		texts_to_erase[class][texts_count[class]].font = font;
		texts_count[class]++;
	}

}

void Menu_EraseTexts(uint8_t class) {
	if (class < TEXT_CLASSES) {
		for (uint16_t i = 0; i < texts_count[class]; i++) {
			LCD_Painter_EraseText(
					texts_to_erase[class][i].len,
					texts_to_erase[class][i].x,
					texts_to_erase[class][i].y,
					COLOR_WHITE,
					texts_to_erase[class][i].font);
		}
		texts_count[class] = 0;
	}
}

uint16_t plotpoints_count = 0;
Menu_PlotpointsToErase plotpoints_to_erase[MAX_PLOTPOINTS];

void Menu_DrawPlotpoint(uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color) {
	ILI9341_DrawPixel(x, y, color);
	ILI9341_DrawPixel(x, y+1, color);
	if (plotpoints_count < MAX_PLOTPOINTS) {
		plotpoints_to_erase[plotpoints_count].x = x;
		plotpoints_to_erase[plotpoints_count].y = y;
		plotpoints_count++;
	}
}

void Menu_ErasePlotpoints() {
	for (uint16_t i = 0; i < plotpoints_count; i++) {
		ILI9341_DrawPixel(plotpoints_to_erase[i].x, plotpoints_to_erase[i].y, COLOR_WHITE);
		ILI9341_DrawPixel(plotpoints_to_erase[i].x, plotpoints_to_erase[i].y+1, COLOR_WHITE);
	}
	plotpoints_count = 0;
}

#endif
