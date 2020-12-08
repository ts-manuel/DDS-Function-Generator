/*
 * commonFunctions.c
 *
 *  Created on: 21 feb 2018
 *      Author: Windows7
 */

#include <math.h>
#include "../data_structures/dataStructures.h"

//Returns the number of digits by witch the number is composed
int GetNumDigitsI(int num){
    char numDigits = 1;
    if(num < 0)
    	num = -num;

    while(num >= 10){
        num /= 10;
        numDigits++;
    }

    return numDigits;
}

//Returns the number of decimal digits
int GetNumDecimals(float f){
	unsigned int count = 0;

	while((f - floorf(f)) > 0.0f){
		f *= 10;
		count++;
	}

	return (count < 3) ? count : 3;
}

//Add a value to an editable value
void AddToEditableValue(s_EditableValue* editableValue, float value){
	float newValue = editableValue->floatValue + value;
	if(newValue <= editableValue->maxValue && newValue >= editableValue->minValue){
		editableValue->floatValue = newValue;
	}
}
