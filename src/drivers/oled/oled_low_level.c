/*
 * oled_low_level.c
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
 */

#include "oled_low_level.h"
//#include "definitions.h"
//#include "sio_char.h"
//#include "rspi_all_cpus.h"
//#include "cfunctions.h"
#include "oled_low_level_all_cpus.h"
//#include "dmac.h"
#include "rspi_iodefine.h"
#include "dmac_iodefine.h"
#include "dmac_iobitmask.h"
#include "intc.h"

#define OLED_CODE_FOR_CV 1

extern void v7_dma_flush_range(start, end);

void setupSPIInterrupts() {

	R_INTC_Disable(INTC_ID_SPRI0 + SPI_CHANNEL_CV * 3);
	R_INTC_RegistIntFunc(INTC_ID_SPRI0 + SPI_CHANNEL_CV * 3, &cvSPITransferComplete);
	R_INTC_SetPriority(INTC_ID_SPRI0 + SPI_CHANNEL_CV * 3, 5);
	R_INTC_Enable(INTC_ID_SPRI0 + SPI_CHANNEL_CV * 3);
}

void enqueueCVMessage(int channel, uint32_t message) {
	enqueueSPITransfer(OLED_CODE_FOR_CV, (uint8_t const*)message);
}

int sendOledDataAfterMessage = 256;

void oledSelectingComplete() {
	sendOledDataAfterMessage = 256;
	RSPI(SPI_CHANNEL_OLED_MAIN).SPDCR = 0x20u; // 8-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPCMD0 = 0b0000011100000010; // 8-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPBFCR.BYTE = 0b01100000;//0b00100000;
	//DMACn(OLED_SPI_DMA_CHANNEL).CHCFG_n = 0b00000000001000000000001001101000 | (OLED_SPI_DMA_CHANNEL & 7);

	int transferSize = (OLED_MAIN_HEIGHT_PIXELS >> 3) * OLED_MAIN_WIDTH_PIXELS;
	DMACn(OLED_SPI_DMA_CHANNEL).N0TB_n = transferSize; // TODO: only do this once?
	uint32_t dataAddress = (uint32_t)spiTransferQueue[spiTransferQueueReadPos].dataAddress;
	DMACn(OLED_SPI_DMA_CHANNEL).N0SA_n = dataAddress;
	spiTransferQueueReadPos = (spiTransferQueueReadPos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);
	//v7_dma_flush_range(dataAddress, dataAddress + transferSize);
	DMACn(OLED_SPI_DMA_CHANNEL).CHCTRL_n |= DMAC_CHCTRL_0S_CLRTC | DMAC_CHCTRL_0S_SETEN; // ---- Enable DMA Transfer and clear TC bit ----
}

void oledSelectingComplete2() {
	sendOledDataAfterMessage = 256;
	RSPI(SPI_CHANNEL_OLED_MAIN).SPDCR = 0x20u; // 8-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPCMD0 = 0b0000011100000010; // 8-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPBFCR.BYTE = 0b01100000;//0b00100000;
	//DMACn(OLED_SPI_DMA_CHANNEL).CHCFG_n = 0b00000000001000000000001001101000 | (OLED_SPI_DMA_CHANNEL & 7);

	int transferSize = (OLED_MAIN_HEIGHT_PIXELS >> 3) * OLED_MAIN_WIDTH_PIXELS;
	DMACn(OLED_SPI_DMA_CHANNEL).N0TB_n = transferSize; // TODO: only do this once?
	uint32_t dataAddress = (uint32_t)spiTransferQueue[spiTransferQueueReadPos].dataAddress;
	DMACn(OLED_SPI_DMA_CHANNEL).N0SA_n = dataAddress;
	spiTransferQueueReadPos = (spiTransferQueueReadPos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);
	//v7_dma_flush_range(dataAddress, dataAddress + transferSize);
	//DMACn(OLED_SPI_DMA_CHANNEL).CHCTRL_n |= DMAC_CHCTRL_0S_CLRTC | DMAC_CHCTRL_0S_SETEN; // ---- Enable DMA Transfer and clear TC bit ----
}

int spiDestinationSendingTo;

void sendCVTransfer() {
	setOutputState(6, 1, false); // Select CV DAC

	RSPI(SPI_CHANNEL_OLED_MAIN).SPDCR = 0x60u; // 32-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPCMD0 = 0b0000001100000010; // 32-bit
	RSPI(SPI_CHANNEL_OLED_MAIN).SPBFCR.BYTE = 0b00110010;//0b11110010;//0b01100010;//0b00100010; // Now we do *not* reset the RX buffer.


	RSPI(SPI_CHANNEL_OLED_MAIN).SPCR |= 1 << 7; // Receive interrupt enable

	uint32_t message = (uint32_t)spiTransferQueue[spiTransferQueueReadPos].dataAddress;

	spiTransferQueueReadPos = (spiTransferQueueReadPos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1); // Must do this before actually sending SPI, cos the interrupt could come before we do this otherwise. True story.

	RSPI(SPI_CHANNEL_CV).SPDR.LONG = message;
}

void initiateSelectingOled() {
	bufferPICUart(248); // Select OLED

	//delayMS(10);
	//oledSelectingComplete();
	//return;

	//uartFlushIfNotSending(UART_ITEM_PIC);
	sendOledDataAfterMessage = 248;

	// Actual queue position gets moved along in oledSelectingComplete() when that gets called.
}

void sendSPITransferFromQueue() {

	spiTransferQueueCurrentlySending = true;

	spiDestinationSendingTo = spiTransferQueue[spiTransferQueueReadPos].destinationId;

	// If it's OLED data...
	if (spiDestinationSendingTo == 0) {
		initiateSelectingOled();
	}

	// Or if it's CV...
	else {
		//sendCVTransfer();
	}
}



void oledTransferComplete(uint32_t int_sense) {

	// If anything else to send, and it's to the OLED again, then just go ahead.
	if (spiTransferQueueWritePos != spiTransferQueueReadPos && spiTransferQueue[spiTransferQueueReadPos].destinationId == 0) {
		oledSelectingComplete();
	}

	// Otherwise, deselect OLED. And when that's finished, we might send some more if there is more.
	else {
		bufferPICUart(249); // Unselect OLED

		//delayMS(10);
		//oledDeselectionComplete();
		//return;

		sendOledDataAfterMessage = 249;
		//uartFlushIfNotSending(UART_ITEM_PIC);
	}
}

void cvSPITransferComplete(uint32_t sense) {

	RSPI(SPI_CHANNEL_OLED_MAIN).SPCR &= ~(1 << 7); // Receive interrupt disable

	setOutputState(6, 1, true); // Deselect CV DAC. We do it here, nice and early, since we might be re-selecting it very soon in sendCVTransfer(),
								// and a real pulse does need to be sent. This should be safe - I even tried only deselecting it in the instruction
								// preceding the re-selection, and it did work.

	RSPI(SPI_CHANNEL_OLED_MAIN).SPBFCR.BYTE |= 1 << 6; // Reset RX buffer. Slightly weird that we have to do it here, after the transfer? Or not weird?

	// If anything else to send, and it's to the CV DAC again, then just go ahead.
	if (spiTransferQueueWritePos != spiTransferQueueReadPos && spiTransferQueue[spiTransferQueueReadPos].destinationId == OLED_CODE_FOR_CV) {
		sendCVTransfer();
	}

	// Otherwise...
	else {
		// And, if there's some OLED data waiting to send, do that.
		if (spiTransferQueueWritePos != spiTransferQueueReadPos) {
			spiDestinationSendingTo = 0;
			initiateSelectingOled();
		}

		// Otherwise, we're all done.
		else {
			spiTransferQueueCurrentlySending = false;
		}
	}
}

void oledDeselectionComplete() {
	sendOledDataAfterMessage = 256;
	spiTransferQueueCurrentlySending = false;

	// There might be something more to send now...
	if (spiTransferQueueWritePos != spiTransferQueueReadPos) {
		//sendSPITransferFromQueue();
	}
}
