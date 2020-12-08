/*
 * delay.h
 *
 *  Created on: 06 dic 2017
 *      Author: Manuel
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

void 			delay_init();				//Inizializza il timer usato per i delay
unsigned int 	readTimer_us();				//Ritorna il valore del timer
unsigned int	GetTimeElapsed_us(unsigned int startTime);
void 			delay_us(unsigned int t);	//Blocca la CPU per il tempo specificato in us
void 			delay_ms(unsigned int ms);	//Blocca la CPU per il tempo specificato in ms

#endif /* SRC_DELAY_H_ */
