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
* File Name     : spibsc_init2.c
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
#include "r_spibsc_ioset_api.h"
#include "rza_io_regrw.h"
#include "io.h"
#include "rspi/rspi.h"
#include "fatfs/ff.h"
#include "sio_char.h"


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
#define SPIBSC_CH                   (0)
#define DEF_USER_PROGRAM_SRC        (0x18080000)
#define DEF_USER_PROGRAM_TOP        (0x20040000)
#define DEF_INTERNAL_RAM_NORM       (0x20020000)
#define DEF_INTERNAL_RAM_NORM_LIMIT (0x40000000)
#define DEF_USER_CODE_START         (uint32_t*)(DEF_USER_PROGRAM_SRC + 0x20)
#define DEF_USER_CODE_END           (uint32_t*)(DEF_USER_PROGRAM_SRC + 0x24)
#define DEF_USER_CODE_EXECUTE       (uint32_t*)(DEF_USER_PROGRAM_SRC + 0x28)
#define DEF_USER_SIGNATURE          (uint32_t*)(DEF_USER_PROGRAM_SRC + 0x2c)

#define DELAY_500MS                 (5000000u)
#define DELAY_20000MS               (DELAY_500MS * 4U)

#define DEF_SPIBSC_DUAL_MODE  SPIBSC_CMNCR_BSZ_DUAL

uint32_t error_sequence[] = {DELAY_20000MS,
	                      	 DELAY_500MS,
		                     DELAY_500MS,
		                     DELAY_500MS,
                             -1u};

char g_signature[] = ".BootLoad_ValidProgramTest.";

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void UserProgJmp (uint32_t addr);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
st_spibsc_cfg_t g_spibsc_cfg;

/******************************************************************************
Private global variables and functions
******************************************************************************/
void spibsc_init2 (void);

/******************************************************************************
* Function Name: error_image
* Description  : This function is only called if the final boot loader stage
*                cannot recognise that a valid users application is available.
*                It will not exit and configures the LED0 as an output and
*                flashes the LED with a predefined recognisable pattern.
* Arguments    : none
* Return Value : none
******************************************************************************/
void error_image(void)
{

	setNumericDisplay("ERRO");
    /* This function will never exit - the system has failed to boot. */
    while(1);
}

/******************************************************************************
* Function Name: check_image
* Description  : This function checks that the g_signature text expected is
*                correct before attempting to jump to the application code.
* Arguments    : none
* Return Value : none
******************************************************************************/
void check_image(void)
{
    uint8_t p    = 0;
    uint8_t * psrc = (uint8_t *)DEF_USER_SIGNATURE;
    uint16_t  len = sizeof(g_signature);

    while(--len)
    {
    	if( (*(psrc++)) != g_signature[p++])
        {
    	    /* This is intentional */
            break;
        }
    }

    /* If the length remaining is not zero then the g_signature validation failed. */
    if(!(0 == len))
    {
        /* This function will never return. */
    	error_image();
    }
}

/******************************************************************************
* Function Name: boot_demo
* Description  : Before calling this function the signature has been checked and
*                a valid users image is detected. This function checks the
*                location of the users code. If it matches the RAM area then it
*                needs to be copied into the final RAM location. If not the
*                code is executed from the provided execution address directly.
* Arguments    : none
* Return Value : none
******************************************************************************/
void boot_demo(void)
{
	uint32_t i;
	uint32_t loop_num;
	int32_t  size;
    uint32_t * psrc;
    uint32_t * pdst;
    uint32_t * preg;
    uint32_t * pexe;
    uint32_t * pcst_base = DEF_USER_CODE_START;
    uint32_t * pcex_base = DEF_USER_CODE_EXECUTE;
    uint32_t * pcen_base = DEF_USER_CODE_END;

    /* Used to suppress the 'variable declared but not used' warning */
    UNUSED_VARIABLE(psrc);
    UNUSED_VARIABLE(pdst);
    UNUSED_VARIABLE(preg);
    UNUSED_VARIABLE(pexe);

    psrc = (uint32_t *)DEF_USER_PROGRAM_SRC;
    pdst = (uint32_t *)*pcst_base;
    pexe = (uint32_t *)*pcex_base;

    size = (int32_t)((*pcen_base) - (*pcst_base));

    if  (
            (size > 0)
        &&  (pdst >= (uint32_t *)DEF_INTERNAL_RAM_NORM)
        &&  (pdst <= (uint32_t *)DEF_INTERNAL_RAM_NORM_LIMIT)
        )
    {

        loop_num = (((uint32_t)size + 3) / (sizeof(uint32_t)));
        for(i=0;i<loop_num;i++)
        {
            (*pdst++) = (*psrc++);
        }
    }
    UserProgJmp((uint32_t )pexe);
}

static char stringBuffer[20];


// May give wrong result for -2147483648
static char* intToString(int32_t number, uint8_t minNumDigits) {
    uint8_t numDigits = 0;
    bool_t isNegative = (number < 0);
    number = abs(number);
    stringBuffer[11] = 0;
    uint8_t charPos = 11;
    while(number != 0 || numDigits < minNumDigits) {
        stringBuffer[--charPos] = 48 + (number % 10);
        number /= 10;
        numDigits++;
    }
    if (isNegative) stringBuffer[--charPos] = 45;
    return stringBuffer + charPos;
}




const uint8_t letterSegments[26] = {
    0x77,
    0x1F,
    0x4E,
    0x3D,
    0x4F,
    0x47, //F
    0x5E,
    0x37, //H
    0x04,
    0x38, //J
    0x57, //0x2F,
    0x0E, //L
    0x55,
    0x15, //N
    0x1D,
    0x67,
    0x73, //Q
    0x05,
    0x5B,
    0x0F, //T
    0x3E,
    0x27,
    0x5C,
    0x49,
    0x3B,
    0x6D
};

/******************************************************************************
* Function Name: spibsc_init2
* Description  :
* Arguments    : none
* Return Value : none
******************************************************************************/

#define UART_CHANNEL_MIDI 0
#define UART_CHANNEL_PIC 1

void spibsc_init2(void)
{

	// Uart 1 for PIC / display
	uartInit(UART_CHANNEL_PIC, 200000);
	setPinMux(3, 15, 5); // TX
	setPinMux(1, 9, 3); // RX


	/* Wait TEND=1 for setting change in SPIBSC. */
    R_SFLASH_WaitTend(SPIBSC_CH);

    // This bit here - I can remove it and everything functions fine. I think it's meant to speed things up, but I can only have these lines sitting here by setting
    // but width to 1, at the top of spibsc_ioset_userdef.c. If I could set that to 4, things would be faster, but it doesn't work... - Rohan
    /* ==== Sets the SPIBSC ==== */
    R_SFLASH_Set_Config(SPIBSC_CH, &g_spibsc_cfg); // Collates a bunch of preset options into the variable-holder g_spibsc_cfg, for us to actually enact in the next step
    if( R_SFLASH_Exmode_Setting(SPIBSC_CH, SPIBSC_CMNCR_BSZ_SINGLE, &g_spibsc_cfg) != 0 ) // This is the line I had to change from the default RSK config - it would only have worked in "DUAL" mode - when 2x flash chips connected
    {
    	error_image();
    }

	//enable_irq();
	//enable_fiq();


    //initUartInterrupts();

	// Uart 0 for MIDI
	uartInit(UART_CHANNEL_MIDI, 31250);
	setPinMux(6, 15, 5); // TX
	setPinMux(6, 14, 5); // RX


    //CPG.STBCR10 = 0x00u; // Enable clock to somewhere

	// Not quite sure why, but we have to wait a while before sending out the UART, otherwise it sometimes doesn't get received by the PIC MCU (At least on Revision E)
	// I did try lowering the baud rate of the UART and it didn't help
	uint32_t i = 600000; // 400000 works usually, but even 500000 seemed not to work one time. 200000 is too low
	while (i--) {}

	// Tell PIC to re-send which buttons are pressed
	uartPutChar(UART_CHANNEL_PIC, 22);

	// Listen for a while
	i = 50000;
	while (i--) {
		char received;
		uint8_t success = uartGetChar(UART_CHANNEL_PIC, &received);

		if (success) {
			io_put_number((unsigned int)received);

			/*
			// If we received the "I've finished sending everything" message, get out
			if (received == 253) {
				break;
			}

			// If the shift button's down, then update the firmware
			if (received == 152) {
				updateFirmware();
				break;
			}
			*/
		}
	}

    check_image();
    boot_demo();
}


void setNumericDisplay(char const* text) {
	uartPutChar(UART_CHANNEL_PIC, 224);
	int i;
	for (i = 0; i < 4; i++) {
		uartPutChar(UART_CHANNEL_PIC, letterSegments[*(text++) - 65]);
	}
}

uint8_t loadingAnimationPos;

void progressLoadingAnimation() {

	char segmentsInWaiting[5];
	memset(&segmentsInWaiting, 0, sizeof(segmentsInWaiting));


    if (loadingAnimationPos < 4) segmentsInWaiting[loadingAnimationPos] = 0x40;
    else if (loadingAnimationPos == 4) segmentsInWaiting[3] = 0x30;
    else if (loadingAnimationPos < 9) segmentsInWaiting[3 - (loadingAnimationPos - 5)] = 0x08;
    else segmentsInWaiting[0] = 0x06;

    loadingAnimationPos = (loadingAnimationPos + 1) % 10;

	uartPutChar(2, 116);
	int i;
	for (i = 0; i < 4; i++) {
		uartPutChar(2, segmentsInWaiting[i]);
	}
}


void updateFirmware() {
	loadingAnimationPos = 0;

	//uartPrintln("updating firmware");
	setNumericDisplay("UPDA");

	while(1);
	FATFS fileSystem;

	FRESULT result = f_mount(&fileSystem, "", 1);
    if (result != FR_OK) goto cardError;
	DIR dir;

	result = f_opendir(&dir, "");
    if (result != FR_OK) goto cardError;
	while (true) {
		FILINFO fno;
		result = f_readdir(&dir, &fno); // Read a directory item
		if (result != FR_OK || fno.fname[0] == 0) break; // Break on error or end of dir

		char* dotPos = strrchr(fno.fname, '.');
		if (dotPos != 0 && !strcmp(dotPos, ".BIN")) {

			// We found our .bin file!

			f_closedir(&dir);

			FIL currentFile;
			// Open the file
			result = f_open(&currentFile, fno.fname, FA_READ);
			if (result != FR_OK) goto fileError;

			// The file opened. Erase the flash memory
			uint32_t numSectors = ((currentFile.fsize) >> 16) + 1;
			uartPrintln(intToString(numSectors, 1));

			uint32_t eraseAddress = 0x80000;
			while (numSectors-- && eraseAddress < 0x01000000) {
				R_SFLASH_EraseSector(eraseAddress, 0, SPIBSC_CMNCR_BSZ_SINGLE, 1, SPIBSC_OUTPUT_ADDR_24);
				eraseAddress += 0x10000; // 64K
				progressLoadingAnimation();
			}

			char buffer[256];
			uint32_t writeAddress = 0x80000;

			uint32_t i = 0;

			while (writeAddress < 0x01000000) {
				if (!(i--)) {
					//uartPrintln(intToString(writeAddress, 1));
					i = 16;
				}
				UINT numBytesRead;
				result = f_read(&currentFile, (UINT*)buffer, 256, &numBytesRead);
				if (result != FR_OK || numBytesRead == 0) break;

				R_SFLASH_ByteProgram(writeAddress, buffer, numBytesRead, 0, SPIBSC_CMNCR_BSZ_SINGLE, SPIBSC_1BIT, SPIBSC_OUTPUT_ADDR_24);
				//R_SFLASH_ByteProgram(writeAddress, buffer, 256, 0, SPIBSC_CMNCR_BSZ_SINGLE, SPIBSC_1BIT, SPIBSC_OUTPUT_ADDR_24);
				//R_SFLASH_ByteProgram(writeAddress, buf, 5, 0, SPIBSC_CMNCR_BSZ_SINGLE, SPIBSC_1BIT, SPIBSC_OUTPUT_ADDR_24);
				if (numBytesRead != 256) break;
				writeAddress += 256;
				//if (writeAddress & 0b00000000000000000000011111111111 == 0) progressLoadingAnimation();
				if ((writeAddress >> 8) % 16 == 0) progressLoadingAnimation();
			}

			// Success. It's all updated.
			setNumericDisplay("DONE");
			// Go back into external SPI bus mode thing
			spibsc_exmode(0);
			return;
		}
	}
	f_closedir(&dir);

	fileError:
	setNumericDisplay("FILE");
	while (1) {}

	cardError:
	setNumericDisplay("CARD");
	while (1) {}
}


/* End of File */
