/*
 * commander.c
 *
 *  Created on: Apr 28, 2022
 *      Author: asky
 */

#include "stm32f4xx_hal.h"
#include "commander.h"
#include <string.h>

void Commander_Init(Commander_HandleTypeDef *hcommander, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, void (*command_callback)(Command command)) {
	hcommander->huart = huart;
	hcommander->hdma_uart_rx = hdma_uart_rx;
	hcommander->command_index = 0;
	hcommander->commands_to_handle = 0;
	hcommander->counter = 0;
	hcommander->command_callback = command_callback;
}

void Commander_Start(Commander_HandleTypeDef *hcommander) {
	HAL_UART_Receive_DMA(hcommander->huart, hcommander->uart_rx_buffer, COMMAND_BYTESIZE * 2);
}

void Commander_Send(Commander_HandleTypeDef *hcommander, Command *command) {
	//HAL_StatusTypeDef status;
	/*status = */HAL_UART_Transmit(hcommander->huart, (uint8_t *)command, COMMAND_BYTESIZE, 1000);
}

void Commander_Process(Commander_HandleTypeDef *hcommander) {
	while (hcommander->commands_to_handle) {
		uint8_t index = (hcommander->command_index + hcommander->commands_to_handle) % COMMANDS_COUNT;
		hcommander->command_index++;
		hcommander->command_index %= COMMANDS_COUNT;
		hcommander->commands_to_handle--;
		hcommander->command_callback(hcommander->command_buffer[index]);
	}
}

void Commander_Enqueue(Commander_HandleTypeDef *hcommander, Command *command) {
	uint8_t index = (hcommander->command_index + hcommander->commands_to_handle) % COMMANDS_COUNT;
	memcpy(hcommander->command_buffer + index, command, COMMAND_BYTESIZE);
	hcommander->commands_to_handle++;
	hcommander->counter++;
}
