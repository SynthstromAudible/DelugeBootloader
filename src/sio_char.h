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
******************************************************************************/
/******************************************************************************
* File Name     : sio_char.h
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Sample Program - Terminal I/O header file
******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/* Multiple inclusion prevention macro */
#ifndef SIO_CHAR_H
#define SIO_CHAR_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "intc.h"





#define SCIF_UART_MODE_W        (1)
#define SCIF_UART_MODE_R        (2)
#define SCIF_UART_MODE_RW       (3)

typedef enum scif_cks_division
{
    SCIF_CKS_DIVISION_1,
    SCIF_CKS_DIVISION_4,
    SCIF_CKS_DIVISION_16,
    SCIF_CKS_DIVISION_64
} scif_cks_division_t;

/******************************************************************************
Functions Prototypes
******************************************************************************/
int32_t SioWrite(int32_t file_no, const char_t * buffer, uint32_t writing_b);
int32_t SioRead(int32_t file_no, char_t * buffer, uint32_t reading_b);

void   IoInitScif2(void);
char_t IoGetchar(uint8_t scifID);
void   IoPutchar(uint8_t scifID, char_t buffer);

uint16_t getOutputBufferCount(uint8_t scifID);

void io_put_char_buffered(uint8_t scifID, char_t charToSend);
void initUartInterrupts();
void io_init_scif (uint8_t scifID);
uint16_t getInputBufferCount(uint8_t scifID);
void uartInit(uint8_t scifID, uint32_t baudRate);
void uartPutChar(uint8_t scifID, char_t charToSend);
uint8_t uartGetChar(uint8_t scifID, char_t* readData);
void uartPrintln(char const* output);
void uartPrint(char const* output);
void Userdef_SCIF_UART_Init (uint8_t channel, uint8_t mode, uint16_t cks, uint32_t baudRate);
void uartSetBaudRate(uint8_t scifID, uint32_t baudRate);




#define TX_BUFFER_SIZE 512
extern char_t txBuffer[3][TX_BUFFER_SIZE];
extern uint16_t txBufferWrite[3];
extern volatile uint16_t txBufferRead[3];


#define RX_BUFFER_SIZE 512
extern char_t rxBuffer[3][RX_BUFFER_SIZE];
extern uint16_t rxBufferWrite[3];
extern uint16_t rxBufferRead[3];


#define thisScif (*(struct st_scif *)((uint32_t)&SCIF0 + scifID * 0x800uL))

/* SIO_CHAR_H */
#endif  

/* End of File */
