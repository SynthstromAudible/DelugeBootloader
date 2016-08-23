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
* File Name     : spibsc_init1.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Flash boot setting
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "spibsc.h"
#include "rza_io_regrw.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
void spibsc_init1 (void);

/******************************************************************************
* Function Name: spibsc_init1
* Description  : This is the first code that runs in RAM following the load.
*                It stops the QPSI interface, reconfigures it to high speed
*                and then restarts the access for the copy of the main program
*                loader.
* Arguments    : none
* Return Value : none
******************************************************************************/
void spibsc_init1(void)
{
    /* Temporary (unused) variable for synchronisation read. */
    volatile uint16_t temp = 0;

    /* Used to suppress the 'variable declared but not used' warning */
    UNUSED_VARIABLE(temp);

    /* Release pin function for memory control without changing pin state. */
    if(rza_io_reg_read_16((uint16_t *)&CPG.DSFR, CPG_DSFR_IOKEEP_SHIFT,  CPG_DSFR_IOKEEP) == 1)
    {
        rza_io_reg_write_16((uint16_t *)&CPG.DSFR, 0, CPG_DSFR_IOKEEP_SHIFT,  CPG_DSFR_IOKEEP);

        /* Perform a read after the write for synchronisation. */
        temp = rza_io_reg_read_16((uint16_t *)&CPG.DSFR, CPG_DSFR_IOKEEP_SHIFT,  CPG_DSFR_IOKEEP);
    }

    /* SPIBSC stop accessing the SPI in bus mode */
    rza_io_reg_write_32(&SPIBSC0.DRCR, 1, SPIBSC_DRCR_SSLN_SHIFT, SPIBSC_DRCR_SSLN);

    /* Wait for the setting to be accepted. */
    while(1)
    {
        if(rza_io_reg_read_32(&SPIBSC0.CMNSR, SPIBSC_CMNSR_SSLF_SHIFT, SPIBSC_CMNSR_SSLF) == SPIBSC_SSL_NEGATE)
        {
            /* This is intentional */
            break;
        }
    }

    /* Wait for the final transfers to end (TEND=1) for setting change in SPIBSC. */
    while(rza_io_reg_read_32(&SPIBSC0.CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        /* wait for transfer-end */
    }

    /* The next access delayed setting : 1SPBCLK   */
    /* SPBSSL negate delayed setting   : 1.5SPBCLK */
    /* Clock delayed setting           : 1SPBCLK   */
    SPIBSC0.SSLDR = 0;

    /* bitrate setting register(SPBCR)  33.33[MHz] */
    rza_io_reg_write_32(&SPIBSC0.SPBCR, 0, SPIBSC_SPBCR_BRDV_SHIFT, SPIBSC_SPBCR_BRDV);
    rza_io_reg_write_32(&SPIBSC0.SPBCR, 2, SPIBSC_SPBCR_SPBR_SHIFT, SPIBSC_SPBCR_SPBR);

    /* Data read control register(DRCR) set to enable the Read Cache */
    rza_io_reg_write_32(&SPIBSC0.DRCR, 1, SPIBSC_DRCR_RBE_SHIFT, SPIBSC_DRCR_RBE);
}


/* End of File */
