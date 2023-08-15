/*
 * menu.h
 *
 *  Created on: Jun 14, 2022
 *      Author: asky
 */

#ifndef MENU_MENU_H_
#define MENU_MENU_H_

#ifdef F103

#include "stm32f1xx_hal.h"

#include "pedalboard.h"
#include "commander.h"
#include "drummachine.h"
#include "lines.h"

#include "ili9341.h"
#include "lcd_painter.h"

#include "main.h"

#endif

#define SIGNAL_SIZE 128
#define PAGE_TYPES 6

// Commands are from F103 (ui) perspective
enum Menu_CommandHeader {
	OTHER = 0,
	GET_PB = 1,
	SET_PB = 2,
	GET_SIGNALS = 3,
	GET_SPECTRUM = 4,
	GET_LOAD = 5,
	GET_FLASH = 6,
	SAVE_FLASH = 7,
	LOAD_FLASH = 8,
	DEL_FLASH = 9,
	GET_DRUMS = 10,
	SET_DRUMS = 11,
	GET_LINES = 12,
	SET_LINES = 13
};

enum Menu_PageType {
	OVERVIEW = 0,
	PLOT = 1,
	PEDALS = 2,
	LINES = 3,
	DRUMS = 4,
	FILES = 5
};

#ifdef F103

extern const char page_manifest[PAGE_TYPES][10];

// interval in ms for each page to issue a new command
extern const uint16_t sync_interval[PAGE_TYPES];

typedef struct _Menu_Overview_Data {
	uint16_t elapsedus;
} Menu_Overview_Data;

typedef struct _Menu_Plot_Data {
	uint8_t xscale;
	uint8_t yscale;
	int8_t signal_in[SIGNAL_SIZE];
	int8_t signal_out[SIGNAL_SIZE];
} Menu_Plot_Data;

typedef struct _Menu_Pedals_Data {
	uint8_t index1;
	uint8_t index2;
	uint8_t active;
	int32_t oldvalue1;
	int32_t oldvalue2;
	int32_t initialvalue1;
	int32_t initialvalue2;
} Menu_Pedals_Data;

typedef struct _Menu_Lines_Data {
	uint8_t input_selected;
	uint8_t output_selected;
} Menu_Lines_Data;

typedef struct _Menu_Drums_Data {
	uint8_t index1;
	uint8_t index2;
	uint8_t active;
	int32_t oldvalue1;
	int32_t oldvalue2;
	int32_t initialvalue1;
	int32_t initialvalue2;
} Menu_Drums_Data;

typedef struct _Menu_Files_Data {
	uint8_t save_slots;
	uint8_t saves[16][EFFECT_SLOTS_COUNT];
	uint8_t full[16];
	uint8_t slot_selected;
	uint8_t action_selected;
} Menu_Files_Data;

typedef struct _Menu_Data {
	// common stuff
	Commander_HandleTypeDef *hcptr;
	Pedalboard_Handler pedalboard;
	drum_conf_t drums_conf;
	lines_conf_t lines_conf;
	uint32_t tick;
	uint32_t debug;
	enum Menu_PageType page;
	// data for each page type
	Menu_Overview_Data overview_data;
	Menu_Plot_Data plot_data;
	Menu_Pedals_Data pedals_data;
	Menu_Lines_Data lines_data;
	Menu_Drums_Data drums_data;
	Menu_Files_Data files_data;
	// stuff that changed
	uint8_t pedalboard_changed;
	uint8_t lines_changed;
	uint8_t drums_changed;
	uint8_t cursor1_changed;
	uint8_t cursor2_changed;
	uint8_t text_changed;
	uint8_t signal_changed;
} Menu_Data;

// setting page

uint8_t Menu_GoTo(Menu_Data *data, enum Menu_PageType new_page, uint8_t forced);

// init

void Menu_Init(Menu_Data *data, Commander_HandleTypeDef *hcptr);

// use commander to ask for stuff

void Menu_Sync(Menu_Data *data, enum Menu_CommandHeader type);

// render

void Menu_Render(Menu_Data *data);

// update values

void Menu_UpdateFloatValue(
		float *value,
		float value_min, float value_max,
		float micro_step, float macro_step,
		int32_t micro_counter_curr, int32_t *micro_counter_old,
		int32_t macro_counter_curr, int32_t *macro_counter_old);

void Menu_UpdateIntValue(
		int32_t *value,
		int32_t value_min, int32_t value_max,
		int32_t micro_step, int32_t macro_step,
		int32_t micro_counter_curr, int32_t *micro_counter_old,
		int32_t macro_counter_curr, int32_t *macro_counter_old);

// smart draw and erase

#define TEXT_CLASSES 5
#define MAX_TEXTS 50
typedef struct _Menu_TextToErase {
	uint16_t x;
	uint16_t y;
	uint16_t len;
	enum Font font;
} Menu_TextToErase;


void Menu_DrawText(uint8_t class, const char text[], uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, enum Font font);
void Menu_EraseTexts(uint8_t class);

#define MAX_PLOTPOINTS 300
typedef struct _Menu_PlotpointsToErase {
	uint16_t x;
	uint16_t y;
} Menu_PlotpointsToErase;


void Menu_DrawPlotpoint(uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color);
void Menu_ErasePlotpoints();

#endif

#endif /* MENU_MENU_H_ */
