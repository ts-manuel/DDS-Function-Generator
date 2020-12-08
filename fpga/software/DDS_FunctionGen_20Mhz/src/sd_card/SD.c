/*
 * SD.c
 *
 *  Created on: 12 apr 2018
 *      Author: Manuel
 */

#include "SD.h"
#include "SPI.h"
#include <stdio.h>

#define _CMD0 	 0	//GO_IDLE_STATE 			R1 	Resets the SD Memory Card
#define _CMD1	 1	//SEND_OP_COND				R1	Sends host capacity support information and activates the card's initialization process. HCS is effective when card receives SEND_IF_COND command. Reserved bits shall be set to 0
#define _CMD6 	 6	//SWITCH_FUNC				R1	Checks switchable function (mode 0) and switches card function (mode 1)
#define _CMD8 	 8	//SEND_IF_COND				R7	Sends SD Memory Card interface condition that includes host supply voltage information and asks the accessed card whether card can operate in supplied voltage range. Reserved bits shall be set to 0
#define _CMD9 	 9	//SEND_CSD					R1	Asks the selected card to send its card specific data (CSD)
#define _CMD10	10	//SEND_CID					R1	Asks the selected card to send its card identification (CID)
#define _CMD12	12	//STOP_TRANSMISSION			R1b	Forces the card to stop transmission in Multiple Block Read Operation
#define _CMD13	13	//SEND_STATUS				R2	Asks the selected card to send its status register
#define _CMD16	16	//SET_BLOCKLEN				R1	In case of SDSC Card, block length is set by this command. In case of SDHC and SDXC Cards, block length of the memory access commands are fixed to 512 bytes. The length of LOCK_UNLOCK command is set by this command regardless of card capacity.
#define _CMD17	17	//READ_SINGLE_BLOCK			R1	Reads a block of the size selected by the SET_BLOCKLEN command.
#define _CMD18	18	//READ_MULTIPLE_BLOCK		R1	Continuously transfers data blocks from card to host until interrupted by a STOP_TRANSMISSION command.
#define _CMD24	24	//WRITE_BLOCK				R1	Writes a block of size selected by the SET_BLOCKLEN command.
#define _CMD25	25	//WRITE_MULTIPLE_BLOCK		R1	Continuously writes blocks of data until 'Stop Tran' token is sent (instead 'Start Block').
#define _CMD27	27	//PROGRAM_CSD				R1	Programming of the programmable bits of the CSD.
#define _CMD28	28	//SET_WRITE_PROT			R1b	If the card has write protection features, this command sets the write protection bit of the addressed group. The properties of write protection are coded in the card specific data (WP_GRP_SIZE). SDHC and SDXC Cards do non support this command.
#define _CMD29	29	//CLR_WRITE_PROT			R1b	If the cars has write protection features, this command clears the write protection bit of the addressed group. SDHC and SDXC Cards do not support this command.
#define _CMD30	30	//SEND_WRITE_PROT			R1	If the card has write protection features, this command asks the cars to send the status of the write protection bits. SDHC and SDXC Cards do not support this command.
#define _CMD32	32	//ERASE_WR_BLK_START_ADDR	R1	Sets the address of the first write block to be erased.
#define _CMD33	33	//ERASE_WR_BLK_END_ADDR		R1	Sets the address of the last write block of the continuous range to be erased.
#define _CMD38	38	//ERASE						R1b	Erase all previously selected write blocks. FULE and DISCARD are not supported through SPI interface.
#define _CMD42	42	//LOCK_UNLOCK				R1	Used to Set/Reset the Password or lock/unlock the card. A transferred data block includes all the command details. The size of the Data Block is defined with SET_BLOCK_LEN command. Reserved bits in the argument and in Lock Card Data Structure shall ba set to 0
#define _CMD55	55	//APP_CMD					R1	Defines to the card that the next command is an application specific command rather than a standard command
#define _CMD56	56	//GEN_CMD					R1	Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands. In case of Standard Capacity SD Memory Card, the size of the Data Block shall be defined with SET_BLOCK_LEN command. In case of SDHC and SDXC Cards, block length of this command is fixed to 512-byte.
#define _CMD58	58	//READ_OCR					R3	Reads the OCR register of a card. CSS bit is assigned to OCR[30].
#define _CMD59	59	//CRC_ON_OFF				R1	Turns the CRC option on or off, A '1' in the CRC option bit will turn the option on, a '0' will turn it off
//Application specific commands, shall be preceded with APP_CMD (CMD55)
#define _ACMD13	13	//SD_STATUS					R2	Send the SD Status. The status fields are given in Table 4-44
#define _ACMD18	18	//--						--	Reserved for SD security applications
#define _ACMD22	22	//SEND_NUM_WR_BLOCKS		R1	Send the number of well written (without errors) blocks. Responds with 32-bit+CRC data block.
#define _ACMD23	23	//SET_WR_BLK_ERASE_COUNT	R1	Set the number of write blocks to be pre-erased before writing (to be used for faster Multiple Block  WR command). "1"=default(one wr block).
#define _ACMD25	25	//--						--	Reserved for SD security applications
#define _ACMD26	26	//--						--	Reserved for SD security applications
#define _ACMD38	38	//--						--	Reserved for SD security applications
#define _ACMD41	41	//SD_SEND_OP_COND			R1	Sends host capacity support information and activates the card's initialization process. reserved bits shall be set to '0'
#define _ACMD42	42	//SET_CLR_CARD_DETECT		R1	Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CS (pin 1) of the card. The pull-up may be used for card detection.
#define _ACMD43	43	//--						--	Reserved for SD security applications
#define _ACMD49	49	//--						--	Reserved for SD security applications
#define _ACMD51	51	//SEND_SCR					R1	Reads the SD Configuration register (SCR).

//Bits of the response
#define _R1_IN_IDLE_STATE_MSK			(1<<0)
#define _R1_ERASE_RESET_MSK				(1<<1)
#define _R1_ILLEGAL_COMMAND_MSK			(1<<2)
#define _R1_COM_CRC_ERROR_MSK			(1<<3)
#define _R1_ERASE_SEQUENCE_ERROR_MSK	(1<<4)
#define _R1_ADDRESS_ERROR_MSK			(1<<5)
#define _R1_PARAMETER_ERROR_MSK			(1<<6)

typedef struct{
	uint8_t  R1;
	uint32_t OCR;
} s_ResponseR3;

typedef struct{
	uint8_t R1;
	uint8_t CommandVersion;
	uint8_t VoltageAccepted;
	uint8_t CheckPattern;
} s_ResponseR7;

//SD card types
typedef enum {e_SD_NONE, e_SD_Ver1X, e_SD_Ver200_SC, e_SD_Ver200_HC_XC} e_SDType;
static e_SDType sdType = e_SD_NONE;

//STA_NOINIT		0x01	/* Drive not initialized */
//STA_NODISK		0x02	/* No medium in the drive */
//STA_PROTECT		0x04	/* Write protected */
static DSTATUS sdStatus = STA_NOINIT;

//Functions prototypes
static void 		SD_SendCommand(uint8_t cmdNumber, uint32_t data);
static void 		SD_SendApplicationSpecificCommand(uint8_t cmdNumber, uint32_t data);
static uint8_t 		SD_ReadResponseR1();
static uint8_t 		SD_ReadResponseR1b();
//static uint16_t 	SD_ReadResponseR2();
static s_ResponseR3 SD_ReadResponseR3();
static s_ResponseR7 SD_ReadResponseR7();
static void 		SD_SetBlockLength(uint32_t length);
//static void 		SD_ReadSingleBlock(uint8_t* buffer, uint32_t address);

static void 		GenerateCRCTable();
static uint8_t 		CRCAdd(uint8_t CRC, uint8_t message_byte);
static uint8_t		GetCommandCRC(uint8_t cmdNumber, uint32_t data);

//Function used by the FatFs library: returns the status of the SD card
DSTATUS MMC_disk_status(){
	return sdStatus;
}

DSTATUS MMC_disk_initialize(){
	if(SD_Initialize())
		sdStatus = 0;
	else
		sdStatus = STA_NOINIT;

	return sdStatus;
}

/*
 * Return values
		RES_OK = 0,		0: Successful
		RES_ERROR,		1: R/W Error
		RES_WRPRT,		2: Write Protected
		RES_NOTRDY,		3: Not Ready
		RES_PARERR		4: Invalid Parameter
*/
DRESULT	MMC_disk_read(BYTE *buff, DWORD sector, UINT count){

	if(count > 1){
		//_DEBUG_CALL(printf("MMC_disk_read tried to read %d bytes\n", (int)count * 512));
		if(!SD_ReadMultipleBlocks(buff, sector * 512, count)){
			printf("ERROR: SD_ReadMultipleBlocks() returned false\n");
			return RES_ERROR;
		}
	}
	else{
		if(!SD_ReadSingleBlock(buff, sector * 512)){
			printf("ERROR: SD_ReadSingleBlock() returned false\n");
			return RES_ERROR;
		}
	}

	/*for(int i = 0; i < count; i++){
		if(!SD_ReadSingleBlock(buff + (i * 512), (sector + i) * 512)){
			_DEBUG_CALL(printf("ERROR: SD_ReadSingleBlock() returned false\n"));
			return RES_ERROR;
		}
	}*/

	return RES_OK;
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, UINT count){

	return RES_OK;
}

DRESULT MMC_disk_ioctl (BYTE cmd, void *buff){
	_DEBUG_CALL(printf("WARNING: disk_ioctl is been called!!!\n"));
	return RES_OK;
}

//Initializes the SD card into SPI mode
bool SD_Initialize(){
	_DEBUG_CALL(printf("SD Card initialization sequence...\n"));
	SPI_Init();
	GenerateCRCTable();

	//Send 80 clock cycles to synchronize SD
	SPI_CS(false);
	for(unsigned int i = 0; i < 10; i++)
		SPI_WriteByte(0xFF);

	//Send CMD0
	SPI_CS(true);
	SD_SendCommand(_CMD0, 0x00000000);
	//CMD0 response
	uint8_t cmd0response = 0;
	for(unsigned int i = 0; i < 100 && cmd0response != 0x01; i++)
		cmd0response = SD_ReadResponseR1();

	//Send CMD8
	SD_SendCommand(_CMD8, 0x000001AA);
	s_ResponseR7 r7Resp = SD_ReadResponseR7();

	if((r7Resp.R1 & _R1_ILLEGAL_COMMAND_MSK) || (r7Resp.R1 & 0x80)){	//Ver1.X SD Memory Card or Not SD Memory Card
		_DEBUG_CALL(printf("Ver1.X SD Memory Card or Not SD Memory Card\n"));

		//Send CMD58
		SD_SendCommand(_CMD58, 0);
		s_ResponseR3 r3Resp = SD_ReadResponseR3();
		if(r3Resp.OCR & 0xFF8000){	//Compatible voltage range
			uint8_t r1Response = _R1_IN_IDLE_STATE_MSK;
			for(unsigned int i = 0; i < 100 && (r1Response & _R1_IN_IDLE_STATE_MSK); i++){
				SD_SendApplicationSpecificCommand(_ACMD41, 0);
				r1Response = SD_ReadResponseR1();
			}
			if(r1Response & _R1_IN_IDLE_STATE_MSK){
				_DEBUG_CALL(printf("Failed initialization\n"));
				sdType = e_SD_NONE;
				return false;	//Failed initialization
			}

			_DEBUG_CALL(printf("Ver1.x Standard Capacity SD Memory Card\n"));
			sdType = e_SD_Ver1X;

		}
		else{	//Non compatible voltage range
			_DEBUG_CALL(printf("Non compatible voltage range\n"));
			sdType = e_SD_NONE;
			return false;	//Failed initialization
		}

	}
	else{	//Ver2.00 or later SD Memory Card
		_DEBUG_CALL(printf("Ver2.00 or later SD Memory Card\n"));

		if(r7Resp.VoltageAccepted == 0x01 && r7Resp.CheckPattern == 0xAA){	//Compatible voltage range and check pattern is correct
			//Send CMD58
			SD_SendCommand(_CMD58, 0);
			s_ResponseR3 r3Resp = SD_ReadResponseR3();
			if(r3Resp.OCR & 0xFF8000){	//Compatible voltage range
				uint8_t r1Response = _R1_IN_IDLE_STATE_MSK;
				for(unsigned int i = 0; i < 100 && (r1Response & _R1_IN_IDLE_STATE_MSK); i++){
					SD_SendApplicationSpecificCommand(_ACMD41, 0/*(1 << 30)*/);
					r1Response = SD_ReadResponseR1();
				}
				if(r1Response & _R1_IN_IDLE_STATE_MSK){
					_DEBUG_CALL(printf("Non compatible voltage range or check pattern error\n"));
					sdType = e_SD_NONE;
					return false;	//Failed initialization
				}

				SD_SendCommand(_CMD58, 0);
				s_ResponseR3 r3Resp = SD_ReadResponseR3();
				if(r3Resp.OCR & (1 << 30)){
					_DEBUG_CALL(printf("Ver2.00 or later High Capacity or Extended Capacity SD Memory Card\n"));
					sdType = e_SD_Ver200_HC_XC;
				}
				else{
					_DEBUG_CALL(printf("Ver2.00 or later Standard Capacity SD Memory Card\n"));
					sdType = e_SD_Ver200_SC;
				}
			}
			else{	//Non compatible voltage range or check pattern error
				_DEBUG_CALL(printf("Non compatible voltage range or check pattern error\n"));
				sdType = e_SD_NONE;
				return false;	//Failed initialization
			}
		}
		else{	//Non compatible voltage range or check pattern error
			_DEBUG_CALL(printf("Non compatible voltage range or check pattern error\n"));
			sdType = e_SD_NONE;
			return false;	//Failed initialization
		}
	}

	//If Standard capacity SD Card set the block length to 512Byte (SDHC and SDXC block length is always 521 Byte)
	if(sdType == e_SD_Ver1X || sdType == e_SD_Ver200_SC)
		SD_SetBlockLength(512);

	SPI_CS(false);
	return true;
}

// Send a command to the SD (01 + cmdNumber + data + crc + 1)
static void SD_SendCommand(uint8_t cmdNumber, uint32_t data){
	_DEBUG_CALL(printf("CMD%d  data: %lX\n", cmdNumber, data));
	//Start bits 01 + CMD number
	SPI_WriteByte(0x40 | cmdNumber);

	//DATA
	SPI_WriteByte(data >> 24);
	SPI_WriteByte(data >> 16);
	SPI_WriteByte(data >> 8);
	SPI_WriteByte(data);

	//CRC + stop bit
	SPI_WriteByte((GetCommandCRC(cmdNumber, data) << 1) | 0x01);

	//Dummy byte to give time to the SD to stuff
	SPI_WriteByte(0xFF);
}

static void SD_SendApplicationSpecificCommand(uint8_t cmdNumber, uint32_t data){
	SD_SendCommand(_CMD55, 0x00000000);
	SD_ReadResponseR1();
	SD_SendCommand(cmdNumber, data);
}

// Read the R1 response from the SD card
static uint8_t SD_ReadResponseR1(){
	uint8_t response;

	SPI_WriteByte(0xFF); response = SPI_ReadByte();
	_DEBUG_CALL(printf("  ResponseR1: %X\n", response));

	return response;
}

// Read the R1b response from the SD card
static uint8_t SD_ReadResponseR1b(){
	uint8_t response = 0;

	//Wait until the response is 0
	for(unsigned int i = 0; i < 100 && !response; i++){
		SPI_WriteByte(0xFF); response = SPI_ReadByte();
		_DEBUG_CALL(printf("  ResponseR1b: %X\n", response));
	}
	_DEBUG_CALL(printf("  ResponseR1b: %X\n", response));

	return response;
}
/*
// Read the R2 response from the SD card
static uint16_t SD_ReadResponseR2(){
	uint16_t response = 0;

	SPI_WriteByte(0xFF); response  = SPI_ReadByte() << 8;
	SPI_WriteByte(0xFF); response |= SPI_ReadByte();
	_SD_PRINTF("  ResponseR2: %X\n", response);

	return response;
}*/

// Read the R3 response from the SD card
static s_ResponseR3 SD_ReadResponseR3(){
	s_ResponseR3 response;

	SPI_WriteByte(0xFF); response.R1   = SPI_ReadByte();
	SPI_WriteByte(0xFF); response.OCR  = SPI_ReadByte() << 24;
	SPI_WriteByte(0xFF); response.OCR |= SPI_ReadByte() << 16;
	SPI_WriteByte(0xFF); response.OCR |= SPI_ReadByte() << 8;
	SPI_WriteByte(0xFF); response.OCR |= SPI_ReadByte();
	_DEBUG_CALL(printf("  ResponseR3: R1 %X, OCR: %lX\n", response.R1, response.OCR));

	return response;
}

// Read the R7 response from the SD card
static s_ResponseR7 SD_ReadResponseR7(){
	s_ResponseR7 response;

	SPI_WriteByte(0xFF); response.R1 			  = SPI_ReadByte();
	SPI_WriteByte(0xFF); response.CommandVersion  = SPI_ReadByte() >> 4;
	SPI_WriteByte(0xFF);
	SPI_WriteByte(0xFF); response.VoltageAccepted = SPI_ReadByte() & 0x0F;
	SPI_WriteByte(0xFF); response.CheckPattern 	  = SPI_ReadByte();
	_DEBUG_CALL(printf("  ResponseR7: R1 %X, CommandVersion: %X, VoltageAccepted: %X, CheckPattern: %X\n",
			response.R1, response.CommandVersion, response.VoltageAccepted, response.CheckPattern));

	return response;
}


//Sets the reading and writing block size
static void SD_SetBlockLength(uint32_t length){
	SD_SendCommand(_CMD16, length);
	SD_ReadResponseR1();
};

//Reads a single block from the SD card (512 byte)
bool SD_ReadSingleBlock(uint8_t* buffer, uint32_t address){
	SPI_CS(true);

	_DEBUG_CALL(printf("SD_ReadSingleBlock()\n"));

	SD_SendCommand(_CMD17, address);
	SD_ReadResponseR1();

	unsigned int responseToken = 0;
	unsigned int timeoutCounter = 0;
	do{
		SPI_WriteByte(0xFF);
		responseToken = SPI_ReadByte();
		_DEBUG_CALL(printf(" ResponseTkn: 0x%2X \n", responseToken));
	} while(responseToken != 0xFE && timeoutCounter++ < 500);
	if(timeoutCounter >= 500)
		return false;

	//Read 512Byte block
	/*for(uint16_t i = 0; i < 512; i++){
		SPI_WriteByte(0xFF);
		buffer[i] = SPI_ReadByte();
	}*/
	SPI_Read512Byte(buffer);

	//Read CRC
	uint16_t crc16;
	SPI_WriteByte(0xFF); crc16  = SPI_ReadByte() << 8;
	SPI_WriteByte(0xFF); crc16 |= SPI_ReadByte();
	SPI_CS(false);

	return true;
}

//Reads multiple consecutive blocks from the SD card (512 byte * count)
bool SD_ReadMultipleBlocks(uint8_t* buffer, uint32_t address, uint32_t count){
	SPI_CS(true);

	_DEBUG_CALL(printf("SD_ReadSingleBlock()\n"));

	SD_SendCommand(_CMD18, address);
	SD_ReadResponseR1();

	unsigned int responseToken = 0;
	unsigned int timeoutCounter = 0;
	do{
		SPI_WriteByte(0xFF);
		responseToken = SPI_ReadByte();
		_DEBUG_CALL(printf(" ResponseTkn: 0x%2X \n", responseToken));
	} while(responseToken != 0xFE && timeoutCounter++ < 500);
	if(timeoutCounter >= 500)
		return false;

	//Read count blocks
	for(uint32_t i = 0; i < count; i++){
		SPI_Read512Byte(buffer + (512 * i));

		//Read CRC
		uint16_t crc16;
		SPI_WriteByte(0xFF); crc16  = SPI_ReadByte() << 8;
		SPI_WriteByte(0xFF); crc16 |= SPI_ReadByte();
	}

	//STOP_TRANSMISSINO command
	SD_SendCommand(_CMD12, 0x00);
	SD_ReadResponseR1b();

	SPI_CS(false);
	return true;
}

// ---------------------------------------------------------------
// CRC Functions
static uint8_t CRCTable[256];

static void GenerateCRCTable(){
	int i, j;
	uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial

	// generate a table value for all 256 possible byte values
	for (i = 0; i < 256; ++i) {
		CRCTable[i] = (i & 0x80) ? i ^ CRCPoly : i;
		for (j = 1; j < 8; ++j) {
			CRCTable[i] <<= 1;
			if (CRCTable[i] & 0x80)
				CRCTable[i] ^= CRCPoly;
		}
	}
}

// adds a message byte to the current CRC-7 to get a the new CRC-7
static uint8_t CRCAdd(uint8_t CRC, uint8_t message_byte){
	return CRCTable[(CRC << 1) ^ message_byte];
}

// returns the CRC-7 for a command
static uint8_t GetCommandCRC(uint8_t cmdNumber, uint32_t data){
	uint8_t crc = 0;

	crc = CRCAdd(crc, (0x40 | cmdNumber));
	crc = CRCAdd(crc, data >> 24);
	crc = CRCAdd(crc, data >> 16);
	crc = CRCAdd(crc, data >> 8);
	crc = CRCAdd(crc, data);

	return crc;
}
