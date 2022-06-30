/*
 * commander.h
 *
 *  Created on: Apr 28, 2022
 *      Author: asky
 */

#ifndef COMMANDER_COMMANDER_H_
#define COMMANDER_COMMANDER_H_

#define COMMANDS_COUNT 10
#define COMMAND_BYTESIZE 259

#ifdef F103
#include "stm32f1xx_hal.h"
#endif

#ifdef F407
#include "stm32f4xx_hal.h"
#endif

typedef union {
	uint8_t bytes[256];
	int16_t shorts[128];
	int32_t ints[64];
	float floats[64];
	char chars[256];
} Payload;

typedef struct {
	uint8_t header; // 1 Byte
	uint8_t subheader; // 1 Byte
	uint8_t update; // 1 Byte
	Payload payload; // 256 Bytes
} Command;

typedef struct {
	UART_HandleTypeDef *huart;
	DMA_HandleTypeDef *hdma_uart_rx;
	uint8_t uart_rx_buffer[COMMAND_BYTESIZE * 2];
	Command command_buffer[COMMANDS_COUNT];
	int8_t rear;
	int8_t front;
	uint8_t commands_to_handle;
	uint32_t counter;
	void (*command_callback)(Command command);
} Commander_HandleTypeDef;

void Commander_Init(Commander_HandleTypeDef *hcommander, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, void (*command_callback)(Command command));

void Commander_Start(Commander_HandleTypeDef *hcommander);

void Commander_Pause(Commander_HandleTypeDef *hcommander);

void Commander_Resume(Commander_HandleTypeDef *hcommander);

void Commander_Send(Commander_HandleTypeDef *hcommander, Command *command);

void Commander_Process(Commander_HandleTypeDef *hcommander);

void Commander_Enqueue(Commander_HandleTypeDef *hcommander, Command *command);


#endif /* COMMANDER_COMMANDER_H_ */
