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
* File Name     : sio_char.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Serial I/O character R/W (SCIF 2-ch process)
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "sio_char.h"
#include "typedefine.h"
#include "dev_drv.h"
#include "iodefine.h"
#include "iobitmasks/scif_iobitmask.h"
#include "iobitmasks/gpio_iobitmask.h"
#include "rza_io_regrw.h"
#include "iodefine.h"


// 1.65% lower, for SSCG
#define XTAL_SPEED_MHZ 13007402.1875//13225625





void uartInit(uint8_t scifID, uint32_t baudRate) {

	/* P1 clock=66.67MHz CKS=0 SCBRR=17 Bit rate error=0.46% => Baud rate=115200bps */
	Userdef_SCIF_UART_Init(scifID, SCIF_UART_MODE_RW, SCIF_CKS_DIVISION_1, baudRate);

	/*
    // Enable rx interrupts
    R_INTC_Disable(INTC_ID_BRI0 + scifID * 4);
    R_INTC_Disable(INTC_ID_ERI0 + scifID * 4);
    R_INTC_Disable(INTC_ID_RXI0 + scifID * 4);
    R_INTC_Set_Priority(INTC_ID_RXI0 + scifID * 4, 12uL);
    R_INTC_Enable(INTC_ID_RXI0 + scifID * 4);

    // Enable tx interrupts
    R_INTC_Disable(INTC_ID_TXI0 + scifID * 4);
	R_INTC_Set_Priority(INTC_ID_TXI0 + scifID * 4, 12uL);
	R_INTC_Enable(INTC_ID_TXI0 + scifID * 4);

	txBufferRead[scifID] = 0;
    txBufferWrite[scifID] = 0;
    rxBufferRead[scifID] = 0;
    rxBufferWrite[scifID] = 0;
*/
    /* === Enable SCIF2 transmission/reception ==== */
    /* ---- Serial control register (SCSCRi) setting ---- */
    thisScif.SCSCR = 0x0070u; // Receive interrupt only}
}



void Userdef_SCIF_UART_Init (uint8_t channel, uint8_t mode, uint16_t cks, uint32_t baudRate)
{
    volatile uint8_t dummy_buf = 0u;

    /* Used to suppress the variable declared but unused warning */
    UNUSED_VARIABLE(dummy_buf);

    /* Module standby clear */
    /* Supply clock to the SCIF(channel 2) */
    // I've deactivated this because I think all clocks are switched on in stb.c, and we don't want just channel 2 anymore

    CPG.STBCR4 = 0x00u;

    /*
    rza_io_reg_write_8((uint8_t *)&CPG.STBCR4,
    		          0,
    		          CPG_STBCR4_MSTP45_SHIFT,
    		          CPG_STBCR4_MSTP45);
*/

    /* Dummy read */
    dummy_buf = CPG.STBCR4;

    /* SCIF initial setting */
    /* Serial control register (SCSCR2) setting */
    /* SCIF transmitting and receiving operations stop */
    (*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCSCR = 0x0000u;

    /* FIFO control register (SCFCR2) setting */
    if (SCIF_UART_MODE_W == (mode & SCIF_UART_MODE_W))
    {
        /* Transmit FIFO reset */
        rza_io_reg_write_16((uint16_t *)&(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCFCR,
        		           1,
        		           SCIF2_SCFCR_TFRST_SHIFT,
        		           SCIF2_SCFCR_TFRST);
    }

    if (SCIF_UART_MODE_R == (mode & SCIF_UART_MODE_R))
    {
        /* Receive FIFO data register reset */
        rza_io_reg_write_16((uint16_t *)&(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCFCR,
        		            1,
        		            SCIF2_SCFCR_RFRST_SHIFT,
        		            SCIF2_SCFCR_RFRST);
    }

    /* Serial status register (SCFSR2) setting */
    /* ER,BRK,DR bit clear */
    (*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCFSR &= 0xFF6Eu;

    /* Line status register (SCLSR2) setting */
    /* ORER bit clear */
    rza_io_reg_write_16((uint16_t *)&(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCLSR,
    		           0,
    		           SCIF2_SCLSR_ORER_SHIFT,
    		           SCIF2_SCLSR_ORER);

    /* Serial control register (SCSCR2) setting */
    /* B'00 : Internal CLK */
    rza_io_reg_write_16((uint16_t *)&(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCSCR,
    		           0,
    		           SCIF2_SCSCR_CKE_SHIFT,
    		           SCIF2_SCSCR_CKE);

    /* Serial mode register (SCSMR2) setting
    b7    C/A  - Communication mode : Asynchronous mode
    b6    CHR  - Character length   : 8-bit data
    b5    PE   - Parity enable      : Add and check are disabled
    b3    STOP - Stop bit length    : 1 stop bit
    b1:b0 CKS  - Clock select       : cks(argument) */
    (*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCSMR = (cks & 0x0003u);

    /* Serial extension mode register (SCEMR2) setting
    b7 BGDM - Baud rate generator double-speed mode  : Double speed mode (increased by Rohan)
    b0 ABCS - Base clock select in asynchronous mode : Base clock is 16
    times the bit rate */
    (*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCEMR = 0b10000000;

    uartSetBaudRate(channel, baudRate);

   /*
	b10:b8 RSTRG - RTS output active trigger         : Initial value
	b7:b6  RTRG  - Receive FIFO data trigger         : 1-data
	b5:b4  TTRG  - Transmit FIFO data trigger        : 0-data
	b3     MCE   - Modem control enable              : Disabled
	b2     TFRST - Transmit FIFO data register reset : Disabled
	b1     RFRST - Receive FIFO data register reset  : Disabled
	b0     LOOP  - Loop-back test                    : Disabled */
	//(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCFCR = 0x0030u; // TX trigger 0; RX trigger 1
	(*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCFCR = 0x00F0u; // TX trigger 0; RX trigger 14

    /* Serial port register (SCSPTR2) setting
    b1 SPB2IO - Serial port break output : Enabled
    b0 SPB2DT - Serial port break data   : High-level */
    (*(struct st_scif *)((uint32_t)&SCIF0 + channel * 0x800uL)).SCSPTR |= 0x0003u;
}

void uartSetBaudRate(uint8_t scifID, uint32_t baudRate) {

	uint8_t scbrr = round((float)(XTAL_SPEED_MHZ * 5) / (16 * baudRate) - 1);

    /* Bit rate register (SCBRR2) setting */
    (*(struct st_scif *)((uint32_t)&SCIF0 + scifID * 0x800uL)).SCBRR = scbrr;
}

void uartPutChar(uint8_t scifID, char_t charToSend) {

	// Wait til we can...
	while (!(getOutputBufferCount(scifID) < 16));

	/* Write the receiving data in TDR */
	thisScif.SCFTDR = charToSend;

	/* Clear TDFE and TEND flag */
	thisScif.SCFSR &= (uint16_t)~0x0060u;
}





uint8_t uartGetChar(uint8_t scifID, char_t* readData) {


	/* Is there receive FIFO data? */
	if (getInputBufferCount(scifID) == 0) return 0;

	/* Read receive data */
	*readData = thisScif.SCFRDR;

	/* Clear RDF */
	//rza_io_reg_write_16(&thisScif.SCFSR, 0, SCIF2_SCFSR_RDF_SHIFT, SCIF2_SCFSR_RDF);
	return 1;
}



void uartPrintln(char const* output) {
	return;
	uartPrint(output);
	uartPutChar(0, 10);
}

void uartPrint(char const* output) {
	return;
	while (*output != 0) {
		uartPutChar(0, *output);
		output++;
	}
}


char stringBufferUSB[100];

static char* intToStringUSB(int32_t number) {
	uint8_t minNumDigits = 1;
	if (number == -2147483648) { // Special case that code below doesn't deal with
		strcpy(stringBufferUSB, "-2147483648");
		return stringBufferUSB;
	}
    uint8_t numDigits = 0;
    uint8_t isNegative = (number < 0);
    number = abs(number);
    stringBufferUSB[11] = 0;
    uint8_t charPos = 11;
    while(number != 0 || numDigits < minNumDigits) {
    	stringBufferUSB[--charPos] = 48 + (number % 10);
        number /= 10;
        numDigits++;
    }
    if (isNegative) stringBufferUSB[--charPos] = 45;
    return stringBufferUSB + charPos;
}

void io_put_number(int32_t number) {
	//return;
	char* output = intToStringUSB(number);
	while (*output != 0) {
		uartPutChar(0, *output);
		output++;
	}
	uartPutChar(0, 10);
}



uint16_t getInputBufferCount(uint8_t scifID) {
	return thisScif.SCFDR & 31;
}




uint16_t getOutputBufferCount(uint8_t scifID) {
	return (thisScif.SCFDR >> 8) & 31;
}

