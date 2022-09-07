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

// Beware - for some reason, when fiddling with such settings, the "garbage collection" linker option keeps disabling itself. It should be on, but I don't think it makes a
// huge difference.


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "spibsc.h"
#include "r_spibsc_ioset_api.h"
#include "rza_io_regrw.h"
#include "io.h"
#include "fatfs/ff.h"
#include "sio_char.h"
#include "peripheral_init_basic.h"
#include "oled.h"
#include "mtu.h"

#define HAVE_OLED 1

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
#if HAVE_OLED
	displayPrompt("No valid firmware is installed.\nPlease re-install firmware.");
	monitorInputFromPICForever();
#else
	setNumericDisplay("ERRO");
    /* This function will never exit - the system has failed to boot. */
    while(1);
#endif
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
    //uartPrint("going to overwrite from: ");
    io_put_number((int)pdst);

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
        	if (!(i & 8191)) drawNextLogoPixel();
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


const uint8_t numberSegments[10] = {
    0x7E,
    0x30,
    0x6D,
    0x79,
    0x33,
    0x5B,
    0x5F,
    0x70,
    0x7F,
    0x7B
};


/******************************************************************************
* Function Name: spibsc_init2
* Description  :
* Arguments    : none
* Return Value : none
******************************************************************************/

#define UART_CHANNEL_MIDI 0
#define UART_CHANNEL_PIC 1

uint8_t bigBuffer[1 << 19]; // Half a meg

#define EXTERNAL_MEMORY_BEGIN 0x0C000000
#define FLASH_WRITE_SIZE 256 // Bigger doesn't seem to work...

extern void L1CacheInit(void);
extern void enable_mmu(void);




extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

int bootedUp = 0;



void spibsc_init2(void)
{

/*
	uint32_t* here = (uint32_t*)__bss_start__;
	while (here < (uint32_t*)__bss_end__) {
		*here = 0;
		here++;
	}
*/



	while (false) {
		bool_t result = drawNextLogoPixel();
		if (!result) break;
		delayMSWhileMonitoringPIC(25);
	}


	//char value;
	//while (uartGetChar(UART_CHANNEL_PIC, &value)) {}

/*
	int j = 0;
	clearMainImage();
	while (j < 128) {
		//displayPrompt(intToString((int)j, 1));

		drawString("a", j, OLED_MAIN_TOPMOST_PIXEL, oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED, 0, OLED_MAIN_WIDTH_PIXELS);
		//invertArea(j, 1, 20, 20, oledMainImage[0]);
		sendMainImage();

		//delayMS(25);
		//monitorInputFromPIC();
		delayMSWhileMonitoringPIC(25);
		j++;
	}
*/
	//displayPrompt("Deluge");




	// Set mux for SPIBSC pins 2 and 3, which isn't done by default and needs to be done here for quad SPI to work, I think
	//setPinMux(4, 2, 2);
	//setPinMux(4, 3, 2);

	//enable_mmu(); // Nope, causes crash

	//Peripheral_Basic_Init(); // Makes clocks go way fast - oh except it actually doesn't help here


	//L1CacheInit(); // Enable caching. Speeds up the RAM test a little bit - I guess because stuff can be pre-fetched and stuff

	userdef_bsc_cs2_init(); // Setup external RAM


#if 1
	/* Wait TEND=1 for setting change in SPIBSC. */
    R_SFLASH_WaitTend(SPIBSC_CH);

    /* ==== Sets the SPIBSC ==== */
    R_SFLASH_Set_Config(SPIBSC_CH, &g_spibsc_cfg); // Collates a bunch of preset options into the variable-holder g_spibsc_cfg, for us to actually enact in the next step

    if( R_SFLASH_Exmode_Setting(SPIBSC_CH, SPIBSC_CMNCR_BSZ_SINGLE, &g_spibsc_cfg) != 0 )	// This is the line I had to change from the default RSK config
    																						// - it would only have worked in "DUAL" mode - when 2x flash chips connected.
    																						// (Or would that actually mean the DDR mode instead?)
    {
    	error_image();
    }
#endif

    bootedUp = 1;

    // Set mux for SD host interface
	setPinMux(7, 0, 3); // CD
	setPinMux(7, 1, 3); // WP
	setPinMux(7, 2, 3); // D1
	setPinMux(7, 3, 3); // D0
	setPinMux(7, 4, 3); // CLK
	setPinMux(7, 5, 3); // CMD
	setPinMux(7, 6, 3); // D3
	setPinMux(7, 7, 3); // D2

    //initUartInterrupts();

	// Uart 0 for MIDI
	/*
	uartInit(UART_CHANNEL_MIDI, 31250);
	setPinMux(6, 15, 5); // TX
	setPinMux(6, 14, 5); // RX
	*/






	// Not quite sure why, but we have to wait a while before sending out the UART, otherwise it sometimes doesn't get received by the PIC MCU (At least on Revision E)
	// I did try lowering the baud rate of the UART and it didn't help
	uint32_t i = 1200000; // 400000 works usually, but even 500000 seemed not to work one time. 200000 is too low
	while (i--) {}

	// Tell PIC to re-send which buttons are pressed
	uartPutChar(UART_CHANNEL_PIC, 22);

	uint8_t buttonsPressed = 0;

	//uartPrintln("listening");
	// Listen for a while
	uint16_t startTime = *TCNT[TIMER_SYSTEM_SLOW];
	uint16_t stopTime = startTime + msToSlowTimerCount(50);

	while ((int16_t)(*TCNT[TIMER_SYSTEM_SLOW] - stopTime) < 0) {
		char received;
		uint8_t success = uartGetChar(UART_CHANNEL_PIC, &received);

		if (success) {
			//io_put_number((unsigned int)received);

			switch (received) {

			// Pad combination for updating the bootloader itself
			case 63:
				buttonsPressed |= 0b1;
				break;
			case 126:
				buttonsPressed |= 0b10;
				break;
			case 46:
				buttonsPressed |= 0b100;
				break;



			// Shift button: update the firmware
			case 152:
				updateFirmware(false, 0x80000, 3670016, HAVE_OLED ? "Updating firmware..." : "UPDA", EXTERNAL_MEMORY_BEGIN); // Max 3.5MB
				goto bootUp;

			// Pad to display bootloader version
			case 0:
#if HAVE_OLED
				displayPrompt("Bootloader version: TES8");
				monitorInputFromPICForever();
#else
				setNumericDisplay("TES8");
				while (1) {}
#endif
				break;

			// Pad to test RAM
			case 80:
				ramTest();
				while (1) {}
				break;

			// If we received the "I've finished sending everything" message, get out
			case 253:
				goto finishedListening;

			default:
				uartInputReceivedPossiblyOledRelated(received);
				break;

			}
		}
	}
finishedListening:

	// If they pressed the super secret combination to overwrite the bootloader itself...
	if (buttonsPressed == 0b111) {
#if HAVE_OLED
		displayPrompt("Are you sure you wish to\noverwrite the Deluge's\nbootloader? If an error\noccurs or power is lost,\nthis device will become\nunusable.");
#else
		setNumericDisplay("SURE");
#endif

		while (1) {
			char received;
			uint8_t success = uartGetChar(UART_CHANNEL_PIC, &received);
			if (success) {
				if (received == 175) break;
				else {
					uartInputReceivedPossiblyOledRelated(received);
				}
			}

		}

		updateFirmware(1, 0, 0x80000 - 0x1000, HAVE_OLED ? "Updating bootloader.\nDo not switch device off." : "BOOT", bigBuffer);
	}

bootUp:

    check_image();
    boot_demo();
}



void setNumericDisplay(char const* text) {
	uartPutChar(UART_CHANNEL_PIC, 224);
	int i;
	for (i = 0; i < 4; i++) {
		char output;

		if (*text == ' ') {
			output = 0;
		}
		else if (*text >= 'A') {
			output = letterSegments[*text - 'A'];
		}
		else {
			output = numberSegments[*text - '0'];
		}
		uartPutChar(UART_CHANNEL_PIC, output);
		text++;
	}
}

uint8_t loadingAnimationPos;

void progressLoadingAnimation() {
	return;
	char segmentsInWaiting[5];
	memset(&segmentsInWaiting, 0, sizeof(segmentsInWaiting));


    if (loadingAnimationPos < 4) segmentsInWaiting[loadingAnimationPos] = 0x40;
    else if (loadingAnimationPos == 4) segmentsInWaiting[3] = 0x30;
    else if (loadingAnimationPos < 9) segmentsInWaiting[3 - (loadingAnimationPos - 5)] = 0x08;
    else segmentsInWaiting[0] = 0x06;

    loadingAnimationPos = (loadingAnimationPos + 1) % 10;

	uartPutChar(UART_CHANNEL_PIC, 224);
	int i;
	for (i = 0; i < 4; i++) {
		uartPutChar(UART_CHANNEL_PIC, segmentsInWaiting[i]);
	}
}

FATFS fileSystem;

void displayPromptAndWait(char const* message) {
	if (!bootedUp) return;

    displayPrompt(message);
	delayMS(10);
	monitorInputFromPIC();
}


void updateFirmware(uint8_t doingBootloader, uint32_t startFlashAddress, uint32_t maxSize, char const* message, uint8_t* buffer) {
	loadingAnimationPos = 0;
	char const* errorMessage = "NONE";

#if HAVE_OLED
	monitorInputFromPIC();
	//displayPrompt(message);
	delayMS(10);
	monitorInputFromPIC();
	delayMS(10);
	monitorInputFromPIC();
#else
	setNumericDisplay(message);
#endif


	FRESULT result = f_mount(&fileSystem, "", 1);
	//setNumericDisplay("2");
	//uartPrintln("mounted");
    if (result != FR_OK) {
cardError:
    	errorMessage = HAVE_OLED ? "Card error" : "CARD";
		goto displayError;
    }

    //displayPrompt("Filesystem mounted");
	delayMS(10);
	monitorInputFromPIC();
	delayMS(10);
	monitorInputFromPIC();

	DIR dir;

	result = f_opendir(&dir, "");
	//uartPrintln("dir opened");
    if (result != FR_OK) goto cardError;
	while (true) {
		FILINFO fno;
		result = f_readdir(&dir, &fno); // Read a directory item
		if (result != FR_OK || fno.fname[0] == 0) break; // Break on error or end of dir

		if (fno.fname[0] == '_') continue; // Avoid hidden files created by stupid Macs

		// Only bootloader bin files should start with "BOOT"
		uint8_t startsWithBoot = (!memcmp(fno.fname, "BOOT", 4) || !memcmp(fno.fname, "boot", 4)) ? 1 : 0;
		if (startsWithBoot != doingBootloader) continue;

		char const* dotPos = strchr(fno.fname, '.');
		if (dotPos != 0 && !strcmp(dotPos, ".BIN")) {

			// We found our .bin file!
			//uartPrintln("found bin file");


		    //displayPrompt("Found file");
			delayMS(10);
			monitorInputFromPIC();
			delayMS(10);
			monitorInputFromPIC();

			f_closedir(&dir);

			FIL currentFile;
			// Open the file
			result = f_open(&currentFile, fno.fname, FA_READ);
			if (result != FR_OK) {
fileError:
				errorMessage = HAVE_OLED ? "File error" : "FILE";
				goto displayError;
			}

			uint32_t fileSize = currentFile.fsize;

			// But make sure it's not too big
			if (fileSize > maxSize) {
#if HAVE_OLED
				message = doingBootloader ? "Bootloader file found is\ntoo large to be valid." : "Firmware file found is\ntoo large to be valid.";
				goto displayError;
#else
				goto fileError;
#endif
			}

			// Or to small
			if (!fileSize) {
#if HAVE_OLED
				message = doingBootloader ? "Bootloader file found is\n0 bytes." : "Firmware file found is\n0 bytes.";
				goto displayError;
#else
				goto fileError;
#endif
			}


			// The file opened. Copy it to RAM
			UINT numBytesRead;
			result = f_read(&currentFile, buffer, fileSize, &numBytesRead);
			if (result != FR_OK || numBytesRead != fileSize) goto fileError;
			f_close(&currentFile);

			/*
		    //displayPrompt("File copied to RAM");
			delayMS(10);
			monitorInputFromPIC();
			delayMS(10);
			monitorInputFromPIC();
*/
			// Erase the flash memory
			uint32_t numFlashSectors = ((fileSize - 1) >> 16) + 1;
			//uartPrintln(intToString(numSectors, 1));

			uint32_t eraseAddress;






eraseFlash:
			//monitorInputFromPIC();
			eraseAddress = startFlashAddress;
			while (numFlashSectors-- && eraseAddress < 0x01000000) {

				if (eraseAddress == startFlashAddress) {// + 0x10000) {
					//
				}


				//displayPrompt(intToString(numFlashSectors, 1));
				//monitorInputFromPIC();
				R_SFLASH_EraseSector(eraseAddress, 0, SPIBSC_CMNCR_BSZ_SINGLE, 1, SPIBSC_OUTPUT_ADDR_24);
				eraseAddress += 0x10000; // 64K
				//progressLoadingAnimation();
			}



/*
			spibsc_exmode(0); // Very weirdly, you have to call this in order to output to the OLED after
			// doing a R_SFLASH_EraseSector(). It's something to do with interrupts - if you R_INTC_Disable(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL),
			// that at least saves everything from freezing.
		    displayPrompt("Flash memory erased");
			delayMS(10);
			monitorInputFromPIC();
			delayMS(10);
			monitorInputFromPIC();
*/
			// Copy new program from RAM to flash memory
			uint32_t flashWriteAddress = startFlashAddress;
			uint8_t* readAddress = buffer;

			while (true) {

				int bytesLeft = startFlashAddress + fileSize - flashWriteAddress;
				if (bytesLeft <= 0) break;

				//displayPrompt(intToString(bytesLeft, 1));
				//monitorInputFromPIC();

				int bytesToWrite = bytesLeft;
				if (bytesToWrite > FLASH_WRITE_SIZE) bytesToWrite = FLASH_WRITE_SIZE;

				int32_t error = R_SFLASH_ByteProgram(flashWriteAddress, readAddress, bytesToWrite, 0, SPIBSC_CMNCR_BSZ_SINGLE, SPIBSC_1BIT, SPIBSC_OUTPUT_ADDR_24);
				if (error) {
#if HAVE_OLED
					displayPrompt("Internal flash memory error.\nTrying again...");
#else
					setNumericDisplay("RETR");
#endif
					goto eraseFlash;
				}

				flashWriteAddress += FLASH_WRITE_SIZE;
				readAddress += FLASH_WRITE_SIZE;
				//if (((flashWriteAddress >> 8) & 63) == 0) progressLoadingAnimation();
			}

			// Success. It's all updated.
#if HAVE_OLED
			displayPrompt(doingBootloader ? "Bootloader updated successfully\nPlease reboot." : "Firmware updated\nsuccessfully.");
			if (doingBootloader) monitorInputFromPICForever();
			monitorInputFromPIC();
#else
			setNumericDisplay("DONE");
#endif
			//uartPrintln("copying finished");
			// Go back into external SPI bus mode thing
			spibsc_exmode(0);
			monitorInputFromPIC();
			return;
		}
	}
	f_closedir(&dir);
displayError:
#if HAVE_OLED
	displayPrompt(errorMessage);
#else
	setNumericDisplay(errorMessage);
#endif
	monitorInputFromPICForever();
}


#define SDRAM_MODE_CS2    (*(volatile uint16_t *)(0x3FFFD040))
#define SDRAM_MODE_CS3    (*(volatile uint16_t *)(0x3FFFE040))

void userdef_bsc_cs2_init()
{

	setPinMux(3, 0, 1); // A1
	setPinMux(3, 1, 1); // A1
	setPinMux(3, 2, 1); // A1
	setPinMux(3, 3, 1); // A1
	setPinMux(3, 4, 1); // A1
	setPinMux(3, 5, 1); // A1
	setPinMux(3, 6, 1); // A1
	setPinMux(3, 7, 1); // A1
	setPinMux(3, 8, 1); // A1
	setPinMux(3, 9, 1); // A1
	setPinMux(3, 10, 1); // A1
	setPinMux(3, 11, 1); // A1
	setPinMux(3, 12, 1); // A1
	setPinMux(3, 13, 1); // A1
	setPinMux(3, 14, 1); // A1

	// D pins
	setPinMux(5, 0, 1);
	setPinMux(5, 1, 1);
	setPinMux(5, 2, 1);
	setPinMux(5, 3, 1);
	setPinMux(5, 4, 1);
	setPinMux(5, 5, 1);
	setPinMux(5, 6, 1);
	setPinMux(5, 7, 1);
	setPinMux(5, 8, 1);
	setPinMux(5, 9, 1);
	setPinMux(5, 10, 1);
	setPinMux(5, 11, 1);
	setPinMux(5, 12, 1);
	setPinMux(5, 13, 1);
	setPinMux(5, 14, 1);
	setPinMux(5, 15, 1);

	//setPinMux(7, 8, 1); // CS2
	setPinMux(2, 0, 1); // CS3
	setPinMux(2, 1, 1); // RAS
	setPinMux(2, 2, 1); // CAS
	setPinMux(2, 3, 1); // CKE
	setPinMux(2, 4, 1); // WE0
	setPinMux(2, 5, 1); // WE1
	setPinMux(2, 6, 1); // RD/!WR



    /* ==== CS2BCR settings ==== */
    /* Idle Cycles between Write-read Cycles  */
    /* and Write-write Cycles : 0 idle cycles */
    /* Memory type :SDRAM                     */
    /* Data Bus Size : 16-bit                 */
    BSC.CS2BCR = 0x00004C00ul;

    /* ==== CS3BCR settings ==== */
    /* SDRAM WORKAROUND - see Note */
    /* Idle Cycles between Write-read Cycles  */
    /* and Write-write Cycles : 0 idle cycles */
    /* Memory type :SDRAM                     */
    /* Data Bus Size : 16-bit                 */
    BSC.CS3BCR = 0x00004C00ul;

    /* ==== CS2/3WCR settings ==== */
    /* Precharge completion wait cycles: 1 cycle     */
    /* Wait cycles between ACTV command              */
    /* and READ(A)/WRITE(A) command : 1 cycles       */
    /* CAS latency for Area 3 : 2 cycles             */
    /* Auto-precharge startup wait cycles : 2 cycles */
    /* Idle cycles from REF command/self-refresh     */
    /* Release to ACTV/REF/MRS command : 5 cycles    */
    BSC.CS3WCR = 0x00004492ul;

    /* SDRAM WORKAROUND - see Note */
    BSC.CS2WCR = 0x00000480ul;

    /* ==== SDCR settings ==== */
    /* SDRAM WORKAROUND - see Note*/
    /* Row address for Area 2 : 13-bit    */
    /* Column Address for Area 2 : 9-bit  */
    /* Refresh Control :Refresh           */
    /* RMODE :Auto-refresh is performed   */
    /* BACTV :Auto-precharge mode         */
    /* Row address for Area 3 : 13-bit    */
    /* Column Address for Area 3 : 9-bit  */
    BSC.SDCR = 0x00110812ul; // CS3 cols 10-bit. For 64MB chip

    /* ==== RTCOR settings ==== */
    /* 7.64usec / 240nsec              */
    /*   = 128(0x80)cycles per refresh */
    BSC.RTCOR = 0xA55A0080ul;

    /* ==== RTCSR settings ==== */
    /* initialisation sequence start */
    /* Clock select B-phy/4          */
    /* Refresh count :Once           */
    BSC.RTCSR = 0xA55A0008ul;

    /* ==== SDRAM Mode Register ==== */
    /* Burst read (burst length 1)./Burst write */
    SDRAM_MODE_CS2 = 0;

    /* SDRAM WORKAROUND - see Note */
    SDRAM_MODE_CS3 = 0;
}


void ramTest() {

#if HAVE_OLED
	displayPrompt("Testing RAM...");
#else
	setNumericDisplay("RAM ");
#endif

	loadingAnimationPos = 0;

	//delayMS(1);

/*
	int i = 0;
	while (true) {
		displayPrompt(intToString((int)i, 1));
		delayMS(1);
		monitorInputFromPIC();
		i++;
	}
*/

	uint32_t* address;

	// Write RAM
	address = (uint32_t*)0x0C000000;
	while (address != (uint32_t*)0x10000000) {
		*address = (uint32_t)address;
		address++;

		/*
		if (!((uint32_t)address & 0b111111)) {
			char buffer[12];
			displayPrompt(intToString((int)address, 1));
			delayMS(10);
			monitorInputFromPIC();
		}
*/

		/*
		if (address >= (uint32_t*)0x0D000000 && !((uint32_t)address & (uint32_t)((1 << 20) - 1))) { // Every megabyte
			progressLoadingAnimation();
		}
		*/
	}

	displayPrompt("Writing finished...");
	delayMS(10);
	monitorInputFromPIC();

	// Read RAM
	address = (uint32_t*)0x0C000000;
	while (address != (uint32_t*)0x10000000) {

		if (*address != (uint32_t)address) {
#if HAVE_OLED
			displayPrompt("RAM error detected.\nThis device will not function properly.");
			monitorInputFromPICForever();
#else
			setNumericDisplay("FAIL");
			while (1) {}
#endif
		}
		address++;

		if (!((uint32_t)address & (uint32_t)((1 << 20) - 1))) { // Every megabyte
			progressLoadingAnimation();
		}
	}

#if HAVE_OLED
	displayPrompt("RAM test passed.\nPlease reboot.");
	monitorInputFromPICForever();
#else
	setNumericDisplay("GOOD");
#endif
}


/* End of File */
