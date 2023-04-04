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
	GET_USB = 4,
	SET_USB = 5,
	GET_SPECTRUM = 6,
	GET_LOAD = 7
};

enum Menu_PageType {
	OVERVIEW = 0,
	PLOT = 1,
	EDIT = 2,
	MODE = 3,
	SPECTRUM = 4,
	FILES = 5
};

#ifdef F103

extern const char page_manifest[PAGE_TYPES][20];

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

typedef struct _Menu_Edit_Data {
	uint8_t index1;
	uint8_t index2;
	uint8_t active;
	int16_t oldvalue1;
	int16_t oldvalue2;
	int16_t initialvalue1;
	int16_t initialvalue2;
} Menu_Edit_Data;

typedef struct _Menu_Mode_Data {
	uint8_t input_active;
	uint8_t input_selected;
	uint8_t output_active;
	uint8_t output_selected;
} Menu_Mode_Data;

typedef struct _Menu_Spectrum_Data {
	uint8_t yscale;
	uint8_t xshift;
	uint8_t signal[PAYLOAD_BYTESIZE];
} Menu_Spectrum_Data;

typedef struct _Menu_Files_Data {
	uint8_t usb_ready;
	uint8_t usb_selected;
	int8_t usb_result;
} Menu_Files_Data;

typedef struct _Menu_Data {
	// common stuff
	Commander_HandleTypeDef *hcptr;
	Pedalboard_Handler pedalboard;
	uint32_t tick;
	uint32_t debug;
	enum Menu_PageType page;
	// data for each page type
	Menu_Overview_Data overview_data;
	Menu_Plot_Data plot_data;
	Menu_Edit_Data edit_data;
	Menu_Mode_Data mode_data;
	Menu_Spectrum_Data spectrum_data;
	Menu_Files_Data files_data;
	// stuff that changed
	uint8_t pedalboard_changed;
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

// smart draw and erase

#define TEXT_CLASSES 5
#define MAX_TEXTS 20
typedef struct _Menu_TextToErase {
	uint16_t x;
	uint16_t y;
	uint16_t len;
	enum Font font;
} Menu_TextToErase;


void Menu_DrawText(uint8_t class, char text[], uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color, enum Font font);
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
