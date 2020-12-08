/*
 * menu_fm_modulation.c
 *
 *  Created on: 22 feb 2018
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
const s_MenuPage menu_fm_modulation = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};
static int selectedChannel = 0;

typedef enum {e_PARAMETER_NONE = 0, e_PARAMETER_FM_FREQUENCY = 4, e_PARAMETER_FM_FREQUENCY_DEV = 5} e_SelectedParameter;
static e_SelectedParameter selectedParameter;
static s_DDSVariables* selectedDDS;


static void init(){
	selectedDDS = &DDS_Variables[selectedChannel];
	selectedParameter = e_PARAMETER_NONE;
}

static void draw(){
	GLCD_ClrScr();
	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuFrequencyModulation);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_SetFont(_FONT_SMALL);
	GLCD_PrintString(39, 5, "FM MOD..");

	//Draw wave
	GP_DrawWave(20, selectedDDS->FM_ModulationWaveType, 50.0f);

	//Draw separator
	GLCD_HLine(0, 61, 94, true);

	//Draw AM modulation variables
	uint8_t textOffsetY = 65;
	const uint8_t fontSizeY = GLCD_GetFontSizeY(_FONT_TINY);
	GLCD_SetFont(_FONT_TINY);
	GLCD_PrintString(0, textOffsetY, "CHANNEL");	GLCD_PrintNumI(50, textOffsetY, selectedChannel, 1, ' ');
	textOffsetY += fontSizeY + 1;
	GLCD_PrintString(0, textOffsetY, "STATE");		GLCD_PrintString(50, textOffsetY, selectedDDS->FM_ModulationON ? "ON" : "OFF");
	textOffsetY += fontSizeY + 1;
	GLCD_PrintString(0,	 textOffsetY,"WAVE:              ");
	GLCD_PrintString(50, textOffsetY,   str_WaveNames[(int)selectedDDS->FM_ModulationWaveType]);
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "MOD FREQ:", &selectedDDS->FM_ModulationFrequency, (selectedParameter == e_PARAMETER_FM_FREQUENCY));
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "FREQ_DEV:", &selectedDDS->FM_FrequencyDeviation, (selectedParameter == e_PARAMETER_FM_FREQUENCY_DEV));

	//Draw buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {"CH_S"}, {" ON"}, {"WAVE"}, {"FREQ"}, {"DEVI"}};
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
			selectedDDS->FM_ModulationON = !selectedDDS->FM_ModulationON;
			resetDDS = true;	//Reset phase difference between channels
			break;
		case _BTN_DISPLAY_3:	//WAVE
			selectedDDS->FM_ModulationWaveType = (selectedDDS->FM_ModulationWaveType + 1) % (_NUM_WAVES - 1);
			break;
		case _BTN_DISPLAY_4:	//FREQ
			selectedParameter = e_PARAMETER_FM_FREQUENCY;
			GP_ResetTypingScreen(&selectedDDS->FM_ModulationFrequency, true, false);
			break;
		case _BTN_DISPLAY_5:	//MODULATION INDEX
			selectedParameter = e_PARAMETER_FM_FREQUENCY_DEV;
			GP_ResetTypingScreen(&selectedDDS->FM_FrequencyDeviation, true, false);
			break;
		}

	selectedDDS = &DDS_Variables[selectedChannel];	//Update selected channel
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	menu_ProcessEncoder(btnEncoder, encoder);

}

static void btn_keypad(uint8_t KeyPressed){
	menu_ProcessKeypadPress(KeyPressed);
}
