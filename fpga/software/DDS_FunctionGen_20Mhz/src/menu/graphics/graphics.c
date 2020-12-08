/*
 * utilityFunctions.c
 *
 *  Created on: 11 feb 2018
 *      Author: Windows7
 */

#include "graphics.h"
#include "../../lcd/GLCD.h"
#include "../../delay/delay.h"
#include "../../common_functions/commonFunctions.h"
#include "menuBitmaps.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


void GP_ResetTypingScreen(s_EditableValue* editableValue, bool enable, bool show){
	typingScreen.show				= show;
	typingScreen.enabled			= enable;
	typingScreen.value				= editableValue;
	typingScreen.measuringUnitIndex = 0;

	typingScreen.initialized		= false;
	typingScreen.decimalPoint		= false;
	typingScreen.numDigits			= 0;
	typingScreen.selectedDigit 		= 0;
	typingScreen.invalidValue		= false;
	typingScreen.numDecimalValues	= 0;
}

//Draws the typing screen un to the display
void GP_DrawTypingScreen(){
	const int height	= 40;
	const int width 	= 100;
	const int offX		= 7;
	const int offY		= 16;
	const int numDigits = 8;


	if(typingScreen.show){
		//Clear the area
		GLCD_SolidRectangle(offX - 1, offY - 1, offX + width + 1, offY + height + 1, false);

		//Draw upper title
		GLCD_SetFont(_FONT_SMALL);
		GLCD_PrintString(offX + 2, offY + 2, "CH");
		GLCD_PrintNumI(offX + 15, offY + 2, menuVariables.selectedChannel, 1, ' ');
		if(typingScreen.value->name){
			GLCD_PrintString(offX + 25, offY + 2, typingScreen.value->name);
		}
		GLCD_VLine(offX + 22, offY, 10, true);
		GLCD_HLine(offX, offY + 10, 23, true);

		unsigned int numberOffsetY = offY + height / 2 - 5;
		//Draw buffer
		/*if(typingScreen.value->type == e_INTEGER){
			GLCD_PrintNumI(offX + 20, numberOffsetY, (int)typingScreen.buffer, numDigits, ' ');
		}
		else{*/
		unsigned int numDecimals = GetNumDecimals(typingScreen.buffer);
		GLCD_printNumF(offX + 20, numberOffsetY, typingScreen.buffer, 8, numDecimals, '.', ' ');
		//}
			//GLCD_printNumF(offX + 20, numberOffsetY, typingScreen.buffer, numDigits, numDigits - 2, '.', ' ');

		//Draw cursor
		//GLCD_HLine(offX + 14 + (numDigits - typingScreen.selectedDigit) * 6, offY + height / 2 + 3, 8, true);

		//Print measuring unit
		/*if(typingScreen.value->measuringUnitsCount > 0){
			const char* str_MeasuringUnit = typingScreen.value->measuringUnitsNames[typingScreen.measuringUnitIndex];
			GLCD_PrintString(offX + 20 + numDigits * 6 + 6, numberOffsetY, str_MeasuringUnit);
		}*/
		if(typingScreen.value->availableMeasuringUnits->measuringUnitsCount > 0){
			const char* str_MeasuringUnit = typingScreen.value->availableMeasuringUnits->measuringUnitsNames[typingScreen.measuringUnitIndex];
			GLCD_PrintString(offX + 20 + numDigits * 6 + 6, numberOffsetY, str_MeasuringUnit);
		}

		//Draw borders
		GLCD_VLine(offX, offY + 1, height - 1, true);
		GLCD_VLine(offX + 1, offY, height - 1, true);
		GLCD_VLine(offX + width - 1, offY, height - 1, true);

		GLCD_HLine(offX + 2, offY, width - 3, true);
		GLCD_HLine(offX + 2, offY + height - 2, width - 2, true);
		GLCD_HLine(offX, offY + height - 1, width - 1, true);

		if(typingScreen.invalidValue){
			GLCD_PrintString(offX + 10, offY + height - 10, "INVALID VALUE!");
		}
	}
}

//Draws the lateral buttons
void GP_DrawLateralButtons(u_ButtonNames* buttonsArray, int selected){
	const uint8_t* font = GLCD_GetFont();	//Save current font

	GLCD_HLine(_LCD_WIDTH - 29, 0, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 2, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 1, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 21, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 23, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 22, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 42, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 44, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 43, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 63, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 65, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 64, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 84, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 86, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 85, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 105, 30, true);
	GLCD_VLine(_LCD_WIDTH - 30, 107, 19, true);
	GLCD_HLine(_LCD_WIDTH - 29, 106, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 126, 30, true);
	GLCD_HLine(_LCD_WIDTH - 29, 127, 30, true);

	GLCD_SetFont(_FONT_SMALL);

	for(int i = 0; i < 6; i++){
		GLCD_SetTextModeInverted(selected == i);

		if(buttonsArray[i].button > e_BTN_ARROW_RIGHT){
			GLCD_PrintString(_LCD_WIDTH - 27, i * 21 + 7, 	buttonsArray[i].text);
		}
		else{
			GL_DrawIcon(_LCD_WIDTH - 20, i * 21 + 7, buttonsArray[i].button);
		}
	}

	GLCD_SetTextModeInverted(false);

	GLCD_SetFont(font);	//Restore previous font
}

//Draws an icon
void GL_DrawIcon(int x, int y, e_buttonsType icon){
	switch(icon){
	case e_BTN_ARROW_RETURN:
		//GLCD_DrawBitmap(x, y, &bmp_8x8_ArrowReturn);
		GLCD_DrawBitmap(x - 3, y, &bmp_12x12_ArrowReturn);
		break;
	case e_BTN_ARROW_UP:
		GLCD_DrawBitmap(x, y, &bmp_8x8_ArrowUp);
		break;
	case e_BTN_ARROW_DOWN:
		GLCD_DrawBitmap(x, y, &bmp_8x8_ArrowDown);
		break;
	case e_BTN_ARROW_LEFT:
		GLCD_DrawBitmap(x, y, &bmp_8x8_ArrowLeft);
		break;
	case e_BTN_ARROW_RIGHT:
		GLCD_DrawBitmap(x, y, &bmp_8x8_ArrowRight);
		break;
	}
}

//-------------------------------------------------------------------------------------------------
// Function : 		GP_DrawWave
// Arguments : 		int 		offset: offset from the top of the screen to where the wave will be drawn
//					e_WaveType	waveType: type of wave to draw
//					float		dcOffset: offset of the middle line (0.0f to 100.0f)
// Return value	: 	none
//Draws a wave on the screen
//-------------------------------------------------------------------------------------------------
void GP_DrawWave(int offsetY, e_WaveType waveType, float dcOffset){
	const int Xr = 80;				//X coordinate of the right of the graph
	const int Xm = Xr / 2;
	const int Xq = Xr / 4;
	const int Yt = offsetY + 12;	//Y coordinate of the top of the graph
	const int Ym = offsetY + 25;	//Y coordinate of the middle of the graph
	const int Yb = offsetY + 38;	//Y coordinate of the bottom of the graph
	const int Ys = (Yb - Yt);

	GLCD_HLine(0, Yt + Ys * (dcOffset / 100.0f), Xr + 5, true);	//Middle line

	switch(waveType){
		case e_WAVE_NONE:

			break;
		case e_WAVE_SQUARE:
			GLCD_VLine(Xm,Yt, Yb - Yt,	true);
			GLCD_HLine(1, Yt, Xm, 		true);
			GLCD_HLine(Xm,Yb, Xm, 		true);
			break;
		case e_WAVE_TRIANGELE:
			GLCD_Line(1,    Ym, Xq, 	Yt, true);
			GLCD_Line(Xq,   Yt, Xq*3,	Yb, true);
			GLCD_Line(Xq*3,	Yb,	Xr, 	Ym, true);
			break;
		case e_WAVE_SAWTOOTH:
			GLCD_Line(1, Ym, Xq, Yb, true);
			GLCD_Line(Xq, Yt, Xq, Yb, true);
			GLCD_Line(Xq, Yt, Xq * 3, Yb, true);
			GLCD_Line(Xq * 3, Yt, Xq * 3, Yb, true);
			GLCD_Line(Xq * 3, Yt, Xr, Ym, true);
			break;
		case e_WAVE_SINE:
			for(int i = 0; i < Xr; i++){
				GLCD_Pixel(i, (int)(-sin(((M_PI * 2.0f) / (float)Xr) * (float)i) * ((float)Ys / 2)) + Ym, true);
			}
			break;
		case e_WAVE_ARB:

			break;
	}
}
