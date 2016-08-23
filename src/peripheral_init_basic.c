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
* File Name     : peripheral_init_basic.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Sample Program - Initialise peripheral function sample
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
/* Default  type definition header */
#include "r_typedefs.h"
/* Common Driver header */
#include "bsc_userdef.h"
/* I/O Register root header */
#include "iodefine.h"
/* Peripheral startup interface header */
#include "peripheral_init_basic.h"
/* Low level register read/write header */
#include "rza_io_regrw.h"


/*******************************************************************************
Private global variables and functions
*******************************************************************************/
static void CPG_Init(void);
static void CS0_PORT_Init(void);


/*******************************************************************************
* Function Name: Peripheral_Basic_Init
* Description  : Calls the CPG_Init and the CS0_PORT_Init of sample functions
*              : and the BSC_Init of API function. Sets the CPG, enables
*              : writing to the data-retention RAM area, and executes the 
*              : port settings for the CS0 and the CS1 spaces and the BSC 
*              : setting.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void Peripheral_Basic_Init (void)
{
    /* ==== CPG setting ====*/
    CPG_Init();

    /* ==== Port setting ==== */
    //CS0_PORT_Init();

    /* ==== BSC setting ==== */
    /* Initialise rest of BSC later in startup sequence */
    //BSC_Init((uint8_t)(BSC_AREA_CS0 | BSC_AREA_CS1));
}

/*******************************************************************************
* Function Name: CPG_Init
* Description  : Executes initial setting for the CPG.
*              : In the sample code, the internal clock ratio is set to be 
*              : I:G:B:P1:P0 = 30:20:10:5:5/2 in the state that the 
*              : clock mode is 0. The frequency is set to be as below when the
*              : input clock is 13.33MHz.
*              : CPU clock (I clock)              : 400MHz
*              : Image processing clock (G clock) : 266.67MHz
*              : Internal bus clock (B clock)     : 133.33MHz
*              : Peripheral clock1 (P1 clock)     : 66.67MHz
*              : Peripheral clock0 (P0 clock)     : 33.33MHz
*              : Sets the data-retention RAM area (H'2000 0000 to H'2001 FFFF)
*              : to be enabled for writing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void CPG_Init (void)
{
    volatile uint32_t dummy = 0;

    UNUSED_VARIABLE(dummy);

    /* standby_mode_en bit of Power Control Register setting */
    (*(volatile uint32_t *)(0x3fffff80)) = 0x00000001;
    dummy = (*(volatile uint32_t *)(0x3fffff80));

    /* ==== CPG Settings ==== */

    /* PLL(x30), I:G:B:P1:P0 = 30:20:10:5:5/2 */
    CPG.FRQCR  = 0x1035u;

    /* CKIO:Output at time usually output     *
     * when bus right is opened output at     *
     * standby "L"                            *
	 * Clockin  = 13.33MHz, CKIO = 66.67MHz,  *
	 * I  Clock = 400.00MHz,                  *
	 * G  Clock = 266.67MHz,                  *
	 * B  Clock = 133.33MHz,                  *
	 * P1 Clock =  66.67MHz,                  *
	 * P0 Clock =  33.33MHz                   */
    CPG.FRQCR2 = 0x0001u;

    /* ----  Writing to On-Chip Data-Retention RAM is enabled. ---- */
    CPG.SYSCR3 = 0x0Fu;
    dummy = CPG.SYSCR3;
}

/*******************************************************************************
* Function Name: CS0_PORT_Init
* Description  : Sets the PORT multiplexed pin to use the CS0 and the CS1 
*              : spaces. In this sample code, the PORT setting is executed to 
*              : use the NOR flash memory in the CS0 and the CS1 spaces.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void CS0_PORT_Init (void)
{
    /* WARNING:                                                           */
    /* The section area for the RW data or the ZI data is not initialised */
    /* because this function is called by the Peripheral_BasicInit        */
    /* function. Do not use the variables allocated to the section area   */
    /* for the RW data or the ZI data within this function and the user-  */
    /* defined function called by this function.                          */

    /* ==== BSC settings ==== */

    /* ---- P9_1 : A25 ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC9,  0, GPIO_PIBC9_PIBC91_SHIFT,   GPIO_PIBC9_PIBC91);
    rza_io_reg_write_16(&GPIO.PBDC9,  0, GPIO_PBDC9_PBDC91_SHIFT,   GPIO_PBDC9_PBDC91);
    rza_io_reg_write_16(&GPIO.PM9,    1, GPIO_PM9_PM91_SHIFT,       GPIO_PM9_PM91);
    rza_io_reg_write_16(&GPIO.PMC9,   0, GPIO_PMC9_PMC91_SHIFT,     GPIO_PMC9_PMC91);
    rza_io_reg_write_16(&GPIO.PIPC9,  0, GPIO_PIPC9_PIPC91_SHIFT,   GPIO_PIPC9_PIPC91);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC9,  0, GPIO_PBDC9_PBDC91_SHIFT,   GPIO_PBDC9_PBDC91);
    rza_io_reg_write_16(&GPIO.PFC9,   0, GPIO_PFC9_PFC91_SHIFT,     GPIO_PFC9_PFC91);
    rza_io_reg_write_16(&GPIO.PFCE9,  0, GPIO_PFCE9_PFCE91_SHIFT,   GPIO_PFCE9_PFCE91);
    rza_io_reg_write_16(&GPIO.PFCAE9, 0, GPIO_PFCAE9_PFCAE91_SHIFT, GPIO_PFCAE9_PFCAE91);
    rza_io_reg_write_16(&GPIO.PIPC9,  1, GPIO_PIPC9_PIPC91_SHIFT,   GPIO_PIPC9_PIPC91);
    rza_io_reg_write_16(&GPIO.PMC9,   1, GPIO_PMC9_PMC91_SHIFT,     GPIO_PMC9_PMC91);

    /* ---- P9_0 : A24 ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC9,  0, GPIO_PIBC9_PIBC90_SHIFT,   GPIO_PIBC9_PIBC90);
    rza_io_reg_write_16(&GPIO.PBDC9,  0, GPIO_PBDC9_PBDC90_SHIFT,   GPIO_PBDC9_PBDC90);
    rza_io_reg_write_16(&GPIO.PM9,    1, GPIO_PM9_PM90_SHIFT,       GPIO_PM9_PM90);
    rza_io_reg_write_16(&GPIO.PMC9,   0, GPIO_PMC9_PMC90_SHIFT,     GPIO_PMC9_PMC90);
    rza_io_reg_write_16(&GPIO.PIPC9,  0, GPIO_PIPC9_PIPC90_SHIFT,   GPIO_PIPC9_PIPC90);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC9,  0, GPIO_PBDC9_PBDC90_SHIFT,   GPIO_PBDC9_PBDC90);
    rza_io_reg_write_16(&GPIO.PFC9,   0, GPIO_PFC9_PFC90_SHIFT,     GPIO_PFC9_PFC90);
    rza_io_reg_write_16(&GPIO.PFCE9,  0, GPIO_PFCE9_PFCE90_SHIFT,   GPIO_PFCE9_PFCE90);
    rza_io_reg_write_16(&GPIO.PFCAE9, 0, GPIO_PFCAE9_PFCAE90_SHIFT, GPIO_PFCAE9_PFCAE90);
    rza_io_reg_write_16(&GPIO.PIPC9,  1, GPIO_PIPC9_PIPC90_SHIFT,   GPIO_PIPC9_PIPC90);
    rza_io_reg_write_16(&GPIO.PMC9,   1, GPIO_PMC9_PMC90_SHIFT,     GPIO_PMC9_PMC90);

    /* ---- P8_15 : A23 ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC8,  0, GPIO_PIBC8_PIBC815_SHIFT,   GPIO_PIBC8_PIBC815);
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC815_SHIFT,   GPIO_PBDC8_PBDC815);
    rza_io_reg_write_16(&GPIO.PM8,    1, GPIO_PM8_PM815_SHIFT,       GPIO_PM8_PM815);
    rza_io_reg_write_16(&GPIO.PMC8,   0, GPIO_PMC8_PMC815_SHIFT,     GPIO_PMC8_PMC815);
    rza_io_reg_write_16(&GPIO.PIPC8,  0, GPIO_PIPC8_PIPC815_SHIFT,   GPIO_PIPC8_PIPC815);
    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC815_SHIFT,   GPIO_PBDC8_PBDC815);
    rza_io_reg_write_16(&GPIO.PFC8,   0, GPIO_PFC8_PFC815_SHIFT,     GPIO_PFC8_PFC815);
    rza_io_reg_write_16(&GPIO.PFCE8,  0, GPIO_PFCE8_PFCE815_SHIFT,   GPIO_PFCE8_PFCE815);
    rza_io_reg_write_16(&GPIO.PFCAE8, 0, GPIO_PFCAE8_PFCAE815_SHIFT, GPIO_PFCAE8_PFCAE815);
    rza_io_reg_write_16(&GPIO.PIPC8,  1, GPIO_PIPC8_PIPC815_SHIFT,   GPIO_PIPC8_PIPC815);
    rza_io_reg_write_16(&GPIO.PMC8,   1, GPIO_PMC8_PMC815_SHIFT,     GPIO_PMC8_PMC815);

    /* ---- P8_14 : A22 ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC8,  0, GPIO_PIBC8_PIBC814_SHIFT,   GPIO_PIBC8_PIBC814);
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC814_SHIFT,   GPIO_PBDC8_PBDC814);
    rza_io_reg_write_16(&GPIO.PM8,    1, GPIO_PM8_PM814_SHIFT,       GPIO_PM8_PM814);
    rza_io_reg_write_16(&GPIO.PMC8,   0, GPIO_PMC8_PMC814_SHIFT,     GPIO_PMC8_PMC814);
    rza_io_reg_write_16(&GPIO.PIPC8,  0, GPIO_PIPC8_PIPC814_SHIFT,   GPIO_PIPC8_PIPC814);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC814_SHIFT,   GPIO_PBDC8_PBDC814);
    rza_io_reg_write_16(&GPIO.PFC8,   0, GPIO_PFC8_PFC814_SHIFT,     GPIO_PFC8_PFC814);
    rza_io_reg_write_16(&GPIO.PFCE8,  0, GPIO_PFCE8_PFCE814_SHIFT,   GPIO_PFCE8_PFCE814);
    rza_io_reg_write_16(&GPIO.PFCAE8, 0, GPIO_PFCAE8_PFCAE814_SHIFT, GPIO_PFCAE8_PFCAE814);
    rza_io_reg_write_16(&GPIO.PIPC8,  1, GPIO_PIPC8_PIPC814_SHIFT,   GPIO_PIPC8_PIPC814);
    rza_io_reg_write_16(&GPIO.PMC8,   1, GPIO_PMC8_PMC814_SHIFT,     GPIO_PMC8_PMC814);

    /* ---- P8_13 : A21 ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC8,  0, GPIO_PIBC8_PIBC813_SHIFT,   GPIO_PIBC8_PIBC813);
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC813_SHIFT,   GPIO_PBDC8_PBDC813);
    rza_io_reg_write_16(&GPIO.PM8,    1, GPIO_PM8_PM813_SHIFT,       GPIO_PM8_PM813);
    rza_io_reg_write_16(&GPIO.PMC8,   0, GPIO_PMC8_PMC813_SHIFT,     GPIO_PMC8_PMC813);
    rza_io_reg_write_16(&GPIO.PIPC8,  0, GPIO_PIPC8_PIPC813_SHIFT,   GPIO_PIPC8_PIPC813);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC813_SHIFT,   GPIO_PBDC8_PBDC813);
    rza_io_reg_write_16(&GPIO.PFC8,   0, GPIO_PFC8_PFC813_SHIFT,     GPIO_PFC8_PFC813);
    rza_io_reg_write_16(&GPIO.PFCE8,  0, GPIO_PFCE8_PFCE813_SHIFT,   GPIO_PFCE8_PFCE813);
    rza_io_reg_write_16(&GPIO.PFCAE8, 0, GPIO_PFCAE8_PFCAE813_SHIFT, GPIO_PFCAE8_PFCAE813);
    rza_io_reg_write_16(&GPIO.PIPC8,  1, GPIO_PIPC8_PIPC813_SHIFT,   GPIO_PIPC8_PIPC813);
    rza_io_reg_write_16(&GPIO.PMC8,   1, GPIO_PMC8_PMC813_SHIFT,     GPIO_PMC8_PMC813);

    /* ---- P7_6 : WE0# / DQMLL ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC7,  0, GPIO_PIBC7_PIBC76_SHIFT,   GPIO_PIBC7_PIBC76);
    rza_io_reg_write_16(&GPIO.PBDC7,  0, GPIO_PBDC7_PBDC76_SHIFT,   GPIO_PBDC7_PBDC76);
    rza_io_reg_write_16(&GPIO.PM7,    1, GPIO_PM7_PM76_SHIFT,       GPIO_PM7_PM76);
    rza_io_reg_write_16(&GPIO.PMC7,   0, GPIO_PMC7_PMC76_SHIFT,     GPIO_PMC7_PMC76);
    rza_io_reg_write_16(&GPIO.PIPC7,  0, GPIO_PIPC7_PIPC76_SHIFT,   GPIO_PIPC7_PIPC76);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 1st multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC7,  0, GPIO_PBDC7_PBDC76_SHIFT,   GPIO_PBDC7_PBDC76);
    rza_io_reg_write_16(&GPIO.PFC7,   0, GPIO_PFC7_PFC76_SHIFT,     GPIO_PFC7_PFC76);
    rza_io_reg_write_16(&GPIO.PFCE7,  0, GPIO_PFCE7_PFCE76_SHIFT,   GPIO_PFCE7_PFCE76);
    rza_io_reg_write_16(&GPIO.PFCAE7, 0, GPIO_PFCAE7_PFCAE76_SHIFT, GPIO_PFCAE7_PFCAE76);
    rza_io_reg_write_16(&GPIO.PIPC7,  1, GPIO_PIPC7_PIPC76_SHIFT,   GPIO_PIPC7_PIPC76);
    rza_io_reg_write_16(&GPIO.PMC7,   1, GPIO_PMC7_PMC76_SHIFT,     GPIO_PMC7_PMC76);

    /* ---- P3_7 : CS1# ---- */
    /* Port initialise */
    rza_io_reg_write_16(&GPIO.PIBC3,  0, GPIO_PIBC3_PIBC37_SHIFT,   GPIO_PIBC3_PIBC37);
    rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC37_SHIFT,   GPIO_PBDC3_PBDC37);
    rza_io_reg_write_16(&GPIO.PM3,    1, GPIO_PM3_PM37_SHIFT,       GPIO_PM3_PM37);
    rza_io_reg_write_16(&GPIO.PMC3,   0, GPIO_PMC3_PMC37_SHIFT,     GPIO_PMC3_PMC37);
    rza_io_reg_write_16(&GPIO.PIPC3,  0, GPIO_PIPC3_PIPC37_SHIFT,   GPIO_PIPC3_PIPC37);

    /* Port mode : Multiplex mode                     */
    /* Port function setting : 7th multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    rza_io_reg_write_16(&GPIO.PBDC3,  0, GPIO_PBDC3_PBDC37_SHIFT,   GPIO_PBDC3_PBDC37);
    rza_io_reg_write_16(&GPIO.PFC3,   0, GPIO_PFC3_PFC37_SHIFT,     GPIO_PFC3_PFC37);
    rza_io_reg_write_16(&GPIO.PFCE3,  1, GPIO_PFCE3_PFCE37_SHIFT,   GPIO_PFCE3_PFCE37);
    rza_io_reg_write_16(&GPIO.PFCAE3, 1, GPIO_PFCAE3_PFCAE37_SHIFT, GPIO_PFCAE3_PFCAE37);
    rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC37_SHIFT,   GPIO_PIPC3_PIPC37);
    rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC37_SHIFT,     GPIO_PMC3_PMC37);
}
