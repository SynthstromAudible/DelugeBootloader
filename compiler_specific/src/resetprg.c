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
*******************************************************************************/
/*******************************************************************************
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name     : resetprg.c
* Device(s)     : RZ/A1H RSK+RZA1H
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Sample Program - C library entry point
*               : Variants of this file must be created for each compiler
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
******************************************************************************/

#include "r_typedefs.h"
#include "bsc_userdef.h"
#include "intc.h"
#include "stb.h"
#include "gpio.h"

#if defined(__thumb2__) || (defined(__thumb__) && defined(__ARM_ARCH_6M__))
# define THUMB_V7_V6M
#endif

/* Defined if this target supports the BLX Rm instruction.  */
#if  !defined(__ARM_ARCH_2__) && !defined(__ARM_ARCH_3__) && !defined(__ARM_ARCH_3M__)    && !defined(__ARM_ARCH_4__)    && !defined(__ARM_ARCH_4T__)
# define HAVE_CALL_INDIRECT
#endif

#ifdef HAVE_INITFINI_ARRAY
#define _init    __libc_init_array
#define _fini    __libc_fini_array
#endif

//extern int L1CacheInit(void);

extern void init_spibsc_init1_section(void);
extern void spibsc_init1(void);

extern void init_spibsc_init2_section(void);
extern void spibsc_init2(void);

/*******************************************************************************
* Function Name: PowerON_Reset
* Description  :
* Arguments    : none
* Return Value : none
*******************************************************************************/
void PowerON_Reset (void)
{
    init_spibsc_init1_section();
    spibsc_init1();
    /* The initial loader has run and the SPI interface is now at high speed */
    init_spibsc_init2_section();
    spibsc_init2();

	/* Stops program from running off */
    while(1)
    {
		__asm__("nop");
   	}
}

