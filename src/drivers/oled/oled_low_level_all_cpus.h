/*
 * oled_low_level_all_cpus.h
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
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
extern volatile uint8_t spiTransferQueueReadPos;
extern uint8_t spiTransferQueueWritePos;


struct SpiTransferQueueItem {
	uint8_t destinationId;
	uint8_t const* dataAddress;
};

extern struct SpiTransferQueueItem spiTransferQueue[SPI_TRANSFER_QUEUE_SIZE];

#endif /* DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_H_ */
