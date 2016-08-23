/*
 * mmc.c
 *
 *  Created on: 2/10/2015
 *      Author: Rohan
 */


/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "iodefine.h"
#include "r_typedefs.h"
#include "fatfs/diskio.h"
#include "fatfs/ff.h"




/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */



//int32_t cmd_sd_init(int32_t argc, char_t **argv);


BYTE diskStatus = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return diskStatus;
}



int32_t sdIntCallback(int32_t sd_port, int32_t cd)
{
    if ( sd_port == 0 )
    {
        if (cd)
        {
        	uartPrintln("SD Card insert in port0!\n");
        }
        else
        {
        	uartPrintln("SD Card extract in port0!\n");
        }
    }
    else{
        if (cd)
        {
        	uartPrintln("SD Card insert in port1!\n");
        }
        else
        {
        	uartPrintln("SD Card extract in port1!\n");
        }
    }

    return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

#include "drivers/sdhi/inc/sdif.h"
#include "drivers/sdhi/inc/sd_cfg.h"

#define SD_RW_BUFF_SIZE    (1 * 1024)
static uint32_t test_sd_rw_buff[ SD_RW_BUFF_SIZE / sizeof(uint32_t) ];


DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{

	uartPrintln("initializing disk");
	diskStatus = STA_NOINIT; // But then we'll try and initialize it now
	int error;

	uint32_t initializationWorkArea[SD_SIZE_OF_INIT / sizeof(uint32_t) ];

	//error = cmd_sd_init(0, 0);
	error = sd_init(1, SDCFG_IP1_BASE, &initializationWorkArea[0], SD_CD_SOCKET);
	if (error) {
		uartPrintln("fail");

		return STA_NOINIT;
	}


	uartPrintln("interrupt setup");
	/*
#ifdef SDCFG_CD_INT
    uartPrintln("set card detect by interrupt\n");
    error = sd_cd_int(1, SD_CD_INT_ENABLE, sdIntCallback);
#else
    uartPrintln("set card detect by polling\n");
    error = sd_cd_int(1, SD_CD_INT_DISABLE, 0);
#endif

*/

    error = sd_cd_int(1, SD_CD_INT_DISABLE, 0);

    if (error) {
		uartPrintln("fail2");
    	return STA_NOINIT;
    }

	uartPrintln("buffer setup");

    error = sd_set_buffer(1, &test_sd_rw_buff[0], SD_RW_BUFF_SIZE);
    if (error) {
		uartPrintln("fail3");
    	return STA_NOINIT;
    }










	uartPrintln("mounting");

	//error = cmd_sd_ioatt(0, 0);
    error = sd_mount(1, SDCFG_DRIVER_MODE, SD_VOLT_3_3);
    if (error) {
		uartPrintln("fail4");
		io_put_number(error);
    	return STA_NOINIT;
    }

	diskStatus = 0; // Disk is ok!

	uartPrintln("initializing success");

	return 0; // Success
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{

	BYTE err;

	err = sd_read_sect(1, buff, sector, count);

    if ( err == 0 ) {
    	return RES_OK;
    }
    else {
    	uartPrintln(err);
    	return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{

	BYTE err;

	err = sd_write_sect(1, buff, sector, count, 0x0001u);

    if ( err == 0 ) {
    	return RES_OK;
    }
    else {
    	uartPrintln(err);
    	return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case ATA :

		// Process of the command for the ATA drive

		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

DWORD get_fattime (void) {
	return 0;
}


void disk_timerproc (UINT msPassed)
{

}

