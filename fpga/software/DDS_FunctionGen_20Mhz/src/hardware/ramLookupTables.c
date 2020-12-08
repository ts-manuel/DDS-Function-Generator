/*
 * ramLookupTables.c
 *
 *  Created on: 11 feb 2018
 *      Author: Windows7
 */

#include "ramLookupTables.h"
#include "../definitions.h"
#include "../data_structures/dataStructures.h"
#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//Static functions
static void UpdateRam(volatile int16_t* ram, e_WaveType waveType, const char** waveName);
static void	PrecomputeSineTable();
static int16_t ClampSample(int32_t sample);
static int16_t SquareWaveSample(int t);
static int16_t TriangleWaveSample(int t);
static int16_t SawtoothWaveSample(int t);
static int16_t SineWaveSample(int t);
static int16_t ArbWaveSample(int t);

static uint16_t sinePrecomputedArray[_LOOKUP_RAM_SIZE];

//Pre computes the values of the sine function
static void	PrecomputeSineTable(){
	for(unsigned int i = 0; i < _LOOKUP_RAM_SIZE; i++){
		sinePrecomputedArray[i] = ClampSample((int16_t)(sin(((M_PI * 2.0f) / (float)_LOOKUP_RAM_SIZE) * (float)i) * (float)(_FIXED_POINT_CONSTANT - 3000)));
	}
}

//initializes the ram lookup tables,
//this function needs to be called once before the ram lookups are used
void InitRamLookups(){
	PrecomputeSineTable();
}

//Checks if the lookup ram needs to be changed
void UpdateRamLookups(){
	for(int i = 0; i < _NUM_DDS_CHANNELS; i++){
		if(DDS_Variables[i].WaveType != ramLookupDDS[i].waveType){
			UpdateRam(ramLookupDDS[i].ramLookup, DDS_Variables[i].WaveType, &ramLookupDDS[i].waveTypeString);
			ramLookupDDS[i].waveType = DDS_Variables[i].WaveType;
		}
		if(DDS_Variables[i].AM_ModulationWaveType != ramLookupDDS[i].AM_waveType){
			UpdateRam(ramLookupDDS[i].AM_ramLookup, DDS_Variables[i].AM_ModulationWaveType, &ramLookupDDS[i].AM_waveTypeString);
			ramLookupDDS[i].AM_waveType = DDS_Variables[i].AM_ModulationWaveType;
		}
		if(DDS_Variables[i].FM_ModulationWaveType != ramLookupDDS[i].FM_waveType){
			UpdateRam(ramLookupDDS[i].FM_ramLookup, DDS_Variables[i].FM_ModulationWaveType, &ramLookupDDS[i].FM_waveTypeString);
			ramLookupDDS[i].FM_waveType = DDS_Variables[i].FM_ModulationWaveType;
		}
	}
}

//Updates the content of the lookup ram
static void UpdateRam(volatile int16_t* ram, e_WaveType waveType, const char** waveName){
	int16_t(*ComputeSample)(int t);	//Function pointer

	switch(waveType){
	case e_WAVE_SQUARE:
		ComputeSample = SquareWaveSample;
		break;
	case e_WAVE_TRIANGELE:
		ComputeSample = TriangleWaveSample;
		break;
	case e_WAVE_SAWTOOTH:
		ComputeSample = SawtoothWaveSample;
		break;
	case e_WAVE_SINE:
		ComputeSample = SineWaveSample;
		break;
	case e_WAVE_ARB:
		ComputeSample = ArbWaveSample;
		break;
	default:
		ComputeSample = SineWaveSample;
	}

	*waveName = str_WaveNames[(int)waveType];

	//Calculate samples for the entire lookup ram size
	for(int i = 0; i < _LOOKUP_RAM_SIZE; i++){
		ram[i] = ComputeSample(i);
	}
}

#define _ONE_QUARTER_WAVE 	(_LOOKUP_RAM_SIZE / 4)
#define _ONE_HALF_WAVE 		(_LOOKUP_RAM_SIZE / 2)
#define _ONE_THIRD_WAVE		(_ONE_HALF_WAVE + _ONE_QUARTER_WAVE)

//Computes the sample value of a square wave at a particular point in time t
static int16_t SquareWaveSample(int t){
	return (t < _LOOKUP_RAM_SIZE / 2) ? _FIXED_POINT_CONSTANT : -_FIXED_POINT_CONSTANT ;
}

static int16_t TriangleWaveSample(int t){
	int32_t sample;

	if(t < _ONE_QUARTER_WAVE){
		sample = t * (_FIXED_POINT_CONSTANT / _ONE_QUARTER_WAVE);
	}
	else if(t < _ONE_THIRD_WAVE){
		sample = _FIXED_POINT_CONSTANT - (t - _ONE_QUARTER_WAVE) * (_FIXED_POINT_CONSTANT * 2 / _ONE_HALF_WAVE);
	}
	else{
		sample = (t - _ONE_THIRD_WAVE) * (_FIXED_POINT_CONSTANT / _ONE_QUARTER_WAVE) - _FIXED_POINT_CONSTANT;
	}

	return ClampSample(sample);
}

static int16_t SawtoothWaveSample(int t){
	int32_t sample;

	sample = t * (_FIXED_POINT_CONSTANT / _ONE_HALF_WAVE) - _FIXED_POINT_CONSTANT;

	return ClampSample(sample);
}

static int16_t SineWaveSample(int t){
	return sinePrecomputedArray[t];
}

static int16_t ArbWaveSample(int t){

	//TODO: implement an arbitrary wave

	return 0;
}

static int16_t ClampSample(int32_t sample){
	if(sample > 0)
		sample += 1280;
	else
		sample -= 1280;

	if(sample >= _FIXED_POINT_CONSTANT)
		sample = _FIXED_POINT_CONSTANT - 1;
	else if(sample <= -_FIXED_POINT_CONSTANT)
		sample = -_FIXED_POINT_CONSTANT + 1;

	return sample;
}
