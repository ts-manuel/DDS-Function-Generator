/*
 * wavPlayer.h
 *
 *  Created on: 30 apr 2018
 *      Author: Windows7
 */

#ifndef SRC_SD_CARD_WAVPLAYER_H_
#define SRC_SD_CARD_WAVPLAYER_H_

#include <stdbool.h>
#include <stdint.h>
#include "../sd_card/fat_fs/ff.h"
#include "../sd_card/fat_fs/integer.h"

	extern volatile bool updateRamL;
	extern volatile bool updateRamH;
	extern FILINFO	wavFile;
	extern bool 	isFileValid;

	void ram_wav_update(volatile int16_t* ramL, volatile int16_t* ramR);
	/*void ram_dds0_AM_update(bool updateLow);
	void ram_dds0_FM_update(bool updateLow);
	void ram_dds1_update(bool updateLow);
	void ram_dds1_AM_update(bool updateLow);
	void ram_dds1_FM_update(bool updateLow);*/

#endif /* SRC_SD_CARD_WAVPLAYER_H_ */
