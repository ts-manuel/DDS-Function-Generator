/*
 * lcd_NHD_C128128.h
 *
 *  Created on: 28 gen 2018
 *      Author: Manuel
 */

#include "GLCD_Fonts.h"

#ifndef SRC_HARDWERE_LCD_NHD_C128128_H_
#define SRC_HARDWERE_LCD_NHD_C128128_H_

	void LCD_NHD_C128128_Initialize(unsigned char contrast);
	void LCD_NHD_C128128_SetBackLight(char b);
	void LCD_NHD_C128128_Update(unsigned char *scrBuffer);
	void LCD_NHD_C128128_Test();

#endif /* SRC_HARDWERE_LCD_NHD_C128128_H_ */
