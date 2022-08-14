/*
 * rencoder.h
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#ifndef ROTARYENCODER_RENCODER_H_
#define ROTARYENCODER_RENCODER_H_

#include "stm32f1xx_hal.h"

typedef struct _RE_HandleTypeDef {
	GPIO_TypeDef *portA, *portB;
	uint16_t pinA, pinB;
	int16_t counter;
	uint16_t currentA, lastA;
	uint16_t currentB, lastB;
} RE_HandleTypeDef;

void RE_Init(RE_HandleTypeDef *hre, GPIO_TypeDef *portA, GPIO_TypeDef *portB, uint16_t pinA, uint16_t pinB);

void RE_Process(RE_HandleTypeDef *hre);

void RE_Reset(RE_HandleTypeDef *hre);

uint8_t RE_GetCount(RE_HandleTypeDef *hre, uint8_t max);

#endif /* ROTARYENCODER_RENCODER_H_ */
