/*
 * menu.h
 *
 *  Created on: Jun 14, 2022
 *      Author: asky
 */

#ifndef MENU_MENU_H_
#define MENU_MENU_H_

#include "stm32f1xx_hal.h"
#include "pedalboard.h"
#include "commander.h"
#include "epddriver.h"
#include "painter2.h"
#include "mode.h"

#define SIGNAL_SIZE 128

enum page_types {
	OVERVIEW = 0,
	PLOT = 1,
	EDIT = 2,
	MODE = 3,
	TUNER = 4,
	FILES = 5
};

enum message_types {
	FIRST = 0,
	PERIODIC = 1,
	USER = 2
};

enum render_types {
	FULL = 0,
	PARTIAL = 1
};

typedef struct _Menu_HandleTypeDef {
	enum page_types selected_page;
	pedal_union_t pedals[MAX_PEDALS_COUNT];
	pedal_manifest_t pedal_manifest[PEDAL_TYPES];
	mode_manifest_t mode_manifest[MODE_TYPES];
	int8_t signal_in[SIGNAL_SIZE];
	int8_t signal_out[SIGNAL_SIZE];
	Command command;
	Commander_HandleTypeDef *hcommander;
	EPD_HandleTypeDef *hepd;
	// state related variables
	uint8_t plot_xscale;
	uint8_t plot_yscale;
	uint8_t edit_index1;
	uint8_t edit_index2;
	uint8_t edit_active;
	uint8_t mode_active;
	uint8_t mode_selected;
	uint8_t usb_ready;
	uint32_t tick;
	// desperate debug
	uint32_t debug;
} Menu_HandleTypeDef;

void Menu_Init(Menu_HandleTypeDef *hm, Commander_HandleTypeDef *hcommander, EPD_HandleTypeDef *hepd);

void Menu_Render(Menu_HandleTypeDef *hm, enum render_types render);

void Menu_SendMessage(Menu_HandleTypeDef *hm, enum message_types type);

void Menu_UpdatePage(Menu_HandleTypeDef *hm);

#endif /* MENU_MENU_H_ */
