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
	hcommander->rear = -1;
	hcommander->front = -1;
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

	if(hcommander->front == -1) {
		// underflow
	} else {

		//printf("Element deleted from queue is : %dn",cqueue_arr[front]);
		hcommander->command_callback(hcommander->command_buffer[hcommander->front]);
		hcommander->commands_to_handle--;

		if(hcommander->front == hcommander->rear) {
			hcommander->front = -1;
			hcommander->rear = -1;
		} else {
			if(hcommander->front == COMMANDS_COUNT-1) hcommander->front = 0;
			else hcommander->front++;
		}
	}
}

void Commander_Enqueue(Commander_HandleTypeDef *hcommander, Command *command) {


	if((hcommander->front == 0 && hcommander->rear == COMMANDS_COUNT-1) || (hcommander->front == hcommander->rear+1)) {
		// overflow
	} else {
		if(hcommander->front == -1) {
			hcommander->front = 0;
			hcommander->rear = 0;
		} else {
			if(hcommander->rear == COMMANDS_COUNT-1) hcommander->rear = 0;
			else hcommander->rear++;
		}
		memcpy(hcommander->command_buffer + hcommander->rear, command, COMMAND_BYTESIZE);
		hcommander->commands_to_handle++;
	}


}
