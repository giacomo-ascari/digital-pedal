/*
 * epd_driver.c
 *
 *  Created on: Mar 7, 2022
 *      Author: asky
 */

#include "epd_driver.h"

void EPD_Init(void) {
	EPD_2IN9_V2_Reset();
	DEV_Delay_ms(100);

	EPD_2IN9_V2_ReadBusy();
	EPD_2IN9_V2_SendCommand(0x12);  //SWRESET
	EPD_2IN9_V2_ReadBusy();

	EPD_2IN9_V2_SendCommand(0x01); //Driver output control
	EPD_2IN9_V2_SendData(0x27);
	EPD_2IN9_V2_SendData(0x01);
	EPD_2IN9_V2_SendData(0x00);

	EPD_2IN9_V2_SendCommand(0x11); //data entry mode
	EPD_2IN9_V2_SendData(0x03);

	EPD_2IN9_V2_SetWindows(0, 0, EPD_2IN9_V2_WIDTH-1, EPD_2IN9_V2_HEIGHT-1);

		// EPD_2IN9_V2_SendCommand(0x3C); //BorderWavefrom
		// EPD_2IN9_V2_SendData(0x05);

	EPD_2IN9_V2_SendCommand(0x21); //  Display update control
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x80);

	EPD_2IN9_V2_SetCursor(0, 0);
	EPD_2IN9_V2_ReadBusy();

	EPD_2IN9_V2_LUT_by_host(WS_20_30);
}
void EPD_Clear(void);
void EPD_Display(uint8_t *image);
void EPD_Display_Base(uint8_t *image);
void EPD_Display_Partial(uint8_t *image);
void EPD_Sleep(void);

static void EPD_2IN9_V2_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(100);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2IN9_V2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2IN9_V2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_2IN9_V2_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
	while(1)
	{	 //=1 BUSY
		if(DEV_Digital_Read(EPD_BUSY_PIN)==0)
			break;
		DEV_Delay_ms(50);
	}
	DEV_Delay_ms(50);
    Debug("e-Paper busy release\r\n");
}

static void EPD_2IN9_V2_LUT(UBYTE *lut)
{
	UBYTE count;
	EPD_2IN9_V2_SendCommand(0x32);
	for(count=0; count<153; count++)
		EPD_2IN9_V2_SendData(lut[count]);
	EPD_2IN9_V2_ReadBusy();
}

static void EPD_2IN9_V2_LUT_by_host(UBYTE *lut)
{
	EPD_2IN9_V2_LUT((UBYTE *)lut);			//lut
	EPD_2IN9_V2_SendCommand(0x3f);
	EPD_2IN9_V2_SendData(*(lut+153));
	EPD_2IN9_V2_SendCommand(0x03);	// gate voltage
	EPD_2IN9_V2_SendData(*(lut+154));
	EPD_2IN9_V2_SendCommand(0x04);	// source voltage
	EPD_2IN9_V2_SendData(*(lut+155));	// VSH
	EPD_2IN9_V2_SendData(*(lut+156));	// VSH2
	EPD_2IN9_V2_SendData(*(lut+157));	// VSL
	EPD_2IN9_V2_SendCommand(0x2c);		// VCOM
	EPD_2IN9_V2_SendData(*(lut+158));

}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2IN9_V2_TurnOnDisplay(void)
{
	EPD_2IN9_V2_SendCommand(0x22); //Display Update Control
	EPD_2IN9_V2_SendData(0xc7);
	EPD_2IN9_V2_SendCommand(0x20); //Activate Display Update Sequence
	EPD_2IN9_V2_ReadBusy();
}

static void EPD_2IN9_V2_TurnOnDisplay_Partial(void)
{
	EPD_2IN9_V2_SendCommand(0x22); //Display Update Control
	EPD_2IN9_V2_SendData(0x0F);
	EPD_2IN9_V2_SendCommand(0x20); //Activate Display Update Sequence
	EPD_2IN9_V2_ReadBusy();
}

/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
static void EPD_2IN9_V2_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_2IN9_V2_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_2IN9_V2_SendData((Xstart>>3) & 0xFF);
    EPD_2IN9_V2_SendData((Xend>>3) & 0xFF);

    EPD_2IN9_V2_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_2IN9_V2_SendData(Ystart & 0xFF);
    EPD_2IN9_V2_SendData((Ystart >> 8) & 0xFF);
    EPD_2IN9_V2_SendData(Yend & 0xFF);
    EPD_2IN9_V2_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
static void EPD_2IN9_V2_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_2IN9_V2_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_2IN9_V2_SendData(Xstart & 0xFF);

    EPD_2IN9_V2_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_2IN9_V2_SendData(Ystart & 0xFF);
    EPD_2IN9_V2_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2IN9_V2_Init(void)
{
	EPD_2IN9_V2_Reset();
	DEV_Delay_ms(100);

	EPD_2IN9_V2_ReadBusy();
	EPD_2IN9_V2_SendCommand(0x12);  //SWRESET
	EPD_2IN9_V2_ReadBusy();

	EPD_2IN9_V2_SendCommand(0x01); //Driver output control
	EPD_2IN9_V2_SendData(0x27);
	EPD_2IN9_V2_SendData(0x01);
	EPD_2IN9_V2_SendData(0x00);

	EPD_2IN9_V2_SendCommand(0x11); //data entry mode
	EPD_2IN9_V2_SendData(0x03);

	EPD_2IN9_V2_SetWindows(0, 0, EPD_2IN9_V2_WIDTH-1, EPD_2IN9_V2_HEIGHT-1);

	// EPD_2IN9_V2_SendCommand(0x3C); //BorderWavefrom
	// EPD_2IN9_V2_SendData(0x05);

	EPD_2IN9_V2_SendCommand(0x21); //  Display update control
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x80);

	// EPD_2IN9_V2_SendCommand(0x18); //Read built-in temperature sensor
	// EPD_2IN9_V2_SendData(0x80);

	EPD_2IN9_V2_SetCursor(0, 0);
	EPD_2IN9_V2_ReadBusy();

	EPD_2IN9_V2_LUT_by_host(WS_20_30);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2IN9_V2_Clear(void)
{
	UWORD i;
	EPD_2IN9_V2_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<4736;i++)
	{
		EPD_2IN9_V2_SendData(0xff);
	}
	EPD_2IN9_V2_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_2IN9_V2_Display(UBYTE *Image)
{
	UWORD i;
	EPD_2IN9_V2_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<4736;i++)
	{
		EPD_2IN9_V2_SendData(Image[i]);
	}
	EPD_2IN9_V2_TurnOnDisplay();
}

void EPD_2IN9_V2_Display_Base(UBYTE *Image)
{
	UWORD i;

	EPD_2IN9_V2_SendCommand(0x24);   //Write Black and White image to RAM
	for(i=0;i<4736;i++)
	{
		EPD_2IN9_V2_SendData(Image[i]);
	}
	EPD_2IN9_V2_SendCommand(0x26);   //Write Black and White image to RAM
	for(i=0;i<4736;i++)
	{
		EPD_2IN9_V2_SendData(Image[i]);
	}
	EPD_2IN9_V2_TurnOnDisplay();
}

void EPD_2IN9_V2_Display_Partial(UBYTE *Image)
{
	UWORD i;

//Reset
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(2);

	EPD_2IN9_V2_LUT(_WF_PARTIAL_2IN9);
	EPD_2IN9_V2_SendCommand(0x37);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x40);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);
	EPD_2IN9_V2_SendData(0x00);

	EPD_2IN9_V2_SendCommand(0x3C); //BorderWavefrom
	EPD_2IN9_V2_SendData(0x80);

	EPD_2IN9_V2_SendCommand(0x22);
	EPD_2IN9_V2_SendData(0xC0);
	EPD_2IN9_V2_SendCommand(0x20);
	EPD_2IN9_V2_ReadBusy();

	EPD_2IN9_V2_SetWindows(0, 0, EPD_2IN9_V2_WIDTH-1, EPD_2IN9_V2_HEIGHT-1);
	EPD_2IN9_V2_SetCursor(0, 0);

	EPD_2IN9_V2_SendCommand(0x24);   //Write Black and White image to RAM
	for(i=0;i<4736;i++)
	{
		EPD_2IN9_V2_SendData(Image[i]);
	}
	EPD_2IN9_V2_TurnOnDisplay_Partial();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2IN9_V2_Sleep(void)
{
	EPD_2IN9_V2_SendCommand(0x10); //enter deep sleep
	EPD_2IN9_V2_SendData(0x01);
	DEV_Delay_ms(100);
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
