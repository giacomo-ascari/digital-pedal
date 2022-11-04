/*
 * commander.h
 *
 *  Created on: Apr 28, 2022
 *      Author: asky
 */

#ifndef COMMANDER_COMMANDER_H_
#define COMMANDER_COMMANDER_H_

#define TIMEOUT 1000

#ifdef F103
#include "stm32f1xx_hal.h"
#endif

#ifdef F407
#include "stm32f4xx_hal.h"
#endif

#define PAYLOAD_BYTESIZE 256

typedef union {
	uint8_t bytes[PAYLOAD_BYTESIZE];
	int16_t shorts[PAYLOAD_BYTESIZE / 2];
	int32_t ints[PAYLOAD_BYTESIZE / 4];
	float floats[PAYLOAD_BYTESIZE / 4];
	char chars[PAYLOAD_BYTESIZE];
} Payload;

typedef struct {
	uint8_t header;
	uint8_t subheader;
	uint8_t param;
	Payload payload;
} Command;

#define COMMAND_BYTESIZE (PAYLOAD_BYTESIZE + 3)

typedef struct {
	UART_HandleTypeDef *huart;
	DMA_HandleTypeDef *hdma_uart_rx;
	uint8_t uart_rx_buffer[COMMAND_BYTESIZE * 2];
	Command in_command;
	Command out_command;
	uint32_t tick_send;
	uint8_t awaiting_response;
	uint8_t command_to_process;
	void (*incoming_callback)(void);
} Commander_HandleTypeDef;

void Commander_Init(Commander_HandleTypeDef *hcommander, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, void (*incoming_callback)(void));

void Commander_Start(Commander_HandleTypeDef *hcommander);

void Commander_Pause(Commander_HandleTypeDef *hcommander);

void Commander_Resume(Commander_HandleTypeDef *hcommander);

uint8_t Commander_SendAndWait(Commander_HandleTypeDef *hcommander);

uint8_t Commander_Send(Commander_HandleTypeDef *hcommander);

void Commander_ProcessIncoming(Commander_HandleTypeDef *hcommander);


#endif
