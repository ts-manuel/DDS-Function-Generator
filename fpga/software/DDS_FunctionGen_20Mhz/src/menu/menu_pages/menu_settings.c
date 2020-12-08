/*
 * menu_settings.c
 *
 *  Created on: 01 apr 2018
 *      Author: Manuel
 */


#include "../menu.h"
#include "../graphics/graphics.h"
#include "../graphics/menuBitmaps.h"
#include "../../lcd/GLCD.h"
#include "../../hardware/hardware.h"
#include "../../data_structures/dataStructures.h"
#include "../../common_functions/commonFunctions.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Function declaration
static void init();
static void draw();
static void btn_display(uint8_t btnDisplay);
static void btn_encoder(uint8_t btnEncoder, int8_t encoder);
static void btn_keypad(uint8_t KeyPressed);

//Struttura che descrive una pagina del menu
const s_MenuPage menu_settings = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

static const int 		menuElementsCount = 3;
static s_MenuElement	menuElementsArray[3];
static int 				selectedElement = 0;

static void init(){
	menuElementsArray[0].name = "LCD BAKLIGHT";
	menuElementsArray[0].type = e_PARAM_TYPE_EDITABLE_VALUE;
	menuElementsArray[0].editableValue = &menuVariables.backlight;

	menuElementsArray[1].name = "CH0 IMPEDANCE";
	menuElementsArray[1].type = e_PARAM_TYPE_STRING_ARRAY;
	menuElementsArray[1].stringArray = &DDS_Variables[0].OutputEmpedance;

	menuElementsArray[2].name = "CH1 IMPEDANCE";
	menuElementsArray[2].type = e_PARAM_TYPE_STRING_ARRAY;
	menuElementsArray[2].stringArray = &DDS_Variables[1].OutputEmpedance;

	selectedElement 		= 0;
	typingScreen.enabled	= false;
}

static void draw(){
	GLCD_ClrScr();
	GLCD_SetFont(_FONT_SMALL);

	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuSettings);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_PrintString(39, 5, "SETTINGS");

	//Draw menu elements
	const uint8_t offsetY = 48;
	const uint8_t fontTinySizeY = GLCD_GetFontSizeY(_FONT_TINY);
	const uint8_t fontTinySizeX = GLCD_GetFontSizeX(_FONT_TINY);
	GLCD_SetFont(_FONT_TINY);
	for(unsigned int i = 0; i < menuElementsCount; i++){
		int y = offsetY + (fontTinySizeY + 2) * i;

		//Print menu element name
		GLCD_PrintString(8, y, menuElementsArray[i].name);

		//Print element parameter
		GLCD_SetFont(_FONT_TINY);

		//y += (menuElementsArray[i].type == e_PARAM_TYPE_MENU_TITLE) ? 11 : 8;
		if(menuElementsArray[i].type == e_PARAM_TYPE_STRING_ARRAY){
			const char* str = menuElementsArray[i].stringArray->array[menuElementsArray[i].stringArray->selected];
			const int offsetX = (_LCD_WIDTH - 35) - strlen(str) * fontTinySizeX;
			GLCD_PrintString(offsetX, y, str);

			//Draw arrows
			if(menuElementsArray[i].stringArray->count > 1 && menuElementsArray[i].stringArray->selected > 0)
				GLCD_DrawBitmap(offsetX - 3, y, &bmp_2x3_SmallArrowLeft);
			if(menuElementsArray[i].stringArray->count > 1 && menuElementsArray[i].stringArray->selected < (menuElementsArray[i].stringArray->count - 1))
				GLCD_DrawBitmap((_LCD_WIDTH - 34), y, &bmp_2x3_SmallArrowRight);
		}
		else if(menuElementsArray[i].type == e_PARAM_TYPE_EDITABLE_VALUE){
			const int value = (int)menuElementsArray[i].editableValue->floatValue;
			const unsigned int numDigits = GetNumDigitsI(value);
			const int offsetX = (_LCD_WIDTH - 35) - numDigits * fontTinySizeX;
			GLCD_PrintNumI(offsetX, y, value, numDigits, ' ');
		}

		//Draw arrow at the left of the selected element
		if(i == selectedElement){
			GLCD_DrawBitmap(0, offsetY + (fontTinySizeY + 2) * i - 1, &bmp_8x8_ArrowRight);
		}
	}

	//Draw lateral buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {.button=e_BTN_ARROW_UP}, {.button=e_BTN_ARROW_DOWN},
								 {.button=e_BTN_ARROW_LEFT}, {.button=e_BTN_ARROW_RIGHT}, {" OK "}};
	GP_DrawLateralButtons(buttons, -1);

	//Transfer screen buffer to LCD
	GLCD_Update();
}

static void btn_display(uint8_t btnDisplay){
	//menu_ProcessButtonPress(btnDisplay);
	switch(btnDisplay){
		case _BTN_DISPLAY_0:	//RETURN
			menu_PopPage();
			break;
		case _BTN_DISPLAY_1:	//UP
			if(selectedElement - 1 >= 0)
				selectedElement -= 1;
			break;
		case _BTN_DISPLAY_2:	//DOWN
			if(selectedElement + 1 < menuElementsCount)
				selectedElement += 1;
			break;
		case _BTN_DISPLAY_3:	//LEFT
			menu_AddToMenuElement(&menuElementsArray[selectedElement], -1.0f);
			break;
		case _BTN_DISPLAY_4:	//RIGHT
			menu_AddToMenuElement(&menuElementsArray[selectedElement], +1.0f);
			break;
		case _BTN_DISPLAY_5:	//OK

			break;
		}
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	if(encoder){
		menu_AddToMenuElement(&menuElementsArray[selectedElement], (float)encoder);
	}
}

static void btn_keypad(uint8_t KeyPressed){

}
