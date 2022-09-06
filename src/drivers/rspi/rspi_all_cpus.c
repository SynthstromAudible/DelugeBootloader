/*
 * rspi_all_cpus.c
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
 */

#include "rspi_all_cpus.h"
#include "rspi_iodefine.h"
#include "rspi_iobitmask.h"
#include "rza_io_regrw.h"

void R_RSPI_SendBasic8(uint8_t channel, uint8_t data) {

	// If the TX buffer doesn't have 4 bytes of empty space in it, we'd better wait
    while (0 == rza_io_reg_read_8( &(RSPI(channel).SPSR),
    							RSPI_SPSR_SPTEF_SHIFT,
								RSPI_SPSR_SPTEF));

	// Clear the RX buffer - we get problems if it gets full
	RSPI(channel).SPBFCR.BYTE |= 0b01000000;

	// Send data
	RSPI(channel).SPDR.BYTE.LL = data;
}


void R_RSPI_SendBasic32(uint8_t channel, uint32_t data) {

	// If the TX buffer doesn't have 4 bytes of empty space in it, we'd better wait
    while (0 == rza_io_reg_read_8( &(RSPI(channel).SPSR.BYTE),
                               RSPI_SPSR_SPTEF_SHIFT,
    		                   RSPI_SPSR_SPTEF));

	// Clear the RX buffer - we get problems if it gets full
    RSPI(channel).SPBFCR.BYTE |= 0b01000000;

	// Send data
	RSPI(channel).SPDR.LONG = data;
}

void R_RSPI_WaitEnd(uint8_t channel) {
	while (!RSPI(channel).SPSR.BIT.TEND) {}
}

int R_RSPI_HasEnded(uint8_t channel) {
	return RSPI(channel).SPSR.BIT.TEND;
}
