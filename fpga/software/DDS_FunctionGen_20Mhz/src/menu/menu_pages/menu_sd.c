/*
 * menu_sd.c
 *
 *  Created on: 10 apr 2018
 *      Author: Manuel
 */

#include <altera_avalon_pio_regs.h>
#include "../../sd_card/SPI.h"
#include "../../sd_card/wavPlayer.h"
#include "common.h"
#include "../menu.h"
#include "../graphics/graphics.h"
#include "../graphics/menuBitmaps.h"
#include "../../lcd/GLCD.h"
#include "../../hardware/hardware.h"
#include "../../data_structures/dataStructures.h"
#include "../../common_functions/commonFunctions.h"
#include "../../sd_card/SD.h"
#include "../../sd_card/fat_fs/ff.h"
#include "../../sd_card/fat_fs/integer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <altera_avalon_spi_regs.h>
#include <system.h>

//Function declaration
static void init();
static void draw();
static void btn_display(uint8_t btnDisplay);
static void btn_encoder(uint8_t btnEncoder, int8_t encoder);
static void btn_keypad(uint8_t KeyPressed);
static void mountRootDir();
static void drawFileNames();
static void drawScrollbar(unsigned int selected, unsigned int count);
static void processOkButton();
//static void readSector(unsigned int sector);

//Struttura che descrive una pagina del menu
const s_MenuPage menu_sd = {&init, &draw, &btn_display, &btn_encoder, &btn_keypad, false, NULL};

static FATFS 	fs;     //File system object
static FRESULT 	res;
static char			currentDirPath[256];	//Current directory
static FILINFO  	selectedFile;
static bool 		driveMounted = false;
static unsigned int selectedFileIndex = 0;
static unsigned int fileCount = 1;

static void init(){
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LOOKUP_RAM_ISR_BASE, 0x00);
	mountRootDir();		//Mount the drive if it is not mounted yet
}

static void mountRootDir(){
	if(!driveMounted){	//Mount the drive if it is not mounted yet
		res = f_mount(&fs, "", 0);
		if(res != FR_OK)
			return;

		currentDirPath[0] = '/';
		currentDirPath[1] = 0;
		/*res = f_opendir(&currentDir, "/");	//Open root dir
		if(res != FR_OK)
			return;*/

		driveMounted = true;
	}
}

static void draw(){
	GLCD_ClrScr();
	GLCD_SetFont(_FONT_SMALL);
	//Draw bitmap on top left corner
	GLCD_DrawBitmap(0, 0, &bmp_28x28_MenuSD);
	GLCD_HLine(0, 27, 28, true);
	GLCD_VLine(28, 0, 28, true);
	//Draw title
	GLCD_PrintString(48, 5, "SD MENU");

	//Draw file names
	drawFileNames();

	//Draw scroll bar
	drawScrollbar(selectedFileIndex, fileCount);

	//Draw lateral buttons
	u_ButtonNames buttons[6] = { 	{.button=e_BTN_ARROW_RETURN}, {.button=e_BTN_ARROW_UP}, {.button=e_BTN_ARROW_DOWN},
									{.button=e_BTN_ARROW_LEFT}, {.button=e_BTN_ARROW_RIGHT}, {" OK "} };
	GP_DrawLateralButtons(buttons, -1);

	//Transfer screen buffer to LCD
	GLCD_Update();
}

static void drawFileNames(){
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LOOKUP_RAM_ISR_BASE, 0x00);
	FILINFO fno;
	DIR		dir;
	const int ofsetX = 10, offsetY = 40, fontHeight = GLCD_GetFontSizeY(_FONT_SMALL);
	fileCount = 1;

	if(selectedFileIndex == 0)
		GLCD_SetTextModeInverted(true);
	GLCD_PrintString(ofsetX, offsetY + fontHeight * fileCount, "..");
	GLCD_SetTextModeInverted(false);

	res = f_opendir(&dir, currentDirPath);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);                   // Read a directory item
			if (res != FR_OK || fno.fname[0] == 0) break;  // Break on error or end of dir

			if(fileCount++ == selectedFileIndex){
				GLCD_SetTextModeInverted(true);
				selectedFile = fno;
			}
			GLCD_PrintString(ofsetX, offsetY + fontHeight * fileCount, fno.fname);
			GLCD_SetTextModeInverted(false);

				/*if (fno.fattrib & AM_DIR) {                    // It is a directory
		        	i = strlen(path);
		        	sprintf(&path[i], "/%s", fno.fname);
		        	res = scan_files(path);                    // Enter the directory
		        	if (res != FR_OK) break;
		        		path[i] = 0;
		        } else {                                       // It is a file.
		        	printf("%s/%s\n", path, fno.fname);
		     	}*/
		}
		f_closedir(&dir);
	}

	GLCD_PrintString(10, 30, selectedFile.fname);

}

static void drawScrollbar(unsigned int selected, unsigned int count){
	const unsigned int offX = 2;
	const unsigned int offY = 40;
	const unsigned int width = 5;
	const unsigned int height = (_LCD_HEIGHT - 10) - offY;

	GLCD_Rectangle(offX, offY, offX + width, offY + height, true);
	unsigned int srOffY = offY + (unsigned int)(((float)height / (float)count) * (float)selected);
	unsigned int brHeight = (height / count) + 1;
	GLCD_SolidRectangle(offX, srOffY, offX + width, srOffY + brHeight, true);
}

extern bool endOfFile;

static void btn_display(uint8_t btnDisplay){
	switch(btnDisplay){
		case _BTN_DISPLAY_0:	//RETURN
			menu_PopPage();
			break;
		case _BTN_DISPLAY_1:	//UP
			if(selectedFileIndex > 0)
				selectedFileIndex --;
			break;
		case _BTN_DISPLAY_2:	//DOWN
			if(selectedFileIndex < (fileCount - 1))
				selectedFileIndex ++;
			break;
		case _BTN_DISPLAY_3:	//LEFT

			//readSector(0x81);
			endOfFile 	= true;
			isFileValid = false;

			break;
		case _BTN_DISPLAY_4:	//RIGHT

			break;
		case _BTN_DISPLAY_5:	//OK
			processOkButton();
			break;
		}
}

static void btn_encoder(uint8_t btnEncoder, int8_t encoder){
	if(encoder){
		int newIndex = selectedFileIndex + encoder;
		if(newIndex >= 0 && newIndex < fileCount)
			selectedFileIndex = newIndex;
	}

	switch(btnEncoder){
	case _BTN_ENCODER_L:
		break;
	case _BTN_ENCODER_R:
		break;
	case _BTN_ENCODER_PB:
		processOkButton();
		break;
	}
}

static void btn_keypad(uint8_t KeyPressed){

}

static void processOkButton(){
	if(selectedFileIndex == 0){	//Go back to parent directory
		for(int i = strlen(currentDirPath); i > 0; i--){
			if(currentDirPath[i] == '/'){
				currentDirPath[i] = 0;
				break;
			}
		}
	}
	else if(selectedFile.fattrib & AM_DIR){	//Enter in to directory
		sprintf(&currentDirPath[strlen(currentDirPath)], "/%s", selectedFile.fname);
		selectedFileIndex = 0;
	}
	else{	//Load file
		wavFile 	= selectedFile;
		isFileValid	= true;
		endOfFile	= false;
		//printf("File: %s loaded\n", selectedFile.fname);
	}
}

/*static void readSector(unsigned int sector){
	uint8_t buffer[512];

	SD_ReadSingleBlock(buffer, sector * 512);

	printf("\n");

	for(unsigned int i = 0; i < 512; i += 16){
		printf("0x%08lX: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", (uint32_t)i + sector * 512,
		buffer[i  ], buffer[i+1], buffer[i+ 2], buffer[i+ 3], buffer[i+ 4], buffer[i+ 5], buffer[i+ 6], buffer[i+ 7],
		buffer[i+8], buffer[i+9], buffer[i+10], buffer[i+11], buffer[i+12], buffer[i+13], buffer[i+14], buffer[i+15]);

		printf("    %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
		buffer[i  ], buffer[i+1], buffer[i+ 2], buffer[i+ 3], buffer[i+ 4], buffer[i+ 5], buffer[i+ 6], buffer[i+ 7],
		buffer[i+8], buffer[i+9], buffer[i+10], buffer[i+11], buffer[i+12], buffer[i+13], buffer[i+14], buffer[i+15]);
	}

	printf("\n");
}*/
