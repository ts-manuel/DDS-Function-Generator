/*
 * utilityFunctions.h
 *
 *  Created on: 11 feb 2018
 *      Author: Windows7
 */

#ifndef SRC_MENU_GRAPHICS_H_
#define SRC_MENU_GRAPHICS_H_

#include <stdbool.h>
#include "../../data_structures/dataStructures.h"


void GP_Initialize();
void GP_ResetTypingScreen(s_EditableValue* editableValue, bool enable, bool show);
void GP_DrawTypingScreen();
void GP_DrawLateralButtons(u_ButtonNames* buttonsArray, int selected);
void GP_DrawWave(int offsetY, e_WaveType waveType, float dcOffset);
void GL_DrawIcon(int x, int y, e_buttonsType icon);

#endif /* SRC_MENU_GRAPHICS_H_ */
