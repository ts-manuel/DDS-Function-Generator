/*
 * lcd_NHD_C128128.c
 *
 *  Created on: 28 gen 2018
 *      Author: Manuel
 */

#include <stdbool.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <altera_avalon_dma_regs.h>
#include "../delay/delay.h"
#include "GLCD.h"

#define _USE_DMA true	//if true DMA is used to transfer the screen buffer to the LCD

//#define _LCD_DATA_INPUT		ALTERA_AVALON_PIO_DIRECTION_INPUT
//#define _LCD_DATA_OUTPUT	ALTERA_AVALON_PIO_DIRECTION_OUTPUT
#define _LCD_CS		(1 << 0)
#define _LCD_RST	(1 << 1)
#define _LCD_A0		(1 << 2)
#define _LCD_RW		(1 << 3)
#define _LCD_E		(1 << 4)
#define _LCD_SET_CONTROL_BIT(x) 	IOWR_ALTERA_AVALON_PIO_SET_BITS(LCD_CONTROL_BASE, x)
#define _LCD_CLR_CONTROL_BIT(x) 	IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(LCD_CONTROL_BASE, x)
#define _LCD_SET_DATA_DIRECTION(x)	//IOWR_ALTERA_AVALON_PIO_DIRECTION(LCD_DATA_BASE, x)
#define _LCD_SEND_DATA(x) 			IOWR_ALTERA_AVALON_PIO_DATA(LCD_DATA_BASE, x)

static unsigned char  vopcode	= 35; //45
static unsigned char  Ra_Rb		= 0x27; //0x27;

//Writes a byte of data to the DDRAM data register
void write_data(unsigned char data){
	_LCD_SET_CONTROL_BIT(_LCD_A0);	//DDRAM data register
	_LCD_CLR_CONTROL_BIT(_LCD_RW);	//_LCD_RW = 0: Write
	_LCD_CLR_CONTROL_BIT(_LCD_CS);	//Clear Active LOW ChipSelect
	_LCD_SET_CONTROL_BIT(_LCD_E);	//Set Enable

	_LCD_SET_DATA_DIRECTION(_LCD_DATA_OUTPUT);
	_LCD_SEND_DATA(data);			//Put data on data bus
	//delay_us(1);	//Delay 80nS minimum


	_LCD_CLR_CONTROL_BIT(_LCD_E);	//Clear Enable
	_LCD_SET_CONTROL_BIT(_LCD_CS);	//Set Active LOW ChipSelect
	//delay_us(1);	//Delay 80nS minimum
	//Delay between consecutive commands must be a least 240nS
}
//Writes a byte to the Instruction register
void write_command(unsigned char data){
	_LCD_CLR_CONTROL_BIT(_LCD_A0);	//Instruction register
	_LCD_CLR_CONTROL_BIT(_LCD_RW);	//_LCD_RW = 0: Write
	_LCD_CLR_CONTROL_BIT(_LCD_CS);	//Clear Active LOW ChipSelect
	_LCD_SET_CONTROL_BIT(_LCD_E);	//Set Enable

	_LCD_SET_DATA_DIRECTION(_LCD_DATA_OUTPUT);
	_LCD_SEND_DATA(data);			//Put data on data bus
	//delay_us(1);	//Delay 80nS minimum


	_LCD_CLR_CONTROL_BIT(_LCD_E);	//Clear Enable
	_LCD_SET_CONTROL_BIT(_LCD_CS);	//Set Active LOW ChipSelect
	//delay_us(1);	//Delay 80nS minimum
	//Delay between consecutive commands must be a least 240nS
}
//Reset the LCD controller
void LCD_Reset(){
	_LCD_SET_CONTROL_BIT(_LCD_CS);
	_LCD_CLR_CONTROL_BIT(_LCD_RW);

	_LCD_CLR_CONTROL_BIT(_LCD_CS);
	_LCD_SET_CONTROL_BIT(_LCD_RST);
	delay_ms(2);
	_LCD_CLR_CONTROL_BIT(_LCD_RST);	//Reset lcd controller
	delay_ms(2);
	_LCD_SET_CONTROL_BIT(_LCD_RST);
	delay_ms(20);
	_LCD_SET_CONTROL_BIT(_LCD_CS);
}
//Initializes the display
void LCD_NHD_C128128_Initialize(unsigned char contrast){
	//Reset display controller
	LCD_Reset();

	//Initialize display
	write_command(0xA2);        //ICON  OFF;
	write_command(0xAE);        //Display OFF

    write_command(0x48);        //Set Duty ratio
	write_command(0x80);        //No operation
	write_command(0xa1);        //Set scan direction
	write_command(0xc8);        //SHL  select
	write_command(0x40);        //Set START LINE
	write_command(0x00);
	write_command(0xab);        //OSC on

	write_command(0x64);        //3x
	delay_ms(20);
	write_command(0x65);        //4x
	delay_ms(20);
	write_command(0x66);        //5x
	delay_ms(20);
	write_command(0x67);        //6x
	delay_ms(20);

	write_command(Ra_Rb);       //RESISTER SET
	write_command(0x81);        //Set electronic volume register
	write_command(vopcode);     //n=0~3f


	write_command(0x57);        //1/12bias
	write_command(0x92);        //FRC and pwm


	write_command(0x2C);
	delay_ms(200);//200ms
	write_command(0x2E);
	delay_ms(200);//200ms
	write_command(0x2F);
	delay_ms(200);//200ms


    write_command(0x92);        //frc and pwm
	write_command(0x38);        //external  mode
	write_command(0x75);
		//start settings for 16-level grayscale

	write_command(0x80);
	write_command(0x00);
	write_command(0x81);
	write_command(0x00);
	write_command(0x82);
	write_command(0x00);
	write_command(0x83);
	write_command(0x00);

	write_command(0x84);
	write_command(0x06);
	write_command(0x85);
	write_command(0x06);
	write_command(0x86);
	write_command(0x06);
	write_command(0x87);
	write_command(0x06);

	write_command(0x88);
	write_command(0x0b);
	write_command(0x89);
    write_command(0x0b);
    write_command(0x8a);
    write_command(0x0b);
	write_command(0x8b);
	write_command(0x0b);

	write_command(0x8c);
	write_command(0x10);
	write_command(0x8d);
	write_command(0x10);
	write_command(0x8e);
	write_command(0x10);
	write_command(0x8f);
	write_command(0x10);

	write_command(0x90);
	write_command(0x15);
	write_command(0x91);
	write_command(0x15);
	write_command(0x92);
	write_command(0x15);
	write_command(0x93);
	write_command(0x15);

	write_command(0x94);
	write_command(0x1a);
	write_command(0x95);
	write_command(0x1a);
	write_command(0x96);
	write_command(0x1a);
	write_command(0x97);
	write_command(0x1a);

	write_command(0x98);
	write_command(0x1e);
	write_command(0x99);
	write_command(0x1e);
	write_command(0x9a);
	write_command(0x1e);
	write_command(0x9b);
	write_command(0x1e);

	write_command(0x9c);
	write_command(0x23);
	write_command(0x9d);
	write_command(0x23);
	write_command(0x9e);
	write_command(0x23);
	write_command(0x9f);
	write_command(0x23);

	write_command(0xa0);
	write_command(0x27);
	write_command(0xa1);
	write_command(0x27);
	write_command(0xa2);
	write_command(0x27);
	write_command(0xa3);
	write_command(0x27);

	write_command(0xa4);
	write_command(0x2b);
	write_command(0xa5);
	write_command(0x2b);
	write_command(0xa6);
	write_command(0x2b);
	write_command(0xa7);
	write_command(0x2b);

	write_command(0xa8);
	write_command(0x2f);
	write_command(0xa9);
	write_command(0x2f);
	write_command(0xaa);
	write_command(0x2f);
	write_command(0xab);
	write_command(0x2f);

	write_command(0xac);
	write_command(0x32);
	write_command(0xad);
	write_command(0x32);
	write_command(0xae);
	write_command(0x32);
	write_command(0xaf);
	write_command(0x32);

	write_command(0xb0);
	write_command(0x35);
	write_command(0xb1);
	write_command(0x35);
	write_command(0xb2);
	write_command(0x35);
	write_command(0xb3);
	write_command(0x35);

	write_command(0xb4);
	write_command(0x38);
	write_command(0xb5);
	write_command(0x38);
	write_command(0xb6);
	write_command(0x38);
	write_command(0xb7);
	write_command(0x38);

	write_command(0xb8);
	write_command(0x3a);
	write_command(0xb9);
	write_command(0x3a);
	write_command(0xba);
	write_command(0x3a);
	write_command(0xbb);
	write_command(0x3a);

	write_command(0xbc);
	write_command(0x3c);
	write_command(0xbd);
	write_command(0x3c);
	write_command(0xbe);
	write_command(0x3c);
	write_command(0xbf);
	write_command(0x3c);
		//end settings for 16-level grayscale
	write_command(0x38);
	write_command(0x74);
	write_command(0xaf);     //Display ON
}

//Writes a char array to the lcd data DDRAM
void LCD_NHD_C128128_Update(unsigned char *scrBuffer){
#if _USE_DMA == true
	IOWR_ALTERA_AVALON_DMA_RADDRESS(LCD_DMA_BASE, scrBuffer);
	IOWR_ALTERA_AVALON_DMA_WADDRESS(LCD_DMA_BASE, 0x00);
	IOWR_ALTERA_AVALON_DMA_LENGTH(LCD_DMA_BASE, 2048);
	IOWR_ALTERA_AVALON_DMA_CONTROL(LCD_DMA_BASE, ALTERA_AVALON_DMA_CONTROL_WCON_MSK | ALTERA_AVALON_DMA_CONTROL_LEEN_MSK | ALTERA_AVALON_DMA_CONTROL_GO_MSK | ALTERA_AVALON_DMA_CONTROL_BYTE_MSK);
#else
	unsigned char page;
	unsigned char col;

	unsigned int bufferIndex = 0;
	for (page = 0xB0; page < 0xC0; page++){		//write to page 0 then go to next page . 128pixels / 8per page = 16 pages
		write_command(page);					//Set page address
		write_command(0x10);					//Set column address MSB
		write_command(0x04);					//Set column address LSB*/
		for(col = 0; col < 128; col++){			//each page has 128 pixel columns
			write_data(scrBuffer[bufferIndex]);//16 level gray scale; write each byte 4 times
			write_data(scrBuffer[bufferIndex]);
			write_data(scrBuffer[bufferIndex]);
			write_data(scrBuffer[bufferIndex]);//increment to next byte of data
			bufferIndex++;
		}
	}
#endif
}

void LCD_NHD_C128128_Test(){
	unsigned char page;
	unsigned char col;
	for (page = 0xB0; page < 0xC0; page++){		//write to page 0 then go to mext page . 128pixels / 8per page = 16 pages
		write_command(page);					//Set page address
		write_command(0x10);					//Set column address MSB
		write_command(0x00);					//Set column address LSB
		for(col = 0; col < 128; col++){			//each page has 128 pixel columns
			if(page == 0xB0){
				write_data(0xFF);			//16 level grayscale; write each byte 4 times
				write_data(0xFF);
				write_data(0xFF);
				write_data(0xFF);
			}
			else if(page == 0xB1){
				write_data(0xFF);			//16 level grayscale; write each byte 4 times
				write_data(0x00);
				write_data(0x00);
				write_data(0x00);
			}
			else{
				write_data((col & 0x10) ? 0xFF : 0x00);			//16 level grayscale; write each byte 4 times
				write_data((col & 0x10) ? 0xFF : 0x00);
				write_data((col & 0x10) ? 0xFF : 0x00);
				write_data((col & 0x10) ? 0xFF : 0x00);
			}
		}
	}
}
//Sets the intensity of the LCD Back-light (0 to 255)
void LCD_NHD_C128128_SetBackLight(char b){
	IOWR_ALTERA_AVALON_PIO_DATA(LCD_BACKLIGHT_PWM_BASE, b);
}
