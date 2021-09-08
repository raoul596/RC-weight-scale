/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

/**********************************************************************
 Requiem Projects (c) 2016

 Modification for the DMA use

 The Tilen Library was modify by Requiem to use the DMA transfer to
 enhance the speed of the SPI data transfer to control the ili9341
 Display Driver

 **********************************************************************/
#include "main.h"
#include "tm_stm32_ili9341.h"
#include "Digits.h"
#include "RPM.h"

SPI_HandleTypeDef hspi1;

/**
 * @brief  Orientation
 * @note   Used private
 */
typedef enum
{
	TM_ILI9341_Landscape,
	TM_ILI9341_Portrait
} TM_ILI9341_Orientation;

/**
 * @brief  LCD options
 * @note   Used private
 */
typedef struct
{
	uint16_t width;
	uint16_t height;
	TM_ILI9341_Orientation orientation; // 1 = portrait; 0 = landscape
} TM_ILI931_Options_t;

/* Pin definitions */

//#define ILI9341_RST_SET				HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin,GPIO_PIN_SET)
//#define ILI9341_RST_RESET			HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin,GPIO_PIN_RESET)
//#define ILI9341_CS_SET				HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin,GPIO_PIN_SET)
//#define ILI9341_CS_RESET			HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin,GPIO_PIN_RESET)
//#define ILI9341_WRX_SET				HAL_GPIO_WritePin(ILI9341_WRX_GPIO_Port, ILI9341_WRX_Pin,GPIO_PIN_SET)
//#define ILI9341_WRX_RESET			HAL_GPIO_WritePin(ILI9341_WRX_GPIO_Port, ILI9341_WRX_Pin,GPIO_PIN_RESET)

/* Private defines */
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7

/* Pin functions */
uint16_t ILI9341_x;
uint16_t ILI9341_y;
TM_ILI931_Options_t ILI9341_Opts;
uint8_t ILI9341_INT_CalledFromPuts = 0;

/** Requiem  Buffer size  **/
#define MAX_BUFFER 5000
unsigned char tabletmp[MAX_BUFFER + 2];

/* Private functions */
void TM_ILI9341_InitLCD(void);
void TM_ILI9341_SendData(uint8_t data);
void TM_ILI9341_SendCommand(uint8_t data);
void TM_ILI9341_Delay(volatile unsigned int delay);
void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);


/**   function that start block transfer (c) Requiem-Projects **/
void ILI9341_SendData_Block(uint8_t *buf, uint32_t len)
{
	// init W/R and CS of the ili9341 to start transfer
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi1, buf, len, 1000);
	ILI9341_CS_SET;
}

void TM_ILI9341_Init()
{
	/* CS high */
	ILI9341_CS_SET;
	/* Init LCD */
	TM_ILI9341_InitLCD();

	/* Set default settings */
	ILI9341_x = ILI9341_y = 0;
	ILI9341_Opts.width = ILI9341_WIDTH;
	ILI9341_Opts.height = ILI9341_HEIGHT;
	ILI9341_Opts.orientation = TM_ILI9341_Portrait;

	/* Fill with white color */
	TM_ILI9341_Fill(WHITE);
}

void TM_ILI9341_InitLCD(void)
{
	ILI9341_RST_RESET;
	HAL_Delay(20);
	ILI9341_RST_SET;
	HAL_Delay(50);
	TM_ILI9341_SendCommand(ILI9341_RESET);
	HAL_Delay(50);

	TM_ILI9341_SendCommand(ILI9341_POWERA);
	TM_ILI9341_SendData(0x39);
	TM_ILI9341_SendData(0x2C);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x34);
	TM_ILI9341_SendData(0x02);
	TM_ILI9341_SendCommand(ILI9341_POWERB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x30);
	TM_ILI9341_SendCommand(ILI9341_DTCA);
	TM_ILI9341_SendData(0x85);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendCommand(ILI9341_DTCB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_POWER_SEQ);
	TM_ILI9341_SendData(0x64);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x12);
	TM_ILI9341_SendData(0x81);
	TM_ILI9341_SendCommand(ILI9341_PRC);
	TM_ILI9341_SendData(0x20);
	TM_ILI9341_SendCommand(ILI9341_POWER1);
	TM_ILI9341_SendData(0x23);
	TM_ILI9341_SendCommand(ILI9341_POWER2);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_VCOM1);
	TM_ILI9341_SendData(0x3E);
	TM_ILI9341_SendData(0x28);
	TM_ILI9341_SendCommand(ILI9341_VCOM2);
	TM_ILI9341_SendData(0x86);
	TM_ILI9341_SendCommand(ILI9341_MAC);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendCommand(ILI9341_PIXEL_FORMAT);
	TM_ILI9341_SendData(0x55);
	TM_ILI9341_SendCommand(ILI9341_FRC);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x18);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x82);
	TM_ILI9341_SendData(0x27);
	TM_ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xEF);
	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x3F);
	TM_ILI9341_SendCommand(ILI9341_GAMMA);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x2B);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x4E);
	TM_ILI9341_SendData(0xF1);
	TM_ILI9341_SendData(0x37);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x14);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x36);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendCommand(ILI9341_SLEEP_OUT);
	HAL_Delay(10);

	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
}

void TM_ILI9341_DisplayOn(void)
{
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
}

void TM_ILI9341_DisplayOff(void)
{
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_OFF);
}

void TM_ILI9341_SendCommand(uint8_t data)
{
	ILI9341_WRX_RESET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &data, 1, 1000);
	ILI9341_CS_SET;
}

void TM_ILI9341_SendData(uint8_t data)
{
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &data, 1, 1000);
	ILI9341_CS_SET;
}

void TM_ILI9341_SendData16(uint16_t data)
{
	uint8_t dt;
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;

	dt = data >> 8;
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &dt, 1, 1000);
	dt = data & 0xFF;
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &dt, 1, 1000);
//	HAL_SPI_Transmit(&hspi1, (uint8_t*) &data, 2, 1000);
	ILI9341_CS_SET;

}

void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{

	TM_ILI9341_SetCursorPosition(x, y, x, y);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	TM_ILI9341_SendData(color >> 8);
	TM_ILI9341_SendData(color & 0xFF);
}

void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(x1 >> 8);
	TM_ILI9341_SendData(x1 & 0xFF);
	TM_ILI9341_SendData(x2 >> 8);
	TM_ILI9341_SendData(x2 & 0xFF);

	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(y1 >> 8);
	TM_ILI9341_SendData(y1 & 0xFF);
	TM_ILI9341_SendData(y2 >> 8);
	TM_ILI9341_SendData(y2 & 0xFF);
}

void TM_ILI9341_Fill(uint32_t color)
{
	unsigned int n, i, j;
	i = color >> 8;
	j = color & 0xFF;
	TM_ILI9341_SetCursorPosition(0, 0, ILI9341_Opts.width - 1, ILI9341_Opts.height - 1);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	uint16_t len = 0;
	for (n = 0; n < ILI9341_PIXEL; n++)
	{
		tabletmp[len++] = i;
		tabletmp[len++] = j;
		if (len >= MAX_BUFFER)
		{
			ILI9341_SendData_Block(tabletmp, len);
			len = 0;
		}
	}
	if (len > 0)
		ILI9341_SendData_Block(tabletmp, len);
}

void TM_ILI9341_Delay(volatile unsigned int delay)
{
	for (; delay != 0; delay--)
		;
}

void TM_ILI9341_Rotate(TM_ILI9341_Orientation_t orientation)
{
	TM_ILI9341_SendCommand(ILI9341_MAC);
	if (orientation == TM_ILI9341_Orientation_Portrait_1)
	{
		TM_ILI9341_SendData(0x58);
	}
	else if (orientation == TM_ILI9341_Orientation_Portrait_2)
	{
		TM_ILI9341_SendData(0x88);
	}
	else if (orientation == TM_ILI9341_Orientation_Landscape_1)
	{
		TM_ILI9341_SendData(0x28);
	}
	else if (orientation == TM_ILI9341_Orientation_Landscape_2)
	{
		TM_ILI9341_SendData(0xE8);
	}

	if (orientation == TM_ILI9341_Orientation_Portrait_1 || orientation == TM_ILI9341_Orientation_Portrait_2)
	{
		ILI9341_Opts.width = ILI9341_WIDTH;
		ILI9341_Opts.height = ILI9341_HEIGHT;
		ILI9341_Opts.orientation = TM_ILI9341_Portrait;
	}
	else
	{
		ILI9341_Opts.width = ILI9341_HEIGHT;
		ILI9341_Opts.height = ILI9341_WIDTH;
		ILI9341_Opts.orientation = TM_ILI9341_Landscape;
	}
}

void TM_ILI9341_Puts(uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground, uint32_t background)
{
	uint16_t startX = x;

	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	while (*str)
	{
		//New line
		if (*str == '\n')
		{
			ILI9341_y += font->FontHeight + 1;
			//if after \n is also \r, than go to the left of the screen
			if (*(str + 1) == '\r')
			{
				ILI9341_x = 0;
				str++;
			}
			else
			{
				ILI9341_x = startX;
			}
			str++;
			continue;
		}
		else if (*str == '\r')
		{
			str++;
			continue;
		}

		TM_ILI9341_Putc(ILI9341_x, ILI9341_y, *str++, font, foreground, background);
	}
}

void TM_ILI9341_Putc_Transparent(uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground)
{
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width)
	{
		//If at the end of a line of display, go to new line and set x to 0 position
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}
	//uint16_t len = 0 ;
	for (i = 0; i < font->FontHeight; i++)
	{
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++)
		{
			if ((b << j) & 0x8000)
			{
				TM_ILI9341_DrawPixel(ILI9341_x + j, (ILI9341_y + i), foreground);
			}
		}
	}
	ILI9341_x += font->FontWidth;
}

void TM_ILI9341_Puts_Transparent(uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground)
{
	uint16_t startX = x;

	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	while (*str)
	{
		//New line
		if (*str == '\n')
		{
			ILI9341_y += font->FontHeight + 1;
			//if after \n is also \r, than go to the left of the screen
			if (*(str + 1) == '\r')
			{
				ILI9341_x = 0;
				str++;
			}
			else
			{
				ILI9341_x = startX;
			}
			str++;
			continue;
		}
		else if (*str == '\r')
		{
			str++;
			continue;
		}

		TM_ILI9341_Putc_Transparent(ILI9341_x, ILI9341_y, *str++, font, foreground);
	}
}

void TM_ILI9341_GetStringSize(char *str, TM_FontDef_t *font, uint16_t *width, uint16_t *height)
{
	uint16_t w = 0;
	*height = font->FontHeight;
	while (*str++)
	{
		w += font->FontWidth;
	}
	*width = w;
}

void TM_ILI9341_Putc(uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground, uint32_t background)
{
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width)
	{
		//If at the end of a line of display, go to new line and set x to 0 position
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}
	TM_ILI9341_SetCursorPosition(ILI9341_x, ILI9341_y, ILI9341_x + font->FontWidth - 1, ILI9341_y + font->FontHeight - 1);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	uint16_t len = 0;
	for (i = 0; i < font->FontHeight; i++)
	{
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++)
		{
			if ((b << j) & 0x8000)
			{
				tabletmp[len++] = (foreground >> 8);
				tabletmp[len++] = (foreground & 0xFF);
			}
			else if ((background & ILI9341_TRANSPARENT) == 0)
			{
				tabletmp[len++] = (background >> 8);
				tabletmp[len++] = (background & 0xFF);
			}
			if (len >= MAX_BUFFER)
			{
				ILI9341_SendData_Block(tabletmp, len);
				len = 0;
			}
		}
	}
	if (len > 0)
	{
		ILI9341_SendData_Block(tabletmp, len);
	}

	ILI9341_x += font->FontWidth;
}

void __attribute__((optimize("O1"))) TM_ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	/* Code by dewoller: https://github.com/dewoller */
	int16_t dx, dy, c; //e2,err,sx, sy,
	uint16_t leng = 0;

	/* Check for overflow */
	if (x0 >= ILI9341_Opts.width)
	{
		x0 = ILI9341_Opts.width - 1;
	}
	if (x1 >= ILI9341_Opts.width)
	{
		x1 = ILI9341_Opts.width - 1;
	}
	if (y0 >= ILI9341_Opts.height)
	{
		y0 = ILI9341_Opts.height - 1;
	}
	if (y1 >= ILI9341_Opts.height)
	{
		y1 = ILI9341_Opts.height - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);

	if (x0 > x1)
	{
		c = x0;
		x0 = x1;
		x1 = c;
	}
	if (y0 > y1)
	{
		c = y0;
		y0 = y1;
		y1 = c;
	}

	TM_ILI9341_SetCursorPosition(x0, y0, x1, y1);
	if (dx == 0)
		dx = 1;
	if (dy == 0)
		dy = 1;
	c = dx * dy;
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	while (c > 0)
	{
		tabletmp[leng++] = (color >> 8);
		tabletmp[leng++] = (color & 0xFF);
		if (leng >= MAX_BUFFER)
		{
			ILI9341_SendData_Block(tabletmp, leng);
			leng = 0;
		}
		c--;
	}
	if (leng > 0)
		ILI9341_SendData_Block(tabletmp, leng);
}

void __attribute__((optimize("O1"))) TM_ILI9341_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	TM_ILI9341_DrawLine(x0, y0, x1, y0, color); //Top
	TM_ILI9341_DrawLine(x0, y0, x0, y1, color);	//Left
	TM_ILI9341_DrawLine(x1, y0, x1, y1, color);	//Right
	TM_ILI9341_DrawLine(x0, y1, x1, y1, color);	//Bottom
}

void __attribute__((optimize("O1"))) TM_ILI9341_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{

	int16_t dx, dy; // , e2;sx, sy, err
	uint32_t c;
	if (x0 >= ILI9341_Opts.width)
	{
		x0 = ILI9341_Opts.width - 1;
	}
	if (x1 >= ILI9341_Opts.width)
	{
		x1 = ILI9341_Opts.width - 1;
	}
	if (y0 >= ILI9341_Opts.height)
	{
		y0 = ILI9341_Opts.height - 1;
	}
	if (y1 >= ILI9341_Opts.height)
	{
		y1 = ILI9341_Opts.height - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);

	if (x0 > x1)
	{
		c = x0;
		x0 = x1;
		x1 = c;
	}
	if (y0 > y1)
	{
		c = y0;
		y0 = y1;
		y1 = c;
	}
	TM_ILI9341_SetCursorPosition(x0, y0, x1, y1);
	if (x0 == 0)
		dx += 1;
	if (y0 == 0)
		dy += 1;
	if (dx == 0)
		dx = 1;
	if (dy == 0)
		dy = 1;
	c = (dx + 1) * (dy + 1);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	uint16_t len = 0;
	while (c > 0)
	{
		tabletmp[len++] = (color >> 8);
		tabletmp[len++] = (color & 0xFF);
		if (len >= MAX_BUFFER)
		{
			ILI9341_SendData_Block(tabletmp, len);
			len = 0;
		}
		c--;
	}
	if (len > 0)
		ILI9341_SendData_Block(tabletmp, len);

}

void __attribute__((optimize("O1"))) TM_ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	TM_ILI9341_DrawPixel(x0, y0 + r, color);
	TM_ILI9341_DrawPixel(x0, y0 - r, color);
	TM_ILI9341_DrawPixel(x0 + r, y0, color);
	TM_ILI9341_DrawPixel(x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		TM_ILI9341_DrawPixel(x0 + x, y0 + y, color);
		TM_ILI9341_DrawPixel(x0 - x, y0 + y, color);
		TM_ILI9341_DrawPixel(x0 + x, y0 - y, color);
		TM_ILI9341_DrawPixel(x0 - x, y0 - y, color);

		TM_ILI9341_DrawPixel(x0 + y, y0 + x, color);
		TM_ILI9341_DrawPixel(x0 - y, y0 + x, color);
		TM_ILI9341_DrawPixel(x0 + y, y0 - x, color);
		TM_ILI9341_DrawPixel(x0 - y, y0 - x, color);
	}
}

void __attribute__((optimize("O1"))) TM_ILI9341_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	TM_ILI9341_DrawPixel(x0, y0 + r, color);
	TM_ILI9341_DrawPixel(x0, y0 - r, color);
	TM_ILI9341_DrawPixel(x0 + r, y0, color);
	TM_ILI9341_DrawPixel(x0 - r, y0, color);
	TM_ILI9341_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		TM_ILI9341_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		TM_ILI9341_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		TM_ILI9341_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		TM_ILI9341_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
}


void __attribute__((optimize("O1"))) Bitmap(int posx, int posy, unsigned char index, uint16_t bit0, uint16_t bit1)
{
	int b; // h,
	const unsigned char *dig = Res_Digits[index];
	unsigned int n = 120 * 80 / 8;
	TM_ILI9341_SetCursorPosition(posx, posy, posx + Digits_width, posy + Digits_height);
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	unsigned int i = 0;
	unsigned int nbbit = 0;
	uint32_t len = 0;
	unsigned char c;
	while (i < n)
	{
		c = dig[i];
		b = 8;
		while (b > 0)
		{
			//if (len < 2000)
			if (c & 0x01)
			{

				tabletmp[len++] = bit1 >> 8;
				tabletmp[len++] = bit1 & 0xFF;

			}
			else
			{

				tabletmp[len++] = bit0 >> 8;
				tabletmp[len++] = bit0 & 0xFF;

			}
			if (len >= MAX_BUFFER)
			{
				ILI9341_SendData_Block(tabletmp, len);
				len = 0;
			}
			b--;
			c = c >> 1;
			nbbit++;
			if (nbbit > Digits_width)
			{
				nbbit = 0;
				b = 0;
			}

		}
		i++;
	}
	if (len > 0)
		ILI9341_SendData_Block(tabletmp, len);

}

void __attribute__((optimize("O1"))) BitmapPointer(int posx, int posy, int w, int h, uint8_t * dig, uint16_t bit0, uint16_t bit1)
{
	int b;
	//unsigned char * dig = Res_Digits[index];
	unsigned int n = w * h / 8;
	//taskENTER_CRITICAL();
	TM_ILI9341_SetCursorPosition(posx, posy, posx + w - 1, posy + h - 1);
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	unsigned int i = 0;
	unsigned int nbbit = 0;
	uint32_t len = 0;
	unsigned char c;
	while (i < n)
	{
		c = dig[i];
		b = 8;
		while (b > 0)
		{
			//if (len < 2000)
			if (c & 0x01)
			{

				tabletmp[len++] = bit1 >> 8;
				tabletmp[len++] = bit1 & 0xFF;

			}
			else
			{

				tabletmp[len++] = bit0 >> 8;
				tabletmp[len++] = bit0 & 0xFF;

			}
			if (len >= MAX_BUFFER)
			{
				ILI9341_SendData_Block(tabletmp, len);
				len = 0;
			}
			b--;
			c = c >> 1;
			nbbit++;
			if (nbbit >= w)
			{
				nbbit = 0;
				b = 0;
			}

		}
		i++;
	}
	if (len > 0)
		ILI9341_SendData_Block(tabletmp, len);
}

void __attribute__((optimize("O1"))) ButtonVertical(int posx, int posy, int w, int h, uint8_t * tab)
{

	TM_ILI9341_SetCursorPosition(posx, posy, posx + w, posy + h);
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	unsigned int i, j;
	uint32_t len = 0;
	uint8_t color1, color2;

	for (i = 0; i < h; i++)
	{
		color1 = tab[(i * 2)];
		color2 = tab[(i * 2) + 1];
		for (j = 0; j < w; j++)
		{
			tabletmp[len++] = color2;
			tabletmp[len++] = color1;
			if (len >= MAX_BUFFER)
			{
				ILI9341_SendData_Block(tabletmp, len);
				len = 0;
			}
		}

	}
	if (len > 0)
	{
		ILI9341_SendData_Block(tabletmp, len);
	}
}

void __attribute__((optimize("O1"))) ButtonHorizontal(int posx, int posy, int w, int h, uint8_t *tab)
{

	TM_ILI9341_SetCursorPosition(posx, posy, posx + (w - 1), posy + (h - 1));
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	unsigned int i, j;
	uint32_t len = 0;
	uint8_t color1, color2;

	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			color1 = tab[(j * 2)];
			color2 = tab[(j * 2) + 1];
			tabletmp[len++] = color2;
			tabletmp[len++] = color1;
			if (len >= MAX_BUFFER)
			{
				ILI9341_SendData_Block(tabletmp, len);
				len = 0;
			}
		}

	}
	if (len > 0)
	{
		ILI9341_SendData_Block(tabletmp, len);
	}
}


void __attribute__((optimize("O1"))) Picture(uint16_t posx, uint16_t posy, uint16_t w, uint16_t h, uint8_t * pict)
{

	unsigned int n = w * h * 2;
	uint16_t len = 0;
	TM_ILI9341_SetCursorPosition(posx, posy, posx + w - 1, posy + h - 1);
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	unsigned int i = 0;
	while (i < n)
	{
		tabletmp[len++] = pict[i + 1];
		tabletmp[len++] = pict[i];
		i += 2;
		if (len >= MAX_BUFFER)
		{
			ILI9341_SendData_Block(tabletmp, len);
			len = 0;
		}
	}
	if (len > 0)
		ILI9341_SendData_Block(tabletmp, len);
}

void __attribute__((optimize("O1"))) PictureLength(uint16_t posx, uint16_t posy, uint16_t len)
{
	unsigned int n = rpm_image.height * rpm_image.width * 2;
	TM_ILI9341_SetCursorPosition(posx, posy, posx + rpm_image.width - 1, posy + rpm_image.height - 1);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	uint16_t leng = 0;
	unsigned int i = 0;
	unsigned int l = 0;
	while (i < n)
	{
		if (l >= rpm_image.width)
		{
			l = 0;
		}
		if (l > len)
		{
			tabletmp[leng++] = ( WHITE >> 8);
			tabletmp[leng++] = (WHITE & 0xFF);
		}
		else
		{
			if (((l % 20) == 0) || ((l % 20) == 1))
			{
				tabletmp[leng++] = ( WHITE >> 8);
				tabletmp[leng++] = (WHITE & 0xFF);
			}
			else
			{

				tabletmp[leng++] = (rpm_image.pixel_data[i + 1]);
				tabletmp[leng++] = (rpm_image.pixel_data[i]);
			}
		}
		if (leng >= MAX_BUFFER)
		{
			ILI9341_SendData_Block(tabletmp, leng);
			leng = 0;
		}
		i += 2;
		l++;
	}
	if (leng > 0)
		ILI9341_SendData_Block(tabletmp, leng);
}
