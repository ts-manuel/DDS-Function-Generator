/*
 * menu.h
 *
 *  Created on: 07 dic 2017
 *      Author: Manuel
 */

#ifndef SRC_MENU_MENU_H_
#define SRC_MENU_MENU_H_

#include <stdint.h>
#include <stdbool.h>
#include "../data_structures/dataStructures.h"

//Available menu pages
extern const s_MenuPage menu_main;
extern const s_MenuPage menu_am_modulation;
extern const s_MenuPage menu_fm_modulation;
extern const s_MenuPage menu_graphicMenu;
extern const s_MenuPage menu_settings;
extern const s_MenuPage menu_phaseOffset;
extern const s_MenuPage menu_sweep;
extern const s_MenuPage menu_sd;

//Functions
void menu_init();
void menu_draw();
void menu_PushPage(const s_MenuPage* newMenuPage);
void menu_PopPage();
void menu_AddToMenuElement(s_MenuElement* menuElement, float value);
void menu_ProcessKeypadPress(uint8_t KeyPressed);
void menu_ProcessEncoder(uint8_t btnEncoder, int8_t encoder);

#endif /* SRC_MENU_MENU_H_ */
