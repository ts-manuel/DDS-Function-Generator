/*
 * GLCD.c
 *
 *  Created on: 30 gen 2018
 *      Author: Manuel
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "GLCD.h"
#include "lcd_NHD_C128128.h"
#include "../delay/delay.h"

#define _TRUE  1
#define _FALSE 0

//CHANGE DEFINITIONS ACCORDING TO PREFERENCES
#define _DATA_BUSS_WIDTH	8		//Defines what is the width of the data buss, it is used to optimize the code

//-------------------------------------------------------------------------------------
#define _SCREEN_BUFFER_SIZE ((_LCD_HEIGHT * _LCD_WIDTH) / 8)	//Buffer size in bytes
//Macros that are used to calculate the address of a particular bit on the buffer screen
#define _GET_SCREEN_BUFFER_BYTE(x, y)	((((y) / 8) * _LCD_WIDTH) + (x))
#define _GET_SCREEN_BUFFER_BIT(x, y) 	((y) % 8)


static unsigned char scrbuf[_SCREEN_BUFFER_SIZE];	//Buffer video
static char _Sleep		= 0;    					//Indicha se il display è in sleep
static char _Contrast 	= 0; 						//Valore di contrasto impostato nel display
//static bool printDebug 	= false;

typedef struct {
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	const uint8_t* font;
	bool 	inverted;
} s_Font;
static s_Font	cfont;  //Struct that holds the current font


//Function declarations
static int IntLenght(long num);
static void ConvertFloatToString(char *buf, float num, int width, int prec);
static int abs(int num);
void GLCD_SPixel(int x, int y);
void GLCD_CPixel(int x, int y);
void GLCD_SHLine(int x0, int x1, int y);
void GLCD_CHLine(int x0, int x1, int y);
void GLCD_SVLine(int x, int y0, int y1);
void GLCD_CVLine(int x, int y0, int y1);

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_SetBackLight
// Arguments : 		unsigned char byte 0 to 255
// Return value	: 	none
// Sets the back light intensity to the value specified
//-------------------------------------------------------------------------------------------------
void GLCD_SetBackLight(unsigned char intensity){
	LCD_NHD_C128128_SetBackLight(intensity);
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_Initialize
// Arguments : 		unsigned char byte
// Return value	: 	none
// Initializes the display and clears the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(unsigned char contrast){
	LCD_NHD_C128128_Initialize(contrast);	//Initialize display and set contrast

	GLCD_ClrScr();				//Clear screen buffer data
	GLCD_Update();				//Update display ram
	GLCD_SetFont(_FONT_SMALL);	//Set default font to SmallFont
	_Sleep 		= false;
	_Contrast 	= contrast;
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_clrScr
// Arguments : 		none
// Return value	: 	none
// Clears the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_ClrScr(){
#if _DATA_BUSS_WIDTH >= 64
	const int size64	= _SCREEN_BUFFER_SIZE / sizeof(uint64_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint64_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size64; ++i)
		((uint64_t*)scrbuf)[i] = 0x0000000000000000;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size64 * sizeof(uint64_t))] = 0x00;

#elif _DATA_BUSS_WIDTH == 32		//Write to memory using 32 bit operations
	const int size32	= _SCREEN_BUFFER_SIZE / sizeof(uint32_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint32_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size32; ++i)
		((uint32_t*)scrbuf)[i] = 0x00000000;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size32 * sizeof(uint32_t))] = 0x00;

#elif _DATA_BUSS_WIDTH == 16	//Write to memory using 16 bit operations
	const int size16	= _SCREEN_BUFFER_SIZE / sizeof(uint16_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint16_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size16; ++i)
		((uint16_t*)scrbuf)[i] = 0x0000;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size16 * sizeof(uint16_t))] = 0x00;

#else							//Write to memory using 8 bit operations
	for (int i = 0; i < _SCREEN_BUFFER_SIZE; i++){
		scrbuf[i] = 0x00;
	}
#endif
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_update
// Arguments : 		none
// Return value	: 	none
// Sends the screen buffer data to the display RAM
//-------------------------------------------------------------------------------------------------
void GLCD_Update(){
	if (_Sleep == false){
		LCD_NHD_C128128_Update(scrbuf);
	}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_FillScr
// Arguments : 		none
// Return value	: 	none
// Set all the pixel in the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_FillScr(){
#if _DATA_BUSS_WIDTH >= 64
	const int size64	= _SCREEN_BUFFER_SIZE / sizeof(uint64_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint64_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size64; ++i)
		((uint64_t*)scrbuf)[i] = 0xFFFFFFFFFFFFFFFF;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size64 * sizeof(uint64_t))] = 0x00;

#elif _DATA_BUSS_WIDTH == 32		//Write to memory using 32 bit operations
	const int size32	= _SCREEN_BUFFER_SIZE / sizeof(uint32_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint32_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size32; ++i)
		((uint32_t*)scrbuf)[i] = 0xFFFFFFFF;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size32 * sizeof(uint32_t))] = 0x00;

#elif _DATA_BUSS_WIDTH == 16	//Write to memory using 16 bit operations
	const int size16	= _SCREEN_BUFFER_SIZE / sizeof(uint16_t);	//Number of 32bit dwords that compose the screen buffer
	const int size8 	= _SCREEN_BUFFER_SIZE % sizeof(uint16_t);	//Number of 8bit bytes that remain

	for (int i = 0; i < size16; ++i)
		((uint16_t*)scrbuf)[i] = 0xFFFF;
	for (int i = 0; i < size8; ++i)
		scrbuf[i + (size16 * sizeof(uint16_t))] = 0x00;

#else							//Write to memory using 8 bit operations
	for (int i = 0; i < _SCREEN_BUFFER_SIZE; i++){
		scrbuf[i] = 0xFF;
	}
#endif
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_Pixel
// Arguments : 		int x: 	x coordinate
//					int y: 	y coordinate
//					bool s:	true = set pixel, false = clear pixel
// Return value	: 	none
// Sets or clears the pixel at coordinate(x,y) on the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_Pixel(int x, int y, bool s){
	if(x < _LCD_WIDTH && y < _LCD_HEIGHT){
		if(s)
			GLCD_SPixel(x, y);
		else
			GLCD_CPixel(x, y);
	}
}

void GLCD_SPixel(int x, int y){
	int  byte = _GET_SCREEN_BUFFER_BYTE(x, y);
	char bit  = _GET_SCREEN_BUFFER_BIT(x, y);

	scrbuf[byte] |= (1 << bit);	//Set Bit
}

void GLCD_CPixel(int x, int y){
	int  byte = _GET_SCREEN_BUFFER_BYTE(x, y);
	char bit  = _GET_SCREEN_BUFFER_BIT(x, y);

	scrbuf[byte] &= ~(1 << bit);//Clear Bit
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_InvPixel
// Arguments : 		int x: x coordinate
//					int y: y coordinate
// Return value	: 	none
// Inverts the pixel at coordinates(x,y) on the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_InvPixel(int x, int y){
	if(x < _LCD_WIDTH && y < _LCD_HEIGHT){
		int  byte = _GET_SCREEN_BUFFER_BYTE(x, y);
		char bit  = _GET_SCREEN_BUFFER_BIT(x, y);

		scrbuf[byte] ^= (1 << bit);	//Toggle Bit
	}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_drawHLine
// Arguments : 		int x: 	start point x coordinate
//					int y: 	start point y coordinate
//					int l: 	length of the line in pixels
//					bool s:	true = set line, false = clear line
// Return value	: 	none
// Draws or clears an orizzontal line on the screen buffer, starting from the point(x, y) and whit length l
//-------------------------------------------------------------------------------------------------
void GLCD_HLine(int x, int y, int l, bool s){
	if(x < _LCD_WIDTH && y < _LCD_HEIGHT){
		int x1 = (x + l) > _LCD_WIDTH ? _LCD_WIDTH : (x + l);

		if(s)
			GLCD_SHLine(x, x1, y);
		else
			GLCD_CHLine(x, x1, y);
	}
}

void GLCD_SHLine(int x0, int x1, int y){	//200uS
#if _DATA_BUSS_WIDTH >= 64
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size64 	= (stopByte - startByte) / sizeof(uint64_t);
	const int preBytes 	= startByte % sizeof(uint64_t);

	const uint8_t template8 = (1 << bit);
	uint64_t template64 = (1 << bit);
	template64 |= template64 << 8;
	template64 |= template64 << 16;
	template64 |= template64 << 32;

	((uint64_t*)(scrbuf + startByte))[0] |= template64 << (8 * preBytes);
	for(int i = 1; i < size64; ++i)
		((uint64_t*)(scrbuf + startByte))[i] |= template64;
	for (int i = startByte + (size64 * sizeof(uint64_t)) - sizeof(uint64_t); i < stopByte; i++)
		scrbuf[i] |= template8;	//Set Bit

#elif _DATA_BUSS_WIDTH == 32
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size32 	= (stopByte - startByte) / sizeof(uint32_t);
	const int preBytes 	= startByte % sizeof(uint32_t);

	const uint8_t template8 = (1 << bit);
	uint32_t template32 = (1 << bit);
	template32 |= template32 << 8;
	template32 |= template32 << 16;

	((uint32_t*)(scrbuf + startByte))[0] |= template32 << (8 * preBytes);
	for(int i = 1; i < size32; ++i)
		((uint32_t*)(scrbuf + startByte))[i] |= template32;
	for (int i = startByte + (size32 * sizeof(uint32_t)) - sizeof(uint32_t); i < stopByte; i++)
		scrbuf[i] |= template8;	//Set Bit

#elif _DATA_BUSS_WIDTH == 16
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size16 	= (stopByte - startByte) / sizeof(uint16_t);
	const int preBytes 	= startByte % sizeof(uint16_t);

	const uint8_t template8 = (1 << bit);
	uint16_t template16 = (1 << bit);
	template16 |= template16 << 8;

	((uint16_t*)(scrbuf + startByte))[0] |= template16 << (8 * preBytes);
	for(int i = 1; i < size16; ++i)
		((uint16_t*)(scrbuf + startByte))[i] |= template16;
	for (int i = startByte + (size16 * sizeof(uint16_t)) - sizeof(uint16_t); i < stopByte; i++)
		scrbuf[i] |= template8;	//Set Bit
#else
	/*const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);
	const uint8_t template8 = (1 << bit);

	for (int i = stopByte; i < startByte; i++){
		scrbuf[i] |= template8;	//Set Bit
	}*/

	for (int x = x0; x < x1 ; x++){
		GLCD_SPixel(x, y);	//Set or clear all the pixel of the line
	}
#endif
}

void GLCD_CHLine(int x0, int x1, int y){
#if _DATA_BUSS_WIDTH >= 64
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size64 	= (stopByte - startByte) / sizeof(uint64_t);
	const int preBytes 	= startByte % sizeof(uint64_t);

	const uint8_t template8 = ~(1 << bit);
	uint64_t template64 = (1 << bit);
	template64 |= template64 << 8;
	template64 |= template64 << 16;
	template64 |= template64 << 32;
	template64 = ~template64;

	((uint64_t*)(scrbuf + startByte))[0] &= template64 << (8 * preBytes);
	for(int i = 1; i < size64; ++i)
		((uint64_t*)(scrbuf + startByte))[i] &= template64;
	for (int i = startByte + (size64 * sizeof(uint64_t)) - sizeof(uint64_t); i < stopByte; i++)
		scrbuf[i] &= template8;	//Set Bit

#elif _DATA_BUSS_WIDTH == 32
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size32 	= (stopByte - startByte) / sizeof(uint32_t);
	const int preBytes 	= startByte % sizeof(uint32_t);

	const uint8_t template8 = ~(1 << bit);
	uint32_t template32 = (1 << bit);
	template32 |= template32 << 8;
	template32 |= template32 << 16;
	template32 = ~template32;

	((uint32_t*)(scrbuf + startByte))[0] |= template32 << (8 * preBytes);
	for(int i = 1; i < size32; ++i)
		((uint32_t*)(scrbuf + startByte))[i] &= template32;
	for (int i = startByte + (size32 * sizeof(uint32_t)) - sizeof(uint32_t); i < stopByte; i++)
		scrbuf[i] &= template8;	//Set Bit

#elif _DATA_BUSS_WIDTH == 16
	const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);	//Get witch bit to set
	const int size16 	= (stopByte - startByte) / sizeof(uint16_t);
	const int preBytes 	= startByte % sizeof(uint16_t);

	const uint8_t template8 = ~(1 << bit);
	uint16_t template16 = (1 << bit);
	template16 |= template16 << 8;
	templat16 = ~template16;

	((uint16_t*)(scrbuf + startByte))[0] |= template16 << (8 * preBytes);
	for(int i = 1; i < size16; ++i)
		((uint16_t*)(scrbuf + startByte))[i] &= template16;
	for (int i = startByte + (size16 * sizeof(uint16_t)) - sizeof(uint16_t); i < stopByte; i++)
		scrbuf[i] &= template8;	//Set Bit
#else
	/*const int startByte = _GET_SCREEN_BUFFER_BYTE(x0, y);
	const int stopByte  = _GET_SCREEN_BUFFER_BYTE(x1, y);
	const char bit 		= _GET_SCREEN_BUFFER_BIT(0, y);
	const uint8_t template8 = ~(1 << bit);

	for (int i = startByte; i < stopByte; i++){
		scrbuf[i] &= template8;	//Set Bit
	}*/
	for (int x = x0; x < x1 ; x++){
		GLCD_CPixel(x, y);	//Set or clear all the pixel of the line
	}
#endif
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_VLine
// Arguments : 		int x: 	start point x coordinate
//					int y: 	start point y coordinate
//					int l: 	length of the line in pixels
//					bool s: true = set line, false = clear line
// Return value	: 	none
// Draws or clears a vertical line on the screen buffer, starting from the point(x, y) and whit length l
//-------------------------------------------------------------------------------------------------
void GLCD_VLine(int x, int y, int l, bool s){
	if(x < _LCD_WIDTH && y < _LCD_HEIGHT){
		int y1 = (y + l) > _LCD_HEIGHT ? _LCD_HEIGHT : (y + l);

		if(s)
			GLCD_SVLine(x, y, y1);
		else
			GLCD_CVLine(x, y, y1);
	}
}

void GLCD_SVLine(int x, int y0, int y1){
	for (int y = y0; y < y1 ; y++){
		GLCD_SPixel(x, y);	//Set or clear all the pixel of the line
	}
}

void GLCD_CVLine(int x, int y0, int y1){
	for (int y = y0; y < y1; y++){
		GLCD_CPixel(x, y);	//Set or clear all the pixel of the line
	}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_Line
// Arguments : 		int x1: start point x coordinate
//					int y1: start point y coordinate
//					int x1: end point x coordinate
//					int y2: end point y coordinate
//					bool s: true = set line, false = clear line
// Return value	: 	none
// Draws or clears a line on the screen buffer, starting from the point(x, y) and ending at(x,y)
//-------------------------------------------------------------------------------------------------
void GLCD_Line(int x1, int y1, int x2, int y2, bool s){
	int tmp;
	double delta, tx, ty;

	if (((x2-x1)<0)){
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
    if (((y2-y1)<0)){
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if (y1==y2){
		if (x1>x2)
		{
			tmp=x1;
			x1=x2;
			x2=tmp;
		}
		GLCD_HLine(x1, y1, x2-x1, s);
	}
	else if (x1==x2)
	{
		if (y1>y2)
		{
			tmp=y1;
			y1=y2;
			y2=tmp;
		}
		GLCD_VLine(x1, y1, y2-y1, s);
	}
	else if (abs(x2-x1) > abs(y2-y1)){
		delta = (double)(y2 - y1) / (double)(x2 - x1);
		ty = (double)y1;
		if (x1 > x2){
			for (int i = x1; i >= x2; i--){
				GLCD_Pixel(i, (int)(ty + 0.5), s);
        		ty=ty-delta;
			}
		}
		else{
			for (int i = x1; i <= x2; i++){
				GLCD_Pixel(i, (int)(ty + 0.5), s);
        		ty = ty + delta;
			}
		}
	}
	else{
		delta = (float)(x2 - x1) / (float)(y2 - y1);
		tx = (float)x1;
        if (y1 > y2){
			for (int i = y2+1; i > y1; i--){
		 		GLCD_Pixel((int)(tx+0.5), i, s);
        		tx = tx + delta;
			}
        }
        else{
			for (int i = y1; i < y2+1; i++){
		 		GLCD_Pixel((int)(tx+0.5), i, s);
        		tx = tx + delta;
			}
        }
	}

}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_Rectangle
// Arguments : 		int minX: 	start point x coordinate
//					int minY: 	start point y coordinate
//					int maxX: 	end point x coordinate
//					int maxY: 	end point y coordinate
//					bool s:		true = set rectangle, false = clear rectangle
// Return value	: 	none
// Draws or clears a rectangle on the screen buffer, starting from the point(minX, minY) and ending at point(maxX, maxY)
//-------------------------------------------------------------------------------------------------
void GLCD_SolidRectangle(int minX, int minY, int maxX, int maxY, bool s){
	if(maxX > _LCD_WIDTH) 	maxX = _LCD_WIDTH;	//Max value clamping
	if(maxY > _LCD_HEIGHT)	maxX = _LCD_HEIGHT;

	if(maxX - minX > maxY - minY){				//If the rectangle is wider than higher
		for(int lineY = minY; lineY < maxY; lineY++){
			if(s)
				GLCD_SHLine(minX, maxX, lineY);
			else
				GLCD_CHLine(minX, maxX, lineY);
		}
	}
	else{
		for(int columnX = minX; columnX < maxX; columnX++){
			if(s)
				GLCD_SVLine(columnX, minY, maxY);
			else
				GLCD_CVLine(columnX, minY, maxY);
		}
	}
}

void GLCD_Rectangle(int minX, int minY, int maxX, int maxY, bool s){
	if(maxX > _LCD_WIDTH) 	maxX = _LCD_WIDTH;	//Max value clamping
	if(maxY > _LCD_HEIGHT)	maxX = _LCD_HEIGHT;

	if(s){
		GLCD_SHLine(minX, maxX, minY);
		GLCD_SHLine(minX, maxX+1, maxY);
		GLCD_SVLine(minX, minY, maxY);
		GLCD_SVLine(maxX, minY, maxY);
	}
	else{
		GLCD_CHLine(minX, maxX, minY);
		GLCD_CHLine(minX, maxX+1, maxY);
		GLCD_CVLine(minX, minY, maxY);
		GLCD_CVLine(maxX, minY, maxY);
	}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_SetTextMode
// Arguments : 		bool mode:	true = normal mode, false = inverted mode
// Return value	: 	none
// Sets the visualization mode
//-------------------------------------------------------------------------------------------------
void GLCD_SetTextModeInverted(bool mode){
    cfont.inverted = mode;
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_SetTextMode
// Arguments : 		const uint8_t* font:	pointer to the font array
// Return value	: 	none
// Sets the font
//-------------------------------------------------------------------------------------------------
void GLCD_SetFont(const uint8_t* font){
	cfont.font      =   font;
	cfont.x_size    =   font[0];//fontbyte(0);
	cfont.y_size    =   font[1];//fontbyte(1);
	cfont.offset    =   font[2];//fontbyte(2);
	cfont.numchars  =   font[3];//fontbyte(3);
	cfont.inverted  =   0;
}

const uint8_t* GLCD_GetFont(){
	return cfont.font;
}

int  GLCD_GetFontSizeX(const uint8_t* font){
	return ((s_Font*)font)->x_size;
}

int  GLCD_GetFontSizeY(const uint8_t* font){
	return ((s_Font*)font)->y_size;
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_PrintString
// Arguments : 		int x: 			start point x coordinate
//					int y: 			start point y coordinate
//					const char* st:	string to print
// Return value	: 	none
// Prints the string(st) to the screen buffer at the coordinates(x,y)
//-------------------------------------------------------------------------------------------------
void GLCD_PrintString(int x, int y, const char *st){
	GLCD_PrintStringWithSelectedCharacter(x, y, st, -1);
}

void GLCD_PrintStringWithSelectedCharacter(int x, int y, const char *st, int selectedCharacter){
	int stl = strlen(st);	//String length

	if (x == _TEXT_POS_RIGHT)							//If x = _TEXT_POS_RIGHT 
		x = _LCD_WIDTH - (stl * cfont.x_size);			//Calculate x coordinate in order to print the string to the right
	if (x == _TEXT_POS_CENTER)							//If x = _TEXT_POS_CENTER
		x = (_LCD_WIDTH - (stl * cfont.x_size)) / 2;	//Calculate x coordinate in order to print the string in the center

	const bool textMode = cfont.inverted;
	for (int cnt = 0; cnt < stl; cnt++){
		if(selectedCharacter != _NO_CURSOR)
			cfont.inverted = (cnt == selectedCharacter) ^ textMode; //If cnt is equal to the selected character invert the text mode
		GLCD_PrintChar(x + (cnt * cfont.x_size), y, st[cnt]);
    }

	cfont.inverted = textMode;
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_PrintString
// Arguments : 		int x:	start point x coordinate
//					int y: 	start point y coordinate
//					char c:	string to print
// Return value	: 	none
// Prints the character(c) at position(x,y) to the screen buffer
//-------------------------------------------------------------------------------------------------
void GLCD_PrintChar(int x, int y, char c){
	if ((cfont.y_size % 8) == 0){
			int font_idx = ((c - cfont.offset)*(cfont.x_size*(cfont.y_size/8)))+4;
			for (int rowcnt=0; rowcnt<(cfont.y_size/8); rowcnt++){
				for(int cnt=0; cnt<cfont.x_size; cnt++){
					for (int b=0; b<8; b++){

						bool pixelSet = (cfont.font[font_idx + cnt + (rowcnt * cfont.x_size)] & (1 << b));
						GLCD_Pixel(x+cnt, y + (rowcnt * 8) + b, cfont.inverted ? !pixelSet : pixelSet);

	                }
				}
			}
		}
		else{
			int font_idx = ((c - cfont.offset) * ((cfont.x_size * cfont.y_size / 8))) + 4;
			int cbyte = cfont.font[font_idx];//font_fontbyte(font_idx);
			int cbit = 0;
			for (int cx=0; cx<cfont.x_size; cx++){
				for (int cy=0; cy<cfont.y_size; cy++){

					bool pixelSet = (cbyte & (1<<cbit));
					GLCD_Pixel(x+cx, y+cy, cfont.inverted ? !pixelSet : pixelSet);

					cbit++;
					if (cbit == 8){
						cbit = 0;
						font_idx++;
						cbyte = cfont.font[font_idx];// fontbyte(font_idx);
					}
				}
			}
		}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_PrintNumI
// Arguments : 		int x:			x coordinate
//					int y: 			y coordinate
//					int num:		number to print
//					int length:		number of digits to print
//					char filler:	character used to fill the blanck digits
// Return value	: 	none
// Prints the integer(num) to the screen buffer at position(x,y)
//-------------------------------------------------------------------------------------------------
void GLCD_PrintNumI(int x, int y, long num, int length, char filler){
	GLCD_PrintNumWithCursorI(x, y, num, length, filler, -1);
}

void GLCD_PrintNumWithCursorI(int x, int y, long num, int length, char filler, int cursorPosition){
	bool neg = false;   //Indica se il numero è negativo
    char Str[27];       //Buffer che conterra la stringa con il numero
    int StrIndex = IntLenght(num) - 1;  //Byte dell buffer in cui iniziare a scrivere
    if(length > StrIndex){      //Se il parametro lenght e maggiore delle cifre del numero
        StrIndex = length - 1;  //Il primo carattere andra messo alla posizione lenght - 1
    }

    if (num < 0){       //Se il numero è negativo
		neg = true;     //Setto la variabile che indica se il numero è negativo
		num = -num;     //Inverto il numero facendo il complemento a due
        Str[0] = '-';   //Inserisco alla prima posizione dell buffer il segno '-'
        StrIndex++;     //Incremento l'indice del buffer in modo da fare posto per il '-'
	}
    Str[StrIndex + 1] = 0;  //Inserisco infondo alla stringa 0

    do{
    	Str[StrIndex--] = '0' + num % 10;   //Inserico nel buffer il modulo del numero con 10
    	num /= 10;                          //Divido il numero per 10 in modo da shiftare le cifre
    }while(num > 0);						//Fino a che il numero è maggiore di 0
    for(int i = neg; i <= StrIndex; i++){  	//Per tutti i caratteri a sinistra non utilizzati
        Str[i] = filler;                    	//Li metto uguali al parametro filler
    }

    //GLCD_PrintString(x ,y, Str);   //Stampo la strinnga
    GLCD_PrintStringWithSelectedCharacter(x, y, Str, (strlen(Str) - 1) - cursorPosition);
    //Wite the cursor
}

//-------------------------------------------------------------------------------------------------
// Function : 		GLCD_printNumF
// Arguments : 		int x:			x coordinate
//					int y: 			y coordinate
//					int num:		number to print
//					int length:		total number of digits to print
//					int dec:		size of the decimal part
//					char divider:	character used as decimal devider
//					char filler:	character used to fill the blanck digits
// Return value	: 	none
// Prints the float(num) to the screen buffer at position(x,y)
//-------------------------------------------------------------------------------------------------
/*Funzione che stampa un numero decimale sul buffer video*/
void GLCD_printNumF(int x, int y, float num, int length, int dec, char divider, char filler){
	GLCD_printNumWithCursorF(x, y, num, length, dec, divider, filler, _NO_CURSOR);
}

void GLCD_printNumWithCursorF(int x, int y, float num, int length, int dec, char divider, char filler, int cursorPosition){
	char st[27];
	char neg = false;

	if (num < 0)
		neg = true;

	ConvertFloatToString(st, num, length, dec);

	if (divider != '.'){
		for (unsigned char i = 0; i < sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' '){
		if (neg){
			st[0]='-';
			for (unsigned char i = 1; i < sizeof(st); i++)
				if ((st[i] == ' ') || (st[i] == '-' ))
					st[i] = filler;
		}
		else{
			for (unsigned char i = 0; i < sizeof(st); i++)
				if (st[i] == ' ')
					st[i] = filler;
		}
	}

	int characterIndex;
	if(dec == 0){
		characterIndex = (strlen(st) - 1) - cursorPosition;
	}
	else{
		if(cursorPosition >= 0){
			characterIndex = (strlen(st) - (dec + 2)) - cursorPosition;
		}
		else{
			characterIndex = (strlen(st) - 1) - (dec + cursorPosition);
		}
	}

	//GLCD_PrintString(x, y, st);
	GLCD_PrintStringWithSelectedCharacter(x, y, st, characterIndex);
}

//-------------------------------------------------------------------------------------------------
// Function : 		IntLenght
// Arguments : 		long num:
// Return value	: 	int
// Return the number of digits of (num)
//-------------------------------------------------------------------------------------------------
static int IntLenght(long num){
    char numDigits = 1;
    while(num >= 10){
        num /= 10;
        numDigits++;
    }
    return numDigits;
}

//-------------------------------------------------------------------------------------------------
// Function : 		ConvertFloatToString
// Arguments : 		char *buf: 	string in witch to wire the string
//					float num:	number to convert into a string
//					int	width:	total length of the number
//					int prec:	length of the integer part of the number
// Return value	: 	none
// Writes into the string(*buf) thenumber(num) with total length(width) and (prec) as integer part length
//-------------------------------------------------------------------------------------------------
static void ConvertFloatToString(char *buf, float num, int width, int prec){
	char format[10];
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}

//-------------------------------------------------------------------------------------------------
// Function : 		ConvertFloatToString
// Arguments : 		int num: numero di cui calcolare il valore assoluto
// Return value	: 	int
// Returns the absolute value of (num)
//-------------------------------------------------------------------------------------------------
static int abs(int num){
    return num > 0 ? num : -num;
}

void GLCD_DrawBitmap(int x, int y, const s_GLCD_Bitmap* bitmap){
	//GLCD_Rectangle(x, y, x + lenght, y + height, false);
	/*int ByteY0;
	int ByteY1;
	int Ybits = y % 8;

	int xstart;
	int ystart = y;

	unsigned int bitmapSize = bitmap->width * ((bitmap->height <= 8) ? 1 : ((bitmap->height / 8) + 1));

	for(int i = 0; i < bitmapSize; i++){
		xstart = x + i % bitmap->width;
		if (i == bitmap->width){
			ystart += 8;
		}

		ByteY0 = _GET_SCREEN_BUFFER_BYTE(xstart, ystart);
		ByteY1 = _GET_SCREEN_BUFFER_BYTE(xstart, ystart + 8);
		scrbuf[ByteY0] |= bitmap->data[i] >> Ybits;
		scrbuf[ByteY1] |= bitmap->data[i] << (8 - Ybits);
	}*/


	for (int ny = 0; ny < bitmap->height; ny++){
		for (int nx = 0; nx < bitmap->width; nx++){
			int bitmapIndex = nx + (ny / 8) * bitmap->width;

			GLCD_Pixel(nx + x, ny + y, bitmap->data[bitmapIndex] & (1 << (7 - (ny % 8))));
		}
	}
}
