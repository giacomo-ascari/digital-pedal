/*
 * commander.c
 *
 *  Created on: Apr 28, 2022
 *      Author: asky
 */



#include "commander.h"
#include <string.h>

void Commander_Init(Commander_HandleTypeDef *hcommander, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, void (*incoming_callback)(void)) {
	hcommander->huart = huart;
	hcommander->hdma_uart_rx = hdma_uart_rx;
	hcommander->tick_send = 0;
	hcommander->awaiting_response = 0;
	hcommander->command_to_process = 0;
	hcommander->incoming_callback = incoming_callback;
}

void Commander_Start(Commander_HandleTypeDef *hcommander) {
	HAL_UART_Receive_DMA(hcommander->huart, hcommander->uart_rx_buffer, COMMAND_BYTESIZE * 2);
}

void Commander_Pause(Commander_HandleTypeDef *hcommander) {
	HAL_UART_DMAPause(hcommander->huart);
}

void Commander_Resume(Commander_HandleTypeDef *hcommander) {
	HAL_UART_DMAResume(hcommander->huart);
}

uint8_t Commander_SendAndWait(Commander_HandleTypeDef *hcommander) {

	HAL_StatusTypeDef status;

	hcommander->tick_send = HAL_GetTick();
	hcommander->awaiting_response = 1;

	status = HAL_UART_Transmit(hcommander->huart, (uint8_t *)&(hcommander->out_command), COMMAND_BYTESIZE, 1000);

	if (status == HAL_OK) {

		// active wait
		while (hcommander->awaiting_response /*&& HAL_GetTick() < hcommander->tick_send + TIMEOUT*/);

		if (hcommander->awaiting_response == 0) {
			return 1;
		}
	}

	hcommander->awaiting_response = 0;
	return 0; // transmission error

}

uint8_t Commander_Send(Commander_HandleTypeDef *hcommander) {

	HAL_StatusTypeDef status;


	status = HAL_UART_Transmit(hcommander->huart, (uint8_t *)&(hcommander->out_command), COMMAND_BYTESIZE, 1000);

	if (status == HAL_OK) {

		return 1;

	} else {

		return 0; // transmission error
	}

}

void Commander_ProcessIncoming(Commander_HandleTypeDef *hcommander) {
	if (hcommander->command_to_process) {
		hcommander->command_to_process = 0;
		hcommander->incoming_callback();
	}
}
