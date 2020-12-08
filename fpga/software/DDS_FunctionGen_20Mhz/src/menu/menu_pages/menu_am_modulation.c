/*
 * menu_am_modulation.c
 *
 *  Created on: 14 feb 2018
 *      Author: Windows7
 */

#include "../../lcd/GLCD.h"
#include "../../hardware/hardware.h"
#include "../../definitions.h"
#include "../../data_structures/dataStructures.h"
#include "../graphics/menuBitmaps.h"
#include "../graphics/graphics.h"
#include "../menu.h"
#include "common.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


//Function declaration
static void init();
static void draw();
static void btn_display(uint8_t btnDisplay);
static void btn_encoder(uint8_t btnEncoder, int8_t encoder);
static void btn_keypad(uint8_t KeyPressed);

//Struttura che descrive una pagina del menu
const s_MenuPage menu_am_modulation = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};
static int selectedChannel = 0;

typedef enum {e_PARAMETER_NONE = 0, e_PARAMETER_AM_FREQUENCY = 4, e_PARAMETER_AM_MOD_INDX = 5} e_SelectedParameter;
static e_SelectedParameter selectedParameter;

static void init(){
	selectedParameter = e_PARAMETER_NONE;
	GP_ResetTypingScreen(NULL, false, false);
}

static void draw(){
	GLCD_ClrScr();
	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuAmplitudeModulation);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_SetFont(_FONT_SMALL);
	GLCD_PrintString(39, 5, "AM MOD..");

	//Draw wave
	GP_DrawWave(20, DDS_Variables[selectedChannel].AM_ModulationWaveType, 50.0f);

	//Draw separator
	GLCD_HLine(0, 61, 94, true);

	//Draw AM modulation variables
	uint8_t textOffsetY = 65;
	const uint8_t fontSizeY = GLCD_GetFontSizeY(_FONT_TINY);
	GLCD_SetFont(_FONT_TINY);
	GLCD_PrintString(0, textOffsetY, "CHANNEL");	GLCD_PrintNumI(50, textOffsetY, selectedChannel, 1, ' ');
	textOffsetY += fontSizeY + 1;
	GLCD_PrintString(0, textOffsetY, "STATE");		GLCD_PrintString(50, textOffsetY, DDS_Variables[selectedChannel].AM_ModulationON ? "ON" : "OFF");
	textOffsetY += fontSizeY + 1;
	GLCD_PrintString(0,	 textOffsetY,"WAVE:              ");
	GLCD_PrintString(50, textOffsetY,   str_WaveNames[(int)DDS_Variables[selectedChannel].AM_ModulationWaveType]);
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "MOD FREQ:", &DDS_Variables[selectedChannel].AM_ModulationFrequency, (selectedParameter == e_PARAMETER_AM_FREQUENCY));
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "MOD_INDX:", &DDS_Variables[selectedChannel].AM_ModulationIndex, (selectedParameter == e_PARAMETER_AM_MOD_INDX));

	//Draw buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {"CH_S"}, {" ON"}, {"WAVE"}, {"FREQ"}, {"INDX"}};
	int selectedButton = (selectedParameter != e_PARAMETER_NONE) ? (int)selectedParameter : -1;
	GP_DrawLateralButtons(buttons, selectedButton);

	//Draw typing screen
	GP_DrawTypingScreen();

	//Trasferico il buffer video nel display
	GLCD_Update();
}

static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
		case _BTN_DISPLAY_0:	//RETURN
			selectedParameter = e_PARAMETER_NONE;
			GP_ResetTypingScreen(NULL, false, false);
			menu_PopPage();
			break;
		case _BTN_DISPLAY_1:	//CH_S
			selectedParameter = e_PARAMETER_NONE;
			selectedChannel = selectedChannel ? 0 : 1;
			GP_ResetTypingScreen(NULL, false, false);
			break;
		case _BTN_DISPLAY_2:	//ON
			selectedParameter = e_PARAMETER_NONE;
			DDS_Variables[selectedChannel].AM_ModulationON = !DDS_Variables[selectedChannel].AM_ModulationON;
			break;
		case _BTN_DISPLAY_3:	//WAVE
			DDS_Variables[selectedChannel].AM_ModulationWaveType = (DDS_Variables[selectedChannel].AM_ModulationWaveType + 1) % (_NUM_WAVES - 1);
			break;
		case _BTN_DISPLAY_4:	//FREQ
			selectedParameter = e_PARAMETER_AM_FREQUENCY;
			GP_ResetTypingScreen(&DDS_Variables[selectedChannel].AM_ModulationFrequency, true, false);
			break;
		case _BTN_DISPLAY_5:	//MODULATION INDEX
			selectedParameter = e_PARAMETER_AM_MOD_INDX;
			GP_ResetTypingScreen(&DDS_Variables[selectedChannel].AM_ModulationIndex, true, false);
			break;
		}
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	menu_ProcessEncoder(btnEncoder, encoder);
}

static void btn_keypad(uint8_t KeyPressed){
	menu_ProcessKeypadPress(KeyPressed);
}
