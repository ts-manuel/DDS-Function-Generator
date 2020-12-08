/*
 * common.c
 *
 *  Created on: 22 feb 2018
 *      Author: Windows7
 */

#include "common.h"
#include "../../common_functions/commonFunctions.h"
#include "../../data_structures/dataStructures.h"
#include "../../lcd/GLCD.h"
#include <stdbool.h>

void PrintEditableValueWithSelectedDigit(unsigned int x, unsigned int y, unsigned int numOffsX, const char* name, const s_EditableValue* editableValue, bool showCursor){
	const int selectedDigit 	= (showCursor) ? typingScreen.selectedDigit : _NO_CURSOR;
	const char* measuringUnit	= editableValue->availableMeasuringUnits->measuringUnitsNames[0];	//TODO: remove 0 and put good value

	GLCD_PrintString(x,	y, name);
	GLCD_PrintString(x + numOffsX + 35, y, measuringUnit);
	GLCD_printNumWithCursorF(x + numOffsX, y, editableValue->floatValue, 8, GetNumDecimals(editableValue->floatValue), '.', ' ', selectedDigit);
}
