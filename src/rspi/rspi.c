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
* File Name     : rspi.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : This file implements device driver for RSPI module.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/


/******************************************************************************
Includes
******************************************************************************/
/* Default  type definition header */
#include "r_typedefs.h"
/* I/O Register root header */
#include "iodefine.h"
/* INTC Driver Header */
#include "intc.h"
/* Device driver for RSPI header */
#include "rspi.h"
/* Low level register read/write header */
#include "rza_io_regrw.h"
/* Interchangeable compiler specific header */
//#include "compiler_settings.h"

/******************************************************************************
Global variables and functions
******************************************************************************/
/* RSPI1 transmit buffer address */
uint32_t * g_prspi1_tx_address;

/* RSPI1 transmit data number */
uint16_t   g_rspi1_tx_count;

/* RSPI1 receive buffer address */
uint32_t * g_prspi1_rx_address;

/* RSPI1 receive data number */
uint16_t   g_rspi1_rx_count;

/* RSPI1 receive data length */
uint16_t   g_rspi1_rx_length;

/* SCI5 transmit buffer address */
uint8_t * g_pspi_tx_address;

/* SCI5 transmit data number */
uint16_t  g_spi_tx_count;

/*******************************************************************************
* Function Name: R_RSPI1_Create
* Description  : This function initialises the RSPI1 module.
* Arguments    : None
* Return Value : None
*******************************************************************************/

#define RSPIx (*(struct st_rspi *)((uint32_t)&RSPI0 + channel * 0x800uL))


void R_RSPI_Create (uint8_t channel, uint32_t bitRate)
{
	uint16_t dummy_word  = 0u;
	uint8_t  dummy_byte  = 0u;

    UNUSED_VARIABLE(dummy_word);
    UNUSED_VARIABLE(dummy_byte);

	/* Configure and disable SPI0 transmit interrupts */
    // Did they mean SPI1? - Rohan
    /*
	R_INTC_Disable(INTC_ID_SPTI4);
	R_INTC_Regist_Int_Func(INTC_ID_SPTI4, &R_RSPI1_TransmitInterrupt);
	R_INTC_Set_Priority(INTC_ID_SPTI4, ADC_PRI_SPTI1);
*/

    RSPIx.SPPCR = 0u;
	dummy_byte = RSPI1.SPPCR;

    /* P1 clock = 66.67MHz, SPI bit rate = 11.11Mbits/s Check Table 16.3 */
	RSPIx.SPBR  = ceil((float)66666666 / (bitRate * 2) - 1);
	dummy_byte = RSPIx.SPBR;
	RSPIx.SPDCR = 0x20u;
    dummy_byte = RSPIx.SPDCR;
    RSPIx.SPSCR = 0u;
	dummy_byte = RSPIx.SPSCR;
	RSPIx.SPCKD = 0;
	dummy_byte = RSPIx.SPCKD;
	RSPIx.SSLND = 0u;
    dummy_byte =  RSPIx.SSLND;
    RSPIx.SPND  = 0u;
    dummy_byte = RSPIx.SPND;
    RSPIx.SSLP  = 0u;
    dummy_byte = RSPIx.SSLP;
    RSPIx.SPSSR  = 0u;
    dummy_byte = RSPIx.SPSSR;
    RSPIx.SPBFCR  |= 0x30u; // Receive buffer data triggering number is 1 byte
    dummy_byte = RSPIx.SPBFCR;

    /* LSB first, use 16 bit data length */
    // I haven't understood this. I'm sending single bytes and it works. - Rohan
    RSPIx.SPCMD0 = 0x0703u;
    dummy_word = RSPIx.SPCMD0;

   	/* Enable master mode */
    RSPIx.SPCR |= 0x28u;
	dummy_byte = RSPIx.SPCR;
}

/*******************************************************************************
* Function Name: R_RSPI1_Start
* Description  : This function starts the RSPI1 module operation.
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RSPI_Start (uint8_t channel)
{
    volatile uint8_t dummy = 0u;

    UNUSED_VARIABLE(dummy);

    /* Enable RSPI interrupts */
    /*
	R_INTC_Enable(INTC_ID_SPTI4);
	R_INTC_Enable(INTC_ID_SPRI4);
*/

    /* Clear error sources */
    dummy = RSPIx.SPSR;
    RSPIx.SPSR = 0x00U;

    if (0 == rza_io_reg_read_8( &(RSPIx.SPSR),
                               RSPIn_SPSR_MODF_SHIFT,
    		                   RSPIn_SPSR_MODF))
	{
		/* test */
    	RSPIx.SPCR |= 0x40U;
		dummy = RSPIx.SPCR;
	}
	else
	{
		/* clear the MODF flag then set the SPE bit */
		dummy = RSPIx.SPSR;

		RSPIx.SPCR |= 0x40U;
		dummy = RSPIx.SPCR;
	}
}



/*******************************************************************************
* Function Name: R_RSPI1_SendReceive
* Description  : This function sends and receives CSI#n1 data.
* Arguments    : tx_buf -
*                 transfer buffer pointer (not used when data is handled by DTC)
*                tx_num -
*                    buffer size
*                rx_buf -
*                 receive buffer pointer (not used when data is handled by DTC)
* Return Value : status -
*                    MD_OK or MD_ARGERROR
*******************************************************************************/
uint8_t R_RSPI1_SendReceive (uint32_t * const tx_buf,
		                              uint16_t tx_num, uint32_t * const rx_buf)
{
	uint8_t status = 0u;

    if (tx_num < 1U)
    {
        status = 1u;
    }
    else
    {
		g_prspi1_tx_address = tx_buf;
        g_rspi1_tx_count = tx_num;

        g_prspi1_rx_address = rx_buf;
        g_rspi1_rx_length = tx_num;
        g_rspi1_rx_count = 0U;

        /* Enable transmit interrupt */
    	rza_io_reg_write_8( &(RSPI1.SPCR),
    			           1,
    			           RSPIn_SPCR_SPTIE_SHIFT,
    			           RSPIn_SPCR_SPTIE);

        /* Enable receive interrupt */
    	rza_io_reg_write_8( &(RSPI1.SPCR),
    			           1,
    			           RSPIn_SPCR_SPRIE_SHIFT,
    			           RSPIn_SPCR_SPRIE);

        /* Enable RSPI function */
    	rza_io_reg_write_8( &(RSPI1.SPCR),
    			           1,
    			           RSPIn_SPCR_SPE_SHIFT,
    			           RSPIn_SPCR_SPE);
    }

    return (status);
}

uint8_t R_RSPI1_SendReceiveBasic(uint8_t channel, uint8_t data) {

	// Send data
	RSPIx.SPDR.UINT8[0] = data;

	// Wait til we receive the corresponding data
    while (0 == rza_io_reg_read_8( &(RSPIx.SPSR),
                               RSPIn_SPSR_SPRF_SHIFT,
    		                   RSPIn_SPSR_SPRF));

    // Receive data. Note that even if we didn't want the receive data, we still have to read it back, because SPI transmission halts once the RX buffer is full
	return RSPIx.SPDR.UINT8[0];
}


/*******************************************************************************
* Function Name: R_RSPI1_LoopBackReversed
* Description  : This function .
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RSPI1_LoopBackReversed (void)
{
	rza_io_reg_write_8( &(RSPI1.SPPCR),
			           (uint8_t)1,
			           RSPIn_SPPCR_SPLP_SHIFT,
			           RSPIn_SPPCR_SPLP);
}

/*******************************************************************************
* Function Name: R_RSPI1_LoopBackDisable
* Description  : This function disable loop-back mode.
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RSPI1_LoopBackDisable (void)
{
	rza_io_reg_write_8( &(RSPI1.SPPCR),
			           0,
			           RSPIn_SPPCR_SPLP_SHIFT,
			           RSPIn_SPPCR_SPLP);
}
