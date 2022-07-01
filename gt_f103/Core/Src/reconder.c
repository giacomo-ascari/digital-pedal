/*
 * reconder.c
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#include "rencoder.h"

void RE_Init(RE_HandleTypeDef *hre, GPIO_TypeDef *portA, GPIO_TypeDef *portB, uint16_t pinA, uint16_t pinB, int16_t divider) {
	hre->counter = 0;
	hre->portA = portA;
	hre->portB = portB;
	hre->pinA = pinA;
	hre->pinB = pinB;
	hre->divider = divider;
	hre->stateB[0] = 0;
	hre->stateB[1] = 0;
}

uint16_t debouncing_count = 16;

int8_t jenky_debounce(GPIO_TypeDef *port, uint16_t pin) {

	int8_t res = HAL_GPIO_ReadPin(port, pin);
	for (uint16_t i = 1; i < debouncing_count; i++) {
		if (res != HAL_GPIO_ReadPin(port, pin)) {
			res = -1;
			return res;
		}
	}
	return res;

}

uint8_t RE_Process(RE_HandleTypeDef *hre) {
	int8_t risingOnA = jenky_debounce(hre->portA, hre->pinA);
	int8_t stateOnB = jenky_debounce(hre->portB, hre->pinB);


	if (risingOnA == GPIO_PIN_SET && stateOnB != -1) {
		// rising edge
		hre->stateB[0] = stateOnB;
	} else if (risingOnA == GPIO_PIN_RESET && stateOnB != -1) {
		// falling edge
		hre->stateB[1] = stateOnB;
		hre->counter += hre->stateB[1] - hre->stateB[0];
		if (hre->stateB[1] != hre->stateB[0]) return 1;
	}

	return 0;
}

int16_t RE_GetCount(RE_HandleTypeDef *hre) {
	if (hre->divider <= 1) {
		return hre->counter;
	} else {
		return hre->counter / hre->divider;
	}
}
