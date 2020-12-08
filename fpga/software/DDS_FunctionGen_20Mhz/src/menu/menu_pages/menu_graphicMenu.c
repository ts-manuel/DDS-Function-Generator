/*
 * menu_graphicMenu.c
 *
 *  Created on: 31 mar 2018
 *      Author: Manuel
 */


#include "common.h"
#include "../menu.h"
#include "../graphics/graphics.h"
#include "../graphics/menuBitmaps.h"
#include "../../lcd/GLCD.h"
#include "../../delay/delay.h"
#include "../../hardware/hardware.h"
#include "../../definitions.h"
#include "../../data_structures/dataStructures.h"
#include "../../common_functions/commonFunctions.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//Function declaration
static void init();
static void draw();
static void btn_display(uint8_t btnDisplay);
static void btn_encoder(uint8_t btnEncoder, int8_t encoder);
static void btn_keypad(uint8_t KeyPressed);

//Struttura che descrive una pagina del menu
const s_MenuPage menu_graphicMenu = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

//Menu icons array
static const s_MenuIconPage menuPages[9] = {{"SETTINGS", 		&bmp_28x28_MenuSettings, 			&menu_settings},
											{"PHASE OFFSET", 	&bmp_28x28_MenuPhaseOffset,			&menu_phaseOffset},
											{"FREQ SWEEP", 		&bmp_28x28_MenuSweep, 				&menu_sweep},
											{"AMPLITUDE MOD", 	&bmp_28x28_MenuAmplitudeModulation, &menu_am_modulation},
											{"FREQUENCY MOD",	&bmp_28x28_MenuFrequencyModulation, &menu_fm_modulation},
											{"SD CARD MENU", 	&bmp_28x28_MenuSD, 					&menu_sd},
											{"", NULL, NULL}, {"", NULL, NULL}, {"", NULL, NULL}};
static int selectedIcon = 0;

static void init(){
	selectedIcon = 0;
	typingScreen.enabled = false;
}

static void draw(){
	GLCD_ClrScr();

	//Draw top info bar
	GLCD_SetFont(_FONT_SMALL);
	GLCD_PrintString(40, 5, "MENU");

	//Draw menu border
	const uint8_t xOffset = 3;
	const uint8_t yOffset = 20;
	const uint8_t width   = 92;
	const uint8_t height  = 92;
	GLCD_HLine(xOffset, yOffset, width, true);
	GLCD_HLine(xOffset, yOffset + height, width, true);
	GLCD_VLine(xOffset, yOffset, height, true);
	GLCD_VLine(xOffset + width, yOffset, height, true);
	GLCD_SolidRectangle(xOffset, yOffset + height, xOffset + width + 1, yOffset + height + 10, true);

	//Square dimension = 28x28
	const uint8_t iconDimX = 28;
	const uint8_t iconDimY = 28;
	for(uint8_t i = 0; i < 9; i++){
		uint8_t topLeftX = xOffset + 2 + (i % 3) * (iconDimX + 2);
		uint8_t topLeftY = yOffset + 2 + (i / 3) * (iconDimY + 2);

		if(menuPages[i].p_bitmap){
			GLCD_DrawBitmap(topLeftX, topLeftY, menuPages[i].p_bitmap);
		}

		if(i == selectedIcon){
			//Draw selection border
			GLCD_HLine(topLeftX, 			topLeftY, 				iconDimX, true);
			GLCD_HLine(topLeftX, 			topLeftY + iconDimY, 	iconDimX, true);
			GLCD_VLine(topLeftX, 			topLeftY, 				iconDimY + 1, true);
			GLCD_VLine(topLeftX + iconDimX, topLeftY, 				iconDimY + 1, true);

			//Draw menu name
			GLCD_SetFont(_FONT_SMALL);
			GLCD_SetTextModeInverted(true);
			int nameLength = strlen(menuPages[i].name);
			int fontSizeX = GLCD_GetFontSizeX(_FONT_SMALL);
			GLCD_PrintString(xOffset + (width - fontSizeX * nameLength) / 2, yOffset + height + 2, menuPages[i].name);
			GLCD_SetTextModeInverted(false);
		}
	}




	//Lateral Buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {.button=e_BTN_ARROW_UP}, {.button=e_BTN_ARROW_DOWN},
								 {.button=e_BTN_ARROW_LEFT}, {.button=e_BTN_ARROW_RIGHT}, {" OK "}};
	GP_DrawLateralButtons(buttons, -1);

	GLCD_Update();
}

static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
	case _BTN_DISPLAY_0:	//RETURN
		menu_PopPage();
		break;
	case _BTN_DISPLAY_1:	//UP
		if(selectedIcon - 3 >= 0)
			selectedIcon -= 3;
		break;
	case _BTN_DISPLAY_2:	//DOWN
		if(selectedIcon + 3 < 9)
			selectedIcon += 3;
		break;
	case _BTN_DISPLAY_3:	//LEFT
		if(selectedIcon - 1 >= 0)
			selectedIcon -= 1;
		break;
	case _BTN_DISPLAY_4:	//RIGHT
		if(selectedIcon + 1 < 9)
			selectedIcon += 1;
		break;
	case _BTN_DISPLAY_5:	//OK
		if(menuPages[selectedIcon].p_menuPage)
			menu_PushPage(menuPages[selectedIcon].p_menuPage);
		break;
	}
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	if(encoder){
		int newSelectedIcon = selectedIcon + encoder;
		if(newSelectedIcon < 0)
			newSelectedIcon = 0;
		else if(newSelectedIcon >= 9)
			newSelectedIcon = 8;
		selectedIcon = newSelectedIcon;
	}

	switch(btnEncoder){
	case _BTN_ENCODER_PB:
		if(menuPages[selectedIcon].p_menuPage)
			menu_PushPage(menuPages[selectedIcon].p_menuPage);
		break;
	case _BTN_ENCODER_L:
		break;
	case _BTN_ENCODER_R:
		break;
	}
}

static void btn_keypad(uint8_t KeyPressed){

}
