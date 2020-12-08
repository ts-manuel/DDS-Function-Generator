/*
 * definitions.h
 *
 *  Created on: 11 feb 2018
 *      Author: Windows7
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef SRC_HARDWARE_DEFINITIONS_H_
#define SRC_HARDWARE_DEFINITIONS_H_

//Definitions
#define _DDS_PHASE_WORD_WHIDT			0xFFFFFFFF	//Width of the phase word
#define _DDS_CLOCK_FREQ					200000000	//Clock frequency of the DDS module
#define _FIXED_POINT_CONSTANT			32767		//Costante per la quale moltiplicare i numeri float
													//per ottenere la rappresentazioe in virgola fissa
#define _NUM_DDS_CHANNELS 		2		//Number of dds channels
#define _LOOKUP_RAM_SIZE 		1024	//Size in bytes of the lookup ram
#define _NUM_WAVES				6		//Number of predefine waves

//#define _AMPLITUDE_WORD			0xFFFF
//#define _PHASE_OFFSET_WORD		0xFFFFFFFF
#define _AMPLITUDE_OFFSET_WORD	0x0FFF

//Maximum and minimum values
#define _MAX_FREQUENCY 					20000000	//Maximum frequency	Hertz
#define _MIN_FREQUENCY					0
#define _MAX_AMPLITUDE 					8500.0f		//Maximum amplitude milliVolts
#define _MIN_AMPLITUDE 					70.0f
#define _MAX_DC_OFFSET  				100.0f		//Maximum percentage of offset
#define _MIN_DC_OFFSET					0.0f
#define _MAX_PHASE_OFFSET				360.f		//Maximum phase offset in degrees
#define _MIN_PHASE_OFFSET				0.0f
#define _MAX_AM_MOD_INDEX				200.0f		//Maximum am modulation index (0 = 0%, 100 = 100%, 200 = 200%)
#define _MIN_AM_MOD_INDEX				0.0f
#define _MAX_PM_MOD_INDEX				100.0f		//Maximum pm modulation index
#define _MIN_PM_MOD_INDEX				0.0f
#define _MAX_AM_MOD_FREQUENCY			20000000	//Maximum am modulation frequency
#define _MAX_FM_MOD_FREQUENCY			20000000	//Maximum fm modulation frequency
#define _MAX_FM_MOD_DEVIATION			20000000	//Maximum fm frequency deviation
#define _MAX_PM_MOD_FREQUENCY			20000000	//Maximum pm modulation frequency

//Measuring units constants
#define _NUM_FREQUENCY_MEASURING_UNITS 				3
#define _NUM_AMPLITUDE_MEASURING_UNITS 				3
#define _NUM_PERCENTAGE_MEASURING_UNITS 			1
#define _DEFAULT_SELECTED_FREQUENCY_MEASURING_UNIT 	0
#define _DEFAULT_SELECTED_AMPLITUDE_MEASURING_UNIT 	0

#define _ON 	1
#define _OFF 	0

#endif /* SRC_HARDWARE_DEFINITIONS_H_ */
