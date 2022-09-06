/*
 * rspi_all_cpus.h
 *
 *  Created on: 4/04/2021
 *      Author: Rohan
 */

#ifndef DRIVERS_ALL_CPUS_RSPI_ALL_CPUS_RSPI_ALL_CPUS_H_
#define DRIVERS_ALL_CPUS_RSPI_ALL_CPUS_RSPI_ALL_CPUS_H_

#include "iodefine.h"
#include "r_typedefs.h"

void R_RSPI_SendBasic8(uint8_t channel, uint8_t data);
void R_RSPI_SendBasic32(uint8_t channel, uint32_t data);
void R_RSPI_WaitEnd(uint8_t channel);
int R_RSPI_HasEnded(uint8_t channel);

#endif /* DRIVERS_ALL_CPUS_RSPI_ALL_CPUS_RSPI_ALL_CPUS_H_ */
