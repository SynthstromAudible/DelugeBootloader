/*
 * oled_low_level_all_cpus.c
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
 */

//#include "definitions.h"

#include "oled_low_level_all_cpus.h"
//#include "sio_char.h"
//#include "rspi_all_cpus.h"
//#include "dmac.h"
#include "dmac_iodefine.h"
#include "intc.h"
#include "rspi_iodefine.h"
#include "dmac_iobitmask.h"


void bufferPICUart(char data) {
	uartPutChar(UART_CHANNEL_PIC, data);
}

void MainOLED_WCom(char data) {
	R_RSPI_SendBasic8(SPI_CHANNEL_OLED_MAIN, data);
}


struct SpiTransferQueueItem spiTransferQueue[SPI_TRANSFER_QUEUE_SIZE];
volatile bool_t spiTransferQueueCurrentlySending = false;
volatile uint8_t spiTransferQueueReadPos = 0;
uint8_t spiTransferQueueWritePos = 0;

void oledMainInit() {

	spiTransferQueueReadPos = 0;
	spiTransferQueueWritePos = 0;
	spiTransferQueueCurrentlySending = false;

	 MainOLED_WCom(0xFD); // SET COMMAND LOCK
	 MainOLED_WCom(0x12);
	 MainOLED_WCom(0xAE); //DOT MARTIX DISPLAY OFF

	 MainOLED_WCom(0x81); //CONTARST CONTROL(00H-0FFH)
	 MainOLED_WCom(0xFF);

	 MainOLED_WCom(0xA4); //ENTIRE DISPLAY OFF(0A4H-0A5H)

	 MainOLED_WCom(0xA6); //SET NORMAL DISPLAY(0A6H-0A7H)

	 MainOLED_WCom(0x00); // SET LOW COLUMN START ADDRESS
	 MainOLED_WCom(0x10); //SET HIGH COLUMN START ADDRESS

	 MainOLED_WCom(0x20); //SET MEMORRY ADDRESSING MODE
	 MainOLED_WCom(0x00); // Horizontal

/*
	 MainOLED_WCom(0xB0); // Set Page Start
*/


#if OLED_MAIN_HEIGHT_PIXELS != 64
	 MainOLED_WCom(0x22); // Set page address start / end
	 MainOLED_WCom((64 - OLED_MAIN_HEIGHT_PIXELS) >> 3);
	 MainOLED_WCom(7);
#endif

	 MainOLED_WCom(0x40); //SET DISPLAY START LINE (040H-07FH) // Moves entire graphics vertically

	 MainOLED_WCom(0xA0); //SET SEGMENT RE-MAP(0A0H-0A1H) // Flips stuff 180deg!

	 MainOLED_WCom(0xA8); //SET MULTIPLEX RATIO 64
	 MainOLED_WCom(0x3F);

	 MainOLED_WCom(0xC0); //COM SCAN COM1-COM64(0C8H,0C0H)

	 MainOLED_WCom(0xD3); //SET DISPLAY OFFSET(OOH-3FH)
	 MainOLED_WCom(0x0);

	 MainOLED_WCom(0xDA); //COM PIN CONFIGURATION
	 MainOLED_WCom(0x12);

	 MainOLED_WCom(0xD5); //SET FRAME FREQUENCY
	 MainOLED_WCom(0xF0);

	 MainOLED_WCom(0xD9); //SET PRE_CHARGE PERIOD
	 MainOLED_WCom(0xA2);

	 MainOLED_WCom(0xDB); //SET VCOM DESELECT LEVEL
	 MainOLED_WCom(0x34);

	 MainOLED_WCom(0xAF); //DSPLAY ON

	 R_RSPI_WaitEnd(SPI_CHANNEL_OLED_MAIN);

#ifdef OLED_MAIN_DC_PIN
	 R_GPIO_PinWrite(OLED_MAIN_DC_PIN, GPIO_LEVEL_HIGH);
#else
	 bufferPICUart(251); // D/C high
	 //uartFlushIfNotSending(UART_ITEM_PIC);
#endif

	 /*
	R_INTC_RegistIntFunc(INTC_ID_RSPI_SPTI0 + SPI_CHANNEL_OLED_MAIN * 3, oledTransferComplete);
	R_INTC_SetPriority(INTC_ID_RSPI_SPTI0 + SPI_CHANNEL_OLED_MAIN * 3, 13); // Not very important
	R_INTC_Enable(INTC_ID_RSPI_SPTI0 + SPI_CHANNEL_OLED_MAIN * 3);
*/
}




void enqueueSPITransfer(int destinationId, uint8_t const* image) {

	// First check there isn't already an identical transfer enqueued.
	int readPosNow = spiTransferQueueReadPos;
	while (readPosNow != spiTransferQueueWritePos) {
		if (spiTransferQueue[readPosNow].destinationId == destinationId && spiTransferQueue[readPosNow].dataAddress == image) return;
		readPosNow = (readPosNow + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);
	}

	spiTransferQueue[spiTransferQueueWritePos].destinationId = destinationId;
	spiTransferQueue[spiTransferQueueWritePos].dataAddress = image;
	spiTransferQueueWritePos = (spiTransferQueueWritePos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);

	// If DMA not currently sending, and our new entry is still in the queue (it didn't get sent inside an interrupt just now),
	// then send it now.
	if (!spiTransferQueueCurrentlySending && spiTransferQueueWritePos != spiTransferQueueReadPos) {
		sendSPITransferFromQueue();
	}
}

#define DMARS_FOR_RSPI_TX	0b100100001

void setDMARS(int dmaChannel, uint32_t dmarsValue) {

	uint32_t mask = 0xFFFF0000;

	if (dmaChannel & 1) {
		dmarsValue <<= 16;
		mask >>= 16;
	}

	volatile uint32_t* dmarsAddress = DMARSnAddress(dmaChannel);

	*dmarsAddress = (*dmarsAddress & mask) | dmarsValue;
}

void oledDMAInit() {

	// ---- DMA Control Register Setting ----
	DCTRLn(OLED_SPI_DMA_CHANNEL) = 0;

	// ----- Transmission Side Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHCFG_n = 0b00000000001000000000001001101000 | (OLED_SPI_DMA_CHANNEL & 7);

	// ---- DMA Channel Interval Register Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHITVL_n = 0;

	// ---- DMA Channel Extension Register Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHEXT_n = 0;

	DMACn(OLED_SPI_DMA_CHANNEL).N0DA_n = (uint32_t)&RSPI(SPI_CHANNEL_OLED_MAIN).SPDR.BYTE.LL;

	// ---- Software Reset and clear TC bit ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHCTRL_n |= DMAC_CHCTRL_0S_SWRST | DMAC_CHCTRL_0S_CLRTC;

#if DELUGE_MODEL <= DELUGE_MODEL_CLASSIC
	DMACn(OLED_SPI_DMA_CHANNEL).N0DA_n = (uint32_t)&RSPI(SPI_CHANNEL_OLED_MAIN).SPDR.BYTE.LL;

	unsigned int dmarsTX = DMARS_FOR_RSPI_TX + (SPI_CHANNEL_OLED_MAIN << 2);
	setDMARS(OLED_SPI_DMA_CHANNEL, dmarsTX);
#endif

	R_INTC_RegistIntFunc(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL, oledTransferComplete);
	R_INTC_SetPriority(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL, 13); // Priority is not very important
	R_INTC_Enable(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL);
}
