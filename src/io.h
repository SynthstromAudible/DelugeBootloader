/*
 * io.h
 *
 *  Created on: 21/09/2015
 *      Author: Rohan
 */

#ifndef IO_H_
#define IO_H_

#include "iodefine.h"
#include "rza_io_regrw.h"


void ioRegSet(volatile uint16_t *reg, uint8_t p, uint8_t q, uint8_t v);
uint16_t ioRegGet(volatile uint16_t *reg, uint8_t p, uint8_t q);
void setPinMux(uint8_t p, uint8_t q, uint8_t mux);
void setPinAsOutput(uint8_t p, uint8_t q);
void setPinAsInput(uint8_t p, uint8_t q);
uint16_t getOutputState(uint8_t p, uint8_t q);
void setOutputState(uint8_t p, uint8_t q, uint16_t state);
uint16_t readInput(uint8_t p, uint8_t q);



#endif /* IO_H_ */
