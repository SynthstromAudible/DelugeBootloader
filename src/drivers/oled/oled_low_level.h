/*
 * oled_low_level.h
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
 */

#ifndef DRIVERS_RZA1_OLED_OLED_LOW_LEVEL_H_
#define DRIVERS_RZA1_OLED_OLED_LOW_LEVEL_H_

#include "oled_low_level_all_cpus.h"

void oledSelectingComplete();
void sendOledDMA();
void oledTransferComplete(uint32_t int_sense);
void oledDeselectionComplete();
void enqueueCVMessage(int channel, uint32_t message);
void cvSPITransferComplete(uint32_t sense);
void setupSPIInterrupts();

extern int sendOledDataAfterMessage;

#endif /* DRIVERS_RZA1_OLED_OLED_LOW_LEVEL_H_ */
