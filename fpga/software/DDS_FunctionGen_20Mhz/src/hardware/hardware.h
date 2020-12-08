/*
 * hardwere.h
 *
 *  Created on: 27 gen 2018
 *      Author: Manuel
 */

#include "../definitions.h"
#include "../data_structures/dataStructures.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef SRC_HARDWERE_HARDWERE_H_
#define SRC_HARDWERE_HARDWERE_H_

//External global variables
	extern volatile uint8_t keypad;

#define _BTN_DISPLAY 0b00111111
#define _BTN_DISPLAY_0	(1 << 0)
#define _BTN_DISPLAY_1	(1 << 1)
#define _BTN_DISPLAY_2	(1 << 2)
#define _BTN_DISPLAY_3	(1 << 3)
#define _BTN_DISPLAY_4	(1 << 4)
#define _BTN_DISPLAY_5	(1 << 5)
	extern volatile uint8_t btnDisplay;

#define _BTN_ENCODER 0b00011001
#define _BTN_ENCODER_PB	(1 << 0)
#define _ENCODER_1		(1 << 1)
#define _ENCODER_0		(1 << 2)
#define _BTN_ENCODER_R	(1 << 3)
#define _BTN_ENCODER_L	(1 << 4)
	extern volatile uint8_t btnEncoder;
	extern volatile int8_t	encoder;

#define _BTN_ENABLE_0	(1 << 0)
#define _BTN_ENABLE_1	(1 << 1)

	extern volatile bool	resetDDS;	//If set to true the DDS module is reseted (resets the phase difference between channels)

//Functions
	void InitInterrupts();			//Funzione che inizializza gli interrupt
	void updateVariablesOnFpga();	//Funzione che aggiorna i registri della fpga

#endif /* SRC_HARDWERE_HARDWERE_H_ */
