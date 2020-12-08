/*
 * SPI.c
 *
 *  Created on: 12 apr 2018
 *      Author: Manuel
 */

#include <altera_avalon_spi_regs.h>
#include <altera_avalon_dma_regs.h>
#include <system.h>
#include "SPI.h"

void* txAddress;
void* rxAddress;
void* statusAddress;

// Initializes the SPI bus
void SPI_Init(){
	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 0x01);													//Disable all CS lines
	//uint8_t SPI_ControlRegister = IORD_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE);
	//IOWR_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE, SPI_ControlRegister | ALTERA_AVALON_SPI_CONTROL_SSO_MSK);	//Force CS always on when enabled

	txAddress 		= __IO_CALC_ADDRESS_NATIVE (SD_SPI_BASE, ALTERA_AVALON_SPI_TXDATA_REG);
	rxAddress 		= __IO_CALC_ADDRESS_NATIVE (SD_SPI_BASE, ALTERA_AVALON_SPI_RXDATA_REG);
	statusAddress 	= __IO_CALC_ADDRESS_NATIVE (SD_SPI_BASE, ALTERA_AVALON_SPI_STATUS_REG);
}

void SPI_CS(bool active){
	uint8_t SPI_ControlRegister = IORD_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE);
	if(active){
		IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 0x01);
		IOWR_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE, SPI_ControlRegister | ALTERA_AVALON_SPI_CONTROL_SSO_MSK);
	}
	else{
		IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 0x00);
		IOWR_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE, SPI_ControlRegister & ~ALTERA_AVALON_SPI_CONTROL_SSO_MSK);
	}
}

// Writes a byte to the SPI buss and waits until the end of the transaction
inline void SPI_WriteByte(uint8_t b){
	//IOWR_ALTERA_AVALON_SPI_TXDATA(SD_SPI_BASE, b);
	 __builtin_stwio (txAddress, b);


	//Wait until the transmitter shift register is empty
	//while(!(IORD_ALTERA_AVALON_SPI_STATUS(SD_SPI_BASE) & ALTERA_AVALON_SPI_STATUS_TMT_MSK));
}

void SPI_Read512Byte(uint8_t* buffer){
	IOWR_ALTERA_AVALON_DMA_RADDRESS(SPI_DMA_BASE, 0x00);
	IOWR_ALTERA_AVALON_DMA_WADDRESS(SPI_DMA_BASE, buffer);
	IOWR_ALTERA_AVALON_DMA_LENGTH(SPI_DMA_BASE, 512);
	IOWR_ALTERA_AVALON_DMA_CONTROL(SPI_DMA_BASE, ALTERA_AVALON_DMA_CONTROL_LEEN_MSK
			| ALTERA_AVALON_DMA_CONTROL_GO_MSK | ALTERA_AVALON_DMA_CONTROL_BYTE_MSK);

	while(IORD_ALTERA_AVALON_DMA_STATUS(SPI_DMA_BASE) & ALTERA_AVALON_DMA_STATUS_BUSY_MSK);
}

// Reads a byte from the SPI buss
inline uint8_t SPI_ReadByte(){
	/*while(!(IORD_ALTERA_AVALON_SPI_STATUS(SD_SPI_BASE) & ALTERA_AVALON_SPI_STATUS_TMT_MSK));	//Wait end of transaction
	return IORD_ALTERA_AVALON_SPI_RXDATA(SD_SPI_BASE);*/


	while(!(__builtin_ldwio(statusAddress) & ALTERA_AVALON_SPI_STATUS_TMT_MSK));
	return __builtin_ldwio(rxAddress);
}
