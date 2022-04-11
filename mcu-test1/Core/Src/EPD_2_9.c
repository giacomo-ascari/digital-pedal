/*
 * epd_driver.c
 *
 *  Created on: Mar 7, 2022
 *      Author: asky
 */

#include "EPD_2_9.h"

uint16_t state;
uint16_t phase;

uint8_t _WF_PARTIAL_2IN9[159] = { 0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0A,0x0,0x0,0x0,0x0,0x0,0x2,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,0x22,0x17,0x41,0xB0,0x32,0x36 };

uint8_t WS_20_30[159] = { 0x80,0x66,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x10,0x66,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x0,0x0,0x0,0x80,0x66,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x10,0x66,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x14,0x8,0x0,0x0,0x0,0x0,0x1,0xA,0xA,0x0,0xA,0xA,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x14,0x8,0x0,0x1,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x44,0x44,0x44,0x44,0x44,0x44,0x0,0x0,0x0,0x22,0x17,0x41,0x0,0x32,0x36 };

extern SPI_HandleTypeDef hspi1;

static void _EPD_Reset(void) {
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

static void _EPD_SendCommand(uint8_t reg) {
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

static void _EPD_SendData(uint8_t data) {
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

static void _EPD_SendDataArray(uint8_t *array, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		_EPD_SendData(array[i]);
	}
}

void _EPD_ReadBusy(void) {
	while(1) {
		if(HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) == GPIO_PIN_RESET)
			break;
		HAL_Delay(1);
	}
}

static void _EPD_LUT(uint8_t *lut) {
	uint8_t count;
	_EPD_SendCommand(0x32);
	for(count=0; count<153; count++)
		_EPD_SendData(lut[count]);
	_EPD_ReadBusy();
}

static void _EPD_LUT_by_host(uint8_t *lut) {
	_EPD_LUT((uint8_t *)lut);			//lut
	_EPD_SendCommand(0x3f);
	_EPD_SendData(*(lut+153));
	_EPD_SendCommand(0x03);	// gate voltage
	_EPD_SendData(*(lut+154));
	_EPD_SendCommand(0x04);	// source voltage
	_EPD_SendData(*(lut+155));	// VSH
	_EPD_SendData(*(lut+156));	// VSH2
	_EPD_SendData(*(lut+157));	// VSL
	_EPD_SendCommand(0x2c);		// VCOM
	_EPD_SendData(*(lut+158));
}

static void _EPD_TurnOnDisplay(void) {
	_EPD_SendCommand(0x22); //Display Update Control
	_EPD_SendData(0xc7);
	_EPD_SendCommand(0x20); //Activate Display Update Sequence
	_EPD_ReadBusy();
}

static void _EPD_TurnOnDisplay_Partial(void) {
	_EPD_SendCommand(0x22); //Display Update Control
	_EPD_SendData(0x0F);
	_EPD_SendCommand(0x20); //Activate Display Update Sequence
	_EPD_ReadBusy();
}

static void _EPD_SetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
	_EPD_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
	_EPD_SendData((Xstart>>3) & 0xFF);
	_EPD_SendData((Xend>>3) & 0xFF);

	_EPD_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
	_EPD_SendData(Ystart & 0xFF);
	_EPD_SendData((Ystart >> 8) & 0xFF);
	_EPD_SendData(Yend & 0xFF);
	_EPD_SendData((Yend >> 8) & 0xFF);
}

static void _EPD_SetCursor(uint16_t Xstart, uint16_t Ystart) {
	_EPD_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
	_EPD_SendData(Xstart & 0xFF);

	_EPD_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
	_EPD_SendData(Ystart & 0xFF);
	_EPD_SendData((Ystart >> 8) & 0xFF);
}

void EPD_AsyncIdle(EPD_HandleTypeDef *hepd) {
	hepd->state = EPD_IDLE_STATE;
	hepd->phase = 0;
}

void EPD_AsyncState(EPD_HandleTypeDef *hepd, uint8_t state) {
	hepd->state = state;
	hepd->phase = 0;
}

void EPD_AsyncProcess(EPD_HandleTypeDef *hepd) {

	if (HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) == GPIO_PIN_SET
			 || hepd->state == EPD_IDLE_STATE) return;

	if (hepd->state == EPD_INIT_STATE) {
		if (hepd->phase == 0) {
			// Init (hw reset)
			HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
			_EPD_Reset();
			HAL_Delay(100);
			hepd->phase = 1;
		} else if (hepd->phase == 1) {
			// Init (sw reset)
			_EPD_SendCommand(0x12);  //SWRESET
			hepd->phase = 2;
		} else if (hepd->phase == 2) {
			// Init (driver and stuff)
			_EPD_SendCommand(0x01); //Driver output control
			_EPD_SendData(0x27);
			_EPD_SendData(0x01);
			_EPD_SendData(0x00);
			_EPD_SendCommand(0x11); //data entry mode
			_EPD_SendData(0x03);
			_EPD_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
			_EPD_SendCommand(0x21); //  Display update control
			_EPD_SendData(0x00);
			_EPD_SendData(0x80);
			_EPD_SetCursor(0, 0);
			hepd->phase = 3;
		} else if (hepd->phase == 3) {
			// lut
			uint8_t count;
			_EPD_SendCommand(0x32);
			for(count=0; count<153; count++)
				_EPD_SendData(WS_20_30[count]);
			hepd->phase = 4;
		} else if (hepd->phase == 4) {
			// lut by host
			_EPD_SendCommand(0x3f);
			_EPD_SendData(*(WS_20_30+153));
			_EPD_SendCommand(0x03);	// gate voltage
			_EPD_SendData(*(WS_20_30+154));
			_EPD_SendCommand(0x04);	// source voltage
			_EPD_SendData(*(WS_20_30+155));	// VSH
			_EPD_SendData(*(WS_20_30+156));	// VSH2
			_EPD_SendData(*(WS_20_30+157));	// VSL
			_EPD_SendCommand(0x2c);		// VCOM
			_EPD_SendData(*(WS_20_30+158));
			EPD_AsyncState(hepd, EPD_IDLE_STATE);
		}
	} else if (hepd->state == EPD_CLEAR_STATE) {
		if (hepd->phase == 0) {
			// Clear
			uint16_t i;
			_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
			for(i = 0; i < EPD_BYTES; i++) {
				_EPD_SendData(0xff);
			}
			// Clear -> Turn on display
			_EPD_SendCommand(0x22); //Display Update Control
			_EPD_SendData(0xc7);
			_EPD_SendCommand(0x20); //Activate Display Update Sequence
			EPD_AsyncState(hepd, EPD_IDLE_STATE);
		}
	} else if (hepd->state == EPD_DISPLAY_STATE) {
		if (hepd->phase == 0) {
			// Display
			uint16_t i;
			_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
			for(i=0; i<EPD_BYTES; i++) {
				_EPD_SendData(hepd->image[i]);
			}
			// Display -> Turn on display
			_EPD_SendCommand(0x22); //Display Update Control
			_EPD_SendData(0xc7);
			_EPD_SendCommand(0x20); //Activate Display Update Sequence
			EPD_AsyncState(hepd, EPD_IDLE_STATE);
		}
	} else if (hepd->state == EPD_SLEEP_STATE) {
		if (hepd->phase == 0) {
			// Sleep
			_EPD_SendCommand(0x10); //enter deep sleep
			_EPD_SendData(0x01);
			HAL_Delay(100);
			EPD_AsyncState(hepd, EPD_IDLE_STATE);
		}
	}
}


void EPD_Init(EPD_HandleTypeDef *hepd) {

	// start phase 0
	//hepd->state = INIT_STATE;
	//hepd->phase = 0;
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
	_EPD_Reset();
	HAL_Delay(100);
	// end phase 0

	_EPD_ReadBusy();

	// start phase 1
	_EPD_SendCommand(0x12);  //SWRESET
	// end phase 1

	_EPD_ReadBusy();

	_EPD_SendCommand(0x01); //Driver output control
	_EPD_SendData(0x27);
	_EPD_SendData(0x01);
	_EPD_SendData(0x00);

	_EPD_SendCommand(0x11); //data entry mode
	_EPD_SendData(0x03);

	_EPD_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);

	_EPD_SendCommand(0x21); //  Display update control
	_EPD_SendData(0x00);
	_EPD_SendData(0x80);

	_EPD_SetCursor(0, 0);
	_EPD_ReadBusy();

	_EPD_LUT_by_host(WS_20_30);
}

void EPD_Clear(EPD_HandleTypeDef *hepd) {
	uint16_t i;
	_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i = 0; i < EPD_BYTES; i++) {
		_EPD_SendData(0xff);
	}
	_EPD_TurnOnDisplay();
}

void EPD_Display(EPD_HandleTypeDef *hepd) {
	uint16_t i;
	_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<EPD_BYTES; i++) {
		_EPD_SendData(hepd->image[i]);
	}
	_EPD_TurnOnDisplay();
}

void EPD_Display_Partial(EPD_HandleTypeDef *hepd) {
	uint16_t i;

	//Reset
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(2);

	_EPD_LUT(_WF_PARTIAL_2IN9);
	_EPD_SendCommand(0x37);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x40);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);
	_EPD_SendData(0x00);

	_EPD_SendCommand(0x3C); //BorderWavefrom
	_EPD_SendData(0x80);

	_EPD_SendCommand(0x22);
	_EPD_SendData(0xC0);
	_EPD_SendCommand(0x20);
	_EPD_ReadBusy();

	_EPD_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
	_EPD_SetCursor(0, 0);

	_EPD_SendCommand(0x24);   //Write Black and White image to RAM
	for(i=0;i<EPD_BYTES;i++)	{
		_EPD_SendData(hepd->image[i]);
	}
	_EPD_TurnOnDisplay_Partial();
}

void EPD_Sleep(EPD_HandleTypeDef *hepd) {
	_EPD_SendCommand(0x10); //enter deep sleep
	_EPD_SendData(0x01);
	HAL_Delay(100);
}
