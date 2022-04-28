/*
 * commander.h
 *
 *  Created on: Apr 28, 2022
 *      Author: asky
 */

#ifndef COMMANDER_COMMANDER_H_
#define COMMANDER_COMMANDER_H_

#define COMMANDS_COUNT 16
#define COMMAND_BYTESIZE 10

// 1B + 1B + 4B + 4B
// _Command is 10B large
typedef struct {
	uint8_t header;
	uint8_t subheader;
	int32_t arg_int;
	float arg_float;
} Command;

typedef struct {
	UART_HandleTypeDef *huart;
	DMA_HandleTypeDef *hdma_uart_rx;
	uint8_t uart_rx_buffer[COMMAND_BYTESIZE * 2];
	Command command_buffer[COMMANDS_COUNT];
	uint8_t command_index;
	uint8_t commands_to_handle;
	uint32_t counter;
	void (*command_callback)(Command command);
} Commander_HandleTypeDef;

void Commander_Init(Commander_HandleTypeDef *hcommander, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, void (*command_callback)(Command command));

void Commander_Start(Commander_HandleTypeDef *hcommander);

void Commander_Send(Commander_HandleTypeDef *hcommander, Command *command);

void Commander_Process(Commander_HandleTypeDef *hcommander);

void Commander_Enqueue(Commander_HandleTypeDef *hcommander, Command *command);


#endif /* COMMANDER_COMMANDER_H_ */
