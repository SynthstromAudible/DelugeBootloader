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
* File Name     : init_spibsc_init2_section.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Initialise section 2 load module 3, high speed SPI transfer.
* Note          : Use debug version to debug this code and release version to
*                 to down load it into QSPI.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_typedefs.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint32_t __code_spibsc_init_2_start__;
extern uint32_t __code_spibsc_init_2_end__;
//extern uint32_t *LOAD_MODULE3;
#define LOAD_MODULE3 0x18000800

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: init_spibsc_init2_section
* Description  : The initialisation copy function of the section.
* Arguments    : none
* Return Value : none
******************************************************************************/
void init_spibsc_init2_section (void)
{
	/* In DEBUG mode, the code is already loaded into RAM, so this function */
	/* should not be called, otherwise it will over write the code.         */

#ifndef DEBUG
	/* Variables for program copy of code section 3 */
    uint32_t i,loop_num;
    uint32_t *ram_start,*ram_end;
    uint32_t region_size;
    uint32_t *src_start;

    /* Initialisation of LOAD_MODULE3 section */
    /* Global variables are specified in the load file (.ld) */
    ram_start       = &__code_spibsc_init_2_start__;
    ram_end       = &__code_spibsc_init_2_end__;

    /* Calculate the length of the copied section */
    /* Note: The size is in long words */
    region_size     = (uint32_t)(ram_end - ram_start);

    /* Calculate the number of copies for the size of the load section */
    loop_num = region_size + 1; /* add one extra to be on the safe side */


    /* Point src_start to start of code in ROM */
    src_start = (uint32_t *)LOAD_MODULE3;
    
    /* The DMAC controller could be configured here, but little point as
     * we cannot continue until the copy completes. This is an option for
     * future development if other initialisation tasks are required in
     * the customers system.*/

    /* Copy the next load module. */
    for(i = 0; i < loop_num; i++)
    {
        *ram_start++ = *src_start++;
    }
#endif
}

/* End of File */
