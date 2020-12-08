/*
 * menu_sweep.c
 *
 *  Created on: 02 apr 2018
 *      Author: Manuel
 */

#include "common.h"
#include "../../hardware/hardware.h"
#include "../menu.h"
#include "../graphics/graphics.h"
#include "../graphics/menuBitmaps.h"
#include "../../lcd/GLCD.h"
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
static void Compute();

//Struttura che descrive una pagina del menu
const s_MenuPage menu_sweep = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

typedef enum {e_SWEEP_PARAM_NONE, e_SWEEP_PARAM_START_FREQUENCY, e_SWEEP_PARAM_STOP_FREQUENCY, e_SWEEP_PARAM_SWEEP_FREQUENCY} e_SweepParameter;
static e_SweepParameter selectedParameter;
static int selectedChannel = 0;
static s_DDSVariables savedDDSVariables[_NUM_DDS_CHANNELS];	//Variables used to save the DDS variables befour modify them
static bool savedDDSVariablesValid[_NUM_DDS_CHANNELS] = {false, false};

static void init(){
	selectedParameter	= e_SWEEP_PARAM_NONE;
	selectedChannel 	= 0;
	GP_ResetTypingScreen(NULL, false, false);
}

static void draw(){
	GLCD_ClrScr();
	GLCD_SetFont(_FONT_SMALL);

	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuSweep);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_PrintString(48, 5, "SWEEP");

	//Draw frequency sweep graph
	GLCD_Line(5, 60, 44 + 5, 30, true);
	GLCD_VLine(44 + 5, 30, 30, true);
	GLCD_Line(44 + 5, 60, 93, 30, true);
	GLCD_VLine(93, 30, 30, true);
	GLCD_HLine(2, 61, 94, true);

	//Draw sweep variables
	uint8_t textOffsetY = 65;
	const uint8_t fontSizeY = GLCD_GetFontSizeY(_FONT_TINY);
	GLCD_SetFont(_FONT_TINY);
	GLCD_PrintString(0, textOffsetY, "CHANNEL");	GLCD_PrintNumI(50, textOffsetY, selectedChannel, 1, ' ');
	textOffsetY += fontSizeY + 1;
	GLCD_PrintString(0, textOffsetY, "STATE");		GLCD_PrintString(50, textOffsetY, sweepVariables[selectedChannel].on.array[sweepVariables[selectedChannel].on.selected]);
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "START FREQ:", &sweepVariables[selectedChannel].startFrequency, (selectedParameter == e_SWEEP_PARAM_START_FREQUENCY));
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "STOP  FREQ:", &sweepVariables[selectedChannel].stopFrequency, (selectedParameter == e_SWEEP_PARAM_STOP_FREQUENCY));
	textOffsetY += fontSizeY + 1;
	PrintEditableValueWithSelectedDigit(0, textOffsetY, 50, "SWEEP FREQ:", &sweepVariables[selectedChannel].sweepFrequency,  (selectedParameter == e_SWEEP_PARAM_SWEEP_FREQUENCY));

	//Draw lateral buttons
	u_ButtonNames buttons[6] = { {.button=e_BTN_ARROW_RETURN}, {"CH_S"}, {" ON"}, {"STRT"}, {"STOP"}, {"S_FR"} };
	GP_DrawLateralButtons(buttons, (selectedParameter != e_SWEEP_PARAM_NONE ? selectedParameter + 2 : -1));

	//Show typing screen
	GP_DrawTypingScreen();

	//Compute the value of the dds variables in order to acive the desired sweep
	Compute();

	//Transfer screen buffer to LCD
	GLCD_Update();
}

static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
		case _BTN_DISPLAY_0:	//RETURN
			selectedParameter = e_SWEEP_PARAM_NONE;
			menu_PopPage();
			break;
		case _BTN_DISPLAY_1:	//CH_S
			selectedParameter = e_SWEEP_PARAM_NONE;
			selectedChannel = selectedChannel ? 0 : 1;
			break;
		case _BTN_DISPLAY_2:	//ON
			selectedParameter = e_SWEEP_PARAM_NONE;
			sweepVariables[selectedChannel].on.selected = sweepVariables[selectedChannel].on.selected ? 0 : 1;
			break;
		case _BTN_DISPLAY_3:	//START
			selectedParameter = e_SWEEP_PARAM_START_FREQUENCY;
			GP_ResetTypingScreen(&sweepVariables[selectedChannel].startFrequency, true, false);
			break;
		case _BTN_DISPLAY_4:	//STOP
			selectedParameter = e_SWEEP_PARAM_STOP_FREQUENCY;
			GP_ResetTypingScreen(&sweepVariables[selectedChannel].stopFrequency, true, false);
			break;
		case _BTN_DISPLAY_5:	//PER
			selectedParameter = e_SWEEP_PARAM_SWEEP_FREQUENCY;
			GP_ResetTypingScreen(&sweepVariables[selectedChannel].sweepFrequency, true, false);
			break;
		}
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	menu_ProcessEncoder(btnEncoder, encoder);
}

static void btn_keypad(uint8_t KeyPressed){
	menu_ProcessKeypadPress(KeyPressed);
}

//Compute the DDS variables
static void Compute(){
	for(unsigned int i = 0; i < 2; i++){			//For each DDS channel
		if(sweepVariables[i].on.selected == _ON){	//If the sweep is on
			if(savedDDSVariablesValid[i] == false){	//Save the DDS variables before modifying them
				savedDDSVariables[i] 		= DDS_Variables[i];
				savedDDSVariablesValid[i]	= true;
			}


			float startFrequencyHz = sweepVariables[i].startFrequency.floatValue;
			float stopFrequencyHz = sweepVariables[i].stopFrequency.floatValue;
			float sweepfrequencyHz = sweepVariables[i].sweepFrequency.floatValue;

			float frequencyDeviationHz	= (stopFrequencyHz - startFrequencyHz) / 2;
			float centerFrequencyHz 	= startFrequencyHz + frequencyDeviationHz;

			DDS_Variables[i].Frequency.floatValue 								= centerFrequencyHz;
			DDS_Variables[i].Frequency.currentMeasuringUnit						= e_FREQUENCY_Hz;
			DDS_Variables[i].FM_FrequencyDeviation.floatValue 					= frequencyDeviationHz;
			DDS_Variables[i].FM_FrequencyDeviation.currentMeasuringUnit			= e_FREQUENCY_Hz;
			DDS_Variables[i].FM_ModulationFrequency.floatValue 					= sweepfrequencyHz;
			DDS_Variables[i].FM_FrequencyDeviation.currentMeasuringUnit			= e_FREQUENCY_Hz;
			DDS_Variables[i].FM_ModulationWaveType								= e_WAVE_SAWTOOTH;
			DDS_Variables[i].FM_ModulationON 									= true;
		}
		else{	//If the channel is off and the variables were saved restore them
			if(savedDDSVariablesValid[i] == true){
				DDS_Variables[i] = savedDDSVariables[i];
				savedDDSVariablesValid[i] = false;
			}
		}
	}
}
