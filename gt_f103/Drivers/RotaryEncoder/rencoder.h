/*
 * rencoder.h
 *
 *  Created on: May 6, 2022
 *      Author: asky
 */

#ifndef ROTARYENCODER_RENCODER_H_
#define ROTARYENCODER_RENCODER_H_

#include "main.h"

typedef struct _RE_HandleTypeDef {
	uint32_t lasttick;
	uint16_t lastA, lastB;
	uint16_t currentA, currentB;
	GPIO_TypeDef *portA, *portB;
	uint16_t pinA, pinB;
	int16_t counter;
} RE_HandleTypeDef;

void RE_Init(RE_HandleTypeDef *hre, GPIO_TypeDef *portA, GPIO_TypeDef *portB, uint16_t pinA, uint16_t pinB);

uint8_t RE_Process(RE_HandleTypeDef *hre);

#endif /* ROTARYENCODER_RENCODER_H_ */
