/*
 * manu_main.c
 *
 *  Created on: 07 dic 2017
 *      Author: Manuel
 */

#include "common.h"
#include "../menu.h"
#include "../graphics/graphics.h"
#include "../../lcd/GLCD.h"
#include "../../delay/delay.h"
#include "../../hardware/hardware.h"
#include "../../definitions.h"
#include "../../data_structures/dataStructures.h"
#include "../../common_functions/commonFunctions.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

//Function declaration
static void init();
static void draw();
static void btn_display(uint8_t btnDisplay);
static void btn_encoder(uint8_t btnEncoder, int8_t encoder);
static void btn_keypad(uint8_t KeyPressed);

static void DrawCH0();
static void DrawCH1();

//Struttura che descrive una pagina del menu
const s_MenuPage menu_main = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

typedef enum {e_PARAMETER_NONE = 0, e_PARAMETER_FREQUENCY = 3, e_PARAMETER_AMPLITUDE = 4, e_PARAMETER_DC_OFFSET = 5} e_SelectedParameter;
static e_SelectedParameter selectedParameter;

static void init(){
	selectedParameter = e_PARAMETER_NONE;
	GP_ResetTypingScreen(NULL, false, false);
}

//Rendering del menu
static void draw(){
//unsigned int StartTime = readTimer_us();

	GLCD_ClrScr();
//printf("GLCD_ClrScr(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();
	GLCD_SetFont(TinyFont);
//printf("GLCD_SetFont(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	DrawCH0();
//printf("DrawCH0(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	GLCD_HLine(0, _LCD_HALF_H - 1,	_LCD_WIDTH - 32, true);
	GLCD_HLine(0, _LCD_HALF_H,		_LCD_WIDTH - 32, true);
//printf("GLCD_HLine() * 2: %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	DrawCH1();
//printf("DrawCH1(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	//Lateral Buttons
	u_ButtonNames buttons[6] = {{"CH_S"}, {"MENU"}, {"WAVE"}, {"FREQ"}, {"AMPL"}, {"OFFS"}};
	int selectedButton = (selectedParameter != e_PARAMETER_NONE) ? (int)selectedParameter : -1;
	GP_DrawLateralButtons(buttons, selectedButton);
//printf("GP_DrawLateralButtons(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	//Draw typing screen
	GP_DrawTypingScreen();
//printf("GP_DrawTypingScreen(): %d \n", GetTimeElapsed_us(StartTime));
//StartTime = readTimer_us();

	//Update the lcd
	GLCD_Update();
//printf("GLCD_Update(): %d \n\n", GetTimeElapsed_us(StartTime));
}

static void DrawCH0(){
	const unsigned int chSel = menuVariables.selectedChannel;

	//Top row Text and graphics
	GLCD_SetTextModeInverted(menuVariables.selectedChannel == 0);
	GLCD_PrintString(0, 0, 	"CH0");
	GLCD_SetTextModeInverted(false);
	GLCD_PrintString(15, 0, DDS_Variables[0].OutputEmpedance.array[DDS_Variables[0].OutputEmpedance.selected]);
	GLCD_PrintString(37, 0, str_WaveNames[(int)DDS_Variables[0].WaveType]);
	GLCD_VLine(13, 0, 6, true);
	GLCD_VLine(35, 0, 6, true);
	GLCD_VLine(79, 0, 6, true);
	GLCD_HLine(0, 6, 80, true);

	GP_DrawWave(0, DDS_Variables[0].WaveType, DDS_Variables[0].DCOffset.floatValue);

	//Bottom Text (Freq, Ampl, Offs)
	PrintEditableValueWithSelectedDigit(0, _LCD_HALF_H - 18, 35, "FREQ:", &DDS_Variables[0].Frequency, (chSel == 0 && selectedParameter == e_PARAMETER_FREQUENCY));
	PrintEditableValueWithSelectedDigit(0, _LCD_HALF_H - 12, 35, "AMPL:", &DDS_Variables[0].Amplitude, (chSel == 0 && selectedParameter == e_PARAMETER_AMPLITUDE));
	PrintEditableValueWithSelectedDigit(0, _LCD_HALF_H -  6, 35, "OFFS:", &DDS_Variables[0].DCOffset,  (chSel == 0 && selectedParameter == e_PARAMETER_DC_OFFSET));
	GLCD_HLine(0, 		_LCD_HALF_H - 19, 	80, true);
}

static void DrawCH1(){
	const unsigned int chSel = menuVariables.selectedChannel;

	//Top row Text and graphics
	GLCD_SetTextModeInverted(menuVariables.selectedChannel  == 1);
	GLCD_PrintString(0,		_LCD_HALF_H + 1, "CH1");
	GLCD_SetTextModeInverted(false);
	GLCD_PrintString(15,	_LCD_HALF_H + 1, DDS_Variables[1].OutputEmpedance.array[DDS_Variables[1].OutputEmpedance.selected]);
	GLCD_PrintString(37,	_LCD_HALF_H + 1, str_WaveNames[(int)DDS_Variables[1].WaveType]);
	GLCD_VLine(13, 	_LCD_HALF_H + 1, 6, true);
	GLCD_VLine(35, 	_LCD_HALF_H + 1, 6, true);
	GLCD_VLine(79, 	_LCD_HALF_H + 1, 6, true);
	GLCD_HLine(0, 	_LCD_HALF_H + 7, 80, true);

	GP_DrawWave(_LCD_HALF_H + 1, DDS_Variables[1].WaveType, DDS_Variables[1].DCOffset.floatValue);

	//Bottom Text (Freq, Ampl, Offs)
	PrintEditableValueWithSelectedDigit(0, _LCD_HEIGHT - 17, 35, "FREQ:", &DDS_Variables[1].Frequency, (chSel == 1 && selectedParameter == e_PARAMETER_FREQUENCY));
	PrintEditableValueWithSelectedDigit(0, _LCD_HEIGHT - 11, 35, "AMPL:", &DDS_Variables[1].Amplitude, (chSel == 1 && selectedParameter == e_PARAMETER_AMPLITUDE));
	PrintEditableValueWithSelectedDigit(0, _LCD_HEIGHT -  5, 35, "OFFS:", &DDS_Variables[1].DCOffset,  (chSel == 1 && selectedParameter == e_PARAMETER_DC_OFFSET));
	GLCD_HLine(0, 		_LCD_HEIGHT - 18, 	80, true);
}

//-------------------------- Input Processing ------------------------------------------
static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
	case _BTN_DISPLAY_0:	//CHANNEL SELECT
		menuVariables.selectedChannel  = (menuVariables.selectedChannel  + 1) % _NUM_DDS_CHANNELS;
		selectedParameter = e_PARAMETER_NONE;
		GP_ResetTypingScreen(NULL, false, false);
		break;
	case _BTN_DISPLAY_1:	//MENU
		//menu_PushPage(&menu_menu);
		menu_PushPage(&menu_graphicMenu);
		selectedParameter = e_PARAMETER_NONE;
		GP_ResetTypingScreen(NULL, false, false);
		break;
	case _BTN_DISPLAY_2:	//WAVE
		DDS_Variables[menuVariables.selectedChannel].WaveType = (DDS_Variables[menuVariables.selectedChannel ].WaveType + 1) % (_NUM_WAVES - 1);
		break;
	case _BTN_DISPLAY_3:	//FREQUENCY
		selectedParameter = e_PARAMETER_FREQUENCY;
		GP_ResetTypingScreen(&DDS_Variables[menuVariables.selectedChannel].Frequency, true, false);
		break;
	case _BTN_DISPLAY_4:	//AMPLITUDE
		selectedParameter = e_PARAMETER_AMPLITUDE;
		GP_ResetTypingScreen(&DDS_Variables[menuVariables.selectedChannel].Amplitude, true, false);
		break;
	case _BTN_DISPLAY_5:	//PHASE OFFSET
		selectedParameter = e_PARAMETER_DC_OFFSET;
		GP_ResetTypingScreen(&DDS_Variables[menuVariables.selectedChannel].DCOffset, true, false);
		break;
	}
}


static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	menu_ProcessEncoder(btnEncoder, encoder);
}


static void btn_keypad(uint8_t KeyPressed){
	menu_ProcessKeypadPress(KeyPressed);
}
