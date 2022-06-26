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
#include "painter2.h"

#define SIGNAL_SIZE 128

enum page_types {
	OVERVIEW = 0,
	PLOT = 1,
	EDIT = 2,
	STATS = 3,
	TUNER = 4,
	FILES = 5,
};

typedef struct _Menu_HandleTypeDef {
	enum page_types selected_page;
	uint8_t usb_ready;
	pedal_union_t pedals[MAX_PEDALS_COUNT];
	uint8_t signal_in[SIGNAL_SIZE];
	uint8_t signal_out[SIGNAL_SIZE];
	Command command;
	Commander_HandleTypeDef *hcommander;
} Menu_HandleTypeDef;

void Menu_Init(Menu_HandleTypeDef *hm);

void Menu_Render(Menu_HandleTypeDef *hm, uint8_t *image);

void Menu_GoTo(Menu_HandleTypeDef *hm, enum page_types new_page);

#endif /* MENU_MENU_H_ */
