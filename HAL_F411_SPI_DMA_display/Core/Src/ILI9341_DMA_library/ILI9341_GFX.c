//	MIT License
//
//	Copyright (c) 2017 Matej Artnak
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
//
//
//-----------------------------------
//	ILI9341 GFX library for STM32
//-----------------------------------
//
//	Very simple GFX library built upon ILI9342_STM32_Driver library.
//	Adds basic shapes, image and font drawing capabilities to ILI9341
//
//	Library is written for STM32 HAL library and supports STM32CUBEMX. To use the library with Cube software
//	you need to tick the box that generates peripheral initialization code in their own respective .c and .h file


#include "ILI9341_DMA_driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "spi.h"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))

#define LCD_WIDTH 320
#define LCD_HEIGHT 240

extern uint8_t SPI2_TX_completed_flag;

const GFXfont *gfxFont = NULL;

void set_adafruit_font(const GFXfont *font_pointer)
{
	gfxFont = font_pointer;
}

GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c)
{
  return gfxFont->glyph + c;
}

uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont)
{
  return gfxFont->bitmap;
}

/*Draw hollow circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1);

    while (x >= y)
    {
        ILI9341_Draw_Pixel(X + x, Y + y, Colour);
        ILI9341_Draw_Pixel(X + y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - x, Y + y, Colour);
        ILI9341_Draw_Pixel(X - x, Y - y, Colour);
        ILI9341_Draw_Pixel(X - y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius << 1) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	
		int x = Radius;
    int y = 0;
    int xChange = 1 - (Radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = X - x; i <= X + x; i++)
        {
            ILI9341_Draw_Pixel(i, Y + y,Colour);
            ILI9341_Draw_Pixel(i, Y - y,Colour);
        }
        for (int i = X - y; i <= X + y; i++)
        {
            ILI9341_Draw_Pixel(i, Y + x,Colour);
            ILI9341_Draw_Pixel(i, Y - x,Colour);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
		//Really slow implementation, will require future overhaul
		//TODO:	https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles	
}

/*Draw a hollow rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	float 		Calc_Negative = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;		
	}
	else
	{
		X_length = X0 - X1;		
	}
	ILI9341_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
	ILI9341_Draw_Horizontal_Line(X0, Y1, X_length, Colour);
	
	
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;		
	}
	ILI9341_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
	ILI9341_Draw_Vertical_Line(X1, Y0, Y_length, Colour);
	
	if((X_length > 0)||(Y_length > 0)) 
	{
		ILI9341_Draw_Pixel(X1, Y1, Colour);
	}
	
}

/*Draw a filled rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	int32_t 	Calc_Negative = 0;
	
	uint16_t X0_true = 0;
	uint16_t Y0_true = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;
		X0_true = X0;
	}
	else
	{
		X_length = X0 - X1;
		X0_true = X1;
	}
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;
		Y0_true = Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;
		Y0_true = Y1;	
	}
	
	ILI9341_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);	
}

/*Draws a character (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9341_Draw_Char(char Character, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	uint8_t function_char;
	uint8_t i, j;

	function_char = Character;

	if (function_char < ' ')
	{
		Character = 0;
	}
	else
	{
		function_char -= 32;
	}

	char temp[CHAR_WIDTH];
	for (uint8_t k = 0; k < CHAR_WIDTH; k++)
	{
		temp[k] = font[function_char][k];
	}

	// Draw pixels
	ILI9341_Draw_Rectangle(X, Y, CHAR_WIDTH * Size, CHAR_HEIGHT * Size, Background_Colour);
	for (j = 0; j < CHAR_WIDTH; j++)
	{
		for (i = 0; i < CHAR_HEIGHT; i++)
		{
			if (temp[j] & (1 << i))
			{
				if (Size == 1)
				{
					ILI9341_Draw_Pixel(X + j, Y + i, Colour);
				}
				else
				{
					ILI9341_Draw_Rectangle(X + (j * Size), Y + (i * Size), Size, Size, Colour);
				}
			}
		}
	}
}

/*Draws an array of characters (fonts imported from fonts.h) at X,Y location with specified font color, size and Background color*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9341_Draw_Text(const char* Text, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
    while (*Text)
    {
        ILI9341_Draw_Char(*Text++, X, Y, Colour, Size, Background_Colour);
        X += CHAR_WIDTH*Size;
    }
}

void ILI9341_Draw_TextFont(const char* Text, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
    while (*Text)
    {
        ILI9341_Draw_CharFont(*Text, X, Y, Colour, Background_Colour, Size, Size);
        X = X + gfxFont->glyph[*Text-32].xAdvance;
        Text++;
    }
}

/*
 * \brief Write bitmap of specified size to specified location on screen.
 *
 * Bitmap is uint16_t array, where one value corresponds to one pixel. However, display expects to receive low byte first
 * and MCU sends high byte first. For effective DMA utilization bitmaps have bytes reversed. Using online bitmaps converters
 * will result in reversed colors.
 *
 * \param[in] *Image_Array Pointer to image array
 * \param[in] X desired X location of top left corner of bitmap on screen
 * \param[in] Y desired Y location of top left corner of bitmap on screen
 * \param[in] Size_X Width of image
 * \param[in] Size_Y Height of image

 */
void ILI9341_Draw_Image(const uint8_t *Image_Array, uint16_t X, uint16_t Y, uint16_t Size_X, uint16_t Size_Y)
{
	if ((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT))
		return;
	if ((X + Size_X - 1) >= LCD_WIDTH)
	{
		Size_X = LCD_WIDTH - X;
	}
	if ((Y + Size_Y - 1) >= LCD_HEIGHT)
	{
		Size_Y = LCD_HEIGHT - Y;
	}
	uint32_t Size = Size_X*Size_Y*2;
	ILI9341_Set_Address(X, Y, X + Size_X - 1, Y + Size_Y - 1);

	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);


	int x;
	for(x = 0; x < Size/3200; x++)
	{
		SPI2_TX_completed_flag = 0;
		HAL_SPI_Transmit_DMA(HSPI_INSTANCE, (unsigned char*) (Image_Array+(x*3200)), 3200);
		while (SPI2_TX_completed_flag == 0);
	}

	if(Size % 3200 > 0)
	{
		SPI2_TX_completed_flag = 0;
		HAL_SPI_Transmit_DMA(HSPI_INSTANCE, (unsigned char*) (Image_Array+(x*3200)), Size % 3200);
		while (SPI2_TX_completed_flag == 0);
	}

}

/*Draws a character from specified AdafruitGFX font*/
//Adafruit fonts seem to be 2x slower than original built-in font
//Fonts with scale bigger than 1 are even slower.

void ILI9341_Draw_CharFont(unsigned char c, int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y)
{
	if(gfxFont == NULL)
		return;
    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1)
    {
      xo16 = xo;
      yo16 = yo;
    }

    for (yy = 0; yy < h; yy++)
    {
      for (xx = 0; xx < w; xx++)
      {
        if (!(bit++ & 7))
        {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1)
          {
            ILI9341_Draw_Pixel(x + xo + xx, y + yo + yy, color);
          }
          else
          {
        	  ILI9341_Draw_Rectangle(
        			  x + (xo16 + xx) * size_x,
					  y + (yo16 + yy) * size_y,
					  size_x,
					  size_y,
					  color);
          }
        }
        bits <<= 1;
      }
    }
}
