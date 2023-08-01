/*
 * Copyright © 2021-2023 Synthstrom Audible Limited
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

#ifndef DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_H_
#define DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_H_

#include "r_typedefs.h"

#define DMA_INTERRUPT_0 INTC_ID_DMAINT0


#define UART_CHANNEL_MIDI 0
#define UART_CHANNEL_PIC 1

#define SPI_TRANSFER_QUEUE_SIZE 32

#define SPI_CHANNEL_CV 0
#define SPI_CHANNEL_OLED_MAIN 0

#define OLED_SPI_DMA_CHANNEL 4


#define OLED_MAIN_HEIGHT_PIXELS 48
#define OLED_MAIN_TOPMOST_PIXEL 5
#define OLED_HEIGHT_CHARS 4

#define OLED_MAIN_VISIBLE_HEIGHT (OLED_MAIN_HEIGHT_PIXELS - OLED_MAIN_TOPMOST_PIXEL)

#define HAVE_OLED 1

#define OLED_MAIN_WIDTH_PIXELS 128

#define OLED_WIDTH_CHARS 16
#define OLED_MENU_NUM_OPTIONS_VISIBLE (OLED_HEIGHT_CHARS - 1)


#define CONSOLE_IMAGE_HEIGHT (OLED_MAIN_HEIGHT_PIXELS + 16)
#define CONSOLE_IMAGE_NUM_ROWS (CONSOLE_IMAGE_HEIGHT >> 3)

#define OLED_ALLOW_LOWER_CASE 0



void oledMainInit();
void oledDMAInit();
void enqueueSPITransfer(int whichOled, uint8_t const* image);
void oledTransferComplete(uint32_t int_sense);

void bufferPICUart(char data);

extern volatile bool_t spiTransferQueueCurrentlySending;
extern volatile int spiTransferQueueReadPos;
extern int spiTransferQueueWritePos;


struct SpiTransferQueueItem {
	uint8_t destinationId;
	uint8_t const* dataAddress;
};

extern struct SpiTransferQueueItem spiTransferQueue[SPI_TRANSFER_QUEUE_SIZE];

#endif /* DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_H_ */
