/*
 * dataStructures.h
 *
 *  Created on: 20 feb 2018
 *      Author: Windows7
 */

#ifndef SRC_DATA_STRUCTURES_DATASTRUCTURES_H_
#define SRC_DATA_STRUCTURES_DATASTRUCTURES_H_

#include "../definitions.h"
#include "../lcd/GLCD.h"

/* Structure that contains all the lookup rams */
typedef enum {e_WAVE_SQUARE, e_WAVE_TRIANGELE, e_WAVE_SAWTOOTH, e_WAVE_SINE, e_WAVE_ARB, e_WAVE_NONE} e_WaveType;
extern const char* str_WaveNames[_NUM_WAVES];
typedef struct{
	e_WaveType			waveType;
	const char* 		waveTypeString;
	volatile int16_t*	ramLookup;
	e_WaveType			AM_waveType;
	const char*			AM_waveTypeString;
	volatile int16_t*	AM_ramLookup;
	e_WaveType			FM_waveType;
	const char*			FM_waveTypeString;
	volatile int16_t*	FM_ramLookup;
} s_ramLookup;
extern s_ramLookup ramLookupDDS[_NUM_DDS_CHANNELS];	//Declared in ramLookupTables.c

/* Structure used to store a value composed of an array of strings */
typedef struct {
	int 		 selected;	//Index of the selected element into the array
	unsigned int count;		//Number of elements of the array
	const char** array;		//Array of strings
} s_StringArray;

/* Structure used to store a value that can be modified with the typing screen interface */
typedef enum {e_INTEGER, e_FLOAT} e_ValueType;
typedef enum {e_AMPLITUDE_MILLI_VOLT, e_AMPLITUDE_VOLT, e_FREQUENCY_Hz, e_FREQUENCY_KHz, e_FREQUENCY_MHz, e_PERCENTAGE} e_MeasuringUnit;
typedef struct {
	const unsigned int 		measuringUnitsCount;
	const e_MeasuringUnit*	measuringUnitsEnums;
	const char**				measuringUnitsNames;
	const float*				measuringUnitsMultipliers;
} s_AvMeasuringUnits;

typedef struct{
	char*				name;
	float				floatValue;
	float				maxValue;
	float				minValue;

	const s_AvMeasuringUnits* availableMeasuringUnits;
	e_MeasuringUnit		currentMeasuringUnit;
} s_EditableValue;

/* Structure that contains all the variables relative to a DDS channel */
typedef struct {
	s_StringArray	OutputEmpedance;

	s_EditableValue Frequency;
	s_EditableValue	Amplitude;
	s_EditableValue DCOffset;
	s_EditableValue	PhaseOffset;
	e_WaveType		WaveType;
	bool			OutputRelay;
	bool			AM_ModulationON;		//Turns on or of the amplitude modulation
	s_EditableValue	AM_ModulationFrequency;
	e_WaveType		AM_ModulationWaveType;
	s_EditableValue	AM_ModulationIndex;
	bool			FM_ModulationON;		//Turns on or of the frequency modulation
	s_EditableValue	FM_ModulationFrequency;
	e_WaveType		FM_ModulationWaveType;
	s_EditableValue	FM_FrequencyDeviation;
	bool			PM_ModulationON;		//Turns on or of the phase modulation
	s_EditableValue	PM_ModulationFrequency;
	e_WaveType		PM_ModulationWaveType;
	s_EditableValue	PM_ModulationIndex;
} s_DDSVariables;
extern s_DDSVariables DDS_Variables[_NUM_DDS_CHANNELS];

/* ---------------------------------- GRAPHICS VARIABLES -------------------------------------- */
typedef struct {
	bool		 		enabled;			//Enables or disables the typing buffer
	bool		 		initialized;		//Indicates if the typing screen is initialized
	bool		 		show;				//Indicates if the typing screen needs to be draw
	bool		 		invalidValue;		//Indicates that the value entered is invalid
	bool 		 		decimalPoint;		//Indicates if the decimal point is been pressed
	unsigned int		numDecimalValues;	//

	s_EditableValue*	value;				//Pointer to the value to be modified
	float 		 		buffer;				//Buffer that contains the digits entered by the user
	unsigned int		measuringUnitIndex;	//Index of the selected multiplier inside the value struct
	int 				selectedDigit;		//Indicates witch digit is currently selected by the cursor
	unsigned int 		numDigits;			//Indicates the number of digits inserted
} s_TypingScreen;
extern s_TypingScreen typingScreen;

typedef enum {e_BTN_ARROW_RETURN, e_BTN_ARROW_UP, e_BTN_ARROW_DOWN, e_BTN_ARROW_LEFT, e_BTN_ARROW_RIGHT} e_buttonsType;
typedef union {
	const char* 	text;
	e_buttonsType	button;
} u_ButtonNames;


/* ---------------------------------- MENU VARIABLES -------------------------------------- */
typedef struct st_MenuElement	s_MenuElement;
typedef struct st_MenuPageStack	s_MenuPageStack;
typedef struct st_MenuPage 		s_MenuPage;

struct st_MenuPage {
	void (*init)(void);											//Pointer to the function that initializes the menu page
	void (*draw)(void);											//Pointer to the function that draws the menu page
	void (*btn_display)	(uint8_t btnDisplay);					//Pointer to the function called when a button is pressed
	void (*btn_encoder)	(uint8_t btnEncoder, int8_t encoder);	//Pointer to the function called when the encoder is rotated or a button is pressed
	void (*btn_keypad)	(uint8_t KeyPressed);					//Pointer to the function called when a button on the keypad is pressed

	bool				menuInitialized;	//Flag that indicates if the fields of the menu need to be initialized
	s_MenuPageStack*	menuPageCallStack;
};

typedef enum {e_PARAM_TYPE_EDITABLE_VALUE, e_PARAM_TYPE_STRING_ARRAY} e_ParameterType;
struct st_MenuElement{
	const char* 		name;	//Name of the menu item
	e_ParameterType		type;	//Type of the menu item: e_PARAM_TYPE_EDITABLE_VALUE or e_PARAM_TYPE_STRING_ARRAY
	union{
		s_StringArray*		stringArray;
		s_EditableValue*	editableValue;
	};
};

struct st_MenuPageStack{
	unsigned int 	sp;
	s_MenuPage		stack[10];
};

//Menu global variables
typedef struct {
	unsigned int	selectedChannel;	//Indicates witch channel is currently selected
	s_EditableValue backlight;
} s_MenuVariables;
extern s_MenuVariables menuVariables;	//Global menu variables
extern s_MenuPage menuPage;				//Current page of the menu

//Struct that defines a menu icon
typedef struct {
	const char* 			name;		//Name of the menu page
	const s_GLCD_Bitmap*	p_bitmap;	//Bitmap to display
	const s_MenuPage*		p_menuPage;	//Menu page entered if the OK button is pressed and the icon is selected
} s_MenuIconPage;

typedef struct{
	s_StringArray	on;
	s_EditableValue	startFrequency;
	s_EditableValue	stopFrequency;
	s_EditableValue	sweepFrequency;
} s_SweepVariables;
extern s_SweepVariables sweepVariables[_NUM_DDS_CHANNELS];

#endif /* SRC_DATA_STRUCTURES_DATASTRUCTURES_H_ */
