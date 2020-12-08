/*
 * dataStructure.c
 *
 *  Created on: 20 feb 2018
 *      Author: Windows7
 *
 *      Contains all the global variables used in the project
 */

#include "dataStructures.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* ------------------------------------------------------------ DDS VARIABLES ------------------------------------------------------------ */
static const char* 				str_Impedance[]	= {"50Ohm", "1MOhm"};
static const char* 				str_OnOff[]		= {"OFF", "ON"};

static const e_MeasuringUnit 	frequencyMeasuringUnitsEnums[_NUM_FREQUENCY_MEASURING_UNITS] 		= {e_FREQUENCY_Hz, e_FREQUENCY_KHz, e_FREQUENCY_MHz};
static const char* 				frequencyMeasuringUnitsNames[_NUM_FREQUENCY_MEASURING_UNITS] 		= {"Hz", "KHz", "MHz"};
static const float				frequencyMeasuringUnitsMultipliers[_NUM_FREQUENCY_MEASURING_UNITS] 	= {1.0f, 1000.0f, 1000000.0f};
const s_AvMeasuringUnits frequencyMeasuringUnits = {_NUM_FREQUENCY_MEASURING_UNITS, frequencyMeasuringUnitsEnums, frequencyMeasuringUnitsNames, frequencyMeasuringUnitsMultipliers};

static const e_MeasuringUnit 	amplitudeMeasuringUnitsEnums[_NUM_AMPLITUDE_MEASURING_UNITS] 		= {e_AMPLITUDE_MILLI_VOLT, e_AMPLITUDE_VOLT};
static const char* 				amplitudeMeasuringUnitsNames[_NUM_AMPLITUDE_MEASURING_UNITS] 		= {"mV", "V"};
static const float				amplitudeMeasuringUnitsMultipliers[_NUM_AMPLITUDE_MEASURING_UNITS] 	= {1.0f, 1000.0f};
const s_AvMeasuringUnits amplitudeMeasuringUnits = {_NUM_AMPLITUDE_MEASURING_UNITS, amplitudeMeasuringUnitsEnums, amplitudeMeasuringUnitsNames, amplitudeMeasuringUnitsMultipliers};

static const e_MeasuringUnit	percentageMeasuringUnitsEnums[_NUM_PERCENTAGE_MEASURING_UNITS] 			= {e_PERCENTAGE};
static const char* 				percentageMeasuringUnitsNames[_NUM_PERCENTAGE_MEASURING_UNITS] 			= {"%"};
static const float				percentageMeasuringUnitsMultipliers[_NUM_PERCENTAGE_MEASURING_UNITS]	= {1.0f};
const s_AvMeasuringUnits percentageMeasuringUnits = {_NUM_AMPLITUDE_MEASURING_UNITS, percentageMeasuringUnitsEnums, percentageMeasuringUnitsNames, percentageMeasuringUnitsMultipliers};

s_DDSVariables DDS_Variables[_NUM_DDS_CHANNELS] = {
	//DDS0
		{{0, 2, str_Impedance},
		{"FREQUENCY", 1000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"AMPLITUDE",  500.0f, _MAX_AMPLITUDE, _MIN_AMPLITUDE, &amplitudeMeasuringUnits, e_AMPLITUDE_MILLI_VOLT},
		{"DC_OFFSET",   50.0f, _MAX_DC_OFFSET, _MIN_DC_OFFSET, &percentageMeasuringUnits, e_PERCENTAGE},
		{"PHASE OFFSET", 0.0f, _MAX_PHASE_OFFSET, _MIN_PHASE_OFFSET, &percentageMeasuringUnits, e_PERCENTAGE},
		e_WAVE_SQUARE,	//WaveType;
		false,			//OutputRelay;
		false,			//AM_ModulationON;
		{"FREQUENCY",    1000.0f, _MAX_AM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//AM_ModulationFrequency;
		e_WAVE_SQUARE,	//AM_ModulationWaveType;
		{"AM MOD INDX",    10.0f, _MAX_AM_MOD_INDEX, _MIN_AM_MOD_INDEX, &percentageMeasuringUnits, e_PERCENTAGE}, 	//AM_ModulationIndex;
		false,			//FM_ModulationON;		//Turns on or of the frequency modulation
		{"FREQUENCY",    1000.0f, _MAX_FM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//FM_ModulationFrequency;
		e_WAVE_SQUARE,	//FM_ModulationWaveType;
		{"FM DEVIATION", 1000.0f, _MAX_FM_MOD_DEVIATION, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//FM_FrequencyDeviation;
		false,			//PM_ModulationON;		//Turns on or of the phase modulation
		{"FREQUENCY",    1000.0f, _MAX_PM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//PM_ModulationFrequency;
		e_WAVE_SQUARE,	//PM_ModulationWaveType;
		{"PM MOD INDX",    10.0f, _MAX_PM_MOD_INDEX, 	_MIN_PM_MOD_INDEX, &percentageMeasuringUnits, e_PERCENTAGE} //PM_ModulationIndex;
		},
	//DDS1
		{{0, 2, str_Impedance},
		{"FREQUENCY", 1000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"AMPLITUDE", 500.0f, _MAX_AMPLITUDE, _MIN_AMPLITUDE, &amplitudeMeasuringUnits, e_AMPLITUDE_MILLI_VOLT},
		{"DC_OFFSET", 50.0f, _MAX_DC_OFFSET, _MIN_DC_OFFSET, &percentageMeasuringUnits, e_PERCENTAGE},
		{"PHASE OFFSET", 0.0f, _MAX_PHASE_OFFSET, _MIN_PHASE_OFFSET, &percentageMeasuringUnits, e_PERCENTAGE}, 		//PM_PhaseOffset;
		e_WAVE_SQUARE,	//WaveType;
		false,			//OutputRelay;
		false,			//AM_ModulationON;
		{"FREQUENCY", 1000.0f, _MAX_AM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//AM_ModulationFrequency;
		e_WAVE_SQUARE,	//AM_ModulationWaveType;
		{"AM MOD INDX", 10.0f, _MAX_AM_MOD_INDEX, _MIN_AM_MOD_INDEX, &percentageMeasuringUnits, e_PERCENTAGE}, 		//AM_ModulationIndex;
		false,			//FM_ModulationON;		//Turns on or of the frequency modulation
		{"FREQUENCY", 1000.0f, _MAX_FM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//FM_ModulationFrequency;
		e_WAVE_SQUARE,	//FM_ModulationWaveType;
		{"FM DEVIATION", 1000.0f, _MAX_FM_MOD_DEVIATION, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//FM_FrequencyDeviation;
		false,			//PM_ModulationON;		//Turns on or of the phase modulation
		{"FREQUENCY", 1000.0f, _MAX_PM_MOD_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},	//PM_ModulationFrequency;
		e_WAVE_SQUARE,	//PM_ModulationWaveType;
		{"PM MOD INDX", 10.0f, _MAX_PM_MOD_INDEX, _MIN_PM_MOD_INDEX, &percentageMeasuringUnits, e_PERCENTAGE}, 		//PM_ModulationIndex;
		}};


/* ------------------------------------------------------------ RAM LOOKUP TABLES ------------------------------------------------------------ */
const char* str_WaveNames[_NUM_WAVES] = {"SQUARE", "TRIANGLE", "SAWTOOTH", "SINE", "ARB", "NONE"};
volatile int16_t __attribute__((section(".ram_dds0"))) 		ramDDS0[_LOOKUP_RAM_SIZE];
volatile int16_t __attribute__((section(".ram_dds0_am"))) 	ramDDS0_am[_LOOKUP_RAM_SIZE];
volatile int16_t __attribute__((section(".ram_dds0_fm"))) 	ramDDS0_fm[_LOOKUP_RAM_SIZE];
//DDS1 ram
volatile int16_t __attribute__((section(".ram_dds1")))		ramDDS1[_LOOKUP_RAM_SIZE];
volatile int16_t __attribute__((section(".ram_dds1_am"))) 	ramDDS1_am[_LOOKUP_RAM_SIZE];
volatile int16_t __attribute__((section(".ram_dds1_fm"))) 	ramDDS1_fm[_LOOKUP_RAM_SIZE];
//Lookup ram array
s_ramLookup ramLookupDDS[_NUM_DDS_CHANNELS] = {{e_WAVE_NONE, NULL, ramDDS0,
												e_WAVE_NONE, NULL, ramDDS0_am,
												e_WAVE_NONE, NULL, ramDDS0_fm},
											   {e_WAVE_NONE, NULL, ramDDS1,
												e_WAVE_NONE, NULL, ramDDS1_am,
												e_WAVE_NONE, NULL, ramDDS1_fm }};


/* ------------------------------------------------------------ GRAPHICS VARIABLES ------------------------------------------------------------ */
s_TypingScreen 	typingScreen;


/* ------------------------------------------------------------ MENU VARIABLES ------------------------------------------------------------ */
s_MenuPage menuPage;				//Current page of the menu
s_MenuVariables	menuVariables = {	//Menu global variables
		0,		//DDS channel selected
		{"BACK_LIGHT", 15.0f, 25.0f, 0.0f, 0, 0, NULL, NULL, NULL}};

s_SweepVariables sweepVariables[2] = {
	   {{_OFF, 2, str_OnOff}, //on
		{"START FREQUANCY", 1000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"STOP FREQUANCY", 10000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"PERIOD", 1.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz}},
	   {{_OFF, 2, str_OnOff}, //on
		{"START FREQUANCY", 1000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"STOP FREQUANCY", 10000.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz},
		{"PERIOD", 1.0f, _MAX_FREQUENCY, _MIN_FREQUENCY, &frequencyMeasuringUnits, e_FREQUENCY_Hz}}
};
