/*
 * oled.h
 *
 *  Created on: 4/09/2022
 *      Author: Rohan
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
