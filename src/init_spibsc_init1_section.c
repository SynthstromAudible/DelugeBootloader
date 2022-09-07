/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : init_spibsc_init1_section.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Initialise section 1 load module 2, low speed SPI transfer.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "r_spibsc_ioset_api.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint32_t __code_spibsc_init_1_end__;
extern uint32_t __code_spibsc_init_1_start__;
extern uint32_t __code_spibsc_init_1_src__;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static inline void rza_io_reg_write_16 (volatile uint16_t * ioreg,
		                          uint16_t write_value,
		                          uint16_t shift,
		                          uint16_t mask)
{
    uint16_t reg_value;

    /* Read from register */
    reg_value = (*ioreg);

    /* Modify value       */
    reg_value = (uint16_t)((reg_value & (~mask)) | (write_value << shift));

    /* Write to register  */
    (*ioreg)    = reg_value;
}

static inline void ioRegSet(volatile uint16_t *reg, uint8_t p, uint8_t q, uint8_t v) {
	rza_io_reg_write_16((volatile uint16_t *) ((uint32_t) reg + (p - 1) * 4), v,
			q, (uint16_t) 1 << q);
}

static inline void setPinAsOutput(uint8_t p, uint8_t q) {
	ioRegSet(&GPIO.PMC1, p, q, 0);
	ioRegSet(&GPIO.PM1, p, q, 0);
	ioRegSet(&GPIO.PIPC1, p, q, 0);
}

static inline void setOutputState(uint8_t p, uint8_t q, uint16_t state) {
	ioRegSet(&GPIO.P1, p, q, state);
}
/******************************************************************************
* Function Name: init_spibsc_init1_section
* Description  : The initialisation copy function of the section.
* Arguments    : none
* Return Value : none
******************************************************************************/
void init_spibsc_init1_section (void)
{
	/* In DEBUG mode, the code is already loaded into RAM, so this function */
	/* should not be called, otherwise it will over write the code.         */


	//setOutputState(6, 7, true);
	//setPinAsOutput(6, 7);


#ifndef DEBUG
	/* Variables for program copy of code section 2 */
    uint32_t i;
    uint32_t loop_num;
    uint32_t * pram_start;
    uint32_t * pram_end;
    uint32_t   region_size;
    uint32_t * psrc_start;

    /* Initialisation of LOAD_MODULE2 section */
    /* Global variables are specified in the load file (.ld) */
    pram_start     = &__code_spibsc_init_1_start__;
    pram_end       = &__code_spibsc_init_1_end__;

    /* Calculate the length of the copied section */
    /* Note: The size is in long words */
    region_size     = (uint32_t)(pram_end - pram_start);

    /* Calculate the number of copies for the size of the load section */
    /* add one extra to be on the safe side */
    loop_num = (region_size);// + 1L);


    /* Point src_start to start of code in ROM */
    psrc_start = &__code_spibsc_init_1_src__;
    
    /* The DMAC controller could be configured here, but little point as
     * we cannot continue until the copy completes. This is an option for
     * future development if other initialisation tasks are required in
     * the customers system.*/

    /* Copy the next load module. */
    for(i = 0; i < loop_num; i++)
    {
        (*pram_start++) = (*psrc_start++);
    }
#endif
}

/* End of File */
