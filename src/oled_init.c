/*
 * oled_init.c
 *
 *  Created on: 7/09/2022
 *      Author: Rohan
 */

#include "r_typedefs.h"
#include "cpg_iodefine.h"
#include "mtu.h"
#include "oled.h"
#include "dmac_iobitmask.h"

extern uint32_t __code_spibsc_init_2_start__;
extern uint32_t __code_spibsc_init_2_end__;
extern uint32_t __code_spibsc_init_2_src__;

void MainOLED_WCom(char data) {
	R_RSPI_SendBasic8(SPI_CHANNEL_OLED_MAIN, data);
}


void bufferPICUart(char data) {
	uartPutChar(UART_CHANNEL_PIC, data);
}



void enableAllModuleClocks() {

    volatile uint8_t dummy_buf = 0u;

    UNUSED_VARIABLE(dummy_buf);

    /* ---- Enable all module clocks ---- */

    /* Port level is keep in standby mode, [1], [1], [0],           */
    /* [1], [0], [1], CoreSight                                     */
    CPG.STBCR2 = 0x6Au;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR2;

    /* IEBus, IrDA, LIN0, LIN1, MTU2, RSCAN2, [0], PWM              */
    CPG.STBCR3 = 0x00u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR3;

    /* SCIF0, SCIF1, SCIF2, SCIF3, SCIF4, SCIF5, SCIF6, SCIF7       */
    CPG.STBCR4 = 0b00000111;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR4;

    /* SCIM0, SCIM1, SDG0, SDG1, SDG2, SDG3, OSTM0, OSTM1           */
    CPG.STBCR5 = 0x00u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR5;

    /* A/D, CEU, DISCOM0, DISCOM1, DRC0, DRC1, JCU, RTClock         */
    CPG.STBCR6 = 0x00u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR6;

    /* DVDEC0, DVDEC1, [1], ETHER, FLCTL, [1], USB0, USB1           */
    CPG.STBCR7 = 0x24u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR7;

    /* IMR-LS20, IMR-LS21, IMR-LSD, MMCIF, MOST50, [1], SCUX, [1]   */
    CPG.STBCR8 = 0x05u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR8;

    /* I2C0, I2C1, I2C2, I2C3, SPIBSC0, SPIBSC1, VDC50, VDC51       */
    CPG.STBCR9 = 0x00u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR9;

    /* RSPI0, RSPI1, RSPI2, RSPI3, RSPI4, CD-ROMDEC, RSPDIF, RGPVG  */
    CPG.STBCR10 = 0b00011111;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR10;

    /* [1], [1], SSIF0, SSIF1, SSIF2, SSIF3, SSIF4, SSIF5           */
    CPG.STBCR11 = 0xC0u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR11;

    /* [1], [1], [1], [1], SDHI00, SDHI01, SDHI10, SDHI11           */
    CPG.STBCR12 = 0xF0u;

    /* (Dummy read)                                                 */
    dummy_buf = CPG.STBCR12;
}

struct SpiTransferQueueItem spiTransferQueue[SPI_TRANSFER_QUEUE_SIZE];
volatile bool_t spiTransferQueueCurrentlySending = false;
volatile int spiTransferQueueReadPos = 0;
int spiTransferQueueWritePos = 0;

void oledMainInit() {

	spiTransferQueueReadPos = 0;
	spiTransferQueueWritePos = 0;
	spiTransferQueueCurrentlySending = false;

	 MainOLED_WCom(0xFD); // SET COMMAND LOCK
	 MainOLED_WCom(0x12);
	 MainOLED_WCom(0xAE); //DOT MARTIX DISPLAY OFF

	 MainOLED_WCom(0x81); //CONTARST CONTROL(00H-0FFH)
	 MainOLED_WCom(0xFF);

	 MainOLED_WCom(0xA4); //ENTIRE DISPLAY OFF(0A4H-0A5H)

	 MainOLED_WCom(0xA6); //SET NORMAL DISPLAY(0A6H-0A7H)

	 MainOLED_WCom(0x00); // SET LOW COLUMN START ADDRESS
	 MainOLED_WCom(0x10); //SET HIGH COLUMN START ADDRESS

	 MainOLED_WCom(0x20); //SET MEMORRY ADDRESSING MODE
	 MainOLED_WCom(0x00); // Horizontal

/*
	 MainOLED_WCom(0xB0); // Set Page Start
*/


#if OLED_MAIN_HEIGHT_PIXELS != 64
	 MainOLED_WCom(0x22); // Set page address start / end
	 MainOLED_WCom((64 - OLED_MAIN_HEIGHT_PIXELS) >> 3);
	 MainOLED_WCom(7);
#endif

	 MainOLED_WCom(0x40); //SET DISPLAY START LINE (040H-07FH) // Moves entire graphics vertically

	 MainOLED_WCom(0xA0); //SET SEGMENT RE-MAP(0A0H-0A1H) // Flips stuff 180deg!

	 MainOLED_WCom(0xA8); //SET MULTIPLEX RATIO 64
	 MainOLED_WCom(0x3F);

	 MainOLED_WCom(0xC0); //COM SCAN COM1-COM64(0C8H,0C0H)

	 MainOLED_WCom(0xD3); //SET DISPLAY OFFSET(OOH-3FH)
	 MainOLED_WCom(0x0);

	 MainOLED_WCom(0xDA); //COM PIN CONFIGURATION
	 MainOLED_WCom(0x12);

	 MainOLED_WCom(0xD5); //SET FRAME FREQUENCY
	 MainOLED_WCom(0xF0);

	 MainOLED_WCom(0xD9); //SET PRE_CHARGE PERIOD
	 MainOLED_WCom(0xA2);

	 MainOLED_WCom(0xDB); //SET VCOM DESELECT LEVEL
	 MainOLED_WCom(0x34);

	 MainOLED_WCom(0xAF); //DSPLAY ON



	 R_RSPI_WaitEnd(SPI_CHANNEL_OLED_MAIN);

	bufferPICUart(251); // D/C high
}

void oledDeselectionComplete() {
	sendOledDataAfterMessage = 256;
	spiTransferQueueCurrentlySending = false;

	// There might be something more to send now...
	if (spiTransferQueueWritePos != spiTransferQueueReadPos) {
		//sendSPITransferFromQueue();
	}
}


void delayMSWhileMonitoringPIC(uint32_t ms) {
	uint16_t startTime = *TCNT[TIMER_SYSTEM_SLOW];
	uint16_t stopTime = startTime + msToSlowTimerCount(ms);
	while ((int16_t)(*TCNT[TIMER_SYSTEM_SLOW] - stopTime) < 0) {
		monitorInputFromPIC();
	}
}


void uartInputReceivedPossiblyOledRelated(int value) {
	if (value == sendOledDataAfterMessage) {
		//delayUS(2500); // TODO: fix
		if (value == 248)
			oledSelectingComplete();
		else
			oledDeselectionComplete();
	}
}

void monitorInputFromPIC() {

	//return;
goAgain: {}
	char value;
	uint8_t success = uartGetChar(UART_CHANNEL_PIC, &value);
	if (success) {
		uartInputReceivedPossiblyOledRelated(value);
		goto goAgain;
	}
}

void waitForPICWithTimeout(int message, int timeOut) {
	uint16_t startTime = *TCNT[TIMER_SYSTEM_SLOW];
	uint16_t stopTime = startTime + msToSlowTimerCount(timeOut);
	while ((int16_t)(*TCNT[TIMER_SYSTEM_SLOW] - stopTime) < 0) {
		char value;
		uint8_t success = uartGetChar(UART_CHANNEL_PIC, &value);
		if (success) {
			if (value == message) return;
		}
	}
}

uint32_t msToSlowTimerCount(uint32_t ms) {
#if CPU_MODEL == CPU_RZ_A1
	return ms * 33;
#else
	return ms * 66;
#endif
}

#define DMARS_FOR_RSPI_TX	0b100100001

void setDMARS(int dmaChannel, uint32_t dmarsValue) {

	uint32_t mask = 0xFFFF0000;

	if (dmaChannel & 1) {
		dmarsValue <<= 16;
		mask >>= 16;
	}

	volatile uint32_t* dmarsAddress = DMARSnAddress(dmaChannel);

	*dmarsAddress = (*dmarsAddress & mask) | dmarsValue;
}

int sendOledDataAfterMessage = 256;

void oledSelectingComplete() {
	sendOledDataAfterMessage = 256;

	spiTransferQueueReadPos = (spiTransferQueueReadPos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);
	//v7_dma_flush_range((uint32_t)oledMainImage, (uint32_t)oledMainImage + sizeof(oledMainImage));
	DMACn(OLED_SPI_DMA_CHANNEL).CHCTRL_n |= DMAC_CHCTRL_0S_CLRTC | DMAC_CHCTRL_0S_SETEN; // ---- Enable DMA Transfer and clear TC bit ----
}

void oledTransferComplete(uint32_t int_sense) {

	// If anything else to send, and it's to the OLED again, then just go ahead.
	if (spiTransferQueueWritePos != spiTransferQueueReadPos && spiTransferQueue[spiTransferQueueReadPos].destinationId == 0) {
		oledSelectingComplete();
	}

	// Otherwise, deselect OLED. And when that's finished, we might send some more if there is more.
	else {
		bufferPICUart(249); // Unselect OLED

		//delayMS(10);
		//oledDeselectionComplete();
		//return;

		sendOledDataAfterMessage = 249;
		//uartFlushIfNotSending(UART_ITEM_PIC);
	}
}

void oledDMAInit() {

	DMACn(OLED_SPI_DMA_CHANNEL).N0TB_n = sizeof(oledMainImage);
	DMACn(OLED_SPI_DMA_CHANNEL).N0SA_n = (uint32_t)oledMainImage;


	// ---- DMA Control Register Setting ----
	DCTRLn(OLED_SPI_DMA_CHANNEL) = 0;

	// ----- Transmission Side Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHCFG_n = 0b00000000001000000000001001101000 | (OLED_SPI_DMA_CHANNEL & 7);

	// ---- DMA Channel Interval Register Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHITVL_n = 0;

	// ---- DMA Channel Extension Register Setting ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHEXT_n = 0;

	DMACn(OLED_SPI_DMA_CHANNEL).N0DA_n = (uint32_t)&RSPI(SPI_CHANNEL_OLED_MAIN).SPDR.BYTE.LL;

	// ---- Software Reset and clear TC bit ----
	DMACn(OLED_SPI_DMA_CHANNEL).CHCTRL_n |= DMAC_CHCTRL_0S_SWRST | DMAC_CHCTRL_0S_CLRTC;

#if DELUGE_MODEL <= DELUGE_MODEL_CLASSIC
	DMACn(OLED_SPI_DMA_CHANNEL).N0DA_n = (uint32_t)&RSPI(SPI_CHANNEL_OLED_MAIN).SPDR.BYTE.LL;

	unsigned int dmarsTX = DMARS_FOR_RSPI_TX + (SPI_CHANNEL_OLED_MAIN << 2);
	setDMARS(OLED_SPI_DMA_CHANNEL, dmarsTX);
#endif

	R_INTC_RegistIntFunc(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL, oledTransferComplete);
	R_INTC_SetPriority(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL, 13); // Priority is not very important
	R_INTC_Enable(DMA_INTERRUPT_0 + OLED_SPI_DMA_CHANNEL);
}

int currentLogoPixel;


void clearMainImage() {
	//stopBlink();
	//stopScrollingAnimation();
	//memset(oledMainImage, 0, sizeof(oledMainImage));

	uint32_t* address = (uint32_t*)oledMainImage;
	while (address < (uint32_t*)((int)oledMainImage + sizeof(oledMainImage))) {
		*address = 0;
		address++;
	}
}

// Caller must ensure area doesn't go beyond edge of canvas.
// Inverts area inclusive of endY, I think...
void invertArea(int xMin, int width, int startY, int endY, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	int firstRowY = startY >> 3;
	int lastRowY = endY >> 3;

	uint8_t currentRowMask = (255 << (startY & 7));
	uint8_t lastRowMask = (255 >> (7 - (endY & 7)));

	// For each row
	int rowY;
	for (rowY = firstRowY; rowY <= lastRowY; rowY++) {

		if (rowY == lastRowY) currentRowMask &= lastRowMask;

		uint8_t* __restrict__ currentPos = &image[rowY][xMin];
		uint8_t* const endPos = currentPos + width;

		while (currentPos < endPos) {
			*currentPos ^= currentRowMask;
			currentPos++;
		}

		currentRowMask = 0xFF;
	}
}

void initiateSelectingOled() {
	bufferPICUart(248); // Select OLED

	//delayMS(10);
	//oledSelectingComplete();
	//return;

	//uartFlushIfNotSending(UART_ITEM_PIC);
	sendOledDataAfterMessage = 248;

	// Actual queue position gets moved along in oledSelectingComplete() when that gets called.
}

int spiDestinationSendingTo;

void sendSPITransferFromQueue() {

	spiTransferQueueCurrentlySending = true;

	spiDestinationSendingTo = spiTransferQueue[spiTransferQueueReadPos].destinationId;

	// If it's OLED data...
	if (spiDestinationSendingTo == 0) {
		initiateSelectingOled();
	}

	// Or if it's CV...
	else {
		//sendCVTransfer();
	}
}

void enqueueSPITransfer(int destinationId, uint8_t const* image) {

	// First check there isn't already an identical transfer enqueued.
	int readPosNow = spiTransferQueueReadPos;
	while (readPosNow != spiTransferQueueWritePos) {
		if (spiTransferQueue[readPosNow].destinationId == destinationId && spiTransferQueue[readPosNow].dataAddress == image) return;
		readPosNow = (readPosNow + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);
	}

	spiTransferQueue[spiTransferQueueWritePos].destinationId = destinationId;
	spiTransferQueue[spiTransferQueueWritePos].dataAddress = image;
	spiTransferQueueWritePos = (spiTransferQueueWritePos + 1) & (SPI_TRANSFER_QUEUE_SIZE - 1);

	// If DMA not currently sending, and our new entry is still in the queue (it didn't get sent inside an interrupt just now),
	// then send it now.
	if (!spiTransferQueueCurrentlySending && spiTransferQueueWritePos != spiTransferQueueReadPos) {
		sendSPITransferFromQueue();
	}
}

void sendMainImage() {

	/*
	uint8_t (*backgroundImage)[OLED_MAIN_WIDTH_PIXELS] = oledMainImage;

	if (numConsoleItems) {
		copyBackgroundAroundForeground(oledMainImage, oledMainConsoleImage, consoleMinX, consoleItems[numConsoleItems - 1].minY - 1, consoleMaxX, OLED_MAIN_HEIGHT_PIXELS - 1);
		backgroundImage = oledMainConsoleImage;
	}
	if (oledPopupWidth) {
		copyBackgroundAroundForeground(backgroundImage, oledMainPopupImage, popupMinX, popupMinY, popupMaxX, popupMaxY);
		backgroundImage = oledMainPopupImage;
	}
	*/
	enqueueSPITransfer(0, oledMainImage[0]);
}




uint8_t const logoPixels[] = {
		(1 << 4) | 0,
		(2 << 4) | 1,
		(3 << 4) | 2,
		(4 << 4) | 3,
		(4 << 4) | 0,
		(5 << 4) | 1,
		(6 << 4) | 2,
		(7 << 4) | 3,
		(0 << 4) | 2,
		(1 << 4) | 3,
		(2 << 4) | 4,
		(3 << 4) | 5,
		(5 << 4) | 5,
		(6 << 4) | 6,
		(7 << 4) | 7,
		(1 << 4) | 6,
		(2 << 4) | 7,
		(3 << 4) | 8,
		(4 << 4) | 9,
		(8 << 4) | 5,
		(9 << 4) | 6,
		(10 << 4) | 7,
		(5 << 4) | 7,
		(6 << 4) | 8,
		(7 << 4) | 9,
};

#define LOGO_END_X 11
#define LOGO_END_Y 10

#define LOGO_SCALE 3

bool_t drawNextLogoPixel() {
	monitorInputFromPIC();

	if (currentLogoPixel >= sizeof(logoPixels)) return false;

	int x = logoPixels[currentLogoPixel] >> 4;
	int y = logoPixels[currentLogoPixel] & 15;

	int xFromCenter = x - (LOGO_END_X >> 1) - 1;
	int yFromCentre = y - (LOGO_END_Y >> 1) - 1;

	int startY = OLED_MAIN_HEIGHT_PIXELS - (OLED_MAIN_VISIBLE_HEIGHT >> 1) + yFromCentre * LOGO_SCALE;

	invertArea((OLED_MAIN_WIDTH_PIXELS >> 1) + xFromCenter * LOGO_SCALE, LOGO_SCALE,
			startY, startY + LOGO_SCALE - 1, oledMainImage);

	sendMainImage();
	currentLogoPixel++;
	return true;
}



void oledInitMain() {

	enableAllModuleClocks();

	// Uart 1 for PIC / display
	uartInit(UART_CHANNEL_PIC, 31250);
	setPinMux(3, 15, 5); // TX
	setPinMux(1, 9, 3); // RX

	bufferPICUart(250); // D/C low
	bufferPICUart(247); // Enable OLED
	bufferPICUart(248); // Select OLED




	mtuEnableAccess();
	// Set up slow system timer - 33 ticks per millisecond (30.30303 microseconds per tick) on A1
	disableTimer(TIMER_SYSTEM_SLOW);
    timerControlSetup(TIMER_SYSTEM_SLOW, 0, 1024);
	enableTimer(TIMER_SYSTEM_SLOW);


	// SPI for CV
	R_RSPI_Create(SPI_CHANNEL_CV,
			10000000, // Higher than this would probably work... but let's stick to the OLED datasheet's spec of 100ns (10MHz).
			0, 8);
	R_RSPI_Start(SPI_CHANNEL_CV);
#if SPI_CHANNEL_CV == 1
	setPinMux(6, 12, 3);
	setPinMux(6, 14, 3);
	setPinMux(6, 13, 3);
#elif SPI_CHANNEL_CV == 0
	setPinMux(6, 0, 3); // CLK
	setPinMux(6, 2, 3);	// MOSI
#endif
	// If OLED sharing SPI channel, have to manually control SSL pin.
	setOutputState(6, 1, true);
	setPinAsOutput(6, 1);

	currentLogoPixel = 0;


	waitForPICWithTimeout(250, 10);
	waitForPICWithTimeout(247, 10);
	waitForPICWithTimeout(248, 10);

	oledMainInit();


	bufferPICUart(249); // Unselect OLED

	R_INTC_Init();
	enable_irq();
	enable_fiq();

	oledDMAInit();



	clearMainImage();




	{
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
		loop_num = region_size;// + 1; /* add one extra to be on the safe side */


		/* Point src_start to start of code in ROM */
		src_start = &__code_spibsc_init_2_src__;


		/* The DMAC controller could be configured here, but little point as
		 * we cannot continue until the copy completes. This is an option for
		 * future development if other initialisation tasks are required in
		 * the customers system.*/

		waitForPICWithTimeout(251, 10);
		waitForPICWithTimeout(249, 10);

		/* Copy the next load module. */
		for(i = 0; i < loop_num; i++)
		{
			if (!(i & 16383)) drawNextLogoPixel();
			*ram_start++ = *src_start++;
			monitorInputFromPIC();
		}
	}
}
