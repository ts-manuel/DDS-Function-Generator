/*
 * delay.c
 *
 *  Created on: 06 dic 2017
 *      Author: Manuel
 */

#include <system.h>
#include <altera_avalon_timer_regs.h>
#include "delay.h"

//Definizioni da impostare in base al nome del timer utilizzato
#define _TIMER_BASE_ADDRESS TIMER_DELAY_32BIT_BASE
#define _TIMER_FREQUENCY	TIMER_DELAY_32BIT_FREQ
#define _TIMER_MAX_VALUE	TIMER_DELAY_32BIT_PERIOD

//Inizializza il timer usato per i delay
//THE TIMER COUNTS DOWN
void delay_init(){
	IOWR_ALTERA_AVALON_TIMER_CONTROL(_TIMER_BASE_ADDRESS, 0x04);	//Attivo il timer in modalità free-running
}

//Returns the current value of the timer that increments every microsecond
unsigned int readTimer_us(){
	unsigned int TimerValue = 0;
	IOWR_ALTERA_AVALON_TIMER_SNAPL(_TIMER_BASE_ADDRESS, 0xff);	//Scrittura che carica salva una snapshot del contatore

	TimerValue = (unsigned int)IORD_ALTERA_AVALON_TIMER_SNAPH(_TIMER_BASE_ADDRESS);
	TimerValue <<= 16;
	TimerValue |= (unsigned int)IORD_ALTERA_AVALON_TIMER_SNAPL(_TIMER_BASE_ADDRESS);

	return TimerValue;
}

//Returns the number of microsecond elapsed from the provided startTime (timer overflow every 80s)
unsigned int GetTimeElapsed_us(unsigned int startTime){
	unsigned int timerSnap = readTimer_us();	//Get a timer time stamp
	unsigned int elapsedClockCycles;

	if(startTime > timerSnap){	//If timer hasn't underflow
		elapsedClockCycles = startTime - timerSnap;
	}
	else{
		elapsedClockCycles = (_TIMER_MAX_VALUE - timerSnap) + startTime;
	}

	return elapsedClockCycles / (2500000000ULL / _TIMER_FREQUENCY);	//Divide the elapsed clock in order to get uS
}

//Blocca la CPU per il tempo specificato in us
void delay_us(unsigned int t){
	unsigned int TimerTick = t * (2500000000ULL / _TIMER_FREQUENCY);
	unsigned int TimerSnap = readTimer_us();

	if(TimerSnap > TimerTick){	//Gestione owerflow timer
		while(readTimer_us() > TimerSnap - TimerTick );
	}
	else{
		while(readTimer_us() < TimerSnap - TimerTick );
	}
}

//Blocca la CPU per il tempo specificato in ms
void delay_ms(unsigned int ms){
	delay_us(ms * 1000);
}

