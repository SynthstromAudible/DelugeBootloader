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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : sd_dev_low.c
* $Rev: $
* $Date::                           $
* Device(s)    : RZ/A1H
* Tool-Chain   : 
*              : 
* OS           : 
* H/W Platform : RZ/A1H CPU Board
* Description  : RZ/A1H SD Driver Sample Program
* Operation    : 
* Limitations  : 
*******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_typedefs.h"
#include "iodefine.h"
#include "rza_io_regrw.h"
#include "devdrv_intc.h"
#include "../inc/sdif.h"
#include "../inc/sd_cfg.h"
#include "sd_dev_dmacdrv.h"
#include "mtu.h"

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
#define MTU_TIMER_CNT      33    /* P-phy = 33MHz         */
#define INT_LEVEL_SDHI     10    /* SDHI interrupt level  */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
#if 0
static uint8_t g_sdhi_priority_backup;
#endif

static int sddev_init_0(void);
static int sddev_init_1(void);
static int sddev_set_port_0(int mode);
static int sddev_set_port_1(int mode);

static int sddev_init_dma_0(unsigned long buff,unsigned long reg,long cnt,int dir);
static int sddev_init_dma_1(unsigned long buff,unsigned long reg,long cnt,int dir);

static int sddev_wait_dma_end_0(long cnt);
static int sddev_wait_dma_end_1(long cnt);

static int sddev_disable_dma_0(void);
static int sddev_disable_dma_1(void);

static void sddev_sd_int_handler_0(uint32_t int_sense);
static void sddev_sd_int_handler_1(uint32_t int_sense);
static void sddev_sdio_int_handler_0(uint32_t int_sense);
static void sddev_sdio_int_handler_1(uint32_t int_sense);
static void sddev_start_timer(int msec);
static void sddev_end_timer(void);
static int  sddev_check_timer(void);

/******************************************************************************
* Function Name: int sddev_cmd0_sdio_mount(int sd_port);
* Description  : Select to issue CMD0 before SDIO Mount
* Arguments    : none
* Return Value : SD_OK  : issue CMD0
*              : SD_ERR : not issue CMD0
******************************************************************************/
int sddev_cmd0_sdio_mount(int sd_port)
{
#ifdef SDCFG_IO
    return SD_ERR;
#else
    return SD_ERR;
#endif
}

/******************************************************************************
* Function Name: int sddev_cmd8_sdio_mount(int sd_port);
* Description  : Select to issue CMD8 before SDIO Mount
* Arguments    : none
* Return Value : SD_OK  : issue CMD8
*              : SD_ERR : not issue CMD8
******************************************************************************/
int sddev_cmd8_sdio_mount(int sd_port)
{
#ifdef SDCFG_IO
    return SD_OK;
#else
    return SD_ERR;
#endif
}

/******************************************************************************
* Function Name: int sddev_init(void);
* Description  : Initialize H/W to use SDHI
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_init(int sd_port)
{
    int ret;

    if (sd_port == 0)
    {
        ret = sddev_init_0();
    }
    else
    {
        ret = sddev_init_1();
    }

    return ret;
}

/******************************************************************************
* Function Name: static int sddev_init_0(void);
* Description  : Initialize H/W to use SDHI
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_init_0(void)
{
    volatile uint8_t dummy_buf;

    CPG.STBCR12 = 0xF0u;        /* [1], [1], [1], [1], SDHI00, SDHI01, SDHI10, SDHI11           */
    dummy_buf   = CPG.STBCR12;  /* (Dummy read)                                                 */

    // I deleted pin mux stuff from here - it was for the wrong pins anyway. Rohan

    sddev_set_port_0(SD_PORT_SERIAL);

#ifdef    SDCFG_HWINT
    R_INTC_RegistIntFunc(INTC_ID_SDHI0_0, sddev_sd_int_handler_0);
    R_INTC_SetPriority(INTC_ID_SDHI0_0, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI0_0);

    R_INTC_RegistIntFunc(INTC_ID_SDHI0_3, sddev_sd_int_handler_0);
    R_INTC_SetPriority(INTC_ID_SDHI0_3, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI0_3);

    R_INTC_RegistIntFunc(INTC_ID_SDHI0_1, sddev_sdio_int_handler_0);
    R_INTC_SetPriority(INTC_ID_SDHI0_1, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI0_1);
#endif

#ifdef    SDCFG_CD_INT
#endif

    /* ---- wait card detect ---- */
    sddev_start_timer(1000);            /* wait 1s */
    while (1)
    {
        if (sddev_check_timer() == SD_ERR)
        {
            break;
        }
    }
    sddev_end_timer();

    return SD_OK;
}

/******************************************************************************
* Function Name: static int sddev_init_1(void);
* Description  : Initialize H/W to use SDHI
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_init_1(void)
{
    volatile uint8_t dummy_buf;

    CPG.STBCR12 = 0xF0u;        /* [1], [1], [1], [1], SDHI00, SDHI01, SDHI10, SDHI11           */
    dummy_buf   = CPG.STBCR12;  /* (Dummy read)                                                 */

    // I deleted pin mux stuff from here - it was for the wrong pins anyway. Rohan

    sddev_set_port_1(SD_PORT_SERIAL);

#ifdef    SDCFG_HWINT
    R_INTC_RegistIntFunc(INTC_ID_SDHI1_0, sddev_sd_int_handler_1);
    R_INTC_SetPriority(INTC_ID_SDHI1_0, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI1_0);

    R_INTC_RegistIntFunc(INTC_ID_SDHI1_3, sddev_sd_int_handler_1);
    R_INTC_SetPriority(INTC_ID_SDHI1_3, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI1_3);

    R_INTC_RegistIntFunc(INTC_ID_SDHI1_1, sddev_sdio_int_handler_1);
    R_INTC_SetPriority(INTC_ID_SDHI1_1, INT_LEVEL_SDHI);
    R_INTC_Enable(INTC_ID_SDHI1_1);
#endif

#ifdef    SDCFG_CD_INT
#endif

    /* ---- wait card detect ---- */
    sddev_start_timer(1000);            /* wait 1s */
    while (1)
    {
        if (sddev_check_timer() == SD_ERR)
        {
            break;
        }
    }
    sddev_end_timer();

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_power_on(int sd_port);
* Description  : Power-on H/W to use SDHI
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_power_on(int sd_port)
{
    /* ---Power On SD ---- */

    /* ---- Wait for  SD Wake up ---- */
    sddev_start_timer(100);            /* wait 100ms */
    while (sddev_check_timer() == SD_OK)
    {
        /* wait */
    }
    sddev_end_timer();

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_power_off(int sd_port);
* Description  : Power-off H/W to use SDHI
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_power_off(int sd_port)
{
    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_read_data(int sd_port, unsigned char *buff,unsigned long reg_addr,long num);
* Description  : read from SDHI buffer FIFO
* Arguments    : unsigned char *buff    : buffer addrees to store reading datas
*              : unsigned long reg_addr : SDIP FIFO address
*              : long num               : counts to read(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_read_data(int sd_port, unsigned char *buff,unsigned long reg_addr,long num)
{
    long i;
    long cnt;
    unsigned long *reg;
    unsigned long *ptr_l;
    unsigned char *ptr_c;
    unsigned long tmp;

    reg = (unsigned long *)(reg_addr);

    cnt = (num / 4);
    if (((unsigned long)buff & 0x3) != 0)
    {
        ptr_c = (unsigned char *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            tmp = *reg;
            *ptr_c++ = (unsigned char)(tmp);
            *ptr_c++ = (unsigned char)(tmp >> 8);
            *ptr_c++ = (unsigned char)(tmp >> 16);
            *ptr_c++ = (unsigned char)(tmp >> 24);
        }

        cnt = (num % 4);
        if ( cnt != 0 )
        {
            tmp = *reg;
            for (i = cnt; i > 0 ; i--)
            {
                *ptr_c++ = (unsigned char)(tmp);
                tmp >>= 8;
            }
        }
    }
    else
    {
        ptr_l = (unsigned long *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            *ptr_l++ = *reg;
        }

        cnt = (num % 4);
        if ( cnt != 0 )
        {
            ptr_c = (unsigned char *)ptr_l;
            tmp = *reg;
            for (i = cnt; i > 0 ; i--)
            {
                *ptr_c++ = (unsigned char)(tmp);
                tmp >>= 8;
            }
        }
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_write_data(int sd_port, unsigned char *buff,unsigned long reg_addr,long num);
* Description  : write to SDHI buffer FIFO
* Arguments    : unsigned char *buff    : buffer addrees to store writting datas
*              : unsigned long reg_addr : SDIP FIFO address
*              : long num               : counts to write(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_write_data(int sd_port, unsigned char *buff,unsigned long reg_addr,long num)
{
    long i;
    unsigned long *reg = (unsigned long *)(reg_addr);
    unsigned long *ptr = (unsigned long *)buff;
    unsigned long tmp;

    /* dont care non 4byte allignment data */
    num += 3;
    num /= 4;
    if (((unsigned long)buff & 0x3) != 0)
    {
        for (i = num; i > 0 ; i--)
        {
            tmp  = *buff++ ;
            tmp |= *buff++ << 8;
            tmp |= *buff++ << 16;
            tmp |= *buff++ << 24;
            *reg = tmp;
        }
    }
    else
    {
        for (i = num; i > 0 ; i--)
        {
            *reg = *ptr++;
        }
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: unsigned int sddev_get_clockdiv(int sd_port, int clock);
* Description  : write to SDHI buffer FIFO
* Arguments    : int clock : request clock frequency
*              :   SD_CLK_50MHz
*              :   SD_CLK_25MHz
*              :   SD_CLK_20MHz
*              :   SD_CLK_10MHz
*              :   SD_CLK_5MHz
*              :   SD_CLK_1MHz
*              :   SD_CLK_400kHz
* Return Value : clock div value
*              :   SD_DIV_2 : 1/2   clock
*              :   SD_DIV_2 : 1/4   clock
*              :   SD_DIV_2 : 1/8   clock
*              :   SD_DIV_2 : 1/16  clock
*              :   SD_DIV_2 : 1/128 clock
*              :   SD_DIV_2 : 1/256 clock
******************************************************************************/
unsigned int sddev_get_clockdiv(int sd_port, int clock)
{
    unsigned int div;

    switch (clock)
    {
    case SD_CLK_50MHz:
        div = SD_DIV_2;        /* 66.6MHz/2 = 33.3MHz */
        break;
    case SD_CLK_25MHz:
        div = SD_DIV_4;        /* 66.6MHz/4 = 16.6MHz */
        break;
    case SD_CLK_20MHz:
        div = SD_DIV_4;        /* 66.6MHz/4 = 16.6MHz */
        break;
    case SD_CLK_10MHz:
        div = SD_DIV_8;        /* 66.6MHz/8 = 8.32MHz */
        break;
    case SD_CLK_5MHz:
        div = SD_DIV_16;       /* 66.6MHz/16 = 4.16MHz */
        break;
    case SD_CLK_1MHz:
        div = SD_DIV_128;      /* 66.6MHz/128 = 520kHz */
        break;
    case SD_CLK_400kHz:
        div = SD_DIV_256;      /* 66.6MHz/256 = 260kHz */
        break;
    default:
        div = SD_DIV_256;
        break;
    }

    return div;
}

/******************************************************************************
* Function Name: int sddev_set_port(int sd_port, int mode);
* Description  : setting ports to use MMCHI
* Arguments    : int mode : SD_PORT_PARALLEL : 4bit mode
*                         : SD_PORT_SERIAL   : 1bit mode
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_set_port(int sd_port, int mode)
{
    int ret;

    if (sd_port == 0)
    {
        ret = sddev_set_port_0(mode);
    }
    else
    {
        ret = sddev_set_port_1(mode);
    }

    return ret;
}

/******************************************************************************
* Function Name: int sddev_set_port_0(int mode);
* Description  : setting ports to use MMCHI
* Arguments    : int mode : SD_PORT_PARALLEL : 4bit mode
*                         : SD_PORT_SERIAL   : 1bit mode
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_set_port_0(int mode)
{
	return SD_OK; // This pin mux stuff is for the wrong pins anyway! Rohan
    if (mode == SD_PORT_SERIAL)
    {
        /* ---- P4_11 : SD_D0_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC411_SHIFT,   GPIO_PIBC4_PIBC411);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC411_SHIFT,   GPIO_PBDC4_PBDC411);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM411_SHIFT,       GPIO_PM4_PM411);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC411_SHIFT,     GPIO_PMC4_PMC411);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC411_SHIFT,   GPIO_PIPC4_PIPC411);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC411_SHIFT,   GPIO_PBDC4_PBDC411);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC411_SHIFT,     GPIO_PFC4_PFC411);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE411_SHIFT,   GPIO_PFCE4_PFCE411);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE411_SHIFT, GPIO_PFCAE4_PFCAE411);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC411_SHIFT,   GPIO_PIPC4_PIPC411);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC411_SHIFT,     GPIO_PMC4_PMC411);


        /* ---- P4_12 : SD_CLK_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC412_SHIFT,   GPIO_PIBC4_PIBC412);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC412_SHIFT,   GPIO_PBDC4_PBDC412);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM412_SHIFT,       GPIO_PM4_PM412);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC412_SHIFT,     GPIO_PMC4_PMC412);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC412_SHIFT,   GPIO_PIPC4_PIPC412);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Disable                   */
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC412_SHIFT,   GPIO_PBDC4_PBDC412);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC412_SHIFT,     GPIO_PFC4_PFC412);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE412_SHIFT,   GPIO_PFCE4_PFCE412);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE412_SHIFT, GPIO_PFCAE4_PFCAE412);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC412_SHIFT,   GPIO_PIPC4_PIPC412);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC412_SHIFT,     GPIO_PMC4_PMC412);

        /* ---- P4_13 : SD_CMD_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC413_SHIFT,   GPIO_PIBC4_PIBC413);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC413_SHIFT,   GPIO_PBDC4_PBDC413);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM413_SHIFT,       GPIO_PM4_PM413);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC413_SHIFT,     GPIO_PMC4_PMC413);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC413_SHIFT,   GPIO_PIPC4_PIPC413);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC413_SHIFT,   GPIO_PBDC4_PBDC413);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC413_SHIFT,     GPIO_PFC4_PFC413);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE413_SHIFT,   GPIO_PFCE4_PFCE413);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE413_SHIFT, GPIO_PFCAE4_PFCAE413);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC413_SHIFT,   GPIO_PIPC4_PIPC413);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC413_SHIFT,     GPIO_PMC4_PMC413);
    }
    else if (mode == SD_PORT_PARALLEL)
    {
        /* ---- P4_10 : SD_D1_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC410_SHIFT,   GPIO_PIBC4_PIBC410);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC410_SHIFT,   GPIO_PBDC4_PBDC410);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM410_SHIFT,       GPIO_PM4_PM410);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC410_SHIFT,     GPIO_PMC4_PMC410);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC410_SHIFT,   GPIO_PIPC4_PIPC410);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC410_SHIFT,   GPIO_PBDC4_PBDC410);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC410_SHIFT,     GPIO_PFC4_PFC410);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE410_SHIFT,   GPIO_PFCE4_PFCE410);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE410_SHIFT, GPIO_PFCAE4_PFCAE410);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC410_SHIFT,   GPIO_PIPC4_PIPC410);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC410_SHIFT,     GPIO_PMC4_PMC410);

        /* ---- P4_11 : SD_D0_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC411_SHIFT,   GPIO_PIBC4_PIBC411);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC411_SHIFT,   GPIO_PBDC4_PBDC411);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM411_SHIFT,       GPIO_PM4_PM411);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC411_SHIFT,     GPIO_PMC4_PMC411);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC411_SHIFT,   GPIO_PIPC4_PIPC411);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC411_SHIFT,   GPIO_PBDC4_PBDC411);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC411_SHIFT,     GPIO_PFC4_PFC411);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE411_SHIFT,   GPIO_PFCE4_PFCE411);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE411_SHIFT, GPIO_PFCAE4_PFCAE411);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC411_SHIFT,   GPIO_PIPC4_PIPC411);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC411_SHIFT,     GPIO_PMC4_PMC411);

        /* ---- P4_12 : SD_CLK_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC412_SHIFT,   GPIO_PIBC4_PIBC412);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC412_SHIFT,   GPIO_PBDC4_PBDC412);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM412_SHIFT,       GPIO_PM4_PM412);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC412_SHIFT,     GPIO_PMC4_PMC412);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC412_SHIFT,   GPIO_PIPC4_PIPC412);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Disable                   */
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC412_SHIFT,   GPIO_PBDC4_PBDC412);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC412_SHIFT,     GPIO_PFC4_PFC412);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE412_SHIFT,   GPIO_PFCE4_PFCE412);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE412_SHIFT, GPIO_PFCAE4_PFCAE412);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC412_SHIFT,   GPIO_PIPC4_PIPC412);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC412_SHIFT,     GPIO_PMC4_PMC412);

        /* ---- P4_13 : SD_CMD_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC413_SHIFT,   GPIO_PIBC4_PIBC413);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC413_SHIFT,   GPIO_PBDC4_PBDC413);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM413_SHIFT,       GPIO_PM4_PM413);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC413_SHIFT,     GPIO_PMC4_PMC413);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC413_SHIFT,   GPIO_PIPC4_PIPC413);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC413_SHIFT,   GPIO_PBDC4_PBDC413);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC413_SHIFT,     GPIO_PFC4_PFC413);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE413_SHIFT,   GPIO_PFCE4_PFCE413);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE413_SHIFT, GPIO_PFCAE4_PFCAE413);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC413_SHIFT,   GPIO_PIPC4_PIPC413);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC413_SHIFT,     GPIO_PMC4_PMC413);

        /* ---- P4_14 : SD_D3_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC414_SHIFT,   GPIO_PIBC4_PIBC414);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC414_SHIFT,   GPIO_PBDC4_PBDC414);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM414_SHIFT,       GPIO_PM4_PM414);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC414_SHIFT,     GPIO_PMC4_PMC414);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC414_SHIFT,   GPIO_PIPC4_PIPC414);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC414_SHIFT,   GPIO_PBDC4_PBDC414);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC414_SHIFT,     GPIO_PFC4_PFC414);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE414_SHIFT,   GPIO_PFCE4_PFCE414);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE414_SHIFT, GPIO_PFCAE4_PFCAE414);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC414_SHIFT,   GPIO_PIPC4_PIPC414);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC414_SHIFT,     GPIO_PMC4_PMC414);

        /* ---- P4_15 : SD_D2_0 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC4,  0, GPIO_PIBC4_PIBC415_SHIFT,   GPIO_PIBC4_PIBC415);
        rza_io_reg_write_16(&GPIO.PBDC4,  0, GPIO_PBDC4_PBDC415_SHIFT,   GPIO_PBDC4_PBDC415);
        rza_io_reg_write_16(&GPIO.PM4,    1, GPIO_PM4_PM415_SHIFT,       GPIO_PM4_PM415);
        rza_io_reg_write_16(&GPIO.PMC4,   0, GPIO_PMC4_PMC415_SHIFT,     GPIO_PMC4_PMC415);
        rza_io_reg_write_16(&GPIO.PIPC4,  0, GPIO_PIPC4_PIPC415_SHIFT,   GPIO_PIPC4_PIPC415);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 3rd multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC4,  1, GPIO_PBDC4_PBDC415_SHIFT,   GPIO_PBDC4_PBDC415);
        rza_io_reg_write_16(&GPIO.PFC4,   0, GPIO_PFC4_PFC415_SHIFT,     GPIO_PFC4_PFC415);
        rza_io_reg_write_16(&GPIO.PFCE4,  1, GPIO_PFCE4_PFCE415_SHIFT,   GPIO_PFCE4_PFCE415);
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE415_SHIFT, GPIO_PFCAE4_PFCAE415);
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC415_SHIFT,   GPIO_PIPC4_PIPC415);
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC415_SHIFT,     GPIO_PMC4_PMC415);
    }
    else
    {
        return SD_ERR;
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_set_port_1(int mode);
* Description  : setting ports to use MMCHI
* Arguments    : int mode : SD_PORT_PARALLEL : 4bit mode
*                         : SD_PORT_SERIAL   : 1bit mode
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_set_port_1(int mode)
{
	return SD_OK; // This pin mux stuff is for the wrong pins anyway! Rohan
    if (mode == SD_PORT_SERIAL)
    {
        /* ---- P3_11 : SD_D0_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC311_SHIFT,   GPIO_PIBC3_PIBC311);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC311_SHIFT,   GPIO_PBDC3_PBDC311);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM311_SHIFT,       GPIO_PM3_PM311);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC311_SHIFT,     GPIO_PMC3_PMC311);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC311_SHIFT,   GPIO_PIPC3_PIPC311);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC311_SHIFT,   GPIO_PBDC3_PBDC311);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC311_SHIFT,     GPIO_PFC3_PFC311);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE311_SHIFT,   GPIO_PFCE3_PFCE311);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE311_SHIFT, GPIO_PFCAE3_PFCAE311);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC311_SHIFT,   GPIO_PIPC3_PIPC311);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC311_SHIFT,     GPIO_PMC3_PMC311);

        /* ---- P3_12 : SD_CLK_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC312_SHIFT,   GPIO_PIBC3_PIBC312);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC312_SHIFT,   GPIO_PBDC3_PBDC312);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM312_SHIFT,       GPIO_PM3_PM312);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC312_SHIFT,     GPIO_PMC3_PMC312);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC312_SHIFT,   GPIO_PIPC3_PIPC312);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Disable                   */
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC312_SHIFT,   GPIO_PBDC3_PBDC312);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC312_SHIFT,     GPIO_PFC3_PFC312);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE312_SHIFT,   GPIO_PFCE3_PFCE312);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE312_SHIFT, GPIO_PFCAE3_PFCAE312);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC312_SHIFT,   GPIO_PIPC3_PIPC312);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC312_SHIFT,     GPIO_PMC3_PMC312);

        /* ---- P3_13 : SD_CMD_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC313_SHIFT,   GPIO_PIBC3_PIBC313);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC313_SHIFT,   GPIO_PBDC3_PBDC313);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM313_SHIFT,       GPIO_PM3_PM313);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC313_SHIFT,     GPIO_PMC3_PMC313);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC313_SHIFT,   GPIO_PIPC3_PIPC313);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Disable                   */
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC313_SHIFT,   GPIO_PBDC3_PBDC313);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC313_SHIFT,     GPIO_PFC3_PFC313);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE313_SHIFT,   GPIO_PFCE3_PFCE313);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE313_SHIFT, GPIO_PFCAE3_PFCAE313);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC313_SHIFT,   GPIO_PIPC3_PIPC313);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC313_SHIFT,     GPIO_PMC3_PMC313);
    }
    else if (mode == SD_PORT_PARALLEL)
    {
        /* ---- P3_10 : SD_D1_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC310_SHIFT,   GPIO_PIBC3_PIBC310);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC310_SHIFT,   GPIO_PBDC3_PBDC310);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM310_SHIFT,       GPIO_PM3_PM310);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC310_SHIFT,     GPIO_PMC3_PMC310);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC310_SHIFT,   GPIO_PIPC3_PIPC310);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC310_SHIFT,   GPIO_PBDC3_PBDC310);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC310_SHIFT,     GPIO_PFC3_PFC310);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE310_SHIFT,   GPIO_PFCE3_PFCE310);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE310_SHIFT, GPIO_PFCAE3_PFCAE310);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC310_SHIFT,   GPIO_PIPC3_PIPC310);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC310_SHIFT,     GPIO_PMC3_PMC310);

        /* ---- P3_11 : SD_D0_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC311_SHIFT,   GPIO_PIBC3_PIBC311);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC311_SHIFT,   GPIO_PBDC3_PBDC311);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM311_SHIFT,       GPIO_PM3_PM311);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC311_SHIFT,     GPIO_PMC3_PMC311);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC311_SHIFT,   GPIO_PIPC3_PIPC311);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                   */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC311_SHIFT,   GPIO_PBDC3_PBDC311);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC311_SHIFT,     GPIO_PFC3_PFC311);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE311_SHIFT,   GPIO_PFCE3_PFCE311);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE311_SHIFT, GPIO_PFCAE3_PFCAE311);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC311_SHIFT,   GPIO_PIPC3_PIPC311);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC311_SHIFT,     GPIO_PMC3_PMC311);

        /* ---- P3_12 : SD_CLK_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC312_SHIFT,   GPIO_PIBC3_PIBC312);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC312_SHIFT,   GPIO_PBDC3_PBDC312);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM312_SHIFT,       GPIO_PM3_PM312);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC312_SHIFT,     GPIO_PMC3_PMC312);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC312_SHIFT,   GPIO_PIPC3_PIPC312);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC312_SHIFT,   GPIO_PBDC3_PBDC312);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC312_SHIFT,     GPIO_PFC3_PFC312);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE312_SHIFT,   GPIO_PFCE3_PFCE312);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE312_SHIFT, GPIO_PFCAE3_PFCAE312);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC312_SHIFT,   GPIO_PIPC3_PIPC312);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC312_SHIFT,     GPIO_PMC3_PMC312);

        /* ---- P3_13 : SD_CMD_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC313_SHIFT,   GPIO_PIBC3_PIBC313);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC313_SHIFT,   GPIO_PBDC3_PBDC313);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM313_SHIFT,       GPIO_PM3_PM313);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC313_SHIFT,     GPIO_PMC3_PMC313);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC313_SHIFT,   GPIO_PIPC3_PIPC313);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC313_SHIFT,   GPIO_PBDC3_PBDC313);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC313_SHIFT,     GPIO_PFC3_PFC313);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE313_SHIFT,   GPIO_PFCE3_PFCE313);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE313_SHIFT, GPIO_PFCAE3_PFCAE313);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC313_SHIFT,   GPIO_PIPC3_PIPC313);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC313_SHIFT,     GPIO_PMC3_PMC313);

        /* ---- P3_14 : SD_D3_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC314_SHIFT,   GPIO_PIBC3_PIBC314);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC314_SHIFT,   GPIO_PBDC3_PBDC314);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM314_SHIFT,       GPIO_PM3_PM314);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC314_SHIFT,     GPIO_PMC3_PMC314);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC314_SHIFT,   GPIO_PIPC3_PIPC314);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC314_SHIFT,   GPIO_PBDC3_PBDC314);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC314_SHIFT,     GPIO_PFC3_PFC314);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE314_SHIFT,   GPIO_PFCE3_PFCE314);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE314_SHIFT, GPIO_PFCAE3_PFCAE314);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC314_SHIFT,   GPIO_PIPC3_PIPC314);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC314_SHIFT,     GPIO_PMC3_PMC314);

        /* ---- P3_15 : SD_D2_1 ---- */
        /* Port initialize */
        rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC315_SHIFT,   GPIO_PIBC3_PIBC315);
        rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC315_SHIFT,   GPIO_PBDC3_PBDC315);
        rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM315_SHIFT,       GPIO_PM3_PM315);
        rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC315_SHIFT,     GPIO_PMC3_PMC315);
        rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC315_SHIFT,   GPIO_PIPC3_PIPC315);
        /* Port mode : Multiplex mode                     */
        /* Port function setting : 7th multiplex function */
        /* I/O control mode : Peripheral function         */
        /* Bidirectional mode : Enable                    */
        rza_io_reg_write_16(&GPIO.PBDC3,  1, GPIO_PBDC3_PBDC315_SHIFT,   GPIO_PBDC3_PBDC315);
        rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC315_SHIFT,     GPIO_PFC3_PFC315);
        rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE315_SHIFT,   GPIO_PFCE3_PFCE315);
        rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE315_SHIFT, GPIO_PFCAE3_PFCAE315);
        rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC315_SHIFT,   GPIO_PIPC3_PIPC315);
        rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC315_SHIFT,     GPIO_PMC3_PMC315);
    }
    else
    {
        return SD_ERR;
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_int_wait(int sd_port, int time);
* Description  : Waitting for SDHI Interrupt
* Arguments    : int time : time out value to wait interrupt
* Return Value : get interrupt : SD_OK
*              : time out      : SD_ERR
******************************************************************************/
int sddev_int_wait(int sd_port, int time)
{
    int loop;

    if (time > (0x0000ffff / MTU_TIMER_CNT))
    {
        /* @1000ms */
        loop = (time / 1000);
        if ( (time % 1000) != 0 )
        {
            loop++;
        }
        time = 1000;
    }
    else
    {
        loop = 1;
    }

    do{
        sddev_start_timer(time);

        while (1)
        {
            /* interrupt generated? */
            if (sd_check_int(sd_port) == SD_OK)
            {
                sddev_end_timer();
                return SD_OK;
            }
            /* detect timeout? */
            if (sddev_check_timer() == SD_ERR)
            {
                break;
            }
        }

        loop--;
        if ( loop <= 0 )
        {
            break;
        }

    } while (1);

    sddev_end_timer();

    return SD_ERR;
}

/******************************************************************************
* Function Name: int sddev_init_dma(unsigned long buff,unsigned long reg,long cnt,int dir);
* Description  : Initialize DMAC to transfer data from SDHI FIFO
* Arguments    : unsigned long buff : buffer addrees to transfer datas
*              : unsigned long reg  : SDIP FIFO address
*              : long cnt           : counts to transfer(unit:byte)
*              : int dir            : direction to transfer
*              :                    :   0 : FIFO -> buffer
*              :                    :   1 : buffer -> FIFO
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_init_dma(int sd_port, unsigned long buff,unsigned long reg,long cnt,int dir)
{
    int ret = SD_ERR;

    if ( sd_port == 0 )
    {
        ret = sddev_init_dma_0(buff, reg, cnt, dir);
    }
    else if ( sd_port == 1 )
    {
        ret = sddev_init_dma_1(buff, reg, cnt, dir);
    }

    return ret;
}

/******************************************************************************
* Function Name: static int sddev_init_dma_0(unsigned long buff,unsigned long reg,long cnt,int dir);
* Description  : Initialize DMAC to transfer data from SDHI FIFO
* Arguments    : unsigned long buff : buffer addrees to transfer datas
*              : unsigned long reg  : SDIP FIFO address
*              : long cnt           : counts to transfer(unit:byte)
*              : int dir            : direction to transfer
*              :                    :   0 : FIFO -> buffer
*              :                    :   1 : buffer -> FIFO
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_init_dma_0(unsigned long buff,unsigned long reg,long cnt,int dir)
{
#ifdef    SDCFG_TRNS_DMA
    dmac_transinfo_t    trans_info;
    uint32_t            request_factor = DMAC_REQ_SDHI_0_RX;
    int32_t             ret;

    trans_info.count     = (uint32_t)cnt;
    #ifdef SDCFG_TRANS_DMA_64
    if ( (cnt % 64) != 0 )
    {
        trans_info.src_size  = DMAC_TRANS_SIZE_32;
        trans_info.dst_size  = DMAC_TRANS_SIZE_32;
        if ( reg & 0x0000003f )
        {
            trans_info.src_size  = DMAC_TRANS_SIZE_32;
            trans_info.dst_size  = DMAC_TRANS_SIZE_32;
        }
    }
    else
    {
        trans_info.src_size  = DMAC_TRANS_SIZE_512;
        trans_info.dst_size  = DMAC_TRANS_SIZE_512;
    }
    #else
    trans_info.src_size  = DMAC_TRANS_SIZE_32;
    trans_info.dst_size  = DMAC_TRANS_SIZE_32;
    #endif

    if ( dir == 0 )
    {
        request_factor       = DMAC_REQ_SDHI_0_RX;
        trans_info.src_addr  = (uint32_t)reg;
        trans_info.dst_addr  = (uint32_t)buff;
        trans_info.saddr_dir = DMAC_TRANS_ADR_NO_INC;
        trans_info.daddr_dir = DMAC_TRANS_ADR_INC;
    }
    else if ( dir == 1 )
    {
        request_factor       = DMAC_REQ_SDHI_0_TX;
        trans_info.src_addr  = (uint32_t)buff;
        trans_info.dst_addr  = (uint32_t)reg;
        trans_info.saddr_dir = DMAC_TRANS_ADR_INC;
        trans_info.daddr_dir = DMAC_TRANS_ADR_NO_INC;
    }

    sd_DMAC1_PeriReqInit(    (const dmac_transinfo_t *)&trans_info,
                            DMAC_MODE_REGISTER,
                            DMAC_SAMPLE_SINGLE,
                            request_factor,
                            0    );        /* Dont care DMAC_REQ_REQD is setting in usb0_host_DMAC1_PeriReqInit() */

    ret = sd_DMAC1_Open(DMAC_REQ_MODE_PERI);
    if ( ret != 0 )
    {
        printf("DMAC1 Open error!!\n");
        return SD_ERR;
    }
#endif

    return SD_OK;
}

/******************************************************************************
* Function Name: static int sddev_init_dma_1(unsigned long buff,unsigned long reg,long cnt,int dir);
* Description  : Initialize DMAC to transfer data from SDHI FIFO
* Arguments    : unsigned long buff : buffer addrees to transfer datas
*              : unsigned long reg  : SDIP FIFO address
*              : long cnt           : counts to transfer(unit:byte)
*              : int dir            : direction to transfer
*              :                    :   0 : FIFO -> buffer
*              :                    :   1 : buffer -> FIFO
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_init_dma_1(unsigned long buff,unsigned long reg,long cnt,int dir)
{
#ifdef    SDCFG_TRNS_DMA
    dmac_transinfo_t    trans_info;
    uint32_t            request_factor  = DMAC_REQ_SDHI_0_RX;
    int32_t             ret;

    trans_info.count     = (uint32_t)cnt;
    #ifdef SDCFG_TRANS_DMA_64
    if ( (cnt % 64) != 0 )
    {
        trans_info.src_size  = DMAC_TRANS_SIZE_32;
        trans_info.dst_size  = DMAC_TRANS_SIZE_32;
        if ( reg & 0x0000003f )
        {
            trans_info.src_size  = DMAC_TRANS_SIZE_32;
            trans_info.dst_size  = DMAC_TRANS_SIZE_32;
        }
    }
    else
    {
        trans_info.src_size  = DMAC_TRANS_SIZE_512;
        trans_info.dst_size  = DMAC_TRANS_SIZE_512;
    }
    #else
    trans_info.src_size  = DMAC_TRANS_SIZE_32;
    trans_info.dst_size  = DMAC_TRANS_SIZE_32;
    #endif

    if ( dir == 0 )
    {
        request_factor       = DMAC_REQ_SDHI_1_RX;
        trans_info.src_addr  = (uint32_t)reg;
        trans_info.dst_addr  = (uint32_t)buff;
        trans_info.saddr_dir = DMAC_TRANS_ADR_NO_INC;
        trans_info.daddr_dir = DMAC_TRANS_ADR_INC;
    }
    else if ( dir == 1 )
    {
        request_factor       = DMAC_REQ_SDHI_1_TX;
        trans_info.src_addr  = (uint32_t)buff;
        trans_info.dst_addr  = (uint32_t)reg;
        trans_info.saddr_dir = DMAC_TRANS_ADR_INC;
        trans_info.daddr_dir = DMAC_TRANS_ADR_NO_INC;
    }

    sd_DMAC2_PeriReqInit(    (const dmac_transinfo_t *)&trans_info,
                            DMAC_MODE_REGISTER,
                            DMAC_SAMPLE_SINGLE,
                            request_factor,
                            0    );        /* Dont care DMAC_REQ_REQD is setting in usb0_host_DMAC1_PeriReqInit() */

    ret = sd_DMAC2_Open(DMAC_REQ_MODE_PERI);
    if ( ret != 0 )
    {
        printf("DMAC1 Open error!!\n");
        return SD_ERR;
    }
#endif

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_wait_dma_end(int sd_port, long cnt);
* Description  : Wait to complete DMAC transfer
* Arguments    : long cnt           : counts to transfer(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_wait_dma_end(int sd_port, long cnt)
{
    int ret = SD_ERR;

    if ( sd_port == 0 )
    {
        ret = sddev_wait_dma_end_0(cnt);
    }
    else if ( sd_port == 1 )
    {
        ret = sddev_wait_dma_end_1(cnt);
    }

    return ret;
}

/******************************************************************************
* Function Name: static int sddev_wait_dma_end_0(long cnt);
* Description  : Wait to complete DMAC transfer
* Arguments    : long cnt           : counts to transfer(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_wait_dma_end_0(long cnt)
{
#ifdef    SDCFG_TRNS_DMA
    int loop;
    int time;

    time = (cnt / 512);
    time = ((time * 1000) / 1024);
    if (time < 1000)
    {
        time = 1000;
    }

    if (time > (0x0000ffff / MTU_TIMER_CNT))
    {
        /* @1000ms */
        loop = (time / 1000);
        if ( (time % 1000) != 0 )
        {
            loop++;
        }
        time = 1000;
    }
    else
    {
        loop = 1;
    }

    do{
        sddev_start_timer(time);

        while (1)
        {
            /* get end flag? */
            if ( sd_DMAC1_Get_Endflag() == 1 )
            {
                sddev_end_timer();
                return SD_OK;
            }
            /* detect timeout? */
            if (sddev_check_timer() == SD_ERR)
            {
                break;
            }
        }

        loop--;
        if ( loop <= 0 )
        {
            break;
        }

    } while (1);

    sddev_end_timer();

    return SD_ERR;
#else
    return SD_OK;

#endif
}

/******************************************************************************
* Function Name: static int sddev_wait_dma_end_1(long cnt);
* Description  : Wait to complete DMAC transfer
* Arguments    : long cnt           : counts to transfer(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_wait_dma_end_1(long cnt)
{
#ifdef    SDCFG_TRNS_DMA
    int loop;
    int time;

    time = (cnt / 512);
    time = ((time * 1000) / 1024);
    if (time < 1000)
    {
        time = 1000;
    }

    if (time > (0x0000ffff / MTU_TIMER_CNT))
    {
        /* @1000ms */
        loop = (time / 1000);
        if ( (time % 1000) != 0 )
        {
            loop++;
        }
        time = 1000;
    }
    else
    {
        loop = 1;
    }

    do{
        sddev_start_timer(time);

        while (1)
        {
            /* get end flag? */
            if ( sd_DMAC2_Get_Endflag() == 1 )
            {
                sddev_end_timer();
                return SD_OK;
            }
            /* detect timeout? */
            if (sddev_check_timer() == SD_ERR)
            {
                break;
            }
        }

        loop--;
        if ( loop <= 0 )
        {
            break;
        }

    } while (1);

    sddev_end_timer();

    return SD_ERR;
#else
    return SD_OK;

#endif
}

/******************************************************************************
* Function Name: int sddev_disable_dma(int sd_port);
* Description  : Disable DMAC transfer
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_disable_dma(int sd_port)
{
    int ret = SD_ERR;

    if ( sd_port == 0 )
    {
        ret = sddev_disable_dma_0();
    }
    else
    {
        ret = sddev_disable_dma_1();
    }

    return ret;
}

/******************************************************************************
* Function Name: static int sddev_disable_dma_0(void);
* Description  : Disable DMAC transfer
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_disable_dma_0(void)
{
#ifdef    SDCFG_TRNS_DMA
    uint32_t    remain;

    sd_DMAC1_Close(&remain);
#endif
    return SD_OK;
}

/******************************************************************************
* Function Name: staticint sddev_disable_dma_1(void);
* Description  : Disable DMAC transfer
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int sddev_disable_dma_1(void)
{
#ifdef    SDCFG_TRNS_DMA
    uint32_t    remain;

    sd_DMAC2_Close(&remain);
#endif
    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_loc_cpu(int sd_port);
* Description  : lock cpu to disable interrupt
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_loc_cpu(int sd_port)
{
#if 0
    R_INTC_GetMaskLevel(&g_sdhi_priority_backup);
    R_INTC_SetMaskLevel(0);
#endif

    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_unl_cpu(int sd_port);
* Description  : unlock cpu to enable interrupt
* Arguments    : none
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int sddev_unl_cpu(int sd_port)
{
#if 0
    R_INTC_SetMaskLevel(g_sdhi_priority_backup);
#endif
    return SD_OK;
}

/******************************************************************************
* Function Name: int sddev_finalize(int sd_port);
* Description  : finalize SDHI
* Arguments    : none
* Return Value : none
******************************************************************************/
int sddev_finalize(int sd_port)
{
    return SD_OK;
}

/******************************************************************************
* Function Name: static void sddev_sd_int_handler_0(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDHI0_0,INTC_ID_SDHI0_3)
* Arguments    : Interrupt mode
* Return Value : none
******************************************************************************/
static void sddev_sd_int_handler_0(uint32_t int_sense)
{
    sd_int_handler(0);
}

/******************************************************************************
* Function Name: static void sddev_sd_int_handler_1(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDHI0_0,INTC_ID_SDHI0_3)
* Arguments    : Interrupt mode
* Return Value : none
******************************************************************************/
static void sddev_sd_int_handler_1(uint32_t int_sense)
{
    sd_int_handler(1);
}

/******************************************************************************
* Function Name: static void sddev_sdio_int_handler_0(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDHI0_1)
* Arguments    : Interrupt mode
* Return Value : none
******************************************************************************/
static void sddev_sdio_int_handler_0(uint32_t int_sense)
{
    sdio_int_handler(0);
}

/******************************************************************************
* Function Name: static void sddev_sdio_int_handler_1(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDHI1_1)
* Arguments    : Interrupt mode
* Return Value : none
******************************************************************************/
static void sddev_sdio_int_handler_1(uint32_t int_sense)
{
    sdio_int_handler(1);
}

/******************************************************************************
* Function Name: static void sddev_start_timer(int msec);
* Description  : start timer
* Arguments    : 
* Return Value : none
******************************************************************************/
/******************************************************************************
* Function Name: static void sddev_start_timer(int msec);
* Description  : start timer
* Arguments    :
* Return Value : none
******************************************************************************/
uint16_t stopTime;
void sddev_start_timer(int msec)
{
	stopTime = *TCNT[TIMER_SYSTEM_SLOW] + msToSlowTimerCount(msec);
}

/******************************************************************************
* Function Name: static void sddev_end_timer(void);
* Description  : end timer
* Arguments    :
* Return Value : none
******************************************************************************/
void sddev_end_timer(void)
{
}

/******************************************************************************
* Function Name: static int sddev_check_timer(void);
* Description  : check
* Arguments    :
* Return Value : t
******************************************************************************/
int sddev_check_timer(void)
{

	uint16_t howFarAbove = *TCNT[TIMER_SYSTEM_SLOW] - stopTime;

    if (howFarAbove < 16384)
    {
        return SD_ERR;
    }

    return SD_OK;
}

/* End of File */
