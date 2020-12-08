/*
 * menu.c
 *
 *  Created on: 07 dic 2017
 *      Author: Manuel
 */

#include "menu.h"
#include "../lcd/GLCD.h"
#include "../hardware/hardware.h"
#include <stdio.h>
#include <system.h>
#include <math.h>
#include <altera_avalon_pio_regs.h>
#include "graphics/graphics.h"
#include "../data_structures/dataStructures.h"
#include "../common_functions/commonFunctions.h"
#include "../delay/delay.h"

static s_MenuPageStack pageStack	= {0, {}};
static bool redrawScreen = true;

//Static functions
static void menu_ClampSelectedDigit();

//Initialization of the menu global variables and draw default menu page
void menu_init(){
	menuPage = menu_main;	//Set menu_main as main page
	menuPage.menuPageCallStack = &pageStack;
}

//Input processing and menu rendering
void menu_draw(){
	//Input processing
	if(btnEncoder || encoder){
		(*menuPage.btn_encoder)(btnEncoder, encoder);
		btnEncoder 		= 0;
		encoder 		= 0;
		redrawScreen	= true;
	}
	if(btnDisplay){
		(*menuPage.btn_display)(btnDisplay);
		btnDisplay 		= 0;
		redrawScreen 	= true;
	}
	if(keypad != 0xFF){
		(*menuPage.btn_keypad)(keypad);
		keypad 			= 0xFF;
		redrawScreen 	= true;
	}

	//Redraw the menu screen
	if(redrawScreen){
		//unsigned int StartTime = readTimer_us();

		if(!menuPage.menuInitialized){			//If the menu page is not been initialized yet
			(*menuPage.init)();					//Initialize menu page
			menuPage.menuInitialized = true;
		}
		(*menuPage.draw)();
		GLCD_SetBackLight(menuVariables.backlight.floatValue * 10);
		redrawScreen = false;

		//printf("Redraw Screen: %d \n", GetTimeElapsed_us(StartTime));
	}
}

//Sets newMenuPage as the new menu page
void menu_PushPage(const s_MenuPage* newMenuPage){
	if(newMenuPage){
		s_MenuPageStack* p_stack = menuPage.menuPageCallStack;	//Save pointer to the stack
		p_stack->stack[p_stack->sp++] = menuPage;				//Push current page into stack
		menuPage = *newMenuPage;								//Copy new page into current page
		menuPage.menuPageCallStack = p_stack;					//Restore pointer to the stack
		menuPage.menuInitialized = false;
	}
}

//Restore the previous menu page from the stack
void menu_PopPage(){
	s_MenuPageStack* p_stack = menuPage.menuPageCallStack;	//Save pointer to the stack
	if(p_stack->sp > 0){									//If there are elements onto the stack
		menuPage = p_stack->stack[--p_stack->sp];
		menuPage.menuPageCallStack = p_stack;				//Restore pointer to the stack
	}
}

//Increments or decrements the value of the menu element
void menu_AddToMenuElement(s_MenuElement* menuElement, float value){
	if(menuElement->type == e_PARAM_TYPE_EDITABLE_VALUE){
		float newValue = menuElement->editableValue->floatValue + value;
		if(newValue <= menuElement->editableValue->maxValue && newValue >= menuElement->editableValue->minValue)
			menuElement->editableValue->floatValue = newValue;
	}
	else if(menuElement->type == e_PARAM_TYPE_STRING_ARRAY){
		int newSelectedIndex = menuElement->stringArray->selected + (int)value;
		if(newSelectedIndex < menuElement->stringArray->count && newSelectedIndex >= 0)
			menuElement->stringArray->selected = newSelectedIndex;
	}
}

void menu_ProcessKeypadPress(uint8_t KeyPressed){
	if(typingScreen.enabled){
		if(!typingScreen.initialized){
			typingScreen.buffer				= 0;
			typingScreen.show 				= true;
			typingScreen.numDigits 			= GetNumDigitsI(typingScreen.value->maxValue);
			typingScreen.initialized 		= true;
			typingScreen.decimalPoint		= false;
			typingScreen.numDecimalValues	= 0;
			typingScreen.invalidValue		= false;
		}

		if(KeyPressed < 0x0A){			//If Key pressed is a NUMBER
			if(!typingScreen.decimalPoint){
				typingScreen.buffer 		= typingScreen.buffer * 10 + KeyPressed;
				typingScreen.show 			= true;
			}
			else{
				typingScreen.buffer 		= typingScreen.buffer + KeyPressed / pow10f(++typingScreen.numDecimalValues);
				typingScreen.show 			= true;
			}
		}
		else if(KeyPressed < 0x10){		//If Key pressed is (A, B, C, D)
			unsigned int indexMeasuringUnitKey = KeyPressed - 0x0A;

			//Update measuring unit
			float multiplier = 1.0f;
			if(indexMeasuringUnitKey < typingScreen.value->availableMeasuringUnits->measuringUnitsCount){
				//typingScreen.value->measuringUnitSelectedIndex = indexMeasuringUnitKey;
				//multiplier = typingScreen.value->measuringUnitsMultipliers[indexMeasuringUnitKey];
				typingScreen.measuringUnitIndex = indexMeasuringUnitKey;
				multiplier = typingScreen.value->availableMeasuringUnits->measuringUnitsMultipliers[indexMeasuringUnitKey];
			}
			float finalValue = typingScreen.buffer * multiplier;

			if(finalValue > typingScreen.value->maxValue || finalValue < typingScreen.value->minValue){
				typingScreen.invalidValue 	= true;
			}
			else{
				typingScreen.invalidValue	= false;
			}

		}
		else if(KeyPressed == 0x10){		//If Key pressed is (#)
			if(!typingScreen.invalidValue){	//If the buffer is not 0 clears the buffer
				if(typingScreen.initialized && typingScreen.show){
					float finalValue = typingScreen.buffer * typingScreen.value->availableMeasuringUnits->measuringUnitsMultipliers[typingScreen.measuringUnitIndex];

					if(finalValue >= typingScreen.value->minValue && finalValue <= typingScreen.value->maxValue){	//If final value is in range
						typingScreen.show 				= false;
						typingScreen.initialized		= false;
						typingScreen.value->floatValue	= finalValue;
						typingScreen.value->currentMeasuringUnit = typingScreen.value->availableMeasuringUnits->measuringUnitsEnums[typingScreen.measuringUnitIndex];
					}
					else{ //else the value entered is invalid
						typingScreen.invalidValue 	= true;
						typingScreen.initialized	= false;
					}
				}
			}
			else{
				if(!typingScreen.initialized){
					typingScreen.show = false;
				}
				typingScreen.initialized 	= false;
				typingScreen.invalidValue 	= false;
			}
		}
		else if(KeyPressed == 0x11){	//If Key pressed is (*)
			typingScreen.decimalPoint = true;
		}
		menu_ClampSelectedDigit();
	}
}

void menu_ProcessEncoder(uint8_t btnEncoder, int8_t encoder){
	if(typingScreen.enabled){
		if(encoder){
			//float multiplier = typingScreen.value->measuringUnitsMultipliers[typingScreen.value->measuringUnitSelectedIndex];
			float i = typingScreen.value->floatValue + (float)encoder * pow10f((float)typingScreen.selectedDigit);
			float finalValue = i /** multiplier*/;

			if(finalValue >= typingScreen.value->minValue && finalValue <= typingScreen.value->maxValue)
				typingScreen.value->floatValue = i;
		}

		switch(btnEncoder){
		case _BTN_ENCODER_L:
			typingScreen.selectedDigit ++;	//Increment selected digit
			break;
		case _BTN_ENCODER_R:
			typingScreen.selectedDigit --;	//Decrement selected digit
			break;
		case _BTN_ENCODER_PB:
			//typingScreen.show = false;
			//typingScreen.updateVarible	= true;
			//typingScreen.initialized = false;
			break;
		}
		menu_ClampSelectedDigit();
	}
	else{
		/*if(encoder){
			if(encoder > 0)
				menu_IncrementSlectedElement();
			else
				menu_DecrementSelectedElement();
		}

		switch(btnEncoder){
		case _BTN_ENCODER_L:
			menu_DecrementSelectedParameter();
			break;
		case _BTN_ENCODER_R:
			menu_IncrementSlectedParameter();
			break;
		case _BTN_ENCODER_PB:
			menu_PushPage(menuPage.menuElements[menuPage.selectedElement].subMenu);
			break;
		}*/
	}
}

//clamps the value of the selected digit to the number of digits in the value
static void menu_ClampSelectedDigit(){
	if(typingScreen.value){	//if value is not NULL
		int numIntegerDigits = GetNumDigitsI((int)typingScreen.value->floatValue);
		int numDecimalDigits = GetNumDecimals(typingScreen.value->floatValue);

		if(typingScreen.selectedDigit >= numIntegerDigits)		//If selected digit is greater than the number of digits
			typingScreen.selectedDigit = numIntegerDigits - 1;	//Clamp to the maximum value
		if(typingScreen.selectedDigit < (-numDecimalDigits))	//If selected digit is less than the number of digits
			typingScreen.selectedDigit = (-numDecimalDigits);	//Clamp to minimum value
	}
}
