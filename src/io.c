/*
 * io.cpp
 *
 *  Created on: 21/09/2015
 *      Author: Rohan
 */

#include "io.h"
#include "rza_io_regrw.h"

void ioRegSet(volatile uint16_t *reg, uint8_t p, uint8_t q, uint8_t v) {
	rza_io_reg_write_16((volatile uint16_t *) ((uint32_t) reg + (p - 1) * 4), v,
			q, (uint16_t) 1 << q);
}

uint16_t ioRegGet(volatile uint16_t *reg, uint8_t p, uint8_t q) {
	return rza_io_reg_read_16(
			(volatile uint16_t *) ((uint32_t) reg + (p - 1) * 4), q,
			(uint16_t) 1 << q);
}

void setPinMux(uint8_t p, uint8_t q, uint8_t mux) {
	// Manual page 2111 explains
	ioRegSet(&GPIO.PFCAE1, p, q, mux >= 5);
	ioRegSet(&GPIO.PFCE1, p, q, ((mux - 1) >> 1) & 1);
	ioRegSet(&GPIO.PFC1, p, q, (mux - 1) & 1);
	ioRegSet(&GPIO.PMC1, p, q, 1);
	ioRegSet(&GPIO.PIPC1, p, q, 1); // Haven't bothered understanding why - the examples just do it
}

void setPinAsOutput(uint8_t p, uint8_t q) {
	ioRegSet(&GPIO.PMC1, p, q, 0);
	ioRegSet(&GPIO.PM1, p, q, 0);
	ioRegSet(&GPIO.PIPC1, p, q, 0);
}

void setPinAsInput(uint8_t p, uint8_t q) {
	ioRegSet(&GPIO.PMC1, p, q, 0);
	ioRegSet(&GPIO.PM1, p, q, 1);
	ioRegSet(&GPIO.PIBC1, p, q, 1);
}

uint16_t getOutputState(uint8_t p, uint8_t q) {
	return ioRegGet(&GPIO.P1, p, q);
}

void setOutputState(uint8_t p, uint8_t q, uint16_t state) {
	ioRegSet(&GPIO.P1, p, q, state);
}

uint16_t readInput(uint8_t p, uint8_t q) {
	return ioRegGet(&GPIO.PPR1, p, q);
}
