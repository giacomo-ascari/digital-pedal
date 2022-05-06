/*
 * reconder.c
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#include "rencoder.h"

void RE_Init(RE_HandleTypeDef *hre, GPIO_TypeDef *portA, GPIO_TypeDef *portB, uint16_t pinA, uint16_t pinB) {
	hre->lasttick = 0;
	hre->lastA = 0;
	hre->lastB = 0;
	hre->currentA = 0;
	hre->currentB = 0;
	hre->counter = 0;
	hre->portA = portA;
	hre->portB = portB;
	hre->pinA = pinA;
	hre->pinB = pinB;
}

uint8_t RE_Process(RE_HandleTypeDef *hre) {
	if (HAL_GetTick() != hre->lasttick) {
		hre->lasttick = HAL_GetTick();
		hre->lastA = hre->currentA;
		hre->lastB = hre->currentB;

		uint16_t temp;
		uint16_t alpha = 75; //%
		uint16_t ceil = 10000, thr = 5000;

		temp = HAL_GPIO_ReadPin(hre->portA, hre->pinA) == GPIO_PIN_SET ? ceil : 0;
		hre->currentA = (hre->lastA * alpha / 100) + (temp * (100 - alpha) / 100);

		temp = HAL_GPIO_ReadPin(hre->portB, hre->pinB) == GPIO_PIN_SET ? ceil : 0;
		hre->currentB = (hre->lastB * alpha / 100) + (temp * (100 - alpha) / 100);

		if (hre->lastA < thr && hre->currentA >= thr) {

			if (hre->currentB > thr) {
				hre->counter--;
			} else {
				hre->counter++;
			}
			return 1;

		} else if (hre->lastA > thr && hre->currentA <= thr) {

			if (hre->currentB > thr) {
				hre->counter++;
			} else {
				hre->counter--;
			}
			return 1;

		}
	}
	return 0;
}
