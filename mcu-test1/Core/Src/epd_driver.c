/*
 * epd_driver.c
 *
 *  Created on: Mar 7, 2022
 *      Author: asky
 */

#include "epd_driver.h"

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

void _EPD_ReadBusy(void) {
	while(1) {
		if(HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) == GPIO_PIN_RESET)
			break;
		HAL_Delay(5);
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



void EPD_Init(void) {
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);

	_EPD_Reset();
	HAL_Delay(100);

	_EPD_ReadBusy();
	_EPD_SendCommand(0x12);  //SWRESET
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

void EPD_Clear(void) {
	uint16_t i;
	_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i = 0; i < EPD_BYTES; i++) {
		_EPD_SendData(0xff);
	}
	_EPD_TurnOnDisplay();
}

void EPD_Display(uint8_t *image) {
	uint16_t i;
	_EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<EPD_BYTES; i++) {
		_EPD_SendData(image[i]);
	}
	_EPD_TurnOnDisplay();
}

void EPD_Display_Partial(uint8_t *image) {
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
		_EPD_SendData(image[i]);
	}
	_EPD_TurnOnDisplay_Partial();
}

void EPD_Sleep(void) {
	_EPD_SendCommand(0x10); //enter deep sleep
	_EPD_SendData(0x01);
	HAL_Delay(100);
}

/*
#include "EPD_Test.h"
#include "EPD_2in9_V2.h"

int EPD_2in9_V2_test(void)
{
    printf("EPD_2IN9_V2_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    printf("e-Paper Init and Clear...\r\n");
	EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);

#if 1  //show image for array
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in9);

    EPD_2IN9_V2_Display(BlackImage);
    DEV_Delay_ms(3000);
#endif

#if 1  // Drawing on the image
	Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 2.Drawing on the image
    printf("Drawing:BlackImage\r\n");
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);

    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);

    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

    Paint_DrawString_CN(130, 0,"你好abc", &Font12CN, BLACK, WHITE);
    Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

    EPD_2IN9_V2_Display_Base(BlackImage);
    DEV_Delay_ms(3000);
#endif

#if 1   //Partial refresh, example shows time
	Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    printf("Partial refresh\r\n");
    Paint_SelectImage(BlackImage);

    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 10;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
        Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
		EPD_2IN9_V2_Display_Partial(BlackImage);
        DEV_Delay_ms(500);//Analog clock 1s
    }
#endif

	printf("Clear...\r\n");
	EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    printf("Goto Sleep...\r\n");
    EPD_2IN9_V2_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
    return 0;
}

 */
