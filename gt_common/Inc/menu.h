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
#include "epddriver.h"
#include "painter2.h"

#endif

#define SIGNAL_SIZE 128
#define PAGE_COUNT 6

// Commands are from F103 (ui) perspective
// since it's the one that initiates comunication
enum command_header_types {
	OTHER = 0,
	GET_PB = 1,
	SET_PB = 2,
	GET_SIGNALS = 3,
	GET_USB = 4,
	SET_USB = 5,
	GET_SPECTRUM = 6
};

enum page_types {
	OVERVIEW = 0,
	PLOT = 1,
	EDIT = 2,
	MODE = 3,
	SPECTRUM = 4,
	FILES = 5
};

enum render_types {
	FULL = 0,
	PARTIAL = 1
};

#ifdef F103

typedef struct _Menu_HandleTypeDef {
	enum page_types selected_page;
	Pedalboard_Handler pedalboard;
	int8_t signal_in[SIGNAL_SIZE];
	int8_t signal_out[SIGNAL_SIZE];
	uint8_t spectrum[PAYLOAD_BYTESIZE];
	Commander_HandleTypeDef *hcommander;
	EPD_HandleTypeDef *hepd;
	// state related variables
	uint8_t plot_xscale;
	uint8_t plot_yscale;
	uint8_t edit_index1;
	uint8_t edit_index2;
	uint8_t edit_active;
	int16_t edit_oldvalue1;
	int16_t edit_oldvalue2;
	int16_t edit_initialvalue1;
	int16_t edit_initialvalue2;
	uint8_t mode_input_active;
	uint8_t mode_input_selected;
	uint8_t mode_output_active;
	uint8_t mode_output_selected;
	uint8_t usb_ready;
	uint8_t usb_selected;
	int8_t usb_result;
	uint32_t tick;
	// desperate debug
	uint32_t debug;
} Menu_HandleTypeDef;

uint8_t Menu_GoTo(Menu_HandleTypeDef *hm, enum page_types new_page);

void Menu_Init(Menu_HandleTypeDef *hm, Commander_HandleTypeDef *hcommander, EPD_HandleTypeDef *hepd);

void Menu_Sync(Menu_HandleTypeDef *hm, enum command_header_types type);

void Menu_Render(Menu_HandleTypeDef *hm, enum render_types render);

#endif

#endif /* MENU_MENU_H_ */
