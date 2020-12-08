/*
 * GLCD_Fonts.h
 *
 *  Created on: 30 gen 2018
 *      Author: Manuel
 */

#include <stdint.h>

#ifndef SRC_HARDWERE_LCD_NHDC128128_GLCD_FONTS_H_
#define SRC_HARDWERE_LCD_NHDC128128_GLCD_FONTS_H_

#define _FONT_SMALL				SmallFont
#define _FONT_TINY				TinyFont
#define _FONT_MEDIUM_NUMBERS	MediumNumbers
#define _FONT_BIG_NUMBERS		BigNumbers

	extern const uint8_t SmallFont[];
	extern const uint8_t MediumNumbers[];
	extern const uint8_t BigNumbers[];
	extern const uint8_t TinyFont[];

#endif /* SRC_HARDWERE_LCD_NHDC128128_GLCD_FONTS_H_ */
