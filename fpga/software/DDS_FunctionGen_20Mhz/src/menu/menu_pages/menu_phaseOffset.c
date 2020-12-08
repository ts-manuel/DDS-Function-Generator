/*
 * menu_phaseOffset.c
 *
 *  Created on: 02 apr 2018
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
const s_MenuPage menu_phaseOffset = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

static int selectedChannel = 0;

static void init(){
	selectedChannel			= 0;
	GP_ResetTypingScreen(&DDS_Variables[selectedChannel].PhaseOffset, true, false);
}

static void draw(){
	GLCD_ClrScr();
	GLCD_SetFont(_FONT_SMALL);

	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuPhaseOffset);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_PrintString(30, 5, "PHASE OFF..");

	const uint8_t xOffset 	= 5;
	const uint8_t yOffset0	= 40;
	const uint8_t yOffset1 	= 84;
	//Draw CH0
	float phaseOffset0 = DDS_Variables[0].PhaseOffset.floatValue;
	GLCD_SetTextModeInverted(selectedChannel == 0);
	GLCD_PrintString(xOffset, yOffset0, "CH0");
	GLCD_SetTextModeInverted(false);
	GLCD_PrintNumI(xOffset + 20, yOffset0, (int)phaseOffset0, 3, ' ');
	GLCD_HLine(2, yOffset0 + 22, 94, true);
	float offset = (phaseOffset0 / (360.0f / 50.0f));
	bool LevelHigh = ((int)offset % 50) < 25;
	for (int i = 0; i < 92; i++){
		if((i + (int)offset) % 25 == 0 && i > 0){
			GLCD_VLine(i + 2, yOffset0 + 10, 27, true);
			LevelHigh = !LevelHigh;
		}
		if(LevelHigh)
			GLCD_Pixel(i + 2, yOffset0 + 10, true);
		else
			GLCD_Pixel(i + 2, yOffset0 + 36, true);
	}
	//Draw CH1
	float phaseOffset1 = DDS_Variables[1].PhaseOffset.floatValue;
	GLCD_SetTextModeInverted(selectedChannel == 1);
	GLCD_PrintString(xOffset, yOffset1, "CH1");
	GLCD_SetTextModeInverted(false);
	GLCD_PrintNumI(xOffset + 20, yOffset1, (int)phaseOffset1, 3, ' ');
	GLCD_HLine(2, yOffset1 + 22, 94, true);
	offset = (phaseOffset1 / (360.0f / 50.0f));
	LevelHigh = ((int)offset % 50) < 25;
	for (int i = 0; i < 92; i++){
		if((i + (int)offset) % 25 == 0 && i > 0){
			GLCD_VLine(i + 2, yOffset1 + 10, 27, true);
			LevelHigh = !LevelHigh;
		}
		if(LevelHigh)
			GLCD_Pixel(i + 2, yOffset1 + 10, true);
		else
			GLCD_Pixel(i + 2, yOffset1 + 36, true);
	}

	//Draw lateral buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {"CH_0"}, {"CH_1"},
								 {.button=e_BTN_ARROW_LEFT}, {.button=e_BTN_ARROW_RIGHT}, {""}};
	GP_DrawLateralButtons(buttons, -1);

	//Show typing screen
	GP_DrawTypingScreen();

	//Transfer screen buffer to LCD
	GLCD_Update();
}

static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
		case _BTN_DISPLAY_0:	//RETURN
			menu_PopPage();
			break;
		case _BTN_DISPLAY_1:	//CH_0
			selectedChannel = 0;
			break;
		case _BTN_DISPLAY_2:	//CH_1
			selectedChannel = 1;
			break;
		case _BTN_DISPLAY_3:	//LEFT
			AddToEditableValue(&DDS_Variables[selectedChannel].PhaseOffset, -1.0f);
			break;
		case _BTN_DISPLAY_4:	//RIGHT
			AddToEditableValue(&DDS_Variables[selectedChannel].PhaseOffset, 1.0f);
			break;
		case _BTN_DISPLAY_5:	//
			break;
		}

	GP_ResetTypingScreen(&DDS_Variables[selectedChannel].PhaseOffset, true, false);
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	if(encoder){
		AddToEditableValue(&DDS_Variables[selectedChannel].PhaseOffset, encoder);
	}
}

static void btn_keypad(uint8_t KeyPressed){
	menu_ProcessKeypadPress(KeyPressed);
}
