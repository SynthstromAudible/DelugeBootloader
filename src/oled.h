/*
 * Copyright © 2022-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OLED_H_
#define OLED_H_

#include "oled_low_level.h"

extern uint8_t oledMainImage[OLED_MAIN_HEIGHT_PIXELS >> 3][OLED_MAIN_WIDTH_PIXELS];
extern int currentLogoPixel;


void displayPrompt(char const* message);
void monitorInputFromPICForever();
void monitorInputFromPIC();
void uartInputReceivedPossiblyOledRelated(int value);

void delayMSWhileMonitoringPIC(uint32_t ms);
uint32_t msToSlowTimerCount(uint32_t ms);

#define TEXT_SPACING_X 6
#define TEXT_SPACING_Y 9
#define TEXT_SIZE_Y_UPDATED 7

#define TEXT_TITLE_SPACING_X 9
#define TEXT_TITLE_SIZE_Y 10

#define TEXT_BIG_SPACING_X 11
#define TEXT_BIG_SIZE_Y 13

#define TEXT_HUGE_SPACING_X 18
#define TEXT_HUGE_SIZE_Y 20

#endif /* OLED_H_ */
