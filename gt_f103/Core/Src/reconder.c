/*
 * reconder.c
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#include "rencoder.h"

void RE_Init(RE_HandleTypeDef *hre, GPIO_TypeDef *portA, GPIO_TypeDef *portB, uint16_t pinA, uint16_t pinB) {
	hre->counter = 0;
	hre->portA = portA;
	hre->portB = portB;
	hre->pinA = pinA;
	hre->pinB = pinB;
	hre->currentA = 0;
	hre->currentB = 0;
	hre->lastA = 0;
	hre->lastB = 0;
}

void RE_Process(RE_HandleTypeDef *hre) {

	uint16_t temp;
	uint16_t alpha = 7; // out of 10
	uint16_t ceil = 2000; //0xFFFF
	uint16_t thr = 1000; //0xEFFF

	hre->lastA = hre->currentA;
	hre->lastB = hre->currentB;

	temp = HAL_GPIO_ReadPin(hre->portA, hre->pinA) == GPIO_PIN_SET ? ceil : 0;
	hre->currentA = (hre->lastA * alpha / 10) + (temp * (10 - alpha) / 10);

	temp = HAL_GPIO_ReadPin(hre->portB, hre->pinB) == GPIO_PIN_SET ? ceil : 0;
	hre->currentB = (hre->lastB * alpha / 10) + (temp * (10 - alpha) / 10);

	if (hre->lastA < thr && hre->currentA >= thr) {

		if (hre->currentB > thr) {
			hre->counter--;
		} else {
			hre->counter++;
		}

	} else if (hre->lastA > thr && hre->currentA <= thr) {

		if (hre->currentB > thr) {
			hre->counter++;
		} else {
			hre->counter--;
		}

	}
}

void RE_Reset(RE_HandleTypeDef *hre) {
	hre->counter = 0;
}

int16_t RE_GetCount(RE_HandleTypeDef *hre) {
	return hre->counter;
}
