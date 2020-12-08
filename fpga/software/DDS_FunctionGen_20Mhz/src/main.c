/*
 * main.c
 *
 *  Created on: 27 gen 2018
 *      Author: Manuel
 */

#include <stdio.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <stdbool.h>
#include <math.h>
#include "delay/delay.h"
#include "hardware/hardware.h"
#include "hardware/ramLookupTables.h"
#include "lcd/GLCD.h"
#include "menu/menu.h"
#include "menu/graphics/menuBitmaps.h"
#include "sd_card/SPI.h"
#include "sd_card/wavPlayer.h"

void WriteToDebugLED(bool b){
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_DEBUG_BASE, b? 1 : 0);
}

void DrawBootScreen(){
	GLCD_SetFont(_FONT_SMALL);
	GLCD_ClrScr();
	GLCD_PrintString(1, 40, "DDS FunctionGen 20MHz");
	GLCD_PrintString(32, 60, "LOADING ...");
	GLCD_SetFont(_FONT_TINY);
	GLCD_PrintString(2, _LCD_HEIGHT - 12, "Software version: 1.0");
	GLCD_Update();//Transfer buffer to screen
}

int main(){
	WriteToDebugLED(false);

	//Initializations
	delay_ms(500);
	GLCD_Initialize(45);
	GLCD_SetBackLight(0x80);
	delay_ms(100);

	InitInterrupts();
	menu_init();
	SPI_Init();
	delay_ms(100);

	//Initialize ram lookup
	DrawBootScreen();	//Draw boot screen
	InitRamLookups();

	while(true){
		//WriteToDebugLED(true);

		menu_draw();
		updateVariablesOnFpga();
		UpdateRamLookups();

		//ram_dds0_update();


		//delay_ms(500);
		//WriteToDebugLED(false);
		//delay_ms(250);
	}

	return 0;
}
