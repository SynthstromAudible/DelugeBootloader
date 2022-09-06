/*
 * mtu.c
 *
 *  Created on: 9/05/2020
 *      Author: Rohan
 */
#include "mtu.h"

void mtuEnableAccess() {
	MTU2.TRWER = 1u;
}
