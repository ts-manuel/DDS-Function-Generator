/*
 * wavPlayer.c
 *
 *  Created on: 30 apr 2018
 *      Author: Windows7
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../delay/delay.h"
#include "../data_structures/dataStructures.h"
#include "../sd_card/fat_fs/ff.h"
#include "../sd_card/fat_fs/integer.h"

#define _USE_UART_DEBUG false

#if _USE_UART_DEBUG == true
	#define _REPORT_ERROR_AND_RETURN(error, b) {printf("%s\n", error); return b;}
#else
	#define _REPORT_ERROR_AND_RETURN(error, b) return b;
#endif


typedef struct{
	uint32_t	chunkSize;		//File size - 8 	(bytes)
	uint16_t 	numChannels;	//Number of audio channels
	uint32_t 	sampleRate;		//Sample rate 		(Samples per Second)
	uint32_t	byteRate;		//Byte rate			(Bytes per Second) 						SampleRate * NumChannels * BitsPerSample/8
	uint16_t	blockAlign;		//Number of Bytes for one sample including all channels		NumChannels * BitsPerSample/8
	uint16_t	bitsPerSample;	//Number of bits per each sample
	uint32_t	dataLength;		//Number of data bytes										NumSamples * NumChannels * BitsPerSample/8
	uint32_t	dataOffset;		//Number of bytes between the start of the file and the beginning of the data
} s_FileHeader;

bool wave_ReadHeader(s_FileHeader* pHeader, const FILINFO* pFile);

volatile bool updateRamL = false;
volatile bool updateRamH = false;

FILINFO	wavFile;
FIL		fp;
bool 	isFileValid = false;
bool 	isOpen		= false;
bool 	endOfFile 	= false;

/* Reads the header from the wave file, returns true if no ERROR */
bool wave_ReadHeader(s_FileHeader* pHeader, const FILINFO* pFile){
	//Open file
	FIL 	fp;
	FRESULT	fRes;

	pHeader->dataOffset = 0;

	fRes = f_open(&fp, pFile->fname, FA_READ);
	if(fRes != FR_OK)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() can't open file", false)

	//Read header
	UINT br;
	BYTE buffer[5]; buffer[4] = 0;
	f_read (&fp, &buffer, 4, &br);					//Read ChunkID = "RIFF"
	if(strcmp((char*)buffer, "RIFF") != 0)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() Header RIFF doesn't match", false)

	f_read (&fp, &(pHeader->chunkSize), 4, &br);	//Read ChunckSize = file_size - 8 (bytes)

	f_read (&fp, &buffer, 4, &br);					//Read Format = "WAVE"
	if(strcmp((char*)buffer, "WAVE") != 0)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() Header WAVE doesn't match", false)

	f_read (&fp, &buffer, 4, &br);					//Read Subchunk1ID = "fmt "
	if(strcmp((char*)buffer, "fmt ") != 0)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() Header fmt  doesn't match", false)

	uint32_t Subchunk1Size;
	f_read (&fp, &Subchunk1Size, 4, &br);			//Read Subchunk1Size = 16 for PCM
	//printf("Subchunk1Size: %ld\n", Subchunk1Size);
	pHeader->dataOffset += 20 + Subchunk1Size;
	//if(Subchunk1Size != 16)
	// _REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() data format is not PCM", false)

	uint16_t AudioFormat;
	f_read (&fp, &AudioFormat, 2, &br);				//Read AudioFormat = 1 no compression, other values indicate some sort of compression
	//printf("AudioFormat: %d\n", AudioFormat);
	if(AudioFormat != 1)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() can't decode compressed audio", false)

	f_read (&fp, &(pHeader->numChannels), 2, &br);	//Read numChannels: 1 = mono, 2 = stereo

	f_read (&fp, &(pHeader->sampleRate), 4, &br);	//Read sampleRate: (Samples per Second)

	f_read (&fp, &(pHeader->byteRate), 4, &br);		//Read byteRate: (Bytes per Second)

	f_read (&fp, &(pHeader->blockAlign), 2, &br);	//Read blockAlign: Number of Bytes for one sample including all channels

	f_read(&fp, &(pHeader->bitsPerSample), 2, &br);//Read bitsPerSample: Number of bit per each sample

	if(Subchunk1Size == 18 || Subchunk1Size == 40){
		uint16_t cbSize;
		f_read (&fp, &cbSize, 2, &br);		//Read cbSize: Size of extension 0 or 22
		if(cbSize == 22){					//Discard 22 bytes
			BYTE dbff[22];
			f_read (&fp, dbff, 22, &br);	//Read cbSize: Size of extension 0 or 22
		}
	}
	else if(Subchunk1Size != 16)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() Subchunk1Size invalid value", false)



	f_read (&fp, &buffer, 4, &br);					//Read Subchunk2ID = "data" or ckID = "fact"
	//printf("DATA or FACT: %s-\n", buffer);
	pHeader->dataOffset += 4;
	if(strcmp((char*)buffer, "fact") == 0){
		uint32_t cksize;
		f_read(&fp, &cksize, 4, &br);
		uint32_t dwSampleLength;
		f_read(&fp, &dwSampleLength, 4, &br);

		f_read(&fp, &buffer, 4, &br);				//Read Subchunk2ID = "data"
		pHeader->dataOffset += 12;
	}

	//printf("DATA: %s-\n", buffer);
	if(strcmp((char*)buffer, "data") != 0)
		_REPORT_ERROR_AND_RETURN("ERROR: wave_ReadHeader() Header data  doesn't match", false)

	f_read (&fp, &(pHeader->dataLength), 4, &br);	//Read dataLength: Number of bytes into the data segment
	pHeader->dataOffset += 4;

#if _USE_UART_DEBUG == true
	printf("WAVE Header\n"
		   "	chunkSize:      %ld\n"
		   "	numChannels:    %d\n"
		   "	sampleRate:     %ld\n"
		   "	byteRate:       %ld\n"
		   "	blockAlign:     %d\n"
		   "	bitsPerSample:  %d\n"
		   "	dataLength:     %ld\n",
		   pHeader->chunkSize, pHeader->numChannels, pHeader->sampleRate, pHeader->byteRate,
		   pHeader->blockAlign, pHeader->bitsPerSample, pHeader->dataLength);

	printf("data Offset: %ld\n", pHeader->dataOffset);
#endif



	f_close (&fp);	//Close file
	return true;
}


s_FileHeader waveHeader;

/*static inline int16_t ClampSample(int32_t sample){
	if(sample > 0)
		sample += 1280;
	else
		sample -= 1280;

	if(sample >= _FIXED_POINT_CONSTANT)
		sample = _FIXED_POINT_CONSTANT - 1;
	else if(sample <= -_FIXED_POINT_CONSTANT)
		sample = -_FIXED_POINT_CONSTANT + 1;

	return sample;
}*/

bool ramUpdate(volatile int16_t* ramL, volatile int16_t* ramR, s_FileHeader* header, FIL* file){
	uint8_t buffer[2048];
	UINT br;
	UINT bytesToRead;

	if(header->bitsPerSample == 8){
		bytesToRead = 512;
		f_read(file, buffer, bytesToRead, &br);
		if(ramL)
			for(unsigned int i = 0; i < 512; i++){
				ramL[i] = ((int)buffer[i] - 128) * 256;
			}
		if(ramR)
			for(unsigned int i = 0; i < 512; i++){
				ramR[i] = ((int)buffer[i] - 128) * 256;
			}
	}
	else if(header->bitsPerSample == 16){
		if(header->numChannels == 1){
			bytesToRead = 1024;
			f_read(file, buffer, bytesToRead, &br);
			if(ramL)
				for(unsigned int i = 0; i < 512; i++){
					ramL[i] = ((int16_t*)buffer)[i];
				}
			if(ramR)
				for(unsigned int i = 0; i < 512; i++){
					ramR[i] = ((int16_t*)buffer)[i];
				}
		}
		else if(header->numChannels == 2){
			bytesToRead = 2048;
			//f_read(file, buffer, bytesToRead, &br);
			f_read(file, buffer, 1024, &br);
			f_read(file, buffer + 1024, 1024, &br);
			if(ramL)
				for(unsigned int i = 0; i < 512; i++){
					int32_t sample = ((int16_t*)buffer)[i * 2];
					if(sample > 0)
						ramL[i] = sample += 1200;
					else
						ramL[i] = sample -= 1200;
				//ram[i] = ((int16_t*)buffer)[i * 2];
				}
			if(ramR)
				for(unsigned int i = 0; i < 512; i++){
					int32_t sample = ((int16_t*)buffer)[i * 2 + 1];
					if(sample > 0)
						ramR[i] = sample += 1200;
					else
						ramR[i] = sample -= 1200;
					//ramR[i] = ClampSample(sample);
					//ram[i] = ((int16_t*)buffer)[i * 2];
				}
		}
		else
			return false;
	}
	else
		return false;


	//Return false if the file is finished
	if(f_tell(file) >= header->dataOffset + header->dataLength){
		return false;
	}
	else
		return true;
}


void ram_wav_update(volatile int16_t* ramL, volatile int16_t* ramR){
	//bool endOfFile = false;

	if(isFileValid /*&& (updateRamL || updateRamH)*/){
		if(!isOpen){	//if the file is not open
			//printf("Opening File: %s\n", wavFile.fname);
			if(wave_ReadHeader(&waveHeader, &wavFile)){	//Read the header of the file
				//printf("Header read OK\n");
			}
			else{
				//printf("ERROR: unable to read Header\n");
				endOfFile  	= false;
				isFileValid	= false;
				isOpen 		= false;
				return;
			}

			f_open(&fp, wavFile.fname, FA_READ);		//Open the file
			f_lseek(&fp, waveHeader.dataOffset);		//Begin reading data from data block
			isOpen = true;
		}

		if(!ramUpdate(ramL, ramR, &waveHeader, &fp))
			endOfFile = true;

		/*if(updateRamL){
			if(!ramUpdate(&(ram[0]), &waveHeader, &fp))
				endOfFile = true;
			updateRamL = false;
		}
		if(updateRamH){
			if(!ramUpdate(&(ram[512]), &waveHeader, &fp))
				endOfFile = true;
			updateRamH = false;
		}*/
	}

	if(endOfFile){
		if(isOpen && !isFileValid){
			if(ramL)
				for(int i = 0; i < 512; i++)
					ramL[i] = 0;
			if(ramR)
				for(int i = 0; i < 512; i++)
					ramR[i] = 0;

			f_close(&fp);
			isFileValid	= false;
			isOpen 		= false;
		}
		else{
			f_lseek(&fp, waveHeader.dataOffset);
			endOfFile = false;
		}
	}
}
/*
void ram_dds0_AM_update(bool updateLow){

}
void ram_dds0_FM_update(bool updateLow){

}
void ram_dds1_update(bool updateLow){

}
void ram_dds1_AM_update(bool updateLow){

}
void ram_dds1_FM_update(bool updateLow){

}*/
