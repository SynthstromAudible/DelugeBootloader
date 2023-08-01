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

