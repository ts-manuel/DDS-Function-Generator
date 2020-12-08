/*
 * SPI.h
 *
 *  Created on: 12 apr 2018
 *      Author: Manuel
 */

#include <stdint.h>
#include <stdbool.h>
#include <altera_avalon_spi_regs.h>
#include <system.h>

#ifndef SRC_SD_CARD_SPI_H_
#define SRC_SD_CARD_SPI_H_

/*#define _SPI_CS_LOW 	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 0x01);
#define _SPI_CS_HIGH	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 0x00);*/

	void 	SPI_Init();
	void 	SPI_CS(bool active);
	void 	SPI_WriteByte(uint8_t b);
	uint8_t	SPI_ReadByte();
	void 	SPI_Read512Byte(uint8_t* buffer);


#endif /* SRC_SD_CARD_SPI_H_ */
