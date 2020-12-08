/*
 * SD.h
 *
 *  Created on: 12 apr 2018
 *      Author: Manuel
 */

#ifndef SRC_SD_CARD_SD_H_
#define SRC_SD_CARD_SD_H_

#include <stdint.h>
#include <stdbool.h>
#include "fat_fs/diskio.h"
#include "fat_fs/integer.h"

#define _SD_DEBUG true

/*#if _SD_DEBUG == true
	#define _SD_PRINTF(x, ...) (printf(x))
#else
	#define _SD_PRINTF(x, ...)
#endif*/

#if _SD_DEBUG == false
	#define _DEBUG_CALL(x) x
#else
	#define _DEBUG_CALL(x)
#endif

	bool SD_Initialize();
	bool SD_ReadSingleBlock(uint8_t* buffer, uint32_t address);
	bool SD_ReadMultipleBlocks(uint8_t* buffer, uint32_t address, uint32_t count);

	DSTATUS MMC_disk_status();
	DSTATUS MMC_disk_initialize();
	DRESULT	MMC_disk_read(BYTE *buff, DWORD sector, UINT count);
	DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, UINT count);
	DRESULT MMC_disk_ioctl (BYTE cmd, void *buff);

#endif /* SRC_SD_CARD_SD_H_ */
