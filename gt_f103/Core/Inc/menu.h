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

#define SIGNAL_SIZE 128

enum page_types {
	OVERVIEW,
	PLOT,
	EDIT,
	STATS,
	TUNER,
	FILES,
};

typedef struct _Menu_HandleTypeDef {
	enum page_types selected_page;
	uint8_t usb_ready;
	pedal_union_t pedals[MAX_PEDALS_COUNT];
	uint8_t signal_in[SIGNAL_SIZE];
	uint8_t signal_out[SIGNAL_SIZE];
} Menu_HandleTypeDef;

void Menu_Init(Menu_HandleTypeDef *hm);

void Menu_ParseSignal(Menu_HandleTypeDef *hm, uint8_t *data);

void Menu_Render(Menu_HandleTypeDef *hm, uint8_t *image);

#endif /* MENU_MENU_H_ */
