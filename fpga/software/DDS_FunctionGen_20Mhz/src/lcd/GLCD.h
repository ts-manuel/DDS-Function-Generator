/*
 * GLCD.h
 *
 *  Created on: 30 gen 2018
 *      Author: Manuel
 */

#include <stdint.h>
#include <stdbool.h>
#include "GLCD_Fonts.h"

#ifndef SRC_HARDWERE_LCD_NHDC128128_GLCD_H_
#define SRC_HARDWERE_LCD_NHDC128128_GLCD_H_

//Dimensions of the display, change for the display in use
#define _LCD_HEIGHT	128
#define _LCD_WIDTH	128

#define _LCD_HALF_H			(_LCD_HEIGHT / 2)
#define _LCD_HALF_W			(_LCD_WIDTH / 2)
#define _TEXT_POS_RIGHT 	9999
#define _TEXT_POS_CENTER	9998
#define _NO_CURSOR			9997

	//Bitmap struct to be used with the function GLCD_DrawBitmap()
	typedef struct{
		unsigned int   width;	//Width in pixels of the bitmap
		unsigned int   height;	//Height in pixels of the bitmap
		const uint8_t* data;	//Data to be displayed, the number of bytes must be ((width * height) / 8), one byte represents a rectangle 1x8,
									//if the bitmap is higher that 8 pixels more bytes are used
	} s_GLCD_Bitmap;

	void GLCD_SetBackLight(unsigned char intensity);
	void GLCD_Initialize(unsigned char contrast);
	void GLCD_ClrScr();
	void GLCD_FillScr();
	void GLCD_Pixel(int x, int y, bool s);
	void GLCD_InvPixel(int x, int y);
	void GLCD_Update();
	void GLCD_HLine(int x, int y, int l, bool s);
	void GLCD_VLine(int x, int y, int l, bool s);
	void GLCD_Line(int x1, int y1, int x2, int y2, bool s);
	void GLCD_SolidRectangle(int minX, int minY, int maxX, int maxY, bool s);
	void GLCD_Rectangle(int minX, int minY, int maxX, int maxY, bool s);
	void GLCD_SetTextModeInverted(bool mode);
	void GLCD_SetFont(const uint8_t* font);
	const uint8_t* GLCD_GetFont();
	int  GLCD_GetFontSizeX(const uint8_t* font);
	int  GLCD_GetFontSizeY(const uint8_t* font);
	void GLCD_PrintString(int x, int y, const char *st);
	void GLCD_PrintStringWithSelectedCharacter(int x, int y, const char *st, int selectedCharacter);
	void GLCD_PrintChar(int x, int y, char c);
	void GLCD_PrintNumWithCursorI(int x, int y, long num, int length, char filler, int cursorPosition);
	void GLCD_PrintNumI(int x, int y, long num, int length, char filler);
	void GLCD_printNumWithCursorF(int x, int y, float num, int length, int dec, char divider, char filler, int cursorPosition);
	void GLCD_printNumF(int x, int y, float num, int length, int dec, char divider, char filler);
	void GLCD_DrawBitmap(int x, int y, const s_GLCD_Bitmap* bitmap);

#endif /* SRC_HARDWERE_LCD_NHDC128128_GLCD_H_ */
