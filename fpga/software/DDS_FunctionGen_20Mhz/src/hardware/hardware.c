/*
 * hardwere.c
 *
 *  Created on: 27 gen 2018
 *      Author: Manuel
 */

#include "../lcd/GLCD.h"
#include "hardware.h"
#include "../definitions.h"
#include "../data_structures/dataStructures.h"
#include "../sd_card/wavPlayer.h"
#include <stdint.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <sys/alt_irq.h>
#include <math.h>
#include <stdio.h>

//Global variables
volatile uint8_t keypad 	= 0x00;
volatile uint8_t btnDisplay = 0x00;
volatile uint8_t btnEncoder = 0x00;
volatile int8_t	 encoder 	= 0;
volatile bool 	 resetDDS 	= true;

//Function declaration
static uint32_t ComputePhaseStep(s_EditableValue* editableValue);
static uint16_t ComputeAmplitude(s_EditableValue* editableValue, bool impedance50Ohm);
static uint32_t ComputePhaseOffset(s_EditableValue* phaseOffset);
static uint16_t ComputeAmplitudeOffset(s_EditableValue* amplitudeOffset);
static uint16_t ComputeModIndex(s_EditableValue* modulationIndex);
static void ResetDDSModules();
static void ISR_Keypad(void* context);
static void ISR_btnDisplay(void* context);
static void ISR_btnEncoder(void* context);
static void ISR_btnEnable(void* context);
static void ISR_lookupRam(void* context);

//Updates the variables onto the fpga
void updateVariablesOnFpga(){
	//DDS0
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PHASESTEP_BASE, 		ComputePhaseStep(&DDS_Variables[0].Frequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PHASEOFFSET_BASE, 		ComputePhaseOffset(&DDS_Variables[0].PhaseOffset));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PWM_AMPLITUDE_BASE,	ComputeAmplitude(&DDS_Variables[0].Amplitude, DDS_Variables[0].OutputEmpedance.selected == 0));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PWM_OFFSET_BASE, 		ComputeAmplitudeOffset(&DDS_Variables[0].DCOffset));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_OUTPUTRELAY_BASE, 		DDS_Variables[0].OutputRelay);

	uint16_t DDS0_AM_ModIndex 	= ComputeModIndex(&DDS_Variables[0].AM_ModulationIndex);
	DDS0_AM_ModIndex = (DDS_Variables[0].AM_ModulationON) ? DDS0_AM_ModIndex : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_AM_MODPHASESTEP_BASE,	ComputePhaseStep(&DDS_Variables[0].AM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_AM_MODINDEX_BASE, DDS0_AM_ModIndex);

	uint32_t DDS0_FM_ModDeviation = ComputePhaseStep(&DDS_Variables[0].FM_FrequencyDeviation);
	DDS0_FM_ModDeviation = (DDS_Variables[0].FM_ModulationON) ? DDS0_FM_ModDeviation : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_FM_MODPHASESTEP_BASE, 		ComputePhaseStep(&DDS_Variables[0].FM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_FM_MODDEVIATIONPHASE_BASE, DDS0_FM_ModDeviation);

	uint16_t DDS0_PM_ModIndex = ComputeModIndex(&DDS_Variables[0].PM_ModulationIndex);
	DDS0_PM_ModIndex = (DDS_Variables[0].PM_ModulationON)  ? DDS0_PM_ModIndex : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PM_MODPHASESTEP_BASE,	ComputePhaseStep(&DDS_Variables[0].PM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS0_PM_MODINDEX_BASE, DDS0_PM_ModIndex);

	//DDS1
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PHASESTEP_BASE, 		ComputePhaseStep(&DDS_Variables[1].Frequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PHASEOFFSET_BASE, 	 	ComputePhaseOffset(&DDS_Variables[1].PhaseOffset));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PWM_AMPLITUDE_BASE,	ComputeAmplitude(&DDS_Variables[1].Amplitude, DDS_Variables[1].OutputEmpedance.selected == 0));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PWM_OFFSET_BASE, 		ComputeAmplitudeOffset(&DDS_Variables[1].DCOffset));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_OUTPUTRELAY_BASE, 		DDS_Variables[1].OutputRelay);

	uint16_t DDS1_AM_ModIndex = ComputeModIndex(&DDS_Variables[1].AM_ModulationIndex);
	DDS1_AM_ModIndex = (DDS_Variables[1].AM_ModulationON) ? DDS1_AM_ModIndex : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_AM_MODPHASESTEP_BASE,	ComputePhaseStep(&DDS_Variables[1].AM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_AM_MODINDEX_BASE, 		DDS1_AM_ModIndex);

	uint32_t DDS1_FM_ModDeviation = ComputePhaseStep(&DDS_Variables[1].FM_FrequencyDeviation);
	DDS1_FM_ModDeviation = (DDS_Variables[1].FM_ModulationON) ? DDS1_FM_ModDeviation : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_FM_MODPHASESTEP_BASE, 		ComputePhaseStep(&DDS_Variables[1].FM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_FM_MODDEVIATIONPHASE_BASE,	DDS1_FM_ModDeviation);

	uint16_t DDS1_PM_ModIndex = ComputeModIndex(&DDS_Variables[1].PM_ModulationIndex);
	DDS1_PM_ModIndex = (DDS_Variables[1].PM_ModulationON) ? DDS1_PM_ModIndex : 0;
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PM_MODPHASESTEP_BASE,	ComputePhaseStep(&DDS_Variables[1].PM_ModulationFrequency));
	IOWR_ALTERA_AVALON_PIO_DATA(DDS1_PM_MODINDEX_BASE, 		DDS1_PM_ModIndex);

	ResetDDSModules();	//Reset DDS modules if needed

	//Enable Ram Interrupt only if wave is ARB and frequency is less than 500Hz
	uint8_t IRQ_Mask = 0x00;
	if(DDS_Variables[0].WaveType == e_WAVE_ARB && (DDS_Variables[0].Frequency.floatValue < 500.f))
		IRQ_Mask |= 0x01;
	if(DDS_Variables[0].AM_ModulationWaveType == e_WAVE_ARB && DDS_Variables[0].AM_ModulationON && (DDS_Variables[0].AM_ModulationFrequency.floatValue < 500.f))
		IRQ_Mask |= 0x02;
	if(DDS_Variables[0].FM_ModulationWaveType == e_WAVE_ARB && DDS_Variables[0].FM_ModulationON && (DDS_Variables[0].FM_ModulationFrequency.floatValue < 500.f))
		IRQ_Mask |= 0x04;

	if(DDS_Variables[1].WaveType == e_WAVE_ARB && (DDS_Variables[1].Frequency.floatValue < 500.f))
		IRQ_Mask |= 0x08;
	if(DDS_Variables[1].AM_ModulationWaveType == e_WAVE_ARB && DDS_Variables[1].AM_ModulationON && (DDS_Variables[0].AM_ModulationFrequency.floatValue < 500.f))
		IRQ_Mask |= 0x10;
	if(DDS_Variables[1].FM_ModulationWaveType == e_WAVE_ARB && DDS_Variables[1].FM_ModulationON && (DDS_Variables[0].FM_ModulationFrequency.floatValue < 500.f))
		IRQ_Mask |= 0x20;

	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LOOKUP_RAM_ISR_BASE, IRQ_Mask);
}

//Computes the phase step necessary in order to generate the specified frequency
static uint32_t ComputePhaseStep(s_EditableValue* editableValue){
	return (uint32_t)(editableValue->floatValue /** multiplier */* ((float)_DDS_PHASE_WORD_WHIDT / (float)_DDS_CLOCK_FREQ));
}

//Computes the amplitude of the signal
static uint16_t ComputeAmplitude(s_EditableValue* editableValue, bool impedance50Ohm){
	const float Offset = 14.06f;	//Correction factors
	const float Gain = 85.0f;

	float voltage = editableValue->floatValue / 1000.0f;
	voltage = impedance50Ohm ? voltage : voltage / 2;
	float dBm = 20 * log10f(voltage / sqrtf(0.05f));

	return (uint16_t)((dBm + Offset) * Gain);
}

//Computes the value of the phase offset
static uint32_t ComputePhaseOffset(s_EditableValue* phaseOffset){
	return (uint32_t)((phaseOffset->floatValue / phaseOffset->maxValue) * (float)_DDS_PHASE_WORD_WHIDT);
}

//Computes the value of the amplitude offset applied to the signal
static uint16_t ComputeAmplitudeOffset(s_EditableValue* amplitudeOffset){
	const float maxOffsetValue = 1023.0f;	//Value of the DAC when the offset is to the maximum
	const float minOffsetValue = 2940.0f;	//Value of the DAC when the offset is to the minimum

	return (uint16_t)(amplitudeOffset->floatValue * ((maxOffsetValue - minOffsetValue) / 100.0f) + minOffsetValue);
}

//Computes the modulation index
static uint16_t ComputeModIndex(s_EditableValue* modulationIndex){
	uint16_t modIndex;
	modIndex = (uint16_t)((modulationIndex->floatValue / modulationIndex->maxValue) * (float)_FIXED_POINT_CONSTANT);
	return modIndex;
}

static void ResetDDSModules(){
	if(resetDDS){
		IOWR_ALTERA_AVALON_PIO_DATA(DDS_RESET_BASE, 0xFF);
		IOWR_ALTERA_AVALON_PIO_DATA(DDS_RESET_BASE, 0x00);
		resetDDS = false;
	}
}

//---------------------------------------------------------------------------------------
//Interrupts

void InitInterrupts(){
	//Encoder interrupt
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BTN_ENCODER_BASE, 0b00011111);	//Setup interrupt mask
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_ENCODER_BASE, 0x00);		//Clear interrupt flag
	alt_ic_isr_register(BTN_ENCODER_IRQ_INTERRUPT_CONTROLLER_ID, BTN_ENCODER_IRQ, ISR_btnEncoder, 0, 0);
	//Display buttons interrupt
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BTN_DISPLAY_BASE, 0b00111111);	//Setup interrupt mask
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_DISPLAY_BASE, 0x00);		//Clear interrupt flag
	alt_ic_isr_register(BTN_DISPLAY_IRQ_INTERRUPT_CONTROLLER_ID, BTN_DISPLAY_IRQ, ISR_btnDisplay, 0, 0);
	//Keypad interrupt handler
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEYPAD_BASE, 0xFF);				//Setup interrupt mask
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEYPAD_BASE, 0x00);				//Clear interrupt flag
	alt_ic_isr_register(KEYPAD_IRQ_INTERRUPT_CONTROLLER_ID, KEYPAD_IRQ, ISR_Keypad, 0, 0);
	//
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BTN_CH_ONOFF_BASE, 0x03);		//Setup interrupt mask
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_CH_ONOFF_BASE, 0x00);		//Clear interrupt flag
	alt_ic_isr_register(BTN_CH_ONOFF_IRQ_INTERRUPT_CONTROLLER_ID, BTN_CH_ONOFF_IRQ, ISR_btnEnable, 0, 0);
	//Lookup ram interrupt
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LOOKUP_RAM_ISR_BASE, 0x00);		//Setup interrupt mask
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LOOKUP_RAM_ISR_BASE, 0x00);		//Clear interrupt flag
	alt_ic_isr_register(LOOKUP_RAM_ISR_IRQ_INTERRUPT_CONTROLLER_ID, LOOKUP_RAM_ISR_IRQ, ISR_lookupRam, 0, 0);
}

//Falling edge interrupt generated when a button is pressed or the encoder is rotated
static void ISR_btnEncoder(void* context){
	uint8_t value = ~IORD_ALTERA_AVALON_PIO_DATA(BTN_ENCODER_BASE);
	btnEncoder |= value & _BTN_ENCODER;

	//Process encoder rotation
	if(value & _ENCODER_0){			//IF ENCODER0 goes LOW
		if(!(value & _ENCODER_1)){	//IF ENCODER1 is HIGH
			encoder --;
		}
	}
	else if(value & _ENCODER_1){	//IF ENCODER1 goes LOW
		if(!(value & _ENCODER_0)){	//IF ENCODER0 if HIGH
			encoder ++;
		}
	}

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_ENCODER_BASE, 0x00);	//Clear interrupt flag
}

//Falling edge interrupt generated when a button of the display is pressed
static void ISR_btnDisplay(void* context){
	uint8_t value = ~IORD_ALTERA_AVALON_PIO_DATA(BTN_DISPLAY_BASE);
	btnDisplay |= value & _BTN_DISPLAY;

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_DISPLAY_BASE, 0x00);	//Clear interrupt flag
}

//Falling edge interrupt generated when a button on the keypad is pressed
static void ISR_Keypad(void* context){
	uint8_t value = IORD_ALTERA_AVALON_PIO_DATA(KEYPAD_BASE);
	if (value != 0xFF)
		keypad = value;

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEYPAD_BASE, 0x00);			//Clear interrupt flag
}

//Falling edge interrupt generated when one of the enable buttons is pressed
static void ISR_btnEnable(void* context){
	uint8_t value = ~IORD_ALTERA_AVALON_PIO_DATA(BTN_CH_ONOFF_BASE);
	if(value & _BTN_ENABLE_0)
		DDS_Variables[0].OutputRelay = !DDS_Variables[0].OutputRelay;
	if(value & _BTN_ENABLE_1)
		DDS_Variables[1].OutputRelay = !DDS_Variables[1].OutputRelay;

	resetDDS = true;

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTN_CH_ONOFF_BASE, 0x00);		//Clear interrupt flag
}

//Interrupt generated when the sink signal from the ram changes the state
uint8_t previusValue = 0;
static void ISR_lookupRam(void* context){
	uint8_t value = IORD_ALTERA_AVALON_PIO_DATA(LOOKUP_RAM_ISR_BASE);

	if((value & 0x01) != (previusValue & 0x01)  && ramLookupDDS[0].waveType == e_WAVE_ARB && ramLookupDDS[1].waveType == e_WAVE_ARB){
		if(value & 0x01)
			ram_wav_update(&(ramLookupDDS[0].ramLookup[0]), &(ramLookupDDS[1].ramLookup[0]));
		else
			ram_wav_update(&(ramLookupDDS[0].ramLookup[512]), &(ramLookupDDS[1].ramLookup[512]));
	}
	else if((value & 0x01) != (previusValue & 0x01)  && ramLookupDDS[0].waveType == e_WAVE_ARB){
		if(value & 0x01)
			ram_wav_update(&(ramLookupDDS[0].ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[0].ramLookup[512]), NULL);
	}
	else if(((value & 0x08) != (previusValue & 0x08))  && ramLookupDDS[1].waveType == e_WAVE_ARB && ramLookupDDS[0].waveType != e_WAVE_ARB){
		if(value & 0x08)
			ram_wav_update(&(ramLookupDDS[1].ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[1].ramLookup[512]), NULL);
	}

	if(((value & 0x02) != (previusValue & 0x02))  && ramLookupDDS[0].AM_waveType == e_WAVE_ARB && DDS_Variables[0].AM_ModulationON){
		if(value & 0x02)
			ram_wav_update(&(ramLookupDDS[0].AM_ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[0].AM_ramLookup[512]), NULL);
	}
	if(((value & 0x04) != (previusValue & 0x04))  && ramLookupDDS[0].FM_waveType == e_WAVE_ARB && DDS_Variables[0].FM_ModulationON){
		if(value & 0x04)
			ram_wav_update(&(ramLookupDDS[0].FM_ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[0].FM_ramLookup[512]), NULL);
	}
	if(((value & 0x10) != (previusValue & 0x10))  && ramLookupDDS[1].AM_waveType == e_WAVE_ARB && DDS_Variables[1].AM_ModulationON){
		if(value & 0x10)
			ram_wav_update(&(ramLookupDDS[1].AM_ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[1].AM_ramLookup[512]), NULL);
	}
	if(((value & 0x20) != (previusValue & 0x20)) && ramLookupDDS[1].FM_waveType == e_WAVE_ARB && DDS_Variables[1].FM_ModulationON){
		if(value & 0x20)
			ram_wav_update(&(ramLookupDDS[1].FM_ramLookup[0]), NULL);
		else
			ram_wav_update(&(ramLookupDDS[1].FM_ramLookup[512]), NULL);
	}

	previusValue = value;
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LOOKUP_RAM_ISR_BASE, 0x00);		//Clear interrupt flag
}
